#ifndef CIMPORT_3DRW_H
#define CIMPORT_3DRW_H

#include <CFile.h>
#include <CRGBA.h>
#include <CGeomObject3D.h>
#include <CAutoPtr.h>

class CImport3DRW {
 public:
  CImport3DRW(CGeomScene3D *scene=nullptr, const std::string &name="3drw");

 ~CImport3DRW();

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
  void swapInt(int *word);
  void swapShort(short *word);

 private:
  CGeomScene3D            *scene_ { nullptr };
  CAutoPtr<CGeomScene3D>   pscene_;
  CGeomObject3D           *object_ { nullptr };
  CAutoPtr<CGeomObject3D>  pobject_;
  bool                     debug_ { false };
};

#endif
