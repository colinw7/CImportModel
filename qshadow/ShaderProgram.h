#ifndef ShaderProgram_H
#define ShaderProgram_H

#include <QOpenGLShaderProgram>
#include <QString>

class CQGLBuffer;

namespace CQShadow3D {

class App;

class ShaderProgram : public QOpenGLShaderProgram {
 public:
  ShaderProgram(App *app, const QString &name);

  const QString &id() const { return id_; }
  void setId(const QString &s) { id_ = s; }

  void addShaders(const QString &vertex, const QString &fragment);

  void addVertexShader  (const QString &name);
  void addFragmentShader(const QString &name);
  void addGeometryShader(const QString &name);

  CQGLBuffer *createBuffer();

 private:
  App*    app_ { nullptr };
  QString name_;
  QString id_;
};

}

#endif
