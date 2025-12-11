#include <CQCamera3DShape.h>
#include <CQCamera3DShaderProgram.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DCamera.h>
#include <CQCamera3DShapes.h>

#include <CGeomObject3D.h>
#include <CGeomSphere3D.h>
#include <CGeomCylinder3D.h>
#include <CQGLBuffer.h>
#include <CQGLUtil.h>

CQCamera3DShape::
CQCamera3DShape(CQCamera3DCanvas *canvas) :
 CQCamera3DObject(canvas)
{
}

CQCamera3DShaderProgram *
CQCamera3DShape::
shaderProgram()
{
  return getShader("shape.vs", "shape.fs");
}

void
CQCamera3DShape::
updateGeometry()
{
  auto *buffer = initBuffer();

  //---

  auto bbox = canvas_->bbox();

  //---

  CQCamera3DShapes::ShapeData shapeData;

  auto addSphere = [&](const CPoint3D &c, double r) {
    auto p1 = c;
    auto p2 = c + CPoint3D(r, r, r);

    CQCamera3DFaceDataList faceDataList;
    faceDataList.pos = faceDataList_.pos;
    CQCamera3DShapes::addSphere(buffer, p1, p2, shapeData, 0.0, 2*M_PI, faceDataList);
    faceDataList_.add(faceDataList);
    faceDataList_.pos += faceDataList.pos;
  };

  auto addCube = [&](const CPoint3D &c, double r) {
    auto p1 = c - CPoint3D(r, r, r);
    auto p2 = c + CPoint3D(r, r, r);

    CQCamera3DFaceDataList faceDataList;
    faceDataList.pos = faceDataList_.pos;
    CBBox3D bbox(p1, p2);
    CQCamera3DShapes::addCube(buffer, bbox, shapeData, faceDataList);
    faceDataList_.add(faceDataList);
    faceDataList_.pos += faceDataList.pos;
  };

  auto addCylinder = [&](const CPoint3D &p1, const CPoint3D &p2, double r) {
    CQCamera3DFaceDataList faceDataList;
    faceDataList.pos = faceDataList_.pos;
    CQCamera3DShapes::addCylinder(buffer, p1, p2, r, shapeData, faceDataList);
    faceDataList_.add(faceDataList);
    faceDataList_.pos += faceDataList.pos;
  };

  //---

  if (canvas_->isShowCamera()) {
    shapeData.color = CRGBA(1, 0, 0);

    for (auto *camera : canvas_->cameras()) {
      if (! camera->isVisible())
        continue;

      auto p1 = camera->origin  ().point();
      //auto p2 = camera->position().point();

      auto s = bbox.getMaxSize()/50.0;

      addSphere(p1, s);

      //addCylinder(p1, p2, 0.02);
    }
  }

  //---

  auto *camera = canvas_->currentCamera();

  auto viewMatrix  = camera->viewMatrix();
  auto iViewMatrix = viewMatrix.inverse();

  auto addPoint = [&](const CPoint3D &p, const CVector3D &n, const CRGBA &c) {
    auto f = 0.80;
    buffer->addPoint(f*p.x, f*p.y, f*p.z);
    buffer->addNormal(n.getX(), n.getY(), n.getZ());
    buffer->addColor(c);
    buffer->addTexturePoint(0, 0);
  };

  auto addPlane = [&](const CPoint3D &p1, const CPoint3D &p2,
                      const CPoint3D &p3, const CPoint3D &p4,
                      const CVector3D &n, const CRGBA &c, bool draw=false) {
    CPoint3D vp1, vp2, vp3, vp4;
    iViewMatrix.multiplyPoint(p1, vp1);
    iViewMatrix.multiplyPoint(p2, vp2);
    iViewMatrix.multiplyPoint(p3, vp3);
    iViewMatrix.multiplyPoint(p4, vp4);

    if (draw) {
      CQCamera3DFaceData faceData;

      faceData.pos = faceDataList_.pos;
      faceData.len = 4;

      addPoint(vp1, n, c);
      addPoint(vp2, n, c);
      addPoint(vp3, n, c);
      addPoint(vp4, n, c);

      faceDataList_.faceDatas.push_back(faceData);

      faceDataList_.pos += faceData.len;
    }

    return CPlane3D(vp1, vp2, vp3);
  };

  bool showPlanes = canvas_->isShowPlanes();

  auto xyPlane = addPlane(
    CPoint3D(0, 0, 0), CPoint3D(1, 0, 0), CPoint3D(1, 1, 0), CPoint3D(0, 1, 0),
    CVector3D(0, 0, 1), CRGBA(1, 0, 0));

  leftPlane_ = addPlane(
    CPoint3D(-1, -1, -1), CPoint3D(-1, -1, 1), CPoint3D(-1, 1, 1), CPoint3D(-1, 1, -1),
    CVector3D(1, 0, 0), CRGBA(0, 1, 0, 0.1), showPlanes);
  rightPlane_ = addPlane(
    CPoint3D(1, -1, -1), CPoint3D(1, -1, 1), CPoint3D(1, 1, 1), CPoint3D(1, 1, -1),
    CVector3D(-1, 0, 0), CRGBA(0, 1, 0), showPlanes);

  topPlane_ = addPlane(
    CPoint3D(-1, 1, -1), CPoint3D(-1, 1, 1), CPoint3D(1, 1, 1), CPoint3D(1, 1, -1),
    CVector3D(0, -1, 0), CRGBA(0, 0, 1), showPlanes);
  bottomPlane_ = addPlane(
    CPoint3D(-1, -1, -1), CPoint3D(-1, -1, 1), CPoint3D(1, -1, 1), CPoint3D(1, -1, -1),
    CVector3D(0, 1, 0), CRGBA(0, 0, 1), showPlanes);

  canvas_->setXYPlane(xyPlane);

  //---

  const auto &eyeLine = canvas_->eyeLine();

  if (canvas_->isShowEyeLine() && eyeLine.isSet) {
    shapeData.color = CRGBA(0, 1, 0);

    CLine3D line(eyeLine.pv1, eyeLine.pv2);
    double t = 0.0;
    (void) xyPlane.intersectLine(line, &t);
    auto pi = line.point(t);

//  auto center = bbox.getCenter();
//  auto center = eyeLine.p2;
//  auto center = xyPlane.getPoint();
//  auto size   = bbox.getMaxSize();

//  auto p1 = pi + eyeLine.v*size;
//  auto p2 = pi - eyeLine.v*size;

    //addCylinder(p1, p2, 0.02);

    addSphere(pi, 0.04);
    addSphere(eyeLine.pv1, 0.04);
    addSphere(eyeLine.pv2, 0.04);

    //addCylinder(pi + CPoint3D(0, 0, size), pi - CPoint3D(0, 0, size), 0.02);
    addCylinder(eyeLine.pv1, pi, 0.02);
    addCylinder(eyeLine.pv2, pi, 0.02);
  }

  //---

  if (eyeLine.isSet) {
    shapeData.color = CRGBA(1, 0, 0);
    addSphere(canvas_->intersectPoint1(), 0.1);
  //std::cerr << canvas_->intersectPoint1() << "\n";

    shapeData.color = CRGBA(0, 1, 0);
    addCube(canvas_->intersectPoint2(), 0.01);
  //std::cerr << canvas_->intersectPoint2() << "\n";

    auto bbox = canvas_->bbox();
    auto size = bbox.getMaxSize();

    auto v = CVector3D(canvas_->intersectPoint1(), canvas_->intersectPoint2()).normalized();
    auto p1 = canvas_->intersectPoint1() - 10*v*size;
    auto p2 = canvas_->intersectPoint1() + 10*v*size;
    addCylinder(p1, p2, 0.02);
  }

  //---

  buffer->load();
}

void
CQCamera3DShape::
drawGeometry()
{
  glDisable(GL_CULL_FACE);

  //---

  auto *program = shaderProgram();

  program->bind();

  //---

  auto *camera = canvas_->currentCamera();

  // camera projection
  auto projectionMatrix = camera->perspectiveMatrix();
  program->setUniformValue("projection", CQGLUtil::toQMatrix(projectionMatrix));

  // camera/view transformation
  auto viewMatrix = camera->viewMatrix();
  program->setUniformValue("view", CQGLUtil::toQMatrix(viewMatrix));

  // model matrix
  auto modelMatrix = CMatrix3DH::identity();
  program->setUniformValue("model", CQGLUtil::toQMatrix(modelMatrix));

  program->setUniformValue("viewPos", CQGLUtil::toVector(camera->position()));

  //---

  // render shapes
  buffer_->bind();

  for (const auto &faceData : faceDataList_.faceDatas) {
    if (isWireframe()) {
      program->setUniformValue("isWireframe", true);

      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

      glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
    }

    program->setUniformValue("isWireframe", false);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
  }

  buffer_->unbind();

  //---

  program->release();
}
