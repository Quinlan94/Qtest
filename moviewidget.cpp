#include "moviewidget.h"
#include "mywindow.h"  //前置声明，在实现文件中也要包含该头文件

template <typename T>
T Sigmoid(const T x, const T alpha) {
  return T(1) / (T(1) + exp(-x * alpha));
}

template <typename T>
T ScaleSigmoid(T x, const T alpha, const T x0) {
  const T t0 = Sigmoid(-x0, alpha);
  const T t1 = Sigmoid(x0, alpha);
  x = (Sigmoid(2 * x0 * x - x0, alpha) - t0) / (t1 - t0);
  return x;
}
Eigen::Vector3d ProjectionCenterFromMatrix(
    const Eigen::Matrix3x4d& proj_matrix) {
  return -proj_matrix.leftCols<3>().transpose() * proj_matrix.rightCols<1>();
}
Eigen::Vector4d NormalizeQuaternion(const Eigen::Vector4d& qvec) {
  const double norm = qvec.norm();
  if (norm == 0) {
    // We do not just use (1, 0, 0, 0) because that is a constant and when used
    // for automatic differentiation that would lead to a zero derivative.
    return Eigen::Vector4d(1.0, qvec(1), qvec(2), qvec(3));
  } else {
    const double inv_norm = 1.0 / norm;
    return inv_norm * qvec;
  }
}
QMatrix4x4 EigenToQMatrix(const Eigen::Matrix4f& matrix) {
  QMatrix4x4 qt;
  for (size_t r = 0; r < 4; ++r) {
    for (size_t c = 0; c < 4; ++c) {
      qt(r, c) = matrix(r, c);
    }
  }
  return qt;
}

Eigen::Matrix4f QMatrixToEigen(const QMatrix4x4& matrix) {
  Eigen::Matrix4f eigen;
  for (size_t r = 0; r < 4; ++r) {
    for (size_t c = 0; c < 4; ++c) {
      eigen(r, c) = matrix(r, c);
    }
  }
  return eigen;
}

Eigen::Vector4d RotationMatrixToQuaternion(const Eigen::Matrix3d& rot_mat) {
  const Eigen::Quaterniond quat(rot_mat);
  return Eigen::Vector4d(quat.w(), quat.x(), quat.y(), quat.z());
}

MovieWidget::MovieWidget(QWidget* parent, mywindow* opengl_window)
    :QWidget(parent), opengl_window_(opengl_window)
{

      setWindowFlags(Qt::Widget | Qt::WindowStaysOnTopHint | Qt::Tool);
      setWindowTitle("Grab movie");

      QGridLayout* grid = new QGridLayout(this);
      grid->setContentsMargins(0, 5, 0, 5);

      add_button_ = new QPushButton(tr("Add"), this);
      connect(add_button_, &QPushButton::released, this, &MovieWidget::Add);
      grid->addWidget(add_button_, 0, 0);

      delete_button_ = new QPushButton(tr("Delete"), this);
      connect(delete_button_, &QPushButton::released, this,
              &MovieWidget::Delete);
      grid->addWidget(delete_button_, 0, 1);

      clear_button_ = new QPushButton(tr("Clear"), this);
      connect(clear_button_, &QPushButton::released, this,
              &MovieWidget::Clear);
      grid->addWidget(clear_button_, 0, 2);

      table_ = new QTableWidget(this);
      table_->setColumnCount(1);
      QStringList table_header;
      table_header << "Time [seconds]";
      table_->setHorizontalHeaderLabels(table_header);
      table_->resizeColumnsToContents();
      table_->setShowGrid(true);
      table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
      table_->verticalHeader()->setVisible(true);
      table_->verticalHeader()->setDefaultSectionSize(18);
      table_->setSelectionMode(QAbstractItemView::SingleSelection);
      table_->setSelectionBehavior(QAbstractItemView::SelectRows);
      connect(table_, &QTableWidget::itemChanged, this,
              &MovieWidget::TimeChanged);
      connect(table_->selectionModel(), &QItemSelectionModel::selectionChanged,
              this, &MovieWidget::SelectionChanged);
      grid->addWidget(table_, 1, 0, 1, 3);

      grid->addWidget(new QLabel(tr("Frame rate"), this), 2, 1);
      frame_rate_sb_ = new QSpinBox(this);
      frame_rate_sb_->setMinimum(1);
      frame_rate_sb_->setMaximum(1000);
      frame_rate_sb_->setSingleStep(1);
      frame_rate_sb_->setValue(100);
      grid->addWidget(frame_rate_sb_, 2, 2);

      grid->addWidget(new QLabel(tr("Smooth transition"), this), 3, 1);
      smooth_cb_ = new QCheckBox(this);
      smooth_cb_->setChecked(true);
      grid->addWidget(smooth_cb_, 3, 2);

      grid->addWidget(new QLabel(tr("Smoothness"), this), 4, 1);
      smoothness_sb_ = new QDoubleSpinBox(this);
      smoothness_sb_->setMinimum(0);
      smoothness_sb_->setMaximum(1);
      smoothness_sb_->setSingleStep(0.01);
      smoothness_sb_->setValue(0.5);
      grid->addWidget(smoothness_sb_, 4, 2);

      assemble_button_ = new QPushButton(tr("Assemble movie"), this);
      connect(assemble_button_, &QPushButton::released, this,
              &MovieWidget::Assemble);
      grid->addWidget(assemble_button_, 5, 1, 1, 2);
      //qDebug(" movie initial");
}

void MovieWidget::Add()
{
      const QMatrix4x4 matrix = opengl_window_->ModelViewMatrix();

      double time = 0;
      if (table_->rowCount() > 0) {
        time = table_->item(table_->rowCount() - 1, 0)->text().toDouble() + 1;
      }

      QTableWidgetItem* item = new QTableWidgetItem();
      item->setData(Qt::DisplayRole, time);
      item->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable |
                     Qt::ItemIsEditable);
      item->setTextAlignment(Qt::AlignRight);

      // Save size state of current viewpoint.
      ViewData view_data;
      view_data.model_view_matrix = matrix;
      view_data.point_size = opengl_window_->PointSize();
      view_data.image_size = opengl_window_->ImageSize();
      view_data_.emplace(item, view_data);

      table_->insertRow(table_->rowCount());
      table_->setItem(table_->rowCount() - 1, 0, item);
      table_->selectRow(table_->rowCount() - 1);

      // Zoom out a little, so that we can see the newly added camera
      opengl_window_->ChangeFocusDistance(-5);
}

void MovieWidget::Delete()
{
     QModelIndexList selection = table_->selectionModel()->selectedIndexes();
     foreach (QModelIndex index, selection) { table_->removeRow(index.row()); }
     UpdateViews();
     opengl_window_->UpdateMovieGrabber();
}

void MovieWidget::Clear()
{

}
void MovieWidget::Assemble() {
  if (table_->rowCount() < 2) {
    QMessageBox::critical(this, tr("Error"),
                          tr("You must add at least two control views."));
    return;
  }


  const QString path = QFileDialog::getExistingDirectory(
      this, tr("Choose destination..."), "", QFileDialog::ShowDirsOnly);

  // File dialog cancelled?
  if (path == "") {
    return;
  }

  const QDir dir = QDir(path);

  const QMatrix4x4 model_view_matrix_cached = opengl_window_->ModelViewMatrix();
  const float point_size_cached = opengl_window_->PointSize();
  const float image_size_cached = opengl_window_->ImageSize();
//  const std::vector<Image> views_cached = views;

//  // Make sure we do not render movie grabber path.
//  views.clear();
//  opengl_window_->UpdateMovieGrabber();
//  opengl_window_->DisableCoordinateGrid();

  const float frame_rate = frame_rate_sb_->value();
  const float frame_time = 1.0f / frame_rate;
  size_t frame_number = 0;

  // Data of first view.
  const Eigen::Matrix4d prev_model_view_matrix =
      QMatrixToEigen(view_data_[table_->item(0, 0)].model_view_matrix)
          .cast<double>();
  const Eigen::Matrix3x4d prev_view_model_matrix =
      InvertProjectionMatrix(prev_model_view_matrix.topLeftCorner<3, 4>());
  Eigen::Vector4d prev_qvec =
      RotationMatrixToQuaternion(prev_view_model_matrix.block<3, 3>(0, 0));
  Eigen::Vector3d prev_tvec = prev_view_model_matrix.block<3, 1>(0, 3);

  for (int row = 1; row < table_->rowCount(); ++row) {
    const auto logical_idx = table_->verticalHeader()->logicalIndex(row);
    QTableWidgetItem* prev_table_item = table_->item(logical_idx - 1, 0);
    QTableWidgetItem* table_item = table_->item(logical_idx, 0);

    const ViewData& prev_view_data = view_data_.at(prev_table_item);
    const ViewData& view_data = view_data_.at(table_item);

    // Data of next view.
    const Eigen::Matrix4d curr_model_view_matrix =
        QMatrixToEigen(view_data.model_view_matrix).cast<double>();
    const Eigen::Matrix3x4d curr_view_model_matrix =
        InvertProjectionMatrix(curr_model_view_matrix.topLeftCorner<3, 4>());
    const Eigen::Vector4d curr_qvec =
        RotationMatrixToQuaternion(curr_view_model_matrix.block<3, 3>(0, 0));
    const Eigen::Vector3d curr_tvec = curr_view_model_matrix.block<3, 1>(0, 3);

    // Time difference between previous and current view.
    const float dt = std::abs(table_item->text().toFloat() -
                              prev_table_item->text().toFloat());

    // Point size differences between previous and current view.
    const float dpoint_size = view_data.point_size - prev_view_data.point_size;
    const float dimage_size = view_data.image_size - prev_view_data.image_size;

    const auto num_frames = dt * frame_rate;
    for (size_t i = 0; i < num_frames; ++i) {
      const float t = i * frame_time;
      float tt = t / dt;

      if (smooth_cb_->isChecked()) {
        tt = ScaleSigmoid(tt, static_cast<float>(smoothness_sb_->value()));
      }

      // Compute current model-view matrix.
      Eigen::Vector4d interp_qvec;
      Eigen::Vector3d interp_tvec;
      InterpolatePose(prev_qvec, prev_tvec, curr_qvec, curr_tvec, tt,
                      &interp_qvec, &interp_tvec);

      Eigen::Matrix4d frame_model_view_matrix = Eigen::Matrix4d::Identity();
      frame_model_view_matrix.topLeftCorner<3, 4>() = InvertProjectionMatrix(
          ComposeProjectionMatrix(interp_qvec, interp_tvec));

      opengl_window_->SetModelViewMatrix(
          EigenToQMatrix(frame_model_view_matrix.cast<float>()));

      // Set point and image sizes.
//      opengl_window_->SetPointSize(prev_view_data.point_size +
//                                   dpoint_size * tt);
//      opengl_window_->SetImageSize(prev_view_data.image_size +
//                                   dimage_size * tt);

      QImage image = opengl_window_->GrabImage();
      image.save(dir.filePath(
          "frame" + QString().sprintf("%06zu", frame_number) + ".png"));
      frame_number += 1;
    }

    prev_qvec = curr_qvec;
    prev_tvec = curr_tvec;
  }

  //views = views_cached;
  opengl_window_->SetPointSize(point_size_cached);
  opengl_window_->SetImageSize(image_size_cached);
  //opengl_window_->UpdateMovieGrabber();
  opengl_window_->EnableCoordinateGrid();
  opengl_window_->SetModelViewMatrix(model_view_matrix_cached);
}

void MovieWidget::TimeChanged(QTableWidgetItem* item) {
  table_->sortItems(0, Qt::AscendingOrder);
  UpdateViews();
  opengl_window_->UpdateMovieGrabber();
}

void MovieWidget::SelectionChanged(const QItemSelection& selected,
                                          const QItemSelection& deselected) {
//  foreach (QModelIndex index, table_->selectionModel()->selectedIndexes())
//  {
//    opengl_window_->SelectMoviewGrabberView(index.row());
//  }
}

void MovieWidget::UpdateViews() {
//  views.clear();
//  for (int row = 0; row < table_->rowCount(); ++row) {
//    const auto logical_idx = table_->verticalHeader()->logicalIndex(row);
//    QTableWidgetItem* item = table_->item(logical_idx, 0);

//    const Eigen::Matrix4d model_view_matrix =
//        QMatrixToEigen(view_data_.at(item).model_view_matrix).cast<double>();
//    Image image;
//    image.Qvec() =
//        RotationMatrixToQuaternion(model_view_matrix.block<3, 3>(0, 0));
//    image.Tvec() = model_view_matrix.block<3, 1>(0, 3);
//    views.push_back(image);
//  }
}

void InterpolatePose(const Eigen::Vector4d &qvec1, const Eigen::Vector3d &tvec1, const Eigen::Vector4d &qvec2, const Eigen::Vector3d &tvec2, const double t, Eigen::Vector4d *qveci, Eigen::Vector3d *tveci)
{
    const Eigen::Vector4d normalized_qvec1 = NormalizeQuaternion(qvec1);
      const Eigen::Vector4d normalized_qvec2 = NormalizeQuaternion(qvec2);
      const Eigen::Quaterniond quat1(normalized_qvec1(0), normalized_qvec1(1),
                                     normalized_qvec1(2), normalized_qvec1(3));
      const Eigen::Quaterniond quat2(normalized_qvec2(0), normalized_qvec2(1),
                                     normalized_qvec2(2), normalized_qvec2(3));
      const Eigen::Vector3d tvec12 = tvec2 - tvec1;

      const Eigen::Quaterniond quati = quat1.slerp(t, quat2);

      *qveci = Eigen::Vector4d(quati.w(), quati.x(), quati.y(), quati.z());
      *tveci = tvec1 + tvec12 * t;
}

Eigen::Matrix3x4d InvertProjectionMatrix(const Eigen::Matrix3x4d &proj_matrix)
{
    Eigen::Matrix3x4d inv_proj_matrix;
    inv_proj_matrix.leftCols<3>() = proj_matrix.leftCols<3>().transpose();
    inv_proj_matrix.rightCols<1>() = ProjectionCenterFromMatrix(proj_matrix);
    return inv_proj_matrix;
}

Eigen::Matrix3x4d ComposeProjectionMatrix(const Eigen::Vector4d &qvec, const Eigen::Vector3d &tvec)
{
     Eigen::Matrix3x4d proj_matrix;
     proj_matrix.leftCols<3>() = QuaternionToRotationMatrix(qvec);
     proj_matrix.rightCols<1>() = tvec;
     return proj_matrix;
}

Eigen::Matrix3d QuaternionToRotationMatrix(const Eigen::Vector4d &qvec)
{
    const Eigen::Vector4d normalized_qvec = NormalizeQuaternion(qvec);
    const Eigen::Quaterniond quat(normalized_qvec(0), normalized_qvec(1),
                                  normalized_qvec(2), normalized_qvec(3));
    return quat.toRotationMatrix();
}
