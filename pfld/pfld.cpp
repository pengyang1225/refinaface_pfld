#include "pfld.h"
#include<ctime>
#include <pfld_export.h>

using namespace std;
void pfld_init(MAX_HANDLE* handle, const char* model_path, const bool gpu_flag)
{
    torch::DeviceType device_type;
    if (torch::cuda::is_available() && gpu_flag) {
        device_type = torch::kCUDA;
    } else {
        device_type = torch::kCPU;
    }

    Detector_PFLD * m_detect =new Detector_PFLD(model_path,device_type);
    *handle = (MAX_HANDLE )m_detect;

}

void  pfld_detect(MAX_HANDLE handle,followImage* image, DirectionNum_point* direct_num,const float conf)
{
    Detector_PFLD *m_detect = (Detector_PFLD*)handle;


    cv::Mat srcMat =cv::Mat(image->h,image->w,CV_8UC3,image->data);
  //  std::cout << "image_size" <<srcMat.size <<std::endl;

    std::vector<cv::Point> result =m_detect->Run(srcMat,conf,0.5);
    direct_num->num =result.size();
    direct_num->point = (Direction_point*)calloc(result.size(), sizeof(Direction_point));

    for(int i =0; i <result.size();i++)
    {
        direct_num->point[i].x = result[i].x;
        direct_num->point[i].y = result[i].y;
    }
    std::cout << "result.size()=" <<result.size() <<std::endl;

}

void  pfld_free(MAX_HANDLE *handle)
{
    if(NULL == handle || NULL == (*handle))
       {
           return ;
       }


       delete *handle;
       return ;
}


Detector_PFLD::Detector_PFLD(const std::string& model_path, const torch::DeviceType& device_type) : device_(device_type) {
    try {
        // Deserialize the ScriptModule from a file using torch::jit::load().
        module_ = torch::jit::load(model_path);
    }
    catch (const c10::Error& e) {
        std::cerr << "Error loading the model!\n";
        std::exit(EXIT_FAILURE);
    }

    module_.to(device_);
    module_.eval();
}


std::vector<cv::Point>
Detector_PFLD::Run(const cv::Mat& img, float conf_threshold, float iou_threshold) {
    torch::NoGradGuard no_grad;
    clock_t start,end2;

    // keep the original image for visualization purpose
    start=clock();

    cv::Mat img_input = img.clone();

//    std::vector<float> pad_info = LetterboxImage(img_input, img_input, cv::Size(112, 112));
//         start  =clock();
//    const float pad_w = pad_info[0];
//    const float pad_h = pad_info[1];
//    const float scale = pad_info[2];
    cv::resize(img_input,img_input,cv::Size(112,112));

    cv::cvtColor(img_input, img_input, cv::COLOR_BGR2RGB);  // BGR -> RGB
    img_input.convertTo(img_input, CV_32FC3, 1.0f / 255.0f);  // normalization 1/255
    auto tensor_img = torch::from_blob(img_input.data, {1, img_input.rows, img_input.cols, img_input.channels()}).to(device_);
    tensor_img = tensor_img.permute({0, 3, 1, 2}).contiguous();  // BHWC -> BCHW (Batch, Channel, Height, Width)
  //  cout<<"ctensor_img:"<<tensor_img<<endl;

    end2 =clock();
    double endtime=(double)(end2-start)/CLOCKS_PER_SEC;
    cout<<"conver images time:"<<endtime*1000<<"ms"<<endl;

    std::vector<torch::jit::IValue> inputs;
    inputs.emplace_back(tensor_img);
    // inference
    start  =clock();
    module_.to(device_);
    module_.eval();
    torch::jit::IValue output = module_.forward(inputs);
    end2 =clock();
    double endtime2=(double)(end2-start)/CLOCKS_PER_SEC;
    cout<<"module_.forward(inputs) time:"<<endtime2*1000<<"ms"<<endl;	//ms为单位
    at::Tensor detections = module_.forward(inputs).toTensor();//.toTensor();
//auto detections = output.toTuple()->elements()[0].toTensor();

    // result: n * 7
    // batch index(0), top-left x/y (1,2), bottom-right x/y (3,4), score(5), class id(6)
      start  =clock();
    auto result = PostProcessing(detections, conf_threshold, iou_threshold);
    end2 =clock();
    double endtime3=(double)(end2-start)/CLOCKS_PER_SEC;
    cout<<"PostProcessing time:"<<endtime3*1000<<"ms"<<endl;	//ms为单位

    return result;
}


std::vector<float> Detector_PFLD::LetterboxImage(const cv::Mat& src, cv::Mat& dst, const cv::Size& out_size) {
    auto in_h = static_cast<float>(src.rows);
    auto in_w = static_cast<float>(src.cols);
    float out_h = out_size.height;
    float out_w = out_size.width;

    float scale = std::min(out_w / in_w, out_h / in_h);

    int mid_h = static_cast<int>(in_h * scale);
    int mid_w = static_cast<int>(in_w * scale);

    cv::resize(src, dst, cv::Size(mid_w, mid_h));

    int top = (static_cast<int>(out_h) - mid_h) / 2;
    int down = (static_cast<int>(out_h)- mid_h + 1) / 2;
    int left = (static_cast<int>(out_w)- mid_w) / 2;
    int right = (static_cast<int>(out_w)- mid_w + 1) / 2;

    cv::copyMakeBorder(dst, dst, top, down, left, right, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));

    std::vector<float> pad_info{static_cast<float>(left), static_cast<float>(top), scale};
    return pad_info;
}


// returns the IoU of bounding boxes
torch::Tensor Detector_PFLD::GetBoundingBoxIoU(const torch::Tensor& box1, const torch::Tensor& box2) {
    // get the coordinates of bounding boxes
    const torch::Tensor& b1_x1 = box1.select(1, 0);
    const torch::Tensor& b1_y1 = box1.select(1, 1);
    const torch::Tensor& b1_x2 = box1.select(1, 2);
    const torch::Tensor& b1_y2 = box1.select(1, 3);

    const torch::Tensor& b2_x1 = box2.select(1, 0);
    const torch::Tensor& b2_y1 = box2.select(1, 1);
    const torch::Tensor& b2_x2 = box2.select(1, 2);
    const torch::Tensor& b2_y2 = box2.select(1, 3);

    // get the coordinates of the intersection rectangle
    torch::Tensor inter_rect_x1 =  torch::max(b1_x1, b2_x1);
    torch::Tensor inter_rect_y1 =  torch::max(b1_y1, b2_y1);
    torch::Tensor inter_rect_x2 =  torch::min(b1_x2, b2_x2);
    torch::Tensor inter_rect_y2 =  torch::min(b1_y2, b2_y2);

    // calculate intersection area
    torch::Tensor inter_area = torch::max(inter_rect_x2 - inter_rect_x1 + 1,torch::zeros(inter_rect_x2.sizes()))
                               * torch::max(inter_rect_y2 - inter_rect_y1 + 1, torch::zeros(inter_rect_x2.sizes()));

    // calculate union area
    torch::Tensor b1_area = (b1_x2 - b1_x1 + 1)*(b1_y2 - b1_y1 + 1);
    torch::Tensor b2_area = (b2_x2 - b2_x1 + 1)*(b2_y2 - b2_y1 + 1);

    // calculate IoU
    torch::Tensor iou = inter_area / (b1_area + b2_area - inter_area);

    return iou;
}


std::vector<cv::Point>  Detector_PFLD::PostProcessing(const torch::Tensor& detections, float conf_thres, float iou_thres) {


    torch::Tensor re_detections=detections.contiguous().view({-1, 2});
    //torch::Tensor re_detections = detections.reshape({-1,2});
   // std::cout <<"detections.size =" <<detections<<std::endl;
    re_detections = re_detections.to(torch::kCPU);
    // 直接用指针从tensor中取数据的方式
    //float* ptr = (float*)detections.data_ptr();
    std::vector<cv::Point>vec_point;
    for (int i = 0; i < re_detections.size(0); ++i) {
        auto x = re_detections[i][0] *112;
        auto y = re_detections[i][1] *112;
       // int(*bbox[0].data<float >()
        cv::Point point(int(*x.data<float>()),int(*y.data<float>()));
      //  std::cout <<point<<std::endl;
        vec_point.push_back(point);


    }

    return vec_point;
}
