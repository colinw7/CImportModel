#ifndef CIMPORT_3DS_H
#define CIMPORT_3DS_H

#include <CFile.h>
#include <CRGBA.h>
#include <CGeomScene3D.h>
#include <CGeomObject3D.h>
#include <CAutoPtr.h>

struct CImport3DSMaterial {
  std::string name;
  CMaterial   material;
  double      transparency;
  int         shading; // 1=flat, 2=gouraud, 3=phong, 4=metal
  bool        two_sided;

  CImport3DSMaterial() :
   name(), material(), transparency(0.0), shading(0), two_sided(false) {
  }
};

struct CImport3DSChunk {
  CImport3DSChunk(CImport3DSChunk *parent1) :
   parent(parent1), id(0), len(0), left(0) {
  }

  CImport3DSChunk *parent;
  ushort           id;
  uint             len;
  int              left;
};

class CImport3DS {
 private:
  typedef std::vector<CImport3DSMaterial *> MaterialList;
  typedef std::vector<uint>                 FaceList;
  typedef std::map<uint, FaceList>          VertexFaceList;
  typedef std::map<uint, FaceList>          SmoothGroupFaceList;

  CFile                  *file_;
  CGeomScene3D           *scene_;
  CAutoPtr<CGeomScene3D>  pscene_;
  CGeomObject3D          *object_;
  CImport3DSMaterial     *material_;
  MaterialList            materials_;
  VertexFaceList          vertexFaceList_;
  SmoothGroupFaceList     smoothGroupFaceList_;
  bool                    debug_;

 public:
  CImport3DS(CGeomScene3D *scene=NULL, const std::string &name="3ds");

 ~CImport3DS() { }

  void setDebug(bool debug = true) { debug_ = debug; }

  bool read(CFile &file);

  CGeomScene3D &getScene() { return *scene_; }

  CGeomScene3D *releaseScene() {
    pscene_.release();

    return scene_;
  }

 private:
  bool        readM3DVersion      (CImport3DSChunk *chunk);
  bool        readMData           (CImport3DSChunk *chunk);
  bool        readMeshVersion     (CImport3DSChunk *chunk);
  bool        readMatEntry        (CImport3DSChunk *chunk);
  bool        readMatName         (CImport3DSChunk *chunk);
  bool        readMatAmbient      (CImport3DSChunk *chunk);
  bool        readMatDiffuse      (CImport3DSChunk *chunk);
  bool        readMatSpecular     (CImport3DSChunk *chunk);
  bool        readMatShininess    (CImport3DSChunk *chunk);
  bool        readMatTransparency (CImport3DSChunk *chunk);
  bool        readMatTwoSided     (CImport3DSChunk *chunk);
  bool        readMatShading      (CImport3DSChunk *chunk);
  bool        readColor           (CImport3DSChunk *chunk, CRGBA &color);
  bool        readIntPercentage   (CImport3DSChunk *chunk, ushort *percent);
  bool        readFloatPercentage (CImport3DSChunk *chunk, float *percent);
  bool        readMasterScale     (CImport3DSChunk *chunk);
  bool        readNamedObject     (CImport3DSChunk *chunk);
  bool        readNTriObject      (CImport3DSChunk *chunk);
  bool        readPointArray      (CImport3DSChunk *chunk);
  bool        readFaceArray       (CImport3DSChunk *chunk);
  bool        readMshMatGroup     (CImport3DSChunk *chunk);
  bool        readSmoothGroup     (CImport3DSChunk *chunk);
  bool        readMeshMatrix      (CImport3DSChunk *chunk);
  bool        readChunk           (CImport3DSChunk *chunk);
  bool        skipChunk           (CImport3DSChunk *chunk);
  bool        readChar            (CImport3DSChunk *chunk, uchar *c);
  bool        readShort           (CImport3DSChunk *chunk, ushort *s);
  bool        readLong            (CImport3DSChunk *chunk, uint *l);
  std::string readString          (CImport3DSChunk *chunk);
  bool        readFloat           (CImport3DSChunk *chunk, float *f);
  std::string getChunkName        (CImport3DSChunk *chunk);
  void        adjustChunkLeft     (CImport3DSChunk *chunk, int offset);
  void        printChunk          (CImport3DSChunk *chunk);
  uint        getChunkDepth       (CImport3DSChunk *chunk);
  std::string getChunkPad         (CImport3DSChunk *chunk);
};

#endif
