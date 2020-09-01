#ifndef FACE_DETECTION_H
#define FACE_DETECTION_H
#include <iostream>
#include <iostream>
#include <caffe/caffe.hpp>
#include <opencv2/opencv.hpp>
using namespace caffe;
//using namespace cv;


class Face_Detection
{
public:
    Face_Detection(std::string modelpath);
    int facedetect(cv::Mat image, float *image_boxes);


private:
    std::string net_file;//= "faceboxes_deploy.prototxt";
    std::string caffe_model;//="faceboxes_iter_120000.caffemodel";
    boost::shared_ptr<Net<float> > net_;
};

#endif // FACE_DETECTION_H
