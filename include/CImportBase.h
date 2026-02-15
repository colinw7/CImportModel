#ifndef CImportBase_H
#define CImportBase_H

#include <CGeom3DType.h>
#include <CVector3D.h>
#include <CPoint3D.h>

#include <string>
#include <memory>
#include <map>

class CGeomScene3D;
class CGeomObject3D;
class CFile;

class CImportBase {
 public:
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
    else if (suffix == "blend") return CGEOM_3D_TYPE_BLEND;
    else if (suffix == "cob"  ) return CGEOM_3D_TYPE_COB;
    else if (suffix == "dae"  ) return CGEOM_3D_TYPE_DAE;
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

 public:
  CImportBase() { }

  virtual ~CImportBase() { }

  //---

  bool isDebug() const { return debug_; }
  void setDebug(bool b=true) { debug_ = b; }

  //---

  bool isInvertX() const { return invertX_; }
  void setInvertX(bool b) { invertX_ = b; }

  bool isInvertY() const { return invertY_; }
  void setInvertY(bool b) { invertY_ = b; }

  bool isInvertZ() const { return invertZ_; }
  void setInvertZ(bool b) { invertZ_ = b; }

  //---

  bool isSwapXY() const { return swapXY_; }
  void setSwapXY(bool b) { swapXY_ = b; }

  bool isSwapYZ() const { return swapYZ_; }
  void setSwapYZ(bool b) { swapYZ_ = b; }

  bool isSwapZX() const { return swapZX_; }
  void setSwapZX(bool b) { swapZX_ = b; }

  //---

  bool isFlipTexture() const { return flipTexture_; }
  void setFlipTexture(bool b) { flipTexture_ = b; }

  const std::string &textureDir() const { return textureDir_; }
  void setTextureDir(const std::string &s) { textureDir_ = s; }

  //---

  virtual bool read(CFile &file) = 0;

  virtual CGeomScene3D &getScene() = 0;

  virtual CGeomScene3D *releaseScene() { return nullptr; }

  bool readFileMap(const std::string &fileName);

  void addFileMap(const std::string &oldName, const std::string &newName);

  std::string remapFile(const std::string &oldName) const;

  //---

  virtual bool write(CFile *, CGeomScene3D *) const { return false; }

 protected:
  CPoint3D adjustPoint(const CPoint3D &p) const {
    auto x = p.x, y = p.y, z = p.z;

    if (isSwapXY()) std::swap(x, y);
    if (isSwapYZ()) std::swap(y, z);
    if (isSwapZX()) std::swap(z, x);

    if (isInvertX()) x = -x;
    if (isInvertY()) y = -y;
    if (isInvertZ()) z = -z;

    return CPoint3D(x, y, z);
  }

  CVector3D adjustNormal(const CVector3D &v) const {
    auto x = v.x(), y = v.y(), z = v.z();

    if (isSwapXY()) std::swap(x, y);
    if (isSwapYZ()) std::swap(y, z);
    if (isSwapZX()) std::swap(z, x);

    if (isInvertX()) x = -x;
    if (isInvertY()) y = -y;
    if (isInvertZ()) z = -z;

    return CVector3D(x, y, z);
  }

 protected:
  using SceneP  = std::unique_ptr<CGeomScene3D>;
  using ObjectP = std::unique_ptr<CGeomObject3D>;

  bool debug_ { false };

  bool invertX_ { false };
  bool invertY_ { false };
  bool invertZ_ { false };

  bool swapXY_ { false };
  bool swapYZ_ { false };
  bool swapZX_ { false };

  bool        flipTexture_ { false };
  std::string textureDir_;

  using FileNameMap = std::map<std::string, std::string>;

  FileNameMap fileNameMap_;
};

#endif
