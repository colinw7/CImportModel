#ifndef CIMPORT_Ply_H
#define CIMPORT_Ply_H

#include <CImportBase.h>
#include <CGeomObject3D.h>
#include <CFile.h>
#include <CAutoPtr.h>

#include <set>

class CImportPly : public CImportBase {
 public:
  enum class FormatType {
    NONE,
    CHAR,
    UCHAR,
    SHORT,
    USHORT,
    INT,
    UINT,
    FLOAT,
    DOUBLE
  };

 public:
  static bool stringToFormat(const std::string &format, FormatType &formatType);

  CImportPly(CGeomScene3D *scene=nullptr, const std::string &name="ply");

 ~CImportPly();

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
  struct Vertex {
    double x  { 0.0 };
    double y  { 0.0 };
    double z  { 0.0 };
    double r  { 1.0 };
    double g  { 1.0 };
    double b  { 1.0 };
    double a  { 0.0 };
    double nx { 0.0 };
    double ny { 0.0 };
    double nz { 0.0 };
    double s  { 0.0 };
    double t  { 0.0 };
  };

  struct Face {
    std::vector<int> inds;
  };

  using Vertices = std::vector<Vertex>;
  using Faces    = std::vector<Face>;

  CGeomScene3D            *scene_   { nullptr };
  CAutoPtr<CGeomScene3D>   pscene_;
  CGeomObject3D           *object_  { nullptr };
  CAutoPtr<CGeomObject3D>  pobject_;
  CFile                   *file_    { nullptr };

  Vertices vertices_;
  Faces    faces_;
};

#endif
