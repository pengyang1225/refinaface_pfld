#-------------------------------------------------
#
# Project created by QtCreator 2020-06-17T16:37:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FaceRecognition
TEMPLATE = app
CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        pfld/pfld.cpp \
        libmessageQueue/src/messageQueue.cpp \
        libmessageQueue/src/Msg.cpp \
        src/camera_thread.cpp \
        main.cpp \
        mainwindow.cpp \
        msg_handle.cpp \
        detectface_thread.cpp \
    retinaface_trt/image.cc \
    retinaface_trt/retinaface_trt.cpp \
    retinaface_trt/tensorrt_engine.cc

HEADERS += \
    inc/camera_thread.h \
    libmessageQueue/inc/messageQueue.h \
    libmessageQueue/inc/Msg.h \
    mainwindow.h \
    msg_handle.h \
    detectface_thread.h \
    pfld/pfld.h \
    pfld/pfld_export.h  \
    retinaface_trt/tensorrt_engine.h \
    retinaface_trt/tensorrt_common.h \
    retinaface_trt/tensorrt_utils.h \
    retinaface_trt/image.h \
    retinaface_trt/retinaface_trt.h

FORMS += \
        mainwindow.ui


#inclde
INCLUDEPATH +=$$PWD/detect_face/    \
              $$PWD/pfld/    \
              $$PWD/inc/    \
              $$PWD/libmessageQueue/inc/    \
              $$PWD/retinaface_trt/   \

DEPENDPATH += $$PWD/detect_face/   \
              $$PWD/pfld/    \
              $$PWD/inc/   \
              $$PWD/libmessageQueue/inc/ \
               $$PWD/retinaface_trt/   \
##opencv
INCLUDEPATH += /usr/local/include\
               /usr/local/include/opencv\
               /usr/local/include/opencv2
LIBS +=/usr/lib/x86_64-linux-gnu/libopencv_*.so






INCLUDEPATH += /home/pengyang/TensorRT-7.0.0.11/include
LIBS += -L/home/pengyang/TensorRT-7.0.0.11/lib -lnvinfer -lnvcaffe_parser -lmyelin -lnvinfer_plugin  -lnvonnxparser -lnvparsers \

#LIBS += -L/home/pengyang/thor/build/ -lthor

unix {
    CUDA_DIR = /usr/local/cuda
    SYSTEM_TYPE = 64            #操作系统位数 '32' or '64',
    CUDA_ARCH = sm_61         # cuda架构, for example 'compute_10', 'compute_11', 'sm_10'
    NVCC_OPTIONS = -lineinfo -Xcompiler -fPIC

    INCLUDEPATH += $$CUDA_DIR/include
    LIBS += -L$$CUDA_DIR/lib64
    LIBS += -lcuda -lcudart -lcublas -lcudnn -lcurand
    # npp
    LIBS += -lnppig -lnppicc -lnppc -lnppidei -lnppist

    CUDA_OBJECTS_DIR = ./

    # The following makes sure all path names (which often include spaces) are put between quotation marks
    CUDA_INC = $$join(INCLUDEPATH,'" -I"','-I"','"')

    CONFIG(debug, debug|release): {
        cuda_d.input = CUDA_SOURCES
        cuda_d.output = $$CUDA_OBJECTS_DIR/${QMAKE_FILE_BASE}_cuda.o
        cuda_d.commands = $$CUDA_DIR/bin/nvcc -D_DEBUG $$NVCC_OPTIONS $$CUDA_INC --machine $$SYSTEM_TYPE \
            -arch=$$CUDA_ARCH -c -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_NAME}
        cuda_d.dependency_type = TYPE_C
        QMAKE_EXTRA_COMPILERS += cuda_d
    } else:CONFIG(release, debug|release): {
        cuda.input = CUDA_SOURCES
        cuda.output = $$CUDA_OBJECTS_DIR/${QMAKE_FILE_BASE}_cuda.o
        cuda.commands = $$CUDA_DIR/bin/nvcc $$NVCC_OPTIONS $$CUDA_INC --machine $$SYSTEM_TYPE \
            -arch=$$CUDA_ARCH -c -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_NAME}
        cuda.dependency_type = TYPE_C
        QMAKE_EXTRA_COMPILERS += cuda
    }
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
INCLUDEPATH += /home/pengyang/miniconda3/libtorch/include \
              /home/pengyang/miniconda3/libtorch/include/torch/csrc/api/include \

DEPENDPATH += /home/pengyang/miniconda3/libtorch/include \
              /home/pengyang/miniconda3/libtorch/include/torch/csrc/api/include \


LIBS += -L/home/pengyang/miniconda3/libtorch/lib -lc10 -lc10_cuda \
-lcaffe2_detectron_ops_gpu \
-lcaffe2_module_test_dynamic \
-lcaffe2_nvrtc -lcaffe2_observers \
-lonnx -lonnx_proto \
-ltorch_cuda -ltorch_cpu -ltorch
QMAKE_CXXFLAGS += -D_GLIBCXX_USE_CXX11_ABI=0



