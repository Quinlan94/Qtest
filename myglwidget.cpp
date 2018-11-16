#include "myglwidget.h"
#include <GL/glut.h>
#include <QKeyEvent>

MyGLWidget::MyGLWidget(QWidget *parent) :
    QGLWidget(parent)
{
    fullscreen = false;
}

MyGLWidget::~MyGLWidget()
{

}

void MyGLWidget::initializeGL()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);                   //黑色背景
        glShadeModel(GL_SMOOTH);                            //启用阴影平滑

        glClearDepth(1.0);                                  //设置深度缓存
        glEnable(GL_DEPTH_TEST);                            //启用深度测试
        glDepthFunc(GL_LEQUAL);                             //所作深度测试的类型
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  //告诉系统对透视进行修正
}

void MyGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, (GLint)w, (GLint)h);               //重置当前的视口
        glMatrixMode(GL_PROJECTION);                        //选择投影矩阵
        glLoadIdentity();                                   //重置投影矩阵
        //设置视口的大小
        //gluPerspective(45.0, (GLfloat)w/(GLfloat)h, 0.1, 100.0);
        glMatrixMode(GL_MODELVIEW);                         //选择模型观察矩阵
        glLoadIdentity();                                   //重置模型观察矩阵
}

void MyGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清除屏幕和深度缓存
    glLoadIdentity();
}

void MyGLWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
        {
        //F1为全屏和普通屏的切换键
        case Qt::Key_F1:
            fullscreen = !fullscreen;
            if (fullscreen)
            {
                showFullScreen();
            }
            else
            {
                showNormal();
            }
            updateGL();
            break;
        //ESC为退出键
        case Qt::Key_Escape:
            close();
        }
}
