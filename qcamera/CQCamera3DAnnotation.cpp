#include <CQCamera3DAnnotation.h>
#include <CQCamera3DShaderProgram.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DCamera.h>
#include <CQCamera3DShapes.h>
#include <CQCamera3DGeomObject.h>

#include <CGeomObject3D.h>
#include <CGeomSphere3D.h>
#include <CQGLBuffer.h>
#include <CQGLUtil.h>

CQCamera3DAnnotation::
CQCamera3DAnnotation(CQCamera3DCanvas *canvas) :
 CQCamera3DObject(canvas)
{
  connect(canvas, SIGNAL(cameraStateChanged()), this, SLOT(updateGeometry()));
}

CQCamera3DShaderProgram *
CQCamera3DAnnotation::
shaderProgram()
{
  return getShader("annotation.vs", "annotation.fs");
}

void
CQCamera3DAnnotation::
updateGeometry()
{
  (void) initBuffer();

  //---

  drawMoveDirection();

  drawCamera();

  //---

  buffer_->disableTexturePart();

  buffer_->load();
}

void
CQCamera3DAnnotation::
drawMoveDirection()
{
  auto bbox = canvas_->bbox();

  auto lineWidth = bbox.getMaxSize()/1000.0;

  CQCamera3DShapes::ShapeData shapeData;

  shapeData.color = CRGBA::red();

#if 0
  auto addCube = [&](const CPoint3D &c, double r) {
    auto p1 = c - CPoint3D(r, r, r);
    auto p2 = c + CPoint3D(r, r, r);

    CQCamera3DFaceDataList faceDataList;
    faceDataList.pos = faceDataList_.pos;
    CBBox3D bbox(p1, p2);
    CQCamera3DShapes::addCube(buffer_, bbox, shapeData, faceDataList);
    faceDataList_.add(faceDataList);
    faceDataList_.pos += faceDataList.pos;
  };
#endif

  auto addLine = [&](const CPoint3D &c1, const CPoint3D &c2) {
    CQCamera3DShapes::addCylinder(buffer_, c1, c2, lineWidth, shapeData, faceDataList_);
  };

  auto dir = canvas_->moveDirection();

  if (dir != CQCamera3DCanvas::MoveDirection::NONE) {
    auto *object = canvas_->currentObject();

    auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);

    CPoint3D center;

    if (object) {
      auto bbox1 = object1->bbox();

      center = bbox1.getCenter();
    }
    else
      center = canvas_->cursor();

    if      (dir == CQCamera3DCanvas::MoveDirection::X) {
      auto s = bbox.getXSize();

      addLine(CPoint3D(center.x - 2*s, center.y, center.z),
              CPoint3D(center.x + 2*s, center.y, center.z));
    }
    else if (dir == CQCamera3DCanvas::MoveDirection::Y) {
      auto s = bbox.getYSize();

      addLine(CPoint3D(center.x, center.y - 2*s, center.z),
              CPoint3D(center.x, center.y + 2*s, center.z));
    }
    else if (dir == CQCamera3DCanvas::MoveDirection::Z) {
      auto s = bbox.getZSize();

      addLine(CPoint3D(center.x, center.y, center.z - 2*s),
              CPoint3D(center.x, center.y, center.z + 2*s));
    }
  }

#if 0
  CPoint3D c(0, 0, 0);
  double   r { 0.1 };

  addCube(c, r);
#endif
}

void
CQCamera3DAnnotation::
drawCamera()
{
  if (! canvas_->isDebugCamera())
    return;

  //---

  auto *camera = canvas_->debugCamera();

  auto p1 = camera->origin().point();
  auto p2 = camera->position().point();

  auto v = CVector3D(p1, p2);
  auto r = v.length();


  //---

  CQCamera3DShapes::ShapeData shapeData;

  shapeData.color = CRGBA::white();

  CQCamera3DFaceDataList faceDataList;
  faceDataList.pos = faceDataList_.pos;
  CQCamera3DShapes::addSphere(buffer_, p1, p2, shapeData, 0.0, 2*M_PI, faceDataList);

  for (auto &faceData : faceDataList.faceDatas) {
    faceData.wireframe = true;
    faceData.lineWidth = 1;
  }

  faceDataList_.add(faceDataList);
  faceDataList_.pos = faceDataList.pos;

  //---

  auto addSpherePoint = [&](const CPoint3D &p, const CRGBA &c) {
    CQCamera3DShapes::ShapeData shapeData;

    shapeData.color = c;

    auto p1 = p + r*CVector3D(1, 1, 1)/100.0;

    CQCamera3DShapes::addSphere(buffer_, p, p1, shapeData, 0.0, 2*M_PI, faceDataList_);
  };

  addSpherePoint(p2, CRGBA::white());

  //---

//auto bbox      = canvas_->bbox();
//auto lineWidth = bbox.getMaxSize()/1000.0;

  auto addPoint = [&](const CPoint3D &p, const CVector3D &n, const CRGBA &c) {
    buffer_->addPoint(p.x, p.y, p.z);
    buffer_->addNormal(n.getX(), n.getY(), n.getZ());
    buffer_->addColor(c);
    buffer_->addTexturePoint(0, 0);
  };

  auto addLine = [&](const CPoint3D &p1, const CPoint3D &p2, const CRGBA &c) {
    addPoint(p1, CVector3D(0, 1, 0), c);
    addPoint(p2, CVector3D(0, 1, 0), c);

    CQCamera3DFaceData faceData;

    faceData.pos = faceDataList_.pos;
    faceData.len = 2;

    faceData.lines     = true;
    faceData.lineWidth = 8;

    faceDataList_.faceDatas.push_back(faceData);

    faceDataList_.pos += faceData.len;
  };

  auto pf = p1 - r*camera->front();
  auto pr = p1 + r*camera->right();
  auto pu = p1 + r*camera->up();

  addLine(p1, pf, CRGBA::red  ());
  addLine(p1, pr, CRGBA::green());
  addLine(p1, pu, CRGBA::blue ());

  addSpherePoint(pf, CRGBA::red  ());
  addSpherePoint(pr, CRGBA::green());
  addSpherePoint(pu, CRGBA::blue ());
}

void
CQCamera3DAnnotation::
drawGeometry()
{
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);

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

  program->setUniformValue("viewPos", CQGLUtil::toVector(camera->position()));

  //---

  int canvasLineWidth = canvas_->lineWidth();

  int lineWidth = -1;

  // render shapes
  buffer_->bind();

  for (const auto &faceData : faceDataList_.faceDatas) {
    int lineWidth1 = (faceData.lineWidth >= 0 ? faceData.lineWidth : canvasLineWidth);

    if (lineWidth1 != lineWidth) {
      lineWidth = lineWidth1;

      glLineWidth(lineWidth);
    }

    if      (faceData.wireframe) {
      program->setUniformValue("isWireframe", false);
      program->setUniformValue("isLine"     , true);

      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

      glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
    }
    else if (faceData.lines) {
      program->setUniformValue("isWireframe", false);
      program->setUniformValue("isLine"     , true);

      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

      glDrawArrays(GL_LINES, faceData.pos, faceData.len);
    }
    else {
      program->setUniformValue("isLine", false);

      if (isWireframe()) {
        program->setUniformValue("isWireframe", true);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
      }

      program->setUniformValue("isWireframe", false);

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

      glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
    }
  }

  buffer_->unbind();

  //---

  program->release();
}
