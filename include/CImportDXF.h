#ifndef CIMPORT_DXF_H
#define CIMPORT_DXF_H

#include <CFile.h>
#include <CGeomObject3D.h>
#include <CAutoPtr.h>

class CImportDXF {
 private:
  typedef std::vector<CGeomVertex3D *> VertexList;

  CGeomScene3D            *scene_;
  CAutoPtr<CGeomScene3D>   pscene_;
  CGeomObject3D           *object_;
  CAutoPtr<CGeomObject3D>  pobject_;
  CFile                   *file_;
  bool                     debug_;
  int                      line_num_;
  std::string              buffer_;
  bool                     buffer_valid_;

 public:
  CImportDXF(CGeomScene3D *scene=NULL, const std::string &name="dxf");

 ~CImportDXF() { }

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
};

#endif
