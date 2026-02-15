#ifndef ShaderProgram_H
#define ShaderProgram_H

#include <QOpenGLShaderProgram>
#include <QString>

class CQGLBuffer;

namespace CQTextureGen {

class App;

class ShaderProgram : public QOpenGLShaderProgram {
 public:
  using App = CQTextureGen::App;

 public:
  ShaderProgram(App *app);

  void addShaders(const QString &vertex, const QString &fragment);

  void addVertexShader  (const QString &name);
  void addFragmentShader(const QString &name);

  CQGLBuffer *createBuffer();

 private:
  App* app_ { nullptr };
};

}

#endif
