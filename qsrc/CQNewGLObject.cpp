#include <CQNewGLObject.h>
#include <CQNewGLShaderProgram.h>
#include <CQNewGLFont.h>
#include <CQNewGLWidget.h>
#include <CQNewGLUtil.h>

#include <CQGLBuffer.h>

CQNewGLObject::
CQNewGLObject(CQNewGLWidget *widget) :
 widget_(widget)
{
}

//---

CQNewGLModel *
CQNewGLObject::
app() const
{
  return widget_->app();
}

//---

CQNewGLShaderProgram *
CQNewGLObject::
getShader(const QString &vertex, const QString &fragment)
{
  return widget_->getShader(vertex, fragment);
}

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

CQGLBuffer *
CQNewGLObject::
initBuffer()
{
  assert(shaderProgram());

  if (! buffer_)
    buffer_ = shaderProgram()->createBuffer();

  buffer_->clearBuffers();

  faceDataList_.faceDatas.clear();

  return buffer_;
}

//---

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

//---

void
CQNewGLObject::
addFaceData(const CQNewGLFaceData &faceData)
{
  faceDataList_.faceDatas.push_back(faceData);
}

//---

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

CMatrix3D
CQNewGLObject::
getTransform() const
{
  return CMatrix3D::identity();
}

//---

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
  //checkError("drawTexts");

  for (auto *text : texts_)
    text->render(widget_);
}
