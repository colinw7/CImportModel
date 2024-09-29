#include <CImportPly.h>
#include <CGeometry3D.h>

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
    scene_  = CGeometryInst->createScene3D();
    pscene_ = scene_;
  }

  object_ = CGeometryInst->createObject3D(scene_, name);

  scene_->addObject(object_);

  if (! pobject_)
    pobject_ = object_;
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

  auto readLine = [&](Line &line) {
    if (file.eof())
      return false;

    line.line = "";
    line.pos  = 0;

    while (! file.eof()) {
      auto c = file.getC();

      if (c == '\n')
        break;

      line.line += char(c);
    }

    return true;
  };

  //---

  struct Property {
    std::string name;
    std::string type;

    Property() { }

    Property(const std::string &n, const std::string &t) :
     name(n), type(t) {
    }
  };

  using Properties = std::vector<Property>;

  Properties vertexProperties, faceProperties;

  //---

  Line line;

  if (! readLine(line) || line.line != "ply") {
    std::cerr << "Not a ply file\n";
    return false;
  }

  bool in_header    = true;
  bool in_vertex    = false;
  bool in_face      = false;
  bool ascii        = false;
  int  num_vertices = 0;
  int  num_faces    = 0;

  while (readLine(line)) {
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
          else if (word1 == "binary_little_endian")
            ascii = false;
          else if (word1 == "binary_big_endian")
            ascii = false;
          else
            std::cerr << "Invalid format: " << word1 << "\n";
        }
      }
      else if (word == "element") {
        std::string word1;

        if (line.getWord(word1)) {
          if      (word1 == "vertex") {
            in_vertex = true;
            in_face   = false;

            std::string word2;

            if (line.getWord(word2)) {
              //std::cerr << "      " << word2 << "\n";

              num_vertices = std::stoi(word2);
            }
          }
          else if (word1 == "face") {
            in_vertex = false;
            in_face   = true;

            std::string word2;

            if (line.getWord(word2)) {
              //std::cerr << "      " << word2 << "\n";

              num_faces = std::stoi(word2);
            }
          }
          else {
            std::cerr << "Unhandled element : " << word1 << "\n";
            return false;
          }
        }
      }
      else if (word == "property") {
        std::string type;

        if (line.getWord(type)) {
          //std::cerr << "    " << type << "\n";

          if (type == "list") {
            std::string type1, type2;

            if (line.getWord(type1) && line.getWord(type2)) {
              std::string name;

              if (line.getWord(name)) {
                //std::cerr << "      " << name << "\n";

                auto ltype = type + ":" + type1 + ":" + type2;

                if      (in_vertex)
                  vertexProperties.emplace_back(name, ltype);
                else if (in_face)
                  faceProperties.emplace_back(name, ltype);
              }
            }
          }
          else {
            FormatType format { FormatType::NONE };

            if (! CImportPly::stringToFormat(type, format)) {
              std::cerr << "Invalid format : " << type << "\n";
              return false;
            }

            if      (format == FormatType::FLOAT) {
              std::string name;

              if (line.getWord(name)) {
                //std::cerr << "      " << name << "\n";

                if      (in_vertex)
                  vertexProperties.emplace_back(name, type);
                else if (in_face)
                  faceProperties.emplace_back(name, type);
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
      }
    }
  }

  if (in_header) {
    std::cerr << "No end_header\n";
    return false;
  }

  if (! ascii) {
    std::cerr << "File must be ascii\n";
    return false;
  }

  auto numVertexProperties = vertexProperties.size();

#if 0
  for (const auto &p : vertexProperties) {
    std::cerr << "V: " << p.name << " : " << p.type << "\n";
  }

  for (const auto &p : faceProperties) {
    std::cerr << "F: " << p.name << " : " << p.type << "\n";
  }
#endif

  //std::cerr << "num_vertices: " << num_vertices << "\n";

  for (int i = 0; i < num_vertices; ++i) {
    Line line1;

    if (! readLine(line1)) {
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
      else
        std::cerr << "Bad vertex name : " << p.name << "\n";
    }

    vertices_.push_back(v);
  }

  //std::cerr << "num_faces: " << num_faces << "\n";

  for (int i = 0; i < num_faces; ++i) {
    Line line1;

    if (! readLine(line1)) {
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
        std::cerr << "Bad face index: " << vi << "\n";
        return false;
      }

      f.inds.push_back(vi);
    }

    faces_.push_back(f);
  }

  int i = 0;

  for (const auto &v : vertices_) {
    int i1 = object_->addVertex(CPoint3D(v.x, v.y, v.z));
    assert(i == i1);

    double a = v.a;

    if (a < 0.0)
      a = 1.0;

    object_->setVertexColor(i, CRGBA(v.r, v.g, v.b, a));

    object_->setVertexNormal(i, CVector3D(v.nx, v.ny, v.nz));

    object_->setVertexTextureMap(i, CPoint3D(v.s, v.t, 0.0));

    ++i;
  }

  for (const auto &f : faces_) {
    auto ni = f.inds.size();

    if (ni == 3)
      object_->addITriangle(f.inds[0], f.inds[1], f.inds[2]);
  }

  return true;
}
