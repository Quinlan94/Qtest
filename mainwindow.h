#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "mywindow.h"

#include <QMainWindow>
#include <QAction>



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void Import();
    void GrabMovies();
    void Reset();


    void StringTrim(std::string* str);
    void StringLeftTrim(std::string* str);
    void StringRightTrim(std::string* str);
    std::vector<std::string> StringSplit(const std::string& str,
                                         const std::string& delim);

private:
   mywindow *opengl;
   QMenuBar *file;
   QMenu *file_menu;

   QAction* action_project_new_;
   QAction* action_grab_movie_;
   QAction* action_reset_;
   QAction* light_control_;



};

#endif // MAINWINDOW_H
