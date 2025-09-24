#include <CImportVoxel.h>
#include <CGeometry3D.h>
#include <CGeomBox3D.h>
#include <CVoxel.h>
#include <CStrUtil.h>

CImportVoxel::
CImportVoxel(CGeomScene3D *scene, const std::string &name) :
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

CImportVoxel::
~CImportVoxel()
{
}

bool
CImportVoxel::
read(CFile &file)
{
  file_ = &file;

  CVoxel voxel;

  if (! voxel.readVox(file))
    return false;

  double xs = voxel.xmax() + voxel.xmin();
  double ys = voxel.ymax() + voxel.ymin();
  double zs = voxel.zmax() + voxel.zmin();

  double xc = (voxel.xmin() + voxel.xmax())/2.0;
  double yc = (voxel.ymin() + voxel.ymax())/2.0;
  double zc = (voxel.zmin() + voxel.zmax())/2.0;

  double scale = std::max(std::max(xs, ys), zs);

  double bs = 1.0/scale;

  auto addBox = [&](double x, double y, double z, const CRGBA &c) {
    std::vector<uint> faces, vertices;
    CGeomBox3D::addGeometry(object_, x, y, z, bs, bs, bs, faces, vertices);
    for (const auto &i : faces) {
      auto &face = object_->getFace(i);
      face.setColor(c);
    }
  };

  for (const auto &v : voxel.voxels()) {
    auto c = voxel.color(v.c);

    double x = (v.x - xc)/scale;
    double y = (v.y - yc)/scale;
    double z = (v.z - zc)/scale;

    addBox(x, y, z, c);
  }

  return true;
}
