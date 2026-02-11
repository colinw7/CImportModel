#include <CQCamera3DGeomObject.h>
#include <CQCamera3DWidget.h>
#include <CQCamera3DShaderProgram.h>

#include <CQGLBuffer.h>

CQCamera3DGeomObject::
CQCamera3DGeomObject(CGeomScene3D *pscene, const std::string &name) :
 CGeomObject3D(pscene, name)
{
}

CQCamera3DGeomObject::
CQCamera3DGeomObject(const CQCamera3DGeomObject &object) :
 CGeomObject3D(object) {
}

CQCamera3DGeomObject *
CQCamera3DGeomObject::
dup() const
{
  return new CQCamera3DGeomObject(*this);
}

//---

CPoint3D
CQCamera3DGeomObject::
translationValues() const
{
  auto m = getTransform();
  CPoint3D p;
  m.getTranslate(p);
  return p;
}

void
CQCamera3DGeomObject::
setTranslationValues(const CPoint3D &p)
{
  setTranslate(CMatrix3D::translation(p.x, p.y, p.z));
}

//---

CPoint3D
CQCamera3DGeomObject::
rotationValues() const
{
  auto m = getTransform();
  CPoint3D p;
  m.getRotation(p);
  return p;
}

void
CQCamera3DGeomObject::
setRotationValues(const CPoint3D &p)
{
  setRotate(CMatrix3D::rotation(p.x, p.y, p.z));
}

CPoint3D
CQCamera3DGeomObject::
rotationValuesDeg() const
{
  auto p = rotationValues();
  return CPoint3D(CMathGen::RadToDeg(p.x), CMathGen::RadToDeg(p.y), CMathGen::RadToDeg(p.z));
}

void
CQCamera3DGeomObject::
setRotationValuesDeg(const CPoint3D &p)
{
  setRotationValues(CPoint3D(CMathGen::DegToRad(p.x),
                             CMathGen::DegToRad(p.y),
                             CMathGen::DegToRad(p.z)));
}

//---

CPoint3D
CQCamera3DGeomObject::
scaleValues() const
{
  auto m = getTransform();
  CPoint3D p;
  m.getScale(p);
  return p;
}

void
CQCamera3DGeomObject::
setScaleValues(const CPoint3D &p)
{
  setScale(CMatrix3D::scale(p.x, p.y, p.z));
}

//---

CQGLBuffer *
CQCamera3DGeomObject::
initBuffer(CQCamera3DWidget *widget)
{
  assert(widget->shaderProgram());

  if (! buffer_)
    buffer_ = widget->shaderProgram()->createBuffer();

  buffer_->clearBuffers();

  faceDatas_.clear();

  return buffer_;
}

void
CQCamera3DGeomObject::
addFaceData(const CQCamera3DFaceData &faceData)
{
  faceDatas_.push_back(faceData);
}
