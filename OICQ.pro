#-------------------------------------------------
#
# Project created by QtCreator 2019-09-20T17:34:31
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OICQ
TEMPLATE = app

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
        main.cpp \
    ui/mainwindow.cpp \
    ui/registerservice.cpp \
    ui/loginservice.cpp \
    manager/managerservice.cpp \
    storage/storageservice.cpp \
    communication/client.cpp \
    communication/server.cpp \
    config/configservice.cpp \
    controller/controllerservice.cpp \
    ui/messageservice.cpp

HEADERS += \
    ui/mainwindow.h \
    ui/registerservice.h \
    ui/loginservice.h \
    manager/managerservice.h \
    storage/storageservice.h \
    communication/client.h \
    communication/server.h \
    config/configservice.h \
    controller/controllerservice.h \
    logger/logger.h \
    ui/messageservice.h

FORMS += \
        mainwindow.ui \
    ui/registerservice.ui \
    ui/loginservice.ui \
    ui/messageservice.ui

INCLUDEPATH += /usr/local/include \
               /usr/local/include/opencv4

LIBS += /usr/local/lib/libopencv_aruco.so \
/usr/local/lib/libopencv_bgsegm.so \
/usr/local/lib/libopencv_bioinspired.so \
/usr/local/lib/libopencv_calib3d.so \
/usr/local/lib/libopencv_ccalib.so \
/usr/local/lib/libopencv_core.so \
/usr/local/lib/libopencv_datasets.so \
/usr/local/lib/libopencv_dnn.so \
/usr/local/lib/libopencv_dpm.so \
/usr/local/lib/libopencv_face.so \
/usr/local/lib/libopencv_features2d.so \
/usr/local/lib/libopencv_flann.so \
/usr/local/lib/libopencv_freetype.so \
/usr/local/lib/libopencv_fuzzy.so \
/usr/local/lib/libopencv_highgui.so \
/usr/local/lib/libopencv_imgcodecs.so \
/usr/local/lib/libopencv_img_hash.so \
/usr/local/lib/libopencv_imgproc.so \

RESOURCES += \
    image.qrc
