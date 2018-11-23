#ifndef GLMATH_H
#define GLMATH_H


#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>



#include <QOpenGLShaderProgram>



namespace Eigen {

    typedef Eigen::Matrix<float,6,1> Vector6f;
    typedef Eigen::Matrix<double, 3, 4> Matrix3x4d;
}



template <typename T>
T Sigmoid(const T x, const T alpha = 1);


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


Eigen::Vector3d ProjectionCenterFromMatrix(const Eigen::Matrix3x4d& proj_matrix) ;
Eigen::Vector4d NormalizeQuaternion(const Eigen::Vector4d& qvec);
Eigen::Vector4d RotationMatrixToQuaternion(const Eigen::Matrix3d& rot_mat);


Eigen::Matrix4f QMatrixToEigen(const QMatrix4x4& matrix);

QMatrix3x3 EigenToQMatrix3(const Eigen::Matrix3f &matrix);

QMatrix4x4 EigenToQMatrix(const Eigen::Matrix4f &matrix);





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







#endif // GLMATH_H
