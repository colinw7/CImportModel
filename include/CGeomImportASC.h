#ifndef CGEOM_IMPORT_ASC_H
#define CGEOM_IMPORT_ASC_H

#include <CImportASC.h>
#include <CGeomObject3D.h>
#include <CAutoPtr.h>

class CGeomImportASC : public CImportASC {
 public:
  CGeomImportASC(CGeomScene3D *scene=nullptr, const std::string &name="asc");

 ~CGeomImportASC();

  CGeomScene3D &getScene() override { return *scene_; }

  CGeomObject3D &getObject() { return *object_; }

  CGeomScene3D *releaseScene() override {
    pscene_ .release();
    pobject_.release();

    return scene_;
  }

  CGeomObject3D *releaseObject() {
    pobject_.release();

    return object_;
  }

  void addVertex(double x, double y, double z) override;

  void addITriangle(int i1, int i2, int i3, const CRGBA &rgba) override;

 private:
  CGeomScene3D            *scene_ { nullptr };
  CAutoPtr<CGeomScene3D>   pscene_;
  CGeomObject3D           *object_ { nullptr };
  CAutoPtr<CGeomObject3D>  pobject_;
};

#endif
