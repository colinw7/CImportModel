#ifndef CQNewGLShaderProgram_H
#define CQNewGLShaderProgram_H

#include <QOpenGLShaderProgram>
#include <QString>

class CQNewGLModel;
class CQGLBuffer;

class CQNewGLShaderProgram : public QOpenGLShaderProgram {
 public:
  CQNewGLShaderProgram(CQNewGLModel *app);

  void addShaders(const QString &vertex, const QString &fragment);

  void addVertexShader  (const QString &name);
  void addFragmentShader(const QString &name);

  CQGLBuffer *createBuffer();

 private:
  CQNewGLModel* app_ { nullptr };
};

#endif
