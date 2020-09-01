#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "face_detection.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_camera =new Camera_Thread();
    m_camera->Open_Camera(1);
    m_camera->start();
    m_detection =new DetectFace_Thread();
    m_detection->init("/home/pengyang/Code/qt_example/FaceRecognition/model/retinaface.trt");
    m_detection->start();

    connect(m_detection,SIGNAL(emit_frame(QImage)),
            this, SLOT(dispaly_frame(QImage)),
            Qt::QueuedConnection );

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::dispaly_frame(QImage qimage)
{
      ui->label_view_camera->setPixmap(QPixmap::fromImage(qimage.scaled(ui->label_view_camera->size())));
}
