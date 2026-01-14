#include <CImportPly.h>
#include <CGeometry3D.h>

namespace {

struct Line {
  std::string line;
  size_t      pos { 0 };

  bool getWord(std::string &word) {
    word = "";

    auto len = line.size();

    while (pos < len && isspace(line[pos]))
      ++pos;

    if (pos >= len)
      return false;

    while (pos < len && ! isspace(line[pos]))
      word += line[pos++];

    return true;
  }
};

bool readLine(CFile *file, Line &line) {
  if (file->eof())
    return false;

  line.line = "";
  line.pos  = 0;

  while (! file->eof()) {
    auto c = file->getC();

    if (c == '\n')
      break;

    line.line += char(c);
  }

  return true;
}

}

//---

bool
CImportPly::
stringToFormat(const std::string &format, FormatType &formatType)
{
  if      (format == "char"   || format == "int8")
    formatType = FormatType::CHAR;
  else if (format == "uchar"  || format == "uint8")
    formatType = FormatType::UCHAR;
  else if (format == "short"  || format == "int16")
    formatType = FormatType::SHORT;
  else if (format == "ushort" || format == "uint16")
    formatType = FormatType::USHORT;
  else if (format == "int"    || format == "int3s")
    formatType = FormatType::INT;
  else if (format == "uint"   || format == "uint3s")
    formatType = FormatType::UINT;
  else if (format == "float"  || format == "float32")
    formatType = FormatType::FLOAT;
  else if (format == "double" || format == "float64")
    formatType = FormatType::DOUBLE;
  else
    return false;

  return true;
}

CImportPly::
CImportPly(CGeomScene3D *scene, const std::string &name) :
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

CImportPly::
~CImportPly()
{
}

bool
CImportPly::
read(CFile &file)
{
  file_ = &file;

  //---

  Line line;

  if (! readLine(file_, line) || line.line != "ply") {
    std::cerr << "Not a ply file\n";
    return false;
  }

  std::string elementName;

  bool in_header = true;
  bool ascii     = false;

  big_endian_ = false;

  while (readLine(file_, line)) {
    std::string word;

    if (line.getWord(word)) {
      if      (word == "comment") {
        // skip
      }
      else if (word == "format") {
        std::string word1;

        if (line.getWord(word1)) {
          if      (word1 == "ascii")
            ascii = true;
          else if (word1 == "binary_little_endian") {
            ascii       = false;
            big_endian_ = false;
          }
          else if (word1 == "binary_big_endian") {
            ascii       = false;
            big_endian_ = true;
          }
          else {
            std::cerr << "Invalid format: " << word1 << "\n";
            return false;
          }
        }
      }
      else if (word == "element") {
        std::string word1;

        if (line.getWord(word1)) {
          elementName = word1;

          elementNames_.push_back(elementName);

          std::string word2;

          if (line.getWord(word2)) {
            //std::cerr << "      " << word2 << "\n";

            int count = std::stoi(word2);

            elementCount_[elementName] = count;

            if      (word1 == "vertex") {
            }
            else if (word1 == "face") {
            }
            else if (word1 == "multi_texture_vertex") {
              std::cerr << "TODO: " << word1 << "\n";
            }
            else if (word1 == "multi_texture_face") {
              std::cerr << "TODO: " << word1 << "\n";
            }
            else {
              std::cerr << "Unhandled element : " << word1 << "\n";
              return false;
            }
          }
          else {
            std::cerr << "Invalid element: " << line.line << "\n";
            return false;
          }
        }
      }
      else if (word == "property") {
        auto &properties = getElementProperties(elementName);

        std::string type;

        if (line.getWord(type)) {
          //std::cerr << "    " << type << "\n";

          if (type == "list") {
            std::string type1, type2;

            if (line.getWord(type1) && line.getWord(type2)) {
              std::string name;

              FormatType format1 { FormatType::NONE };
              if (! CImportPly::stringToFormat(type1, format1)) {
                std::cerr << "Invalid format : " << type1 << "\n";
                return false;
              }

              FormatType format2 { FormatType::NONE };
              if (! CImportPly::stringToFormat(type2, format2)) {
                std::cerr << "Invalid format : " << type2 << "\n";
                return false;
              }

              if (line.getWord(name)) {
                //std::cerr << "      " << name << "\n";

                auto ltype = type + ":" + type1 + ":" + type2;

                properties.emplace_back(true, name, ltype, format1, format2);
              }
            }
          }
          else {
            FormatType format { FormatType::NONE };
            if (! CImportPly::stringToFormat(type, format)) {
              std::cerr << "Invalid format : " << type << "\n";
              return false;
            }

            if      (format == FormatType::UCHAR ||
                     format == FormatType::UINT  ||
                     format == FormatType::FLOAT) {
              std::string name;

              if (line.getWord(name)) {
                //std::cerr << "      " << name << "\n";

                properties.emplace_back(name, type, format);
              }
            }
            else {
              std::cerr << "Unhandled property format : " << type << "\n";
              return false;
            }
          }
        }
      }
      else if (word == "end_header") {
        in_header = false;
        break;
      }
      else {
        std::cerr << "Unhandled line: " << line.line << "\n";
        return false;
      }
    }
  }

  if (in_header) {
    std::cerr << "No end_header\n";
    return false;
  }

  bool rc;

  if (ascii)
    rc = readAscii();
  else
    rc = readBinary();

  return rc;
}

bool
CImportPly::
readAscii()
{
  const auto &vertexProperties = getElementProperties("vertex");

  auto numVertexProperties = vertexProperties.size();

#if 0
  for (const auto &p : vertexProperties) {
    std::cerr << "V: " << p.name << " : " << p.type << "\n";
  }
#endif

#if 0
  const auto &faceProperties = getElementProperties("face");

  for (const auto &p : faceProperties) {
    std::cerr << "F: " << p.name << " : " << p.type << "\n";
  }
#endif

  auto num_vertices = elementCount_["vertex"];

  //std::cerr << "num_vertices: " << num_vertices << "\n";

  for (int i = 0; i < num_vertices; ++i) {
    Line line1;

    if (! readLine(file_, line1)) {
      std::cerr << "Missing vertex line\n";
      return false;
    }

    std::vector<std::string> words;

    std::string word1;

    while (line1.getWord(word1))
      words.push_back(word1);

    if (words.size() != numVertexProperties) {
      std::cerr << "Bad vertex properties: " <<
        words.size() << " != " << numVertexProperties << "\n";
      return false;
    }

    Vertex v;

    int iw = 0;

    for (const auto &p : vertexProperties) {
      const auto &word = words[iw++];

      if      (p.name == "x")
        v.x = std::stod(word);
      else if (p.name == "y")
        v.y = std::stod(word);
      else if (p.name == "z")
        v.z = std::stod(word);
      else if (p.name == "r")
        v.r = std::stod(word);
      else if (p.name == "g")
        v.g = std::stod(word);
      else if (p.name == "b")
        v.b = std::stod(word);
      else if (p.name == "a")
        v.a = std::stod(word);
      else if (p.name == "nx")
        v.nx = std::stod(word);
      else if (p.name == "ny")
        v.ny = std::stod(word);
      else if (p.name == "nz")
        v.nz = std::stod(word);
      else if (p.name == "s" || p.name == "u")
        v.s = std::stod(word);
      else if (p.name == "t" || p.name == "v")
        v.t = std::stod(word);
      else {
        std::cerr << "Bad vertex name : " << p.name << "\n";
        return false;
      }
    }

    vertices_.push_back(v);
  }

  //---

  auto num_faces = elementCount_["face"];

  //std::cerr << "num_faces: " << num_faces << "\n";

  for (int i = 0; i < num_faces; ++i) {
    Line line1;

    if (! readLine(file_, line1)) {
      std::cerr << "Missing face line\n";
      return false;
    }

    // TODO: face properties ?

    std::string word1;

    if (! line1.getWord(word1)) {
      std::cerr << "Bad face line: " << line1.line << "\n";
      return false;
    }

    int ni = std::stoi(word1);

    std::vector<std::string> words;

    while (line1.getWord(word1))
      words.push_back(word1);

    if (int(words.size()) != ni) {
      std::cerr << "Bad face line: " << line1.line << "\n";
      return false;
    }

    Face f;

    for (const auto &w : words) {
      int vi = std::stoi(w);

      if (vi < 0 || vi >= num_vertices) {
        std::cerr << "Bad vertex index: " << vi << "\n";
        return false;
      }

      f.inds.push_back(vi);
    }

    faces_.push_back(f);
  }

  //---

  int i = 0;

  for (const auto &v : vertices_) {
    int i1 = object_->addVertex(CPoint3D(v.x, v.y, v.z));
    assert(i == i1);

    double a = v.a;

    if (a < 0.0)
      a = 1.0;

    object_->setVertexColor(i, CRGBA(v.r, v.g, v.b, a));

    object_->setVertexNormal(i, CVector3D(v.nx, v.ny, v.nz));

    object_->setVertexTextureMap(i, CPoint2D(v.s, v.t));

    ++i;
  }

  for (const auto &f : faces_) {
    auto ni = f.inds.size();

    if (ni != 3) {
      std::cerr << "Invalid face vertex count\n";
      continue;
    }

    (void) object_->addITriangle(f.inds[0], f.inds[1], f.inds[2]);
  }

  return true;
}

bool
CImportPly::
readBinary()
{
  const auto &vertexProperties = getElementProperties("vertex");

#if 0
  for (const auto &p : vertexProperties) {
    std::cerr << "V: " << p.name << " : " << p.type << "\n";
  }
#endif

  auto num_vertices = elementCount_["vertex"];
//std::cerr << "num_vertices: " << num_vertices << "\n";

  for (int i = 0; i < num_vertices; ++i) {
    Vertex v;

    for (const auto &p : vertexProperties) {
      float f;

      if (p.type == FormatType::FLOAT) {
        readFloat(&f);
      }
      else {
        std::cerr << "Bad vertex format : " << p.typeStr << "\n";
        continue;
      }

      if      (p.name == "x")
        v.x = f;
      else if (p.name == "y")
        v.y = f;
      else if (p.name == "z")
        v.z = f;
      else if (p.name == "r")
        v.r = f;
      else if (p.name == "g")
        v.g = f;
      else if (p.name == "b")
        v.b = f;
      else if (p.name == "a")
        v.a = f;
      else if (p.name == "nx")
        v.nx = f;
      else if (p.name == "ny")
        v.ny = f;
      else if (p.name == "nz")
        v.nz = f;
      else if (p.name == "s" || p.name == "u")
        v.s = f;
      else if (p.name == "t" || p.name == "v")
        v.t = f;
      else {
        std::cerr << "Bad vertex name : " << p.name << "\n";
        return false;
      }
    }

    vertices_.push_back(v);
  }

  //---

#if 0
  const auto &faceProperties = getElementProperties("face");

  for (const auto &p : faceProperties) {
    std::cerr << "F: " << p.name << " : " << p.type << "\n";
  }
#endif

  auto num_faces = elementCount_["face"];
//std::cerr << "num_faces: " << num_faces << "\n";

  for (int i = 0; i < num_faces; ++i) {
    // TODO: face properties ?

    uchar ni;
    readUChar(&ni);

    Face f;

    for (int i1 = 0; i1 < ni; ++i1) {
      int vi;
      readInt(&vi);

      if (vi < 0 || vi >= num_vertices) {
        std::cerr << "Bad vertex index: " << vi << "\n";
        return false;
      }

      f.inds.push_back(vi);
    }

    faces_.push_back(f);
  }

  //---

  auto num_texture_vertices = elementCount_["multi_texture_vertex"];

  for (int i = 0; i < num_texture_vertices; ++i) {
    // TODO: properties ?

    TexturePoint tp;

    readUChar(&tp.tx);
    readFloat(&tp.u);
    readFloat(&tp.v);

    texturePoints_.push_back(tp);
  }

  //---

  auto num_texture_faces = elementCount_["multi_texture_face"];

  for (int i = 0; i < num_texture_faces; ++i) {
    // TODO: properties ?

    TextureFace tf;

    readUChar(&tf.tx);
    readUInt (&tf.tn);

    uchar ni;
    readUChar(&ni);

    for (int i1 = 0; i1 < ni; ++i1) {
      int vi;
      readInt(&vi);

      if (vi < 0 || vi >= num_texture_vertices) {
        std::cerr << "Bad texture vertex index: " << vi << "\n";
        return false;
      }

      tf.inds.push_back(vi);
    }

    textureFaces_.push_back(tf);
  }

  //---

  int i = 0;

  for (const auto &v : vertices_) {
    int i1 = object_->addVertex(CPoint3D(v.x, v.y, v.z));
    assert(i == i1);

    double a = v.a;

    if (a < 0.0)
      a = 1.0;

    object_->setVertexColor(i, CRGBA(v.r, v.g, v.b, a));

    object_->setVertexNormal(i, CVector3D(v.nx, v.ny, v.nz));

    object_->setVertexTextureMap(i, CPoint2D(v.s, v.t));

    ++i;
  }

  bool has_texture_points = (num_texture_faces == num_faces);

  i = 0;

  for (const auto &f : faces_) {
    auto ni = f.inds.size();

    if (ni != 3) {
      std::cerr << "Invalid face vertex count\n";
      continue;
    }

    auto faceInd = object_->addITriangle(f.inds[0], f.inds[1], f.inds[2]);

    if (has_texture_points) {
      auto &face = object_->getFace(faceInd);

      const auto &tf = textureFaces_[i];

      std::vector<CPoint2D> tpoints;

      for (const auto &ti : tf.inds) {
        const auto &tp = texturePoints_[ti];

        tpoints.push_back(CPoint2D(tp.u, tp.v));
      }

      face.setTexturePoints(tpoints);
    }

    ++i;
  }

  return true;
}

CImportPly::Properties &
CImportPly::
getElementProperties(const std::string &name)
{
  auto pe = elementProperties_.find(name);

  if (pe == elementProperties_.end())
    pe = elementProperties_.insert(pe, ElementProperties::value_type(name, Properties()));

  return (*pe).second;
}

bool
CImportPly::
readFloat(float *f) const
{
  CFileData file_data(sizeof(float));

  try {
    file_->read(&file_data);
  }
  catch (...) {
    return false;
  }

  auto *data = file_data.getData();

  uint fi;

  if (big_endian_)
    fi = ((data[3] & 0xFF)      ) |
         ((data[2] & 0xFF) <<  8) |
         ((data[1] & 0xFF) << 16) |
         ((data[0] & 0xFF) << 24);
  else
    fi = ((data[0] & 0xFF)      ) |
         ((data[1] & 0xFF) <<  8) |
         ((data[2] & 0xFF) << 16) |
         ((data[3] & 0xFF) << 24);

  *f = *reinterpret_cast<float *>(&fi);

//std::cerr << "f: " << *f << "\n";

  return true;
}

bool
CImportPly::
readInt(int *i) const
{
  CFileData file_data(sizeof(int));

  try {
    file_->read(&file_data);
  }
  catch (...) {
    return false;
  }

  auto *data = file_data.getData();

  uint ii;

  if (big_endian_)
    ii = ((data[3] & 0xFF)      ) |
         ((data[2] & 0xFF) <<  8) |
         ((data[1] & 0xFF) << 16) |
         ((data[0] & 0xFF) << 24);
  else
    ii = ((data[0] & 0xFF)      ) |
         ((data[1] & 0xFF) <<  8) |
         ((data[2] & 0xFF) << 16) |
         ((data[3] & 0xFF) << 24);

  *i = *reinterpret_cast<int *>(&ii);

//std::cerr << "i: " << *i << "\n";

  return true;
}

bool
CImportPly::
readUInt(uint *i) const
{
  CFileData file_data(sizeof(int));

  try {
    file_->read(&file_data);
  }
  catch (...) {
    return false;
  }

  auto *data = file_data.getData();

  uint ii;

  if (big_endian_)
    ii = ((data[3] & 0xFF)      ) |
         ((data[2] & 0xFF) <<  8) |
         ((data[1] & 0xFF) << 16) |
         ((data[0] & 0xFF) << 24);
  else
    ii = ((data[0] & 0xFF)      ) |
         ((data[1] & 0xFF) <<  8) |
         ((data[2] & 0xFF) << 16) |
         ((data[3] & 0xFF) << 24);

  *i = *reinterpret_cast<uint *>(&ii);

//std::cerr << "i: " << *i << "\n";

  return true;
}

bool
CImportPly::
readUChar(uchar *i) const
{
  CFileData file_data(sizeof(char));

  try {
    file_->read(&file_data);
  }
  catch (...) {
    return false;
  }

  auto *data = file_data.getData();

  *i = uchar(data[0] & 0xFF);

//std::cerr << "i: " << *i << "\n";

  return true;
}
