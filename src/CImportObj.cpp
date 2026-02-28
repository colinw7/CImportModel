#include <CImportObj.h>
#include <CGeometry3D.h>
#include <CStrUtil.h>

namespace {
  static std::string s_line;
  static int         s_line_num { 0 };
  static std::string s_line1;
  static std::string s_value;

  void error(const std::string &msg) {
    std::cerr << "Error: " << msg << ": '" << s_line1 << "' @" << s_line_num << "\n";
  }

  void warning(const std::string &msg) {
    std::cerr << "Warning: " << msg << ": '" << s_line1 << "' @" << s_line_num << "\n";
  }
}

CImportObj::
CImportObj(CGeomScene3D *scene, const std::string &name) :
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

    // vertex
    if      (len > 2 && line1[0] == 'v' && line1[1] == ' ') {
      line1 = CStrUtil::stripSpaces(line1.substr(2));

      if (! readVertex(line1))
        error("Invalid vertex line");

      ++vnum_;
    }
    // vertex texture coord
    else if (len > 2 && line1[0] == 'v' && line1[1] == 't') {
      line1 = CStrUtil::stripSpaces(line1.substr(2));

      if (! readTextureVertex(line1))
        error("Invalid texture vertex line");

      ++vtnum_;
    }
    // vertex normal coord
    else if (len > 2 && line1[0] == 'v' && line1[1] == 'n') {
      line1 = CStrUtil::stripSpaces(line1.substr(2));

      if (! readVertexNormal(line1))
        error("Invalid vertex normal line");

      ++vnnum_;
    }
    // parameter space vertex
    else if (len > 2 && line1[0] == 'v' && line1[1] == 'p') {
      line1 = CStrUtil::stripSpaces(line1.substr(2));

      if (! readParameterVertex(line1))
        error("Invalid parameter vertex line");
    }
    // group
    else if (len > 2 && line1[0] == 'g' && line1[1] == ' ') {
      line1 = CStrUtil::stripSpaces(line1.substr(2));

      if (! readGroupName(line1))
        error("Invalid group name line");
    }
    else if (len == 1 && line1[0] == 'g') {
      if (! readGroupName(""))
        error("Invalid group name line");
    }
    // face
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
    else if (len > 2 && line1[0] == 'l' && line1[1] == ' ') {
      // skip line
    }
    else if (len > 6 && line1.substr(0, 6) == "mtllib" && line1[6] == ' ') {
      line1 = CStrUtil::stripSpaces(line1.substr(6));

      if (! readMaterialFile(line1))
        error("Invalid material file");
    }
    else if (len > 6 && line1.substr(0, 6) == "usemtl" && line1[6] == ' ') {
      line1 = CStrUtil::stripSpaces(line1.substr(6));

      auto pm = materials_.find(line1);

      if (pm == materials_.end()) {
        warning("Invalid material name");

        material_ = addMaterial(line1);
      }
      else
        material_ = (*pm).second;
    }
    else {
      error("Unrecognised material line");
    }
  }

  //---

  if (isTriangulate()) {
    auto faces = object_->getFaces();

    for (auto *face : faces)
      face->triangulate();
  }

  //---

  if (isSplitByMaterial()) {
    std::vector<CGeomObject3D *> newObjects;

    if (object_->splitFacesByMaterial(newObjects)) {
      for (auto *newObject : newObjects) {
        scene_->addObject(newObject);

        object_->addChild(newObject);
      }

      object_->clearGeometry(/*destroy*/false);
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

  if (words.size() < 3)
    return false;

  if (! CStrUtil::isReal(words[0]) ||
      ! CStrUtil::isReal(words[1]) ||
      ! CStrUtil::isReal(words[2]))
    return false;

  double x = CStrUtil::toReal(words[0]);
  double y = CStrUtil::toReal(words[1]);
  double z = CStrUtil::toReal(words[2]);

  CPoint3D p(x, y, z);

  int ind = object_->addVertex(p);

  if (words.size() >= 6) {
    if (! CStrUtil::isReal(words[3]) ||
        ! CStrUtil::isReal(words[4]) ||
        ! CStrUtil::isReal(words[5]))
      return false;

    double r = CStrUtil::toReal(words[3]);
    double g = CStrUtil::toReal(words[4]);
    double b = CStrUtil::toReal(words[5]);

    object_->setVertexColor(ind, CRGBA(r, g, b));
  }

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
    double z = CStrUtil::toReal(words[2]);

    p = CPoint3D(x, y, z);
  }

  object_->addTexturePoint(p);

//texturePoints_.push_back(p);

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

  auto *face = object_->getFaceP(faceNum);

  if (groupName_ != "") {
    auto &group = object_->getGroup(groupName_);

    group.addFace(faceNum);

    face->setGroup(group.id());
  }

  //---

  auto ntp = texturePoints.size();
  auto nn1 = normals.size();

  assert(ntp == nn1);

  std::vector<CPoint2D>  texturePoints1;
  std::vector<CVector3D> normals1;

  for (size_t i = 0; i < ntp; ++i) {
    auto ind = vertices[i];

    auto &v = object_->getVertex(ind);

    auto ti = texturePoints[i];

    if (ti >= 0) {
      const auto &p = object_->texturePoint(uint(ti));

      auto p1 = CPoint2D(p.x, p.y);

      texturePoints1.push_back(p1);

      v.setTextureMap(p1);
    }

    auto ni = normals[i];

    if (ni >= 0) {
      const auto &n = object_->normal(uint(ni));

      normals1.push_back(n);

      v.setNormal(n);
    }
  }

  //---

  if (material_) {
    auto *material = pscene_->getMaterial(material_->name);

    if (! material) {
      material = CGeometry3DInst->createMaterial();

      material->setName(material_->name);

      pscene_->addMaterial(material);
    }

    //---

    if (material_->ambientColor)
      material->setAmbient(*material_->ambientColor);

    if (material_->diffuseColor)
      material->setDiffuse(*material_->diffuseColor);

    if (material_->specularColor)
      material->setSpecular(*material_->specularColor);

    auto getTexture = [&](const std::string &name, CImagePtr &image) {
      auto *texture = pscene_->getTextureByName(name);

      if (! texture) {
        texture = CGeometry3DInst->createTexture(image);

        texture->setName(name);

        pscene_->addTexture(texture);
      }

      return texture;
    };

    if (! material_->diffuseMap.name.empty()) {
      auto *texture = getTexture(material_->diffuseMap.name, material_->diffuseMap.image);

      material->setDiffuseTexture(texture);
      face    ->setDiffuseTexture(texture);
    }

    if (! material_->bumpMap.name.empty()) {
      auto *texture = getTexture(material_->bumpMap.name, material_->bumpMap.image);

      material->setNormalTexture(texture);
      face    ->setNormalTexture(texture);
    }

    if (! material_->specularMap.name.empty()) {
      auto *texture = getTexture(material_->specularMap.name, material_->specularMap.image);

      material->setSpecularTexture(texture);
      face    ->setSpecularTexture(texture);
    }

    if (! material_->emissiveMap.name.empty()) {
      auto *texture = getTexture(material_->emissiveMap.name, material_->emissiveMap.image);

      material->setEmissiveTexture(texture);
      face    ->setEmissiveTexture(texture);
    }

    if (material_->transparency)
      material->setTransparency(*material_->transparency);

    face->setMaterialP(material);
  }

  //---

  if (texturePoints1.size() == ntp)
    face->setTexturePoints(texturePoints1);

  if (normals1.size() == nn1)
    face->setVertexNormals(normals1);

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

  auto fixFilename = [](const std::string &fname) {
    return CStrUtil::replaceChar(CStrUtil::stripSpaces(fname), '\\', '/');
  };

  auto *material = addMaterial("default");

  auto nameMatch = [&](const std::string &line, const std::string &name) {
    auto len = name.size();

    if (line.size() <= len)
      return false;

    if (line.substr(0, len) != name || line[len] != ' ')
      return false;

    s_value = CStrUtil::stripSpaces(line.substr(len));

    return true;
  };

  while (file.readLine(s_line)) {
    auto line1 = CStrUtil::stripSpaces(s_line);

    auto len = line1.size();

    if (len <= 0 || line1[0] == '#')
      continue;

    s_line1 = line1;

    if      (nameMatch(line1, "newmtl")) {
      //std::cout << "newmtl " << s_value_ << "\n";

      material = addMaterial(s_value);

      auto p = materials_.find(base);

      if (p == materials_.end())
        materials_[base] = material;
    }
    // ambient
    else if (nameMatch(line1, "Ka")) {
      CRGBA c;
      if (readRGB(s_value, c))
        material->ambientColor = c;
      else
        error("Invalid color for Ka");
    }
    // diffuse
    else if (nameMatch(line1, "Kd")) {
      CRGBA c;
      if (readRGB(s_value, c))
        material->diffuseColor = c;
      else
        error("Invalid color for Kd");
    }
    // emissive value
    else if (nameMatch(line1, "Ke")) {
      CRGBA c;
      if (readRGB(s_value, c))
        material->emissionColor = c;
      else
        error("Invalid color for Ke");
    }
    // emissive map
    else if (nameMatch(line1, "map_Ke")) {
      auto imageFilename = fixFilename(s_value);

      CFile imageFile(imageFilename);

      if (imageFile.exists()) {
        CImageFileSrc src(imageFile);

        auto image = CImageMgrInst->createImage(src);

        image = image->flippedH();

        material->emissiveMap.image = image;
        material->emissiveMap.name  = imageFilename;
      }
      else
        error("Invalid file for map_Ke");
    }
    // specular
    else if (nameMatch(line1, "Ks")) {
      CRGBA c;
      if (readRGB(s_value, c))
        material->specularColor = c;
      else
        error("Invalid color for Ks");
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
    else if (nameMatch(line1, "illum")) {
      if (CStrUtil::isInteger(s_value))
        material->illuminationModel = int(CStrUtil::toInteger(s_value));
      else
        error("Invalid integer for illum");
    }
    // specular exponent
    else if (nameMatch(line1, "Ns")) {
      if (CStrUtil::isReal(s_value))
        material->specularExponent = CStrUtil::toReal(s_value);
      else
        error("Invalid real for Ns");
    }
    // optical density
    else if (nameMatch(line1, "Ni")) {
      if (CStrUtil::isReal(s_value))
        material->refractionIndex = CStrUtil::toReal(s_value);
      else
        error("Invalid real for Ni");
    }
    // transparency (inverted) 0.0 = opaque, 1.0 = tramsparent
    else if (nameMatch(line1, "Tr")) {
      if (CStrUtil::isReal(s_value))
        material->transparency = CStrUtil::toReal(s_value);
      else
        error("Invalid real for Tr");
    }
    else if (nameMatch(line1, "Tf")) {
      CRGBA tf;
      if (readRGB(s_value, tf))
        material->transmissionFilterColor = tf;
      else
        error("Invalid color for Tf");
    }
    // transparency (normal) 1.0 = opaque, 0.0 = tramsparent
    else if (nameMatch(line1, "d")) {
      if (CStrUtil::isReal(s_value))
        material->transparency = 1.0 - CStrUtil::toReal(s_value);
      else
        error("Invalid real for d");
    }
    // ambient
    else if (nameMatch(line1, "map_Ka")) {
      auto imageFilename = fixFilename(s_value);

      CFile imageFile(imageFilename);

      if (imageFile.exists()) {
        CImageFileSrc src(imageFile);

        auto image = CImageMgrInst->createImage(src);

        image = image->flippedH();

        material->ambientMap.image = image;
        material->ambientMap.name  = imageFilename;
      }
      else
        error("Invalid file '" + imageFilename + "' for map_Ka");
    }
    // diffuse
    else if (nameMatch(line1, "map_Kd")) {
      auto imageFilename = fixFilename(s_value);

      CFile imageFile(imageFilename);

      if (imageFile.exists()) {
        CImageFileSrc src(imageFile);

        auto image = CImageMgrInst->createImage(src);

        image = image->flippedH();

        material->diffuseMap.image = image;
        material->diffuseMap.name  = imageFilename;
      }
      else
        error("Invalid file for map_Kd");
    }
    // specular
    else if (nameMatch(line1, "map_Ks")) {
      auto imageFilename = fixFilename(s_value);

      CFile imageFile(imageFilename);

      if (imageFile.exists()) {
        CImageFileSrc src(imageFile);

        auto image = CImageMgrInst->createImage(src);

        image = image->flippedH();

        material->specularMap.image = image;
        material->specularMap.name  = imageFilename;
      }
      else
        error("Invalid file for map_Ks");
    }
    // bump map
    else if (nameMatch(line1, "map_Bump") || nameMatch(line1, "map_bump")) {
      std::vector<std::string> words;
      CStrUtil::addWords(s_value, words);

      if (words[0] == "-bm" && words.size() == 3)
        s_value = words[2];

      auto imageFilename = fixFilename(s_value);

      CFile imageFile(imageFilename);

      if (imageFile.exists()) {
        CImageFileSrc src(imageFile);

        auto image = CImageMgrInst->createImage(src);

        image = image->flippedH();

        material->bumpMap.image = image;
        material->bumpMap.name  = imageFilename;
      }
      else
        error("Invalid file for map_Bump");
    }
    // Pm - physical based rendering (PBR) metallic value
    else if (nameMatch(line1, "Pm")) {
      if (CStrUtil::isReal(s_value))
        material->metallic = CStrUtil::toReal(s_value);
      else
        error("Invalid real for pM");
    }
    // map_Pm - physical based rendering (PBR) metallic map
    else if (nameMatch(line1, "map_Pm")) {
      auto imageFilename = fixFilename(s_value);

      CFile imageFile(imageFilename);

      if (imageFile.exists()) {
        CImageFileSrc src(imageFile);

        auto image = CImageMgrInst->createImage(src);

        image = image->flippedH();

        material->metallicMap.image = image;
        material->metallicMap.name  = imageFilename;
      }
      else
        error("Invalid file for map_Pm");
    }
    // Pr - physical based rendering (PBR) roughness value
    else if (nameMatch(line1, "Pr")) {
      if (CStrUtil::isReal(s_value))
        material->roughness = CStrUtil::toReal(s_value);
      else
        error("Invalid real for Pr");
    }
    // map_Pr - physical based rendering (PBR) roughness map
    else if (nameMatch(line1, "map_Pr")) {
      auto imageFilename = fixFilename(s_value);

      CFile imageFile(imageFilename);

      if (imageFile.exists()) {
        CImageFileSrc src(imageFile);

        auto image = CImageMgrInst->createImage(src);

        image = image->flippedH();

        material->roughnessMap.image = image;
        material->roughnessMap.name  = imageFilename;
      }
      else
        error("Invalid file for map_Pr");
    }
    // map_Ns - physical based rendering (PBR) specular highlight map
    else if (nameMatch(line1, "map_Ns") || nameMatch(line1, "map_NS")) {
      // TODO
    }
    // map d ?
    else if (nameMatch(line1, "map_d")) {
      // TODO
    }
    // normal map
    else if (nameMatch(line1, "norm")) {
      // TODO
    }
    // anisotropy
    else if (nameMatch(line1, "aniso")) {
      // TODO
    }
    // anisotropy rotation
    else if (nameMatch(line1, "anisor")) {
      // TODO
    }
    // bump map ?
    else if (nameMatch(line1, "bump")) {
      // TODO
    }
    // displacement map ?
    else if (nameMatch(line1, "disp")) {
      // TODO
    }
    // Pc - physical based rendering (PBR) clearcoat thickness value
    else if (nameMatch(line1, "Pc")) {
      // TODO
    }
    // Pcr - physical based rendering (PBR) clearcoat roughness value
    else if (nameMatch(line1, "Pcr")) {
      // TODO
    }
    else {
      // TODO: map_d, disp, decal
      error("Unrecognised line");
    }
  }

  if (isDebug()) {
    auto printOptValue = [&](const std::string &name, const auto &v) {
      if (v)
        std::cerr << "  " << name << ": " << *v << "\n";
    };

    auto printMapImage = [&](const std::string &name, const MapImage &v) {
      if (v.name != "")
        std::cerr << "  " << name << ": " << v.name << "\n";
    };

    for (const auto &pm : materials_) {
      auto *material1 = pm.second;

      std::cerr << "Material: '" << material1->name << "'\n";

      printOptValue("ambientColor"           , material1->ambientColor           );
      printOptValue("diffuseColor"           , material1->diffuseColor           );
      printOptValue("specularColor"          , material1->specularColor          );
      printOptValue("emissionColor"          , material1->emissionColor          );
      printOptValue("illuminationModel"      , material1->illuminationModel      );
      printOptValue("specularExponent"       , material1->specularExponent       );
      printOptValue("refractionIndex"        , material1->refractionIndex        );
      printOptValue("transparency"           , material1->transparency           );
      printOptValue("transmissionFilterColor", material1->transmissionFilterColor);

      printMapImage("ambientMap" , material1->ambientMap );
      printMapImage("diffuseMap" , material1->diffuseMap );
      printMapImage("specularMap", material1->specularMap);
      printMapImage("emissiveMap", material1->emissiveMap);
      printMapImage("bumpMap"    , material1->bumpMap    );
    }
  }

  return true;
}

CImportObj::Material *
CImportObj::
addMaterial(const std::string &name)
{
  auto *material = new Material;

  material->name = name;

  materials_[material->name] = material;

  return material;
}
