#include <CImportSTL.h>
#include <CGeometry3D.h>
#include <CStrParse.h>

CImportSTL::
CImportSTL(CGeomScene3D *scene, const std::string &name) :
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

CImportSTL::
~CImportSTL()
{
}

bool
CImportSTL::
read(CFile &file)
{
  file_ = &file;

  //---

  uchar header[81];

  if (! file.read(header, 80))
    return false;

  header[80] = '\0';

  //if (header[80]) return false;

  if (strncmp(reinterpret_cast<char *>(&header[0]), "solid ", 6) == 0)
    return readAscii();
  else
    return readBinary();
}

bool
CImportSTL::
readBinary()
{
  file_->rewind();

  uchar header[81];

  if (! file_->read(header, 80))
    return false;

  header[80] = '\0';

  if (isDebug())
    std::cerr << "Header: " << header << "\n";

  //---

  auto readShort = [&](ushort *integer) {
    uchar buffer[2];

    if (! file_->read(buffer, 2))
      return false;

    *integer = ((buffer[0] & 0xFF)     ) |
               ((buffer[1] & 0xFF) << 8);
/*
    *integer = ((buffer[1] & 0xFF)     ) |
               ((buffer[0] & 0xFF) << 8);
*/
    return true;
  };

  auto readInteger = [&](uint *integer) {
    uchar buffer[4];

    if (! file_->read(buffer, 4))
      return false;

    *integer = ((buffer[0] & 0xFF)      ) |
               ((buffer[1] & 0xFF) <<  8) |
               ((buffer[2] & 0xFF) << 16) |
               ((buffer[3] & 0xFF) << 24);
/*
    *integer = ((buffer[3] & 0xFF)      ) |
               ((buffer[2] & 0xFF) <<  8) |
               ((buffer[1] & 0xFF) << 16) |
               ((buffer[0] & 0xFF) << 24);
*/
    return true;
  };

  auto readFloat = [&](float *real) {
#if 0
    uchar buffer[4];

    if (! file_->read(buffer, 4))
      return false;

    *real = ((buffer[0] & 0xFF)      ) |
            ((buffer[1] & 0xFF) <<  8) |
            ((buffer[2] & 0xFF) << 16) |
            ((buffer[3] & 0xFF) << 24);
/*
    *real = ((buffer[3] & 0xFF)      ) |
            ((buffer[2] & 0xFF) <<  8) |
            ((buffer[1] & 0xFF) << 16) |
            ((buffer[0] & 0xFF) << 24);
*/
    return true;
#else
    float f;

    if (! file_->read(reinterpret_cast<uchar *>(&f), 4))
      return false;

    *real = f;

    return true;
#endif
  };

  auto readVector = [&](Vector &v) {
    if (! readFloat(&v.x) || ! readFloat(&v.y) || ! readFloat(&v.z))
      return false;

//  v.x = std::abs(v.x);
//  v.y = std::abs(v.y);
//  v.z = std::abs(v.z);

    return true;
  };

  auto readTriangle = [&](Triangle &t) {
    if (! readVector(t.normal)) return false;

    if (! readVector(t.p1)) return false;
    if (! readVector(t.p2)) return false;
    if (! readVector(t.p3)) return false;

    return true;
  };

#if 0
  auto printVector = [&](const Vector &v) {
    std::cerr << v.x << " " << v.y << " " << v.z << "\n";
  };

  auto printTriangle = [&](const Triangle &t) {
    printVector(t.normal);
    printVector(t.p1);
    printVector(t.p2);
    printVector(t.p3);
  };
#endif

  uint n = 0;
  if (! readInteger(&n))
    return false;

  if (isDebug())
    std::cerr << "Num Objects: " << n << "\n";

  triangles_.resize(n);

  for (uint i = 0; i < n; ++i) {
    if (! readTriangle(triangles_[i]))
      return false;

    ushort na;
    if (! readShort(&na))
      return false;

//  if (na > 0)
//    std::cerr << "NA: " << na << "\n";

//  printTriangle(triangles_[i]);
  }

  minV_ = Vector();
  maxV_ = Vector();

  uint i = 0;

  for (const auto &t : triangles_) {
    zvals_.insert(t.p1.z);

    if (i == 0) {
      minV_ = t.p1;
      maxV_ = t.p1;
    }
    else {
      auto updateRange = [&](const Vector &v) {
        minV_.x = std::min(minV_.x, v.x);
        minV_.y = std::min(minV_.y, v.y);
        minV_.z = std::min(minV_.z, v.z);

        maxV_.x = std::max(maxV_.x, v.x);
        maxV_.y = std::max(maxV_.y, v.y);
        maxV_.z = std::max(maxV_.z, v.z);
      };

      updateRange(t.p1);
      updateRange(t.p2);
      updateRange(t.p3);
    }

    ++i;
  }

  if (isDebug()) {
    std::cerr << "X Range: " << minV_.x << " " << maxV_.x << "\n";
    std::cerr << "Y Range: " << minV_.y << " " << maxV_.y << "\n";
    std::cerr << "Z Range: " << minV_.z << " " << maxV_.z << "\n";
  }

  for (const auto &t : triangles_) {
    int i1 = object_->addVertex(CPoint3D(t.p1.x, t.p1.y, t.p1.z));
    int i2 = object_->addVertex(CPoint3D(t.p2.x, t.p2.y, t.p2.z));
    int i3 = object_->addVertex(CPoint3D(t.p3.x, t.p3.y, t.p3.z));

    auto faceId = object_->addITriangle(i1, i2, i3);

    auto &face = object_->getFace(faceId);

    face.setNormal(CVector3D(t.normal.x, t.normal.y, t.normal.z));
  }

  return true;
}

bool
CImportSTL::
readAscii()
{
  file_->rewind();

  //---

  auto readLine = [&](std::string &line) {
    line = "";

    if (file_->eof())
      return false;

    while (! file_->eof()) {
      auto c = file_->getC();

      if (c == EOF) {
        if (line == "")
          return false;

        break;
      }

      if (c == '\n')
        break;

      line += char(c);
    }

    return true;
  };

  auto errorMsg = [&](const std::string &msg) {
    std::cerr << msg << "\n";
    return false;
  };

  enum class State {
    NONE,
    SOLID,
    FACET,
    LOOP
  };

  //---

  auto state = State::NONE;

  std::string line;

  CVector3D        normal;
  std::vector<int> vertices;

  while (readLine(line)) {
    if (isDebug())
      std::cerr << line << "\n";

    CStrParse parse(line);

    parse.skipSpace();

    if      (state == State::NONE) {
      if (parse.isString("solid "))
        state = State::SOLID;
      else
        return errorMsg("Invalid line '" + line + "'");
    }
    else if (state == State::SOLID) {
      if      (parse.isString("facet ")) {
        parse.skipChars("facet");
        parse.skipSpace();

        if (parse.isString("normal ")) {
          parse.skipChars("normal");

          double x, y, z;

          parse.skipSpace();
          if (! parse.readReal(&x))
            return errorMsg("Invalid real for x");

          parse.skipSpace();
          if (! parse.readReal(&y))
            return errorMsg("Invalid real for y");

          parse.skipSpace();
          if (! parse.readReal(&z))
            return errorMsg("Invalid real for z");

          normal = CPoint3D(x, y, z);
        }
        else
          normal = CPoint3D(0, 0, 1);

        vertices.clear();

        state = State::FACET;
      }
      else if (parse.isString("endsolid"))
        state = State::NONE;
      else
        return errorMsg("Invalid line '" + line + "'");
    }
    else if (state == State::FACET) {
      if      (parse.isString("outer loop"))
        state = State::LOOP;
      else if (parse.isString("endfacet"))
        state = State::SOLID;
      else
        return errorMsg("Invalid line '" + line + "'");
    }
    else if (state == State::LOOP) {
      if      (parse.isString("endloop")) {
        if (vertices.size() != 3)
          return errorMsg("Invalid number of vertices");

        auto faceId = object_->addITriangle(vertices[0], vertices[1], vertices[2]);

        auto &face = object_->getFace(faceId);

        face.setNormal(normal);

        state = State::FACET;
      }
      else if (parse.isString("vertex ")) {
        parse.skipChars("vertex");

        double x, y, z;

        parse.skipSpace();
        if (! parse.readReal(&x))
          return errorMsg("Invalid real for x");

        parse.skipSpace();
        if (! parse.readReal(&y))
          return errorMsg("Invalid real for y");

        parse.skipSpace();
        if (! parse.readReal(&z))
          return errorMsg("Invalid real for z");

        int iv = object_->addVertex(CPoint3D(x, y, z));

        vertices.push_back(iv);
      }
      else
        return errorMsg("Invalid line '" + line + "'");
    }
    else
      return errorMsg("Invalid line '" + line + "'");
  }

  return true;
}
