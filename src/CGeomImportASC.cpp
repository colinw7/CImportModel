#include <CGeomImportASC.h>
#include <CGeometry3D.h>

CGeomImportASC::
CGeomImportASC(CGeomScene3D *scene, const std::string &name) :
 scene_(scene)
{
  if (scene_ == NULL) {
    scene_  = CGeometryInst->createScene3D();
    pscene_ = scene_;
  }

  object_ = CGeometryInst->createObject3D(scene_, name);

  scene_->addObject(object_);

  if (scene_ == NULL)
    pobject_ = object_;
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
  CMaterial *material = new CMaterial;

  material->setAmbient(rgba);

  uint face_num = object_->addITriangle(i1, i2, i3);

  object_->setFaceMaterial(face_num, *material);
}
