#ifndef CGEOM_IMPORT_ASC_H
#define CGEOM_IMPORT_ASC_H

#include <CAutoPtr.h>
#include <CImportASC.h>
#include <CGeomObject3D.h>

class CGeomImportASC : public CImportASC {
 private:
  CGeomScene3D            *scene_;
  CAutoPtr<CGeomScene3D>   pscene_;
  CGeomObject3D           *object_;
  CAutoPtr<CGeomObject3D>  pobject_;

 public:
  CGeomImportASC(CGeomScene3D *scene=NULL, const std::string &name="asc");

 ~CGeomImportASC() { }

  CGeomObject3D &getObject() { return *object_; }

  CGeomScene3D *releaseScene() {
    pscene_ .release();
    pobject_.release();

    return scene_;
  }

  CGeomObject3D *releaseObject() {
    pobject_.release();

    return object_;
  }

  void addVertex(double x, double y, double z);

  void addITriangle(int i1, int i2, int i3, const CRGBA &rgba);
};

#endif
