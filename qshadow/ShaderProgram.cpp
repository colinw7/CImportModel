#include <ShaderProgram.h>
#include <App.h>

#include <CQGLBuffer.h>

namespace CQShadow3D {

ShaderProgram::
ShaderProgram(App *app, const QString &name) :
 app_(app), name_(name)
{
}

void
ShaderProgram::
addShaders(const QString &vertex, const QString &fragment)
{
  addVertexShader  (vertex  );
  addFragmentShader(fragment);

  link();
}

void
ShaderProgram::
addVertexShader(const QString &name)
{
  auto buildDir = app_->buildDir();

  if (! addShaderFromSourceFile(QOpenGLShader::Vertex, buildDir + "/shaders/" + name))
    std::cerr << log().toStdString() << "\n";
}

void
ShaderProgram::
addFragmentShader(const QString &name)
{
  auto buildDir = app_->buildDir();

  if (! addShaderFromSourceFile(QOpenGLShader::Fragment, buildDir + "/shaders/" + name))
    std::cerr << log().toStdString() << "\n";
}

void
ShaderProgram::
addGeometryShader(const QString &name)
{
  auto buildDir = app_->buildDir();

  if (! addShaderFromSourceFile(QOpenGLShader::Geometry, buildDir + "/shaders/" + name))
    std::cerr << log().toStdString() << "\n";
}

CQGLBuffer *
ShaderProgram::
createBuffer()
{
  return new CQGLBuffer(this);
}

}
