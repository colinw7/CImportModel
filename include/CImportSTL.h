#ifndef CImportSTL_H
#define CImportSTL_H

#include <CImportBase.h>
#include <CGeomObject3D.h>
#include <CFile.h>

#include <set>

class CImportSTL : public CImportBase {
 public:
  struct Vector {
    float x { 0.0 };
    float y { 0.0 };
    float z { 0.0 };
  };

  struct Triangle {
    Vector normal;
    Vector p1;
    Vector p2;
    Vector p3;
  };

  using Triangles = std::vector<Triangle>;

  using ZVals = std::set<float>;

 public:
  CImportSTL(CGeomScene3D *scene=nullptr, const std::string &name="stl");

 ~CImportSTL();

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
  bool readBinary();
  bool readAscii();

 private:
  CGeomScene3D*  scene_   { nullptr };
  SceneP         pscene_;
  CGeomObject3D* object_  { nullptr };
  ObjectP        pobject_;
  CFile*         file_    { nullptr };

  Triangles triangles_;

  Vector minV_;
  Vector maxV_;

  ZVals zvals_;
};

#endif
