#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "camera_thread.h"
#include "detectface_thread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Camera_Thread *m_camera;
    DetectFace_Thread *m_detection;
    Ui::MainWindow *ui;
private slots:
    void dispaly_frame(QImage);
};

#endif // MAINWINDOW_H
