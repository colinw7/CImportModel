#ifndef CIMPORT_X3D_H
#define CIMPORT_X3D_H

#include <CImportBase.h>
#include <CGeomObject3D.h>
#include <CFile.h>

class CImportX3D : public CImportBase {
 public:
  CImportX3D(CGeomScene3D *scene=nullptr, const std::string &name="x3d");

 ~CImportX3D();

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
  CGeomScene3D*  scene_  { nullptr };
  SceneP         pscene_;
  CGeomObject3D* object_ { nullptr };
  ObjectP        pobject_;
  CFile*         file_   { nullptr };
};

#endif
