#include <CImportX3D.h>
#include <CGeometry3D.h>
#include <CStrUtil.h>
#include <CThrow.h>

CImportX3D::
CImportX3D(CGeomScene3D *scene, const std::string &name) :
 scene_(scene)
{
  if (! scene_) {
    scene_  = CGeometry3DInst->createScene3D();
    pscene_ = SceneP(scene_);
  }

  object_ = CGeometry3DInst->createObject3D(scene_, name);

  scene_->addObject(object_);

  if (! pobject_)
    pobject_ = ObjectP(object_);
}

CImportX3D::
~CImportX3D()
{
}

bool
CImportX3D::
read(CFile &file)
{
  CThrowScope throwScope(CTHROW_PRINT);

  file_ = &file;

  std::string line;

  file_->readLine(line);

  auto words = CStrUtil::toWords(line, nullptr);
  if (words.size() < 2) return false;

  auto num_points = CStrUtil::toInteger(words[0].getWord());
  auto num_lines  = CStrUtil::toInteger(words[1].getWord());

  for (int i = 0; i < num_points; i++) {
    file_->readLine(line);

    auto words1 = CStrUtil::toWords(line, nullptr);
    if (words1.size() < 3) return false;

    auto x =  CStrUtil::toReal(words1[0].getWord());
    auto y = -CStrUtil::toReal(words1[1].getWord());
    auto z =  CStrUtil::toReal(words1[2].getWord());

    object_->addVertex(CPoint3D(x, y, z));
  }

  for (int j = 0; j < num_lines; j++) {
    file_->readLine(line);

    auto words1 = CStrUtil::toWords(line, nullptr);
    if (words1.size() < 2) return false;

    auto start = CStrUtil::toInteger(words1[0].getWord());
    auto end   = CStrUtil::toInteger(words1[1].getWord());

    object_->addLine(uint(start), uint(end));
  }

  return true;
}
