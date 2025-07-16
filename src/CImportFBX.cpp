#include <CImportFBX.h>
#include <CGeometry3D.h>
#include <CStrParse.h>

#include <array>

//---

namespace {

using FileData = CImportFBX::FileData;

std::string hexStr(uint i) {
  std::stringstream ss;
  ss << std::showbase << std::hex << i;
  return ss.str();
}

std::string hexStr(ulong i) {
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

  auto rc = readBinary();

  if (! rc)
    rc = readAscii();

  if (! rc)
    return errorMsg("Invalid FBX file");

  return true;
}

bool
CImportFBX::
readBinary()
{
  const uint headerLen = 27;

  // check magic
  {
  uchar buffer[headerLen];

  if (file_->read(buffer, headerLen)) {
    if (strcmp(reinterpret_cast<char *>(buffer), "Kaydara FBX Binary  ") != 0)
      return false;
  }

  int version = *reinterpret_cast<int *>(&buffer[23]);
  infoMsg("Version: " + std::to_string(version));

  longInt_ = (version >= 7500);
  }

  auto readFileBytes = [&](ulong len, std::vector<uchar> &bytes) {
    bytes.resize(len);

    if (! file_->read(&bytes[0], len))
      return false;

    return true;
  };

  FileData fileData;

  fileData.filePos  = 27;
  fileData.fileSize = uint(file_->getSize());

  file_->setPos(0);

  if (! readFileBytes(fileData.fileSize, fileData.fileBytes))
    return errorMsg("Failed to read file");

  if (! readFileData(fileData))
    return false;

  return true;
}

bool
CImportFBX::
readAscii()
{
  uchar* data;
  size_t len;

  if (! file_->readAll(&data, &len))
    return false;

  auto str = std::string(reinterpret_cast<char *>(data), len);

  CStrParse parse(str);

  auto *block = new TextBlock;

  if (! readTextBlock(parse, block))
    std::cerr << "readAscii failed\n";

//printTextBlock(block);

  processTextBlock(block);

  return true;
}

bool
CImportFBX::
readTextBlock(CStrParse &parse, TextBlock *block)
{
  auto readLine = [&]() {
    std::string line;

    while (! parse.eof() && ! parse.isChar('\n'))
      line += parse.readChar();

    if (! parse.eof())
      parse.skipChar();

    return line;
  };

  while (! parse.eof()) {
    auto line1 = readLine();

    CStrParse parse1(line1);

    auto indent1 = parse1.skipSpace();

    // skip comment
    if (parse1.eof() || parse1.isChar(';'))
      continue;

    //---

    if (parse1.isChar('}') && indent1 <= block->indent) {
      break;
    }

    NameValue nameValue;

    // read name
    while (! parse1.eof() && ! parse1.isChar(':'))
      nameValue.name += parse1.readChar();

    if (! parse1.eof()) {
      parse1.skipChar();

      parse1.skipSpace();
    }

    // read value
    char lastChar = '\0';

    while (! parse1.eof() && ! parse1.isChar('{')) {
      lastChar = parse1.readChar();

      nameValue.value += lastChar;
    }

    while (lastChar == ',') {
      auto line2 = readLine();

      if (line2.empty())
        break;

      lastChar = line2[line2.size() - 1];

      nameValue.value += line2;
    }

    // read children
    if (parse1.isChar('{')) {
      parse1.skipChar();

      parse1.skipSpace();

      if (! parse1.eof()) {
        std::cerr << "readTextBlock failed - bad {\n";
        return false;
      }

      std::string lines;

      while (! parse.eof()) {
        auto line2 = readLine();

        CStrParse parse2(line2);

        auto indent2 = parse2.skipSpace();

        if (parse2.isChar('}') && indent2 <= indent1) {
          parse2.skipChar();

          parse2.skipSpace();

          if (! parse2.eof()) {
            std::cerr << "readTextBlock failed - bad }\n";
            return false;
          }

          break;
        }
        else {
          if (! lines.empty())
            lines += "\n";

          lines += line2;
        }
      }

      CStrParse parse2(lines);

      auto *block1 = new TextBlock;

      block1->parent    = block;
      block1->indent    = indent1;
      block1->nameValue = nameValue;

      block->children.push_back(block1);

      readTextBlock(parse2, block1);
    }
    else {
      block->nameValues.push_back(nameValue);
    }
  }

  return true;
}

void
CImportFBX::
printTextBlock(TextBlock *block)
{
  auto printIndent = [&]() {
    for (int i = 0; i < block->indent; ++i)
      std::cerr << " ";
  };

  printIndent();

  std::cerr << block->nameValue.name << ":" << block->nameValue.value << "\n";

  for (const auto &nv : block->nameValues) {
    printIndent();

    std::cerr << " " << nv.name << ":" << nv.value << "\n";
  }

  for (auto *child : block->children)
    printTextBlock(child);
}

void
CImportFBX::
processTextBlock(TextBlock *block)
{
  auto readReals = [](const std::string &value) {
    std::vector<double> reals;

    CStrParse parse(value);

    while (! parse.eof()) {
      parse.skipSpace();

      double r;
      if (! parse.readReal(&r)) {
        std::cerr << "Invalid real\n";
        break;
      }

      reals.push_back(r);

      parse.skipSpace();

      if (! parse.eof() && parse.isChar(',')) {
        parse.skipChar();

        parse.skipSpace();
      }
    }

    return reals;
  };

  //---

  auto readIntegers = [](const std::string &value) {
    std::vector<int> integers;

    CStrParse parse(value);

    while (! parse.eof()) {
      parse.skipSpace();

      int i;
      if (! parse.readInteger(&i)) {
        std::cerr << "Invalid integer\n";
        break;
      }

      integers.push_back(i);

      parse.skipSpace();

      if (! parse.eof() && parse.isChar(',')) {
        parse.skipChar();

        parse.skipSpace();
      }
    }

    return integers;
  };

  //---

#if 0
  auto printReals = [](const std::string &name, const std::vector<double> &reals) {
    if (! reals.empty()) {
      std::cerr << name << ":";
      for (const auto &r : reals)
        std::cerr << " " << r;
      std::cerr << "\n";
    }
  };

  auto printIntegers = [](const std::string &name, const std::vector<int> &integers) {
    if (! integers.empty()) {
      std::cerr << name << ":";
      for (const auto &i : integers)
        std::cerr << " " << i;
      std::cerr << "\n";
    }
  };
#endif

  //---

  if (block->nameValue.name == "Model") {
    GeometryData geometryData;

    for (const auto &nv : block->nameValues) {
      if      (nv.name == "PolygonVertexIndex")
        geometryData.polygonVertexIndex = readIntegers(nv.value);
      else if (nv.name == "Vertices")
        geometryData.vertices = readReals(nv.value);
      else if (nv.name == "Edges")
        geometryData.edges = readIntegers(nv.value);
      else if (nv.name == "NormalsIndex")
        geometryData.normalsIndex = readIntegers(nv.value);
      else if (nv.name == "Normals")
        geometryData.normals = readReals(nv.value);
      else if (nv.name == "ColorIndex")
        geometryData.colorIndex = readIntegers(nv.value);
      else if (nv.name == "Colors")
        geometryData.colors = readReals(nv.value);
      else if (nv.name == "UVIndex")
        geometryData.uvIndex = readIntegers(nv.value);
      else if (nv.name == "UV")
        geometryData.uv = readReals(nv.value);
      else if (nv.name == "Materials")
        geometryData.materials = readIntegers(nv.value);
    }

#if 0
    printIntegers("PolygonVertexIndex", geometryData.polygonVertexIndex);
    printReals   ("Vertices"          , geometryData.vertices);
    printIntegers("Edges"             , geometryData.edges);
    printIntegers("NormalsIndex"      , geometryData.normalsIndex);
    printReals   ("Normals"           , geometryData.normals);
    printIntegers("ColorIndex"        , geometryData.colorIndex);
    printReals   ("Colors"            , geometryData.colors);
    printIntegers("UVIndex"           , geometryData.uvIndex);
    printReals   ("UV"                , geometryData.uv);
    printIntegers("Materials"         , geometryData.materials);
#endif

    addGeometryObject(geometryData);
  }

  for (auto *child : block->children)
    processTextBlock(child);
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

    if (isHierName())
      std::cerr << prefix << hierName(tree) << "\n";
    else
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

std::string
CImportFBX::
hierName(PropDataTree *tree) const
{
  if (! tree->parent || tree->parent->name == "")
    return tree->name;

  return hierName(tree->parent) + "/" + tree->name;
}

void
CImportFBX::
addGeometry(PropDataTree *tree)
{
  GeometryData geometryData;

  addSubGeometry(tree, geometryData);

  addGeometryObject(geometryData);
}

void
CImportFBX::
addGeometryObject(GeometryData &geometryData)
{
  if (geometryData.polygonVertexIndex.empty() ||
      geometryData.vertices.empty())
    return;

  //---

  assert(! geometryData.object);

  geometryData.object = CGeometryInst->createObject3D(scene_, getObjectName());

  scene_->addObject(geometryData.object);

  //---

  struct PolyPoint {
    int pointInd   { -1 };
    int normalInd  { -1 };
    int colorInd   { -1 };
    int textureInd { -1 };
  };

  using Polygon  = std::vector<PolyPoint>;
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
  auto ni = uint(geometryData.polygonVertexIndex.size());

  auto nn = uint(geometryData.normalsIndex.size());
  auto hasNormal = (nn == ni);

  auto nci = uint(geometryData.colorIndex.size());
  auto ncv = uint(geometryData.colors    .size());
  auto hasColor = (nci == ni && ncv > 0);

  auto nt = uint(geometryData.uvIndex.size());
  auto hasTexture = (nt == ni);

  for (uint i = 0; i < ni; ++i) {
    PolyPoint polyPoint;

    polyPoint.pointInd = geometryData.polygonVertexIndex[i];

    bool flush = (polyPoint.pointInd < 0);

    if (flush)
      polyPoint.pointInd = ~polyPoint.pointInd;

    //---

    if (hasNormal)
      polyPoint.normalInd = geometryData.normalsIndex[i];

    if (hasColor)
      polyPoint.colorInd = geometryData.colorIndex[i];

    if (hasTexture)
      polyPoint.textureInd = geometryData.uvIndex[i];

    //---

    polygon.push_back(polyPoint);

    if (flush) {
      polygons.push_back(polygon);

      polygon = Polygon();
    }
  }
#endif

  if (! polygon.empty())
    polygons.push_back(polygon);

  auto getPoint = [&](int v) {
    return CPoint3D(geometryData.vertices[v*3    ],
                    geometryData.vertices[v*3 + 1],
                    geometryData.vertices[v*3 + 2]);
  };

  auto getNormal = [&](int v) {
    return CVector3D(geometryData.normals[v*3    ],
                     geometryData.normals[v*3 + 1],
                     geometryData.normals[v*3 + 2]);
  };

  auto getColor = [&](int v) {
    return CRGBA(geometryData.colors[v*3    ],
                 geometryData.colors[v*3 + 1],
                 geometryData.colors[v*3 + 2]);
  };

  auto getTexturePoint = [&](int v) {
    return CPoint2D(geometryData.uv[v*2    ],
                    geometryData.uv[v*2 + 1]);
  };

  auto np = polygons.size();

  for (uint i = 0; i < np; ++i) {
    const auto &polygon1 = polygons[i];

    auto n1 = polygon1.size();
    if (n1 < 3)
      continue;

    std::vector<uint>     inds;
    std::vector<CPoint2D> tpoints;

    for (uint j = 0; j < n1; ++j) {
      const auto &polygon2 = polygon1[j];

      auto ind = geometryData.object->addVertex(getPoint(polygon2.pointInd));

      auto &vertex = geometryData.object->getVertex(ind);

      if (hasNormal) {
        auto n = getNormal(polygon2.normalInd);

        vertex.setNormal(n);
      }

      if (hasColor) {
        auto c = getColor(polygon2.colorInd);

        vertex.setColor(c);
      }

      if (hasTexture) {
        auto p = getTexturePoint(polygon2.textureInd);

        vertex.setTextureMap(p);

        tpoints.push_back(p);
      }

      inds.push_back(ind);
    }

    auto faceInd = geometryData.object->addIPolygon(&inds[0], uint(inds.size()));

    auto &face = geometryData.object->getFace(faceInd);

    if (! tpoints.empty())
      face.setTexturePoints(tpoints);
  }
}

void
CImportFBX::
addSubGeometry(PropDataTree *tree, GeometryData &geometryData)
{
  for (const auto &pd : tree->dataMap) {
    const auto &name = pd.first;

    if      (name == "PolygonVertexIndex") {
      for (const auto &pa : pd.second) {
        auto *data = pa.second;

        if (data->type() == DataType::INT_ARRAY)
          geometryData.polygonVertexIndex = data->getArrayData<int>();
        else
          std::cerr << "Unexpected type for PolygonVertexIndex\n";
      }
    }
    else if (name == "Vertices") { // vertices
      for (const auto &pa : pd.second) {
        auto *data = pa.second;

        if (data->type() == DataType::DOUBLE_ARRAY)
          geometryData.vertices = data->getArrayData<double>();
        else
          std::cerr << "Unexpected type for Vertices\n";
      }
    }
    else if (name == "Edges") { // edges
      for (const auto &pa : pd.second) {
        auto *data = pa.second;

        if (data->type() == DataType::INT_ARRAY)
          geometryData.edges = data->getArrayData<int>();
        else
          std::cerr << "Unexpected type for Edges\n";
      }
    }
    else if (name == "NormalsIndex") {
      for (const auto &pa : pd.second) {
        auto *data = pa.second;

        if (data->type() == DataType::INT_ARRAY)
          geometryData.normalsIndex = data->getArrayData<int>();
        else
          std::cerr << "Unexpected type for NormalsIndex\n";
      }
    }
    else if (name == "Normals") { // normals
      for (const auto &pa : pd.second) {
        auto *data = pa.second;

        if (data->type() == DataType::DOUBLE_ARRAY)
          geometryData.normals = data->getArrayData<double>();
        else
          std::cerr << "Unexpected type for Normals\n";
      }
    }
    else if (name == "ColorIndex") {
      for (const auto &pa : pd.second) {
        auto *data = pa.second;

        if (data->type() == DataType::INT_ARRAY)
          geometryData.colorIndex = data->getArrayData<int>();
        else
          std::cerr << "Unexpected type for ColorIndex\n";
      }
    }
    else if (name == "Colors") { // colors
      for (const auto &pa : pd.second) {
        auto *data = pa.second;

        if (data->type() == DataType::DOUBLE_ARRAY)
          geometryData.colors = data->getArrayData<double>();
        else
          std::cerr << "Unexpected type for Colors\n";
      }
    }
    else if (name == "UVIndex") {
      for (const auto &pa : pd.second) {
        auto *data = pa.second;

        if (data->type() == DataType::INT_ARRAY)
          geometryData.uvIndex = data->getArrayData<int>();
        else
          std::cerr << "Unexpected type for UVIndex\n";
      }
    }
    else if (name == "UV") { // texture coords
      for (const auto &pa : pd.second) {
        auto *data = pa.second;

        if (data->type() == DataType::DOUBLE_ARRAY)
          geometryData.uv = data->getArrayData<double>();
        else
          std::cerr << "Unexpected type for UV\n";
      }
    }
    else if (name == "Materials") {
      for (const auto &pa : pd.second) {
        auto *data = pa.second;

        if (data->type() == DataType::INT_ARRAY)
          geometryData.materials = data->getArrayData<int>();
        else
          std::cerr << "Unexpected type for UV\n";
      }
    }
  }

  for (auto *tree1 : tree->children) {
    if (tree1->name == "LayerElementColor" ||
        tree1->name == "LayerElementMaterial" ||
        tree1->name == "LayerElementNormal" ||
        tree1->name == "LayerElementUV")
      addSubGeometry(tree1, geometryData);
    else
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

  if (longInt_) {
    if (! fileData.readULong(&endOffset))
      return errorMsg("Failed to read endOffset");
  }
  else {
    if (! fileData.readUInt(&endOffset))
      return errorMsg("Failed to read endOffset");
  }

  //---

  ulong numProperties, propertyListLen;

  if (longInt_) {
    if (! fileData.readULong(&numProperties))
      return errorMsg("Failed to read numProperties");
  }
  else {
    if (! fileData.readUInt(&numProperties))
      return errorMsg("Failed to read numProperties");
  }

  if (longInt_) {
    if (! fileData.readULong(&propertyListLen))
      return errorMsg("Failed to read propertyListLen");
  }
  else {
    if (! fileData.readUInt(&propertyListLen))
      return errorMsg("Failed to read propertyListLen");
  }

  infoMsg("Block: " + hexStr(uint(startOffset)) + " " + hexStr(endOffset));

  if (endOffset <= 0) {
//  endOffset = fileData.fileSize - 1;
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

  uint nullLen = (longInt_ ? 25 : 13);

  auto testNullRecord = [&]() {
    if (extraLen < nullLen)
      return false;

    auto filePos1 = fileData.filePos;

    fileData.filePos = endOffset - nullLen;

    if (! fileData.readBytes(nullLen)) {
      fileData.filePos = filePos1;
      return false;
    }

    for (uint ic = 0; ic < nullLen; ++ic) {
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

  if (fileData1.fileSize > fileData.fileBytes.size())
    fileData1.fileSize = int(fileData.fileBytes.size());

  auto isNullRecord = testNullRecord();

  if (isNullRecord) {
    //std::cerr << " nullRecord\n";

    fileData1.fileSize -= nullLen;

    //---

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
  }
  else {
    std::cerr << " No nullRecord\n";

    //fileData1.filePos = fileData1.filePos - 1;

#if 0
    if (propertyListLen > 0) {
      auto *propDataTree1 = new PropDataTree;

      propDataTree1->name   = name;
      propDataTree1->parent = propDataTree;

      propDataTree->children.push_back(propDataTree1);

      for (uint pi = 0; pi < propertyListLen; ++pi) {
        if (! readScope(fileData1, concatStrs(scopeName, name), propDataTree1)) {
          (void) errorMsg("readScopeData failed");
          fileData.filePos = endOffset;
          return true;
        }
      }
    }
#endif
  }

  fileData.filePos = endOffset;

  return true;
}

std::string
CImportFBX::
getObjectName()
{
  ++objectInd_;

  return "Object" + std::to_string(objectInd_);
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
