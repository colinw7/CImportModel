#ifndef CQCamera3DShaderProgram_H
#define CQCamera3DShaderProgram_H

#include <QOpenGLShaderProgram>
#include <QString>

class CQCamera3DApp;
class CQGLBuffer;

class CQCamera3DShaderProgram : public QOpenGLShaderProgram {
 public:
  CQCamera3DShaderProgram(CQCamera3DApp *app);

  void addShaders(const QString &vertex, const QString &fragment);

  void addVertexShader  (const QString &name);
  void addFragmentShader(const QString &name);

  CQGLBuffer *createBuffer();

 private:
  CQCamera3DApp* app_ { nullptr };
};

#endif
