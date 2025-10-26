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
updateGeometry()
{
  if (! active_)
    return;

  updateGeometry_ = true;

  addGeometry();
}

void
CQNewGLShape::
addGeometry()
{
  if (! updateGeometry_)
    return;

  updateGeometry_ = false;

  //---

  initBuffer();

  //---

  CQNewGLCanvas::ShapeData shapeData;

  shapeData.color = color_;
  shapeData.size  = size_;

  //---

  // cone: from start -> end, with base width
  if      (type_ == Type::CONE) {
    int pos = 0;

    auto addCone = [&](const CPoint3D &p1, const CPoint3D &p2, double w) {
      FaceDatas faceDatas;
      canvas_->addCone(buffer_, p1, p2, w, shapeData, faceDatas, pos);

      for (const auto &faceData : faceDatas)
        faceDatas_.push_back(faceData);
    };

    //---

    auto p1 = start();
    auto p2 = end  ();
    auto w  = width();

    addCone(p1, p2, w);
  }
  // cube: bbox from start -> end
  else if (type_ == Type::CUBE) {
    int pos = 0;

    auto addCube = [&](const CPoint3D &p1, const CPoint3D &p2) {
      FaceDatas faceDatas;
      canvas_->addCube(buffer_, p1, p2, shapeData, faceDatas, pos);

      for (const auto &faceData : faceDatas)
        faceDatas_.push_back(faceData);
    };

    //---

    auto p1 = start();
    auto p2 = end  ();

    addCube(p1, p2);
  }
  // cylinder: from start -> end, with base width
  else if (type_ == Type::CYLINDER) {
    int pos = 0;

    auto addCylinder = [&](const CPoint3D &p1, const CPoint3D &p2, double r) {
      FaceDatas faceDatas;
      canvas_->addCylinder(buffer_, p1, p2, r, shapeData, faceDatas, pos);

      for (const auto &faceData : faceDatas)
        faceDatas_.push_back(faceData);
    };

    //---

    auto p1 = start();
    auto p2 = end();
    auto w  = width();

    addCylinder(p1, p2, w/2.0);
  }
  else if (type_ == Type::HYPERBOLOID) {
    std::cerr << "Unimplemented\n";
  }
  // pyramid: from start -> end, with base width
  else if (type_ == Type::PYRAMID) {
    int pos = 0;

    auto addPyramid = [&](const CPoint3D &p1, const CPoint3D &p2, double w) {
      FaceDatas faceDatas;
      canvas_->addPyramid(buffer_, p1, p2, w, shapeData, faceDatas, pos);

      for (const auto &faceData : faceDatas)
        faceDatas_.push_back(faceData);
    };

    //---

    auto p1 = start();
    auto p2 = end();
    auto w  = width();

    addPyramid(p1, p2, w);
  }
  // sphere: centered at start and radius from width
  else if (type_ == Type::SPHERE) {
    int pos = 0;

    auto addSphere = [&](const CPoint3D &p1, const CPoint3D &p2) {
      FaceDatas faceDatas;
      canvas_->addSphere(buffer_, p1, p2, shapeData, angleStart_, angleDelta_, faceDatas, pos);

      for (const auto &faceData : faceDatas)
        faceDatas_.push_back(faceData);
    };

    //---

    auto p1 = start();
    auto p2 = end  ();

    addSphere(p1, p2);
  }
  // torus: centered at start width orbit radius from width and default circle radius
  else if (type_ == Type::TORUS) {
    int pos = 0;

    auto addTorus = [&](const CPoint3D &p1, const CPoint3D &p2, double rfactor,
                        double power1, double power2) {
      FaceDatas faceDatas;
      canvas_->addTorus(buffer_, p1, p2, rfactor, power1, power2, shapeData, faceDatas, pos);

      for (const auto &faceData : faceDatas)
        faceDatas_.push_back(faceData);
    };

    //---

    auto p1 = start();
    auto p2 = end();

    auto rfactor = 0.25;
    auto power1  = 1.0;
    auto power2  = 1.0;

    addTorus(p1, p2, rfactor, power1, power2);
  }
  else {
    std::cerr << "Unimplemented\n";
  }

  buffer_->load();
}

void
CQNewGLShape::
drawGeometry()
{
  if (! active_)
    return;

  addGeometry();

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
