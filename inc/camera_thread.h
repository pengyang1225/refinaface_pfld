#ifndef CAMERA_THREAD_H
#define CAMERA_THREAD_H
#include<QThread>
#include<opencv2/opencv.hpp>
#include<QImage>
//using namespace cv;

enum
{
    ReadCameraOk = 0,
    CanntOpenCamera,
    ReadCameraEmpty
};

class Camera_Thread : public QThread
{
   Q_OBJECT
public:
    Camera_Thread();
    ~Camera_Thread();
    bool Open_Camera(int camera_index);
    int read_image(cv::Mat &src);
private:
    cv::Mat frame;
    bool open_flag;
    cv::VideoCapture cap;

protected:
     void run();
signals:
     void emit_frame(QImage);

};

#endif // CAMERA_THREAD_H
