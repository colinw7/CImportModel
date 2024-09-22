#ifndef CImportBase_H
#define CImportBase_H

#include <CGeom3DType.h>
#include <string>

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

  virtual CGeomScene3D *releaseScene() { return nullptr; }

  //---

  static CGeom3DType suffixToType(const std::string &suffix) {
    if      (suffix == "3ds"  ) return CGEOM_3D_TYPE_3DS;
    else if (suffix == "3drw" ) return CGEOM_3D_TYPE_3DRW;
    else if (suffix == "asc"  ) return CGEOM_3D_TYPE_ASC;
    else if (suffix == "cob"  ) return CGEOM_3D_TYPE_COB;
    else if (suffix == "dxf"  ) return CGEOM_3D_TYPE_DXF;
    else if (suffix == "obj"  ) return CGEOM_3D_TYPE_OBJ;
    else if (suffix == "plg"  ) return CGEOM_3D_TYPE_PLG;
    else if (suffix == "ply"  ) return CGEOM_3D_TYPE_PLY;
    else if (suffix == "scene") return CGEOM_3D_TYPE_SCENE;
    else if (suffix == "stl"  ) return CGEOM_3D_TYPE_STL;
    else if (suffix == "v3d"  ) return CGEOM_3D_TYPE_V3D;
    else if (suffix == "x3d"  ) return CGEOM_3D_TYPE_X3D;
    else return CGEOM_3D_TYPE_NONE;
  }

  static CImportBase *createModel(CGeom3DType type);

 protected:
  bool debug_ { false };
};

#endif
