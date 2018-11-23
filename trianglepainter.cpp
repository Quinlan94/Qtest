#include "trianglepainter.h"


TrianglePainter::TrianglePainter(): num_geoms_(0)
{
    num_geoms_=0;
    texture =0;
    tex =0;

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

//  texture_tri = new QOpenGLTexture(QImage("D:/scene_dense_mesh_texture.png").mirrored());


////  if(texture_tri = NULL)
////  {
////      qDebug("error read image,please check path!");
////      return false;
////  }

// texture_tri->setMinificationFilter(QOpenGLTexture::Nearest);


// texture_tri->setMagnificationFilter(QOpenGLTexture::Linear);


// texture_tri->setWrapMode(QOpenGLTexture::Repeat);




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
  //着色器语句，有几个in，就必须几个out，还要在主函数里赋值！！！！holy shit


  vbo_.release();
  vao_.release();

#if DEBUG
  glDebugLog();
#endif
}

void TrianglePainter::Render(const QMatrix4x4& pmv_matrix, std::vector<QOpenGLTexture *> texture_tri, size_t *num_tex_index) {
  if (num_geoms_ == 0) {
    return;
  }

  QVector4D mat_specular(1.0, 1.0, 1.0, 1.0);  //镜面反射参数
  GLfloat mat_shininess[] = { 50.0 };               //高光指数
  QVector4D light_position(1.0, 1.0, 1.0, 0.0 );
  QVector4D white_light( 1.0, 1.0, 1.0, 1.0 );   //灯位置(1,1,1), 最后1-开关
  QVector4D Light_Model_Ambient(0.2, 0.2, 0.2, 1.0); //环境光参数
  QVector4D diffuseMaterial(0.5, 0.5, 0.5, 1.0 );


  glShadeModel(GL_SMOOTH);
/*  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_shininess);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_shininess);

          //灯光设置
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);   //散射光属性
  glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);  //镜面反射光
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Light_Model_Ambient);  *///环境光参数

  glEnable(GL_LIGHTING);   //开关:使用光
  glEnable(GL_LIGHT0);     //打开0#灯


  shader_program_.bind();
  vao_.bind();
 // texture_tri->bind();




  shader_program_.setUniformValue("u_pmv_matrix", pmv_matrix);
  shader_program_.setUniformValue("tri_mv_matrix_", tri_mv_matrix_);
  shader_program_.setUniformValue("normal_matrix_", normal_matrix_);
  shader_program_.setUniformValue("vLightPosition", light_position);
  shader_program_.setUniformValue("ambientColor", Light_Model_Ambient);
  shader_program_.setUniformValue("diffuseColor", diffuseMaterial);
  shader_program_.setUniformValue("specularColor", mat_specular);

  shader_program_.setUniformValue("texture_tri", 0);

  tex = num_tex_index;
  size_t draw_size =0;
  for(int i =0;i<texture_tri.size();i++)
  {
      texture = texture_tri[i];
      texture->bind();
      glDrawArrays(GL_TRIANGLES, (GLsizei)(draw_size), (GLsizei)(3 * tex[i]));
      draw_size+= tex[i]*3;
  }





  vao_.release();

#if DEBUG
  glDebugLog();
#endif
}
