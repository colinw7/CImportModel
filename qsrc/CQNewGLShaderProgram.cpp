#include <CQNewGLShaderProgram.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLModel.h>

#include <CQGLBuffer.h>

CQNewGLShaderProgram::
CQNewGLShaderProgram(CQNewGLModel *app) :
 app_(app)
{
}

void
CQNewGLShaderProgram::
addShaders(const QString &vertex, const QString &fragment)
{
  addVertexShader  (vertex  );
  addFragmentShader(fragment);

  link();
}

void
CQNewGLShaderProgram::
addVertexShader(const QString &name)
{
  auto buildDir = app_->buildDir();

  if (! addShaderFromSourceFile(QOpenGLShader::Vertex, buildDir + "/shaders/" + name))
    std::cerr << log().toStdString() << "\n";
}

void
CQNewGLShaderProgram::
addFragmentShader(const QString &name)
{
  auto buildDir = app_->buildDir();

  if (! addShaderFromSourceFile(QOpenGLShader::Fragment, buildDir + "/shaders/" + name))
    std::cerr << log().toStdString() << "\n";
}

CQGLBuffer *
CQNewGLShaderProgram::
createBuffer()
{
  return new CQGLBuffer(this);
}
