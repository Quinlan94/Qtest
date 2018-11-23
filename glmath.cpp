
#include "glmath.h"



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
Eigen::Vector4d RotationMatrixToQuaternion(const Eigen::Matrix3d& rot_mat) {
  const Eigen::Quaterniond quat(rot_mat);
  return Eigen::Vector4d(quat.w(), quat.x(), quat.y(), quat.z());
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

Eigen::Matrix3x4d InvertProjectionMatrix(const Eigen::Matrix3x4d &proj_matrix)
{
    Eigen::Matrix3x4d inv_proj_matrix;
    inv_proj_matrix.leftCols<3>() = proj_matrix.leftCols<3>().transpose();
    inv_proj_matrix.rightCols<1>() = ProjectionCenterFromMatrix(proj_matrix);
    return inv_proj_matrix;
}


Eigen::Matrix4f QMatrixToEigen(const QMatrix4x4& matrix){
  Eigen::Matrix4f eigen;
  for (size_t r = 0; r < 4; ++r) {
    for (size_t c = 0; c < 4; ++c) {
      eigen(r, c) = matrix(r, c);
    }
  }
  return eigen;
}
QMatrix3x3 EigenToQMatrix3(const Eigen::Matrix3f &matrix)
{
    QMatrix3x3 qt;
    for (size_t r = 0; r < 3; ++r) {
      for (size_t c = 0; c < 3; ++c) {
        qt(r, c) = matrix(r, c);
      }
    }
    return qt;
}
QMatrix4x4 EigenToQMatrix(const Eigen::Matrix4f &matrix)
{
    QMatrix4x4 qt;
    for (size_t r = 0; r < 4; ++r) {
      for (size_t c = 0; c < 4; ++c) {
        qt(r, c) = matrix(r, c);
      }
    }
    return qt;
}
