#ifndef CQCamera3DGeomObject_H
#define CQCamera3DGeomObject_H

#include <CGeomObject3D.h>
#include <CMathGen.h>

class CQCamera3DGeomObject : public CGeomObject3D {
 public:
  CQCamera3DGeomObject(CGeomScene3D *pscene, const std::string &name);

  CQCamera3DGeomObject(const CQCamera3DGeomObject &object);

  CQCamera3DGeomObject *dup() const override;

  //---

  CPoint3D translationValues() const;

  void setTranslationValues(const CPoint3D &p);

  //---

  CPoint3D rotationValues() const;

  void setRotationValues(const CPoint3D &p);

  CPoint3D rotationValuesDeg() const;

  void setRotationValuesDeg(const CPoint3D &p);

  //---

  CPoint3D scaleValues() const;

  void setScaleValues(const CPoint3D &p);
};

#endif
