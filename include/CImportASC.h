#ifndef CIMPORT_ASC_H
#define CIMPORT_ASC_H

#include <CFile.h>
#include <CRGBA.h>

class CImportASC {
  enum {
    INVERT_X             = (1<<0),
    INVERT_Y             = (1<<1),
    INVERT_Z             = (1<<2),
    SWAP_XY              = (1<<3),
    SWAP_XZ              = (1<<4),
    SWAP_YZ              = (1<<5),
    INVERT_WINDING_ORDER = (1<<6)
  };

 private:
  typedef std::vector<std::string> StringList;

  CFile      *file_;
  bool        debug_;
  int         flags_;
  std::string line_;
  std::string name_;
  int         num_vertices_;
  int         num_faces_;
  std::string pattern_;
  StringList  match_strs_;

 public:
  CImportASC();

  virtual ~CImportASC();

  void setDebug(bool debug=true) { debug_ = debug; }

  bool read(CFile &file);

  virtual void addVertex(double x, double y, double z) = 0;

  virtual void addITriangle(int i1, int i2, int i3, const CRGBA &rgba) = 0;

 private:
  bool readName();
  bool readNumObjects();
  bool readVertices();
  bool readFaces();
  bool readNextLine();
};

#endif
