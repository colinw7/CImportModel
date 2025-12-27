#include <CImportFBX.h>
#include <CGeometry3D.h>
#include <CStrParse.h>

#include <set>

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

bool unhandledTree(const std::string &path) {
  std::cerr << "Unhandled tree " << path << "\n";
  return false;
}

bool unhandledProperty(const std::string &path, const std::string &name) {
  std::cerr << "Unhandled property " << path << " " << name << "\n";
  return false;
}

#if 0
bool errorMsg(const std::string &msg) {
  std::cerr << "Error: " << msg << "\n";
  return false;
}
#endif

void debugMsg(const std::string &msg) {
  std::cerr << msg << "\n";
}

}

//---

CImportFBX::
CImportFBX(CGeomScene3D *scene, const std::string &name) :
 scene_(scene)
{
  auto name1 = (name.size() ? name : "f3d");

  if (! scene_) {
    scene_  = CGeometryInst->createScene3D();
    pscene_ = SceneP(scene_);
  }

  object_ = CGeometryInst->createObject3D(scene_, name1);

  scene_->addObject(object_);

  if (! pobject_)
    pobject_ = ObjectP(object_);
}

CImportFBX::
~CImportFBX()
{
  for (auto &pa : animationStackData_)
    delete pa.second;
}

bool
CImportFBX::
read(CFile &file)
{
  file_ = &file;

  bool isBin = false;
  auto rc = readBinary(isBin);

  if (! rc && ! isBin)
    rc = readAscii();

  if (! rc)
    return errorMsg("Invalid FBX file");

  return true;
}

bool
CImportFBX::
readBinary(bool &isBin)
{
  const uint headerLen = 27;

  // check magic
  {
  uchar buffer[headerLen];

  if (file_->read(buffer, headerLen)) {
    if (strncmp(reinterpret_cast<char *>(buffer), "Kaydara FBX Binary  ", 20) != 0)
      return false;
  }

  isBin = true;

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

  if (! readFileBytes(fileData.fileSize, fileData.fileMem))
    return errorMsg("Failed to read file");

  fileData.fileBytes = &fileData.fileMem[0];

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
    errorMsg("readAscii failed");

  if (isDump())
    printTextBlock(block);

  processTextBlock(block);

  for (const auto &pc : connectionMap_) {
    const auto &name  = pc.first;
    const auto &value = pc.second;

    if (value.substr(0, 7) != "Model::")
      continue;

    auto modelName = value.substr(7);

    auto *obj = scene_->getObjectP(modelName);

    if (! obj) {
    //std::cerr << "No object for " << modelName << "\n";
      continue;
    }

    if      (name.substr(0, 10) == "Material::") {
      auto materialName = name.substr(10);

      auto pm = materialDataMap_.find(materialName);

      if (pm == materialDataMap_.end()) {
        errorMsg("No material for " + materialName);
        continue;
      }
    }
    else if (name.substr(0, 9) == "Texture::") {
      auto textureName = name.substr(9);

      auto pt = textureDataMap_.find(textureName);

      if (pt == textureDataMap_.end()) {
        errorMsg("No texture for " + textureName);
        continue;
      }

      auto *textureData = (*pt).second;

      if (! textureData->texture) {
        if (textureData->fileName != "") {
          if (CFile::exists(textureData->fileName)) {
            CImageFileSrc src(textureData->fileName);

            auto image = CImageMgrInst->createImage(src);

            image->flipH();

            textureData->texture = CGeometryInst->createTexture(image);

            textureData->texture->setName(textureData->fileName);
          }
        }
      }

      if (textureData->texture)
        obj->setDiffuseTexture(textureData->texture);
    }
  }

  return true;
}

bool
CImportFBX::
readTextBlock(CStrParse &parse, TextBlock *block)
{
  if (isDebug())
    std::cerr << "readTextBlock " << hierName(block) << "\n";

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
        errorMsg("readTextBlock failed - bad {");
        return false;
      }

      std::string lines;
      int         depth = 0;

      while (! parse.eof()) {
        auto line2 = readLine();

        CStrParse parse2(line2);

        //---

        bool blockStart = false;
        bool blockEnd   = false;

        /* auto indent2 = */ parse2.skipSpace();

        if (parse2.isChar('}'))
          blockEnd = true;

        auto len2 = parse2.getLen();
        auto pos2 = int(len2) - 1;

        if (pos2 >= 0 && std::isspace(parse2.getCharAt(pos2)))
          --pos2;

        if (pos2 >= 0 && parse2.getCharAt(pos2) == '{')
          blockStart = true;

        //---

        if (blockEnd) {
          if (depth == 0) {
            parse2.skipChar();

            parse2.skipSpace();

            if (! parse2.eof()) {
              errorMsg("readTextBlock failed - bad }");
              return false;
            }

            break;
          }
          else
            --depth;
        }
        else {
          if (blockStart)
            ++depth;

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

  printNameValues(block);

  for (auto *child : block->children)
    printTextBlock(child);
}

void
CImportFBX::
printNameValues(TextBlock *block)
{
  auto printIndent = [&]() {
    for (int i = 0; i < block->indent; ++i)
      std::cerr << " ";
  };

  auto name = (isHierName() ? hierName(block) : block->nameValue.name);

  std::cerr << name << ":" << block->nameValue.value << "\n";

  for (const auto &nv : block->nameValues) {
    printIndent();

    std::cerr << " " << nv.name << ":" << nv.value << "\n";
  }
}

bool
CImportFBX::
processTextBlock(TextBlock *block)
{
  bool rc { true };

  auto name = hierName(block);

  if      (name == "") {
    for (auto *child : block->children) {
      if (! processTextBlock(child))
        rc = false;
    }
  }
  else if (name == "FBXHeaderExtension") {
    // TODO
  }
  else if (name == "Definitions") {
    // TODO
  }
  else if (name == "GlobalSettings") {
    // TODO
  }
  else if (name == "Documents") {
    // TODO
  }
  else if (name == "References") {
    // TODO
  }
  else if (name == "Objects") {
    for (auto *child : block->children) {
      if (! processObjectsTextBlock(child))
        rc = false;
    }
  }
  else if (name == "Connections") {
    //std::cerr << "Connections\n";

    for (const auto &nv : block->nameValues) {
      //printBlockNameValues(block);

      if (nv.name == "Connect") {
        auto values = readValues(nv.value);

        if (values.size() != 3) {
          errorMsg("Invalid Connections:Connect line '" + nv.value + "'");
          rc = false;
          continue;
        }

        connectionMap_[values[1]] = values[2];
      }
    }
  }
  else if (name == "Relations") {
    // TODO
  }
  else if (name == "Takes") {
    // TODO
  }
  else if (name == "Version5") {
    // TODO
  }
  else {
    errorMsg("Unhandled block : " + name);
    rc = true;
  }

  return rc;
}

bool
CImportFBX::
processObjectsTextBlock(TextBlock *block)
{
  auto readReals = [&](const std::string &value) {
    std::vector<double> reals;

    CStrParse parse(value);

    while (! parse.eof()) {
      parse.skipSpace();

      double r;
      if (! parse.readReal(&r)) {
        errorMsg("Invalid real");
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

  auto readIntegers = [&](const std::string &value) {
    std::vector<int> integers;

    CStrParse parse(value);

    while (! parse.eof()) {
      parse.skipSpace();

      int i;
      if (! parse.readInteger(&i)) {
        errorMsg("Invalid integer");
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

  auto printBlockNameValues = [&](TextBlock *tb) {
    for (const auto &nv : tb->nameValues) {
      std::cerr << nv.name << ":";

      auto values = readValues(nv.value);

      for (const auto &v : values)
        std::cerr << v << " ";
      std::cerr << "\n";
    }
  };

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

  //---

  auto valuesColor = [](const std::vector<std::string> &values, OptColor &c) {
    if (values.size() < 6)
      return false;

    // values[0,1,2] = name, type ("Color"), ?
    // values[3,4,5] = r,g,b
    c = CRGBA(std::stod(values[3]), std::stod(values[4]), std::stod(values[4]));

    return true;
  };

  auto valuesPoint3 = [](const std::vector<std::string> &values, OptPoint3 &p) {
    if (values.size() < 6)
      return false;

    // values[0,1,2] = name, type, ?
    // values[3,4,5] = x,y,z
    p = CPoint3D(std::stod(values[3]), std::stod(values[4]), std::stod(values[4]));

    return true;
  };

  //---

  auto name = hierName(block);

  if      (name == "Objects/Model") {
    auto *geometryData = new GeometryData;

    //---

    auto values = readValues(block->nameValue.value);

    // get model name
    if (values.size() > 0) {
      if (values[0].substr(0, 7) == "Model::") {
        geometryData->name = values[0].substr(7);

        if (isDebug())
          debugMsg("Model: " + geometryData->name);
      }
    }

    // get model type
    enum class ModelType {
      NONE,
      CAMERA_SWITCHER,
      CAMERA,
      LIMB,
      LIGHT,
      MESH,
      STEREO_CAMERA
    };

    ModelType modelType { ModelType::NONE };

    if (values.size() > 1) {
      if      (values[1] == "CameraSwitcher")
        modelType = ModelType::CAMERA_SWITCHER;
      else if (values[1] == "Camera")
        modelType = ModelType::CAMERA;
      else if (values[1] == "Limb")
        modelType = ModelType::LIMB;
      else if (values[1] == "Light")
        modelType = ModelType::LIGHT;
      else if (values[1] == "Mesh")
        modelType = ModelType::MESH;
      else if (values[1] == "Model::stereoCamera")
        modelType = ModelType::STEREO_CAMERA;
      else
        errorMsg("Invalid Model Type : " + values[1]);
    }

    if (modelType != ModelType::MESH)
      return true;

    //---

    VertexData vertexData;
    NormalData normalData;
    ColorData  colorData;
    UVData     uvData;

    for (const auto &nv : block->nameValues) {
      if      (nv.name == "Version") {
      }
      else if (nv.name == "PolygonVertexIndex") {
        vertexData.indices = readIntegers(nv.value);
        vertexData.set     = true;
      }
      else if (nv.name == "Vertices") {
        vertexData.values = readReals(nv.value);
        vertexData.set    = true;
      }
      else if (nv.name == "Edges") {
        geometryData->edges = readIntegers(nv.value);
      }
      else if (nv.name == "NormalsIndex") {
        normalData.indices = readIntegers(nv.value);
        normalData.set     = true;
      }
      else if (nv.name == "Normals") {
        normalData.values = readReals(nv.value);
        normalData.set    = true;
      }
      else if (nv.name == "ColorIndex") {
        colorData.indices = readIntegers(nv.value);
        colorData.set     = true;
      }
      else if (nv.name == "Colors") {
        colorData.values = readReals(nv.value);
        colorData.set    = true;
      }
      else if (nv.name == "UVIndex") {
        uvData.indices = readIntegers(nv.value);
        uvData.set     = true;
      }
      else if (nv.name == "UV") {
        uvData.values = readReals(nv.value);
        uvData.set    = true;
      }
      else if (nv.name == "Materials") {
        geometryData->materials = readIntegers(nv.value);
      }
      else if (nv.name == "MultiLayer") {
      }
      else if (nv.name == "MultiTake") {
      }
      else if (nv.name == "Shading") {
      }
      else if (nv.name == "Culling") {
      }
      else if (nv.name == "Hidden") {
      }
      else if (nv.name == "Name") {
      }
      else if (nv.name == "TypeFlags") {
      }
      else if (nv.name == "GeometryVersion") {
      }
      else if (nv.name == "CameraId") {
      }
      else if (nv.name == "CameraName") {
      }
      else if (nv.name == "CameraIndexName") {
      }
      else if (nv.name == "Position") {
      }
      else if (nv.name == "Up") {
      }
      else if (nv.name == "LookAt") {
      }
      else if (nv.name == "ShowInfoOnMoving") {
      }
      else if (nv.name == "ShowAudio") {
      }
      else if (nv.name == "AudioColor") {
      }
      else if (nv.name == "CameraOrthoZoom") {
      }
      else {
        errorMsg("Unhandled name " + nv.name);
      }
    }

    if (vertexData.set)
      geometryData->vertexData.push_back(vertexData);

    if (normalData.set)
      geometryData->normalData.push_back(normalData);

    if (colorData.set)
      geometryData->colorData.push_back(colorData);

    if (uvData.set)
      geometryData->uvData.push_back(uvData);

    for (auto *child : block->children) {
      if     (child->nameValue.name == "Properties60") {
        for (const auto &nv : child->nameValues) {
          if      (nv.name == "Property") {
            auto values1 = readValues(nv.value);

            if (values1.size() < 2) {
              errorMsg("Invalid " + name + "/" + child->nameValue.name +
                       "/Property values for " + values1[0]);
              continue;
            }

            if      (values1[0] == "QuaternionInterpolate") {
            }
            else if (values1[0] == "Visibility") {
            }
            else if (values1[0] == "Lcl Translation") {
              if (! valuesPoint3(values1, geometryData->translation))
                continue; // error
            }
            else if (values1[0] == "Lcl Rotation") {
              if (! valuesPoint3(values1, geometryData->rotation))
                continue; // error
            }
            else if (values1[0] == "Lcl Scaling") {
              if (! valuesPoint3(values1, geometryData->scale))
                continue; // error
            }
            else if (values1[0] == "RotationOffset") {
            }
            else if (values1[0] == "RotationPivot") {
            }
            else if (values1[0] == "ScalingOffset") {
            }
            else if (values1[0] == "ScalingPivot") {
            }
            else if (values1[0] == "TranslationActive") {
            }
            else if (values1[0] == "TranslationMin") {
            }
            else if (values1[0] == "TranslationMax") {
            }
            else if (values1[0] == "TranslationMinX") {
            }
            else if (values1[0] == "TranslationMinY") {
            }
            else if (values1[0] == "TranslationMinZ") {
            }
            else if (values1[0] == "TranslationMaxX") {
            }
            else if (values1[0] == "TranslationMaxY") {
            }
            else if (values1[0] == "TranslationMaxZ") {
            }
            else if (values1[0] == "RotationOrder") {
            }
            else if (values1[0] == "RotationSpaceForLimitOnly") {
            }
            else if (values1[0] == "AxisLen") {
            }
            else if (values1[0] == "PreRotation") {
            }
            else if (values1[0] == "PostRotation") {
            }
            else if (values1[0] == "RotationActive") {
            }
            else if (values1[0] == "RotationMin") {
            }
            else if (values1[0] == "RotationMax") {
            }
            else if (values1[0] == "RotationMinX") {
            }
            else if (values1[0] == "RotationMinY") {
            }
            else if (values1[0] == "RotationMinZ") {
            }
            else if (values1[0] == "RotationMaxX") {
            }
            else if (values1[0] == "RotationMaxY") {
            }
            else if (values1[0] == "RotationMaxZ") {
            }
            else if (values1[0] == "RotationStiffnessX") {
            }
            else if (values1[0] == "RotationStiffnessY") {
            }
            else if (values1[0] == "RotationStiffnessZ") {
            }
            else if (values1[0] == "MinDampRangeX") {
            }
            else if (values1[0] == "MinDampRangeY") {
            }
            else if (values1[0] == "MinDampRangeZ") {
            }
            else if (values1[0] == "MaxDampRangeX") {
            }
            else if (values1[0] == "MaxDampRangeY") {
            }
            else if (values1[0] == "MaxDampRangeZ") {
            }
            else if (values1[0] == "MinDampStrengthX") {
            }
            else if (values1[0] == "MinDampStrengthY") {
            }
            else if (values1[0] == "MinDampStrengthZ") {
            }
            else if (values1[0] == "MaxDampStrengthX") {
            }
            else if (values1[0] == "MaxDampStrengthY") {
            }
            else if (values1[0] == "MaxDampStrengthZ") {
            }
            else if (values1[0] == "PreferedAngleX") {
            }
            else if (values1[0] == "PreferedAngleY") {
            }
            else if (values1[0] == "PreferedAngleZ") {
            }
            else if (values1[0] == "InheritType") {
            }
            else if (values1[0] == "ScalingActive") {
            }
            else if (values1[0] == "ScalingMin") {
            }
            else if (values1[0] == "ScalingMax") {
            }
            else if (values1[0] == "ScalingMinX") {
            }
            else if (values1[0] == "ScalingMinY") {
            }
            else if (values1[0] == "ScalingMinZ") {
            }
            else if (values1[0] == "ScalingMaxX") {
            }
            else if (values1[0] == "ScalingMaxY") {
            }
            else if (values1[0] == "ScalingMaxZ") {
            }
            else if (values1[0] == "GeometricTranslation") {
            }
            else if (values1[0] == "GeometricRotation") {
            }
            else if (values1[0] == "GeometricScaling") {
            }
            else if (values1[0] == "Show") {
            }
            else if (values1[0] == "NegativePercentShapeSupport") {
            }
            else if (values1[0] == "DefaultAttributeIndex") {
            }
            else if (values1[0] == "Color") {
            }
            else if (values1[0] == "Size") {
            }
            else if (values1[0] == "Look") {
            }
            else if (values1[0] == "LookAtProperty") {
            }
            else if (values1[0] == "UpVectorProperty") {
            }
            else
              unhandledProperty(name, values1[0]);
          }
          else {
            errorMsg("Invalid property " + name + " " + nv.name);
          }
        }
      }
      else if (child->nameValue.name == "LayerElementNormal") {
        NormalData normalData1;

        for (const auto &nv1 : child->nameValues) {
          if      (nv1.name == "Version") {
          }
          else if (nv1.name == "Name") {
          }
          else if (nv1.name == "MappingInformationType") {
          }
          else if (nv1.name == "ReferenceInformationType") {
          }
          else if (nv1.name == "NormalsIndex") {
            normalData1.indices = readIntegers(nv1.value);
            normalData1.set     = true;
          }
          else if (nv1.name == "Normals") {
            normalData1.values = readReals(nv1.value);
            normalData1.set     = true;
          }
          else if (nv1.name == "NormalsW") {
          }
          else
            errorMsg("Invalid LayerElementNormal Name " + nv1.name);
        }

        if (normalData1.set)
          geometryData->normalData.push_back(normalData1);
      }
      else if (child->nameValue.name == "LayerElementSmoothing") {
        // Smoothing
        //geometryData->normals = readIntegers(nv.value);
      }
      else if (child->nameValue.name == "LayerElementUV") {
        UVData uvData1;

        for (const auto &nv1 : child->nameValues) {
          if      (nv1.name == "Version") {
          }
          else if (nv1.name == "Name") {
          }
          else if (nv1.name == "MappingInformationType") {
          }
          else if (nv1.name == "ReferenceInformationType") {
          }
          else if (nv1.name == "UVIndex") {
            uvData1.indices = readIntegers(nv1.value);
            uvData1.set     = true;
          }
          else if (nv1.name == "UV") {
            uvData1.values = readReals(nv1.value);
            uvData1.set    = true;
          }
          else
            errorMsg("Invalid LayerElementUV Name " + nv1.name);
        }

        if (uvData1.set)
          geometryData->uvData.push_back(uvData1);
      }
      else if (child->nameValue.name == "LayerElementTexture") {
        for (const auto &nv1 : child->nameValues) {
          if      (nv1.name == "Version") {
          }
          else if (nv1.name == "Name") {
          }
          else if (nv1.name == "MappingInformationType") {
          }
          else if (nv1.name == "ReferenceInformationType") {
          }
          else if (nv1.name == "BlendMode") {
          }
          else if (nv1.name == "TextureAlpha") {
          }
          else if (nv1.name == "TextureId") {
            geometryData->textures = readIntegers(nv1.value);
          }
          else
            errorMsg("Invalid LayerElementTexture Name " + nv1.name);
        }
      }
      else if (child->nameValue.name == "LayerElementMaterial") {
        for (const auto &nv1 : child->nameValues) {
          if      (nv1.name == "Version") {
          }
          else if (nv1.name == "Name") {
          }
          else if (nv1.name == "MappingInformationType") {
          }
          else if (nv1.name == "ReferenceInformationType") {
          }
          else if (nv1.name == "Materials") {
            geometryData->materials = readIntegers(nv1.value);
          }
          else
            errorMsg("Invalid LayerElementMaterial Name " + nv1.name);
        }
      }
      else if (child->nameValue.name == "Layer") {
      }
      else {
        errorMsg("Invalid Objects/Model(Mesh) Child " + child->nameValue.name);
      }
    }

    if (isDebug()) {
      for (auto &data : geometryData->vertexData) {
        printIntegers("PolygonVertexIndex", data.indices);
        printReals   ("Vertices"          , data.values);
      }

      for (auto &data : geometryData->normalData) {
        printIntegers("NormalsIndex", data.indices);
        printReals   ("Normals"     , data.values);
      }

      for (auto &data : geometryData->colorData) {
        printIntegers("ColorIndex", data.indices);
        printReals   ("Colors"    , data.values);
      }

      for (auto &data : geometryData->uvData) {
        printIntegers("UVIndex", data.indices);
        printReals   ("UV"     , data.values);
      }

      printIntegers("Edges", geometryData->edges);

      printIntegers("Materials", geometryData->materials);
    }

    addGeometryObject(geometryData);
  }
  else if (name == "Objects/Material") {
    //printBlockNameValues(block);

    auto values = readValues(block->nameValue.value);

    auto *materialData = new MaterialData;

    materialData->name = values[0];

    for (auto *child : block->children) {
      if (child->nameValue.name == "Properties60") {
        for (const auto &nv : child->nameValues) {
          if (nv.name == "Property") {
            auto values1 = readValues(nv.value);

            if (values1.size() < 3) {
              errorMsg("Invalid " + name + "/" + child->nameValue.name + "/Property values");
              continue;
            }

            if      (values1[0] == "Ambient" || values1[0] == "AmbientColor") {
              if (! valuesColor(values1, materialData->ambientColor))
                continue; // error
            }
            else if (values1[0] == "AmbientFactor") {
              materialData->ambientFactor = std::stod(values1[3]);
            }
            else if (values1[0] == "Bump") {
            }
            else if (values1[0] == "BumpFactor") {
              materialData->bumpFactor = std::stod(values1[3]);
            }
            else if (values1[0] == "Diffuse" || values1[0] == "DiffuseColor") {
              if (! valuesColor(values1, materialData->diffuseColor))
                continue; // error
            }
            else if (values1[0] == "DiffuseFactor") {
              materialData->diffuseFactor = std::stod(values1[3]);
            }
            else if (values1[0] == "Emissive" || values1[0] == "EmissiveColor") {
              if (! valuesColor(values1, materialData->emissionColor))
                continue; // error
            }
            else if (values1[0] == "EmissiveFactor") {
              materialData->emissionFactor = std::stod(values1[3]);
            }
            else if (values1[0] == "MultiLayer") {
            }
            else if (values1[0] == "Opacity") {
              materialData->opacity = std::stod(values1[3]);
            }
            else if (values1[0] == "ReflectionColor") {
              if (! valuesColor(values1, materialData->reflectionColor))
                continue; // error
            }
            else if (values1[0] == "ReflectionFactor") {
              materialData->reflectionFactor = std::stod(values1[3]);
            }
            else if (values1[0] == "Reflectivity") {
              materialData->reflectivity = std::stod(values1[3]);
            }
            else if (values1[0] == "Shininess" || values1[0] == "ShininessExponent") {
              materialData->shininess = std::stod(values1[3]);
            }
            else if (values1[0] == "Specular" || values1[0] == "SpecularColor") {
              if (! valuesColor(values1, materialData->specularColor))
                continue; // error
            }
            else if (values1[0] == "SpecularFactor") {
              materialData->specularFactor = std::stod(values1[3]);
            }
            else if (values1[0] == "ShadingModel") {
            }
            else if (values1[0] == "TransparentColor") {
              if (! valuesColor(values1, materialData->transparencyColor))
                continue; // error
            }
            else if (values1[0] == "TransparencyFactor") {
              materialData->transparencyFactor = std::stod(values1[3]);
            }
            else {
              unhandledProperty(name + "/" + child->nameValue.name + "/" + nv.name, values1[0]);
              continue;
            }
          }
          else {
            errorMsg("Invalid " + name + "/" + child->nameValue.name + " Property " + nv.name);
          }
        }
      }
      else {
        errorMsg("Invalid " + name + " Child " + child->nameValue.name);
      }
    }

    if (materialData->name != "") {
      auto materialName = materialData->name;

      if (materialName.substr(0, 10) == "Material::")
        materialName = materialName.substr(10);

      materialDataMap_[materialName] = materialData;

      if (isDebug())
        printMaterialData(materialData);
    }
  }
  else if (name == "Objects/Texture") {
    //printBlockNameValues(block);

    auto values = readValues(block->nameValue.value);

    auto *textureData = new TextureData;

    textureData->name = values[0];

    for (const auto &nv : block->nameValues) {
      auto values1 = readValues(nv.value);

      if      (nv.name == "TextureName") {
        textureData->name = values1[0];
      }
      else if (nv.name == "FileName") {
        textureData->fileName = values1[0];
      }
    }

    for (auto *child : block->children) {
      //printBlockNameValues(child);

      if (child->nameValue.name == "Properties60") {
        for (const auto &nv : child->nameValues) {
          if (nv.name == "Property") {
            auto values1 = readValues(nv.value);

            if (values1.size() < 3) {
              errorMsg("Invalid " + name + "/" + child->nameValue.name + "/Property values");
              continue;
            }

            if      (values1[0] == "Translation") {
            }
            else if (values1[0] == "Rotation") {
            }
            else if (values1[0] == "Scaling") {
            }
            else if (values1[0] == "Texture alpha") {
            }
            else if (values1[0] == "TextureTypeUse") {
            }
            else if (values1[0] == "CurrentTextureBlendMode") {
            }
            else if (values1[0] == "UseMaterial") {
            }
            else if (values1[0] == "UseMipMap") {
            }
            else if (values1[0] == "CurrentMappingType") {
            }
            else if (values1[0] == "UVSwap") {
            }
            else if (values1[0] == "WrapModeU") {
            }
            else if (values1[0] == "WrapModeV") {
            }
            else if (values1[0] == "TextureRotationPivot") {
            }
            else if (values1[0] == "TextureScalingPivot") {
            }
            else if (values1[0] == "VideoProperty") {
            }
            else
              unhandledProperty(name + "/" + child->nameValue.name + "/" + nv.name, values1[0]);
          }
          else {
            errorMsg("Invalid " + name + "/" + child->nameValue.name + "/" + nv.name);
          }
        }
      }
      else {
        errorMsg("Invalid " + name + " Child " + child->nameValue.name);
      }
    }

    if (textureData->name != "") {
      auto textureName = textureData->name;

      if (textureName.substr(0, 9) == "Texture::")
        textureName = textureName.substr(9);

      textureDataMap_[textureName] = textureData;

      if (isDebug()) {
        debugMsg("Texture: " + textureName + " " + textureData->fileName);
      }
    }
  }
  else if (name == "Objects/GlobalSettings") {
    if (isDebug())
      printBlockNameValues(block);
  }
  else if (name == "Objects/Video") {
    // TODO
  }
  else if (name == "Objects/Deformer") {
    // TODO
  }
  else if (name == "Objects/Pose") {
    //printBlockNameValues(block);

    for (const auto &nv : block->nameValues) {
      errorMsg("Unhandled name " + nv.name);
    }

    for (auto *child : block->children) {
      //printBlockNameValues(child);

      errorMsg("Invalid " + name + " Child " + child->nameValue.name);
    }
  }
  else if (name == "Objects/NodeAttribute") {
    // TODO
  }
  else if (name == "Objects/AnimationStack") {
    // TODO
  }
  else if (name == "Objects/AnimationLayer") {
    // TODO
  }
  else if (name == "Objects/AnimationCurveNode") {
    // TODO
  }
  else if (name == "Objects/Geometry") {
    // TODO
  }
  else
    errorMsg("Unhandled block : " + name);

  //---

  return true;
}

std::vector<std::string>
CImportFBX::
readValues(const std::string &value) const
{
  std::vector<std::string> values;

  CStrParse parse(value);

  while (! parse.eof()) {
    parse.skipSpace();

    if (parse.isChar('"')) {
      parse.skipChar();

      std::string str;
      while (! parse.eof() && ! parse.isChar('"'))
        str += parse.readChar();

      parse.skipChar();

      values.push_back(str);
    }
    else {
      double r;
      if (! parse.readReal(&r)) {
        errorMsg("Invalid real");
        break;
      }

      values.push_back(std::to_string(r));
    }

    parse.skipSpace();

    if (! parse.eof() && parse.isChar(',')) {
      parse.skipChar();

      parse.skipSpace();
    }
  }

  return values;
};

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

  processDataTree(propDataTree_);

  //---

#if 0
  std::vector<ModelData *> rootModels;

  for (const auto &pm : idModelData_) {
    auto *modelData = pm.second;

    if (! modelData->parent)
      rootModels.push_back(modelData);
  }

  for (auto *modelData : rootModels)
    processHierModel(modelData, 0);
#endif

  //---

  for (const auto &pg : idGeometryData_) {
    auto *geometryData = pg.second;

    addGeometryObject(geometryData);

    //---

    // set name and transform
    auto *modelData = geometryData->modelData;

    if (! modelData || ! geometryData->object)
      continue;

    geometryData->object->setName(modelData->name);

#if 0
    auto name = calcModelDataHierName(modelData);

    std::vector<TRSData> hierTRS;
    calcModelDataHierTRS(modelData, hierTRS);

    if (hierTRS.size() > 1) {
      auto printTRS = [](const TRSData &trs, int depth) {
        std::string prefix = "  ";
        for (int i = 0; i < depth; ++i)
          prefix += "  ";

        if (trs.translation) std::cerr << prefix << "T: " << *trs.translation << "\n";
        if (trs.rotation   ) std::cerr << prefix << "R: " << *trs.rotation    << "\n";
        if (trs.scaling    ) std::cerr << prefix << "S: " << *trs.scaling     << "\n";
      };

      std::cerr << "Model: " << name << "\n";
      std::cerr << " Local:\n";
      printTRS(modelData->localTRS, 0);

      std::cerr << " Hier :\n";
      int i = 0;
      for (const auto &trs1 : hierTRS)
        printTRS(trs1, i++);
    }
#endif

    auto localTransform = calcModelDataLocalTransform(modelData);
    geometryData->object->transform(localTransform);

    //---

    setGeometryTextures(geometryData);
  }

  //---

  for (const auto &pm : idModelData_) {
    auto *modelData = pm.second;

    if (modelData->geometryData)
      addGeometryObject(modelData->geometryData);
  }

  //---

#if 0
  for (const auto &pt : idTextureData_) {
    auto *textureData = pt.second;
    if (! textureData->texture) continue;

    for (auto *geometryData : textureData->modelData->geometryDataList) {
      // set texture
      if (textureData->type == "NormalMap")
        geometryData->object->setNormalTexture(textureData->texture);
      else
        geometryData->object->setDiffuseTexture(textureData->texture);
    }
  }
#endif

  //---

  if (! animationStackData_.empty()) {
    std::cerr << "Animation Stacks\n";
    for (const auto &pas : animationStackData_) {
      auto *animationStack = pas.second;

      std::cerr << " Anim: " << animationStack->name << "\n";

      for (auto *animationLayer : animationStack->animationLayers) {
        std::cerr << "  Layer: " << animationLayer->name << "\n";

#if 0
        for (auto *animationCurveNode : animationLayer->animationCurveNodes) {
          std::cerr << "   Curve:" << animationCurveNode->type << " " <<
                       animationCurveNode->p << "\n";
        }
#endif
      }
    }
  }

  //---

#if 0
  if (! animationDeformerData_.empty()) {
    std::cerr << "Animation Deformers\n";
    for (const auto &pad : animationDeformerData_) {
      auto *animationDeformer = pad.second;

      std::cerr << " Deformer: " << animationDeformer->name << "\n";

      if (! animationDeformer->children.empty()) {
        std::cerr << " ";
        for (const auto &animationDeformer1 : animationDeformer->children)
          std::cerr << " " << animationDeformer1->name;
        std::cerr << "\n";
      }
    }
  }
#endif

  //---

#if 0
  for (const auto &pg : idGeometryData_) {
    auto *geometryData = pg.second;

    if (! geometryData->modelData || ! geometryData->modelData->parent)
      continue;

    std::cerr << geometryData->modelData->name << " " <<
                 geometryData->modelData->parent->name << "\n";
  }
#endif

  return true;
}

void
CImportFBX::
processHierModel(ModelData *modelData, int depth)
{
  std::string prefix;
  for (int i = 0; i < depth; ++i)
    prefix += "  ";

  std::cerr << prefix << "Model: " << modelData->name << "\n";

  if (! modelData->geometryDataList.empty()) {
    int j = 0;

    std::cerr << prefix << "  Geometry: ";

    for (auto *geometryData : modelData->geometryDataList) {
      if (j > 0)
        std::cerr << ", ";

      std::cerr << geometryData->name;

      ++j;
    }

    std::cerr << "\n";
  }

  for (auto *modelData1 : modelData->children) {
    processHierModel(modelData1, depth + 1);
  }
}

void
CImportFBX::
dumpTree(PropDataTree *tree, int depth)
{
  dumpDataMap(tree, depth);

  for (auto *tree1 : tree->children)
    dumpTree(tree1, depth + 2);
}

void
CImportFBX::
dumpDataMap(PropDataTree *tree, int depth)
{
  std::string prefix;

  if (depth > 0) {
    for (int i = 0; i < depth; ++i)
      prefix += " ";

    auto name = (isHierName() ? hierName(tree) : tree->name);

    std::cerr << prefix << name << "\n";

    prefix += " ";
  }

  for (const auto &pd : tree->dataMap) {
    const auto &name = pd.first;

    std::cerr << prefix << name << ":\n";

    for (const auto &pa : pd.second) {
      for (const auto &pa1 : pa.second) {
        const auto &ind  = pa1.first;
        auto       *data = pa1.second;

        std::cerr << prefix << " [" << ind << "] ";
        std::cerr << *data;
        std::cerr << "\n";
      }
    }
  }
}

std::string
CImportFBX::
hierName(PropDataTree *tree) const
{
  if (! tree->parent || tree->parent->name == "")
    return tree->name;

  return hierName(tree->parent) + "/" + tree->name;
}

std::string
CImportFBX::
hierName(TextBlock *block) const
{
  if (! block->parent || block->parent->nameValue.name == "")
    return block->nameValue.name;

  return hierName(block->parent) + "/" + block->nameValue.name;
}

void
CImportFBX::
processDataTree(PropDataTree *tree)
{
  auto getPropertyName = [](const PropIndDataArray &pa, int ind=0, const std::string &def="") {
    int ind1 = 0;

    for (const auto &pa1 : pa) {
      for (const auto &pa2 : pa1.second) {
        if (ind1 == ind) {
          auto *data = pa2.second;

          return data->toString();
        }
      }

      ++ind1;
    }

    return def;
  };

  auto getPropertyInt = [&](const PropIndDataArray &pa, int ind=0) {
    int ind1 = 0;

    for (const auto &pa1 : pa) {
      for (const auto &pa2 : pa1.second) {
        if (ind1 == ind) {
          auto *data = pa2.second;

          return data->toLong();
        }
      }

      ++ind1;
    }

    return 0L;
  };

  auto getPropertyReal = [&](const PropIndDataArray &pa, int ind=0) {
    int ind1 = 0;

    for (const auto &pa1 : pa) {
      for (const auto &pa2 : pa1.second) {
        if (ind1 == ind) {
          auto *data = pa2.second;

          return data->toReal();
        }
      }

      ++ind1;
    }

    return 0.0;
  };

  auto getPropertyIntArray = [&](const PropIndDataArray &pa, int ind=0) {
    int ind1 = 0;

    for (const auto &pa1 : pa) {
      for (const auto &pa2 : pa1.second) {
        if (ind1 == ind) {
          auto *data = pa2.second;

          return data->toIntArray();
        }
      }

      ++ind1;
    }

    return std::vector<int>();
  };

   auto getPropertyLongArray = [&](const PropIndDataArray &pa, int ind=0) {
    int ind1 = 0;

    for (const auto &pa1 : pa) {
      for (const auto &pa2 : pa1.second) {
        if (ind1 == ind) {
          auto *data = pa2.second;

          return data->toLongArray();
        }
      }

      ++ind1;
    }

    return std::vector<long>();
  };

  auto getPropertyFloatArray = [&](const PropIndDataArray &pa, int ind=0) {
    int ind1 = 0;

    for (const auto &pa1 : pa) {
      for (const auto &pa2 : pa1.second) {
        if (ind1 == ind) {
          auto *data = pa2.second;

          return data->toFloatArray();
        }
      }

      ++ind1;
    }

    return std::vector<float>();
  };

  auto getPropertyDoubleArray = [&](const PropIndDataArray &pa, int ind=0) {
    int ind1 = 0;

    for (const auto &pa1 : pa) {
      for (const auto &pa2 : pa1.second) {
        if (ind1 == ind) {
          auto *data = pa2.second;

          return data->toDoubleArray();
        }
      }

      ++ind1;
    }

    return std::vector<double>();
  };

  auto getPropertyArrayName = [](const PropDataArray &pa, int ind=0, const std::string &def="") {
    int ind1 = 0;

    for (const auto &pa1 : pa) {
      if (ind1 == ind) {
        auto *data = pa1.second;

        return data->toString();
      }

      ++ind1;
    }

    return def;
  };

  auto getPropertyArrayReal = [&](const PropDataArray &pa, int ind=0) {
    try {
      return std::stod(getPropertyArrayName(pa, ind, "0"));
    }
    catch (...) {
      return 0.0;
    }
  };

  auto getPropertyArrayInt = [&](const PropDataArray &pa, int ind=0) {
    try {
      return std::stol(getPropertyArrayName(pa, ind, "0"));
    }
    catch (...) {
      return 0L;
    }
  };

  auto getPropertyArrayIndName = [&](const PropDataArray &pa, int ind=0) {
    int ind1 = 0;

    for (const auto &pa1 : pa) {
      if (ind1 == ind) {
        auto *data = pa1.second;

        if      (data->type() == DataType::LONG)
          return IndName(data->toLong());
        else if (data->type() == DataType::STRING)
          return IndName(data->toString());
        else {
          std::cerr << "Invalid ind name type: " << *data << "\n";
          return IndName();
        }
      }

      ++ind1;
    }

    return IndName();
  };

  auto getPropertyInts = [&](const PropIndDataArray &pa, int ind=0) {
    int ind1 = 0;

    std::vector<int> i;

    for (const auto &pa1 : pa) {
      if (ind1 == ind) {
        for (const auto &pa2 : pa1.second) {
          auto *data = pa2.second;

          i.push_back(int(data->toLong()));
        }

        break;
      }
    }

    return i;
  };

  auto getPropertyDoubles = [&](const PropIndDataArray &pa, int ind=0) {
    int ind1 = 0;

    std::vector<double> r;

    for (const auto &pa1 : pa) {
      if (ind1 == ind) {
        for (const auto &pa2 : pa1.second) {
          auto *data = pa2.second;

          r.push_back(data->toReal());
        }

        break;
      }
    }

    return r;
  };

#if 0
  auto printPropertyNames = [&](PropDataTree *t) {
    auto treeName = hierName(t);

    std::cerr << treeName;

    for (const auto &pd : t->dataMap) {
      auto propName = getPropertyName(pd.second);

      std::cerr << " " << pd.first << " : " << propName << "\n";
    }
  };
#endif

#if 0
  auto printPropertyNameData = [&](PropDataTree *t) {
    auto treeName = hierName(t);

    std::cerr << treeName;

    for (const auto &pd : t->dataMap) {
      std::cerr << " " << pd.first << " [";

      int ind = 0;

      for (const auto &pa1 : pd.second) {
        auto *data = pa1.second;

        if (ind > 0)
          std::cerr << ", ";

        std::cerr << data->toString();

        ++ind;
      }

      std::cerr << "]";
    }

    std::cerr << "\n";
  };
#endif

  auto printParentDataMap = [&](PropDataTree *t) {
    auto treeName = hierName(t);

    std::cerr << treeName << " ";

    for (const auto &pa1 : t->parentDataMap) {
      auto *data1 = pa1.second;

      std::cerr << " " << data1->toString();
    }

    std::cerr << "\n";
  };

  auto unhandledDataName = [&](const std::string &path, const std::string &name) {
    std::cerr << "Unhandled data name " << path << "/" << name << "\n";
  };

  //---

  struct BlockData {
    IndName     id;
    std::string name;
    std::string type;
  };

  auto readBlockDetails = [&](PropDataTree *t, BlockData &blockData, const std::string &id) {
    int ind = 0;

    for (const auto &pd : t->parentDataMap) {
      auto *data = pd.second;

      if      (ind == 0) {
        if      (data->type() == DataType::LONG)
          blockData.id = IndName(data->toLong());
        else if (data->type() == DataType::STRING)
          blockData.id = IndName(data->toString());
        else
          std::cerr << "Invalid block id: " << *data << "\n";
      }
      else if (ind == 1)
        blockData.name = data->toString();
      else if (ind == 2)
        blockData.type = data->toString();

      //std::cerr << " '" << data->toString() << "'\n";

      ++ind;
    }

    idType_[blockData.id] = id;

    return true;
  };

#if 0
  auto readParentData = [&](PropDataTree *t, int ind=0, const std::string &def="") {
    int ind1 = 0;

    for (const auto &pd : t->parentDataMap) {
      if (ind1 == ind) {
        auto *data = pd.second;

        return data->toString();
      }

      ++ind1;
    }

    return def;
  };

  auto readParentInt = [&](PropDataTree *t, int ind=0) {
    return std::stol(readParentData(t, ind, "0"));
  };
#endif

  //---

  std::string propertyTemplate;

  //---

  for (auto *tree1 : tree->children) {
    auto treeName1 = hierName(tree1);

    if      (treeName1 == "FBXHeaderExtension") {
    }
    else if (treeName1 == "GlobalSettings") {
      //printPropertyNames(tree1);

      for (auto *tree2 : tree1->children) {
        auto treeName2 = hierName(tree2);

        if (treeName2 == "GlobalSettings/Properties70") {
          for (const auto &pd2 : tree2->dataMap) {
            if      (pd2.first == "P") {
              for (const auto &pa2 : pd2.second) {
                auto propName2 = getPropertyArrayName(pa2.second);

                if      (propName2 == "AmbientColor") {
                }
                else if (propName2 == "CoordAxis") {
                  globalData_.coordAxis = getPropertyArrayInt(pa2.second, 4);
                  //std::cerr << propName2 << " " << globalData_.coordAxis << "\n";
                }
                else if (propName2 == "CoordAxisSign") {
                  globalData_.coordAxisSign = getPropertyArrayInt(pa2.second, 4);
                  //std::cerr << propName2 << " " << globalData_.coordAxisSign << "\n";
                }
                else if (propName2 == "CustomFrameRate") {
                }
                else if (propName2 == "CurrentTimeMarker") {
                }
                else if (propName2 == "DefaultCamera") {
                }
                else if (propName2 == "FrontAxis") {
                  globalData_.frontAxis = getPropertyArrayInt(pa2.second, 4);
                  //std::cerr << propName2 << " " << globalData_.frontAxis << "\n";
                }
                else if (propName2 == "FrontAxisSign") {
                  globalData_.frontAxisSign = getPropertyArrayInt(pa2.second, 4);
                  //std::cerr << propName2 << " " << globalData_.frontAxisSign << "\n";
                }
                else if (propName2 == "OriginalUnitScaleFactor") {
                }
                else if (propName2 == "OriginalUpAxis") {
                }
                else if (propName2 == "OriginalUpAxisSign") {
                }
                else if (propName2 == "SnapOnFrameMode") {
                }
                else if (propName2 == "TimeMarker") {
                }
                else if (propName2 == "TimeMode") {
                }
                else if (propName2 == "TimeProtocol") {
                }
                else if (propName2 == "TimeSpanStart") {
                }
                else if (propName2 == "TimeSpanStop") {
                }
                else if (propName2 == "UnitScaleFactor") {
                  globalData_.unitScaleFactor = getPropertyArrayReal(pa2.second, 4);
                  //std::cerr << propName2 << " " << globalData_.unitScaleFactor << "\n";
                }
                else if (propName2 == "UpAxis") {
                  globalData_.upAxis = getPropertyArrayInt(pa2.second, 4);
                  //std::cerr << propName2 << " " << globalData_.upAxis << "\n";
                }
                else if (propName2 == "UpAxisSign") {
                  globalData_.upAxisSign = getPropertyArrayInt(pa2.second, 4);
                  //std::cerr << propName2 << " " << globalData_.upAxisSign << "\n";
                }
                else
                  unhandledProperty(treeName2, propName2);
              }
            }
            else
              unhandledProperty(treeName2, pd2.first);
          }
        }
        else
          unhandledTree(treeName2);
      }
    }
    else if (treeName1 == "Documents") {
    }
    else if (treeName1 == "References") {
    }
    else if (treeName1 == "Definitions") {
      //printPropertyNames(tree1);

      for (auto *tree2 : tree1->children) {
        for (const auto &pd1 : tree1->dataMap) {
          if      (pd1.first == "Count") {
          }
          else if (pd1.first == "ObjectType") {
          }
          else if (pd1.first == "Version") {
          }
          else
            unhandledDataName(treeName1, pd1.first);
        }

        auto treeName2 = hierName(tree2);

        if (treeName2 == "Definitions/ObjectType") {
          propertyTemplate = "";

          for (auto *tree3 : tree2->children) {
            for (const auto &pd2 : tree2->dataMap) {
              if      (pd2.first == "Count") {
              }
              else if (pd2.first == "PropertyTemplate") {
                for (const auto &pa2 : pd2.second) {
                  for (const auto &pa3 : pa2.second) {
                    auto *data2 = pa3.second;

                    propertyTemplate = data2->toString();

                    break;
                  }
                }
              }
              else
                unhandledDataName(treeName2, pd2.first);
            }

            auto treeName3 = hierName(tree3);

            if (treeName3 == "Definitions/ObjectType/PropertyTemplate") {
              for (const auto &pd3 : tree3->dataMap) {
                unhandledDataName(treeName3, pd3.first);
              }

              for (auto *tree4 : tree3->children) {
                auto treeName4 = hierName(tree4);

                if (treeName4 == "Definitions/ObjectType/PropertyTemplate/Properties70") {
                  //std::cerr << "PropertyTemplate: " << propertyTemplate << "\n";

                  // printPropertyNameData(tree4);
                  for (const auto &pd4 : tree4->dataMap) {
                    if      (pd4.first == "P") {
                      for (const auto &pa4 : pd4.second) {
                        auto propName = getPropertyArrayName(pa4.second);

#if 0
                        for (const auto &pa5 : pa4.second) {
                          auto *data4 = pa5.second;

                          unhandledDataName(treeName4, data4->toString());

                          break;
                        }
#endif

                        if      (propertyTemplate == "FbxMesh" ||
                                 propertyTemplate == "KFbxMesh") {
                          if      (propName == "Color") {
                          }
                          else if (propName == "BBoxMin") {
                          }
                          else if (propName == "BBoxMax") {
                          }
                          else if (propName == "Primary Visibility") {
                          }
                          else if (propName == "Casts Shadows") {
                          }
                          else if (propName == "Receive Shadows") {
                          }
                          else
                            unhandledDataName(treeName4, propertyTemplate + "/" + propName);
                        }
                        else if (propertyTemplate == "FbxNode" ||
                                 propertyTemplate == "KFbxNode") {
                          if      (propName == "QuaternionInterpolate") {
                          }
                          else if (propName == "RotationOffset") {
                          }
                          else if (propName == "RotationPivot") {
                          }
                          else if (propName == "ScalingOffset") {
                          }
                          else if (propName == "ScalingPivot") {
                          }
                          else if (propName == "ScalingPivot") {
                          }
                          else if (propName == "TranslationActive") {
                          }
                          else if (propName == "TranslationMin") {
                          }
                          else if (propName == "TranslationMax") {
                          }
                          else if (propName == "TranslationMinX") {
                          }
                          else if (propName == "TranslationMinX") {
                          }
                          else if (propName == "TranslationMinY") {
                          }
                          else if (propName == "TranslationMinZ") {
                          }
                          else if (propName == "TranslationMaxX") {
                          }
                          else if (propName == "TranslationMaxY") {
                          }
                          else if (propName == "TranslationMaxZ") {
                          }
                          else if (propName == "RotationOrder") {
                          }
                          else if (propName == "RotationSpaceForLimitOnly") {
                          }
                          else if (propName == "AxisLen") {
                          }
                          else if (propName == "PreRotation") {
                          }
                          else if (propName == "PostRotation") {
                          }
                          else if (propName == "RotationActive") {
                          }
                          else if (propName == "RotationMin") {
                          }
                          else if (propName == "RotationMax") {
                          }
                          else if (propName == "RotationMinX") {
                          }
                          else if (propName == "RotationMinY") {
                          }
                          else if (propName == "RotationMinZ") {
                          }
                          else if (propName == "RotationMaxX") {
                          }
                          else if (propName == "RotationMaxY") {
                          }
                          else if (propName == "RotationMaxZ") {
                          }
                          else if (propName == "RotationStiffnessX") {
                          }
                          else if (propName == "RotationStiffnessY") {
                          }
                          else if (propName == "RotationStiffnessZ") {
                          }
                          else if (propName == "MinDampRangeX") {
                          }
                          else if (propName == "MinDampRangeY") {
                          }
                          else if (propName == "MinDampRangeZ") {
                          }
                          else if (propName == "MaxDampRangeX") {
                          }
                          else if (propName == "MaxDampRangeY") {
                          }
                          else if (propName == "MaxDampRangeZ") {
                          }
                          else if (propName == "MinDampStrengthX") {
                          }
                          else if (propName == "MinDampStrengthY") {
                          }
                          else if (propName == "MinDampStrengthZ") {
                          }
                          else if (propName == "MaxDampStrengthX") {
                          }
                          else if (propName == "MaxDampStrengthY") {
                          }
                          else if (propName == "MaxDampStrengthZ") {
                          }
                          else if (propName == "PreferedAngleX") {
                          }
                          else if (propName == "PreferedAngleY") {
                          }
                          else if (propName == "PreferedAngleZ") {
                          }
                          else if (propName == "InheritType") {
                          }
                          else if (propName == "ScalingActive") {
                          }
                          else if (propName == "ScalingMin") {
                          }
                          else if (propName == "ScalingMax") {
                          }
                          else if (propName == "ScalingMinX") {
                          }
                          else if (propName == "ScalingMinY") {
                          }
                          else if (propName == "ScalingMinZ") {
                          }
                          else if (propName == "ScalingMaxX") {
                          }
                          else if (propName == "ScalingMaxY") {
                          }
                          else if (propName == "ScalingMaxZ") {
                          }
                          else if (propName == "GeometricTranslation") {
                          }
                          else if (propName == "GeometricRotation") {
                          }
                          else if (propName == "GeometricScaling") {
                          }
                          else if (propName == "Show") {
                          }
                          else if (propName == "NegativePercentShapeSupport") {
                          }
                          else if (propName == "DefaultAttributeIndex") {
                          }
                          else if (propName == "Color") {
                          }
                          else if (propName == "Size") {
                          }
                          else if (propName == "Look") {
                          }
                          else if (propName == "LookAtProperty") {
                          }
                          else if (propName == "UpVectorProperty") {
                          }
                          else if (propName == "Freeze") {
                          }
                          else if (propName == "LODBox") {
                          }
                          else if (propName == "Lcl Translation") {
                            auto x = getPropertyArrayReal(pa4.second, 4);
                            auto y = getPropertyArrayReal(pa4.second, 5);
                            auto z = getPropertyArrayReal(pa4.second, 6);

                            auto localTranslation = CPoint3D(x, y, z);

                            if (isDebug())
                              std::cerr << "  localTranslation: " << localTranslation << "\n";
                          }
                          else if (propName == "Lcl Rotation") {
                            auto x = getPropertyArrayReal(pa4.second, 4);
                            auto y = getPropertyArrayReal(pa4.second, 5);
                            auto z = getPropertyArrayReal(pa4.second, 6);

                            auto localRotation = CPoint3D(x, y, z);

                            if (isDebug())
                              std::cerr << "  localRotation: " << localRotation << "\n";
                          }
                          else if (propName == "Lcl Scaling") {
                            auto x = getPropertyArrayReal(pa4.second, 4);
                            auto y = getPropertyArrayReal(pa4.second, 5);
                            auto z = getPropertyArrayReal(pa4.second, 6);

                            auto localScaling = CPoint3D(x, y, z);

                            if (isDebug())
                              std::cerr << "  localScaling: " << localScaling << "\n";
                          }
                          else if (propName == "Visibility") {
                          }
                          else if (propName == "Visibility Inheritance") {
                          }
                          else
                            unhandledDataName(treeName4, propertyTemplate + "/" + propName);
                        }
                        else if (propertyTemplate == "FbxTexture" ||
                                 propertyTemplate == "KFbxTexture") {
                           if      (propName == "CurrentMappingType") {
                           }
                           else if (propName == "CurrentTextureBlendMode") {
                           }
                           else if (propName == "Rotation") {
                           }
                           else if (propName == "Scaling") {
                           }
                           else if (propName == "TextureTypeUse") {
                           }
                           else if (propName == "Texture alpha") {
                           }
                           else if (propName == "Translation") {
                           }
                           else if (propName == "TextureRotationPivot") {
                           }
                           else if (propName == "TextureScalingPivot") {
                           }
                           else if (propName == "UseMaterial") {
                           }
                           else if (propName == "UseMipMap") {
                           }
                           else if (propName == "UVSet") {
                           }
                           else if (propName == "UVSwap") {
                           }
                           else if (propName == "WrapModeU") {
                           }
                           else if (propName == "WrapModeV") {
                           }
                           else
                            unhandledDataName(treeName4, propertyTemplate + "/" + propName);
                        }
                        else if (propertyTemplate == "FbxSurfacePhong" ||
                                 propertyTemplate == "KFbxSurfacePhong") {
                          if      (propName == "ShadingModel") {
                          }
                          else if (propName == "MultiLayer") {
                          }
                          else if (propName == "EmissiveColor") {
                          }
                          else if (propName == "EmissiveFactor") {
                          }
                          else if (propName == "AmbientColor") {
                          }
                          else if (propName == "AmbientFactor") {
                          }
                          else if (propName == "DiffuseColor") {
                          }
                          else if (propName == "DiffuseFactor") {
                          }
                          else if (propName == "TransparentColor") {
                          }
                          else if (propName == "TransparencyFactor") {
                          }
                          else if (propName == "Opacity") {
                          }
                          else if (propName == "NormalMap") {
                          }
                          else if (propName == "Bump") {
                          }
                          else if (propName == "BumpFactor") {
                          }
                          else if (propName == "DisplacementColor") {
                          }
                          else if (propName == "DisplacementFactor") {
                          }
                          else if (propName == "VectorDisplacementColor") {
                          }
                          else if (propName == "VectorDisplacementFactor") {
                          }
                          else if (propName == "SpecularColor") {
                          }
                          else if (propName == "SpecularFactor") {
                          }
                          else if (propName == "Shininess") {
                          }
                          else if (propName == "ShininessExponent") {
                          }
                          else if (propName == "ReflectionColor") {
                          }
                          else if (propName == "ReflectionFactor") {
                          }
                          else
                            unhandledDataName(treeName4, propertyTemplate + "/" + propName);
                        }
                        else if (propertyTemplate == "FbxFileTexture") {
                          if      (propName == "TextureTypeUse") {
                          }
                          else if (propName == "AlphaSource") {
                          }
                          else if (propName == "Texture alpha") {
                          }
                          else if (propName == "PremultiplyAlpha") {
                          }
                          else if (propName == "CurrentTextureBlendMode") {
                          }
                          else if (propName == "CurrentMappingType") {
                          }
                          else if (propName == "UVSet") {
                          }
                          else if (propName == "UVSwap") {
                          }
                          else if (propName == "WrapModeU") {
                          }
                          else if (propName == "WrapModeV") {
                          }
                          else if (propName == "Translation") {
                          }
                          else if (propName == "Rotation") {
                          }
                          else if (propName == "Scaling") {
                          }
                          else if (propName == "TextureRotationPivot") {
                          }
                          else if (propName == "TextureScalingPivot") {
                          }
                          else if (propName == "UseMaterial") {
                          }
                          else if (propName == "UseMipMap") {
                          }
                          else
                            unhandledDataName(treeName4, propertyTemplate + "/" + propName);
                        }
                        else if (propertyTemplate == "FbxVideo" ||
                                 propertyTemplate == "KFbxVideo") {
                          if      (propName == "Width") {
                          }
                          else if (propName == "Height") {
                          }
                          else if (propName == "Path") {
                          }
                          else if (propName == "AccessMode") {
                          }
                          else if (propName == "StartFrame") {
                          }
                          else if (propName == "StopFrame") {
                          }
                          else if (propName == "Offset") {
                          }
                          else if (propName == "PlaySpeed") {
                          }
                          else if (propName == "FreeRunning") {
                          }
                          else if (propName == "Loop") {
                          }
                          else if (propName == "InterlaceMode") {
                          }
                          else if (propName == "ImageSequence") {
                          }
                          else if (propName == "ImageSequenceOffset") {
                          }
                          else if (propName == "FrameRate") {
                          }
                          else if (propName == "LastFrame") {
                          }
                          else if (propName == "Color") {
                          }
                          else if (propName == "ClipIn") {
                          }
                          else if (propName == "ClipOut") {
                          }
                          else if (propName == "Mute") {
                          }
                          else if (propName == "RelPath") {
                          }
                          else
                            unhandledDataName(treeName4, propertyTemplate + "/" + propName);
                        }
                        else if (propertyTemplate == "FbxAnimStack" ||
                                 propertyTemplate == "KFbxAnimStack") {
                          if      (propName == "Description") {
                          }
                          else if (propName == "LocalStart") {
                          }
                          else if (propName == "LocalStop") {
                          }
                          else if (propName == "ReferenceStart") {
                          }
                          else if (propName == "ReferenceStop") {
                          }
                          else if (propName == "Description") {
                          }
                          else if (propName == "LocalStart") {
                          }
                          else if (propName == "LocalStop") {
                          }
                          else
                            unhandledDataName(treeName4, propertyTemplate + "/" + propName);
                        }
                        else if (propertyTemplate == "FbxAnimLayer" ||
                                 propertyTemplate == "KFbxAnimLayer") {
                          if      (propName == "Weight") {
                          }
                          else if (propName == "Mute") {
                          }
                          else if (propName == "Solo") {
                          }
                          else if (propName == "Lock") {
                          }
                          else if (propName == "BlendMode") {
                          }
                          else if (propName == "RotationAccumulationMode") {
                          }
                          else if (propName == "ScaleAccumulationMode") {
                          }
                          else if (propName == "BlendModeBypass") {
                          }
                        }
                        else if (propertyTemplate == "FbxAnimCurveNode") {
                          if (propName == "d") {
                          }
                          else
                            unhandledDataName(treeName4, propertyTemplate + "/" + propName);
                        }
                        else if (propertyTemplate == "FbxCluster" ||
                                 propertyTemplate == "KFbxCluster") {
                          // ???
                        }
                        else if (propertyTemplate == "FbxSkeleton" ||
                                 propertyTemplate == "KFbxSkeleton") {
                          // ???
                        }
                        else if (propertyTemplate == "FbxNull" ||
                                 propertyTemplate == "KFbxNull") {
                          // ???
                        }
                        else if (propertyTemplate == "FbxCamera") {
                          // ???
                        }
                        else
                          unhandledDataName(treeName4, propertyTemplate + "/" + propName);
                      }
                    }
                    else
                      unhandledDataName(treeName4, pd4.first);
                  }

                  for (auto *tree5 : tree4->children) {
                    auto treeName5 = hierName(tree5);

                    unhandledTree(treeName5);
                  }
                }
                else
                  unhandledTree(treeName4);
              }
            }
            else
              unhandledTree(treeName3);
          }
        }
        else {
          unhandledTree(treeName2);
        }
      }
    }
    else if (treeName1 == "Connections") {
      //printPropertyNames(tree1);

      using IdMap      = IndNameMap<IndName>;
      using IdArraySet = IndNameMap<std::set<IndName>>;

      IdMap      modelGeometry;
      IdArraySet materialModel;

      for (const auto &pdm1 : tree1->dataMap) {
        if      (pdm1.first == "C" || pdm1.first == "Connect") {
          for (const auto &pa1 : pdm1.second) {
            auto id1 = getPropertyArrayIndName(pa1.second, 1);
            auto id2 = getPropertyArrayIndName(pa1.second, 2);
            if (id1.isEmpty() || id2.isEmpty()) continue;

            if (id1.to_long() == 0 || id2.to_long() == 0) continue;

            auto getIdType = [&](const IndName &id) {
              auto pi = idType_.find(id);
              if (pi == idType_.end()) return std::string("???");

              return (*pi).second;
            };

            auto idType1 = getIdType(id1);
            auto idType2 = getIdType(id2);

            auto connectType = getPropertyArrayName(pa1.second, 3);

            auto connectInfo = [&]() {
              std::cerr << "Info: Connect " << idType1 << " (" << id1 << ") -> " <<
                           idType2 << " (" << id2 << "), Type: " << connectType << "\n";
            };

            auto connectError = [&](const std::string &name) {
              std::cerr << "Error: Connect " << name << " " << idType1 << " (" << id1 << ") -> " <<
                           idType2 << " (" << id2 << "), Type: " << connectType << "\n";
            };

            //---

            if (isDebug()) {
              connectInfo();
            }

            //---

            // geometry
            auto pg = idGeometryData_.find(id1);
            if (pg != idGeometryData_.end()) {
              auto *geometryData = (*pg).second;

              // geometry -> model
              auto pm1 = idModelData_.find(id2);
              if (pm1 != idModelData_.end()) {
                auto *modelData1 = (*pm1).second;

                //connectInfo();
                //std::cerr << " " << geometryData->name << " -> " << modelData1->name << "\n";

                if (geometryData->modelData && geometryData->modelData->id != id2) {
                  std::cerr << "Duplicate Geometry->Model (" << id1 << "->" << id2 << ") "
                               "(already associated with " << geometryData->modelData->id << ")\n";
                  connectInfo();
                  //assert(false);
                }

                geometryData->modelData = modelData1;

                modelData1->geometryDataList.push_back(geometryData);

#if 0
                if (geometryData->object) {
                  geometryData->object->setName(modelData1->name);

                  auto hierTransform = calcModelDataHierTransform(modelData1);

                  geometryData->object->transform(hierTransform);
                }
#endif

                modelGeometry[id2] = id1;

                continue;
              }

              // geometry -> animation deformer
              auto pad1 = animationDeformerData_.find(id2);
              if (pad1 != animationDeformerData_.end()) {
                auto *deformerData = (*pad1).second;

                assert(! geometryData->deformerData);
                geometryData->deformerData = deformerData;

                //connectInfo();

                continue;
              }

              connectError("Geometry");
              continue;
            }

            // model
            auto pm = idModelData_.find(id1);
            if (pm != idModelData_.end()) {
              auto *modelData = (*pm).second; // child

              // model -> model
              auto pm1 = idModelData_.find(id2);
              if (pm1 != idModelData_.end()) {
                auto *modelData1 = (*pm1).second; // parent

                if (connectType == "LookAtProperty")
                  continue;

                //std::cerr << "Child: " << modelData->name << "(" << id1 << ") -> "
                //             "Parent: " << modelData1->name << "(" << id2 << ")\n";

                if (! modelData->parent) {
                  modelData->parent = modelData1;
                  modelData1->children.push_back(modelData);
                }
                else
                  std::cerr << "Warning: multiple model parents: " << id1 << " -> " << id2 << "\n";

                //connectInfo();

                continue;
              }

              // model -> animation deformer
              auto pad1 = animationDeformerData_.find(id2);
              if (pad1 != animationDeformerData_.end()) {
                auto *deformerData = (*pad1).second;

                modelData->deformerData.push_back(deformerData);

                //connectInfo();

                continue;
              }

              if (id2.to_string() == "Scene") {
                continue;
              }

              connectError("Model");
              continue;
            }

            // material
            auto pmt = idMaterialData_.find(id1);
            if (pmt != idMaterialData_.end()) {
              auto *materialData = (*pmt).second;

              // material -> model
              auto pm1 = idModelData_.find(id2);
              if (pm1 != idModelData_.end()) {
                auto *modelData1 = (*pm1).second;

                materialData->modelData[modelData1->id] = modelData1;

                modelData1->materialData[materialData->id] = materialData;

                materialModel[id1].insert(id2);

                auto po1 = modelGeometry.find(id2);

                if (po1 != modelGeometry.end()) {
                  //std::cerr << "Connect Material -> Object " << id1 <<
                  //             " " << (*po1).second << "\n";
                }

                continue;
              }

              connectError("Material");
              continue;
            }

            // texture
            auto pt = idTextureData_.find(id1);
            if (pt != idTextureData_.end()) {
              const auto &textureDataList = (*pt).second;

              auto *textureData = textureDataList[0];

              // texture -> model
              auto pn1 = idModelData_.find(id2);
              if (pn1 != idModelData_.end()) {
                auto *modelData1 = (*pn1).second;

                assert(! textureData->modelData);
                textureData->modelData = modelData1;

                auto pt1 = modelData1->textureMap.find(textureData->type);
                assert(pt1 == modelData1->textureMap.end());

                modelData1->textureMap[textureData->type] = textureData;

                //connectInfo();

                continue;
              }

              // texture -> material
              auto pm1 = idMaterialData_.find(id2);
              if (pm1 != idMaterialData_.end()) {
                auto *materialData = (*pm1).second;

                textureData->materialData[materialData->id] = materialData;

                materialData->textureMap[connectType][textureData->id] = textureData;

                auto pm2 = materialModel.find(id2);

                if (pm2 != materialModel.end()) {
                  const auto &idSet = (*pm2).second;

                  for (const auto &id : idSet) {
                    auto po1 = modelGeometry.find(id);

                    if (po1 != modelGeometry.end()) {
                      //std::cerr << "Connect Texture -> Object " << id1 << " " <<
                      //             (*po1).second << "\n";

                      if (! textureData->texture) {
                        std::string fileName;
                        if (! loadTexture(textureData, fileName))
                          errorMsg("Invalid texture file name '" + fileName + "'");
                      }

#if 0
                      auto pg2 = idGeometryData_.find((*po1).second);
                      assert(pg2 != idGeometryData_.end());

                      auto *geometryData = (*pg2).second;

                      if (textureData->texture && geometryData->object) {
                        if (textureData->type == "NormalMap")
                          geometryData->object->setNormalTexture(textureData->texture);
                        else
                          geometryData->object->setDiffuseTexture(textureData->texture);
                      }
#endif
                    }
                  }
                }

                continue;
              }

              connectError("Texture");
              continue;
            }

            // animation curve node
            auto pcn = animationCurveNodeData_.find(id1);
            if (pcn != animationCurveNodeData_.end()) {
              auto *animationCurveNode = (*pcn).second;

              // animation curve node -> animation layer
              auto pl1 = animationLayerData_.find(id2);
              if  (pl1 != animationLayerData_.end()) {
                auto *animationLayerData = (*pl1).second;

                assert(! animationCurveNode->animationLayerData);
                animationCurveNode->animationLayerData = animationLayerData;

                animationLayerData->animationCurveNodes.push_back(animationCurveNode);

                //connectInfo();

                animationLayerData->points.push_back(animationCurveNode->p);

                continue;
              }

              // animation curve node -> model
              auto pn1 = idModelData_.find(id2);
              if (pn1 != idModelData_.end()) {
                auto *modelData1 = (*pn1).second;

                assert(! animationCurveNode->modelData);
                animationCurveNode->modelData = modelData1;

                //connectInfo();

                continue;
              }

              connectError("Animation Curve Node");
              continue;
            }

            //---

            // animation curve
            auto pac = animationCurveData_.find(id1);
            if (pac != animationCurveData_.end()) {
              auto *animationCurve = (*pac).second;

              // animation curve -> animation curve node
              auto pcn1 = animationCurveNodeData_.find(id2);
              if (pcn1 != animationCurveNodeData_.end()) {
                auto *animationCurveNode = (*pcn1).second;

                assert(! animationCurve->animationCurveNode);
                animationCurve->animationCurveNode = animationCurveNode;

                //connectInfo();

                continue;
              }

              connectError("Animation Curve");
              continue;
            }

            //---

            // animation layer
            auto pl = animationLayerData_.find(id1);
            if (pl != animationLayerData_.end()) {
              auto *animationLayer = (*pl).second;

              // animation layer -> animation stack
              auto pas1 = animationStackData_.find(id2);
              if (pas1 != animationStackData_.end()) {
                auto *animationStack = (*pas1).second;

                assert(! animationLayer->animationStack);
                animationLayer->animationStack = animationStack;

                animationStack->animationLayers.push_back(animationLayer);

                //connectInfo();

                continue;
              }

              connectError("Animation Layer");
              continue;
            }

            //---

            // animation deformer
            auto pad = animationDeformerData_.find(id1);
            if (pad != animationDeformerData_.end()) {
              auto *animationDeformer = (*pad).second; // child

              // animation deformer -> model
              auto pn1 = idModelData_.find(id2);
              if (pn1 != idModelData_.end()) {
                auto *modelData1 = (*pn1).second;

                assert(! animationDeformer->modelData);
                animationDeformer->modelData = modelData1;

                //connectInfo();

                continue;
              }

              // animation deformer -> geometry
              auto pg1 = idGeometryData_.find(id2);
              if (pg1 != idGeometryData_.end()) {
                auto *geometryData1 = (*pg1).second;

                assert(! animationDeformer->geometryData);
                animationDeformer->geometryData = geometryData1;

                //connectInfo();

                continue;
              }

              // animation deformer -> animation deformer
              auto pad1 = animationDeformerData_.find(id2);
              if (pad1 != animationDeformerData_.end()) {
                auto *animationDeformer1 = (*pad1).second; // parent

                assert(! animationDeformer->parent);
                animationDeformer->parent = animationDeformer1;

                animationDeformer1->children.push_back(animationDeformer);

                //connectInfo();

                continue;
              }

              connectError("Animation Deformer");
              continue;
            }

            //---

            // animation stack
            auto pas = animationStackData_.find(id1);
            if (pas != animationStackData_.end()) {
              //auto *animationStack = (*pas).second;
              connectError("Animation Stack");
              continue;
            }

            //---

            // node attribute data
            auto pna = idNodeAttributeData_.find(id1);
            if (pna != idNodeAttributeData_.end()) {
              auto *nodeAttribute = (*pna).second;

              // node attribute -> model data
              auto pm1 = idModelData_.find(id2);
              if (pm1 != idModelData_.end()) {
                auto *modelData1 = (*pm1).second;

                assert(! nodeAttribute->modelData);
                nodeAttribute->modelData = modelData1;

                //connectInfo();

                continue;
              }

              connectError("Node Attribute Data");
              continue;
            }

            //---

            // video data
            auto pv = idVideoData_.find(id1);
            if (pv != idVideoData_.end()) {
              auto *videoData = (*pv).second;

              // video -> texture
              auto pt1 = idTextureData_.find(id2);
              if (pt1 != idTextureData_.end()) {
                const auto &textureDataList1 = (*pt1).second;

                auto *textureData1 = textureDataList1[0];

                textureData1->videoData.push_back(videoData);

                auto pt2 = videoData->textureListMap.find(connectType);

                if (pt2 == videoData->textureListMap.end())
                  pt2 = videoData->textureListMap.insert(pt2,
                    TextureListMap::value_type(connectType, TextureList()));

                auto &textureList = (*pt2).second;

                textureList.push_back(textureData1);

                continue;
              }

              connectError("Video Data");
              continue;
            }

            //---

            // constraint data
            auto pc = constraintData_.find(id1);
            if (pc != constraintData_.end()) {
              //auto *constraintData = (*pc).second;
              //connectInfo();

              continue;
            }

            //---

            connectError(idType1);
          }
        }
        else
          unhandledDataName(treeName1, pdm1.first);
      }

      for (auto *tree2 : tree1->children) {
        auto treeName2 = hierName(tree2);

        unhandledTree(treeName2);
      }
    }
    else if (treeName1 == "Takes") {
    }
    else if (treeName1 == "Objects") {
      //printPropertyNames(tree1);
      //printPropertyNameData(tree1);

      for (const auto &pd1 : tree1->dataMap) {
        if      (pd1.first == "AnimationCurve") {
        }
        else if (pd1.first == "AnimationCurveNode") {
        }
        else if (pd1.first == "AnimationLayer") {
        }
        else if (pd1.first == "AnimationStack") {
        }
        else if (pd1.first == "Deformer") {
        }
        else if (pd1.first == "Geometry") {
        }
        else if (pd1.first == "Material") {
        }
        else if (pd1.first == "Model") {
          if (isDebug()) {
            auto name = getPropertyName(pd1.second, 1);

            debugMsg("Model " + name);
          }
        }
        else if (pd1.first == "NodeAttribute") {
        }
        else if (pd1.first == "Pose") {
        }
        else if (pd1.first == "Texture") {
          if (isDebug()) {
            auto name = getPropertyName(pd1.second, 1);

            debugMsg("Texture " + name);
          }
        }
        else if (pd1.first == "Video") {
        }
        else if (pd1.first == "Constraint") {
        }
        else if (pd1.first == "Device") {
        }
        else if (pd1.first == "Folder") {
        }
        else if (pd1.first == "GlobalShading") {
        }
        else
          unhandledDataName(treeName1, pd1.first);
      }

      for (auto *tree2 : tree1->children) {
        auto treeName2 = hierName(tree2);

        if      (treeName2 == "Objects/AnimationCurve") {
          BlockData blockData;
          readBlockDetails(tree2, blockData, treeName2);

          auto *animationCurveData = new AnimationCurveData;
          animationCurveData->name = blockData.name;

          for (const auto &pd2 : tree2->dataMap) {
            if      (pd2.first == "Default") {
              animationCurveData->def = getPropertyReal(pd2.second);
            }
            else if (pd2.first == "KeyAttrDataFloat") {
            }
            else if (pd2.first == "KeyAttrFlags") {
            }
            else if (pd2.first == "KeyAttrRefCount") {
            }
            else if (pd2.first == "KeyTime") {
              animationCurveData->keyTime = getPropertyLongArray(pd2.second);
            }
            else if (pd2.first == "KeyValueFloat") {
              animationCurveData->keyValueFloat = getPropertyFloatArray(pd2.second);
            }
            else if (pd2.first == "KeyVer") {
            }
            else
              unhandledProperty(treeName2, pd2.first);
          }

          animationCurveData_[blockData.id] = animationCurveData;

          // default: real ?
        }
        else if (treeName2 == "Objects/AnimationCurveNode") {
          BlockData blockData;
          readBlockDetails(tree2, blockData, treeName2);

          auto *animationCurveNodeData = new AnimationCurveNodeData;
          animationCurveNodeData->type = blockData.name;

          for (const auto &pd2 : tree2->dataMap) {
            unhandledProperty(treeName2, pd2.first);
          }

          // point (x, y, z)
          CPoint3D p;

          for (auto *tree3 : tree2->children) {
            auto treeName3 = hierName(tree3);

            if (treeName3 == "Objects/AnimationCurveNode/Properties70") {
              for (const auto &pd3 : tree3->dataMap) {
                if      (pd3.first == "P") {
                  for (const auto &pa3 : pd3.second) {
                    auto propName3 = getPropertyArrayName(pa3.second);

                    if      (propName3 == "d|X") {
                      p.x = getPropertyArrayReal(pa3.second, 4);
                    }
                    else if (propName3 == "d|Y") {
                      p.y = getPropertyArrayReal(pa3.second, 4);
                    }
                    else if (propName3 == "d|Z") {
                      p.z = getPropertyArrayReal(pa3.second, 4);
                    }
                    else if (propName3 == "d") {
                    }
                    else if (propName3 == "d|MaxHandle") {
                    }
                    else {
                      if (propName3.size() < 3 || ! std::islower(propName3[2]))
                        unhandledProperty(treeName3, propName3);
                    }
                  }
                }
                else
                  unhandledProperty(treeName3, pd3.first);
              }
            }
            else
              unhandledTree(treeName3);
          }

          //std::cerr << blockData.id << " " << p << "\n";

          animationCurveNodeData_[blockData.id] = animationCurveNodeData;
        }
        else if (treeName2 == "Objects/AnimationLayer") {
          BlockData blockData;
          readBlockDetails(tree2, blockData, treeName2);

          auto *animationLayerData = new AnimationLayerData;
          animationLayerData->name = blockData.name;

          for (const auto &pd2 : tree2->dataMap) {
            unhandledProperty(treeName2, pd2.first);
          }

          animationLayerData_[blockData.id] = animationLayerData;
        }
        else if (treeName2 == "Objects/AnimationStack") {
          // LocalStop, ReferenceStop

          BlockData blockData;
          readBlockDetails(tree2, blockData, treeName2);

          auto *animationStackData = new AnimationStackData;
          animationStackData->name = blockData.name;

          for (const auto &pd2 : tree2->dataMap) {
            unhandledProperty(treeName2, pd2.first);
          }

          animationStackData_[blockData.id] = animationStackData;
        }
        else if (treeName2 == "Objects/Deformer") {
          // ???

          BlockData blockData;
          readBlockDetails(tree2, blockData, treeName2);

          auto *animationDeformerData = new AnimationDeformerData;
          animationDeformerData->name = blockData.name;
          animationDeformerData->type = blockData.type;

          for (const auto &pd2 : tree2->dataMap) {
            if      (pd2.first == "DeformPercent") {
            }
            else if (pd2.first == "FullWeights") {
            }
            else if (pd2.first == "Indexes") {
              animationDeformerData->indexes = getPropertyIntArray(pd2.second);
            }
            else if (pd2.first == "Link_DeformAcuracy") {
            }
            else if (pd2.first == "Mode") {
            }
            else if (pd2.first == "SkinningType") {
            }
            else if (pd2.first == "Transform") {
            }
            else if (pd2.first == "TransformAssociateModel") {
            }
            else if (pd2.first == "TransformLink") {
            }
            else if (pd2.first == "UserData") {
            }
            else if (pd2.first == "Version") {
            }
            else if (pd2.first == "Weights") {
              animationDeformerData->weights = getPropertyDoubleArray(pd2.second);
            }
            else
              unhandledProperty(treeName2, pd2.first);
          }

          animationDeformerData_[blockData.id] = animationDeformerData;
        }
        else if (treeName2 == "Objects/Geometry") {
          if (isDebug())
            printParentDataMap(tree2);
          //printPropertyNames(tree2);

          BlockData blockData;
          readBlockDetails(tree2, blockData, treeName2);

          auto *geometryData = new GeometryData;
          geometryData->name = blockData.name;
          geometryData->type = blockData.type;

          //---

          for (auto *tree3 : tree2->children) {
            auto treeName3 = hierName(tree3);

            if      (treeName3 == "Objects/Geometry/Properties70") {
              for (const auto &pd3 : tree3->dataMap) {
                if      (pd3.first == "P") {
                  for (const auto &pa3 : pd3.second) {
                    auto propName3 = getPropertyArrayName(pa3.second);

                    //unhandledProperty(treeName3, propName3);
                  }
                }
                else
                  unhandledProperty(treeName3, pd3.first);
              }
            }
            else if (treeName3 == "Objects/Geometry/LayerElementBinormal") {
            }
            else if (treeName3 == "Objects/Geometry/LayerElementColor") {
            }
            else if (treeName3 == "Objects/Geometry/LayerElementNormal") {
            }
            else if (treeName3 == "Objects/Geometry/LayerElementSmoothing") {
            }
            else if (treeName3 == "Objects/Geometry/LayerElementTangent") {
            }
            else if (treeName3 == "Objects/Geometry/LayerElementUV") {
            }
            else if (treeName3 == "Objects/Geometry/LayerElementMaterial") {
            }
            else if (treeName3 == "Objects/Geometry/LayerElementTexture") {
            }
            else if (treeName3 == "Objects/Geometry/Layer") {
            }
            else
              unhandledTree(treeName3);
          }

          //---

          processGeometryDataTree(tree2, geometryData);

          //addGeometryObject(geometryData);

          idGeometryData_[blockData.id] = geometryData;
        }
        else if (treeName2 == "Objects/Material") {
          if (isDebug())
            printParentDataMap(tree2);
          //printPropertyNames(tree2);

          BlockData blockData;
          readBlockDetails(tree2, blockData, treeName2);

          auto *materialData = new MaterialData;
          materialData->name = blockData.name;

          for (const auto &pd2 : tree2->dataMap) {
            if      (pd2.first == "MultiLayer") {
            }
            else if (pd2.first == "ShadingModel") {
            }
            else if (pd2.first == "Version") {
            }
            else
              unhandledDataName(treeName2, pd2.first);
          }

          for (auto *tree3 : tree2->children) {
            auto treeName3 = hierName(tree3);

            if      (treeName3 == "Objects/Material/Properties60") {
            }
            else if (treeName3 == "Objects/Material/Properties70") {
              for (const auto &pd3 : tree3->dataMap) {
                if      (pd3.first == "P") {
                  for (const auto &pa3 : pd3.second) {
                    auto propName3 = getPropertyArrayName(pa3.second);

                    if      (propName3 == "Ambient" || propName3 == "AmbientColor") {
                      auto r = getPropertyArrayReal(pa3.second, 4);
                      auto g = getPropertyArrayReal(pa3.second, 5);
                      auto b = getPropertyArrayReal(pa3.second, 6);

                      materialData->ambientColor = CRGBA(r, g, b);
                    }
                    else if (propName3 == "AmbientFactor") {
                      materialData->ambientFactor = getPropertyArrayReal(pa3.second, 4);
                    }
                    else if (propName3 == "BumpFactor") {
                      materialData->bumpFactor = getPropertyArrayReal(pa3.second, 4);
                    }
                    else if (propName3 == "Diffuse" || propName3 == "DiffuseColor") {
                      auto r = getPropertyArrayReal(pa3.second, 4);
                      auto g = getPropertyArrayReal(pa3.second, 5);
                      auto b = getPropertyArrayReal(pa3.second, 6);

                      materialData->diffuseColor = CRGBA(r, g, b);
                    }
                    else if (propName3 == "DiffuseFactor") {
                      materialData->diffuseFactor = getPropertyArrayReal(pa3.second, 4);
                    }
                    else if (propName3 == "Emissive" || propName3 == "EmissiveColor") {
                      auto r = getPropertyArrayReal(pa3.second, 4);
                      auto g = getPropertyArrayReal(pa3.second, 5);
                      auto b = getPropertyArrayReal(pa3.second, 6);

                      materialData->emissionColor = CRGBA(r, g, b);
                    }
                    else if (propName3 == "EmissiveFactor") {
                      materialData->emissionFactor = getPropertyArrayReal(pa3.second, 4);
                    }
                    else if (propName3 == "Opacity") {
                      materialData->opacity = getPropertyArrayReal(pa3.second, 4);
                    }
                    else if (propName3 == "Reflectivity") {
                      materialData->reflectivity = getPropertyArrayReal(pa3.second, 4);
                    }
                    else if (propName3 == "ReflectionColor") {
                      auto r = getPropertyArrayReal(pa3.second, 4);
                      auto g = getPropertyArrayReal(pa3.second, 5);
                      auto b = getPropertyArrayReal(pa3.second, 6);

                      materialData->reflectionColor = CRGBA(r, g, b);
                    }
                    else if (propName3 == "ReflectionFactor") {
                      materialData->reflectionFactor = getPropertyArrayReal(pa3.second, 4);
                    }
                    else if (propName3 == "ShadingModel") {
                      materialData->shadingModel = getPropertyArrayName(pa3.second, 4);
                    }
                    else if (propName3 == "Shininess" || propName3 == "ShininessExponent") {
                      materialData->shininess = getPropertyArrayReal(pa3.second, 4);
                    }
                    else if (propName3 == "Specular" || propName3 == "SpecularColor") {
                      auto r = getPropertyArrayReal(pa3.second, 4);
                      auto g = getPropertyArrayReal(pa3.second, 5);
                      auto b = getPropertyArrayReal(pa3.second, 6);

                      materialData->specularColor = CRGBA(r, g, b);
                    }
                    else if (propName3 == "SpecularFactor") {
                      materialData->specularFactor = getPropertyArrayReal(pa3.second, 4);
                    }
                    else if (propName3 == "TransparentColor") {
                      auto r = getPropertyArrayReal(pa3.second, 4);
                      auto g = getPropertyArrayReal(pa3.second, 5);
                      auto b = getPropertyArrayReal(pa3.second, 6);

                      materialData->transparencyColor = CRGBA(r, g, b);
                    }
                    else if (propName3 == "TransparencyFactor") {
                      materialData->transparencyFactor = getPropertyArrayReal(pa3.second, 4);
                    }
                    else if (propName3 == "UseMaterial") {
                      unhandledProperty(treeName3, propName3);
                    }
                    else
                      unhandledProperty(treeName3, propName3);
                  }
                }
                else
                  unhandledProperty(treeName3, pd3.first);
              }

              for (auto *tree4 : tree3->children) {
                auto treeName4 = hierName(tree4);

                unhandledTree(treeName4);
              }
            }
            else
              unhandledTree(treeName3);
          }

          materialData->id = blockData.id;

          idMaterialData_[blockData.id] = materialData;

          if (isDebug())
            printMaterialData(materialData);
        }
        else if (treeName2 == "Objects/Model") {
          if (isDebug())
            printParentDataMap(tree2);
          //printPropertyNames(tree2);
          //printPropertyNameData(tree2);

          BlockData blockData;
          readBlockDetails(tree2, blockData, treeName2);

          auto *modelData = new ModelData;
          modelData->name = blockData.name;
          modelData->type = blockData.type;

          //std::cerr << "modelId: " << blockData.id << "\n";
          //std::cerr << "modelName: " << modelData->name << "\n";

          VertexData vertexData;

          for (const auto &pd2 : tree2->dataMap) {
            if      (pd2.first == "CameraId") {
            }
            else if (pd2.first == "CameraIndexName") {
            }
            else if (pd2.first == "CameraName") {
            }
            else if (pd2.first == "CameraOrthoZoom") {
            }
            else if (pd2.first == "Culling") {
            }
            else if (pd2.first == "DisplayMode") {
            }
            else if (pd2.first == "GeometryVersion") {
            }
            else if (pd2.first == "Layer") {
            }
            else if (pd2.first == "LayerElementMaterial") {
            }
            else if (pd2.first == "LayerElementNormal") {
            }
            else if (pd2.first == "LookAt") {
            }
            else if (pd2.first == "LookAtModel") {
            }
            else if (pd2.first == "MultiLayer") {
            }
            else if (pd2.first == "MultiTake") {
            }
            else if (pd2.first == "Name") {
            }
            else if (pd2.first == "Position") {
            }
            else if (pd2.first == "Shading") {
            }
            else if (pd2.first == "Shape") {
            }
            else if (pd2.first == "ShowInfoOnMoving") {
            }
            else if (pd2.first == "TypeFlags") {
            }
            else if (pd2.first == "Up") {
            }
            else if (pd2.first == "Version") {
            }
            else if (pd2.first == "PolygonVertexIndex") {
              vertexData.indices = getPropertyInts(pd2.second);
              vertexData.set     = true;
            }
            else if (pd2.first == "Vertices") {
              vertexData.values = getPropertyDoubles(pd2.second);
              vertexData.set    = true;
            }
            else
              unhandledDataName(treeName2, pd2.first);
          }

          if (vertexData.set) {
            if (! modelData->geometryData)
              modelData->geometryData = new GeometryData;

            modelData->geometryData->vertexData.push_back(vertexData);
          }

          //---

          for (auto *tree3 : tree2->children) {
            auto treeName3 = hierName(tree3);

            if      (treeName3 == "Objects/Model/Properties60") {
            }
            else if (treeName3 == "Objects/Model/Properties70") {
              for (const auto &pd3 : tree3->dataMap) {
                if      (pd3.first == "P") {
                  for (const auto &pa3 : pd3.second) {
                    auto propName3 = getPropertyArrayName(pa3.second);

                    if      (propName3 == "InheritType") {
                    }
                    else if (propName3 == "Lcl Translation") {
                      auto x = getPropertyArrayReal(pa3.second, 4);
                      auto y = getPropertyArrayReal(pa3.second, 5);
                      auto z = getPropertyArrayReal(pa3.second, 6);

                      modelData->localTRS.translation = CPoint3D(x, y, z);

                      if (isDebug())
                        std::cerr << "  Lcl Translation: " <<
                          modelData->localTRS.translation.value() << "\n";
                    }
                    else if (propName3 == "Lcl Rotation") {
                      auto x = getPropertyArrayReal(pa3.second, 4);
                      auto y = getPropertyArrayReal(pa3.second, 5);
                      auto z = getPropertyArrayReal(pa3.second, 6);

                      modelData->localTRS.rotation = CPoint3D(x, y, z);

                      if (isDebug())
                        std::cerr << "  Lcl Rotation: " <<
                          modelData->localTRS.rotation.value() << "\n";
                    }
                    else if (propName3 == "Lcl Scaling") {
                      auto x = getPropertyArrayReal(pa3.second, 4);
                      auto y = getPropertyArrayReal(pa3.second, 5);
                      auto z = getPropertyArrayReal(pa3.second, 6);

                      modelData->localTRS.scaling = CPoint3D(x, y, z);

                      if (isDebug())
                        std::cerr << "  Lcl Scaling: " <<
                          modelData->localTRS.scaling.value() << "\n";
                    }
                    else if (propName3 == "AspectW") {
                    }
                    else if (propName3 == "AspectH") {
                    }
                    else if (propName3 == "BackgroundMode") {
                    }
                    else if (propName3 == "DefaultAttributeIndex") {
                    }
                    else if (propName3 == "ForegroundTransparent") {
                    }
                    else if (propName3 == "GeometricRotation") {
                    }
                    else if (propName3 == "GeometricScaling") {
                    }
                    else if (propName3 == "GeometricTranslation") {
                    }
                    else if (propName3 == "MaxHandle") {
                    }
                    else if (propName3 == "Opacity") {
                    }
                    else if (propName3 == "PreferedAngleX") {
                    }
                    else if (propName3 == "PreferedAngleY") {
                    }
                    else if (propName3 == "PreferedAngleZ") {
                    }
                    else if (propName3 == "PreRotation") {
                    }
                    else if (propName3 == "Reflectivity") {
                    }
                    else if (propName3 == "ResolutionMode") {
                    }
                    else if (propName3 == "RotationActive") {
                    }
                    else if (propName3 == "RotationOffset") {
                    }
                    else if (propName3 == "RotationPivot") {
                    }
                    else if (propName3 == "ScalingMax") {
                    }
                    else if (propName3 == "ScalingMin") {
                    }
                    else if (propName3 == "ScalingOffset") {
                    }
                    else if (propName3 == "ViewFrustum") {
                    }
                    else if (propName3 == "Visibility") {
                    }
#if 0
                    else if (propName3 == "orig_loc") {
                    }
                    else if (propName3 == "orig_quat") {
                    }
                    else if (propName3 == "post_quat") {
                    }
#endif
                    else {
                      if (! std::islower(propName3[0]))
                        unhandledProperty(treeName3, propName3);
                    }
                  }
                }
                else
                  unhandledProperty(treeName3, pd3.first);
              }

              for (auto *tree4 : tree3->children) {
                auto treeName4 = hierName(tree4);

                unhandledTree(treeName4);
              }
            }
            else if (treeName3 == "Objects/Model/Limits") {
            }
            else if (treeName3 == "Objects/Model/Shape") {
            }
            else if (treeName3 == "Objects/Model/LayerElementNormal") {
            }
            else if (treeName3 == "Objects/Model/LayerElementMaterial") {
            }
            else if (treeName3 == "Objects/Model/Layer") {
            }
            else
              unhandledTree(treeName3);
          }

          modelData->id = blockData.id;

          idModelData_[blockData.id] = modelData;
        }
        else if (treeName2 == "Objects/NodeAttribute") {
          // Size
          BlockData blockData;
          readBlockDetails(tree2, blockData, treeName2);

          auto *nodeAttributeData = new NodeAttributeData;
          nodeAttributeData->name = blockData.name;

          for (const auto &pd2 : tree2->dataMap) {
            if      (pd2.first == "TypeFlags") {
            }
            else if (pd2.first == "GeometryVersion") {
            }
            else if (pd2.first == "AudioColor") {
            }
            else if (pd2.first == "CameraOrthoZoom") {
            }
            else if (pd2.first == "LookAt") {
            }
            else if (pd2.first == "Position") {
            }
            else if (pd2.first == "ShowAudio") {
            }
            else if (pd2.first == "ShowInfoOnMoving") {
            }
            else if (pd2.first == "Up") {
            }
            else
              unhandledProperty(treeName2, pd2.first);
          }

          idNodeAttributeData_[blockData.id] = nodeAttributeData;
        }
        else if (treeName2 == "Objects/Pose") {
          if (isDebug())
            printParentDataMap(tree2);
          //printPropertyNames(tree2);

          BlockData blockData;
          readBlockDetails(tree2, blockData, treeName2);

          auto *poseData = new PoseData;
          poseData->name = blockData.name;

          for (const auto &pd2 : tree2->dataMap) {
            if      (pd2.first == "NbPoseNodes") {
            }
            else if (pd2.first == "Type") {
            }
            else if (pd2.first == "Version") {
            }
            else
              unhandledDataName(treeName2, pd2.first);
          }

          for (auto *tree3 : tree2->children) {
            auto treeName3 = hierName(tree3);

            PoseNodeData poseNodeData;

            if (treeName3 == "Objects/Pose/PoseNode") {
              for (const auto &pd3 : tree3->dataMap) {
                if      (pd3.first == "Node") {
                  poseNodeData.node = getPropertyInt(pd3.second);

#if 0
                  auto pm = idModelData_.find(node);
                  if (pm != idModelData_.end()) {
                    auto *modelData = (*pm).second;
                    std::cerr << node << " (" << modelData->name << ")\n";
                  }
                  else {
                    std::cerr << node << " (no model)\n";
                  }
#endif
                }
                else if (pd3.first == "Matrix") {
                  poseNodeData.matrix = getPropertyDoubleArray(pd3.second);
                }
                else
                  unhandledDataName(treeName2, pd3.first);
              }

              poseData->nodes.push_back(poseNodeData);
            }
            else
              unhandledTree(treeName3);
          }

          idPoseData_[blockData.id] = poseData;
        }
        else if (treeName2 == "Objects/Texture") {
          if (isDebug())
            printParentDataMap(tree2);
          //printPropertyNames(tree2);

          BlockData blockData;
          readBlockDetails(tree2, blockData, treeName2);

          auto *textureData = new TextureData;
          textureData->name = blockData.name;

          for (const auto &pd2 : tree2->dataMap) {
            if      (pd2.first == "Cropping") {
            }
            else if (pd2.first == "FileName") {
              textureData->fileName = getPropertyName(pd2.second);
            }
            else if (pd2.first == "Media") {
              textureData->media = getPropertyName(pd2.second);
            }
            else if (pd2.first == "ModelUVScaling") {
            }
            else if (pd2.first == "ModelUVTranslation") {
            }
            else if (pd2.first == "RelativeFilename") {
            }
            else if (pd2.first == "TextureName") {
              textureData->name = getPropertyName(pd2.second);
            }
            else if (pd2.first == "Texture_Alpha_Source") {
            }
            else if (pd2.first == "Type") {
            }
            else if (pd2.first == "Version") {
            }
            else
              unhandledDataName(treeName2, pd2.first);
          }

          for (auto *tree3 : tree2->children) {
            auto treeName3 = hierName(tree3);

            if (treeName3 == "Objects/Texture/Properties70") {
              for (const auto &pd3 : tree3->dataMap) {
                if      (pd3.first == "P") {
                  for (const auto &pa3 : pd3.second) {
                    auto propName3 = getPropertyArrayName(pa3.second);

                    if      (propName3 == "CurrentTextureBlendMode") {
                    }
                    else if (propName3 == "UseMipMap") {
                    }
                    else if (propName3 == "UseMaterial") {
                    }
                    else if (propName3 == "UVSet") {
                    }
                    else
                      unhandledProperty(treeName3, propName3);
                  }
                }
                else
                  unhandledProperty(treeName3, pd3.first);
              }

              for (auto *tree4 : tree3->children) {
                auto treeName4 = hierName(tree4);

                unhandledTree(treeName4);
              }
            }
            else
              unhandledTree(treeName3);
          }

          textureData->id = blockData.id;

          idTextureData_[blockData.id].push_back(textureData);
        }
        else if (treeName2 == "Objects/Video") {
          BlockData blockData;
          readBlockDetails(tree2, blockData, treeName2);

          for (const auto &pd2 : tree2->dataMap) {
            if      (pd2.first == "Filename") {
            }
            else if (pd2.first == "RelativeFilename") {
            }
            else if (pd2.first == "Type") {
            }
            else if (pd2.first == "UseMipMap") {
            }
            else
              unhandledProperty(treeName2, pd2.first);
          }

          auto *videoData = new VideoData;
          videoData->id = blockData.id;

          assert(idVideoData_.find(blockData.id) == idVideoData_.end());
          idVideoData_[blockData.id] = videoData;
        }
        else if (treeName2 == "Objects/GlobalShading") {
        }
        else if (treeName2 == "Objects/Device") {
        }
        else if (treeName2 == "Objects/Constraint") {
          BlockData blockData;
          readBlockDetails(tree2, blockData, treeName2);

          for (const auto &pd2 : tree2->dataMap) {
            if (pd2.first == "MultiLayer") {
            }
            else
              unhandledProperty(treeName2, pd2.first);
          }

          auto *constraintData = new ConstraintData;
          constraintData->name = blockData.name;

          constraintData_[blockData.id] = constraintData;
        }
        else if (treeName2 == "Objects/Folder") {
        }
        else
          unhandledTree(treeName2);
      }
    }
    else if (treeName1 == "Relations") {
    }
    else if (treeName1 == "ObjectData") {
    }
    else if (treeName1 == "Version5") {
    }
    else if (treeName1 == "HierarchyView") {
    }
    else
      unhandledTree(treeName1);
  }
}

void
CImportFBX::
printMaterialData(MaterialData *materialData) const
{
  std::cerr << "Material: " << materialData->name << "\n";

  if (materialData->ambientColor)
    std::cerr << " AmbientColor: " << materialData->ambientColor.value() << "\n";
  if (materialData->ambientFactor)
    std::cerr << " AmbientFactor: " << materialData->ambientFactor.value() << "\n";

  if (materialData->diffuseColor)
    std::cerr << " DiffuseColor: " << materialData->diffuseColor.value() << "\n";
  if (materialData->diffuseFactor)
    std::cerr << " DiffuseFactor: " << materialData->diffuseFactor.value() << "\n";

  if (materialData->emissionColor)
    std::cerr << " EmissionColor: " << materialData->emissionColor.value() << "\n";
  if (materialData->emissionFactor)
    std::cerr << " EmissionFactor: " << materialData->emissionFactor.value() << "\n";

  if (materialData->specularColor)
    std::cerr << " SpecularColor: " << materialData->specularColor.value() << "\n";
  if (materialData->specularFactor)
    std::cerr << " SpecularFactor: " << materialData->specularFactor.value() << "\n";

  if (materialData->shininess)
    std::cerr << " Shininess: " << materialData->shininess.value() << "\n";
  if (materialData->bumpFactor)
    std::cerr << " BumpFactor: " << materialData->bumpFactor.value() << "\n";

  if (materialData->transparencyColor)
    std::cerr << " TransparencyColor: " << materialData->transparencyColor.value() << "\n";
  if (materialData->transparencyFactor)
    std::cerr << " TransparencyFactor: " << materialData->transparencyFactor.value() << "\n";
  if (materialData->opacity)
    std::cerr << " Opacity: " << materialData->opacity.value() << "\n";

  if (materialData->reflectionColor)
    std::cerr << " ReflectionColor: " << materialData->reflectionColor.value() << "\n";
  if (materialData->reflectionFactor)
    std::cerr << " ReflectionFactor: " << materialData->reflectionFactor.value() << "\n";
  if (materialData->reflectivity)
    std::cerr << " Reflectivity: " << materialData->reflectivity.value() << "\n";
}

CMatrix3D
CImportFBX::
calcModelDataLocalTransform(ModelData *modelData) const
{
  auto m = calcTRSDataMatrix(modelData->localTRS);

 //geometryData->localTransform = m;

  return m;
}

CMatrix3D
CImportFBX::
calcTRSDataMatrix(const TRSData &trs) const
{
  auto t = CMatrix3D::identity();
  auto s = CMatrix3D::identity();
  auto r = CMatrix3D::identity();

  if (trs.translation) {
    auto p = trs.translation.value();
    t = CMatrix3D::translation(p.x, p.y, p.z);
  }

  if (trs.rotation) {
    auto p = trs.rotation.value();
    r = CMatrix3D::rotation(CMathGen::DegToRad(p.x),
                            CMathGen::DegToRad(p.y),
                            CMathGen::DegToRad(p.z));
  }

  if (trs.scaling) {
    auto p = trs.scaling.value();
    s = CMatrix3D::scale(p.x, p.y, p.z);
  }

  return t*r*s;
//return s*r*t;
}

void
CImportFBX::
calcModelDataHierTRS(ModelData *m, std::vector<TRSData> &trsArray) const
{
  if (m->parent)
    calcModelDataHierTRS(m->parent, trsArray);

  trsArray.push_back(m->localTRS);
}

CMatrix3D
CImportFBX::
calcModelDataHierTransform(ModelData *m) const
{
  auto t = calcModelDataLocalTransform(m);

  if (m->parent)
    t = calcModelDataHierTransform(m->parent)*t;

  return t;
}

std::string
CImportFBX::
calcModelDataHierName(ModelData *m) const
{
  auto n = m->name + "(" + m->type + ")";

  if (m->parent)
    n = calcModelDataHierName(m->parent) + "/" + n;

  return n;
}

void
CImportFBX::
addGeometryObject(GeometryData *geometryData)
{
  if (geometryData->vertexData.empty())
    return;

  if (geometryData->type != "Mesh")
    return;

  //---

  assert(! geometryData->object);

  auto name = geometryData->name;

  if (name == "")
    name = getObjectName();

  geometryData->object = CGeometryInst->createObject3D(scene_, name);

  //---

  scene_->addObject(geometryData->object);

  object_->addChild(geometryData->object);

  //---

  // get material data

  MaterialData *materialData = nullptr;

  if (geometryData->modelData && ! geometryData->modelData->materialData.empty())
    materialData = geometryData->modelData->materialData.begin()->second;

  CGeomMaterial *material = nullptr;

  if (materialData) {
    material = scene_->getMaterial(materialData->name);

    if (! material) {
      material = new CGeomMaterial;

      material->setName(materialData->name);

      if (materialData->ambientColor)
        material->setAmbient(*materialData->ambientColor);

      if (materialData->diffuseColor)
        material->setDiffuse(*materialData->diffuseColor);

      if (materialData->emissionColor)
        material->setEmission(*materialData->emissionColor);

      if (materialData->specularColor)
        material->setSpecular(*materialData->specularColor);

      if (materialData->shininess)
        material->setShininess(*materialData->shininess);

      for (const auto &pt : materialData->textureMap) {
        const auto &textureType = pt.first;
        const auto &textureData = pt.second;

        for (const auto &pt1 : textureData) {
          auto *textureData1 = pt1.second;

          if (textureType == "NormalMap")
            material->setNormalTexture(textureData1->texture);
          else
            material->setDiffuseTexture(textureData1->texture);
        }
      }

      scene_->addMaterial(material);
    }
  }

  if (material)
    geometryData->object->setMaterialP(material);

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

  auto *vertexData = &geometryData->vertexData[0];

  auto ni = uint(vertexData->indices.size());
  auto nv = uint(vertexData->values .size());
  auto nv1 = nv/3;

  //---

  NormalData *normalData = nullptr;
  uint        nni = 0, nnv = 0;

  if (! geometryData->normalData.empty()) {
    normalData = &geometryData->normalData[0];

    nni = uint(normalData->indices.size());
    nnv = uint(normalData->values .size());
  }

  auto nnv1 = nnv/3;

  auto hasNormalIndex   = (nni == ni && nnv > 0);
  auto hasNormalVert    = (nni == 0 && nnv1 == nv1);
  auto hasNormalVertInd = (nni == 0 && nnv1 == ni);

  if (! hasNormalIndex && ! hasNormalVert && ! hasNormalVertInd && nnv > 0)
    std::cerr << "Ignored texture data\n";

  //---

  ColorData *colorData = nullptr;
  uint       nci = 0, ncv = 0;

  if (! geometryData->colorData.empty()) {
    colorData = &geometryData->colorData[0];

    nci = uint(colorData->indices.size());
    ncv = uint(colorData->values .size());
  }

  auto ncv1 = (colorAlpha_ ? ncv/4 : ncv/3);

  auto hasColorIndex = (nci == ni && ncv > 0);
  auto hasColorVert  = (nci == 0 && ncv1 == nv1);

  if (! hasColorIndex && ! hasColorVert && ncv > 0)
    std::cerr << "Ignored color data\n";

  //---

  UVData *uvData = nullptr;
  uint    nti = 0, ntv = 0;

  if (! geometryData->uvData.empty()) {
    uvData = &geometryData->uvData[0];

    nti = uint(uvData->indices.size());
    ntv = uint(uvData->values .size());
  }

  auto ntv1 = ntv/2;

  auto hasTextureIndex = (nti == ni && ntv > 0);
  auto hasTextureVert  = (nti == 0 && ntv1 == nv1);

  if (! hasTextureIndex && ! hasTextureVert && ntv > 0)
    std::cerr << "Ignored texture data\n";

  //---

  auto addPolyPoint = [&](int i) {
    PolyPoint polyPoint;

    polyPoint.pointInd = vertexData->indices[i];

    bool flush = (polyPoint.pointInd < 0);

    if (flush)
      polyPoint.pointInd = ~polyPoint.pointInd;

    //---

    if      (hasNormalIndex)
      polyPoint.normalInd = normalData->indices[i];
    else if (hasNormalVert)
      polyPoint.normalInd = polyPoint.pointInd;
    else if (hasNormalVertInd)
      polyPoint.normalInd = i;

    if      (hasColorIndex)
      polyPoint.colorInd = colorData->indices[i];
    else if (hasColorVert)
      polyPoint.colorInd = polyPoint.pointInd;

    if      (hasTextureIndex)
      polyPoint.textureInd = uvData->indices[i];
    else if (hasTextureVert)
      polyPoint.textureInd = polyPoint.pointInd;

    //---

    polygon.push_back(polyPoint);

    if (flush) {
      polygons.push_back(polygon);

      polygon = Polygon();
    }
  };

#if 0
  auto ne = uint(geometryData->edges.size());

  if (ne > 0) {
    for (uint i = 0; i < ne; ++i)
      addPolyPoint(geometryData->edges[i]);
  }
  else {
    for (uint i = 0; i < ni; ++i)
      addPolyPoint(i);
  }
#else
  for (uint i = 0; i < ni; ++i)
    addPolyPoint(i);
#endif

  if (! polygon.empty())
    polygons.push_back(polygon);

  auto getPoint = [&](int iv) {
    auto p = CPoint3D(vertexData->values[iv*3    ],
                      vertexData->values[iv*3 + 1],
                      vertexData->values[iv*3 + 2]);

    return adjustPoint(p);
  };

  auto getNormal = [&](int iv) {
    auto v = CVector3D(normalData->values[iv*3    ],
                       normalData->values[iv*3 + 1],
                       normalData->values[iv*3 + 2]);

    return adjustNormal(v);
  };

  auto getColor = [&](int iv) {
    if (colorAlpha_) {
      auto r = colorData->values[iv*4    ];
      auto g = colorData->values[iv*4 + 1];
      auto b = colorData->values[iv*4 + 2];
      auto a = colorData->values[iv*4 + 3];

      return CRGBA(r, g, b, a);
    }
    else {
      auto r = colorData->values[iv*3    ];
      auto g = colorData->values[iv*3 + 1];
      auto b = colorData->values[iv*3 + 2];

      return CRGBA(r, g, b);
    }
  };

  auto getTexturePoint = [&](int iv) {
    auto u1 = uvData->values[iv*2    ];
    auto v1 = uvData->values[iv*2 + 1];

    // swap ?

    return CPoint2D(u1, v1);
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

      auto ind = geometryData->object->addVertex(getPoint(polygon2.pointInd));

      auto &vertex = geometryData->object->getVertex(ind);

      if (hasNormalIndex || hasNormalVert || hasNormalVertInd) {
        auto n = getNormal(polygon2.normalInd);

        vertex.setNormal(n);
      }

      if (hasColorIndex || hasColorVert) {
        auto c = getColor(polygon2.colorInd);

        vertex.setColor(c);
      }

      if (hasTextureIndex || hasTextureVert) {
        auto p = getTexturePoint(polygon2.textureInd);

        vertex.setTextureMap(p);

        tpoints.push_back(p);
      }

      inds.push_back(ind);
    }

    auto faceInd = geometryData->object->addIPolygon(&inds[0], uint(inds.size()));

    auto &face = geometryData->object->getFace(faceInd);

    if (! tpoints.empty())
      face.setTexturePoints(tpoints);

    if (material)
      face.setMaterialP(material);
  }
}

void
CImportFBX::
setGeometryTextures(GeometryData *geometryData)
{
  auto *modelData = geometryData->modelData;

  if (! modelData || ! geometryData->object)
    return;

  // set texture
  for (const auto &pt : modelData->textureMap) {
    auto *textureData = pt.second;

    if (textureData->type == "NormalMap")
      geometryData->object->setNormalTexture(textureData->texture);
    else
      geometryData->object->setDiffuseTexture(textureData->texture);
  }

  for (const auto &pm : modelData->materialData) {
    auto *materialData = pm.second;

    for (const auto &pt : materialData->textureMap) {
      const auto &textureType = pt.first;

      for (const auto &pt1 : pt.second) {
        auto *textureData = pt1.second;

        if (textureType == "NormalMap")
          geometryData->object->setNormalTexture(textureData->texture);
        else
          geometryData->object->setDiffuseTexture(textureData->texture);

        if (! textureData->videoData.empty()) {
          auto *videoData = textureData->videoData[0];

          for (const auto &pt2 : videoData->textureListMap) {
            const auto &textureType1 = pt2.first;
            const auto &textureList1 = pt2.second;

            for (auto *textureData1 : textureList1) {
              if (textureType1 == "NormalMap")
                geometryData->object->setNormalTexture(textureData1->texture);
              else
                geometryData->object->setDiffuseTexture(textureData1->texture);

              break;
            }
          }
        }
      }
    }
  }
}

void
CImportFBX::
processGeometryDataTree(PropDataTree *tree, GeometryData *geometryData)
{
  auto getPropertyName = [](const PropIndDataArray &pa) {
    for (const auto &pa1 : pa) {
      for (const auto &pa2 : pa1.second) {
        auto *data = pa2.second;

        return data;
      }
    }

    return static_cast<CImportFBX::PropData *>(nullptr);
  };

  auto unhandledDataName = [&](const std::string &path, const std::string &name) {
    std::cerr << "Unhandled data name " << path << "/" << name << "\n";
  };

  //---

  auto treeName = hierName(tree);

//std::cerr << "processGeometryDataTree " << treeName << "\n";

  if      (treeName == "Objects/Geometry") {
    VertexData vertexData;

    for (const auto &pd : tree->dataMap) {
      const auto &name = pd.first;

      if      (name == "PolygonVertexIndex") { // vertex indices
        for (const auto &pa : pd.second) {
          for (const auto &pa1 : pa.second) {
            auto *data = pa1.second;

            if (data->type() == DataType::INT_ARRAY) {
              vertexData.indices = data->getArrayData<int>();
              vertexData.set     = true;
            }
            else
              errorMsg("Unexpected type for " + name);
          }
        }
      }
      else if (name == "Vertices") { // vertices
        for (const auto &pa : pd.second) {
          for (const auto &pa1 : pa.second) {
            auto *data = pa1.second;

            if (data->type() == DataType::DOUBLE_ARRAY) {
              vertexData.values = data->getArrayData<double>();
              vertexData.set    = true;
            }
            else
              errorMsg("Unexpected type for " + name);
          }
        }
      }
      else if (name == "Edges") { // edges
        for (const auto &pa : pd.second) {
          for (const auto &pa1 : pa.second) {
            auto *data = pa1.second;

            if (data->type() == DataType::INT_ARRAY)
              geometryData->edges = data->getArrayData<int>();
            else
              errorMsg("Unexpected type for " + name);
          }
        }
      }
    }

    if (vertexData.set)
      geometryData->vertexData.push_back(vertexData);
  }
  else if (treeName == "Objects/Geometry/LayerElementNormal") {
    NormalData normalData;

    for (const auto &pd : tree->dataMap) {
      const auto &name = pd.first;

      if      (name == "NormalsIndex") {
        for (const auto &pa : pd.second) {
          for (const auto &pa1 : pa.second) {
            auto *data = pa1.second;

            if (data->type() == DataType::INT_ARRAY) {
              normalData.indices = data->getArrayData<int>();
              normalData.set     = true;
            }
            else
              errorMsg("Unexpected type for " + name);
          }
        }
      }
      else if (name == "Normals") { // normals
        for (const auto &pa : pd.second) {
          for (const auto &pa1 : pa.second) {
            auto *data = pa1.second;

            if (data->type() == DataType::DOUBLE_ARRAY) {
              normalData.values = data->getArrayData<double>();
              normalData.set    = true;
            }
            else
              errorMsg("Unexpected type for " + name);
          }
        }
      }
      else if (name == "Name") {
      }
      else if (name == "Version") {
      }
      else if (name == "MappingInformationType") {
      }
      else if (name == "ReferenceInformationType") {
      }
      else if (name == "NormalsW") {
      }
      else {
        unhandledDataName(treeName, name);
      }
    }

    if (normalData.set)
      geometryData->normalData.push_back(normalData);
  }
  else if (treeName == "Objects/Geometry/LayerElementColor") {
    ColorData colorData;

    for (const auto &pd : tree->dataMap) {
      const auto &name = pd.first;

      if      (name == "ColorIndex") {
        for (const auto &pa : pd.second) {
          for (const auto &pa1 : pa.second) {
            auto *data = pa1.second;

            if (data->type() == DataType::INT_ARRAY) {
              colorData.indices = data->getArrayData<int>();
              colorData.set     = true;
            }
            else
              errorMsg("Unexpected type for " + name);
          }
        }
      }
      else if (name == "Colors") { // colors
        for (const auto &pa : pd.second) {
          for (const auto &pa1 : pa.second) {
            auto *data = pa1.second;

            if (data->type() == DataType::DOUBLE_ARRAY) {
              colorData.values = data->getArrayData<double>();
              colorData.set    = true;
            }
            else
              errorMsg("Unexpected type for " + name);
          }
        }
      }
      else if (name == "Name") {
      }
      else if (name == "Version") {
      }
      else if (name == "MappingInformationType") {
      }
      else if (name == "ReferenceInformationType") {
      }
      else {
        unhandledDataName(treeName, name);
      }
    }

    if (colorData.set)
      geometryData->colorData.push_back(colorData);
  }
  else if (treeName == "Objects/Geometry/LayerElementUV") {
    UVData uvData;

    for (const auto &pd : tree->dataMap) {
      const auto &name = pd.first;

      if      (name == "UVIndex") {
        for (const auto &pa : pd.second) {
          for (const auto &pa1 : pa.second) {
            auto *data = pa1.second;

            if (data->type() == DataType::INT_ARRAY) {
              uvData.indices = data->getArrayData<int>();
              uvData.set     = true;
            }
            else
              errorMsg("Unexpected type for " + name);
          }
        }
      }
      else if (name == "UV") { // texture coords
        for (const auto &pa : pd.second) {
          for (const auto &pa1 : pa.second) {
            auto *data = pa1.second;

            if (data->type() == DataType::DOUBLE_ARRAY) {
              uvData.values = data->getArrayData<double>();
              uvData.set    = true;
            }
            else
              errorMsg("Unexpected type for " + name);
          }
        }
      }
      else if (name == "Name") {
      }
      else if (name == "Version") {
      }
      else if (name == "ReferenceInformationType") {
      }
      else if (name == "MappingInformationType") {
      }
      else {
        unhandledDataName(treeName, name);
      }
    }

    if (uvData.set)
      geometryData->uvData.push_back(uvData);
  }
  else if (treeName == "Objects/Geometry/LayerElementMaterial") {
    for (const auto &pd : tree->dataMap) {
      const auto &name = pd.first;

      if      (name == "Name") {
      }
      else if (name == "Version") {
      }
      else if (name == "MappingInformationType") {
      }
      else if (name == "ReferenceInformationType") {
      }
      else if (name == "Materials") {
        for (const auto &pa : pd.second) {
          for (const auto &pa1 : pa.second) {
            auto *data = pa1.second;

            if (data->type() == DataType::INT_ARRAY)
              geometryData->materials = data->getArrayData<int>();
            else
              errorMsg("Unexpected type for " + name);
          }
        }
      }
      else {
        unhandledDataName(treeName, name);
      }
    }
  }
  else if (treeName == "Objects/Geometry/LayerElementTexture") {
    for (const auto &pd : tree->dataMap) {
      const auto &name = pd.first;

      if      (name == "Name") {
      }
      else if (name == "Version") {
      }
      else if (name == "MappingInformationType") {
      }
      else if (name == "ReferenceInformationType") {
      }
      else if (name == "BlendMode") {
      }
      else if (name == "TextureAlpha") {
      }
      else {
        unhandledDataName(treeName, name);
      }
    }
  }
  else if (treeName == "Objects/Geometry/LayerElementNormal") {
  }
  else if (treeName == "Objects/Geometry/LayerElementSmoothing") {
  }
  else if (treeName == "Objects/Geometry/LayerElementTangent") {
  }
  else if (treeName == "Objects/Geometry/LayerElementBinormal") {
  }
  else if (treeName == "Objects/Geometry/Layer") {
  }
  else if (treeName == "Objects/Geometry/Layer/LayerElement") {
  }
  else if (treeName == "Objects/Geometry/Properties70") {
    if (isDebug()) {
      for (const auto &pd1 : tree->dataMap) {
        auto *data = getPropertyName(pd1.second);

        if (data)
          std::cerr << treeName << " " << *data << "\n";
      }
    }

    for (auto *tree1 : tree->children) {
      auto treeName1 = hierName(tree1);

      unhandledTree(treeName1);
    }
  }
  else {
    errorMsg("Unhandled tree name " + treeName);
  }

  for (auto *tree1 : tree->children)
    processGeometryDataTree(tree1, geometryData);
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
  debugMsg(name + " startOffset=" + std::to_string(startOffset) +
                  " endOffset=" + std::to_string(endOffset) +
                  " numProperties=" + std::to_string(numProperties) +
                  " propertyListLen=" + std::to_string(propertyListLen));
#endif

  if (name != "")
    infoMsg("Name: " + concatStrs(scopeName, name));

  //---

  uint ni = 0;

  auto pn = propDataTree->nameCount.find(name);

  if (pn != propDataTree->nameCount.end())
    ni = (*pn).second + 1;

  propDataTree->nameCount[name] = ni;

  for (uint pi = 0; pi < numProperties; ++pi) {
    if (! readScopeData(name, ni, pi, fileData, scopeName, propDataTree))
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

  if (fileData1.fileSize > fileData.fileMem.size())
    fileData1.fileSize = int(fileData.fileMem.size());

  auto isNullRecord = testNullRecord();

  if (isNullRecord) {
    //std::cerr << " nullRecord\n";

    fileData1.fileSize -= nullLen;

    //---

    auto *propDataTree1 = new PropDataTree;

    propDataTree1->name   = name;
    propDataTree1->parent = propDataTree;

    if (scopeName == "Objects") {
      if (name == "AnimationCurve" || name == "AnimationCurveNode" || name == "AnimationLayer" ||
          name == "AnimationStack" || name == "Deformer" || name == "Geometry" ||
          name == "Material" || name == "Model" || name == "NodeAttribute" ||
          name == "Texture" || name == "Video") {
        auto pd = propDataTree->dataMap.find(name);

        if (pd != propDataTree->dataMap.end()) {
          const auto &parentDataMap = (*pd).second;

          auto pd1 = parentDataMap.find(ni);

          if (pd1 != parentDataMap.end())
            propDataTree1->parentDataMap = (*pd1).second; // copy
        }
      }
    }

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
    if (isDebug())
      debugMsg(" No nullRecord");

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
readScopeData(const std::string &name, uint ni, uint ind, FileData &fileData,
              const std::string & /*scopeName*/, PropDataTree *propDataTree)
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
  // string
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
  // 16 bit int
  else if (t == 'Y') {
    short s;
    if (! fileData.readShort(&s))
      return errorMsg("Failed to read data for " + ucharStr(t));
    infoMsg(" Y " + std::to_string(s));

    propData->setData(s);
  }
  // 1 bit bool flag (yes/no)
  else if (t == 'C') {
    uchar c;
    if (! fileData.readUChar(&c))
      return errorMsg("Failed to read data for " + ucharStr(t));
    infoMsg(" C " + ucharStr(c));
  }
  // 32 bit int
  else if (t == 'I') {
    int i;
    if (! fileData.readInt(&i))
      return errorMsg("Failed to read data for " + ucharStr(t));
    infoMsg(" I " + std::to_string(i));

    propData->setData(i);
  }
  // float
  else if (t == 'F') {
    float f;
    if (! fileData.readFloat(&f))
      return errorMsg("Failed to read data for " + ucharStr(t));
    infoMsg(" F " + std::to_string(f));

    propData->setData(f);
  }
  // double
  else if (t == 'D') {
    double r;
    if (! fileData.readDouble(&r))
      return errorMsg("Failed to read data for " + ucharStr(t));
    infoMsg(" D " + std::to_string(r));

    propData->setData(r);
  }
  // 64 bit int
  else if (t == 'L') {
    long l;
    if (! fileData.readLong(&l))
      return errorMsg("Failed to read data for " + ucharStr(t));
    infoMsg(" L " + std::to_string(l));

    propData->setData(l);
  }
  // array of float
  else if (t == 'f') {
    std::vector<float> data;
    if (! fileData.readArrayData(data, t, depth_))
      return false;

    propData->setData(data);

    infoMsg(" f " + propData->toString());
  }
  // array of double
  else if (t == 'd') {
    std::vector<double> data;
    if (! fileData.readArrayData(data, t, depth_))
      return false;

    propData->setData(data);

    infoMsg(" d " + propData->toString());
  }
  // array of long
  else if (t == 'l') {
    std::vector<long> data;
    if (! fileData.readArrayData(data, t, depth_))
      return false;

    propData->setData(data);

    infoMsg(" l " + propData->toString());
  }
  // array of int
  else if (t == 'i') {
    std::vector<int> data;
    if (! fileData.readArrayData(data, t, depth_))
      return false;

    propData->setData(data);

    infoMsg(" i " + propData->toString());
  }
#if 0
  // array of char
  else if (t == 'c') {
    std::vector<uchar> data;
    if (! fileData.readArrayData(data, t, depth_))
      return false;

    propData->setData(data);

    infoMsg(" c " + propData->toString());
  }
#endif
  // binary data
  else if (t == 'b') {
    std::vector<uchar> data;
    if (! fileData.readArrayData(data, t, depth_))
      return false;

    propData->setData(data);

    infoMsg(" b " + propData->toString());
  }
  else {
    std::cerr << "Invalid Code: "; printUChar(t); std::cerr << "\n";
    return false;
  }

  propDataTree->dataMap[name][ni][ind] = propData;

#if 0
  if (isDebug()) {
    auto hname = hierName(propDataTree);

    if (ind == 0)
      std::cerr << hname << ":" << scopeName << "\n";

    std::cerr << " " << name << "=" << propData->toString() << "\n";
  }
#endif

  return true;
}

bool
CImportFBX::
loadTexture(TextureData *textureData, std::string &fileName)
{
  std::vector<std::string> fileNames;

  auto addFileName = [&](const std::string &fname) {
    if (fname == "")
      return;

    auto fname1 = remapFile(fname);

    if (fname1 != fname)
      fileNames.push_back(fname1);

    fileNames.push_back(fname);
  };

  addFileName(textureData->fileName);
  addFileName(textureData->media);

  if (fileNames.empty())
    return true;

  for (const auto &fileName1 : fileNames) {
    if (CFile::exists(fileName1)) {
      fileName = fileName1;
      break;
    }
  }

  if (fileName == "") {
    fileName = fileNames.front();
    return false;
  }

  CImageFileSrc src(fileName);

  auto image = CImageMgrInst->createImage(src);

  image->flipH();

  textureData->texture = CGeometryInst->createTexture(image);

  textureData->texture->setName(fileName);

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
