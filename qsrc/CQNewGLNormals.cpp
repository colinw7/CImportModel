#include <CQNewGLNormals.h>
#include <CQNewGLShaderProgram.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLShape.h>
#include <CQNewGLTerrain.h>
#include <CQGLBuffer.h>
#include <CQGLUtil.h>
#include <CGeomObject3D.h>

CQNewGLShaderProgram* CQNewGLNormals::shaderProgram_;

CQNewGLNormals::
CQNewGLNormals(CQNewGLCanvas *canvas) :
 CQNewGLObject(canvas)
{
}

void
CQNewGLNormals::
initShader(CQNewGLCanvas *canvas)
{
  shaderProgram_ = new CQNewGLShaderProgram(canvas);

  shaderProgram_->addShaders("normal.vs", "normal.fs");
}

void
CQNewGLNormals::
updateGeometry()
{
  initBuffer();

  //---

  auto *objectData = canvas_->getCurrentObjectData();

  if (objectData)
    addBufferNormals(objectData->modelData.buffer);

  auto *shape = canvas_->getCurrentShape();

  if (shape)
    addBufferNormals(shape->buffer());

  auto *terrain = canvas_->getTerrain();

  if (terrain)
    addBufferNormals(terrain->buffer());

  //---

  buffer_->load();
}

void
CQNewGLNormals::
addBufferNormals(CQGLBuffer *normalBuffer)
{
  auto lineSize = this->lineSize();

  if (lineSize <= 0)
    lineSize = canvas_->sceneScale()/100.0;

  auto lineColor = this->lineColor();

  //---

  int np = normalBuffer->numPoints();

  for (int ip = 0; ip < np; ++ip) {
    CQGLBuffer::PointData pointData;

    normalBuffer->getPointData(ip, pointData);

    auto n = CVector3D(pointData.normal.x, pointData.normal.y, pointData.normal.z);

    auto p1 = CPoint3D(pointData.point.x, pointData.point.y, pointData.point.z);
    auto p2 = p1 + lineSize*n.normalize();

    buffer_->addPoint(float(p1.x), float(p1.y), float(p1.z));
    buffer_->addColor(lineColor);

    buffer_->addPoint(float(p2.x), float(p2.y), float(p2.z));
    buffer_->addColor(lineColor);
  }
}

void
CQNewGLNormals::
drawGeometry()
{
  buffer_->bind();

  shaderProgram_->bind();

  //---

  // model matrix
  auto *object = canvas_->getCurrentObject();

  auto modelMatrix = CMatrix3D::identity();

  if (object)
    modelMatrix = object->getTransform();

  canvas_->addShaderMVP(shaderProgram_, modelMatrix);

  //---

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glDrawArrays(GL_LINES, 0, buffer_->numPoints());

  //---

  buffer_->unbind();

  //---

  shaderProgram_->release();
}
