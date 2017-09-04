#ifndef CIMPORT_PLG_H
#define CIMPORT_PLG_H

#include <CDrawType3D.h>
#include <CShadeType3D.h>
#include <CFile.h>
#include <CGeomObject3D.h>

class CImportPLG {
 public:
  CImportPLG();

  virtual ~CImportPLG();

  void setDebug(bool debug=true) { debug_ = debug; }

  bool read(CFile &file);

 private:
  bool readHeader();
  bool readVertices();
  bool readTriangles();
  bool readNextLine();

  CRGBA paletteLookup(int ind);
  bool  paletteLoad();

  virtual void addVertex(double x, double y, double z) = 0;

  virtual void addITriangle(int i1, int i2, int i3, CDrawType3D draw_type,
                            CShadeType3D shade_type, const CRGBA &rgba) = 0;
  virtual void addITriangles(uint *inds, uint num_inds, CDrawType3D draw_type,
                             CShadeType3D shade_type, const CRGBA &rgba) = 0;

 private:
  CFile       *file_ { nullptr };
  bool         debug_ { false };
  std::string  line_;
  std::string  name_;
  uint         num_vertices_ { 0 };
  uint         num_polygons_ { 0 };
  bool         palette_loaded_ { false };
  CRGBA       *palette_ { nullptr };
};

#endif
