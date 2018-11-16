#include "trianglepainter.h"


TrianglePainter::TrianglePainter(): num_geoms_(0)
{
    num_geoms_=0;

}

TrianglePainter::~TrianglePainter() {
  vao_.destroy();
  vbo_.destroy();
}

void TrianglePainter::Setup() {
  vao_.destroy();
  vbo_.destroy();
  if (shader_program_.isLinked()) {
    shader_program_.release();
    shader_program_.removeAllShaders();
  }

  shader_program_.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                          ":/new/prefix1/shaders/triangles.v.glsl");
  shader_program_.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                          ":/new/prefix1/shaders/triangles.f.glsl");
  shader_program_.link();
  shader_program_.bind();
 qDebug()<<"link success";
  vao_.create();
  vbo_.create();

#if DEBUG
  glDebugLog();
#endif
}

void TrianglePainter::Upload(const std::vector<TrianglePainter::Data>& data) {
  num_geoms_ = data.size();
  if (num_geoms_ == 0) {
    return;
  }

  vao_.bind();
  vbo_.bind();

  // Upload data array to GPU
  vbo_.setUsagePattern(QOpenGLBuffer::DynamicDraw);
  vbo_.allocate(data.data(),
                static_cast<int>(data.size() * sizeof(TrianglePainter::Data)));

  // in_position
  shader_program_.enableAttributeArray(0);
  shader_program_.setAttributeBuffer(0, GL_FLOAT, 0, 3,
                                     sizeof(PointPainter::Data));

  // 法线
  shader_program_.enableAttributeArray(1);
  shader_program_.setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 3,
                                     sizeof(PointPainter::Data));

  //纹理坐标
  shader_program_.enableAttributeArray(2);
  shader_program_.setAttributeBuffer(2, GL_FLOAT, 6 * sizeof(GLfloat), 2,
                                     sizeof(PointPainter::Data));

  // Make sure they are not changed from the outside
  vbo_.release();
  vao_.release();

#if DEBUG
  glDebugLog();
#endif
}

void TrianglePainter::Render(const QMatrix4x4& pmv_matrix) {
  if (num_geoms_ == 0) {
    return;
  }

  qDebug()<<"sanjiaomianpian";
  shader_program_.bind();
  vao_.bind();

  shader_program_.setUniformValue("u_pmv_matrix", pmv_matrix);
  shader_program_.setUniformValue("texture", 0);

  glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(3 * num_geoms_));


  // Make sure the VAO is not changed from the outside
  vao_.release();

#if DEBUG
  glDebugLog();
#endif
}
