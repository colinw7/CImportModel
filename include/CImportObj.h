#ifndef CIMPORT_OBJ_H
#define CIMPORT_OBJ_H

#include <CFile.h>
#include <CGeomObject3D.h>
#include <CAutoPtr.h>

class CImportObj {
 public:
  CImportObj(CGeomScene3D *scene=nullptr, const std::string &name="obj");

 ~CImportObj();

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
  bool                     debug_ { false };
  std::string              groupName_;
};

#endif
