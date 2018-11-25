﻿#include "mywindow.h"
#include <QDebug>
#include <iostream>




#define GRID_RGBA 0.2, 0.2, 0.2, 1
#define X_AXIS_RGBA 0.9, 0, 0, 0.5
#define Y_AXIS_RGBA 0, 0.9, 0, 0.5
#define Z_AXIS_RGBA 0, 0, 0.9, 0.5

inline size_t RGBToIndex(const uint8_t r, const uint8_t g, const uint8_t b) {
  return static_cast<size_t>(r) + static_cast<size_t>(g) * 256 +
         static_cast<size_t>(b) * 65536;
}

// Derive color from unique index, generated by `RGBToIndex`.
inline void IndexToRGB(const size_t index, float& r, float& g, float& b) {
  r = ((index & 0x000000FF) >> 0) / 255.0f;
  g = ((index & 0x0000FF00) >> 8) / 255.0f;
  b = ((index & 0x00FF0000) >> 16) / 255.0f;
}

void FrameBufferToQImage(QImage& image) {
  if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
    uint* p = (uint*)image.bits();
    uint* end = p + image.width() * image.height();
    while (p < end) {
      uint a = *p << 24;
      *p = (*p >> 8) | a;
      p++;
    }
  } else {
    for (int y = 0; y < image.height(); y++) {
      uint* q = (uint*)image.scanLine(y);
      for (int x = 0; x < image.width(); ++x) {
        const uint pixel = *q;
        *q = ((pixel << 16) & 0xff0000) | ((pixel >> 16) & 0xff) |
             (pixel & 0xff00ff00);
        q++;
      }
    }
  }
  image = image.mirrored();
}


mywindow::mywindow(QWidget *parent, QScreen *screen)
    : QWindow(screen),
    focus_distance_(kInitFocusDistance),
    mouse_is_pressed_(false),
    coordinate_grid_enabled_(true),
    movie_grab_enabled_(true),
    near_plane_(kInitNearPlane)
{

    bg_color_[0] = 1.0f;
    bg_color_[1] = 1.0f;
    bg_color_[2] = 1.0f;

    movie_grabber_widget_ = new MovieWidget(parent,this);
    light_control_widget = new LightControl(parent,this);

    num_texures =0;

    SetupGL();
    ResizeGL();

}

mywindow::~mywindow()
{
    texture_tri.clear();

}

void mywindow::SetupGL()
{
     setSurfaceType(OpenGLSurface);//将window的表面显示为OpenGL

      QSurfaceFormat format;
      format.setDepthBufferSize(24);
      format.setMajorVersion(3);
      format.setMinorVersion(2);
      format.setSamples(4);
      format.setProfile(QSurfaceFormat::CoreProfile);
    #ifdef DEBUG
      format.setOption(QSurfaceFormat::DebugContext);
    #endif

      setFormat(format);
      create();//创建一个window，前面已经设置为OpenGL了

      // Create an OpenGL context
      context_ = new QOpenGLContext(this);
      context_->setFormat(format);
      context_->create();


      view_up = QVector3D(0,1,0);
      view_foward = QVector3D(0,0,-1);
      view_epos = QVector3D(0,0,0);
      InitializeGL();
      InitializePainters();

      connect(this, &QWindow::widthChanged, this, &mywindow::ResizeGL);//这步骤必须要，不然视图位置不对
      connect(this, &QWindow::heightChanged, this, &mywindow::ResizeGL);

}

void mywindow::InitializeGL()
{
     context_->makeCurrent(this);
     InitializeSettings();
     InitializeView();


}

void mywindow::ResizeGL()
{

     context_->makeCurrent(this);
     glViewport(0, 0, width(), height());
     ComposeProjectionMatrix();
     UploadCoordinateGridData();
}

void mywindow::PaintGL()
{

    if (!isExposed()) {
       return;
     }

     context_->makeCurrent(this);

     glClearColor(bg_color_[0], bg_color_[1], bg_color_[2], 1.0f);
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




//应用于法线向量的变换矩阵是顶点变换矩阵的逆转置矩阵
     const QMatrix4x4 pmv_matrix = projection_matrix_ *view_matrix_* model_matrix_;
     movie_grabber_triangle_painter_.tri_model_matrix_ = model_matrix_;
      movie_grabber_triangle_painter_.tri_view_matrix_ = view_matrix_;
    Eigen::Matrix3f nor_matrix = QMatrixToEigen(model_matrix_).topLeftCorner(3,3).inverse().transpose();//.topl.inverse().transpose();

     movie_grabber_triangle_painter_.normal_matrix_ = EigenToQMatrix3(nor_matrix);


     QMatrix4x4 model_view_center_matrix = model_matrix_;

     const Eigen::Vector4f rot_center =
         QMatrixToEigen(model_matrix_).inverse() *
         Eigen::Vector4f(0, 0, -focus_distance_, 1);//限制x,y移动，z随坐标变化，可同时改变z坐标轴有变化，网格没裱花

     QVector4D vec3(rot_center(0), rot_center(1),
                    rot_center(2),rot_center(3));

     //直接0，0，0，就不行
     model_view_center_matrix.translate(rot_center(0), rot_center(1),
                                        rot_center(2));//不然的话网格和坐标会重合
     const QMatrix4x4 pmvc_matrix = projection_matrix_ * model_view_center_matrix;



     {
       coordinate_axes_painter_.Render(pmv_matrix, width(), height(), 2);
       coordinate_grid_painter_.Render(pmvc_matrix, width(), height(), 1);
     }


     point_painter_.Render(pmv_matrix, point_size_);
     point_connection_painter_.Render(pmv_matrix, width(), height(), 1);


      if (movie_grab_enabled_)
      {
           movie_grabber_triangle_painter_.Render(pmv_matrix,texture_tri,num_texures);
      }



     context_->swapBuffers(this);
}

void mywindow::initTextures()
{

    QOpenGLTexture *temp_texture;


   for(int i =0;i< texture_names.size();i++)
   {

       temp_texture = new QOpenGLTexture(QImage(texture_names.at(i)).mirrored());
       qDebug()<<"texture name :"<<texture_names[i];
       temp_texture->setMinificationFilter(QOpenGLTexture::Nearest);

       temp_texture->setMagnificationFilter(QOpenGLTexture::Linear);

       temp_texture->setWrapMode(QOpenGLTexture::Repeat);

       texture_tri.push_back(temp_texture);

   }

     qDebug()<<"texture initial finished";

}

float mywindow::PointSize() const
{
    return point_size_;
}

float mywindow::ImageSize() const
{
    return image_size_;
}



void mywindow::Upload()
{

    ComposeProjectionMatrix();
    UploadMeshData();


    PaintGL();

}

void mywindow::UploadMeshData()
{
     std::vector<TrianglePainter::Data> triangle_data_all;
     qDebug()<<"_Vertices.size()"<<_Vertices.size();
     for(int i =0;i<_Vertices.size();i++)
     {
         TrianglePainter::Data triangle;
         triangle.point1 = PointPainter::Data(_Points[_Vertices[i](0)](0)  , _Points[_Vertices[i](0)](1), _Points[_Vertices[i](0)](2),
                 _Points[_Vertices[i](0)](3),_Points[_Vertices[i](0)](4),_Points[_Vertices[i](0)](5), _Textures[i](0),_Textures[i](1));

         triangle.point2 = PointPainter::Data(_Points[_Vertices[i](1)](0)  , _Points[_Vertices[i](1)](1), _Points[_Vertices[i](1)](2),
                 _Points[_Vertices[i](1)](3),_Points[_Vertices[i](1)](4),_Points[_Vertices[i](1)](5), _Textures[i](2),_Textures[i](3));

         triangle.point3 = PointPainter::Data(_Points[_Vertices[i](2)](0)  , _Points[_Vertices[i](2)](1), _Points[_Vertices[i](2)](2),
                 _Points[_Vertices[i](2)](3),_Points[_Vertices[i](2)](4),_Points[_Vertices[i](2)](5), _Textures[i](4),_Textures[i](5));

//         if(i==_Vertices.size()-1)
//         {
//             qDebug()<<"point coordinate"<<_Points[_Vertices[i](0)](0)  , _Points[_Vertices[i](0)](1), _Points[_Vertices[i](0)](2);
//             qDebug()<<"list"<<_Vertices[i](0)<<" "<<_Vertices[i](1)
//                                  <<" "<<_Vertices[i](2)<<" "<<_Textures[i](0)<<" "<<_Textures[i](1)<<_Textures[i](2)<<" "<<_Textures[i](3) <<_Textures[i](4)<<" "<<_Textures[i](5);
//             qDebug()<<"triang ..."<<triangle.point1.x<<" "<<triangle.point1.y<<triangle.point1.z<<triangle.point1.tx<<" "<<triangle.point1.ty;
//              qDebug()<<"triang ..."<<triangle.point2.x<<" "<<triangle.point2.y<<triangle.point2.z<<triangle.point2.tx<<" "<<triangle.point2.ty;
//               qDebug()<<"triang ..."<<triangle.point3.x<<" "<<triangle.point3.y<<triangle.point3.z<<triangle.point3.tx<<" "<<triangle.point3.ty;
//         }

         triangle_data_all.push_back(triangle);


     }

     movie_grabber_triangle_painter_.Upload(triangle_data_all);


}

void mywindow::Clear()
{


    movie_grab_enabled_ = !movie_grab_enabled_;


}



void mywindow::printMatrix(QMatrix4x4 matrix)
{
    int n =0;
    using namespace std;
    float* mat = new float[16];
    matrix.copyDataTo(mat);
    for(int i =0;i<16;i++)
    {

        std::cout <<mat[i]<<" ";
        n++;
        if(n>3)
        {
            std::cout<<std::endl;

            n=0;
        }

    }
    cout<<"====================="<<endl;
}

void mywindow::SetModelMatrix(const QMatrix4x4 &matrix)
{
    model_matrix_ = matrix;
    PaintGL();
}

void mywindow::SetViewMatrix(const QMatrix4x4 &matrix)
{
    view_matrix_ = matrix;
    PaintGL();
}

QImage mywindow::GrabImage()
{
     DisableCoordinateGrid();

     const int scaled_width = static_cast<int>(devicePixelRatio() * width());
     const int scaled_height = static_cast<int>(devicePixelRatio() * height());

     QImage image(scaled_width, scaled_height, QImage::Format_ARGB32);
     glReadPixels(0, 0, scaled_width, scaled_height, GL_RGBA, GL_UNSIGNED_BYTE,
                  image.bits());

     FrameBufferToQImage(image);

     EnableCoordinateGrid();

     return image;
}

void mywindow::GrabMovies()
{
    movie_grabber_widget_->show();
}

void mywindow::LightContro()
{
    light_control_widget->show();
}



void mywindow::EnableCoordinateGrid()
{
    coordinate_grid_enabled_ = true;
    PaintGL();
}

void mywindow::DisableCoordinateGrid()
{

    coordinate_grid_enabled_ = false;
    PaintGL();
}

void mywindow::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);

    if (isExposed())
        PaintGL();
}



void mywindow::InitializePainters()
{
    coordinate_axes_painter_.Setup();
    coordinate_grid_painter_.Setup();

    point_painter_.Setup();
    point_connection_painter_.Setup();


    movie_grabber_triangle_painter_.Setup();
}

void mywindow::InitializeSettings()
{
    glEnable(GL_DEPTH_TEST);//启用深度测试 根据坐标的远近自动隐藏被遮住的图形（材料
    glEnable(GL_BLEND);//启用颜色混合。例如实现半透明效果
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//表示把渲染的图像融合到目标区域。也就是说源的每一个像素的alpha都等于自己的alpha，
                               //目标的每一个像素的alpha等于1减去该位置源像素的alpha。 因此不论叠加多少次，亮度是不变的。

    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);//这样在Shader中可以访问glPointSize;
                         //如果启用，并且顶点着色器处于活动状态，则派生的点大小将从（可能被剪切的）着色器内置的gl_PointSize中获取，并被钳位到实现相关的点大小范围。
    glEnable(GL_TEXTURE_2D);


}

void mywindow::InitializeView()
{
      point_size_ = kInitPointSize;
      image_size_ = kInitImageSize;
      focus_distance_ = kInitFocusDistance;

      model_matrix_.setToIdentity();
      normal_matrix_.setToIdentity();

      view_matrix_.lookAt(view_epos,view_foward,view_up);
      qDebug()<<" view matrix: "<<view_matrix_;
       //view矩阵怎么设置的
      //如果没有调用gluLookAt，照相机就设定一个默认的位置和方向，在默认情况下，照相机位于原点，指向Z轴的负方向，朝上向量为（0，1，0）,为单位矩阵
//      model_matrix_.translate(0, 0, -focus_distance_);

//      model_matrix_.rotate(225, 1, 0, 0);
//      model_matrix_.rotate(-45, 0, 1, 0);



}

void mywindow::mousePressEvent(QMouseEvent *event)
{
    if (mouse_press_timer_.isActive()) {  // Select objects (2. click)
        mouse_is_pressed_ = false;
        mouse_press_timer_.stop();
        selection_buffer_.clear();
        //SelectObject(event->pos().x(), event->pos().y());
      } else {  // Set timer to remember 1. click
        mouse_press_timer_.setSingleShot(true);
        mouse_press_timer_.start(kDoubleClickInterval);
        mouse_is_pressed_ = true;
        prev_mouse_pos_ = event->pos();
      }
    event->accept();
}

void mywindow::mouseReleaseEvent(QMouseEvent *event)
{
    mouse_is_pressed_ = false;
    event->accept();
}

void mywindow::mouseMoveEvent(QMouseEvent *event)
{
    if (mouse_is_pressed_) {
       if (event->buttons() & Qt::RightButton ||
           (event->buttons() & Qt::LeftButton &&
            event->modifiers() & Qt::ControlModifier)) {
         TranslateView(event->pos().x(), event->pos().y(), prev_mouse_pos_.x(),
                       prev_mouse_pos_.y());
       } else if (event->buttons() & Qt::LeftButton) {
         RotateView(event->pos().x(), event->pos().y(), prev_mouse_pos_.x(),
                    prev_mouse_pos_.y());
       }
     }
     prev_mouse_pos_ = event->pos();
     event->accept();
}

void mywindow::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        ChangePointSize(event->delta());
      } else if (event->modifiers() & Qt::AltModifier) {
        ChangeCameraSize(event->delta());
      } else if (event->modifiers() & Qt::ShiftModifier) {
        ChangeNearPlane(event->delta());
      } else {
        ChangeFocusDistance(event->delta());
      }
    event->accept();
}

void mywindow::ChangeFocusDistance(const float delta)
{
    if (delta == 0.0f) {
        return;
      }
      const float prev_focus_distance = focus_distance_;
      float diff = 0.5*delta * ZoomScale() * kFocusSpeed;
      focus_distance_ -= diff;
      if (focus_distance_ < kMinFocusDistance) {
        focus_distance_ = kMinFocusDistance;
        diff = prev_focus_distance - focus_distance_;
      } else if (focus_distance_ > kMaxFocusDistance) {
        focus_distance_ = kMaxFocusDistance;
        diff = prev_focus_distance - focus_distance_;
      }
      const Eigen::Matrix4f vm_mat = QMatrixToEigen(model_matrix_).inverse();
      const Eigen::Vector3f tvec(0, 0, diff);
      const Eigen::Vector3f tvec_rot = vm_mat.block<3, 3>(0, 0) * tvec;
      model_matrix_.translate(tvec_rot(0), tvec_rot(1), tvec_rot(2));
     qDebug()<<"model matrix : "<< model_matrix_;
      ComposeProjectionMatrix();
      UploadCoordinateGridData();
      PaintGL();
}

void mywindow::ChangeNearPlane(const float delta)
{
    if (delta == 0.0f) {
        return;
      }
      near_plane_ *= (1.0f + delta / 100.0f * kNearPlaneScaleSpeed);
      near_plane_ = std::max(kMinNearPlane, std::min(kMaxNearPlane, near_plane_));
      ComposeProjectionMatrix();
      UploadCoordinateGridData();
      PaintGL();
}

void mywindow::ChangePointSize(const float delta)
{
    if (delta == 0.0f) {
        return;
      }
      point_size_ *= (1.0f + delta / 100.0f * kPointScaleSpeed);
      point_size_ = std::max(kMinPointSize, std::min(kMaxPointSize, point_size_));
      PaintGL();
}

void mywindow::ChangeCameraSize(const float delta)
{
    if (delta == 0.0f) {
        return;
      }
      image_size_ *= (1.0f + delta / 100.0f * kImageScaleSpeed);
      image_size_ = std::max(kMinImageSize, std::min(kMaxImageSize, image_size_));

      PaintGL();
}



Eigen::Vector3f mywindow::PositionToArcballVector(const float x, const float y) const
{
    Eigen::Vector3f vec(2.0f * x / width() - 1, 1 - 2.0f * y / height(), 0.0f);
      const float norm2 = vec.squaredNorm();
      if (norm2 <= 1.0f) {
        vec.z() = std::sqrt(1.0f - norm2);
      } else {
        vec = vec.normalized();
      }
      return vec;

}

void mywindow::RotateView(const float x, const float y, const float prev_x, const float prev_y)
{
    if (x - prev_x == 0 && y - prev_y == 0) {
        return;
      }

      // Determine Arcball vector on unit sphere.
      const Eigen::Vector3f u = PositionToArcballVector(x, y);
      const Eigen::Vector3f v = PositionToArcballVector(prev_x, prev_y);

      // Angle between vectors.
      const float angle = 2.0f * std::acos(std::min(1.0f, u.dot(v)));

      const float kMinAngle = 1e-3f;
      const Eigen::Matrix4f v_matrix = QMatrixToEigen(view_matrix_);
      if (angle > kMinAngle) {
        const Eigen::Matrix4f vm_mat = QMatrixToEigen(model_matrix_).inverse();
        const Eigen::Matrix4f v_mat = v_matrix.inverse();
         QMatrix4x4 q_v_mat = EigenToQMatrix(v_mat);

        // Rotation axis.
        Eigen::Vector3f axis = vm_mat.block<3, 3>(0, 0) * v.cross(u);
        Eigen::Vector3f view_axis = v_matrix.block<3, 3>(0, 0) * v.cross(u);
        axis = axis.normalized();
        qDebug()<<"axis"<<view_matrix_;
        view_axis = view_axis.normalized();
        qDebug()<<"axis"<<" "<<view_axis(0)<<" "<<view_axis(1)<<" "<<view_axis(2);
        // Center of rotation is current focus.
        const Eigen::Vector4f rot_center =
            vm_mat * Eigen::Vector4f(0, 0, -focus_distance_, 1);
        const Eigen::Vector4f eye_center =
            v_matrix * Eigen::Vector4f( view_epos.x(),view_epos.y(),view_epos.z(), 1);

        q_v_mat.translate(eye_center(0), eye_center(1), eye_center(2));
        q_v_mat.rotate(-angle*57.3, view_axis(0), view_axis(1), view_axis(2));
        q_v_mat.translate(-eye_center(0), -eye_center(1),-eye_center(2));
        view_matrix_ = q_v_mat.inverted();
        qDebug()<<" view_pos"<<view_epos;
        qDebug()<<" view_matrix"<<view_matrix_;
        qDebug()<<" center"<<eye_center(0)<<" "<<eye_center(1)<<" "<<eye_center(2);
        // First shift to rotation center, then rotate and shift back.
//        model_matrix_.translate(rot_center(0), rot_center(1), rot_center(2));//这句鸡肋啊，不就是模型坐标的原点吗
//        model_matrix_.rotate(angle*57.3, axis(0), axis(1), axis(2));
//        model_matrix_.translate(-rot_center(0), -rot_center(1),
//                                     -rot_center(2));
        PaintGL();
        }
}

void mywindow::TranslateView(const float x, const float y, const float prev_x, const float prev_y)
{
    if (x - prev_x == 0 && y - prev_y == 0) {
        return;
      }

      Eigen::Vector3f tvec(x - prev_x, prev_y - y, 0.0f);
        tvec *= ZoomScale();
      const Eigen::Matrix4f vm_mat = QMatrixToEigen(model_matrix_).inverse();
//无非是世界坐标和模型坐标的相互变换，在世界坐标的变化量，转移到模型坐标中
      const Eigen::Vector3f tvec_rot = vm_mat.block<3, 3>(0, 0) * tvec;
      //model_matrix_.translate(tvec_rot(0), tvec_rot(1), tvec_rot(2));

      PaintGL();
}

QMatrix4x4 mywindow::ModelViewMatrix() const
{
    return model_matrix_;
}


void mywindow::UploadCoordinateGridData()
{
     float scale = ZoomScale();
     scale = 0.5;

     // View center grid
     std::vector<LinePainter::Data> grid_data(3);

     grid_data[0].point1 = PointPainter::Data(-20 * scale, 0, 0, GRID_RGBA);
     grid_data[0].point2 = PointPainter::Data(20 * scale, 0, 0, GRID_RGBA);

     grid_data[1].point1 = PointPainter::Data(0, -20 * scale, 0, GRID_RGBA);
     grid_data[1].point2 = PointPainter::Data(0, 20 * scale, 0, GRID_RGBA);

     grid_data[2].point1 = PointPainter::Data(0, 0, -20 * scale, GRID_RGBA);
     grid_data[2].point2 = PointPainter::Data(0, 0, 20 * scale, GRID_RGBA);

     //coordinate_grid_painter_.Upload(grid_data);

     // Coordinate axes
     std::vector<LinePainter::Data> axes_data(3);

     axes_data[0].point1 = PointPainter::Data(0, 0, 0, X_AXIS_RGBA);
     axes_data[0].point2 = PointPainter::Data(50 * scale, 0, 0, X_AXIS_RGBA);

     axes_data[1].point1 = PointPainter::Data(0, 0, 0, Y_AXIS_RGBA);
     axes_data[1].point2 = PointPainter::Data(0, 50 * scale, 0, Y_AXIS_RGBA);

     axes_data[2].point1 = PointPainter::Data(0, 0, 0, Z_AXIS_RGBA);
     axes_data[2].point2 = PointPainter::Data(0, 0, 50 * scale, Z_AXIS_RGBA);

     //coordinate_axes_painter_.Upload(axes_data);


}


void mywindow::UpdateMovieGrabber()
{

    PaintGL();
}

void mywindow::ComposeProjectionMatrix()
{
    projection_matrix_.setToIdentity();

    projection_matrix_.perspective(kFieldOfView, AspectRatio(), near_plane_,
                                       kFarPlane);
}

float mywindow::ZoomScale() const
{
    return 2.0f * std::tan(static_cast<float>(kFieldOfView*0.017) / 2.0f) *
            std::abs(focus_distance_) / height();
}

float mywindow::AspectRatio() const
{
    return static_cast<float>(width()) / static_cast<float>(height());
}


