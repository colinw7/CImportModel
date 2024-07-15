#include <CImportObj.h>
#include <CGeometry3D.h>
#include <CStrUtil.h>

namespace {
  static std::string s_line;
  static int         s_line_num { 0 };
  static std::string s_line1;

  void error(const std::string &msg) {
    std::cerr << "Error: " << msg << ": '" << s_line1 << "' @" << s_line_num << "\n";
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

    auto line1 = CStrUtil::stripSpaces(s_line);

    auto len = line1.size();

    if (len <= 0 || line1[0] == '#')
      continue;

    s_line1 = line1;

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
    else if (len > 6 && line1.substr(0, 6) == "mtllib" && line1[6] == ' ') {
      line1 = CStrUtil::stripSpaces(line1.substr(6));

      if (! readMaterialFile(line1))
        error("Invalid material file");
    }
    else if (len > 6 && line1.substr(0, 6) == "usemtl" && line1[6] == ' ') {
      line1 = CStrUtil::stripSpaces(line1.substr(6));

      auto pm = materials_.find(line1);

      if (pm != materials_.end())
        material_ = (*pm).second;
      else
        error("Invalid material name");
    }
    else {
      error("Unrecognised material line");
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

  CPoint3D p;

  if      (words.size() == 2) {
    if (! CStrUtil::isReal(words[0]) ||
        ! CStrUtil::isReal(words[1]))
      return false;

    double x = CStrUtil::toReal(words[0]);
    double y = CStrUtil::toReal(words[1]);

    p = CPoint3D(x, y, 0.0);
  }
  else if (words.size() == 3) {
    if (! CStrUtil::isReal(words[0]) ||
        ! CStrUtil::isReal(words[1]) ||
        ! CStrUtil::isReal(words[2]))
      return false;

    double x = CStrUtil::toReal(words[0]);
    double y = CStrUtil::toReal(words[1]);
    double z = CStrUtil::toReal(words[1]);

    p = CPoint3D(x, y, z);
  }

  object_->addTexturePoint(p);

  texturePoints_.push_back(p);

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

  object_->addNormal(v);

  return true;
}

bool
CImportObj::
readParameterVertex(const std::string &)
{
  std::cerr << "readParameterVertex not implemented\n";

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
  std::vector<long> texturePoints;
  std::vector<long> normals;

  std::vector<std::string> words;

  CStrUtil::addWords(line, words);

  auto nv = object_->getNumVertices();
  auto nt = object_->getNumTextuePoints();
  auto nn = object_->getNumNormals();

  auto num_words = words.size();

  for (uint i = 0; i < num_words; i++) {
    std::vector<std::string> fields;

    CStrUtil::addFields(words[i], fields, "/");

    while (fields.size() < 3)
      fields.push_back("");

    //---

    // vertex number
    long num1 = -1;

    if (CStrUtil::isInteger(fields[0]))
      num1 = CStrUtil::toInteger(fields[0]);

    if (num1 < 0)
      num1 += nv + 1;

    if (num1 > 0)
      vertices.push_back(uint(num1 - 1));
    else
      assert(false);

    //---

    // texture number (optional)
    if (fields[1] != "") {
      long num2 = -1;

      if (CStrUtil::isInteger(fields[1]))
        num2 = CStrUtil::toInteger(fields[1]);

      if (num2 < 0)
        num2 += nt + 1;

      if (num2 > 0)
        texturePoints.push_back(long(num2 - 1));
      else
        assert(false);
    }
    else
      texturePoints.push_back(-1);

    //---

    // normal number (optional)
    if (fields[2] != "") {
      long num3 = -1;

      if (CStrUtil::isInteger(fields[2]))
        num3 = CStrUtil::toInteger(fields[2]);

      if (num3 < 0)
         num3 += nn + 1;

      if (num3 > 0)
        normals.push_back(long(num3 - 1));
      else
        assert(false);
    }
    else
      normals.push_back(-1);
  }

  //---

  auto faceNum = object_->addFace(vertices);

  auto &face = object_->getFace(faceNum);

  if (groupName_ != "") {
    auto &group = object_->getGroup(groupName_);

    group.addFace(faceNum);

    face.setGroup(group.id());
  }

  //---

  auto ntp = texturePoints.size();
  auto nn1 = normals.size();

  assert(ntp == nn1);

  for (size_t i = 0; i < ntp; ++i) {
    auto &v = object_->getVertex(face.getVertex(uint(i)));

    auto ti = texturePoints[i];

    if (ti >= 0) {
      const auto &p = object_->texturePoint(uint(ti));

      v.setTextureMap(p);
    }

    auto ni = normals[i];

    if (ni >= 0) {
      const auto &n = object_->normal(uint(ni));

      v.setNormal(n);
    }
  }

  //---

  if (material_) {
    CMaterial material;

    material.setAmbient (material_->ka);
    material.setDiffuse (material_->kd);
    material.setSpecular(material_->ks);

    object_->setFaceMaterial(uint(faceNum), material);

    if (! material_->mapKd.name.empty()) {
      auto pm = textureMap_.find(material_->mapKd.name);

      if (pm == textureMap_.end()) {
        auto *texture = CGeometryInst->createTexture(material_->mapKd.image);

        pm = textureMap_.insert(pm, TextureMap::value_type(material_->mapKd.name, texture));
      }

      object_->setFaceDiffuseTexture(uint(faceNum), (*pm).second);
    }

    if (! material_->mapKs.name.empty()) {
      auto pm = textureMap_.find(material_->mapKs.name);

      if (pm == textureMap_.end()) {
        auto *texture = CGeometryInst->createTexture(material_->mapKs.image);

        pm = textureMap_.insert(pm, TextureMap::value_type(material_->mapKs.name, texture));
      }

      object_->setFaceSpecularTexture(uint(faceNum), (*pm).second);
    }

    if (! material_->mapBump.name.empty()) {
      auto pm = textureMap_.find(material_->mapBump.name);

      if (pm == textureMap_.end()) {
        auto *texture = CGeometryInst->createTexture(material_->mapBump.image);

        pm = textureMap_.insert(pm, TextureMap::value_type(material_->mapBump.name, texture));
      }

      object_->setFaceNormalTexture(uint(faceNum), (*pm).second);
    }
  }

  return true;
}

bool
CImportObj::
readMaterialFile(const std::string &filename)
{
  CFile file(filename);

  if (! file.exists())
    return false;

  auto base = file.getBase();

  auto readRGB = [&](const std::string &line, CRGBA &rgba) {
    std::vector<std::string> words;

    CStrUtil::addWords(line, words);

    if (words.size() != 3 ||
        ! CStrUtil::isReal(words[0]) ||
        ! CStrUtil::isReal(words[1]) ||
        ! CStrUtil::isReal(words[2]))
      return false;

    auto r = CStrUtil::toReal(words[0]);
    auto g = CStrUtil::toReal(words[1]);
    auto b = CStrUtil::toReal(words[2]);

    rgba = CRGBA(r, g, b);

    return true;
  };

  auto *material = new Material;

  material->name = "default";

  materials_[material->name] = material;

  while (file.readLine(s_line)) {
    auto line1 = CStrUtil::stripSpaces(s_line);

    auto len = line1.size();

    if (len <= 0 || line1[0] == '#')
      continue;

    s_line1 = line1;

    if      (len > 6 && line1.substr(0, 6) == "newmtl" && line1[6] == ' ') {
      auto name = CStrUtil::stripSpaces(line1.substr(6));
      //std::cout << "newmtl " << name << "\n";

      material = new Material;

      material->name = name;

      materials_[material->name] = material;

      auto p = materials_.find(base);

      if (p == materials_.end())
        materials_[base] = material;
    }
    // ambient
    else if (len > 2 && line1.substr(0, 2) == "Ka" && line1[2] == ' ') {
      if (! readRGB(CStrUtil::stripSpaces(line1.substr(2)), material->ka))
        error("Invalid data for Ka");
    }
    // diffuse
    else if (len > 2 && line1.substr(0, 2) == "Kd" && line1[2] == ' ') {
      if (! readRGB(CStrUtil::stripSpaces(line1.substr(2)), material->kd))
        error("Invalid data for Kd");
    }
    // emissive
    else if (len > 2 && line1.substr(0, 2) == "Ke" && line1[2] == ' ') {
      if (! readRGB(CStrUtil::stripSpaces(line1.substr(2)), material->ke))
        error("Invalid data for Ke");
    }
    // specular
    else if (len > 2 && line1.substr(0, 2) == "Ks" && line1[2] == ' ') {
      if (! readRGB(CStrUtil::stripSpaces(line1.substr(2)), material->ks))
        error("Invalid data for Ks");
    }
    // illumination model
    //   0. Color on and Ambient off
    //   1. Color on and Ambient on
    //   2. Highlight on
    //   3. Reflection on and Ray trace on
    //   4. Transparency: Glass on, Reflection: Ray trace on
    //   5. Reflection: Fresnel on and Ray trace on
    //   6. Transparency: Refraction on, Reflection: Fresnel off and Ray trace on
    //   7. Transparency: Refraction on, Reflection: Fresnel on and Ray trace on
    //   8. Reflection on and Ray trace off
    //   9. Transparency: Glass on, Reflection: Ray trace off
    //  10. Casts shadows onto invisible surfaces
    else if (len > 5 && line1.substr(0, 5) == "illum" && line1[5] == ' ') {
      auto rhs = CStrUtil::stripSpaces(line1.substr(5));

      if (CStrUtil::isInteger(rhs))
        material->illum = int(CStrUtil::toInteger(rhs));
      else
        error("Invalid data for illum");
    }
    // specular exponent
    else if (len > 2 && line1.substr(0, 2) == "Ns" && line1[2] == ' ') {
      auto rhs = CStrUtil::stripSpaces(line1.substr(2));

      if (CStrUtil::isReal(rhs))
        material->ns = CStrUtil::toReal(rhs);
      else
        error("Invalid data for Ns");
    }
    // optical density
    else if (len > 2 && line1.substr(0, 2) == "Ni" && line1[2] == ' ') {
      auto rhs = CStrUtil::stripSpaces(line1.substr(2));

      if (CStrUtil::isReal(rhs))
        material->ni = CStrUtil::toReal(rhs);
      else
        error("Invalid data for Ni");
    }
    // transparency
    else if (len > 2 && line1.substr(0, 2) == "Tr" && line1[2] == ' ') {
      auto rhs = CStrUtil::stripSpaces(line1.substr(2));

      if (CStrUtil::isReal(rhs))
        material->tr = CStrUtil::toReal(rhs);
      else
        error("Invalid data for Tr");
    }
    else if (len > 2 && line1.substr(0, 2) == "Tf" && line1[2] == ' ') {
      CRGBA tf;
      if (! readRGB(CStrUtil::stripSpaces(line1.substr(2)), tf))
        error("Invalid data for Tf");
    }
    else if (len > 1 && line1.substr(0, 1) == "d" && line1[1] == ' ') {
      auto rhs = CStrUtil::stripSpaces(line1.substr(1));

      if (CStrUtil::isReal(rhs))
        material->tr = 1.0 - CStrUtil::toReal(rhs);
      else
        error("Invalid data for d");
    }
    // ambient
    else if (len > 6 && line1.substr(0, 6) == "map_Ka" && line1[6] == ' ') {
      auto imageFilename = CStrUtil::stripSpaces(line1.substr(6));

      CFile imageFile(imageFilename);

      if (imageFile.exists()) {
        CImageFileSrc src(imageFile);

        auto image = CImageMgrInst->createImage(src);

        image = image->flippedH();

        material->mapKa.image = image;
        material->mapKa.name  = imageFilename;
      }
      else
        error("Invalid file '" + imageFilename + "' for map_Ka");
    }
    // diffuse
    else if (len > 6 && line1.substr(0, 6) == "map_Kd" && line1[6] == ' ') {
      auto imageFilename = CStrUtil::stripSpaces(line1.substr(6));

      CFile imageFile(imageFilename);

      if (imageFile.exists()) {
        CImageFileSrc src(imageFile);

        auto image = CImageMgrInst->createImage(src);

        image = image->flippedH();

        material->mapKd.image = image;
        material->mapKd.name  = imageFilename;
      }
      else
        error("Invalid file for map_Kd");
    }
    // specular
    else if (len > 6 && line1.substr(0, 6) == "map_Ks" && line1[6] == ' ') {
      auto imageFilename = CStrUtil::stripSpaces(line1.substr(6));

      CFile imageFile(imageFilename);

      if (imageFile.exists()) {
        CImageFileSrc src(imageFile);

        auto image = CImageMgrInst->createImage(src);

        image = image->flippedH();

        material->mapKs.image = image;
        material->mapKs.name  = imageFilename;
      }
      else
        error("Invalid file for map_Ks");
    }
    // bump map
    else if (len > 8 && (line1.substr(0, 8) == "map_Bump" ||
                         line1.substr(0, 8) == "map_bump") && line1[8] == ' ') {
      auto imageFilename = CStrUtil::stripSpaces(line1.substr(8));

      CFile imageFile(imageFilename);

      if (imageFile.exists()) {
        CImageFileSrc src(imageFile);

        auto image = CImageMgrInst->createImage(src);

        image = image->flippedH();

        material->mapBump.image = image;
        material->mapBump.name  = imageFilename;
      }
      else
        error("Invalid file for map_Bump");
    }
    // map d ?
    else if (len > 5 && line1.substr(0, 5) == "map_d" && line1[5] == ' ') {
    }
    // bump
    else if (len > 8 && (line1.substr(0, 4) == "bump") && line1[4] == ' ') {
      // TODO
    }
    else {
      // TODO: map_Ns, map_d, bump, disp, decal
      error("Unrecognised line");
    }
  }

  return true;
}
