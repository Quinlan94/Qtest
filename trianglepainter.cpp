#include "trianglepainter.h"


TrianglePainter::TrianglePainter(): num_geoms_(0)
{
    num_geoms_=0;
    texture_tri = 0;

}

TrianglePainter::~TrianglePainter() {
  vao_.destroy();
  vbo_.destroy();
  delete texture_tri;
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

  texture_tri = new QOpenGLTexture(QImage("C:/Users/zou/Desktop/openProject/Qtest/scene_dense_mesh_texture.png").mirrored());
  texture_tri->setMinificationFilter(QOpenGLTexture::Nearest);

  // Set bilinear filtering mode for texture magnification
  texture_tri->setMagnificationFilter(QOpenGLTexture::Linear);

  // Wrap texture coordinates by repeating
  // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
  texture_tri->setWrapMode(QOpenGLTexture::Repeat);


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

  qDebug()<<"first textcoord"<<data[0].point1.tx<<" "<<data[0].point1.ty;
  // Upload data array to GPU
  vbo_.setUsagePattern(QOpenGLBuffer::DynamicDraw);
  vbo_.allocate(data.data(),
                static_cast<int>(data.size() * sizeof(TrianglePainter::Data)));

  // in_position
  shader_program_.enableAttributeArray(0);
  shader_program_.setAttributeBuffer(0, GL_FLOAT, 0, 3,
                                     sizeof(PointPainter::Data));
  qDebug()<<"sizeof(PointPainter::Data)"<<sizeof(PointPainter::Data);

  // 法线
  shader_program_.enableAttributeArray(1);
  shader_program_.setAttributeBuffer(1, GL_FLOAT, 7 * sizeof(GLfloat), 3,
                                     sizeof(PointPainter::Data));

  //纹理坐标
  shader_program_.enableAttributeArray(2);
  shader_program_.setAttributeBuffer(2, GL_FLOAT, 10 * sizeof(GLfloat), 2,
                                     sizeof(PointPainter::Data));


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
    texture_tri->bind();

  shader_program_.bind();
  vao_.bind();



  shader_program_.setUniformValue("u_pmv_matrix", pmv_matrix);
  shader_program_.setUniformValue("texture_tri", 0);

  glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(3 * num_geoms_));



  vao_.release();

#if DEBUG
  glDebugLog();
#endif
}
