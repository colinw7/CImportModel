#include <CImportCOB.h>
#include <CStrUtil.h>
#include <CRegExp.h>
#include <CStrParse.h>

CImportCOB::
CImportCOB() :
 debug_(false), flags_(0)
{
}

CImportCOB::
~CImportCOB()
{
}

bool
CImportCOB::
read(CFile &file)
{
  file_ = &file;

  if (! readName())
    return false;

  if (! readCoordinates())
    return false;

  if (! readTransform())
    return false;

  if (! readVertices())
    return false;

  if (! readTextureVertices())
    return false;

  if (! readFaces())
    return false;

  if (! readMaterials())
    return false;

  return true;
}

bool
CImportCOB::
readName()
{
  pattern_ = "Name \\(.*\\)";

  bool found = false;

  while (readNextLine()) {
    if (CRegExpUtil::parse(line_, pattern_, match_strs_)) {
      name_ = match_strs_[0];

      found = true;

      break;
    }
  }

  if (! found)
    return false;

  return true;
}

bool
CImportCOB::
readCoordinates()
{
  pattern_ = "center \\(.*\\) \\(.*\\) \\(.*\\)";

  bool found = false;

  while (readNextLine()) {
    if (CRegExpUtil::parse(line_, pattern_, match_strs_)) {
      if (! CStrUtil::toReal(match_strs_[0], &center_.x)) return false;
      if (! CStrUtil::toReal(match_strs_[1], &center_.y)) return false;
      if (! CStrUtil::toReal(match_strs_[2], &center_.z)) return false;

      if (! readNextLine())
        return false;

      pattern_ = "x axis \\(.*\\) \\(.*\\) \\(.*\\)";

      if (! CRegExpUtil::parse(line_, pattern_, match_strs_))
        return false;

      if (! CStrUtil::toReal(match_strs_[0], &x_axis_.x)) return false;
      if (! CStrUtil::toReal(match_strs_[1], &x_axis_.y)) return false;
      if (! CStrUtil::toReal(match_strs_[2], &x_axis_.z)) return false;

      if (! readNextLine())
        return false;

      pattern_ = "y axis \\(.*\\) \\(.*\\) \\(.*\\)";

      if (! CRegExpUtil::parse(line_, pattern_, match_strs_))
        return false;

      if (! CStrUtil::toReal(match_strs_[0], &y_axis_.x)) return false;
      if (! CStrUtil::toReal(match_strs_[1], &y_axis_.y)) return false;
      if (! CStrUtil::toReal(match_strs_[2], &y_axis_.z)) return false;

      if (! readNextLine())
        return false;

      pattern_ = "z axis \\(.*\\) \\(.*\\) \\(.*\\)";

      if (! CRegExpUtil::parse(line_, pattern_, match_strs_))
        return false;

      if (! CStrUtil::toReal(match_strs_[0], &z_axis_.x)) return false;
      if (! CStrUtil::toReal(match_strs_[1], &z_axis_.y)) return false;
      if (! CStrUtil::toReal(match_strs_[2], &z_axis_.z)) return false;

      found = true;

      local_matrix_.setIdentity();

      local_matrix_.setColumn(0, x_axis_);
      local_matrix_.setColumn(1, y_axis_);
      local_matrix_.setColumn(2, z_axis_);
      local_matrix_.setColumn(3, center_);

      break;
    }
  }

  if (! found)
    return false;

  return true;
}

bool
CImportCOB::
readTransform()
{
  pattern_ = "Transform";

  bool found = false;

  while (readNextLine()) {
    if (CRegExpUtil::parse(line_, pattern_)) {
      pattern_ = "\\(.*\\) \\(.*\\) \\(.*\\) \\(.*\\)";

      if (! readNextLine())
        return false;

      if (! CRegExpUtil::parse(line_, pattern_, match_strs_))
        return false;

      CPoint3D rx, ry, rz;

      if (! CStrUtil::toReal(match_strs_[0], &rx.x)) return false;
      if (! CStrUtil::toReal(match_strs_[1], &rx.y)) return false;
      if (! CStrUtil::toReal(match_strs_[2], &rx.z)) return false;

      if (! readNextLine())
        return false;

      if (! CRegExpUtil::parse(line_, pattern_, match_strs_))
        return false;

      if (! CStrUtil::toReal(match_strs_[0], &ry.x)) return false;
      if (! CStrUtil::toReal(match_strs_[1], &ry.y)) return false;
      if (! CStrUtil::toReal(match_strs_[2], &ry.z)) return false;

      if (! readNextLine())
        return false;

      if (! CRegExpUtil::parse(line_, pattern_, match_strs_))
        return false;

      if (! CStrUtil::toReal(match_strs_[0], &rz.x)) return false;
      if (! CStrUtil::toReal(match_strs_[1], &rz.y)) return false;
      if (! CStrUtil::toReal(match_strs_[2], &rz.z)) return false;

      world_matrix_.setIdentity();

      world_matrix_.setColumn(0, rx);
      world_matrix_.setColumn(1, ry);
      world_matrix_.setColumn(2, rz);

      found = true;

      break;
    }
  }

  if (! found)
    return false;

  return true;
}

bool
CImportCOB::
readVertices()
{
  pattern_ = "World Vertices \\(.*\\)";

  bool found = false;

  while (readNextLine()) {
    if (CRegExpUtil::parse(line_, pattern_, match_strs_)) {
      if (! CStrUtil::toInteger(match_strs_[0], &num_vertices_)) return false;

      found = true;

      break;
    }
  }

  if (! found)
    return false;

  pattern_  = "\\(.*\\) \\(.*\\) \\(.*\\)";

  int i = 0;

  while (i < num_vertices_) {
    if (! readNextLine())
      return false;

    if (! CRegExpUtil::parse(line_, pattern_, match_strs_))
      return false;

    double x, y, z;

    if (! CStrUtil::toReal(match_strs_[0], &x)) return false;
    if (! CStrUtil::toReal(match_strs_[1], &y)) return false;
    if (! CStrUtil::toReal(match_strs_[2], &z)) return false;

    if (flags_ & TRANSFORM_LOCAL)
      local_matrix_.multiplyPoint(x, y, z, &x, &y, &z);

    if (flags_ & TRANSFORM_WORLD)
      world_matrix_.multiplyPoint(x, y, z, &x, &y, &z);

    if (flags_ & INVERT_X) x = -x;
    if (flags_ & INVERT_Y) y = -y;
    if (flags_ & INVERT_Z) z = -z;

    if (flags_ & SWAP_XY) std::swap(x, y);
    if (flags_ & SWAP_XZ) std::swap(x, z);
    if (flags_ & SWAP_YZ) std::swap(y, z);

    addVertex(x, y, z);

    ++i;
  }

  return true;
}

bool
CImportCOB::
readTextureVertices()
{
  pattern_ = "Texture Vertices \\(.*\\)";

  bool found = false;

  while (readNextLine()) {
    if (CRegExpUtil::parse(line_, pattern_, match_strs_)) {
      if (! CStrUtil::toInteger(match_strs_[0], &num_texture_vertices_))
        return false;

      found = true;

      break;
    }
  }

  if (! found)
    return false;

  pattern_  = "\\(.*\\) \\(.*\\)";

  int i = 0;

  while (i < num_texture_vertices_) {
    if (! readNextLine())
      return false;

    if (! CRegExpUtil::parse(line_, pattern_, match_strs_))
      return false;

    double x, y;

    if (! CStrUtil::toReal(match_strs_[0], &x)) return false;
    if (! CStrUtil::toReal(match_strs_[1], &y)) return false;

    addTexturePoint(x, y);

    ++i;
  }

  return true;
}

bool
CImportCOB::
readFaces()
{
  pattern_ = "Faces \\(.*\\)";

  bool found = false;

  while (readNextLine()) {
    if (CRegExpUtil::parse(line_, pattern_, match_strs_)) {

      if (! CStrUtil::toInteger(match_strs_[0], &num_faces_)) return false;

      found = true;

      break;
    }
  }

  if (! found)
    return false;

  int i = 0;

  while (i < num_faces_) {
    if (! readNextLine())
      return false;

    pattern_ = "Face verts \\(.*\\) flags \\(.*\\) mat \\(.*\\)";

    if (! CRegExpUtil::parse(line_, pattern_, match_strs_))
      return false;

    int num, flags, imat;

    if (! CStrUtil::toInteger(match_strs_[0], &num  )) return false;
    if (! CStrUtil::toInteger(match_strs_[1], &flags)) return false;
    if (! CStrUtil::toInteger(match_strs_[2], &imat )) return false;

    if (num != 3)
      return false;

    if (! readNextLine())
      return false;

    pattern_ = "<\\(.*\\),\\(.*\\)> <\\(.*\\),\\(.*\\)> <\\(.*\\),\\(.*\\)>";

    if (! CRegExpUtil::parse(line_, pattern_, match_strs_))
      return false;

    int v1, v2, v3;
    int t1, t2, t3;

    if (! CStrUtil::toInteger(match_strs_[0], &v1)) return false;
    if (! CStrUtil::toInteger(match_strs_[2], &v2)) return false;
    if (! CStrUtil::toInteger(match_strs_[4], &v3)) return false;

    if (! CStrUtil::toInteger(match_strs_[1], &t1)) return false;
    if (! CStrUtil::toInteger(match_strs_[3], &t2)) return false;
    if (! CStrUtil::toInteger(match_strs_[5], &t3)) return false;

    if (flags_ & INVERT_WINDING_ORDER) {
      std::swap(v1, v3);
      std::swap(t1, t3);
    }

    addITriangle(v1, v2, v3, t1, t2, t3, imat);

    ++i;
  }

  return true;
}

bool
CImportCOB::
readMaterials()
{
  while (readNextLine()) {
    pattern_ = "mat# \\(.*\\)";

    if (! CRegExpUtil::parse(line_, pattern_, match_strs_))
      continue;

    int num;

    if (! CStrUtil::toInteger(match_strs_[0], &num)) return false;

    if (! readMaterial(num))
      return false;
  }

  return true;
}

bool
CImportCOB::
readMaterial(int num)
{
  bool found = false;

  pattern_ = "rgb \\(.*\\),\\(.*\\),\\(.*\\)";

  CRGBA rgb;

  while (readNextLine()) {
    if (! CRegExpUtil::parse(line_, pattern_, match_strs_))
      continue;

    double r, g, b;

    if (! CStrUtil::toReal(match_strs_[0], &r)) return false;
    if (! CStrUtil::toReal(match_strs_[1], &g)) return false;
    if (! CStrUtil::toReal(match_strs_[2], &b)) return false;

    rgb.setRGB(r, g, b);

    found = true;

    break;
  }

  if (! found)
    return false;

  double alpha     = 1.0;
  double shininess = 0.0;

  CRGBA ambient, diffuse, specular, emission;

  emission.setRGB(0,0,0);

  found = false;

  pattern_ = "alpha \\(.*\\) ka \\(.*\\) ks \\(.*\\) exp \\(.*\\) ior \\(.*\\)";

  while (readNextLine()) {
    if (! CRegExpUtil::parse(line_, pattern_, match_strs_))
      continue;

    double ka, ks, exp;

    if (! CStrUtil::toReal(match_strs_[0], &alpha)) return false;
    if (! CStrUtil::toReal(match_strs_[1], &ka   )) return false;
    if (! CStrUtil::toReal(match_strs_[2], &ks   )) return false;
    if (! CStrUtil::toReal(match_strs_[3], &exp  )) return false;

    ambient   = rgb*ka;
    diffuse   = rgb;
    specular  = rgb*ks;
    shininess = exp;

    found = true;

    break;
  }

  if (! found)
    return false;

  found = false;

  pattern_ = "Shader class: \\(.*\\)";

  while (readNextLine()) {
    if (! CRegExpUtil::parse(line_, pattern_, match_strs_))
      continue;

    if (match_strs_[0] == "color") {
      found = true;

      break;
    }
  }

  if (! found)
    return false;

  pattern_ = "Shader name: \"\\(.*\\)\" (\\(.*\\))";

  while (readNextLine()) {
    if (! CRegExpUtil::parse(line_, pattern_, match_strs_))
      continue;

    found = true;

    break;
  }

  if (! found)
    return false;

  CShadeType3D shade_type = CSHADE_TYPE_3D_CONSTANT;

  std::string fileName = "";

  if (match_strs_[0] == "texture map") {
    found = false;

    pattern_ = "file name: string \"\\(.*\\)\"";

    while (readNextLine()) {
      if (! CRegExpUtil::parse(line_, pattern_, match_strs_))
        continue;

      found = true;

      break;
    }

    if (! found)
      return false;

    std::string::size_type p = match_strs_[0].rfind('/');

    if (p == std::string::npos)
      p = match_strs_[0].rfind('\\');

    if (p == std::string::npos)
      fileName = match_strs_[0];
    else
      fileName = match_strs_[0].substr(p + 1);

    shade_type = CSHADE_TYPE_3D_TEXTURE;
  }

  pattern_ = "Shader class: \\(.*\\)";

  while (readNextLine()) {
    if (! CRegExpUtil::parse(line_, pattern_, match_strs_))
      continue;

    if (match_strs_[0] == "reflectance") {
      found = true;

      break;
    }
  }

  if (! found)
    return false;

  pattern_ = "Shader name: \"\\(.*\\)\" (\\(.*\\))";

  while (readNextLine()) {
    if (! CRegExpUtil::parse(line_, pattern_, match_strs_))
      continue;

    found = true;

    break;
  }

  if (! found)
    return false;

  if (shade_type != CSHADE_TYPE_3D_TEXTURE) {
    if      (match_strs_[0] == "constant")
      shade_type = CSHADE_TYPE_3D_CONSTANT;
    else if (match_strs_[0] == "matte")
      shade_type = CSHADE_TYPE_3D_FLAT;
    else if (match_strs_[0] == "plastic")
      shade_type = CSHADE_TYPE_3D_GOURAUD;
    else if (match_strs_[0] == "phong")
      shade_type = CSHADE_TYPE_3D_PHONG;
  }

  setFacesMaterial(num, shade_type, ambient, diffuse, specular, emission, shininess, fileName);

  return true;
}

bool
CImportCOB::
readNextLine()
{
  char c;

  while (true) {
    if (! file_->readLine(line_))
      return false;

    CStrParse parse(line_);

    parse.skipSpace();

    if (! parse.readChar(&c))
      continue;

    if (c == '#')
      continue;

    break;
  }

  return true;
}
