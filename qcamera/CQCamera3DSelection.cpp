#include <CQCamera3DSelection.h>
#include <CQCamera3DShaderProgram.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DGeomObject.h>
#include <CQCamera3DFont.h>
#include <CQCamera3DShapes.h>
#include <CQCamera3DCamera.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DUtil.h>

#include <CQGLBuffer.h>
#include <CQGLUtil.h>
#include <CGeomObject3D.h>

CQCamera3DSelection::
CQCamera3DSelection(CQCamera3DCanvas *canvas) :
 CQCamera3DObject(canvas), canvas_(canvas)
{
}

CQCamera3DShaderProgram *
CQCamera3DSelection::
shaderProgram()
{
  return getShader("selection.vs", "selection.fs");
}

void
CQCamera3DSelection::
updateGeometry()
{
  initBuffer();

  //---

  auto *object = canvas_->currentGeomObject(/*includeRoot*/true);
  if (! object) return;

  auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);

  auto bbox = object1->bbox();

  //---

  auto lineWidth = this->lineWidth();

  if (lineWidth < 0) {
    lineWidth = bbox.getMaxSize()/250.0;
  }

  auto color = QColorToRGBA(this->color());

  //---

  auto addLine = [&](const CPoint3D &p1, const CPoint3D &p2) {
    buffer_->addPoint(float(p1.x), float(p1.y), float(p1.z));
    buffer_->addColor(color);

    buffer_->addPoint(float(p2.x), float(p2.y), float(p2.z));
    buffer_->addColor(color);
  };

  auto addPoint = [&](const CPoint3D &p) {
    buffer_->addPoint(float(p.x), float(p.y), float(p.z));
    buffer_->addColor(color);
  };

  //---

  lineIndex_ = 0;

  for (auto &lineData : canvas_->selectedLineDatas())
    addLine(lineData.p1, lineData.p2);

  //---

  vertexIndex_ = buffer_->numPoints();

  for (auto &vertexData : canvas_->selectedVertexDatas())
    addPoint(vertexData.p);

  endIndex_ = buffer_->numPoints();

  //---

  buffer_->load();
}

void
CQCamera3DSelection::
drawGeometry()
{
  auto *program = shaderProgram();

  buffer_->bind();

  program->bind();

  //---

  // camera projection
  auto *camera = canvas_->getCurrentCamera();

  auto projectionMatrix = camera->worldMatrix();
  program->setUniformValue("projection", CQGLUtil::toQMatrix(projectionMatrix));

  // camera/view transformation
  auto viewMatrix = camera->viewMatrix();
  program->setUniformValue("view", CQGLUtil::toQMatrix(viewMatrix));

  // model matrix
  auto modelMatrix = CMatrix3DH::identity();
  program->setUniformValue("model", CQGLUtil::toQMatrix(modelMatrix));

  //---

  if (vertexIndex_ > lineIndex_) {
    glLineWidth(8);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glDrawArrays(GL_LINES, lineIndex_, vertexIndex_ - lineIndex_);

    glLineWidth(1);
  }

  if (endIndex_ > vertexIndex_) {
    glPointSize(8);

    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

    glDrawArrays(GL_POINTS, vertexIndex_, endIndex_ - vertexIndex_);

    glPointSize(1);
  }

  //---

  buffer_->unbind();

  //---

  program->release();
}
