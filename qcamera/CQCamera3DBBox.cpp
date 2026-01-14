#include <CQCamera3DBBox.h>
#include <CQCamera3DShaderProgram.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DObjectData.h>
#include <CQCamera3DGeomObject.h>
#include <CQCamera3DCamera.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DShapes.h>
#include <CQCamera3DUtil.h>

#include <CGeomScene3D.h>
#include <CQGLBuffer.h>
#include <CQGLUtil.h>
#include <COrientedBBox3D.h>

CQCamera3DBBox::
CQCamera3DBBox(CQCamera3DCanvas *canvas) :
 CQCamera3DObject(canvas), canvas_(canvas)
{
}

CQCamera3DShaderProgram *
CQCamera3DBBox::
shaderProgram()
{
  return getShader("bbox.vs", "bbox.fs");
}

void
CQCamera3DBBox::
updateGeometry()
{
  initBuffer();

  //---

  faceDataList_.clear();

  auto *app   = canvas_->app();
  auto *scene = app->getScene();

  for (auto *object : scene->getObjects()) {
    if (! object->getVisible())
      continue;

    if (isOriented())
      addBufferOrientBBox(object);
    else
      addBufferBBox(object);
  }

  //---

  buffer_->load();
}

void
CQCamera3DBBox::
addBufferOrientBBox(CGeomObject3D *object)
{
  auto *objectData = canvas_->getObjectData(object);

  auto *srcBuffer = objectData->buffer();
  if (! srcBuffer) return;

  //---

  std::vector<CVector3D> points;

  int np = srcBuffer->numPoints();
  if (np <= 0) return;

  for (int ip = 0; ip < np; ++ip) {
    CQGLBuffer::PointData pointData;

    srcBuffer->getPointData(ip, pointData);

    auto p = CPoint3D(pointData.point->x, pointData.point->y, pointData.point->z);

    points.push_back(CVector3D(p.x, p.y, p.z));
  }

  auto bbox = COrientedBBox3D::calcOrientBox(points);

  auto c = bbox.center().point();

  auto xv = bbox.extent(0)*bbox.axis(0);
  auto yv = bbox.extent(1)*bbox.axis(1);
  auto zv = bbox.extent(2)*bbox.axis(2);

//auto v  = xv + yv + zv;
//auto v1 = v.normalized();

  //---

  // add cube
#if 0
  auto calcRotationMatrix = [](const CVector3D &v1, const CVector3D &v2) {
    auto q = CQuaternion::rotationArc(v1, v2);

    CMatrix3D m;
    q.toRotationMatrix(m);

    return m;
  };
#endif

  CQCamera3DShapes::ShapeData shapeData;

  shapeData.color = QColorToRGBA(color_);

  CQCamera3DShapes::addOrientBox(buffer_, c, xv, yv, zv, shapeData, faceDataList_);

#if 0
  auto mt = CMatrix3D::translation(c.x, c.y, c.z);
  auto mr = CMatrix3D::identity();
  auto ms = CMatrix3D::scale(bbox.extent(0), bbox.extent(1), bbox.extent(2));

  CQCamera3DShapes::addCube(buffer_, mt, mr, ms, shapeData, faceDataList_);
#endif

  //---

#if 0
  double r = 0.1;

  shapeData = CQCamera3DShapes::ShapeData();

  shapeData.color = CRGBA::red();
  CQCamera3DShapes::addCylinder(buffer_, c, c + bbox.extent(0)*bbox.axis(0),
                                r, shapeData, faceDataList_);
  shapeData.color = CRGBA::green();
  CQCamera3DShapes::addCylinder(buffer_, c, c + bbox.extent(1)*bbox.axis(1),
                                r, shapeData, faceDataList_);
  shapeData.color = CRGBA::blue();
  CQCamera3DShapes::addCylinder(buffer_, c, c + bbox.extent(2)*bbox.axis(2),
                                r, shapeData, faceDataList_);
  shapeData.color = CRGBA::cyan();
  CQCamera3DShapes::addCylinder(buffer_, c, c + xv + yv + zv,
                                r, shapeData, faceDataList_);
#endif
}

void
CQCamera3DBBox::
addBufferBBox(CGeomObject3D *object)
{
  auto *app = canvas_->app();

  //---

  // calc bbox
  CBBox3D bbox;
  object->getModelBBox(bbox);

  for (auto *child : object->children()) {
    if (! child->getVisible())
      continue;

    CBBox3D bbox1;
    child->getModelBBox(bbox1);

    bbox += bbox1;
  }

  //---

  auto modelMatrix = object->getHierTransform();

  auto bbox1 = app->transformBBox(bbox, modelMatrix);

  //---

  // add cube
  CQCamera3DShapes::ShapeData shapeData;

  shapeData.color = QColorToRGBA(color_);

  CQCamera3DShapes::addCube(buffer_, bbox1, shapeData, faceDataList_);
}

void
CQCamera3DBBox::
drawGeometry()
{
  auto *program = shaderProgram();

  buffer_->bind();

  program->bind();

  //---

  // model matrix
  auto *camera = canvas_->getCurrentCamera();

  auto projectionMatrix = camera->worldMatrix();
  program->setUniformValue("projection", CQGLUtil::toQMatrix(projectionMatrix));

  // camera/view transformation
  auto viewMatrix = camera->viewMatrix();
  program->setUniformValue("view", CQGLUtil::toQMatrix(viewMatrix));

  auto modelMatrix = CMatrix3D::identity(); // already transformed
  program->setUniformValue("model", CQGLUtil::toQMatrix(modelMatrix));

  //---

  for (const auto &faceData : faceDataList_.faceDatas) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
  }

  //---

  buffer_->unbind();

  //---

  program->release();
}
