#ifndef CIMPORT_SCENE_H
#define CIMPORT_SCENE_H

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

class CImportScene {
 public:
  CImportScene(CGeomScene3D *scene=NULL, const std::string &name="scene");

 ~CImportScene() { }

  void setDebug(bool debug=true) { debug_ = debug; }

  bool read(CFile &file);

  CGeomScene3D &getScene() { return *scene_; }

  CGeomScene3D *releaseScene() {
    pscene_.release();

    return scene_;
  }

  CGeomObject3D &getObject();

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
  CGeomScene3D                           *scene_;
  CAutoPtr<CGeomScene3D>                  pscene_;
  CFile                                  *file_;
  bool                                    debug_;
  std::map<std::string, CGeomObject3D *>  objects_;
  std::map<std::string, CGeomObject3D *>  primitives_;
  std::vector<std::string>                colors_;
  std::vector<std::string>                textures_;
  int                                     orientation_;
};

#endif
