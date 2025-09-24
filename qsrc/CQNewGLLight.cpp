#include <CQNewGLLight.h>
#include <CQNewGLShaderProgram.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLUtil.h>
#include <CQGLBuffer.h>
#include <CQGLUtil.h>

CQNewGLShaderProgram *CQNewGLLight::shaderProgram_;

void
CQNewGLLight::
initShader(CQNewGLCanvas *canvas)
{
  shaderProgram_ = new CQNewGLShaderProgram(canvas);
  shaderProgram_->addShaders("light.vs", "light.fs");
}

CQNewGLLight::
CQNewGLLight(CQNewGLCanvas *canvas) :
 CQNewGLObject(canvas)
{
}

void
CQNewGLLight::
addGeometry()
{
  initBuffer();

  FaceDatas faceDatas;

  canvas_->addCube(buffer(), CPoint3D(0, 0, 0), 1.0, QColorToRGBA(color()), faceDatas);

  setFaceDatas(faceDatas);

  buffer()->load();
}

void
CQNewGLLight::
drawGeometry()
{
  auto lightSize = canvas_->sceneScale()/10.0;

  auto *program = shaderProgram();

  // setup light shader
  buffer()->bind();

  auto lightPos = this->position();

  auto m1 = CMatrix3D::translation(lightPos.x(), lightPos.y(), lightPos.z());
  auto m2 = CMatrix3D::scale      (lightSize, lightSize, lightSize);
  auto lightMatrix = m1*m2;

  canvas_->addShaderMVP(program, lightMatrix);

  // draw faces
  for (const auto &faceData : faceDatas()) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

//  glDrawArrays(GL_TRIANGLE_STRIP, faceData.pos, faceData.len);
    glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
  }

  buffer()->unbind();
}

void
CQNewGLLight::
setShaderData(CQNewGLShaderProgram *program, const QString &lightName) const
{
  program->setUniformValue(toCString(lightName + ".type"), int(type()));
  program->setUniformValue(toCString(lightName + ".enabled"), isEnabled());

  program->setUniformValue(toCString(lightName + ".position"),
    CQGLUtil::toVector(position()));
  program->setUniformValue(toCString(lightName + ".direction"),
    CQGLUtil::toVector(direction()));
  program->setUniformValue(toCString(lightName + ".color"),
    CQGLUtil::toVector(QColorToVector(color())));
  program->setUniformValue(toCString(lightName + ".radius"), float(radius()));
  program->setUniformValue(toCString(lightName + ".cutoff"), float(cutoff()));
}
