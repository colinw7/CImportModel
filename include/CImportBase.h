#ifndef CImportBase_H
#define CImportBase_H

#include <CGeom3DType.h>
#include <string>
#include <memory>

class CGeomScene3D;
class CGeomObject3D;
class CFile;

class CImportBase {
 public:
  CImportBase() { }

  virtual ~CImportBase() { }

  //---

  bool isDebug() const { return debug_; }
  void setDebug(bool b=true) { debug_ = b; }

  bool isInvertX() const { return invertX_; }
  void setInvertX(bool b) { invertX_ = b; }

  bool isInvertY() const { return invertY_; }
  void setInvertY(bool b) { invertY_ = b; }

  bool isInvertZ() const { return invertZ_; }
  void setInvertZ(bool b) { invertZ_ = b; }

  //---

  virtual bool read(CFile &file) = 0;

  virtual CGeomScene3D &getScene() = 0;

  virtual CGeomScene3D *releaseScene() { return nullptr; }

  //---

  static CGeom3DType filenameToType(const std::string &filename) {
    auto pos = filename.rfind('.');
    if (pos == std::string::npos)
      return CGEOM_3D_TYPE_NONE;

    auto suffix = filename.substr(pos + 1, -1);

    for (size_t i = 0; i < suffix.size(); ++i)
      suffix[i] = char(std::tolower(suffix[i]));

    return suffixToType(suffix);
  }

  static CGeom3DType suffixToType(const std::string &suffix) {
    if      (suffix == "3ds"  ) return CGEOM_3D_TYPE_3DS;
    else if (suffix == "3drw" ) return CGEOM_3D_TYPE_3DRW;
    else if (suffix == "asc"  ) return CGEOM_3D_TYPE_ASC;
    else if (suffix == "cob"  ) return CGEOM_3D_TYPE_COB;
    else if (suffix == "dxf"  ) return CGEOM_3D_TYPE_DXF;
    else if (suffix == "fbx"  ) return CGEOM_3D_TYPE_FBX;
    else if (suffix == "glb"  ) return CGEOM_3D_TYPE_GLTF;
    else if (suffix == "gltf" ) return CGEOM_3D_TYPE_GLTF;
    else if (suffix == "obj"  ) return CGEOM_3D_TYPE_OBJ;
    else if (suffix == "plg"  ) return CGEOM_3D_TYPE_PLG;
    else if (suffix == "ply"  ) return CGEOM_3D_TYPE_PLY;
    else if (suffix == "scene") return CGEOM_3D_TYPE_SCENE;
    else if (suffix == "stl"  ) return CGEOM_3D_TYPE_STL;
    else if (suffix == "v3d"  ) return CGEOM_3D_TYPE_V3D;
    else if (suffix == "vox"  ) return CGEOM_3D_TYPE_VOXEL;
    else if (suffix == "voxel") return CGEOM_3D_TYPE_VOXEL;
    else if (suffix == "x3d"  ) return CGEOM_3D_TYPE_X3D;
    else return CGEOM_3D_TYPE_NONE;
  }

  static CImportBase *createModel(CGeom3DType type, const std::string &name="");

 protected:
  using SceneP  = std::unique_ptr<CGeomScene3D>;
  using ObjectP = std::unique_ptr<CGeomObject3D>;

  bool debug_ { false };

  bool invertX_ { false };
  bool invertY_ { false };
  bool invertZ_ { false };
};

#endif
