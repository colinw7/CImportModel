#include <CImportBlend.h>
#include <CGeometry3D.h>
#include <CGeomScene3D.h>

CImportBlend::
CImportBlend(CGeomScene3D *scene, const std::string &name) :
 scene_(scene)
{
  if (! scene_) {
    scene_  = CGeometry3DInst->createScene3D();
    pscene_ = SceneP(scene_);
  }

  auto name1 = name;

  if (name1 == "")
    name1 = "blend";

  object_  = CGeometry3DInst->createObject3D(scene_, name);
  pobject_ = ObjectP(object_);

  scene_->addObject(object_);
}

CImportBlend::
~CImportBlend()
{
}

bool
CImportBlend::
read(CFile &file)
{
  file_ = &file;

  uchar header[13];

  if (! file_->read(header, 12))
    return false;

  header[13] = '\0';

  if (strncmp(reinterpret_cast<char *>(&header[0]), "BLENDER", 6) != 0) {
    std::cerr << "Invalid header\n";
    return false;
  }

  if      (header[7] == '_')
    pointerSize_ = 4;
  else if (header[7] == '-')
    pointerSize_ = 8;
  else
    return false;

  if      (header[8] == 'v')
    littleEndian_ = true;
  else if (header[8] == 'V')
    littleEndian_ = false;
  else
    return false;

  //---

  // read DNA
  char code[5];

  while (! file_->eof()) {
    if (! readBlock(code))
      return false;

    if (strncmp(&code[0], "ENDB", 4) == 0)
      break;
  }

  //---

  file_->rewind();

  if (! file_->read(header, 12))
    return false;

  //---

  // process data using DNA
  while (! file_->eof()) {
    if (! processBlock(code))
      return false;

    if (strncmp(&code[0], "ENDB", 4) == 0)
      break;
  }

  //---

  // add mesh geonetry
  int i = 0;

  for (auto *mesh : meshes_) {
    auto meshName = "mesh." + std::to_string(i + 1);

    auto *object1 = CGeometry3DInst->createObject3D(scene_, meshName);

    uint np = uint(mesh->points.size());

    std::vector<int> vinds;

    for (const auto &p : mesh->points) {
      auto vind = object1->addVertex(p);

      vinds.push_back(vind);
    }

    //auto nv = object1->getNumVertices();

    bool hasUV = (mesh->mloopUV.size() == mesh->mloop.size());

    if (! mesh->mpoly.empty() || ! mesh->mface.empty()) {
      for (const auto &pp : mesh->mpoly) {
        if (uint(pp.loopstart + pp.totloop) > uint(mesh->mloop.size())) {
          std::cerr << "Bad poly loop\n";
          continue;
        }

        if (pp.totloop < 3) {
          std::cerr << "Bad polygon size " << pp.totloop << "\n";
          continue;
        }

        auto *mloop = &mesh->mloop[pp.loopstart];

        std::vector<uint> inds;

        inds.resize(pp.totloop);

        for (int ii = 0; ii < pp.totloop; ++ii) {
          inds[ii] = vinds[mloop[ii].v];

          if (inds[ii] >= np) {
            std::cerr << "Bad poly loop vertex " << inds[ii] << "\n";
            break;
          }
        }

        auto faceId = object1->addIPolygon(&inds[0], pp.totloop);

        if (hasUV) {
          auto *mloopUV = &mesh->mloopUV[pp.loopstart];

          std::vector<CPoint2D> points;

          points.push_back(CPoint2D(mloopUV[0].uv[0], mloopUV[0].uv[1]));
          points.push_back(CPoint2D(mloopUV[1].uv[0], mloopUV[1].uv[1]));
          points.push_back(CPoint2D(mloopUV[2].uv[0], mloopUV[2].uv[1]));

          auto *face = object1->getFaceP(faceId);

          face->setTexturePoints(points);
        }
      }

      for (const auto &pf : mesh->mface) {
        if (uint(pf.v1) >= np || uint(pf.v2) >= np || uint(pf.v3) >= np) {
          std::cerr << "Bad poly loop vertices\n";
          continue;
        }

        uint i1 = vinds[pf.v1];
        uint i2 = vinds[pf.v2];
        uint i3 = vinds[pf.v3];

        object1->addITriangle(i1, i2, i3);
      }
    }
    else {
      std::cerr << "No faces\n";
    }

    scene_->addObject(object1);

    pobject_->addChild(object1);

    ++i;
  }

  return true;
}

bool
CImportBlend::
readBlock(char *code)
{
  uchar header[25];

  if (pointerSize_ == 4) {
    if (! file_->read(header, 20))
      return false;
  }
  else {
    if (! file_->read(header, 24))
      return false;
  }

  memcpy(&code[0], &header[0], 4); // 0-3
  code[4] = '\0';

  auto size  = decodeInteger(&header[4]); // 4-7
  // pointer (8-11 or 8-15)
//auto index = decodeInteger(&header[pointerSize_ == 4 ? 12 : 16]); // 12-15 or 16-19
//auto count = decodeInteger(&header[pointerSize_ == 4 ? 16 : 20]); // 16-19 or 20-23

#if 0
  std::cerr << "Code:" << code << " Size:" << size <<
               " Index: " << index << " Count: " << count << "\n";
#endif

  if (size > 0) {
    if (bufferSize_ < size) {
      delete [] buffer_;

      bufferSize_ = size;

      buffer_ = new uchar [bufferSize_ + 1];
    }

    if (! file_->read(buffer_, size))
      return false;
  }

  if (strncmp(reinterpret_cast<char *>(&code[0]), "DNA1", 4) == 0)
    readSDNA(buffer_, size);

  return true;
}

bool
CImportBlend::
processBlock(char *code)
{
  uchar header[25];

  if (pointerSize_ == 4) {
    if (! file_->read(header, 20))
      return false;
  }
  else {
    if (! file_->read(header, 24))
      return false;
  }

  memcpy(&code[0], &header[0], 4); // 0-3
  code[4] = '\0';

  auto size  = decodeInteger(&header[4]); // 4-7
  // pointer (8-11 or 8-15)
  auto index = decodeInteger(&header[pointerSize_ == 4 ? 12 : 16]); // 12-15 or 16-19
  auto count = decodeInteger(&header[pointerSize_ == 4 ? 16 : 20]); // 16-19 or 20-23

  if (size > 0) {
    if (bufferSize_ < size) {
      delete [] buffer_;

      bufferSize_ = size;

      buffer_ = new uchar [bufferSize_ + 1];
    }

    if (! file_->read(buffer_, size))
      return false;
  }

  //---

  // skip DNA
  if (strncmp(reinterpret_cast<char *>(&code[0]), "DNA1", 4) == 0)
    return true;

  //---

  const auto &s = structs_[index];
  const auto &t = types_[s.typeInd];

  if (isDebug())
    std::cerr << t.name << " #" << count << " (" << size << ")\n";

  uint pos       = 0;
  uint structLen = 0;

  struct VarData {
    uint ind  { 0 };
    uint pos  { 0 };
    uint size { 0 };
  };

  std::map<std::string, VarData> varDataMap;

  while (pos < size) {
    uint iv = 0;

    for (const auto &v : s.vars) {
      const auto &t1 = types_[v.typeInd];
      const auto &n1 = names_[v.nameInd];

      VarData varData;

      varData.ind = iv;
      varData.pos = pos;

      if (n1.data.isPtr)
        varData.size = pointerSize_*n1.data.dimLen;
      else
        varData.size = t1.len*n1.data.dimLen;

      varDataMap[n1.name] = varData;

      if (isDebug()) {
        std::cerr << "  " << pos << ":" << " " << t1.name << " (" << t1.len << ") ";

        std::cerr << n1.name << " ";

        printData(n1, t1, &buffer_[pos]);

        std::cerr << "\n";
      }

      pos += varData.size;

      ++iv;
    }

    if (structLen == 0)
      structLen = pos;
  }

  //---

  if      (t.name == "Mesh") {
    mesh_ = new Mesh;

    auto pl = varDataMap.find("loc");

    if (pl != varDataMap.end()) {
      const auto &v = (*pl).second;

      assert(v.size == 12);

      auto x = decodeFloat(&buffer_[v.pos    ]);
      auto y = decodeFloat(&buffer_[v.pos + 4]);
      auto z = decodeFloat(&buffer_[v.pos + 8]);

      mesh_->loc = CPoint3D(x, y, z);
    }

    auto ps = varDataMap.find("size");

    if (ps != varDataMap.end()) {
      const auto &v = (*ps).second;

      assert(v.size == 12);

      auto x = decodeFloat(&buffer_[v.pos    ]);
      auto y = decodeFloat(&buffer_[v.pos + 4]);
      auto z = decodeFloat(&buffer_[v.pos + 8]);

      mesh_->size = CPoint3D(x, y, z);
    }

    meshes_.push_back(mesh_);
  }
  else if (t.name == "MVert") {
    assert(mesh_ && s.vars.size() >= 4 && structLen >= 16);

    uint nv   = size/structLen;
    uint pos1 = 0;

    for (uint i = 0; i < nv; ++i) {
      auto x = decodeFloat(&buffer_[pos1    ]);
      auto y = decodeFloat(&buffer_[pos1 + 4]);
      auto z = decodeFloat(&buffer_[pos1 + 8]);

      mesh_->points.push_back(CPoint3D(x, y, z));

      pos1 += structLen;
    }
  }
  else if (t.name == "MEdge") {
    assert(mesh_ && structLen == sizeof(MEdge));

    auto np   = size/structLen;
    uint pos1 = 0;

    mesh_->medge.resize(np);

    for (uint i = 0; i < np; ++i) {
      memcpy(&mesh_->medge[i], &buffer_[pos1], sizeof(MEdge));

      pos1 += structLen;
    }
  }
  else if (t.name == "MLoop") {
    assert(mesh_ && structLen == sizeof(MLoop));

    auto np   = size/structLen;
    uint pos1 = 0;

    mesh_->mloop.resize(np);

    for (uint i = 0; i < np; ++i) {
      memcpy(&mesh_->mloop[i], &buffer_[pos1], sizeof(MLoop));

      pos1 += structLen;
    }
  }
  else if (t.name == "MLoopUV") {
    assert(mesh_ && structLen == sizeof(MLoopUV));

    auto np   = size/structLen;
    uint pos1 = 0;

    mesh_->mloopUV.resize(np);

    for (uint i = 0; i < np; ++i) {
      memcpy(&mesh_->mloopUV[i], &buffer_[pos1], sizeof(MLoopUV));

      pos1 += structLen;
    }
  }
  else if (t.name == "MFace") {
    assert(mesh_ && structLen >= 12);

    auto np   = size/structLen;
    uint pos1 = 0;

    mesh_->mface.resize(np);

    for (uint i = 0; i < np; ++i) {
      auto *mface = &mesh_->mface[i];

      mface->v1 = decodeInteger(&buffer_[pos1    ]);
      mface->v2 = decodeInteger(&buffer_[pos1 + 4]);
      mface->v3 = decodeInteger(&buffer_[pos1 + 8]);

      pos1 += structLen;
    }
  }
  else if (t.name == "MPoly") {
    assert(mesh_ && structLen == sizeof(MPoly));

    auto np   = size/structLen;
    uint pos1 = 0;

    mesh_->mpoly.resize(np);

    for (uint i = 0; i < np; ++i) {
      memcpy(&mesh_->mpoly[i], &buffer_[pos1], sizeof(MPoly));

      pos1 += structLen;
    }
  }

  return true;
}

void
CImportBlend::
printData(const Name &n, const Type &t, uchar *data) const
{
  if (n.data.isPtr)
    return;

  if      (t.name == "char") {
    for (uint i = 0; i < n.data.dimLen; ++i) {
      if (isprint(data[i]))
        std::cerr << data[i];
      else
        std::cerr << "?";
    }
  }
  else if (t.name == "short") {
    for (uint i = 0; i < n.data.dimLen; ++i) {
      if (i > 0) std::cerr << ", ";

      auto s = decodeShort(&data[i*2]);

      std::cerr << s;
    }
  }
  else if (t.name == "int") {
    for (uint i = 0; i < n.data.dimLen; ++i) {
      if (i > 0) std::cerr << ", ";

      auto ii = decodeInteger(&data[i*4]);

      std::cerr << ii;
    }
  }
  else if (t.name == "float") {
    for (uint i = 0; i < n.data.dimLen; ++i) {
      if (i > 0) std::cerr << ", ";

      auto f = decodeFloat(&data[i*4]);

      std::cerr << f;
    }
  }
}

bool
CImportBlend::
readSDNA(const uchar *buffer, uint size)
{
  uint pos = 0;

  // SDNA 0-3
  if (strncmp(reinterpret_cast<const char *>(&buffer[pos]), "SDNA", 4) != 0)
    return false;

  pos += 4;

  //---

  auto readInteger = [&]() {
    auto i = decodeInteger(&buffer[pos]);

    pos += 4;

    return i;
  };

  auto readShort = [&]() {
    auto i = decodeShort(&buffer[pos]);

    pos += 2;

    return i;
  };

  auto readName = [&]() {
    auto pos1 = pos;

    while (pos1 < size && buffer[pos1] != '\0')
      ++pos1;

    auto len = pos1 - pos;

    if (pos1 < size)
      ++pos1;

    if (nameLen_ < len) {
      delete [] name_;

      nameLen_ = len;

      name_ = new char [nameLen_ + 1];
    }

    memcpy(&name_[0], &buffer[pos], len);
    name_[len] = '\0';

    pos = pos1;
  };

  auto align4 = [](uint i) {
    return (i + 3) & 0xfffffffc;
  };

  //---

  // NAME 4-7
  if (strncmp(reinterpret_cast<const char *>(&buffer[pos]), "NAME", 4) != 0)
    return false;

  pos += 4;

  // read names
  auto n = readInteger(); // 8-11

  //std::cerr << "Number of names: " << n << "\n";

  names_.resize(n);

  for (uint i = 0; i < n; ++i) {
    readName();

    //std::cerr << "  " << i << ":" << name_ << "\n";

    Name name;

    name.name = name_;

    names_[i] = name;

    decodeNameData(names_[i]);
  }

  pos = align4(pos);

  //---

  // TYPE
  if (strncmp(reinterpret_cast<const char *>(&buffer[pos]), "TYPE", 4) != 0)
    return false;

  pos += 4;

  // read type names and lengths
  auto nt = readInteger();

  //std::cerr << "Number of type names: " << nt << "\n";

  types_.resize(nt);

  for (uint i = 0; i < nt; ++i) {
    readName();

    //std::cerr << "  " << i << ":" << name_ << "\n";

    types_[i].name = name_;
  }

  pos = align4(pos);

  //---

  // TLEN
  if (strncmp(reinterpret_cast<const char *>(&buffer[pos]), "TLEN", 4) != 0)
    return false;

  pos += 4;

  // read type lengths
  for (uint i = 0; i < nt; ++i) {
    auto len = readShort();

    //std::cerr << "  " << len << "\n";

    types_[i].len = len;
  }

  pos = align4(pos);

  //---

  // STRC
  if (strncmp(reinterpret_cast<const char *>(&buffer[pos]), "STRC", 4) != 0)
    return false;

  pos += 4;

  auto ns = readInteger();

  structs_.resize(ns);

  //std::cerr << "Number of structs: " << ns << "\n";

  for (uint i = 0; i < ns; ++i) {
    auto &s = structs_[i];

    s.typeInd = readShort();

    auto count = readShort();

    s.vars.resize(count);

    for (ushort j = 0; j < count; ++j) {
      auto &v = s.vars[j];

      v.typeInd = readShort();
      v.nameInd = readShort();
    }

    typeStruct_[s.typeInd] = i;
  }

  pos = align4(pos);

  //---

#if 0
  for (const auto &s : structs_) {
    std::cerr << types_[s.typeInd].name << "\n";

    for (const auto &v : s.vars) {
      std::cerr << "  " << types_[v.typeInd].name << "\n";
      std::cerr << "  " << names_[v.nameInd]      << "\n";
    }
  }
#endif

  return true;
}

void
CImportBlend::
decodeNameData(Name &name) const
{
  int pos = 0;

  if (name.name[0] == '*') {
    name.data.isPtr = true;

    ++pos;
  }

  while (name.name[pos] != '\0') {
    if (name.name[pos] == '[')
      break;

    name.data.name += name.name[pos++];
  }

  while (name.name[pos] == '[') {
    ++pos;

    std::string dimStr;

    while (name.name[pos] != '\0') {
      if (name.name[pos] == ']')
        break;

      dimStr += name.name[pos++];
    }

    if (name.name[pos] == ']')
      ++pos;

    name.data.dims.push_back(std::stoi(dimStr));
  }

  name.data.dimLen = 1;

  for (const auto &d : name.data.dims)
    name.data.dimLen *= d;

  assert(name.name[pos] == '\0');
}

uint
CImportBlend::
decodeInteger(const uchar *data) const
{
  uint fi;

  if (! littleEndian_)
    fi = ((data[3] & 0xFF)      ) |
         ((data[2] & 0xFF) <<  8) |
         ((data[1] & 0xFF) << 16) |
         ((data[0] & 0xFF) << 24);
  else
    fi = ((data[0] & 0xFF)      ) |
         ((data[1] & 0xFF) <<  8) |
         ((data[2] & 0xFF) << 16) |
         ((data[3] & 0xFF) << 24);

  return fi;
}

ushort
CImportBlend::
decodeShort(const uchar *data) const
{
  ushort fi;

  if (! littleEndian_)
    fi = ((data[1] & 0xFF)     ) |
         ((data[0] & 0xFF) << 8);
  else
    fi = ((data[0] & 0xFF)     ) |
         ((data[1] & 0xFF) << 8);

  return fi;
}

float
CImportBlend::
decodeFloat(const uchar *data) const
{
  auto i = decodeInteger(data);

  return *reinterpret_cast<float *>(&i);
}
