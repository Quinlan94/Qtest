#ifndef MOVIEWIDGET_H
#define MOVIEWIDGET_H

#include <initializer_list>
#include <memory>
#include <vector>




#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/StdVector>


#include <unordered_map>

//#include "mywindow.h"





namespace Eigen {

typedef Eigen::Matrix<double, 3, 4> Matrix3x4d;

}

class mywindow;

class MovieWidget : public QWidget
{
    Q_OBJECT
public:
    MovieWidget(QWidget* parent, mywindow* opengl_window);

    //std::vector<Image> views;

    struct ViewData {
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        QMatrix4x4 model_view_matrix;
        float point_size = -1.0f;
        float image_size = -1.0f;
      };
private:

      void Add();
      void Delete();
      void Clear();

      // Assemble movie from current viewpoints.
      void Assemble();

      // Event slot for time modification.
      void TimeChanged(QTableWidgetItem* item);

      // Event slot for changed selection.
      void SelectionChanged(const QItemSelection& selected,
                            const QItemSelection& deselected);

      // Update state when viewpoints reordered.
      void UpdateViews();

      mywindow* opengl_window_;

      QPushButton* assemble_button_;
      QPushButton* add_button_;
      QPushButton* delete_button_;
      QPushButton* clear_button_;
      QTableWidget* table_;

      QSpinBox* frame_rate_sb_;
      QCheckBox* smooth_cb_;
      QDoubleSpinBox* smoothness_sb_;

      std::unordered_map<const QTableWidgetItem*, ViewData> view_data_;

      //EIGEN_STL_UMAP(const QTableWidgetItem*, ViewData) view_data_;


};

template <typename T>
T Sigmoid(const T x, const T alpha = 1);

// Scale values according to sigmoid transform.
//
//   x \in [0, 1] -> x \in [-x0, x0] -> sigmoid(x, alpha) -> x \in [0, 1]
//
// @param x        Value to be scaled in the range [0, 1].
// @param x0       Spread that determines the range x is scaled to.
// @param alpha    Exponential sigmoid factor.
//
// @return         The scaled value in the range [0, 1].
template <typename T>
T ScaleSigmoid(T x, const T alpha = 1, const T x0 = 10);

void InterpolatePose(const Eigen::Vector4d& qvec1, const Eigen::Vector3d& tvec1,
                     const Eigen::Vector4d& qvec2, const Eigen::Vector3d& tvec2,
                     const double t, Eigen::Vector4d* qveci,
                     Eigen::Vector3d* tveci);
Eigen::Matrix3x4d InvertProjectionMatrix(const Eigen::Matrix3x4d& proj_matrix);
Eigen::Matrix3x4d ComposeProjectionMatrix(const Eigen::Vector4d& qvec,
                                          const Eigen::Vector3d& tvec);
Eigen::Matrix3d QuaternionToRotationMatrix(const Eigen::Vector4d& qvec);

#endif // MOVIEWIDGET_H
