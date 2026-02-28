#ifndef CQCamera3DGeomObject_H
#define CQCamera3DGeomObject_H

#include <CQCamera3DFaceData.h>
#include <CQCamera3DApp.h>

#include <CGeomObject3D.h>
#include <CMathGen.h>

#include <vector>

class CQCamera3DWidget;
class CQGLBuffer;

class CQCamera3DGeomObject : public CGeomObject3D {
 public:
  using FaceDatas = std::vector<CQCamera3DFaceData>;

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

  //---

  const CBBox3D &bbox() const { return bbox_; }
  void setBBox(const CBBox3D &v) { bbox_ = v; }

  //---

  CQGLBuffer *buffer() const { return buffer_; }

  const FaceDatas &faceDatas() const { return faceDatas_; }

  CQGLBuffer *initBuffer(CQCamera3DWidget *widget);

  void addFaceData(const CQCamera3DFaceData &faceData);

  //---

  QStringList getAnimNames() const;

 private:
  CBBox3D bbox_;

  CQGLBuffer* buffer_ { nullptr };

  FaceDatas faceDatas_;
};

#endif
