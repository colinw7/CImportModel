#include <CQNewGLShaderProgram.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLModel.h>
#include <CQGLBuffer.h>

CQNewGLShaderProgram::
CQNewGLShaderProgram(CQNewGLCanvas *canvas) :
 canvas_(canvas)
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
  auto buildDir = canvas_->app()->buildDir();

  if (! addShaderFromSourceFile(QOpenGLShader::Vertex, buildDir + "/shaders/" + name))
    std::cerr << log().toStdString() << "\n";
}

void
CQNewGLShaderProgram::
addFragmentShader(const QString &name)
{
  auto buildDir = canvas_->app()->buildDir();

  if (! addShaderFromSourceFile(QOpenGLShader::Fragment, buildDir + "/shaders/" + name))
    std::cerr << log().toStdString() << "\n";
}

CQGLBuffer *
CQNewGLShaderProgram::
getBuffer()
{
  if (! buffer_)
    buffer_ = new CQGLBuffer(this);

  return buffer_;
}
