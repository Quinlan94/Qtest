#-------------------------------------------------
#
# Project created by QtCreator 2017-11-23T16:14:35
#
#-------------------------------------------------

QT       += core gui


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Qtest
TEMPLATE = app
DEFINES += GLOG_NO_ABBREVIATED_SEVERITIES


SOURCES += main.cpp\
        mainwindow.cpp \
    mywindow.cpp \
    linepainter.cpp \
    pointpainter.cpp \
    trianglepainter.cpp

HEADERS  += mainwindow.h \
    mywindow.h \
    linepainter.h \
    pointpainter.h \
    trianglepainter.h
QT       += opengl
#LIBS+=C:\Users\zou\Desktop\openProject\Qtest\glut32.lib
LIBS+=C:\Users\zou\Desktop\openProject\Qtest\freeglutd.lib
#LIBS+=D:\local\boost_1_60_0\lib64-msvc-14.0\libboost_filesystem-vc140-mt-gd-1_60.lib
#LIBS+=D:\local\boost_1_60_0\lib64-msvc-14.0\libboost_system-vc140-mt-gd-1_60.lib

RESOURCES += \
    shader.qrc

