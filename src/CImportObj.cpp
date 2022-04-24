#include <CImportObj.h>
#include <CGeometry3D.h>
#include <CStrUtil.h>

namespace {
  static std::string s_line;
  static int         s_line_num { 0 };

  void error(const char *msg) {
    std::cerr << "Error: " << msg << ": '" << s_line << "'@" << s_line_num << std::endl;
  }
}

CImportObj::
CImportObj(CGeomScene3D *scene, const std::string &name) :
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

CImportObj::
~CImportObj()
{
}

bool
CImportObj::
read(CFile &file)
{
  file_ = &file;

  vnum_  = 0;
  vnnum_ = 0;

  while (file_->readLine(s_line)) {
    ++s_line_num;

    std::string line1 = CStrUtil::stripSpaces(s_line);

    auto len = line1.size();

    if (len <= 0 || line1[0] == '#')
      continue;

    if      (len > 2 && line1[0] == 'v' && line1[1] == ' ') {
      line1 = CStrUtil::stripSpaces(line1.substr(2));

      if (! readVertex(line1))
        error("Invalid vertex line");

      ++vnum_;
    }
    else if (len > 2 && line1[0] == 'v' && line1[1] == 't') {
      line1 = CStrUtil::stripSpaces(line1.substr(2));

      if (! readTextureVertex(line1))
        error("Invalid texture vertex line");

      ++vtnum_;
    }
    else if (len > 2 && line1[0] == 'v' && line1[1] == 'n') {
      line1 = CStrUtil::stripSpaces(line1.substr(2));

      if (! readVertexNormal(line1))
        error("Invalid vertex normal line");

      ++vnnum_;
    }
    else if (len > 2 && line1[0] == 'v' && line1[1] == 'p') {
      line1 = CStrUtil::stripSpaces(line1.substr(2));

      if (! readParameterVertex(line1))
        error("Invalid parameter vertex line");
    }
    else if (len > 2 && line1[0] == 'g' && line1[1] == ' ') {
      line1 = CStrUtil::stripSpaces(line1.substr(2));

      if (! readGroupName(line1))
        error("Invalid group name line");
    }
    else if (len == 1 && line1[0] == 'g') {
      if (! readGroupName(""))
        error("Invalid group name line");
    }
    else if (len > 2 && line1[0] == 'f' && line1[1] == ' ') {
      line1 = CStrUtil::stripSpaces(line1.substr(2));

      if (! readFace(line1))
        error("Invalid face line");
    }
    else if (len > 2 && line1[0] == 'o' && line1[1] == ' ') {
      // skip object name
    }
    else if (len > 2 && line1[0] == 's' && line1[1] == ' ') {
      // skip smoothing group
    }
    else if (len > 6 && line1.substr(0, 6) == "mtllib") {
      // todo
    }
    else if (len > 6 && line1.substr(0, 6) == "usemtl") {
      // todo
    }
    else {
      error("Unrecognised line");
    }
  }

  return true;
}

bool
CImportObj::
readVertex(const std::string &line)
{
  std::vector<std::string> words;

  CStrUtil::addWords(line, words);

  if (words.size() != 3)
    return false;

  if (! CStrUtil::isReal(words[0]) ||
      ! CStrUtil::isReal(words[1]) ||
      ! CStrUtil::isReal(words[2]))
    return false;

  double x = CStrUtil::toReal(words[0]);
  double y = CStrUtil::toReal(words[1]);
  double z = CStrUtil::toReal(words[2]);

  CPoint3D p(x, y, z);

  object_->addVertex(p);

  return true;
}

bool
CImportObj::
readTextureVertex(const std::string &line)
{
  std::vector<std::string> words;

  CStrUtil::addWords(line, words);

  if      (words.size() == 2) {
    if (! CStrUtil::isReal(words[0]) ||
        ! CStrUtil::isReal(words[1]))
      return false;

#if 0
    double x = CStrUtil::toReal(words[0]);
    double y = CStrUtil::toReal(words[1]);

    CPoint2D p(x, y);

    CGeomVertex3D &vertex = object_->getVertex(vtnum_);

    vertex->setTextureMap(p);
#endif
  }
  else if (words.size() == 3) {
    if (! CStrUtil::isReal(words[0]) ||
        ! CStrUtil::isReal(words[1]) ||
        ! CStrUtil::isReal(words[2]))
      return false;

#if 0
    double x = CStrUtil::toReal(words[0]);
    double y = CStrUtil::toReal(words[1]);
    double z = CStrUtil::toReal(words[1]);

    CPoint3D p(x, y, z);

    CGeomVertex3D &vertex = object_->getVertex(vtnum_);

    vertex->setTextureMap(p);
#endif
  }

  return true;
}

bool
CImportObj::
readVertexNormal(const std::string &line)
{
  std::vector<std::string> words;

  CStrUtil::addWords(line, words);

  if (words.size() != 3)
    return false;

  if (! CStrUtil::isReal(words[0]) ||
      ! CStrUtil::isReal(words[1]) ||
      ! CStrUtil::isReal(words[2]))
    return false;

  double x = CStrUtil::toReal(words[0]);
  double y = CStrUtil::toReal(words[1]);
  double z = CStrUtil::toReal(words[2]);

  CVector3D v(x, y, z);

  CGeomVertex3D &vertex = object_->getVertex(uint(vnnum_));

  vertex.setNormal(v);

  return true;
}

bool
CImportObj::
readParameterVertex(const std::string &)
{
  return true;
}

bool
CImportObj::
readGroupName(const std::string &line)
{
  groupName_ = line;

  return true;
}

bool
CImportObj::
readFace(const std::string &line)
{
  std::vector<uint> vertices;

  std::vector<std::string> words;

  CStrUtil::addWords(line, words);

  auto num_words = words.size();

  for (uint i = 0; i < num_words; i++) {
    std::vector<std::string> fields;

    CStrUtil::addFields(words[i], fields, "/");

    while (fields.size() < 3)
      fields.push_back("");

    long num1 = -1;
    long num2 = -1;
    long num3 = -1;

    if (CStrUtil::isInteger(fields[0]))
      num1 = CStrUtil::toInteger(fields[0]);

    if (CStrUtil::isInteger(fields[1]))
      num2 = CStrUtil::toInteger(fields[1]);

    if (CStrUtil::isInteger(fields[2]))
      num3 = CStrUtil::toInteger(fields[2]);

    if (num1 > 0)
      vertices.push_back(uint(num1 - 1));

    assert(num2 >= -1 && num3 >= -1);
  }

  auto faceNum = object_->addFace(vertices);

  if (groupName_ != "") {
    CGeomObject3D::Group &group = object_->getGroup(groupName_);

    group.addFace(faceNum);

    CGeomFace3D &face = object_->getFace(faceNum);

    face.setGroup(group.id());
  }

  return true;
}
