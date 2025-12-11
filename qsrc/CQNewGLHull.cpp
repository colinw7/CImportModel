#include <CQNewGLHull.h>
#include <CQNewGLShaderProgram.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLModelObject.h>
#include <CQNewGLShape.h>
#include <CQNewGLModel.h>

#include <CQGLBuffer.h>
#include <CQGLUtil.h>
#include <CGeomObject3D.h>
#include <CHull3D.h>

CQNewGLHull::
CQNewGLHull(CQNewGLCanvas *canvas) :
 CQNewGLObject(canvas), canvas_(canvas)
{
}

CQNewGLShaderProgram *
CQNewGLHull::
shaderProgram()
{
  return canvas_->getShader("normal.vs", "normal.fs");
}

void
CQNewGLHull::
updateGeometry()
{
  if (! isVisible())
    return;

  //---

  initBuffer();

  //---

  auto objects = canvas_->getAnnotationObjects();

  for (auto *object : objects)
    addBufferHull(object);

  //---

  buffer_->load();
}

void
CQNewGLHull::
addBufferHull(CQNewGLObject *object)
{
  CHull3D hull;

  updateBufferHull(object, hull);

  for (auto *child : object->getChildren())
    updateBufferHull(child, hull);

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

    faceDataList_.faceDatas.push_back(faceData);

    pos += faceData.len;
  }
}

void
CQNewGLHull::
updateBufferHull(CQNewGLObject *object, CHull3D &hull)
{
  auto *srcBuffer = object->buffer();
  if (! srcBuffer) return;

  //---

  int np = srcBuffer->numPoints();

  for (int ip = 0; ip < np; ++ip) {
    CQGLBuffer::PointData pointData;

    srcBuffer->getPointData(ip, pointData);

    auto p = CPoint3D(pointData.point.x, pointData.point.y, pointData.point.z);

    hull.addVertex(p.x, p.y, p.z);
  }
}

void
CQNewGLHull::
drawGeometry()
{
  auto *program = shaderProgram();

  buffer_->bind();

  program->bind();

  //---

  // model matrix
  auto *object = canvas_->getCurrentObject();

  auto modelMatrix = CMatrix3D::identity();

  if (object)
    modelMatrix = object->getTransform();

  canvas_->addShaderMVP(program, modelMatrix);

  //---

  for (const auto &faceData : faceDataList_.faceDatas) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
  }

  buffer_->unbind();

  //---

  program->release();
}
