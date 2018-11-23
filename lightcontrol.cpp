#include "lightcontrol.h"

#include "mywindow.h"

LightControl::LightControl(QWidget* parent, mywindow* opengl_window)
    : QWidget(parent),opengl_window_(opengl_window)
{


    setWindowFlags(Qt::Widget | Qt::WindowStaysOnTopHint | Qt::Tool);
    setWindowTitle("灯光控制");

    layout_light_position = new QGridLayout();
    label_light_position = new QLabel(tr("light position"));

    spinbox_position_x = new QSpinBox();
    spinbox_position_x->setRange(0,255);
    spinbox_position_y = new QSpinBox();
    spinbox_position_y->setRange(0,255);
    spinbox_position_z = new QSpinBox();
    spinbox_position_z->setRange(0,255);

    slider_position_x = new QSlider(Qt::Horizontal);
    slider_position_x->setRange(0,255);
    slider_position_y = new QSlider(Qt::Horizontal);
    slider_position_y->setRange(0,255);
    slider_position_z = new QSlider(Qt::Horizontal);
    slider_position_z->setRange(0,255);

    layout_light_position->addWidget(label_light_position,0,0);
    layout_light_position->addWidget(spinbox_position_x,1,0);
    layout_light_position->addWidget(slider_position_x,1,1);
    layout_light_position->addWidget(spinbox_position_y,2,0);
    layout_light_position->addWidget(slider_position_y,2,1);
    layout_light_position->addWidget(spinbox_position_z,3,0);
    layout_light_position->addWidget(slider_position_z,3,1);




    layout_light_color = new QGridLayout();
    label_light_color = new QLabel(tr("light color"));

    spinbox_color_r = new QSpinBox();
    spinbox_color_r->setRange(0,255);
    spinbox_color_g = new QSpinBox();
    spinbox_color_g->setRange(0,255);
    spinbox_color_b = new QSpinBox();
    spinbox_color_b->setRange(0,255);

    slider_color_r = new QSlider(Qt::Horizontal);
    slider_color_r->setRange(0,255);
    slider_color_g = new QSlider(Qt::Horizontal);
    slider_color_g->setRange(0,255);
    slider_color_b = new QSlider(Qt::Horizontal);
    slider_color_b->setRange(0,255);

    layout_light_color->addWidget(label_light_color,0,0);
    layout_light_color->addWidget(spinbox_color_r,1,0);
    layout_light_color->addWidget(slider_color_r,1,1);
    layout_light_color->addWidget(spinbox_color_g,2,0);
    layout_light_color->addWidget(slider_color_g,2,1);
    layout_light_color->addWidget(spinbox_color_b,3,0);
    layout_light_color->addWidget(slider_color_b,3,1);



    layout_light_ambient = new QGridLayout();
    label_light_ambient = new QLabel(tr("light ambient"));

    spinbox_ambient_r = new QSpinBox();
    spinbox_ambient_r->setRange(0,255);
    spinbox_ambient_g = new QSpinBox();
    spinbox_ambient_g->setRange(0,255);
    spinbox_ambient_b = new QSpinBox();
    spinbox_ambient_b->setRange(0,255);

    slider_ambient_r = new QSlider(Qt::Horizontal);
    slider_ambient_r->setRange(0,255);
    slider_ambient_g = new QSlider(Qt::Horizontal);
    slider_ambient_g->setRange(0,255);
    slider_ambient_b = new QSlider(Qt::Horizontal);
    slider_ambient_b->setRange(0,255);

    layout_light_ambient->addWidget(label_light_ambient,0,0);
    layout_light_ambient->addWidget(spinbox_ambient_r,1,0);
    layout_light_ambient->addWidget(slider_ambient_r,1,1);
    layout_light_ambient->addWidget(spinbox_ambient_g,2,0);
    layout_light_ambient->addWidget(slider_ambient_g,2,1);
    layout_light_ambient->addWidget(spinbox_ambient_b,3,0);
    layout_light_ambient->addWidget(slider_ambient_b,3,1);




    layout_light_diffuse = new QGridLayout();
    label_light_diffuse = new QLabel(tr("light diffuse"));

    spinbox_diffuse_r = new QSpinBox();
    spinbox_diffuse_r->setRange(0,1);
    spinbox_diffuse_g = new QSpinBox();
    spinbox_diffuse_g->setRange(0,1);
    spinbox_diffuse_b = new QSpinBox();
    spinbox_diffuse_b->setRange(0,1);

    slider_diffuse_r = new QSlider(Qt::Horizontal);
    slider_diffuse_r->setRange(0,255);
    slider_diffuse_g = new QSlider(Qt::Horizontal);
    slider_diffuse_g->setRange(0,255);
    slider_diffuse_b = new QSlider(Qt::Horizontal);
    slider_diffuse_b->setRange(0,255);

    layout_light_diffuse->addWidget(label_light_diffuse,0,0);
    layout_light_diffuse->addWidget(spinbox_diffuse_r,1,0);
    layout_light_diffuse->addWidget(slider_diffuse_r,1,1);
    layout_light_diffuse->addWidget(spinbox_diffuse_g,2,0);
    layout_light_diffuse->addWidget(slider_diffuse_g,2,1);
    layout_light_diffuse->addWidget(spinbox_diffuse_b,3,0);
    layout_light_diffuse->addWidget(slider_diffuse_b,3,1);


    layout_main_grid = new QGridLayout(this);
    layout_main_grid->addLayout(layout_light_position,0,0);
    layout_main_grid->addLayout(layout_light_color,0,1);
    layout_main_grid->addLayout(layout_light_ambient,1,0);
    layout_main_grid->addLayout(layout_light_diffuse,1,1);

    //setLayout(layout_main_grid);

}
