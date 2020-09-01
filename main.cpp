#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
#if 0
/**********test pfld *****/
//#include"pfld_export.h"
//#include<opencv2/opencv.hpp>
int main2(int argc, char *argv[])
{
    char caffemodel_dir[512]   = "/home/pengyang/Code/hub_gitlab/PFLD-pytorch-master/pfld_torchscript.pt";

    cv::Mat src=cv::imread("/home/pengyang/Desktop/12334");
   // std::cout <<src.cols <<src.rows <<std::endl;
    MAX_HANDLE handle;
    //yolov5_init(&handle,caffemodel_dir,true);
    pfld_init(&handle,caffemodel_dir,true);
    followImage inputimage;
    inputimage.h = src.rows;
    inputimage.w = src.cols;
    inputimage.data = src.data;
    DirectionNum_point  direct_num;
    for(int i =0;i <10;i++)
    {
        pfld_detect(handle,&inputimage,&direct_num,0.5);
    }

    cv::resize(src,src,cv::Size(112,112));
    for(int j =0; j <direct_num.num; j++)
    {
        int x= direct_num.point[j].x;
        int y= direct_num.point[j].y;
       // cv::circle(src,cv::Point(x,y),1,cv::Scalar(0,0,255));

       // std::cout << "xmin=" <<x <<" ymin=" <<y <<" w=" <<w <<" h=" <<h<<std::endl;
      //  cv::rectangle(src,cv::Rect(x,y,w,h),cv::Scalar(0,0,255));

    }
    cv::imshow("image",src);
    pfld_free(&handle);
    free(direct_num.point);
    cv::waitKey(0);


}
#endif
