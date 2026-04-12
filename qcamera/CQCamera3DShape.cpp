#include <CQCamera3DShape.h>
#include <CQCamera3DShaderProgram.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DCamera.h>
#include <CQCamera3DLight.h>
#include <CQCamera3DShapes.h>

#include <CGeomObject3D.h>
#include <CGeomSphere3D.h>
#include <CGeomCylinder3D.h>

#include <CQGLBuffer.h>
#include <CQGLUtil.h>
#include <CQGLState.h>

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
  buffer_ = initBuffer();

  //---

  xyPlane_ = addPlane(
    CPoint3D(-1, -1, 0), CPoint3D(1, -1, 0), CPoint3D(1, 1, 0), CPoint3D(-1, 1, 0),
    CVector3D(0, 0, 1), CRGBA::red());

  canvas_->setXYPlane(xyPlane_);

  //---

  addCameras();

  //---

  addLights();

  //---

  if (canvas_->isShowPlanes())
    addPlanes();

  //---

  if (canvas_->isShowEyeLine())
    addEyeLine();

  //---

  buffer_->load();
}

void
CQCamera3DShape::
addCameras()
{
  for (auto *camera : canvas_->cameras()) {
    if (! camera->isVisible())
      continue;

    addCamera(camera);
  }
}

void
CQCamera3DShape::
addCamera(CGLCameraIFace *camera)
{
  auto *camera1 = dynamic_cast<CQCamera3DCamera *>(camera);
  if (! camera1) return;

  CQCamera3DCamera::Shape shape;
  camera1->getCameraShape(shape);

  CQCamera3DShapes::ShapeData shapeData;

  shapeData.alpha     = 0.2;
  shapeData.wireframe = true;
  shapeData.solid     = false;

  auto bbox    = canvas_->bbox();
  auto boxSize = bbox.getMaxSize();

  auto s1 = boxSize/5.0;
  auto s2 = boxSize/250.0;

//auto p1 = camera->origin  ().point();
  auto p2 = camera->position().point();

  auto p3 = p2 + camera->front()*s1;
  auto p4 = p2 + camera->up   ()*s1;
  auto p5 = p2 + camera->right()*s1;

  shapeData.num_patches = 4;

  shapeData.color = CRGBA::red();
  addCylinder(p2, p3, s2, shapeData);

  shapeData.color = CRGBA::green();
  addCylinder(p2, p4, s2, shapeData);

  shapeData.color = CRGBA::blue();
  addCylinder(p2, p5, s2, shapeData);

  shapeData.color = CRGBA(0.0, 1.0, 0.0, 0.1);

  addTriangle(shape.p11, shape.p12, shape.p22, CVector3D(0, 0, -1), shapeData);
  addTriangle(shape.p12, shape.p22, shape.p21, CVector3D(0, 0, -1), shapeData);

  addTriangle(p2, shape.p11, shape.p12, CVector3D( 0,  1, 0), shapeData);
  addTriangle(p2, shape.p12, shape.p22, CVector3D( 1,  0, 0), shapeData);
  addTriangle(p2, shape.p22, shape.p21, CVector3D( 0, -1, 0), shapeData);
  addTriangle(p2, shape.p21, shape.p11, CVector3D(-1,  0, 0), shapeData);
}

void
CQCamera3DShape::
addLights()
{
  for (auto *light : canvas_->lights()) {
    if (! light->isVisible())
      continue;

    addLight(light);
  }
}

void
CQCamera3DShape::
addLight(CQCamera3DLight *light)
{
  CQCamera3DShapes::ShapeData shapeData;

  shapeData.alpha     = 0.2;
  shapeData.wireframe = false;
  shapeData.solid     = true;

  auto bbox    = canvas_->bbox();
  auto boxSize = bbox.getMaxSize();

  auto s1 = boxSize/5.0;
  auto s2 = boxSize/250.0;

  auto p2 = light->getPosition();

  auto p3 = p2 + light->front()*s1;
  auto p4 = p2 + light->up   ()*s1;
  auto p5 = p2 + light->right()*s1;

  shapeData.num_patches = 4;

  shapeData.color = CRGBA::red();
  addCylinder(p2, p3, s2, shapeData);

  shapeData.color = CRGBA::green();
  addCylinder(p2, p4, s2, shapeData);

  shapeData.color = CRGBA::blue();
  addCylinder(p2, p5, s2, shapeData);
}

void
CQCamera3DShape::
addPlanes()
{
  leftPlane_ = addPlane(
    CPoint3D(-1, -1, -1), CPoint3D(-1, -1, 1), CPoint3D(-1, 1, 1), CPoint3D(-1, 1, -1),
    CVector3D(1, 0, 0), CRGBA(0, 1, 0, 0.1), true);
  rightPlane_ = addPlane(
    CPoint3D(1, -1, -1), CPoint3D(1, -1, 1), CPoint3D(1, 1, 1), CPoint3D(1, 1, -1),
    CVector3D(-1, 0, 0), CRGBA::green(), true);

  topPlane_ = addPlane(
    CPoint3D(-1, 1, -1), CPoint3D(-1, 1, 1), CPoint3D(1, 1, 1), CPoint3D(1, 1, -1),
    CVector3D(0, -1, 0), CRGBA::blue(), true);
  bottomPlane_ = addPlane(
    CPoint3D(-1, -1, -1), CPoint3D(-1, -1, 1), CPoint3D(1, -1, 1), CPoint3D(1, -1, -1),
    CVector3D(0, 1, 0), CRGBA::blue(), true);
}

void
CQCamera3DShape::
addEyeLine()
{
  const auto &eyeLine = canvas_->eyeLine();
  if (! eyeLine.isSet) return;

  CQCamera3DShapes::ShapeData shapeData;

  shapeData.color = CRGBA::green();

  CLine3D line(eyeLine.pv1, eyeLine.pv2);
  double t = 0.0;
  (void) xyPlane_.intersectLine(line, &t);
  auto pi = line.point(t);

//auto center = bbox.getCenter();
//auto center = eyeLine.p2;
//auto center = xyPlane_.getPoint();
//auto size   = bbox.getMaxSize();

//auto p1 = pi + eyeLine.v*size;
//auto p2 = pi - eyeLine.v*size;

  //addCylinder(p1, p2, 0.02, shapeData);

  addSphere(pi, 0.04);
  addSphere(eyeLine.pv1, 0.04);
  addSphere(eyeLine.pv2, 0.04);

  //addCylinder(pi + CPoint3D(0, 0, size), pi - CPoint3D(0, 0, size), 0.02, shapeData);
  addCylinder(eyeLine.pv1, pi, 0.02, shapeData);
  addCylinder(eyeLine.pv2, pi, 0.02, shapeData);
}

void
CQCamera3DShape::
addEyeLine1()
{
  const auto &eyeLine = canvas_->eyeLine();
  if (! eyeLine.isSet) return;

  CQCamera3DShapes::ShapeData shapeData;

  shapeData.color = CRGBA::red();
  addSphere(canvas_->intersectPoint1(), 0.1);
//std::cerr << canvas_->intersectPoint1() << "\n";

  shapeData.color = CRGBA::green();
  addCube(canvas_->intersectPoint2(), 0.01, shapeData);
//std::cerr << canvas_->intersectPoint2() << "\n";

  auto bbox = canvas_->bbox();
  auto size = bbox.getMaxSize();

  auto v = CVector3D(canvas_->intersectPoint1(), canvas_->intersectPoint2()).normalized();
  auto p1 = canvas_->intersectPoint1() - 10*v*size;
  auto p2 = canvas_->intersectPoint1() + 10*v*size;
  addCylinder(p1, p2, 0.02, shapeData);
}

void
CQCamera3DShape::
addSphere(const CPoint3D &c, double r)
{
  CQCamera3DShapes::ShapeData shapeData;

  auto p1 = c;
  auto p2 = c + CPoint3D(r, r, r);

  CQCamera3DFaceDataList faceDataList;

  faceDataList.pos = faceDataList_.pos;
  CQCamera3DShapes::addSphere(buffer_, p1, p2, shapeData, 0.0, 2*M_PI, faceDataList);

  faceDataList_.add(faceDataList);
  faceDataList_.pos += faceDataList.pos;
}

void
CQCamera3DShape::
addCube(const CPoint3D &c, double r, const CQCamera3DShapes::ShapeData &shapeData)
{
  auto p1 = c - CPoint3D(r, r, r);
  auto p2 = c + CPoint3D(r, r, r);

  CQCamera3DFaceDataList faceDataList;

  faceDataList.pos = faceDataList_.pos;
  CBBox3D bbox(p1, p2);
  CQCamera3DShapes::addCube(buffer_, bbox, shapeData, faceDataList);

  faceDataList_.add(faceDataList);
  faceDataList_.pos += faceDataList.pos;
}

void
CQCamera3DShape::
addCylinder(const CPoint3D &p1, const CPoint3D &p2, double r,
            const CQCamera3DShapes::ShapeData &shapeData)
{
  CQCamera3DFaceDataList faceDataList;

  faceDataList.pos = faceDataList_.pos;
  CQCamera3DShapes::addCylinder(buffer_, p1, p2, r, shapeData, faceDataList);

  faceDataList.setWireframe(shapeData.wireframe);
  faceDataList.setSolid(shapeData.solid);

  faceDataList_.add(faceDataList);
  faceDataList_.pos += faceDataList.pos;
}

CPlane3D
CQCamera3DShape::
addPlane(const CPoint3D &p1, const CPoint3D &p2, const CPoint3D &p3, const CPoint3D &p4,
         const CVector3D &n, const CRGBA &c, bool draw)
{
  auto addPoint = [&](const CPoint3D &p, const CVector3D &n, const CRGBA &c) {
    auto f = 0.80;
    buffer_->addPoint(f*p.x, f*p.y, f*p.z);
    buffer_->addNormal(n.getX(), n.getY(), n.getZ());
    buffer_->addColor(c);
    buffer_->addTexturePoint(0, 0);
  };

  auto *camera = canvas_->getCurrentCamera();

  auto viewMatrix  = camera->viewMatrix();
  auto iViewMatrix = viewMatrix.inverse();

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
}

void
CQCamera3DShape::
addTriangle(const CPoint3D &p1, const CPoint3D &p2, const CPoint3D &p3,
            const CVector3D &n, const ShapeData &shapeData)
{
  auto addPoint = [&](const CPoint3D &p, const CVector3D &n) {
    buffer_->addPoint(p.x, p.y, p.z);
    buffer_->addNormal(n.getX(), n.getY(), n.getZ());
    buffer_->addColor(shapeData.color);
    buffer_->addTexturePoint(0, 0);
  };

  CQCamera3DFaceData faceData;

  faceData.alpha = shapeData.color.getAlpha();

  faceData.pos = faceDataList_.pos;
  faceData.len = 3;

  faceData.wireframe = shapeData.wireframe;
  faceData.solid     = shapeData.solid;

  addPoint(p1, n);
  addPoint(p2, n);
  addPoint(p3, n);

  faceDataList_.faceDatas.push_back(faceData);

  faceDataList_.pos += faceData.len;
}

void
CQCamera3DShape::
drawGeometry()
{
  auto oldCullFace = CQGLStateInst->setCullFace(false);

  //---

  auto *program = shaderProgram();

  program->bind();

  //---

  auto *camera = canvas_->getCurrentCamera();

  // camera projection
  auto projectionMatrix = camera->worldMatrix();
  program->setUniformValue("projection", CQGLUtil::toQMatrix(projectionMatrix));

  // camera/view transformation
  auto viewMatrix = camera->viewMatrix();
  program->setUniformValue("view", CQGLUtil::toQMatrix(viewMatrix));

  // model matrix
  auto modelMatrix = CMatrix3DH::identity();
  program->setUniformValue("model", CQGLUtil::toQMatrix(modelMatrix));

  // camera pos
  program->setUniformValue("viewPos", CQGLUtil::toVector(camera->position()));

  //---

  // render shapes
  buffer_->bind();

  std::vector<CQCamera3DFaceData> transparentFaceDatas;

  auto oldBlend     = CQGLStateInst->setBlend(false);
  auto oldDepthMask = CQGLStateInst->setDepthMask(true);

  program->setUniformValue("transparency", 1.0f);

  for (const auto &faceData : faceDataList_.faceDatas) {
    if (faceData.alpha < 1.0) {
      transparentFaceDatas.push_back(faceData);
      continue;
    }

    if (! faceData.lines) {
      if (faceData.wireframe) {
        program->setUniformValue("isWireframe", true);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
      }

      if (faceData.solid) {
        program->setUniformValue("isWireframe", false);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
      }
    }
    else {
      program->setUniformValue("isWireframe", false);

      glDrawArrays(GL_LINES, faceData.pos, faceData.len);
    }
  }

  CQGLStateInst->setBlend(oldBlend);
  CQGLStateInst->setDepthMask(oldDepthMask);

  if (! transparentFaceDatas.empty()) {
    auto oldBlend     = CQGLStateInst->setBlend(true);
    auto oldDepthMask = CQGLStateInst->setDepthMask(false);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (const auto &faceData : transparentFaceDatas) {
      program->setUniformValue("transparency", float(faceData.alpha));

      if (! faceData.lines) {
        if (faceData.wireframe) {
          program->setUniformValue("isWireframe", true);

          glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

          glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
        }

        if (faceData.solid) {
          program->setUniformValue("isWireframe", false);

          glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

          glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
        }
      }
      else {
        program->setUniformValue("isWireframe", false);

        glDrawArrays(GL_LINES, faceData.pos, faceData.len);
      }
    }

    CQGLStateInst->setBlend(oldBlend);
    CQGLStateInst->setDepthMask(oldDepthMask);
  }

  //---

  buffer_->unbind();

  program->release();

  //---

  CQGLStateInst->setCullFace(oldCullFace);
}
