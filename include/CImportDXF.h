#ifndef CIMPORT_DXF_H
#define CIMPORT_DXF_H

#include <CImportBase.h>
#include <CGeomObject3D.h>
#include <CFile.h>
#include <CAutoPtr.h>

class CImportDXF : public CImportBase {
 private:
  typedef std::vector<CGeomVertex3D *> VertexList;

 public:
  CImportDXF(CGeomScene3D *scene=nullptr, const std::string &name="dxf");

 ~CImportDXF();

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
  bool readSection();
  bool readHeader();
  bool readTables();
  bool readBlocks();
  bool readEntities();
  bool read3DFaceEntity();
  bool readPolyLineEntity();
  bool readVertexEntity(int color);
  bool readEntity();
  bool readLine(std::string &line);
  void unreadLine(std::string line);
  void badLine(std::string line);

 private:
  CGeomScene3D            *scene_        { nullptr };
  CAutoPtr<CGeomScene3D>   pscene_;
  CGeomObject3D           *object_       { nullptr };
  CAutoPtr<CGeomObject3D>  pobject_;
  CFile                   *file_         { nullptr };
  int                      line_num_     { 0 };
  std::string              buffer_;
  bool                     buffer_valid_ { false };
};

#endif
