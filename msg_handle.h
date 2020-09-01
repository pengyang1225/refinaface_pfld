#ifndef MSG_HANDLE_H
#define MSG_HANDLE_H

#include "Msg.h"
#include "messageQueue.h"
#include <QImage>
#include <opencv2/opencv.hpp>
#define MAX_NUM  20
typedef struct Detect_BOX
{
    int x;
    int y;
    int width;
    int height;
    float confidence;


}Detect_BOX;


typedef struct MSG_INFO
{
    cv::Mat frame;
    Detect_BOX bbox[MAX_NUM];




}MSG_INFO;




extern MAX_Message::Queue msgCamearHandle;
extern MAX_Message::Queue test_msgCamearHandle;
extern MAX_Message::Queue msgTransfromHandle;
extern  QImage Mat2QImage( cv::Mat &image );
extern cv::Mat QImageToMat(QImage image);
//extern  QImage Mat2QImage( cv::Mat& mat);
#endif // MSG_HANDLE_H

