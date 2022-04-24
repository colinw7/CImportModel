#ifndef CIMPORT_OBJ_H
#define CIMPORT_OBJ_H

#include <CImportBase.h>
#include <CGeomObject3D.h>
#include <CFile.h>
#include <CAutoPtr.h>

class CImportObj : public CImportBase {
 public:
  CImportObj(CGeomScene3D *scene=nullptr, const std::string &name="obj");

 ~CImportObj();

  bool read(CFile &file) override;

  CGeomScene3D &getScene() override { return *scene_; }

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
  bool readVertex(const std::string &line);
  bool readTextureVertex(const std::string &line);
  bool readVertexNormal(const std::string &line);
  bool readParameterVertex(const std::string &line);
  bool readGroupName(const std::string &line);
  bool readFace(const std::string &line);

 private:
  CGeomScene3D            *scene_ { nullptr };
  CAutoPtr<CGeomScene3D>   pscene_;
  CGeomObject3D           *object_ { nullptr };
  CAutoPtr<CGeomObject3D>  pobject_;
  CFile                   *file_ { nullptr };
  int                      vnum_ { 0 }, vnnum_ { 0 }, vtnum_ { 0 };
  std::string              groupName_;
};

#endif
