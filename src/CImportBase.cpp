#include <CImportBase.h>

#include <CGeomImportASC.h>
#include <CGeomImportCOB.h>
#include <CGeomImportPLG.h>

#include <CImport3DRW.h>
#include <CImport3DS.h>
#include <CImportBlend.h>
#include <CImportDAE.h>
#include <CImportDXF.h>
#include <CImportFBX.h>
#include <CImportGLTF.h>
#include <CImportObj.h>
#include <CImportPly.h>
#include <CImportScene.h>
#include <CImportSTL.h>
#include <CImportV3D.h>
#include <CImportVoxel.h>
#include <CImportX3D.h>

#include <CStrParse.h>
#include <CStrUtil.h>

CImportBase *
CImportBase::
createModel(CGeom3DType type, const std::string &name)
{
  CImportBase *base = nullptr;

  switch (type) {
    case CGEOM_3D_TYPE_3DRW:  base = new CImport3DRW     (nullptr, name); break;
    case CGEOM_3D_TYPE_3DS:   base = new CImport3DS      (nullptr, name); break;
    case CGEOM_3D_TYPE_ASC:   base = new CGeomImportASC  (nullptr, name); break;
    case CGEOM_3D_TYPE_BLEND: base = new CImportBlend    (nullptr, name); break;
    case CGEOM_3D_TYPE_COB:   base = new CGeomImportCOB  (nullptr, name); break;
    case CGEOM_3D_TYPE_DAE:   base = new CImportDAE      (nullptr, name); break;
    case CGEOM_3D_TYPE_DXF:   base = new CImportDXF      (nullptr, name); break;
    case CGEOM_3D_TYPE_FBX:   base = new CImportFBX      (nullptr, name); break;
    case CGEOM_3D_TYPE_GLTF:  base = new CImportGLTF     (nullptr, name); break;
    case CGEOM_3D_TYPE_OBJ:   base = new CImportObj      (nullptr, name); break;
    case CGEOM_3D_TYPE_PLG:   base = new CGeomImportPLG  (nullptr, name); break;
    case CGEOM_3D_TYPE_PLY:   base = new CImportPly      (nullptr, name); break;
    case CGEOM_3D_TYPE_SCENE: base = new CImportScene    (nullptr, name); break;
    case CGEOM_3D_TYPE_STL:   base = new CImportSTL      (nullptr, name); break;
    case CGEOM_3D_TYPE_V3D:   base = new CImportV3D      (nullptr, name); break;
    case CGEOM_3D_TYPE_VOXEL: base = new CImportVoxel    (nullptr, name); break;
    case CGEOM_3D_TYPE_X3D:   base = new CImportX3D      (nullptr, name); break;
    default: return nullptr;
  }

  return base;
}

bool
CImportBase::
readFileMap(const std::string &fileName)
{
  CFile file(fileName);
  if (! file.exists())
    return false;

  while (! file.eof()) {
    std::string line;
    file.readLine(line);

    CStrParse parse(line);

    parse.skipSpace();

    if (parse.eof())
      continue;

    auto readString = [&]() {
      std::string str;

      if (parse.isChar('"')) {
        parse.skipChar();

        while (! parse.eof() && ! parse.isChar('"'))
          str += parse.readChar();

        if (parse.isChar('"'))
          parse.skipChar();
      }
      else {
        while (! parse.eof() && ! parse.isSpace())
          str += parse.readChar();
      }

      return str;
    };

    auto fromStr = readString();

    while (! parse.eof() && parse.isSpace())
      parse.skipChar();

    auto toStr = readString();

    //---

    addFileMap(fromStr, toStr);
  }

  return true;
}

void
CImportBase::
addFileMap(const std::string &oldName, const std::string &newName)
{
  fileNameMap_[oldName] = newName;
}

std::string
CImportBase::
remapFile(const std::string &oldName) const
{
  auto pf = fileNameMap_.find(oldName);

  if (pf != fileNameMap_.end())
    return (*pf).second;

  return oldName;
}
