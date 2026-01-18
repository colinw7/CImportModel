#include <CQCamera3DAnnotation.h>
#include <CQCamera3DShaderProgram.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DCamera.h>
#include <CQCamera3DShapes.h>
#include <CQCamera3DGeomObject.h>
#include <CQCamera3DObjectData.h>

#include <CGeomObject3D.h>
#include <CGeomSphere3D.h>
#include <CQGLBuffer.h>
#include <CQGLUtil.h>

CQCamera3DAnnotation::
CQCamera3DAnnotation(CQCamera3DCanvas *canvas) :
 CQCamera3DObject(canvas)
{
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
  auto *buffer = initBuffer();

  //---

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
    CQCamera3DShapes::addCube(buffer, bbox, shapeData, faceDataList);
    faceDataList_.add(faceDataList);
    faceDataList_.pos += faceDataList.pos;
  };
#endif

  auto addLine = [&](const CPoint3D &c1, const CPoint3D &c2) {
    CQCamera3DShapes::addCylinder(buffer, c1, c2, lineWidth, shapeData, faceDataList_);
  };

  //---

  auto dir = canvas_->moveDirection();

  if (dir != CQCamera3DCanvas::MoveDirection::NONE) {
    auto *object = canvas_->currentObject();

    CPoint3D center;

    if (object) {
      auto *objectData = canvas_->getObjectData(object);

      if (objectData) {
        auto bbox1 = objectData->bbox();

        center = bbox1.getCenter();
      }
    }
    else
      center = canvas_->cursor();

    if      (dir == CQCamera3DCanvas::MoveDirection::X)
      addLine(CPoint3D(bbox.getXMin(), center.y, center.z),
              CPoint3D(bbox.getXMax(), center.y, center.z));
    else if (dir == CQCamera3DCanvas::MoveDirection::Y)
      addLine(CPoint3D(center.x, bbox.getYMin(), center.x),
              CPoint3D(center.x, bbox.getYMax(), center.x));
    else if (dir == CQCamera3DCanvas::MoveDirection::Z)
      addLine(CPoint3D(center.x, center.y, bbox.getZMin()),
              CPoint3D(center.x, center.y, bbox.getZMax()));
  }

#if 0
  CPoint3D c(0, 0, 0);
  double   r { 0.1 };

  addCube(c, r);
#endif

  //---

  buffer->load();
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
