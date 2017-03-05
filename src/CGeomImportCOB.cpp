#include <CGeomImportCOB.h>
#include <CGeometry3D.h>

CGeomImportCOB::
CGeomImportCOB(CGeomScene3D *scene, const std::string &name) :
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
CGeomImportCOB::
addVertex(double x, double y, double z)
{
  object_->addVertex(CPoint3D(x, y, z));
}

void
CGeomImportCOB::
addTexturePoint(double, double)
{
}

void
CGeomImportCOB::
addITriangle(int i1, int i2, int i3, int, int, int, int imat)
{
  int face_num = object_->addITriangle(i1, i2, i3);

  //face->setTextureIds(t1, t2, t3);

  addMaterialFace(imat, face_num);
}

void
CGeomImportCOB::
setFacesMaterial(int imat, CShadeType3D,
                 const CRGBA &ambient, const CRGBA &diffuse,
                 const CRGBA &specular, const CRGBA &emission, double shininess,
                 const std::string &)
{
  CMaterial *material =
    new CMaterial(ambient, diffuse, specular, emission, shininess);

  setFacesMaterial(imat, material);
}

void
CGeomImportCOB::
addMaterialFace(int imat, uint face_num)
{
  FaceList &face_list = getMaterialFaces(imat);

  face_list.push_back(face_num);
}

void
CGeomImportCOB::
setFacesMaterial(int imat, CMaterial *material)
{
  FaceList &face_list = getMaterialFaces(imat);

  FaceList::const_iterator p1 = face_list.begin();
  FaceList::const_iterator p2 = face_list.end  ();

  for ( ; p1 != p2; ++p1) {
    object_->setFaceMaterial(*p1, *material);
  }
}

CGeomImportCOB::FaceList &
CGeomImportCOB::
getMaterialFaces(int imat)
{
  return material_face_list_[imat];
}
