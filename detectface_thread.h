#ifndef DETECTFACE_THREAD_H
#define DETECTFACE_THREAD_H

//#include <QObject>
#include<QThread>
#include<QImage>
//#include <opencv2/opencv.hpp>
#include "retinaface_trt.h"
#include "pfld_export.h"


class DetectFace_Thread : public QThread
{
     Q_OBJECT
public:
    DetectFace_Thread();
    ~DetectFace_Thread();
void init(std::string);
void DetectFace(std::string);

private:
//DetectFace_Thread *m_face_detect;
 Retinaface_TRT *TRT_FACE;
 MAX_HANDLE PFLD_handle;


signals:
 void emit_frame(QImage);

protected:
     void run();
};

#endif // DETECTFACE_THREAD_H


