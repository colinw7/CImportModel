#include <CQCamera3DNormals.h>
#include <CQCamera3DShaderProgram.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DObjectData.h>
#include <CQCamera3DGeomObject.h>
#include <CQCamera3DCamera.h>
#include <CQCamera3DApp.h>

#include <CGeomScene3D.h>
#include <CQGLBuffer.h>
#include <CQGLUtil.h>

CQCamera3DNormals::
CQCamera3DNormals(CQCamera3DCanvas *canvas) :
 CQCamera3DObject(canvas), canvas_(canvas)
{
}

CQCamera3DShaderProgram *
CQCamera3DNormals::
shaderProgram()
{
  return getShader("normal.vs", "normal.fs");
}

void
CQCamera3DNormals::
updateGeometry()
{
  initBuffer();

  //---

  auto *app   = canvas_->app();
  auto *scene = app->getScene();

  for (auto *object : scene->getObjects()) {
    if (! object->getVisible())
      continue;

    auto *objectData = canvas_->getObjectData(object);

    addBufferNormals(objectData);
  }

  //---

  buffer_->load();
}

void
CQCamera3DNormals::
addBufferNormals(CQCamera3DObjectData *objectData)
{
  auto *srcBuffer = objectData->buffer();
  if (! srcBuffer) return;

  auto *object = objectData->object();

  //---

  auto lineSize = this->lineSize();

  if (lineSize <= 0) {
    auto bbox = srcBuffer->getBBox();

    auto sceneScale = bbox.getMaxSize();

    lineSize = sceneScale/100.0;
  }

  auto lineColor = this->lineColor();

  //---

  auto modelMatrix = object->getHierTransform();

  //---

  if (isPointNormals()) {
    // add normal lines
    int np = srcBuffer->numPoints();

    for (int ip = 0; ip < np; ++ip) {
      CQGLBuffer::PointData pointData;
      srcBuffer->getPointData(ip, pointData);

      CPoint3D  p1;
      CVector3D n;

      if (pointData.point)
        p1 = CPoint3D(pointData.point->x, pointData.point->y, pointData.point->z);

      if (pointData.normal)
        n = CVector3D(pointData.normal->x, pointData.normal->y, pointData.normal->z);

      auto p2 = p1 + lineSize*n.normalize();

      auto pm1 = modelMatrix*p1;
      auto pm2 = modelMatrix*p2;

      buffer_->addPoint(float(pm1.x), float(pm1.y), float(pm1.z));
      buffer_->addColor(lineColor);

      buffer_->addPoint(float(pm2.x), float(pm2.y), float(pm2.z));
      buffer_->addColor(lineColor);
    }
  }

  if (isFaceNormals()) {
    const auto &faces = object->getFaces();

    for (const auto *face : faces) {
      CPoint3D p1;
      face->getMidPoint(p1);

      CVector3D n;

      if (face->getNormalSet())
        n = face->getNormal();
      else
        face->calcNormal(n);

      auto p2 = p1 + lineSize*n.normalize();

      auto pm1 = modelMatrix*p1;
      auto pm2 = modelMatrix*p2;

      buffer_->addPoint(float(pm1.x), float(pm1.y), float(pm1.z));
      buffer_->addColor(lineColor);

      buffer_->addPoint(float(pm2.x), float(pm2.y), float(pm2.z));
      buffer_->addColor(lineColor);
    }
  }

  //---

  for (auto *child : object->children()) {
    if (! child->getVisible())
      continue;

    auto *childData = canvas_->getObjectData(child);

    addBufferNormals(childData);
  }
}

void
CQCamera3DNormals::
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

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glDrawArrays(GL_LINES, 0, buffer_->numPoints());

  //---

  buffer_->unbind();

  //---

  program->release();
}
