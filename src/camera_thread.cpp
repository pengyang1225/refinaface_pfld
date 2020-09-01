#include "camera_thread.h"
#include<QImage>
#include<QDebug>
#include"msg_handle.h"


Camera_Thread::Camera_Thread()
{
    open_flag =false;


}

Camera_Thread::~Camera_Thread()
{

}
bool Camera_Thread::Open_Camera(int camera_index)
{
    cap= cv::VideoCapture(camera_index);
    if(!cap.isOpened())
    {
        return false;
    }

//    cap.set(3, 640);
//    cap.set(4, 480);
    open_flag =true;
    return true;
}
int Camera_Thread::read_image(cv::Mat &src)
{
    if(!cap.isOpened())
    {
        return CanntOpenCamera;
    }
    cv::Mat image;
    cap >> image;

    if(image.empty())
    {
        return ReadCameraEmpty;
    }

    image.copyTo(src);

    return ReadCameraOk;
}
void Camera_Thread::run()
{

     //msgCamearHandle.setAttribute(10);
     MSG_INFO msg_info;

    while (open_flag)
    {
        if(ReadCameraOk==read_image(frame))
        {

            msg_info.frame =frame;
            MAX_Message::Error_Code msg_ret = msgCamearHandle.put(MAX_Message::DataMsg<MSG_INFO>(1, msg_info),
                                                   MAX_Message::NO_WAIT ,
                                                   MAX_Message::MESSAGE_PRI_NORMAL);




        }
        else
        {
            qDebug()<<"read frame error";
        }



    }



}
