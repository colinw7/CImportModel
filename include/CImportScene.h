#ifndef CIMPORT_SCENE_H
#define CIMPORT_SCENE_H

#include <CImportBase.h>

#include <CFile.h>
#include <CMatrix3D.h>
#include <CRGBA.h>

#include <map>
#include <optional>

class CGeomScene3D;
class CGeomObject3D;
class CGeomFace3D;
class CGeomLine3D;
class CGeomVertex3D;
class CGeomTexture;
class CGeomMaterial;

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

  bool write(CFile *file, CGeomScene3D *scene) const override;

 private:
  struct TransformData {
    CMatrix3D               matrix { CMatrix3D::identity() };
    std::optional<CPoint3D> center;
    std::optional<CPoint3D> fit;
  };

  void           readScene();
  void           addObject(const std::string &name);
  void           readPrimitive(const std::string &name);
  void           readObject(const std::string &name);
  void           readFaces(CGeomObject3D *object, int pface_num);
  void           readLines(CGeomObject3D *object, int pface_num);
  void           readVertices(CGeomObject3D *object);
  void           readNormals(CGeomObject3D *object);
  void           readTexturePoints(CGeomObject3D *object);
  void           readRotate(CGeomObject3D *object, int num_patches);
  void           readColors();
  void           readTextures();
  void           readMaterial(const std::string &name);
  void           readCSG(const std::string &name);
  TransformData  readTransformData();
  CGeomObject3D *getObject(const std::string &name);
  CGeomObject3D *getPrimitive(const std::string &name);

  CGeomMaterial *getMaterial(const std::string &name) const;

  int lookupCommand(const std::string &command, const char **commands);

  void unrecogisedCommand(const std::string &block, const std::string &command,
                          const char **commands);

  CRGBA wordToColor(const std::string &word) const;
  CRGBA getRGBA(int color) const;

  bool isSkipLine(const std::string &line) const;

  //---

  void writeMaterial(CGeomMaterial *material) const;
  void writeTexture(CGeomTexture *texture) const;
  void writeObject(CGeomObject3D *object) const;

  //---

  void errorMsg(const std::string &msg) const;

 private:
  using NameObjMap = std::map<std::string, CGeomObject3D *>;
  using Names      = std::vector<std::string>;
  using Materials  = std::vector<CGeomMaterial *>;
  using Textures   = std::vector<CGeomTexture *>;

  std::string modelDir_;

  CGeomScene3D* scene_ { nullptr };
  SceneP        pscene_;
  NameObjMap    objects_;
  NameObjMap    primitives_;
  Names         colors_;
  Textures      textures_;
  Materials     materials_;
  int           orientation_ { 1 };

  mutable CFile* file_ { nullptr };
};

#endif
