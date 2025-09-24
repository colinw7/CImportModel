#include <CQNewGLHull.h>
#include <CQNewGLShaderProgram.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLShape.h>
#include <CQGLBuffer.h>
#include <CQGLUtil.h>
#include <CGeomObject3D.h>
#include <CHull3D.h>

CQNewGLShaderProgram* CQNewGLHull::shaderProgram_;

CQNewGLHull::
CQNewGLHull(CQNewGLCanvas *canvas) :
 CQNewGLObject(canvas)
{
}

void
CQNewGLHull::
initShader(CQNewGLCanvas *canvas)
{
  shaderProgram_ = new CQNewGLShaderProgram(canvas);

  shaderProgram_->addShaders("normal.vs", "normal.fs");
}

void
CQNewGLHull::
updateGeometry()
{
  initBuffer();

  //---

  auto *objectData = canvas_->getCurrentObjectData();

  if (objectData)
    addBufferHull(objectData->modelData.buffer);

  auto *shape = canvas_->getCurrentShape();

  if (shape)
    addBufferHull(shape->buffer());

  //---

  buffer_->load();
}

void
CQNewGLHull::
addBufferHull(CQGLBuffer *hullBuffer)
{
  CHull3D hull;

  int np = hullBuffer->numPoints();

  for (int ip = 0; ip < np; ++ip) {
    CQGLBuffer::PointData pointData;

    hullBuffer->getPointData(ip, pointData);

    auto p = CPoint3D(pointData.point.x, pointData.point.y, pointData.point.z);

    hull.addVertex(p.x, p.y, p.z);
  }

  hull.calc();

  int pos = 0;

  auto addPoint = [&](const CPoint3D &p, const QColor &c) {
    buffer_->addPoint(p.x, p.y, p.z);
    buffer_->addColor(c);
  };

  for (auto pf = hull.facesBegin(); pf != hull.facesEnd(); ++pf) {
    auto *f = *pf;

    auto *v1 = f->vertex(0);
    auto *v2 = f->vertex(1);
    auto *v3 = f->vertex(2);

    addPoint(CPoint3D(v1->x(), v1->y(), v1->z()), color_);
    addPoint(CPoint3D(v2->x(), v2->y(), v2->z()), color_);
    addPoint(CPoint3D(v3->x(), v3->y(), v3->z()), color_);

    CQNewGLFaceData faceData;

    faceData.pos = pos;
    faceData.len = 3;

    faceDatas_.push_back(faceData);

    pos += faceData.len;
  }
}

void
CQNewGLHull::
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
