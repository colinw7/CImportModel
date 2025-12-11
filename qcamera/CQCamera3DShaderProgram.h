#ifndef CQCamera3DShaderProgram_H
#define CQCamera3DShaderProgram_H

#include <QOpenGLShaderProgram>
#include <QString>

class CQCamera3DCanvas;
class CQGLBuffer;

class CQCamera3DShaderProgram : public QOpenGLShaderProgram {
 public:
  CQCamera3DShaderProgram(CQCamera3DCanvas *canvas);

  void addShaders(const QString &vertex, const QString &fragment);

  void addVertexShader  (const QString &name);
  void addFragmentShader(const QString &name);

  CQGLBuffer *createBuffer();

 private:
  CQCamera3DCanvas* canvas_ { nullptr };
};

#endif
