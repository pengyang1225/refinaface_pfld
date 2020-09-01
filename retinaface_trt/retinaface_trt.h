#ifndef RETINAFACE_TRT_H
#define RETINAFACE_TRT_H
#include <assert.h>
#include <cuda_runtime_api.h>
#include <sys/stat.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "NvInfer.h"
#include "NvOnnxParser.h"

#include <opencv2/opencv.hpp>
#include "image.h"
#include <sys/time.h>
#include <iterator>

//#include "thor/timer.h"
//#include "thor/logging.h"
//#include "thor/os.h"
//#include "thor/structures.h"

// #include "../include/batch_stream.h"

// #include "../include/entropy_calibrator.h"

// we will using eigen so something
#include "eigen3/Eigen/Eigen"
#include "eigen3/Eigen/Core"

#include <string>
#include <vector>
using namespace std;
/**
 *
 * Inference on a new onnx converted trt model
 * using standalone TensorRT engine
 *
 */

//using namespace thor::log;
using namespace nvinfer1;
using namespace Eigen;



struct Box {
  float x1;
  float y1;
  float x2;
  float y2;
};

struct Landmark {
  float x[5];
  float y[5];
};

struct FaceInfo {
  float score;
  Box box;
  Landmark landmark;
};


class Retinaface_TRT
{

private:
    std::vector<FaceInfo> nms(std::vector<FaceInfo> &bboxes,
                              float threshold);
    vector<Box> createPriors(vector<vector<int>> min_sizes, vector<int> steps, cv::Size img_size);
    vector<FaceInfo> doPostProcess(float *out_box, float *out_landmark, float *out_conf,
        const vector<Box> &priors, float nms_threshold);

    vector<Box> priors;
    IExecutionContext *context ;
    ICudaEngine *engine;
public:
    Retinaface_TRT(string engine_f);
    float * TransformImage(cv::Mat src);
    vector<FaceInfo> DoInference(float *input, int batchSize,
                                 float nms_threshold);
    void Demo_show(const cv::Mat &src,const vector<FaceInfo>&result);
    void free_engine();

};

#endif // RETINAFACE_TRT_H
