#ifndef CQCamera3DObjectData_H
#define CQCamera3DObjectData_H

#include <CQCamera3DFaceData.h>

#include <CBBox3D.h>
#include <vector>

class CQCamera3DCanvas;

class CQGLBuffer;
class CGeomObject3D;

class CQCamera3DObjectData {
 public:
  using FaceDatas = std::vector<CQCamera3DFaceData>;

 public:
  CQCamera3DObjectData(CQCamera3DCanvas *canvas);

  int ind() const { return ind_; }
  void setInd(int i) { ind_ = i; }

  //---

  const CGeomObject3D *object() const { return object_; }
  void setObject(CGeomObject3D *o) { object_ = o; }

  //---

  const CBBox3D &bbox() const { return bbox_; }
  void setBBox(const CBBox3D &v) { bbox_ = v; }

  //---

  CQGLBuffer *buffer() const { return buffer_; }

  const FaceDatas &faceDatas() const { return faceDatas_; }

  CQGLBuffer *initBuffer();

  void addFaceData(const CQCamera3DFaceData &faceData);

 private:
  CQCamera3DCanvas* canvas_ { nullptr };
  CGeomObject3D*    object_ { nullptr };
  CQGLBuffer*       buffer_ { nullptr };

  int ind_ { 0 };

  FaceDatas faceDatas_;
  CBBox3D   bbox_;
};

#endif
