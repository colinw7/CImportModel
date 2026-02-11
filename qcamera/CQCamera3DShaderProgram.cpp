#include <CQCamera3DShaderProgram.h>
#include <CQCamera3DApp.h>
#include <CQGLBuffer.h>

CQCamera3DShaderProgram::
CQCamera3DShaderProgram(CQCamera3DApp *app) :
 app_(app)
{
}

void
CQCamera3DShaderProgram::
addShaders(const QString &vertex, const QString &fragment)
{
  addVertexShader  (vertex  );
  addFragmentShader(fragment);

  link();
}

void
CQCamera3DShaderProgram::
addVertexShader(const QString &name)
{
  auto buildDir = app_->buildDir();

  if (! addShaderFromSourceFile(QOpenGLShader::Vertex, buildDir + "/shaders/" + name))
    std::cerr << log().toStdString() << "\n";
}

void
CQCamera3DShaderProgram::
addFragmentShader(const QString &name)
{
  auto buildDir = app_->buildDir();

  if (! addShaderFromSourceFile(QOpenGLShader::Fragment, buildDir + "/shaders/" + name))
    std::cerr << log().toStdString() << "\n";
}

CQGLBuffer *
CQCamera3DShaderProgram::
createBuffer()
{
  return new CQGLBuffer(this);
}
