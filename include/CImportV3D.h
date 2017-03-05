#ifndef CIMPORT_V3D_H
#define CIMPORT_V3D_H

#include <CFile.h>
#include <CRGBA.h>
#include <CGeomObject3D.h>
#include <CAutoPtr.h>

class CImportV3D {
 private:
  CGeomScene3D            *scene_;
  CAutoPtr<CGeomScene3D>   pscene_;
  CGeomObject3D           *object_;
  CAutoPtr<CGeomObject3D>  pobject_;
  bool                     both_sides_;
  bool                     debug_;

 public:
  CImportV3D(CGeomScene3D *scene=NULL, const std::string &name="v3d");

 ~CImportV3D();

  void setDebug(bool debug=true) { debug_ = debug; }

  bool read(CFile &file);

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

 private:
  void getColorRGBA(long color, CRGBA *rgba);
};

#endif
