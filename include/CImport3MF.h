#ifndef CImport3MF_H
#define CImport3MF_H

#include <CImagePtr.h>
#include <CImportBase.h>
#include <CGeomObject3D.h>
#include <CFile.h>

#include <set>

class CXMLTag;

class CImport3MF : public CImportBase {
 public:
  CImport3MF(CGeomScene3D *scene=nullptr, const std::string &name="3mf");

 ~CImport3MF();

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
  struct FileData {
    std::map<std::string, std::string> files;
  };

 private:
  bool readModel(const std::string &);
  bool readResources(CXMLTag *);
  bool readBuild(CXMLTag *);
  bool readObject(CXMLTag *);
  bool readMesh(CXMLTag *);
  bool readVertices(CXMLTag *);
  bool readTriangles(CXMLTag *);
  bool readComponents(CXMLTag *);
  bool readComponent(CXMLTag *);
  bool readBaseMaterials(CXMLTag *);
  bool readBaseMaterial(CXMLTag *);
  bool readColorGroup(CXMLTag *);
  bool readColor(CXMLTag *, CRGBA &c);
  bool readTexture2D(CXMLTag *);
  bool readTexture2DGroup(CXMLTag *);
  bool readMultiProperties(CXMLTag *);
  bool readMulti(CXMLTag *);
  bool readMetallicDisplayProperties(CXMLTag *);
  bool readMetallic(CXMLTag *);
  bool readItem(CXMLTag *);

  bool extractFiles(const std::string &path);

 private:
  enum class ResourceType {
    COLOR_GROUP,
    TEXTURE,
    TEXTURE_GROUP
  };

  struct Resource {
    ResourceType rtype;
    int          id { -1 };
  };

  struct ColorGroup : Resource {
    std::vector<CRGBA> colors;
  };

  struct Texture : Resource {
    std::string path;
    std::string type;
    CImagePtr   image;
  };

  struct TextureGroup : Resource {
    std::vector<CPoint2D> points;
    CGeomTexture*         geomTexture { nullptr };
  };

 private:
  Resource *getResource(int id) const;

 private:
  using IdResource = std::map<int, Resource *>;
  using IdObject   = std::map<int, CGeomObject3D *>;

  CGeomScene3D*  scene_   { nullptr };
  SceneP         pscene_;
  CGeomObject3D* object_  { nullptr };
  ObjectP        pobject_;

  FileData fileData_;

  CGeomObject3D* currentObject_ { nullptr };
  uint           objectCount_   { 0 };

  IdResource idResource_;
  IdObject   idObject_;
};

#endif
