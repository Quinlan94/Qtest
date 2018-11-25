#ifndef TRIANGLEPAINTER_H
#define TRIANGLEPAINTER_H
#include <QtCore>
#include <QtOpenGL>

#include "pointpainter.h"

class TrianglePainter
{
public:
    TrianglePainter();

    ~TrianglePainter();

     struct Data {
       Data() {}
       Data(const PointPainter::Data& p1, const PointPainter::Data& p2,
            const PointPainter::Data& p3)
           : point1(p1), point2(p2), point3(p3) {}

       PointPainter::Data point1;
       PointPainter::Data point2;
       PointPainter::Data point3;
     };

     void Setup();
     void Upload(const std::vector<TrianglePainter::Data>& data);
     void Render(const QMatrix4x4& pmv_matrix, std::vector<QOpenGLTexture*> texture_tri,
                 size_t *num_tex_index);

      QMatrix4x4 tri_model_matrix_;
      QMatrix4x4 tri_view_matrix_;
      QMatrix3x3 normal_matrix_;

      QOpenGLTexture *texture;
      size_t *tex;



    private:

     QOpenGLShaderProgram shader_program_;
     QOpenGLVertexArrayObject vao_;
     QOpenGLBuffer vbo_;

     size_t num_geoms_;
};

#endif // TRIANGLEPAINTER_H
