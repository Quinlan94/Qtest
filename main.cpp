#include "mainwindow.h"
#include <QApplication>

//#include <mywindow.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

     MainWindow w;
     w.show();

     w.resize(800,600);



    // d.show();
     //d.PaintGL();

    return a.exec();
}
