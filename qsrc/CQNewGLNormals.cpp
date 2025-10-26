#include <CQNewGLNormals.h>
#include <CQNewGLShaderProgram.h>
#include <CQNewGLCanvas.h>
#include <CQGLBuffer.h>
#include <CQGLUtil.h>

CQNewGLShaderProgram* CQNewGLNormals::shaderProgram_;

void
CQNewGLNormals::
initShader(CQNewGLCanvas *canvas)
{
  shaderProgram_ = new CQNewGLShaderProgram(canvas);

  shaderProgram_->addShaders("normal.vs", "normal.fs");
}

CQNewGLNormals::
CQNewGLNormals(CQNewGLCanvas *canvas) :
 CQNewGLObject(canvas)
{
}

void
CQNewGLNormals::
updateGeometry()
{
  if (! shaderProgram_)
    initShader(canvas_);

  //---

  initBuffer();

  //---

  auto objects = canvas_->getAnnotationObjects();

  for (auto *object : objects)
    addBufferNormals(object);

  //---

  buffer_->load();
}

void
CQNewGLNormals::
addBufferNormals(CQNewGLObject *object)
{
  auto lineSize = this->lineSize();

  if (lineSize <= 0)
    lineSize = canvas_->sceneScale()/100.0;

  auto lineColor = this->lineColor();

  //---

  auto *srcBuffer = object->buffer();
  if (! srcBuffer) return;

  int np = srcBuffer->numPoints();

  for (int ip = 0; ip < np; ++ip) {
    CQGLBuffer::PointData pointData;

    srcBuffer->getPointData(ip, pointData);

    auto n = CVector3D(pointData.normal.x, pointData.normal.y, pointData.normal.z);

    auto p1 = CPoint3D(pointData.point.x, pointData.point.y, pointData.point.z);
    auto p2 = p1 + lineSize*n.normalize();

    buffer_->addPoint(float(p1.x), float(p1.y), float(p1.z));
    buffer_->addColor(lineColor);

    buffer_->addPoint(float(p2.x), float(p2.y), float(p2.z));
    buffer_->addColor(lineColor);
  }

  for (auto *child : object->getChildren())
    addBufferNormals(child);
}

void
CQNewGLNormals::
drawGeometry()
{
  buffer_->bind();

  shaderProgram_->bind();

  //---

  // model matrix
  auto modelMatrix = CMatrix3D::identity();
  canvas_->addShaderMVP(shaderProgram_, modelMatrix);

  //---

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glDrawArrays(GL_LINES, 0, buffer_->numPoints());

  //---

  buffer_->unbind();

  //---

  shaderProgram_->release();
}
