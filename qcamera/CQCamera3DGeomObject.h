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
   CGeomObject3D(object),
   translation_(object.translation_),
   rotation_   (object.rotation_   ),
   scale_      (object.scale_      ) {
  }

  CQCamera3DGeomObject *dup() const override {
    return new CQCamera3DGeomObject(*this);
  }

  const CPoint3D &translation() const { return translation_; }
  void setTranslation(const CPoint3D &v) { translation_ = v; invalid_ = true; }

  const CPoint3D &rotation() const { return rotation_; }
  void setRotation(const CPoint3D &v) { rotation_ = v; invalid_ = true; }

  CPoint3D rotationAngles() const {
    return CPoint3D(CMathGen::RadToDeg(rotation_.x),
                    CMathGen::RadToDeg(rotation_.y),
                    CMathGen::RadToDeg(rotation_.z));
  }

  void setRotationAngles(const CPoint3D &v) {
    setRotation(CPoint3D(CMathGen::DegToRad(v.x),
                         CMathGen::DegToRad(v.y),
                         CMathGen::DegToRad(v.z)));
  }

  const CPoint3D &scale() const { return scale_; }
  void setScale(const CPoint3D &v) { scale_ = v; invalid_ = true; }

  CMatrix3D calcTransform() {
    if (invalid_) {
      auto m1 = CMatrix3D::translation(translation_.x, translation_.y, translation_.z);
      auto m2 = CMatrix3D::rotation   (rotation_   .x, rotation_   .y, rotation_   .z);
      auto m3 = CMatrix3D::scale      (scale_      .x, scale_      .y, scale_      .z);

      setTransform(m1*m2*m3);

      invalid_ = false;
    }

    return getHierTransform();
  }

 private:
  CPoint3D translation_ { 0, 0, 0 };
  CPoint3D rotation_    { 0, 0, 0 };
  CPoint3D scale_       { 1, 1, 1 };
  bool     invalid_     { false };
};

#endif
