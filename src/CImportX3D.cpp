#include <CImportX3D.h>
#include <CGeometry3D.h>
#include <CStrUtil.h>

CImportX3D::
CImportX3D(CGeomScene3D *scene, const std::string &name) :
 scene_(scene), debug_(false)
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

CImportX3D::
~CImportX3D()
{
}

bool
CImportX3D::
read(CFile &file)
{
  file_ = &file;

  std::string line;

  file_->readLine(line);

  CStrWords words = CStrUtil::toWords(line, nullptr);

  int num_points = CStrUtil::toInteger(words[0].getWord());
  int num_lines  = CStrUtil::toInteger(words[1].getWord());

  for (int i = 0; i < num_points; i++) {
    file_->readLine(line);

    CStrWords words1 = CStrUtil::toWords(line, nullptr);

    double x =  CStrUtil::toReal(words1[0].getWord());
    double y = -CStrUtil::toReal(words1[1].getWord());
    double z =  CStrUtil::toReal(words1[2].getWord());

    object_->addVertex(CPoint3D(x, y, z));
  }

  for (int j = 0; j < num_lines; j++) {
    file_->readLine(line);

    CStrWords words1 = CStrUtil::toWords(line, nullptr);

    int start = CStrUtil::toInteger(words1[0].getWord());
    int end   = CStrUtil::toInteger(words1[1].getWord());

    object_->addLine(start, end);
  }

  return true;
}
