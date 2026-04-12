#ifndef ShaderProgram_H
#define ShaderProgram_H

#include <QOpenGLShaderProgram>
#include <QString>

class CQGLBuffer;

namespace CQSkybox3D {

class App;

class ShaderProgram : public QOpenGLShaderProgram {
 public:
  ShaderProgram(App *app, const QString &name);

  void addShaders(const QString &vertex, const QString &fragment);

  void addVertexShader  (const QString &name);
  void addFragmentShader(const QString &name);

  CQGLBuffer *createBuffer();

 private:
  App*    app_ { nullptr };
  QString name_;
};

}

#endif
