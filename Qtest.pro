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
    trianglepainter.cpp \
    moviewidget.cpp \
    glmath.cpp \
    lightcontrol.cpp

HEADERS  += mainwindow.h \
    mywindow.h \
    linepainter.h \
    pointpainter.h \
    trianglepainter.h \
    moviewidget.h \
    glmath.h \
    lightcontrol.h
QT       += opengl

#LIBS+=C:\Users\zou\Desktop\openProject\Qtest\glut32.lib
LIBS+=./freeglutd.lib

RESOURCES += \
    shader.qrc

