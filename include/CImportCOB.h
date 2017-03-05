#ifndef CIMPORT_COB_H
#define CIMPORT_COB_H

#include <CFile.h>
#include <CPoint3D.h>
#include <CMatrix3D.h>
#include <CShadeType3D.h>
#include <CRGBA.h>

class CImportCOB {
 public:
  enum {
    INVERT_X             = (1<<0),
    INVERT_Y             = (1<<1),
    INVERT_Z             = (1<<2),
    SWAP_XY              = (1<<3),
    SWAP_XZ              = (1<<4),
    SWAP_YZ              = (1<<5),
    INVERT_WINDING_ORDER = (1<<6),
    TRANSFORM_LOCAL      = (1<<7),
    TRANSFORM_WORLD      = (1<<8)
  };

 private:
  CFile*                   file_;
  bool                     debug_;
  uint                     flags_;
  std::string              line_;
  std::string              name_;
  int                      num_vertices_;
  int                      num_texture_vertices_;
  int                      num_faces_;
  CPoint3D                 center_;
  CPoint3D                 x_axis_;
  CPoint3D                 y_axis_;
  CPoint3D                 z_axis_;
  CMatrix3D                local_matrix_;
  CMatrix3D                world_matrix_;
  std::string              pattern_;
  std::vector<std::string> match_strs_;

 public:
  CImportCOB();

  virtual ~CImportCOB();

  void setDebug(bool debug=true) { debug_ = debug; }

  void addFlag(uint flag) { flags_ |= flag; }

  bool read(CFile &file);

  virtual void addVertex(double x, double y, double z) = 0;

  virtual void addTexturePoint(double x, double y) = 0;

  virtual void addITriangle(int i1, int i2, int i3, int t1, int t2, int t3, int imat) = 0;

  virtual void setFacesMaterial(int imat, CShadeType3D shade_type, const CRGBA &ambient,
                                const CRGBA &diffuse, const CRGBA &specular, const CRGBA &emission,
                                double shininess, const std::string &fileName) = 0;

 private:
  bool readName();
  bool readCoordinates();
  bool readTransform();
  bool readVertices();
  bool readTextureVertices();
  bool readFaces();
  bool readMaterials();
  bool readMaterial(int i);

  bool readNextLine();
};

#endif
