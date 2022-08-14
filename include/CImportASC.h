#ifndef CIMPORT_ASC_H
#define CIMPORT_ASC_H

#include <CImportBase.h>
#include <CFile.h>
#include <CRGBA.h>

class CImportASC : public CImportBase {
  enum {
    INVERT_X             = (1<<0),
    INVERT_Y             = (1<<1),
    INVERT_Z             = (1<<2),
    SWAP_XY              = (1<<3),
    SWAP_XZ              = (1<<4),
    SWAP_YZ              = (1<<5),
    INVERT_WINDING_ORDER = (1<<6)
  };

 public:
  CImportASC();

  virtual ~CImportASC();

  bool read(CFile &file) override;

  virtual void addVertex(double x, double y, double z) = 0;

  virtual void addITriangle(int i1, int i2, int i3, const CRGBA &rgba) = 0;

 private:
  bool readName();
  bool readNumObjects();
  bool readVertices();
  bool readFaces();
  bool readNextLine();

 private:
  typedef std::vector<std::string> StringList;

  CFile      *file_  { nullptr };
  int         flags_ { 0 };
  std::string line_;
  std::string name_;
  int         num_vertices_ { 0 };
  int         num_faces_ { 0 };
  std::string pattern_;
  StringList  match_strs_;
};

#endif
