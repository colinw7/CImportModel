#ifndef CIMPORT_SCENE_H
#define CIMPORT_SCENE_H

#include <CImportBase.h>
#include <CFile.h>
#include <CMatrix3D.h>
#include <CRGBA.h>
#include <CAutoPtr.h>
#include <map>

class CGeomScene3D;
class CGeomObject3D;
class CGeomFace3D;
class CGeomLine3D;
class CGeomVertex3D;

class CImportScene : public CImportBase {
 public:
  CImportScene(CGeomScene3D *scene=nullptr, const std::string &name="scene");

 ~CImportScene();

  bool read(CFile &file) override;

  CGeomScene3D &getScene() override { return *scene_; }

  CGeomObject3D &getObject();

  CGeomScene3D *releaseScene() override {
    pscene_.release();

    return scene_;
  }

 private:
  void           readScene();
  void           addObject(const std::string &name);
  void           readPrimitive(const std::string &name);
  void           readObject(const std::string &name);
  void           readFaces(CGeomObject3D *object, int pface_num);
  void           readLines(CGeomObject3D *object, int pface_num);
  void           readVertices(CGeomObject3D *object);
  void           readRotate(CGeomObject3D *object, int num_patches);
  void           readColors();
  void           readTextures();
  CMatrix3D      readTransforms();
  CGeomObject3D *getObject(const std::string &name);
  CGeomObject3D *getPrimitive(const std::string &name);
  int            lookupCommand(const std::string &command, const char **commands);
  void           getRGBA(int color, CRGBA *rgba);

 private:
  using NameObjMap = std::map<std::string, CGeomObject3D *>;
  using Names      = std::vector<std::string>;

  CGeomScene3D*          scene_ { nullptr };
  CAutoPtr<CGeomScene3D> pscene_;
  CFile*                 file_  { nullptr };
  NameObjMap             objects_;
  NameObjMap             primitives_;
  Names                  colors_;
  Names                  textures_;
  int                    orientation_ { 1 };
};

#endif
