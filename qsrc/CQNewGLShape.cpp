#include <CQNewGLShape.h>
#include <CQNewGLShaderProgram.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLModel.h>
#include <CQNewGLLight.h>
#include <CQNewGLUtil.h>

#include <CQGLBuffer.h>
#include <CQGLUtil.h>
#include <CQGLTexture.h>
#include <CGeometry3D.h>
#include <CGeomTexture.h>

CQNewGLShaderProgram *CQNewGLShape::shaderProgram_;

void
CQNewGLShape::
initShader(CQNewGLCanvas *canvas)
{
  shaderProgram_ = new CQNewGLShaderProgram(canvas);
  shaderProgram_->addShaders("shape.vs", "shape.fs");
}

CQNewGLShape::
CQNewGLShape(CQNewGLCanvas *canvas) :
 CQNewGLObject(canvas)
{
}

void
CQNewGLShape::
setActive(bool b)
{
  active_ = b;

  updateGeometry_ = true;

  canvas_->update();
}

void
CQNewGLShape::
addGeometry()
{
  if (! updateGeometry_)
    return;

  initBuffer();

  //---

  if      (type_ == Type::CONE) {
    int pos = 0;

    auto addCone = [&](const CPoint3D &p1, const CPoint3D &p2, double w) {
      FaceDatas faceDatas;
      canvas_->addCone(buffer_, p1, p2, w, color_, faceDatas, pos);

      for (const auto &faceData : faceDatas)
        faceDatas_.push_back(faceData);
    };

    //---

    auto p1 = start();
    auto p2 = end();
    auto w  = width();

    addCone(p1, p2, w);
  }
  else if (type_ == Type::CYLINDER) {
    int pos = 0;

    auto addCylinder = [&](const CPoint3D &p1, const CPoint3D &p2, double r) {
      FaceDatas faceDatas;
      canvas_->addCylinder(buffer_, p1, p2, r, color_, faceDatas, pos);

      for (const auto &faceData : faceDatas)
        faceDatas_.push_back(faceData);
    };

    //---

    auto p1 = start();
    auto p2 = end();
    auto w  = width();

    addCylinder(p1, p2, w/2.0);
  }
  else if (type_ == Type::SPHERE) {
    int pos = 0;

    auto addSphere = [&](const CPoint3D &c, double r) {
      FaceDatas faceDatas;
      canvas_->addSphere(buffer_, c, r, color_, faceDatas, pos);

      for (const auto &faceData : faceDatas)
        faceDatas_.push_back(faceData);
    };

    //---

    auto c = start();
    auto w = width();

    addSphere(c, w/2.0);
  }
  else {
  }

  buffer_->load();
}

void
CQNewGLShape::
drawGeometry()
{
  if (! active_)
    return;

  if (updateGeometry_) {
    addGeometry();

    updateGeometry_ = false;
  }

  //---

  auto *program = CQNewGLShape::shaderProgram();

  program->bind();

  buffer_->bind();

  //---

  program->setUniformValue("viewPos", CQGLUtil::toVector(canvas_->viewPos()));

  auto modelMatrix = CMatrix3D::identity();
  canvas_->addShaderMVP(program, modelMatrix);

  //---

  canvas_->addShaderLights(program);

  //---

  canvas_->addShaderLightGlobals(program);

  program->setUniformValue("shininess", 10.0f);
  program->setUniformValue("emissionColor", CQGLUtil::toVector(QColor(255, 255, 255)));

  program->setUniformValue("isSelected", isSelected());

  //---

  program->setUniformValue("textureId", 0);

  if (texture_) {
    glActiveTexture(GL_TEXTURE0);
    texture_->bind();

    program->setUniformValue("useTexture", true);
  }
  else
    program->setUniformValue("useTexture", false);

  for (const auto &faceData : faceDatas()) {
    if (isWireframe()) {
      program->setUniformValue("isWireframe", true);

      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

      glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
    }

    if (isSolid()) {
      program->setUniformValue("isWireframe", false);

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

      glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
    }
  }

  //---

  buffer_->unbind();

  program->release();
}
