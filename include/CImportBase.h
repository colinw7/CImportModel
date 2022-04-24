#ifndef CImportBase_H
#define CImportBase_H

class CGeomScene3D;
class CFile;

class CImportBase {
 public:
  CImportBase() { }

  virtual ~CImportBase() { }

  bool isDebug() const { return debug_; }
  void setDebug(bool b=true) { debug_ = b; }

  virtual bool read(CFile &file) = 0;

  virtual CGeomScene3D &getScene() = 0;

 protected:
  bool debug_ { false };
};

#endif
