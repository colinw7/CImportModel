#include <CQNewGLBBox.h>
#include <CQNewGLShaderProgram.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLUtil.h>
#include <CQGLBuffer.h>
#include <CQGLUtil.h>

CQNewGLShaderProgram* CQNewGLBBox::shaderProgram_;

void
CQNewGLBBox::
initShader(CQNewGLCanvas *canvas)
{
  shaderProgram_ = new CQNewGLShaderProgram(canvas);

  shaderProgram_->addShaders("bbox.vs", "bbox.fs");
}

CQNewGLBBox::
CQNewGLBBox(CQNewGLCanvas *canvas) :
 CQNewGLObject(canvas)
{
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
  CBBox3D bbox;

  updateBufferBBox(object, bbox);

  for (auto *child : object->getChildren())
    updateBufferBBox(child, bbox);

  int pos = 0;

  CQNewGLCanvas::ShapeData shapeData;

  shapeData.color = QColorToRGBA(color_);

  canvas_->addCube(buffer_, bbox, shapeData, faceDatas_, pos);
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
  buffer_->bind();

  shaderProgram_->bind();

  //---

  // model matrix
  auto modelMatrix = CMatrix3D::identity();
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
