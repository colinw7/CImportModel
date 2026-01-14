#include <CQNewGLBBox.h>
#include <CQNewGLShaderProgram.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLShapes.h>
#include <CQNewGLModel.h>
#include <CQNewGLUtil.h>

#include <CQGLBuffer.h>
#include <CQGLUtil.h>

CQNewGLBBox::
CQNewGLBBox(CQNewGLCanvas *canvas) :
 CQNewGLObject(canvas), canvas_(canvas)
{
}

CQNewGLShaderProgram *
CQNewGLBBox::
shaderProgram()
{
  return getShader("bbox.vs", "bbox.fs");
}

void
CQNewGLBBox::
updateGeometry()
{
  initBuffer();

  //---

  auto objects = canvas_->getAnnotationObjects();

  for (auto *object : objects) {
    if (object->isVisible())
      addBufferBBox(object);
  }

  //---

  buffer_->load();
}

void
CQNewGLBBox::
addBufferBBox(CQNewGLObject *object)
{
  auto *app = canvas_->app();

  // calc bbox
  CBBox3D bbox;
  updateBufferBBox(object, bbox);

  for (auto *child : object->getChildren())
    updateBufferBBox(child, bbox);

  //---

  auto modelMatrix = object->getTransform();

  auto bbox1 = app->transformBBox(bbox, modelMatrix);

  // add cube
  faceDataList_.clear();

  CQNewGLShapes::ShapeData shapeData;

  shapeData.color = QColorToRGBA(color_);

  CQNewGLShapes::addCube(buffer_, bbox1, shapeData, faceDataList_);
}

void
CQNewGLBBox::
updateBufferBBox(CQNewGLObject *object, CBBox3D &bbox)
{
  bbox += object->getBBox();
}

void
CQNewGLBBox::
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

  for (const auto &faceData : faceDataList_.faceDatas) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
  }

  buffer_->unbind();

  //---

  program->release();
}
