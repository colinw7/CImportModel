#ifndef CIMPORT_3DRW_H
#define CIMPORT_3DRW_H

#include <CImportBase.h>
#include <CGeomObject3D.h>

#include <CFile.h>
#include <CRGBA.h>

class CImport3DRW : public CImportBase {
 public:
  CImport3DRW(CGeomScene3D *scene=nullptr, const std::string &name="3drw");

 ~CImport3DRW();

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
  void swapInt(int *word);
  void swapShort(short *word);

 private:
  CGeomScene3D*  scene_ { nullptr };
  SceneP         pscene_;
  CGeomObject3D* object_ { nullptr };
  ObjectP        pobject_;
};

#endif
