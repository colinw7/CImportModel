#ifndef CIMPORT_Blend_H
#define CIMPORT_Blend_H

#include <CImportBase.h>
#include <CFile.h>
#include <CPoint3D.h>
#include <CMatrix3D.h>
#include <CShadeType3D.h>
#include <CRGBA.h>

class CImportBlend : public CImportBase {
 public:
  CImportBlend(CGeomScene3D *scene=nullptr, const std::string &name="blend");

 ~CImportBlend() override;

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
  struct NameData {
    bool              isPtr { false};
    std::vector<uint> dims;
    uint              dimLen;
    std::string       name;
  };

  struct Name {
    std::string name;
    NameData    data;
  };

  struct Type {
    std::string name;
    ushort      len { 0 };
  };

  struct StructVar {
    uint typeInd;
    uint nameInd;
  };

  struct Struct {
    uint typeInd;

    std::vector<StructVar> vars;
  };

  struct MPoly {
    int   loopstart;
    int   totloop;
    short mat_nr;
    char  flag;
  };

  struct MFace {
    int  v1;
    int  v2;
    int  v3;
    int  v4;
    int  mat_nr;
    char flag;
  };

  struct MEdge {
    int   v1;
    int   v2;
    char  crease;
    char  bweight;
    short flag;
  };

  struct MLoop {
    int v;
    int e;
  };

  struct MLoopUV {
    float uv[2];
    int   flag;
  };

  struct Mesh {
    CPoint3D loc;
    CPoint3D size;

    std::vector<CPoint3D> points;
    std::vector<MPoly>    mpoly;
    std::vector<MFace>    mface;
    std::vector<MEdge>    medge;
    std::vector<MLoop>    mloop;
    std::vector<MLoopUV>  mloopUV;
  };

 private:
  bool readBlock(char *code);
  bool processBlock(char *code);

  bool readSDNA(const uchar *buffer, uint size);

  void printData(const Name &n, const Type &t, uchar *data) const;

  void decodeNameData(Name &type) const;

  uint decodeInteger(const uchar *data) const;
  ushort decodeShort(const uchar *data) const;
  float decodeFloat(const uchar *data) const;

 private:
  CGeomScene3D*  scene_   { nullptr };
  SceneP         pscene_;
  CGeomObject3D* object_  { nullptr };
  ObjectP        pobject_;

  CFile* file_ { nullptr };

  uint pointerSize_  { 0 };
  bool littleEndian_ { true };

  uchar *buffer_     { nullptr };
  uint   bufferSize_ { 0 };

  char *name_    { nullptr };
  uint  nameLen_ { 0 };

  std::vector<Name>    names_;
  std::vector<Type>    types_;
  std::vector<Struct>  structs_;
  std::map<uint, uint> typeStruct_;

  Mesh*               mesh_ { nullptr };
  std::vector<Mesh *> meshes_;
};

#endif
