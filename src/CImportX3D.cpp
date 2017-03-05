#include <CImportX3D.h>
#include <CGeometry3D.h>
#include <CStrUtil.h>

CImportX3D::
CImportX3D(CGeomScene3D *scene, const std::string &name) :
 scene_(scene), debug_(false)
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

bool
CImportX3D::
read(CFile &file)
{
  file_ = &file;

  std::string line;

  file_->readLine(line);

  CStrWords words = CStrUtil::toWords(line, NULL);

  int num_points = CStrUtil::toInteger(words[0].getWord());
  int num_lines  = CStrUtil::toInteger(words[1].getWord());

  for (int i = 0; i < num_points; i++) {
    file_->readLine(line);

    CStrWords words = CStrUtil::toWords(line, NULL);

    double x =  CStrUtil::toReal(words[0].getWord());
    double y = -CStrUtil::toReal(words[1].getWord());
    double z =  CStrUtil::toReal(words[2].getWord());

    object_->addVertex(CPoint3D(x, y, z));
  }

  for (int j = 0; j < num_lines; j++) {
    file_->readLine(line);

    CStrWords words = CStrUtil::toWords(line, NULL);

    int start = CStrUtil::toInteger(words[0].getWord());
    int end   = CStrUtil::toInteger(words[1].getWord());

    object_->addLine(start, end);
  }

  return true;
}
