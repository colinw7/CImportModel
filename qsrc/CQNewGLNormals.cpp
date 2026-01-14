#include <CQNewGLNormals.h>
#include <CQNewGLShaderProgram.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLModel.h>

#include <CQGLBuffer.h>
#include <CQGLUtil.h>

CQNewGLNormals::
CQNewGLNormals(CQNewGLCanvas *canvas) :
 CQNewGLObject(canvas), canvas_(canvas)
{
}

CQNewGLShaderProgram *
CQNewGLNormals::
shaderProgram()
{
  return getShader("normal.vs", "normal.fs");
}

void
CQNewGLNormals::
updateGeometry()
{
  initBuffer();

  //---

  auto objects = canvas_->getAnnotationObjects();

  for (auto *object : objects) {
    if (object->isVisible())
      addBufferNormals(object);
  }

  //---

  buffer_->load();
}

void
CQNewGLNormals::
addBufferNormals(CQNewGLObject *object)
{
  auto *srcBuffer = object->buffer();
  if (! srcBuffer) return;

  //---

  auto lineSize = this->lineSize();

  if (lineSize <= 0) {
    auto bbox = object->getBBox();

    auto sceneScale = bbox.getMaxSize();

    lineSize = sceneScale/100.0;
  }

  auto lineColor = this->lineColor();

  //---

  auto modelMatrix = object->getTransform();

  //---

  // add normal lines
  int np = srcBuffer->numPoints();

  for (int ip = 0; ip < np; ++ip) {
    CQGLBuffer::PointData pointData;

    srcBuffer->getPointData(ip, pointData);

    auto n = CVector3D(pointData.normal.x, pointData.normal.y, pointData.normal.z);

    auto p1 = CPoint3D(pointData.point.x, pointData.point.y, pointData.point.z);
    auto p2 = p1 + lineSize*n.normalize();

    auto pm1 = modelMatrix*p1;
    auto pm2 = modelMatrix*p2;

    buffer_->addPoint(float(pm1.x), float(pm1.y), float(pm1.z));
    buffer_->addColor(lineColor);

    buffer_->addPoint(float(pm2.x), float(pm2.y), float(pm2.z));
    buffer_->addColor(lineColor);
  }

  for (auto *child : object->getChildren())
    addBufferNormals(child);
}

void
CQNewGLNormals::
drawGeometry()
{
  auto *program = shaderProgram();

  buffer_->bind();

  program->bind();

  //---

  // model matrix
  auto modelMatrix = CMatrix3D::identity();
  canvas_->addShaderMVP(program, modelMatrix);

  //---

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glDrawArrays(GL_LINES, 0, buffer_->numPoints());

  //---

  buffer_->unbind();

  //---

  program->release();
}
