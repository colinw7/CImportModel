#include <CGeomImportASC.h>
#include <CGeometry3D.h>

CGeomImportASC::
CGeomImportASC(CGeomScene3D *scene, const std::string &name) :
 scene_(scene)
{
  if (! scene_) {
    scene_  = CGeometryInst->createScene3D();
    pscene_ = scene_;
  }

  object_ = CGeometryInst->createObject3D(scene_, name);

  scene_->addObject(object_);

  if (! pobject_)
    pobject_ = object_;
}

CGeomImportASC::
~CGeomImportASC()
{
}

void
CGeomImportASC::
addVertex(double x, double y, double z)
{
  object_->addVertex(CPoint3D(x, y, z));
}

void
CGeomImportASC::
addITriangle(int i1, int i2, int i3, const CRGBA &rgba)
{
  auto *material = new CMaterial;

  material->setAmbient(rgba);

  uint face_num = object_->addITriangle(uint(i1), uint(i2), uint(i3));

  object_->setFaceMaterial(face_num, *material);
}
