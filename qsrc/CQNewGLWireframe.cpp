#include <CQNewGLWireframe.h>
#include <CQNewGLShaderProgram.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLModel.h>

#include <CQGLBuffer.h>
#include <CQGLUtil.h>

CQNewGLWireframe::
CQNewGLWireframe(CQNewGLCanvas *canvas) :
 CQNewGLObject(canvas), canvas_(canvas)
{
}

CQNewGLShaderProgram *
CQNewGLWireframe::
shaderProgram()
{
  return canvas_->getShader("wireframe.vs", "wireframe.fs");
}

void
CQNewGLWireframe::
updateGeometry()
{
  initBuffer();

  //---

  int offset = 0;

  auto objects = canvas_->getAnnotationObjects();

  for (auto *object : objects)
    addBufferWireframe(object, offset);

  //---

  buffer_->load();
}

void
CQNewGLWireframe::
addBufferWireframe(CQNewGLObject *object, int &offset)
{
  auto *srcBuffer = object->buffer();
  if (! srcBuffer) return;

  int np = srcBuffer->numPoints();

  for (int ip = 0; ip < np; ++ip) {
    CQGLBuffer::PointData pointData;

    srcBuffer->getPointData(ip, pointData);

    auto p = CPoint3D(pointData.point .x, pointData.point .y, pointData.point .z);
    auto n = CPoint3D(pointData.normal.x, pointData.normal.y, pointData.normal.z);

    buffer_->addPoint (float(p.x), float(p.y), float(p.z));
    buffer_->addNormal(float(n.x), float(n.y), float(n.z));
  }

  int offset1 = 0;

  for (const auto &faceData : object->faceDatas()) {
    auto faceData1 = faceData;

    faceData1.pos += offset;

    faceDataList_.faceDatas.push_back(faceData1);

    offset1 = faceData.pos + faceData.len;
  }

  offset += offset1;

  for (auto *child : object->getChildren())
    addBufferWireframe(child, offset);
}

void
CQNewGLWireframe::
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

  for (const auto &faceData : faceDatas()) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
  }

  //---

  buffer_->unbind();

  //---

  program->release();
}
