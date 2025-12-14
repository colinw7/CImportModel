#ifndef CQCamera3DGeomObject_H
#define CQCamera3DGeomObject_H

#include <CGeomObject3D.h>
#include <CMathGen.h>

class CQCamera3DGeomObject : public CGeomObject3D {
 public:
  CQCamera3DGeomObject(CGeomScene3D *pscene, const std::string &name) :
   CGeomObject3D(pscene, name) {
  }

  CQCamera3DGeomObject(const CQCamera3DGeomObject &object) :
   CGeomObject3D(object) {
  }

  CQCamera3DGeomObject *dup() const override {
    return new CQCamera3DGeomObject(*this);
  }

  //---

  CPoint3D translationValues() const {
    auto m = getTransform();
    CPoint3D p;
    m.getTranslate(p);
    return p;
  }

  void setTranslationValues(const CPoint3D &p) {
    setTranslate(CMatrix3D::translation(p.x, p.y, p.z));
  }

  //---

  CPoint3D rotationValues() const {
    auto m = getTransform();
    CPoint3D p;
    m.getRotation(p);
    return p;
  }

  void setRotationValues(const CPoint3D &p) {
    setRotate(CMatrix3D::rotation(p.x, p.y, p.z));
  }

  CPoint3D rotationValuesDeg() const {
    auto p = rotationValues();
    return CPoint3D(CMathGen::RadToDeg(p.x), CMathGen::RadToDeg(p.y), CMathGen::RadToDeg(p.z));
  }

  void setRotationValuesDeg(const CPoint3D &p) {
    setRotationValues(CPoint3D(CMathGen::DegToRad(p.x),
                               CMathGen::DegToRad(p.y),
                               CMathGen::DegToRad(p.z)));
  }

  //---

  CPoint3D scaleValues() const {
    auto m = getTransform();
    CPoint3D p;
    m.getScale(p);
    return p;
  }

  void setScaleValues(const CPoint3D &p) {
    setScale(CMatrix3D::scale(p.x, p.y, p.z));
  }
};

#endif
