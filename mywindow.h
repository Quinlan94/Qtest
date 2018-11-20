#ifndef MYWINDOW_H
#define MYWINDOW_H
#pragma execution_character_set("utf-8")
#include <QWindow>
#include <QtOpenGL>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <QTime>


#include <boost/algorithm/string.hpp>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

#include "moviewidget.h"

#include "linepainter.h"
#include "pointpainter.h"
#include "trianglepainter.h"

#include <unordered_map>



namespace Eigen {

    typedef Eigen::Matrix<float,6,1> Vector6f;
}


class mywindow : public QWindow
{
public:
    mywindow(QWidget* parent,QScreen* screen = 0);

    std::vector<Eigen::Vector6f> _Points;
    std::vector<Eigen::Vector3i> _Vertices;
    std::vector<Eigen::Vector6f> _Textures;

    std::string texture_name;
    QOpenGLTexture *texture_tri;




    const float kInitNearPlane = 1.0f;
      const float kMinNearPlane = 1e-3f;
      const float kMaxNearPlane = 1e5f;
      const float kNearPlaneScaleSpeed = 0.02f;
      const float kFarPlane = 1e5f;
      const float kInitFocusDistance = 100.0f;
      const float kMinFocusDistance = 1e-5f;
      const float kMaxFocusDistance = 1e8f;
      const float kFieldOfView = 25.0f;
      const float kFocusSpeed = 2.0f;
      const float kInitPointSize = 1.0f;
      const float kMinPointSize = 0.5f;
      const float kMaxPointSize = 100.0f;
      const float kPointScaleSpeed = 0.1f;
      const float kInitImageSize = 0.2f;
      const float kMinImageSize = 1e-6f;
      const float kMaxImageSize = 1e3f;
      const float kImageScaleSpeed = 0.1f;
      const int kDoubleClickInterval = 250;


    void SetupGL();
    void InitializeGL();
    void ResizeGL();
    void PaintGL();

    void initTextures();

    float PointSize() const;
    float ImageSize() const;

    void SetPointSize(const float point_size);
    void SetImageSize(const float image_size);

    void Update();
    void Upload();
    void UploadMeshData();
    void Clear();

    void ChangeFocusDistance(const float delta);
    void ChangeNearPlane(const float delta);
    void ChangePointSize(const float delta);
    void ChangeCameraSize(const float delta);

    void printMatrix(QMatrix4x4 matrix);
    void printMatrix(Eigen::Matrix4f matrix);
     void SetModelViewMatrix(const QMatrix4x4& matrix);
     void UpdateMovieGrabber();

      QImage GrabImage();
      void GrabMovies();

      void EnableCoordinateGrid();
      void DisableCoordinateGrid();
       QMatrix4x4 ModelViewMatrix() const;




//    EIGEN_STL_UMAP(colmap::camera_t, colmap::Camera) cameras;
//    EIGEN_STL_UMAP(colmap::image_t, colmap::Image) images;
//    EIGEN_STL_UMAP(colmap::point3D_t, colmap::Point3D) points3D;
//    std::unordered_map<camera_t, Camera, std::hash<camera_t>,
//    std::equal_to<camera_t>,Eigen::aligned_allocator<std::pair<camera_t const, Camera>>> cameras;

//    std::unordered_map<image_t, Image, std::hash<image_t>,
//    std::equal_to<image_t>,Eigen::aligned_allocator<std::pair<image_t const, Image>>> images;

//    std::unordered_map<point3D_t, Point3D, std::hash<point3D_t>,
//    std::equal_to<point3D_t>,Eigen::aligned_allocator<std::pair<point3D_t const, Point3D>>> points3D;

protected:
    void exposeEvent(QExposeEvent *);


    Eigen::Matrix4f QMatrixToEigen(const QMatrix4x4& matrix);


private:
    QOpenGLContext* context_;

    MovieWidget* movie_grabber_widget_;


    bool coordinate_grid_enabled_;
    bool movie_grab_enabled_;

    void InitializePainters();
    void InitializeSettings();
    void InitializeView();

    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);



    void SelectObject(const int x, const int y);


    void RotateView(const float x, const float y, const float prev_x,
                      const float prev_y);
    void TranslateView(const float x, const float y, const float prev_x,
                         const float prev_y);

    void ResetView();



    void UploadCoordinateGridData();
    void UploadPointData(const bool selection_mode = false);
    void UploadPointConnectionData();
    void UploadImageData(const bool selection_mode = false);
    void UploadImageConnectionData();
    void UploadMovieGrabberData();



     void ComposeProjectionMatrix();

     float ZoomScale() const;
     float AspectRatio() const;
     float OrthographicWindowExtent() const;

    // Eigen::Vector4ub ReadPixelColor(int x, int y) const;
     Eigen::Vector3f PositionToArcballVector(const float x, const float y) const;

     bool mouse_is_pressed_;
     QTimer mouse_press_timer_;
     QPoint prev_mouse_pos_;



    float focus_distance_;
    // Size of points (dynamic): does not require re-uploading of points.
    float point_size_;
    // Size of image models (not dynamic): requires re-uploading of image models.
    float image_size_;
    // Near clipping plane.
     float near_plane_;


    float bg_color_[3];
    std::vector<std::pair<size_t, char>> selection_buffer_;


//     image_t selected_image_id_;
//     point3D_t selected_point3D_id_;
     //std::vector<image_t> reg_image_ids;

     size_t selected_movie_grabber_view_;



    QMatrix4x4 model_view_matrix_;
    QMatrix4x4 projection_matrix_;

    LinePainter coordinate_axes_painter_;
    LinePainter coordinate_grid_painter_;

    PointPainter point_painter_;
    LinePainter point_connection_painter_;

    LinePainter image_line_painter_;
    TrianglePainter image_triangle_painter_;
    LinePainter image_connection_painter_;

    LinePainter movie_grabber_path_painter_;
    LinePainter movie_grabber_line_painter_;
    TrianglePainter movie_grabber_triangle_painter_;
};


#endif // MYWINDOW_H
