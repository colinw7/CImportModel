#include <CQNewGLShape.h>
#include <CQNewGLShaderProgram.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLModel.h>
#include <CQNewGLLight.h>
#include <CQNewGLShapes.h>
#include <CQNewGLUtil.h>

#include <CQGLBuffer.h>
#include <CQGLUtil.h>
#include <CQGLTexture.h>
#include <CGeometry3D.h>
#include <CGeomTexture.h>

CQNewGLShape::
CQNewGLShape(CQNewGLCanvas *canvas) :
 CQNewGLObject(canvas), canvas_(canvas)
{
}

CQNewGLShaderProgram *
CQNewGLShape::
shaderProgram()
{
  return getShader("shape.vs", "shape.fs");
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

  CQNewGLShapes::ShapeData shapeData;

  shapeData.color = color_;
  shapeData.size  = size_;

  //---

  // cone: from start -> end, with base width
  if      (type_ == Type::CONE) {
    auto addCone = [&](const CPoint3D &p1, const CPoint3D &p2, double w) {
      CQNewGLFaceDataList faceDataList;
      CQNewGLShapes::addCone(buffer_, p1, p2, w, shapeData, faceDataList);
      faceDataList_.add(faceDataList);
    };

    //---

    auto p1 = start();
    auto p2 = end  ();
    auto w  = width();

    addCone(p1, p2, w);
  }
  // cube: bbox from start -> end
  else if (type_ == Type::CUBE) {
    auto addCube = [&](const CPoint3D &p1, const CPoint3D &p2) {
      CQNewGLFaceDataList faceDataList;
      CQNewGLShapes::addCube(buffer_, p1, p2, shapeData, faceDataList);
      faceDataList_.add(faceDataList);
    };

    //---

    auto p1 = start();
    auto p2 = end  ();

    addCube(p1, p2);
  }
  // cylinder: from start -> end, with base width
  else if (type_ == Type::CYLINDER) {
    auto addCylinder = [&](const CPoint3D &p1, const CPoint3D &p2, double r) {
      CQNewGLFaceDataList faceDataList;
      CQNewGLShapes::addCylinder(buffer_, p1, p2, r, shapeData, faceDataList);
      faceDataList_.add(faceDataList);
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
    auto addPyramid = [&](const CPoint3D &p1, const CPoint3D &p2, double w) {
      CQNewGLFaceDataList faceDataList;
      CQNewGLShapes::addPyramid(buffer_, p1, p2, w, shapeData, faceDataList);
      faceDataList_.add(faceDataList);
    };

    //---

    auto p1 = start();
    auto p2 = end();
    auto w  = width();

    addPyramid(p1, p2, w);
  }
  // sphere: centered at start and radius from width
  else if (type_ == Type::SPHERE) {
    auto addSphere = [&](const CPoint3D &p1, const CPoint3D &p2) {
      CQNewGLFaceDataList faceDataList;
      CQNewGLShapes::addSphere(buffer_, p1, p2, shapeData, angleStart_, angleDelta_, faceDataList);
      faceDataList_.add(faceDataList);
    };

    //---

    auto p1 = start();
    auto p2 = end  ();

    addSphere(p1, p2);
  }
  // torus: centered at start width orbit radius from width and default circle radius
  else if (type_ == Type::TORUS) {
    auto addTorus = [&](const CPoint3D &p1, const CPoint3D &p2, double rfactor,
                        double power1, double power2) {
      CQNewGLFaceDataList faceDataList;
      CQNewGLShapes::addTorus(buffer_, p1, p2, rfactor, power1, power2, shapeData, faceDataList);
      faceDataList_.add(faceDataList);
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
