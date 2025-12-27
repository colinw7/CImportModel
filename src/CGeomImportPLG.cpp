#include <CGeomImportPLG.h>
#include <CGeometry3D.h>

CGeomImportPLG::
CGeomImportPLG(CGeomScene3D *scene, const std::string &name) :
 scene_(scene)
{
  if (! scene_) {
    scene_  = CGeometryInst->createScene3D();
    pscene_ = SceneP(scene_);
  }

  object_ = CGeometryInst->createObject3D(scene_, name);

  scene_->addObject(object_);

  if (! pobject_)
    pobject_ = ObjectP(object_);
}

CGeomImportPLG::
~CGeomImportPLG()
{
}

void
CGeomImportPLG::
addVertex(double x, double y, double z)
{
  object_->addVertex(CPoint3D(x, y, z));
}

void
CGeomImportPLG::
addITriangle(int i1, int i2, int i3, CDrawType3D, CShadeType3D, const CRGBA &rgba)
{
  auto *material = new CGeomMaterial;

  material->setColor(rgba);

  auto face_num = object_->addITriangle(uint(i1), uint(i2), uint(i3));

  object_->setFaceMaterial(face_num, *material);
}

void
CGeomImportPLG::
addITriangles(uint *inds, uint num_inds, CDrawType3D, CShadeType3D, const CRGBA &rgba)
{
  auto *material = new CGeomMaterial;

  material->setColor(rgba);

  std::vector<uint> faces = object_->addITriangles(inds, num_inds);

  for (uint i = 0; i < num_inds - 2; ++i)
    object_->setFaceMaterial(faces[i], *material);
}
