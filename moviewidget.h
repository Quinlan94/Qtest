#ifndef MOVIEWIDGET_H
#define MOVIEWIDGET_H


#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/StdVector>

#include <unordered_map>




//#include "mywindow.h"







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


#endif // MOVIEWIDGET_H
