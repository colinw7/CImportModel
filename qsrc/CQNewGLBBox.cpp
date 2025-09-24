#include <CQNewGLBBox.h>
#include <CQNewGLShaderProgram.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLShape.h>
#include <CQNewGLUtil.h>
#include <CQGLBuffer.h>
#include <CQGLUtil.h>
#include <CGeomObject3D.h>
#include <CBBox3D.h>

CQNewGLShaderProgram* CQNewGLBBox::shaderProgram_;

CQNewGLBBox::
CQNewGLBBox(CQNewGLCanvas *canvas) :
 CQNewGLObject(canvas)
{
}

void
CQNewGLBBox::
initShader(CQNewGLCanvas *canvas)
{
  shaderProgram_ = new CQNewGLShaderProgram(canvas);

  shaderProgram_->addShaders("bbox.vs", "bbox.fs");
}

void
CQNewGLBBox::
updateGeometry()
{
  initBuffer();

  //---

  auto *objectData = canvas_->getCurrentObjectData();

  if (objectData)
    addBufferBBox(objectData->modelData.buffer);

  auto *shape = canvas_->getCurrentShape();

  if (shape)
    addBufferBBox(shape->buffer());

  //---

  buffer_->load();
}

void
CQNewGLBBox::
addBufferBBox(CQGLBuffer *bboxBuffer)
{
  int np = bboxBuffer->numPoints();

  CBBox3D bbox;

  for (int ip = 0; ip < np; ++ip) {
    CQGLBuffer::PointData pointData;

    bboxBuffer->getPointData(ip, pointData);

    auto p = CPoint3D(pointData.point.x, pointData.point.y, pointData.point.z);

    bbox.add(p);
  }

  canvas_->addCube(buffer_, bbox, QColorToRGBA(color_), faceDatas_);
}

void
CQNewGLBBox::
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

  for (const auto &faceData : faceDatas_) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
  }

  buffer_->unbind();

  //---

  shaderProgram_->release();
}
