#include <CQNewGLObject.h>
#include <CQNewGLShaderProgram.h>
#include <CQNewGLFont.h>
#include <CQGLBuffer.h>

CQNewGLObject::
CQNewGLObject(CQNewGLCanvas *canvas) :
 canvas_(canvas)
{
}

//---

void
CQNewGLObject::
bindShader()
{
  shaderProgram()->bind();
}

void
CQNewGLObject::
unbindShader()
{
  shaderProgram()->release();
}

//---

void
CQNewGLObject::
initBuffer()
{
  assert(shaderProgram());

  if (! buffer_)
    buffer_ = shaderProgram()->createBuffer();

  buffer_->clearBuffers();

  faceDatas_.clear();
}

void
CQNewGLObject::
setHierSolid(bool b)
{
  setSolid(b);

  for (auto *child : getChildren())
    child->setHierSolid(b);
}

void
CQNewGLObject::
setHierWireframe(bool b)
{
  setWireframe(b);

  for (auto *child : getChildren())
    child->setHierWireframe(b);
}

void
CQNewGLObject::
addFaceData(const CQNewGLFaceData &faceData)
{
  faceDatas_.push_back(faceData);
}

CBBox3D
CQNewGLObject::
getBBox() const
{
  CBBox3D bbox;

  if (! buffer_)
    return bbox;

  int np = buffer_->numPoints();

  for (int ip = 0; ip < np; ++ip) {
    CQGLBuffer::PointData pointData;

    buffer_->getPointData(ip, pointData);

    auto p = CPoint3D(pointData.point.x, pointData.point.y, pointData.point.z);

    bbox.add(p);
  }

  return bbox;
}

void
CQNewGLObject::
clearTexts()
{
  for (auto *text : texts_)
    delete text;

  texts_.clear();
}

void
CQNewGLObject::
addText(CQNewGLText *text)
{
  texts_.push_back(text);
}

void
CQNewGLObject::
updateTexts()
{
  for (auto *text : texts_)
    text->updateText();
}

void
CQNewGLObject::
drawTexts()
{
  for (auto *text : texts_)
    text->render();
}
