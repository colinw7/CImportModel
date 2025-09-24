#ifndef CQNewGLShaderProgram_H
#define CQNewGLShaderProgram_H

#include <QOpenGLShaderProgram>
#include <QString>

class CQNewGLCanvas;
class CQGLBuffer;

class CQNewGLShaderProgram : public QOpenGLShaderProgram {
 public:
  CQNewGLShaderProgram(CQNewGLCanvas *canvas);

  void addShaders(const QString &vertex, const QString &fragment);

  void addVertexShader  (const QString &name);
  void addFragmentShader(const QString &name);

//CQGLBuffer *getBuffer();
  CQGLBuffer *createBuffer();

 private:
  CQNewGLCanvas* canvas_ { nullptr };
//CQGLBuffer*    buffer_ { nullptr };
};

#endif
