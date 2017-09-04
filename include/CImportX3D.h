#ifndef CIMPORT_X3D_H
#define CIMPORT_X3D_H

#include <CFile.h>
#include <CGeomObject3D.h>
#include <CAutoPtr.h>

class CImportX3D {
 public:
  CImportX3D(CGeomScene3D *scene=NULL, const std::string &name="x3d");

 ~CImportX3D();

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
  CGeomScene3D            *scene_ { nullptr };
  CAutoPtr<CGeomScene3D>   pscene_;
  CGeomObject3D           *object_ { nullptr };
  CAutoPtr<CGeomObject3D>  pobject_;
  CFile                   *file_ { nullptr };
  bool                     debug_ { false };
};

#endif
