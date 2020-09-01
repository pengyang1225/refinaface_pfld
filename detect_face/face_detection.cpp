#include "face_detection.h"

Face_Detection::Face_Detection(std::string modelpath)
{
    net_file= modelpath+"/faceboxes_deploy.prototxt";
    caffe_model=modelpath+"/faceboxes.caffemodel";
    Caffe::set_mode(Caffe::GPU);
    net_.reset(new Net<float>(net_file, TEST));
    net_->CopyTrainedLayersFrom(caffe_model);

}
long time_in_ms(){
      struct timeval t;
      gettimeofday(&t, NULL);
      long time_ms = ((long)t.tv_sec)*1000+(long)t.tv_usec/1000;
      return time_ms;
}
int Face_Detection::facedetect(cv::Mat image,float *image_boxes)
{
     Caffe::set_mode(Caffe::GPU);
    Blob<float>* input_layer = net_->input_blobs()[0];
    input_layer->Reshape(1, 3,1024,1024);
    /* Forward dimension change to all layers. */
    net_->Reshape();
    cv::Mat img=image.clone();
    cv::Mat resized;
    if(img.empty())
    {
        return 2;
    }
    resize(img, resized, cv::Size(1024, 1024), 0, 0, cv::INTER_LINEAR);
    float * input_data = input_layer->mutable_cpu_data();
    cv::Vec3b * img_data = (cv::Vec3b *)resized.data;
    int spatial_size = 1024* 1024;
    for (int k = 0; k < spatial_size; ++k) {
        input_data[k] = float((img_data[k][0] - 104));
        input_data[k + spatial_size] = float((img_data[k][1] - 117) );
        input_data[k + 2 * spatial_size] = float((img_data[k][2] - 123));
    }
    long start=time_in_ms();
    net_->Forward();
    std::cout << "it took " << time_in_ms() - start << " ms forword" << std::endl;

    Blob<float>* result = net_->blob_by_name("detection_out").get();
    const float* reg_data = result->cpu_data();
    int w=result->width();
    int h=result->height();
    int nu=result->num();
    int c=result->channels();
    int sp=w*h;
    float temp_width=0;
//    if(h<1)
//    {
//       return 1;//wei jiance daorenlian
//    }
    bool flag=false;
    for(int j=0;j<h;j++)
    {
        float xmin=result->data_at(0,0,j,3);
        float ymin=result->data_at(0,0,j,4);
        float xmax=result->data_at(0,0,j,5);
        float ymax=result->data_at(0,0,j,6);

        float cls=result->data_at(0,0,j,1);
        float conf=result->data_at(0,0,j,2);
        int a=0;
        std::cout <<"conf=" <<conf<<std::endl;
        if(conf>0.8)
        {
            flag=true;
            if(xmin<0)
            {
                xmin=0;
            }
            if(ymin<0)
            {
                ymin=0;
            }
            float width=xmax-xmin;
            if(width>temp_width)
            {
                image_boxes[0]=xmin;
                image_boxes[1]=ymin;
                image_boxes[2]=(xmax-xmin);
                image_boxes[3]=(ymax-ymin);
                temp_width=width;
            }
        }
    }

    if(flag==false)
    {
        return 1;
    }
    image_boxes[0]=image_boxes[0]*((float)img.cols);
    image_boxes[1]=image_boxes[1]*((float)img.rows);
    image_boxes[2]=image_boxes[2]*((float)img.cols);
    image_boxes[3]=image_boxes[3]*((float)img.rows);

    if((image_boxes[0]+image_boxes[2])>img.cols)
    {
        image_boxes[2]=img.cols-image_boxes[0];
    }
    if((image_boxes[1]+image_boxes[3])>img.rows)
    {
        image_boxes[3]=img.rows-image_boxes[1];
    }

    image_boxes[0]=image_boxes[0]-(image_boxes[2]*0.1);
    if(image_boxes[0]<0)
    {
        image_boxes[0]=0;
    }
    image_boxes[1]=image_boxes[1]-(image_boxes[3]*0.1);
    if(image_boxes[1]<0)
    {
        image_boxes[1]=0;
    }
    image_boxes[2]=image_boxes[2]*1.1;
    image_boxes[3]=image_boxes[3]*1.1;
    if((image_boxes[0]+image_boxes[2])>img.cols)
    {
        image_boxes[2]=img.cols-image_boxes[0];
    }
    if((image_boxes[1]+image_boxes[3])>img.rows)
    {
        image_boxes[3]=img.rows-image_boxes[1];
    }
    return 0;
}
