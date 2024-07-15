#ifndef CIMPORT_STL_H
#define CIMPORT_STL_H

#include <CImportBase.h>
#include <CGeomObject3D.h>
#include <CFile.h>
#include <CAutoPtr.h>

#include <set>

class CImportSTL : public CImportBase {
 private:
  typedef std::vector<CGeomVertex3D *> VertexList;

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
  CGeomScene3D            *scene_   { nullptr };
  CAutoPtr<CGeomScene3D>   pscene_;
  CGeomObject3D           *object_  { nullptr };
  CAutoPtr<CGeomObject3D>  pobject_;
  CFile                   *file_    { nullptr };

  Triangles   triangles_;

  Vector minV_;
  Vector maxV_;

  ZVals zvals_;
};

#endif
