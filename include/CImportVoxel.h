#ifndef CImportVoxel_H
#define CImportVoxel_H

#include <CImportBase.h>
#include <CGeomObject3D.h>
#include <CFile.h>

class CImportVoxel : public CImportBase {
 public:
  CImportVoxel(CGeomScene3D *scene=nullptr, const std::string &name="vox");

 ~CImportVoxel();

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
