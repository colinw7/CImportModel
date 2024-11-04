#include <CImportBase.h>

#include <CGeomImportASC.h>
#include <CGeomImportCOB.h>
#include <CGeomImportPLG.h>
#include <CImport3DS.h>
#include <CImport3DRW.h>
#include <CImportDXF.h>
#include <CImportGLTF.h>
#include <CImportObj.h>
#include <CImportPly.h>
#include <CImportScene.h>
#include <CImportSTL.h>
#include <CImportV3D.h>
#include <CImportX3D.h>

CImportBase *
CImportBase::
createModel(CGeom3DType type)
{
  CImportBase *import = nullptr;

  switch (type) {
    case CGEOM_3D_TYPE_3DRW:  import = new CImport3DRW   ; break;
    case CGEOM_3D_TYPE_3DS:   import = new CImport3DS    ; break;
    case CGEOM_3D_TYPE_ASC:   import = new CGeomImportASC; break;
    case CGEOM_3D_TYPE_COB:   import = new CGeomImportCOB; break;
    case CGEOM_3D_TYPE_DXF:   import = new CImportDXF    ; break;
    case CGEOM_3D_TYPE_GLTF:  import = new CImportGLTF   ; break;
    case CGEOM_3D_TYPE_OBJ:   import = new CImportObj    ; break;
    case CGEOM_3D_TYPE_PLG:   import = new CGeomImportPLG; break;
    case CGEOM_3D_TYPE_PLY:   import = new CImportPly    ; break;
    case CGEOM_3D_TYPE_SCENE: import = new CImportScene  ; break;
    case CGEOM_3D_TYPE_STL:   import = new CImportSTL    ; break;
    case CGEOM_3D_TYPE_V3D:   import = new CImportV3D    ; break;
    case CGEOM_3D_TYPE_X3D:   import = new CImportX3D    ; break;
    default: return nullptr;
  }

  return import;
}
