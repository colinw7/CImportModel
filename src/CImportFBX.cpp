#include <CImportFBX.h>
#include <CGeometry3D.h>

#include <array>

//---

namespace {

using FileData = CImportFBX::FileData;

std::string hexStr(uint i) {
  std::stringstream ss;
  ss << std::showbase << std::hex << i;
  return ss.str();
}

std::string concatStrs(const std::string &lhs, const std::string &rhs) {
  if (lhs == "") return rhs;
  return lhs + "/" + rhs;
}

}

//---

CImportFBX::
CImportFBX(CGeomScene3D *scene, const std::string &name) :
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

CImportFBX::
~CImportFBX()
{
}

bool
CImportFBX::
read(CFile &file)
{
  file_ = &file;

  const uint headerLen = 27;

  // check magic
  {
  uchar buffer[headerLen];

  if (file_->read(buffer, headerLen)) {
    if (strcmp(reinterpret_cast<char *>(buffer), "Kaydara FBX Binary  ") != 0)
      return errorMsg("Invalid header");
  }

  int version = *reinterpret_cast<int *>(&buffer[23]);
  infoMsg("Version: " + std::to_string(version));
  }

  auto readFileBytes = [&](uint len, std::vector<uchar> &bytes) {
    bytes.resize(len);

    if (! file.read(&bytes[0], len))
      return false;

    return true;
  };

  FileData fileData;

  fileData.filePos  = 27;
  fileData.fileSize = uint(file.getSize());

  file.setPos(0);

  if (! readFileBytes(fileData.fileSize, fileData.fileBytes))
    return errorMsg("Failed to read file");

  if (! readFileData(fileData))
    return false;

  return true;
}

bool
CImportFBX::
readFileData(FileData &fileData)
{
  //std::cerr << "readFileData " << fileData.fileSize << "\n";

  //---

  propDataTree_ = new PropDataTree;

  std::string scopeName;

  while (fileData.filePos < fileData.fileSize) {
    if (! readScope(fileData, scopeName, propDataTree_))
      (void) errorMsg("readScope failed");
  }

  //---

  if (isDump())
    dumpTree(propDataTree_, 0);

  //---

  addGeometry(propDataTree_);

  //---

  return true;
}

void
CImportFBX::
dumpTree(PropDataTree *tree, int depth)
{
  std::string prefix;

  if (depth > 0) {
    for (int i = 0; i < depth; ++i)
      prefix += " ";

    std::cerr << prefix << tree->name << "\n";

    prefix += " ";
  }

  for (const auto &pd : tree->dataMap) {
    const auto &name = pd.first;

    std::cerr << prefix << name << ":\n";

    for (const auto &pa : pd.second) {
      const auto &ind  = pa.first;
      auto       *data = pa.second;

      std::cerr << prefix << " [" << ind << "] ";
      std::cerr << *data;
      std::cerr << "\n";
    }
  }

  for (auto *tree1 : tree->children)
    dumpTree(tree1, depth + 2);
}

void
CImportFBX::
addGeometry(PropDataTree *tree)
{
  std::vector<int>    polygonVertexIndex;
  std::vector<double> vertices;
  std::vector<int>    edges;

  for (const auto &pd : tree->dataMap) {
    const auto &name = pd.first;

    if      (name == "PolygonVertexIndex") {
      for (const auto &pa : pd.second) {
        auto *data = pa.second;

        if (data->type() == DataType::INT_ARRAY)
          polygonVertexIndex = data->getArrayData<int>();
      }
    }
    else if (name == "Vertices") {
      for (const auto &pa : pd.second) {
        auto *data = pa.second;

        if (data->type() == DataType::DOUBLE_ARRAY)
          vertices = data->getArrayData<double>();
      }
    }
    else if (name == "Edges") {
      for (const auto &pa : pd.second) {
        auto *data = pa.second;

        if (data->type() == DataType::INT_ARRAY)
          edges = data->getArrayData<int>();
      }
    }
  }

  if (! edges.empty() && ! polygonVertexIndex.empty() && ! vertices.empty()) {
    using Polygon  = std::vector<int>;
    using Polygons = std::vector<Polygon>;

    Polygons polygons;
    Polygon  polygon;

#if 0
    auto ne = uint(edges.size());

    for (uint i = 0; i < ne; ++i) {
      auto ie = edges[i];
      auto i1 = polygonVertexIndex[ie];

      bool flush = (i1 < 0);

      if (flush)
        i1 = ~i1;

      polygon.push_back(i1);

      if (flush) {
        polygons.push_back(polygon);

        polygon = Polygon();
      }
    }
#else
    auto ni = uint(polygonVertexIndex.size());

    for (uint i = 0; i < ni; ++i) {
      auto i1 = polygonVertexIndex[i];

      bool flush = (i1 < 0);

      if (flush)
        i1 = ~i1;

      polygon.push_back(i1);

      if (flush) {
        polygons.push_back(polygon);

        polygon = Polygon();
      }
    }
#endif

    if (! polygon.empty())
      polygons.push_back(polygon);

    auto getPoint = [&](int v) {
      return CPoint3D(vertices[v*3], vertices[v*3 + 1], vertices[v*3 + 2]);
    };

    auto np = polygons.size();

    for (uint i = 0; i < np; ++i) {
      const auto &polygon1 = polygons[i];

      auto n1 = polygon1.size();
      if (n1 < 3)
        continue;

      std::vector<uint> inds;

      for (uint j = 0; j < n1; ++j) {
        int pi = polygon1[j];

        auto ind = object_->addVertex(getPoint(pi));

        inds.push_back(ind);
      }

      object_->addIPolygon(&inds[0], uint(inds.size()));
    }
  }

  for (auto *tree1 : tree->children) {
    addGeometry(tree1);
  }
}

bool
CImportFBX::
readScope(FileData &fileData, const std::string &scopeName, PropDataTree *propDataTree)
{
  class IncDepth {
   public:
    IncDepth(int &d) { d_ = &d; ++(*d_); }
   ~IncDepth() { --(*d_); }
   private:
    int *d_ { nullptr };
  };

  auto startOffset = fileData.filePos;
  auto endOffset   = fileData.fileSize;

  IncDepth idepth(depth_);

  //---

  uint numProperties, propertyListLen;

  if (! fileData.readUInt(&endOffset))
    return errorMsg("Failed to read endOffset");
  if (! fileData.readUInt(&numProperties))
    return errorMsg("Failed to read numProperties");
  if (! fileData.readUInt(&propertyListLen))
    return errorMsg("Failed to read propertyListLen");

  infoMsg("Block: " + hexStr(uint(startOffset)) + " " + hexStr(endOffset));

  if (endOffset <= 0) {
    fileData.filePos = fileData.fileSize;
    return true;
  }

  uchar       nameLen;
  std::string name;

  if (! fileData.readUChar(&nameLen))
    return errorMsg("Failed to read property name len");

  if (! fileData.readStr(nameLen, name))
    return errorMsg("Failed to read property name");

#if 0
  std::cerr << name << " startOffset=" << startOffset << " endOffset=" << endOffset <<
                       " numProperties=" << numProperties <<
                       " propertyListLen=" << propertyListLen << "\n";
#endif

  if (name != "")
    infoMsg("Name: " + concatStrs(scopeName, name));

  //---

  for (uint pi = 0; pi < numProperties; ++pi) {
    if (! readScopeData(name, pi, fileData, propDataTree))
      break;
  }

  //---

  if (fileData.filePos > endOffset) {
    (void) errorMsg("Bad end offset " + std::to_string(endOffset) +
                    " for file pos " + std::to_string(fileData.filePos));
    fileData.filePos = endOffset;
    return true;
  }

  auto extraLen = long(endOffset) - fileData.filePos;
  //std::cerr << " extraLen=" << extraLen << "\n";

  if (extraLen == 0) {
    fileData.filePos = endOffset;
    return true;
  }

  //---

  auto isNullRecord = [&]() {
    if (extraLen <= 13)
      return false;

    auto filePos1 = fileData.filePos;

    fileData.filePos = endOffset - 13;

    if (! fileData.readBytes(13)) {
      fileData.filePos = filePos1;
      return false;
    }

    for (int ic = 0; ic < 13; ++ic) {
      if (fileData.buffer[ic] != 0) {
        fileData.filePos = filePos1;
        return false;
      }
    }

    fileData.filePos = filePos1;

    return true;
  };

  FileData fileData1;

  fileData1.fileBytes = fileData.fileBytes;
  fileData1.filePos   = fileData.filePos;
  fileData1.fileSize  = endOffset + 1;

  if (isNullRecord()) {
    //std::cerr << " nullRecord\n";

    fileData1.fileSize -= 13;
  }

  auto *propDataTree1 = new PropDataTree;

  propDataTree1->name   = name;
  propDataTree1->parent = propDataTree;

  propDataTree->children.push_back(propDataTree1);

  while (fileData1.filePos < fileData1.fileSize - 1) {
    if (! readScope(fileData1, concatStrs(scopeName, name), propDataTree1)) {
      (void) errorMsg("readScope failed");
      fileData.filePos = endOffset;
      return true;
    }
  }

  fileData.filePos = endOffset;

  return true;
}

//---

bool
CImportFBX::
readScopeData(const std::string &name, uint ind, FileData &fileData,
              PropDataTree *propDataTree)
{
  auto ucharStr = [](uchar c) {
    std::string s;
    if (isprint(char(c))) s += char(c);
    s += "(" + std::to_string(int(c)) + ")";
    return s;
  };

  auto printUChar = [&](uchar c) {
    std::cerr << ucharStr(c);
  };

  //---

  auto *propData = new PropData;

  uchar t;
  if (! fileData.readUChar(&t))
    return errorMsg("Failed to read data type");

  // raw binary data
  if      (t == 'R') {
    uint len;
    if (! fileData.readUInt(&len))
      return errorMsg("Failed to read len for " + ucharStr(t));

    if (! fileData.readBytes(len))
      return errorMsg("Failed to read data for " + ucharStr(t));
    infoMsg(" R " + std::to_string(len));

    auto rdata = fileData.buffer;

    propData->setData(rdata);
  }
  else if (t == 'S') {
    uint len;
    if (! fileData.readUInt(&len))
      return errorMsg("Failed to read len for " + ucharStr(t));

    std::string str;
    if (! fileData.readStr(len, str))
      return errorMsg("Failed to read data for " + ucharStr(t));

    infoMsg(" S " + std::to_string(len) + " '" + str + "'");

    propData->setData(str);
  }
  else if (t == 'Y') {
    short s;
    if (! fileData.readShort(&s))
      return errorMsg("Failed to read data for " + ucharStr(t));
    infoMsg(" Y " + std::to_string(s));

    propData->setData(s);
  }
  else if (t == 'C') {
    uchar c;
    if (! fileData.readUChar(&c))
      return errorMsg("Failed to read data for " + ucharStr(t));
    infoMsg(" C " + ucharStr(c));
  }
  else if (t == 'I') {
    int i;
    if (! fileData.readInt(&i))
      return errorMsg("Failed to read data for " + ucharStr(t));
    infoMsg(" I " + std::to_string(i));

    propData->setData(i);
  }
  else if (t == 'F') {
    float f;
    if (! fileData.readFloat(&f))
      return errorMsg("Failed to read data for " + ucharStr(t));
    infoMsg(" F " + std::to_string(f));

    propData->setData(f);
  }
  else if (t == 'D') {
    double r;
    if (! fileData.readDouble(&r))
      return errorMsg("Failed to read data for " + ucharStr(t));
    infoMsg(" D " + std::to_string(r));

    propData->setData(r);
  }
  else if (t == 'L') {
    long l;
    if (! fileData.readLong(&l))
      return errorMsg("Failed to read data for " + ucharStr(t));
    infoMsg(" L " + std::to_string(l));

    propData->setData(l);
  }
  else if (t == 'f') {
    std::vector<float> data;
    if (! fileData.readArrayData(data, t, depth_))
      return false;

    propData->setData(data);
  }
  else if (t == 'd') {
    std::vector<double> data;
    if (! fileData.readArrayData(data, t, depth_))
      return false;

    propData->setData(data);
  }
  else if (t == 'l') {
    std::vector<long> data;
    if (! fileData.readArrayData(data, t, depth_))
      return false;

    propData->setData(data);
  }
  else if (t == 'i') {
    std::vector<int> data;
    if (! fileData.readArrayData(data, t, depth_))
      return false;

    propData->setData(data);
  }
  else if (t == 'b') {
    std::vector<uchar> data;
    if (! fileData.readArrayData(data, t, depth_))
      return false;

    propData->setData(data);
  }
  else {
    std::cerr << "Invalid Code: "; printUChar(t); std::cerr << "\n";
    return false;
  }

  propDataTree->dataMap[name][ind] = propData;

  return true;
}

void
CImportFBX::
infoMsg(const std::string &msg) const
{
  if (! isDebug()) return;

  for (int i = 0; i < depth_; ++i)
    std::cerr << " ";

  std::cerr << msg << "\n";
}

bool
CImportFBX::
errorMsg(const std::string &msg) const
{
  for (int i = 0; i < depth_; ++i)
    std::cerr << " ";

  std::cerr << "Error: " << msg << "\n";

  return false;
}
