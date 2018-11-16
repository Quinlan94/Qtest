#include "mainwindow.h"
#include <QApplication>
#include <myglwidget.h>
#include <mywindow.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MyGLWidget w;
//      w.resize(400, 300);
//      w.show();
    // mywindow d;


     MainWindow w;
     w.show();

     w.resize(800,600);



    // d.show();
     //d.PaintGL();

    return a.exec();
}
