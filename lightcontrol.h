#ifndef LIGHTCONTROL_H
#define LIGHTCONTROL_H

#include <QWidget>

#include <QtCore>
#include <QtGui>

#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QSlider>


class mywindow;
class LightControl : public QWidget
{
    Q_OBJECT
public:
    explicit LightControl(QWidget* parent, mywindow* opengl_window);

signals:

public slots:
private:
    mywindow* opengl_window_;

    QGridLayout *layout_main_grid;
    QGridLayout *layout_light_position;
    QGridLayout *layout_light_color;
    QGridLayout *layout_light_ambient;
    QGridLayout *layout_light_diffuse;

    QLabel *label_light_position;
    QLabel *label_light_color;
    QLabel *label_light_ambient;
    QLabel *label_light_diffuse;

    //QDoubleSpinBox p;


    QSpinBox *spinbox_position_x;
    QSpinBox *spinbox_position_y;
    QSpinBox *spinbox_position_z;

    QSpinBox *spinbox_color_r;
    QSpinBox *spinbox_color_g;
    QSpinBox *spinbox_color_b;

    QSpinBox *spinbox_ambient_r;
    QSpinBox *spinbox_ambient_g;
    QSpinBox *spinbox_ambient_b;

    QSpinBox *spinbox_diffuse_r;
    QSpinBox *spinbox_diffuse_g;
    QSpinBox *spinbox_diffuse_b;


    QSlider *slider_position_x;
    QSlider *slider_position_y;
    QSlider *slider_position_z;

    QSlider *slider_color_r;
    QSlider *slider_color_g;
    QSlider *slider_color_b;

    QSlider *slider_ambient_r;
    QSlider *slider_ambient_g;
    QSlider *slider_ambient_b;

    QSlider *slider_diffuse_r;
    QSlider *slider_diffuse_g;
    QSlider *slider_diffuse_b;

};

#endif // LIGHTCONTROL_H
