#ifndef CIMPORT_OBJ_H
#define CIMPORT_OBJ_H

#include <CImportBase.h>
#include <CGeomObject3D.h>
#include <CFile.h>

class CImportObj : public CImportBase {
 public:
  CImportObj(CGeomScene3D *scene=nullptr, const std::string &name="obj");

 ~CImportObj();

  bool isSplitByMaterial() const { return splitByMaterial_; }
  void setSplitByMaterial(bool b) { splitByMaterial_ = b; }

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
  struct Material;

  bool readVertex(const std::string &line);
  bool readTextureVertex(const std::string &line);
  bool readVertexNormal(const std::string &line);
  bool readParameterVertex(const std::string &line);
  bool readGroupName(const std::string &line);
  bool readFace(const std::string &line);

  bool readMaterialFile(const std::string &filename);

  Material *addMaterial(const std::string &name);

 private:
  using OptColor = std::optional<CRGBA>;
  using OptReal  = std::optional<double>;
  using OptInt   = std::optional<int>;

  struct MapImage {
    std::string name;  // empty when unset
    CImagePtr   image;
  };

  struct Material {
    std::string name;

    OptColor ambientColor; // Ka
    MapImage ambientMap;   // map_Ka

    OptColor diffuseColor; // Kd
    MapImage diffuseMap;   // map_Kd

    MapImage bumpMap; // map_Bump

    OptColor specularColor;    // Ks
    MapImage specularMap;      // map_Ks
    OptReal  specularExponent; // Ns

    OptColor emissionColor; // Ke
    MapImage emissiveMap;   // map_Ke

    OptReal  metallic;    // Pm
    MapImage metallicMap; // map_Pm

    OptReal  roughness;    // Pr
    MapImage roughnessMap; // map_Pr

    OptInt   illuminationModel;       // illum
    OptReal  refractionIndex;         // Ni
    OptReal  transparency;            // Tr
    OptColor transmissionFilterColor; // Tf
  };

  using Materials     = std::map<std::string, Material *>;
  using TexturePoints = std::vector<CPoint3D>;
  using NormalPoints  = std::vector<CPoint3D>;

  CGeomScene3D*  scene_    { nullptr };
  SceneP         pscene_;
  CGeomObject3D* object_   { nullptr };
  ObjectP        pobject_;
  Materials      materials_;
  Material*      material_ { nullptr };
  CFile*         file_     { nullptr };
  int            vnum_     { 0 };
  int            vnnum_    { 0 };
  int            vtnum_    { 0 };
  std::string    groupName_;
  TexturePoints  texturePoints_;
  NormalPoints   normalPoints_;

  bool splitByMaterial_ { true };
};

#endif
