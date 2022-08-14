#ifndef CIMPORT_V3D_H
#define CIMPORT_V3D_H

#include <CImportBase.h>
#include <CGeomObject3D.h>
#include <CFile.h>
#include <CRGBA.h>
#include <CAutoPtr.h>

class CImportV3D : public CImportBase {
 public:
  CImportV3D(CGeomScene3D *scene=nullptr, const std::string &name="v3d");

 ~CImportV3D();

  bool read(CFile &file) override;

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

 private:
  void getColorRGBA(long color, CRGBA *rgba);

 private:
  CGeomScene3D            *scene_      { nullptr };
  CAutoPtr<CGeomScene3D>   pscene_;
  CGeomObject3D           *object_     { nullptr };
  CAutoPtr<CGeomObject3D>  pobject_;
  bool                     both_sides_ { false };
};

#endif
