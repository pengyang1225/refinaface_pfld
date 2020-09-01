#include "detectface_thread.h"
#include"msg_handle.h"


DetectFace_Thread::DetectFace_Thread()
{

}
DetectFace_Thread::~DetectFace_Thread()
{


}

void DetectFace_Thread::init(std::string model_path)
{
   // m_face_detect = new Face_Detection(model_path);
    TRT_FACE =new Retinaface_TRT(model_path);

    //yolov5_init(&handle,caffemodel_dir,true);
    pfld_init(&PFLD_handle,"/home/pengyang/Code/qt_example/FaceRecognition/model/pfld_torchscript.pt",true);



}
void DetectFace_Thread::run()
{


    MAX_Message::Error_Code msg_recv_ret = MAX_Message::OK;

    MSG_INFO msg_info;
    while (1) {


        try
        {
            auto rev = msgCamearHandle.get(MAX_Message::WAIT_FOREVER);
            auto& dm = dynamic_cast<MAX_Message::DataMsg<MSG_INFO>&>(*rev);
            msg_info = dm.getPayload();
            msg_recv_ret =  MAX_Message::OK;

            // std::cout<<"rev image:"<<test_msg_info.cameraId<<std::endl;
        } catch (...) {
            msg_recv_ret = MAX_Message::ERROR;
        }

        if (MAX_Message::OK == msg_recv_ret)
        {
            cv::Mat image =msg_info.frame.clone();

            std::cout <<msg_info.frame.cols;
            float image_boxes[4];
            float *data=TRT_FACE->TransformImage(image);

            vector<FaceInfo> RESULT=TRT_FACE->DoInference(data,1,0.85);

            free(data);


            int ret=0;
            if(ret ==0)
            {

                int ori_w =image.cols;
                int ori_h =image.rows;
                for (size_t i = 0; i < RESULT.size(); i++) {
                    FaceInfo one_face = RESULT[i];
                    int x1 = (int) (one_face.box.x1 * ori_w);
                    int y1 = (int) (one_face.box.y1 * ori_h);
                    int x2 = (int) (one_face.box.x2 * ori_w);
                    int y2 = (int) (one_face.box.y2 * ori_h);
                    if(x1 <0) x1=0;
                    if(y1 <0) y1=0;
                    if(x2 >ori_w) x2=ori_w;
                    if(y2 >ori_h) y2=ori_h;
                    float conf = one_face.score;
                    if (conf > 0.5) {
                        char conf_str[128];
                        sprintf(conf_str, "%.3f", conf);
                        cv::putText(image, conf_str, cv::Point(x1, y1), cv::FONT_HERSHEY_COMPLEX, 0.6,
                                    cv::Scalar(255, 0, 225));
                        cv::rectangle(image, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(255, 0, 255));
                        cv::Mat src2 = image(cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2))).clone();
                        cv::resize(src2,src2,cv::Size(112,112));
                        followImage inputimage;
                        inputimage.h = src2.rows;
                        inputimage.w = src2.cols;
                        inputimage.data = src2.data;
                        DirectionNum_point  direct_num;

                        pfld_detect(PFLD_handle,&inputimage,&direct_num,0.5);


                        for(int j =0; j <direct_num.num; j++)
                        {
                            int x= float (direct_num.point[j].x)/112 *(x2 -x1) +x1;
                            int y= float(direct_num.point[j].y)/112 *(y2-y1) +y1;
                           // std::cout << "point_x =" <<x  <<" point_y=" <<y <<std::endl;
                             sprintf(conf_str, "%.d", j);
                             if ( 88<=j && j <=95)
                             {
                                 cv::circle(image,cv::Point(x,y),1,cv::Scalar(0,0,255),2);
                             }
                             else
                             {
                                 cv::circle(image,cv::Point(x,y),1,cv::Scalar(255,0,0),2);
                             }

//                            cv::putText(image, conf_str, cv::Point(x, y), cv::FONT_HERSHEY_COMPLEX, 0.6,
//                                        cv::Scalar(255, 0, 225),1);

                        }
                        float sum =abs(direct_num.point[94].y -direct_num.point[88].y) +  abs(direct_num.point[89].y -direct_num.point[93].y) + abs(direct_num.point[90].y -direct_num.point[92].y);
                        float sum2 = abs(direct_num.point[75].x -direct_num.point[81].x);
                        std::cout <<"---------------mouse mean distance =" <<sum/(3*sum2) <<std::endl;
                        if (sum/(3*sum2) >0.5)
                        {
                            cv::putText(image, "open", cv::Point(x1+10, y1+10), cv::FONT_HERSHEY_COMPLEX, 0.6,
                                                                   cv::Scalar(255, 0, 0),1);


                        }
                        else
                        {
                            cv::putText(image, "close", cv::Point(x1 +10, y1+10), cv::FONT_HERSHEY_COMPLEX, 0.6,
                                                                   cv::Scalar(255, 0, 225),1);

                        }

                    }





                }
                QImage colorQfram = Mat2QImage(image);
                emit emit_frame(colorQfram);
            }
        }
    }
}
