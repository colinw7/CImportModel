#include <CImportGLTF.h>
#include <CGeometry3D.h>
#include <CJson.h>
#include <CMinMax.h>
#include <CQuaternion.h>
#include <CEncode64.h>

namespace {

auto errorMsg(const std::string &msg) -> bool {
  std::cerr << "[33mError[0m: " << msg << "\n";
  return false;
}

auto warnMsg(const std::string &msg) -> void {
  std::cerr << "[35mWarn[0m: " << msg << "\n";
}

auto debugMsg(const std::string &msg) -> void {
  std::cerr << "[31mDBG:[0m" << msg << "\n";
}

auto TODO(const std::string &msg) -> void {
  std::cerr << "[34mTODO[0m: " << msg << "\n";
}

auto TODOValue(const std::string &msg, const CJson::ValueP &value) -> void {
  std::cerr << "[34mTODO[0m: " << msg << " : " + std::string(value->typeName()) << "\n";
}

enum Constants {
  TYPE_SIGNED_BYTE    = 5120, // GL_SIGNED_BYTE
  TYPE_UNSIGNED_BYTE  = 5121, // GL_UNSIGNED_BYTE
  TYPE_SIGNED_SHORT   = 5122, // GL_SIGNED_SHORT
  TYPE_UNSIGNED_SHORT = 5123, // GL_UNSIGNED_SHORT
  TYPE_SIGNED_INT     = 5124, // GL_SIGNED_INT
  TYPE_UNSIGNED_INT   = 5125, // GL_UNSIGNED_INT
  TYPE_FLOAT          = 5126, // GL_FLOAT

  TARGET_ARRAY_BUFFER         = 0x8892,
  TARGET_ELEMENT_ARRAY_BUFFER = 0x8893,

  MODE_POINTS         = 0,
  MODE_LINE           = 1,
  MODE_LINE_LOOP      = 2,
  MODE_TRIANGLES      = 4,
  MODE_TRIANGLE_STRIP = 5,
  MODE_TRIANGLE_FAN   = 6,
};

CRGBA colorRGBA(const CImportGLTF::Color &c) {
  return CRGBA(c.r, c.g, c.b, c.a);
}

template<typename T>
void printVector(const std::string &name, const std::vector<T> &v) {
  if (v.empty()) return;
  std::cerr << name << "\n";
  for (const auto &v1 : v)
    std::cerr << " " << v1;
  std::cerr << "\n";
}

}

//---

CImportGLTF::
CImportGLTF(CGeomScene3D *scene, const std::string &name) :
 scene_(scene)
{
  if (! scene_) {
    scene_  = CGeometryInst->createScene3D();
    pscene_ = SceneP(scene_);
  }

  auto name1 = name;

  if (name1 == "")
    name1 = "gltf";

  object_ = CGeometryInst->createObject3D(scene_, name1);

  scene_->addObject(object_);

  if (! pobject_)
    pobject_ = ObjectP(object_);

  rootObject_ = object_;
}

CImportGLTF::
~CImportGLTF()
{
}

bool
CImportGLTF::
read(CFile &file)
{
  file_ = &file;

  binary_ = false;

  //---

  // check magic for bin file
  uchar buffer[5];

  if (file_->read(buffer, 4)) {
    // 'glTF'
    if (buffer[0] == 0x67 && buffer[1] == 0x6c && buffer[2] == 0x54 && buffer[3] == 0x46) {
      binary_ = true;

      if (! readBin())
        return false;

      return true;
    }
  }

  //---

  file_->rewind();

  if (! readJson())
    return false;

  return true;
}

bool
CImportGLTF::
readBin()
{
  uchar buffer[256];

  auto readInteger = [&](uint *integer) {
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

  //---

  // get version (should be 1 or 2)
  if (! readInteger(&version_))
    return false;

  if (isDebug())
    std::cerr << "version: " << version_ << "\n";

  //---

  uint fpos = 0;

  // get file length (including 12 byte header)
  uint length;
  if (! readInteger(&length))
    return false;

  fpos   += 12;
  length -= 12;

  //---

  if      (version_ == 1) {
    // json chunk
    Chunk chunk;

    if (! readInteger(&chunk.length))
      return false;

    if (! readInteger(&chunk.type))
      return false;

    chunk.buffer.data.resize(chunk.length + 1);

    if (! file_->read(&chunk.buffer.data[0], chunk.length))
      return errorMsg("failed to read chunk");

    // parse json
    chunk.buffer.data[chunk.length] = '\0';

    std::string str(reinterpret_cast<char *>(&chunk.buffer.data[0]));

    if (! parseJson(str, jsonData_))
      return false;

    fpos   += chunk.length + 8;
    length -= chunk.length + 8;

    //---

    chunk.length = length;

    chunk.buffer.data.resize(chunk.length + 1);

    if (! file_->read(&chunk.buffer.data[0], chunk.length))
      return errorMsg("failed to read chunk");

    jsonData_.chunks.push_back(std::move(chunk));
  }
  else if (version_ == 2) {
    // read chunks
    uint ic = 0;

    while (length > 0) {
      Chunk chunk;

      if (! readInteger(&chunk.length))
        return false;

      if (! readInteger(&chunk.type))
        return false;

      chunk.buffer.data.resize(chunk.length + 1);

      if (! file_->read(&chunk.buffer.data[0], chunk.length))
        return errorMsg("failed to read chunk");

      if (chunk.type == 0) {
        if (ic == 0)
          chunk.type = 0x4e4f534a;
        else
          chunk.type = 0x004e4942;
      }

      if      (chunk.type == 0x4e4f534a) { // "JSON"
        // parse json
        chunk.buffer.data[chunk.length] = '\0';

        std::string str(reinterpret_cast<char *>(&chunk.buffer.data[0]));

        if (! parseJson(str, jsonData_))
          return false;
      }
      else if (chunk.type == 0x004e4942) { // "BIN"
        // add bin
        jsonData_.chunks.push_back(chunk);

        if (isDebug())
          std::cerr << "BIN: " << fpos << " " << chunk.length << "\n";
      }
      else
        return errorMsg("Invalid chunk");

      fpos   += chunk.length + 8;
      length -= chunk.length + 8;

      ++ic;
    }
  }
  else
    return errorMsg("Invalid version");

  //---

  if (! processData())
    return false;

  //---

  return true;
}

bool
CImportGLTF::
readJson()
{
  auto str = file_->toString();

  if (str != "" && ! parseJson(str, jsonData_))
    return false;

  //---

  if (! processData())
    return false;

  //---

  return true;
}

bool
CImportGLTF::
processData()
{
  for (const auto &pa : jsonData_.accessors) {
    const auto &accessor = pa.second;

    if (accessor.sparse)
      processSparseAccessor(pa.first, accessor);
    else
      processAccessor(pa.first, accessor);
  }

  //---

  // get images

  auto processImage = [&](const Image &image) {
    const uchar* data;
    long         len;

    if (! getImageData(image, data, len))
      return false;

    if (image.uri == "") {
      if (image.mimeType == "image/png" ||
          image.mimeType == "image/jpeg" ||
          image.mimeType == "image/webp") {
        if (isSaveImage() && image.name != "") {
          if (! writeFile(image.name, data, len))
            return errorMsg("Image write failed");
        }
      }
      else {
        warnMsg("Invalid image type : " + image.mimeType);
      }
    }

    return true;
  };

  for (const auto &pi : jsonData_.images) {
    if (! processImage(pi.second))
      return false;
  }

  //---

  // set inverseBindMatrix for node
  processSkins();

  //---

  // process scenes (scene root nodes)
  for (const auto &ps : jsonData_.scenes) {
    for (const auto &pn : ps.second.nodes) {
      Node *node;
      if (! getNode(pn, node))
        continue;

      if (! processNodeTransform(node, 0))
        continue;
    }
  }

  //---

  for (const auto &ps : jsonData_.scenes) {
    for (const auto &pn : ps.second.nodes) {
      Node *node;
      if (! getNode(pn, node))
        continue;

      processNodeHierTransform(node);
    }
  }

  //---

  // process nodes with mesh (individual objects)
  for (auto &pn : jsonData_.nodes) {
    auto &node = pn.second;

    if (! node.mesh.isEmpty()) {
      if (! processNodeMesh(&node))
        continue;
    }
  }

  // TODO: node for multiple objects ?
  for (auto &pn : jsonData_.nodes) {
    auto &node = pn.second;
    if (! node.added) continue;

    auto nodeInd = int(node.indName.ind);

    if (rootObject_->hasNode(nodeInd)) {
      rootObject_->setNodeLocalTransforms(nodeInd,
        node.ttransform, node.rtransform, node.stransform);
      rootObject_->setNodeLocalTransform (nodeInd, node.transform);

      rootObject_->setNodeGlobalTransform(nodeInd, node.hierTransform);
//    rootObject_->setNodeGlobalTransform(nodeInd, calcNodeGlobalTransform(&node));
    }
  }

  //---

  // process anim data
  processAnim();

  return true;
}

bool
CImportGLTF::
processSparseAccessor(const IndName &indName, const Accessor &accessor)
{
  AccessorData valuesAccessorData, indicesAccessorData;

  valuesAccessorData.data =
    getAccessorBufferData(accessor.sparseValuesBufferView, 0,
                          valuesAccessorData.byteLength, valuesAccessorData.byteStride);
  if (! valuesAccessorData.data) return false;

  indicesAccessorData.data =
    getAccessorBufferData(accessor.sparseIndicesBufferView, 0,
                          indicesAccessorData.byteLength, indicesAccessorData.byteStride);
  if (! indicesAccessorData.data) return false;

  MeshData valuesMeshData;

  valuesMeshData.type          = accessor.type;
  valuesMeshData.componentType = accessor.componentType;
  valuesMeshData.count         = accessor.sparseCount;

  if (! readMeshData(valuesAccessorData, valuesMeshData))
    return false;

  MeshData indicesMeshData;

  indicesMeshData.type          = "SCALAR";
  indicesMeshData.componentType = accessor.sparseIndicesComponentType;
  indicesMeshData.count         = accessor.sparseCount;

  if (! readMeshData(indicesAccessorData, indicesMeshData))
    return false;

  MeshData meshData;

  meshData.type          = accessor.type;
  meshData.componentType = accessor.componentType;
  meshData.count         = accessor.count;
  meshData.min           = accessor.min;
  meshData.max           = accessor.max;

  if (meshData.type == "VEC3") {
    Vec3 v;

    meshData.vec3.resize(accessor.count);

    for (int i = 0; i < accessor.sparseCount; ++i) {
      auto ind = indicesMeshData.iscalars[i];
      if (ind < 0 || ind > accessor.count)
        return errorMsg("Invalid sparse index " + std::to_string(ind));

      meshData.vec3[ind] = valuesMeshData.vec3[i];
    }
  }
  else
    return errorMsg("Invalid sparse type " + meshData.type);

  meshDatas_.add(meshData, indName);

  return true;
}

// get mesh data from accessors
bool
CImportGLTF::
processAccessor(const IndName &indName, const Accessor &accessor)
{
  AccessorData accessorData;

  accessorData.data = getAccessorBufferData(accessor.bufferView, accessor.byteOffset,
                                            accessorData.byteLength, accessorData.byteStride);
  if (! accessorData.data) return false;

  MeshData meshData;

  meshData.type          = accessor.type;
  meshData.componentType = accessor.componentType;
  meshData.count         = accessor.count;
  meshData.min           = accessor.min;
  meshData.max           = accessor.max;

  if (! readMeshData(accessorData, meshData))
    return false;

  meshDatas_.add(meshData, indName);

  return true;
}

bool
CImportGLTF::
readMeshData(const AccessorData &accessorData, MeshData &meshData) const
{
  const auto *data       = accessorData.data;
  auto        byteStride = accessorData.byteStride;
  auto        byteLength = accessorData.byteLength;

  // 1 component
  if      (meshData.type == "SCALAR") {
    if      (meshData.componentType == Constants::TYPE_UNSIGNED_SHORT) {
      if (byteStride <= 0)
        byteStride = 2;

      if (byteLength < meshData.count*byteStride)
        return errorMsg("Invalid SCALAR count");

      int id1 = 0;

      for (long i = 0; i < meshData.count; ++i) {
        auto s = readShort(data, id1); id1 += byteStride;

        meshData.iscalars.push_back(s);
      }
    }
    else if (meshData.componentType == Constants::TYPE_UNSIGNED_INT) {
      if (byteStride <= 0)
        byteStride = 4;

      if (byteLength < meshData.count*byteStride)
        return errorMsg("Invalid SCALAR count");

      int id1 = 0;

      for (long i = 0; i < meshData.count; ++i) {
        auto l = readInt(data, id1); id1 += byteStride;

        meshData.iscalars.push_back(l);
      }
    }
    else if (meshData.componentType == Constants::TYPE_UNSIGNED_BYTE) {
      if (byteStride <= 0)
        byteStride = 1;

      if (byteLength < meshData.count*byteStride)
        return errorMsg("Invalid SCALAR count");

      int id1 = 0;

      for (long i = 0; i < meshData.count; ++i) {
        auto l = readByte(data, id1); id1 += byteStride;

        meshData.iscalars.push_back(l);
      }
    }
    else if (meshData.componentType == Constants::TYPE_FLOAT) {
      if (byteStride <= 0)
        byteStride = 4;

      if (byteLength < meshData.count*byteStride)
        return errorMsg("Invalid SCALAR count");

      int id1 = 0;

      for (long i = 0; i < meshData.count; ++i) {
        auto f = readFloat(data, id1); id1 += byteStride;

        meshData.fscalars.push_back(f);
      }
    }
    else {
      //std::cerr << " " << meshData.count;
      //std::cerr << " " << meshData.componentType;
      //std::cerr << "\n";

      return errorMsg("Invalid SCALAR component type " + std::to_string(meshData.componentType));
    }
  }
  // 2 components
  else if (meshData.type == "VEC2") {
    if (meshData.componentType == Constants::TYPE_FLOAT) {
      if (byteStride <= 0)
        byteStride = 2*4;

      if (byteLength < meshData.count*byteStride)
        return errorMsg("Invalid VEC2 count");

      int id1 = 0;

      for (long i = 0; i < meshData.count; ++i) {
        Vec2 v;

        v.x = readFloat(data, id1    );
        v.y = readFloat(data, id1 + 4); id1 += byteStride;

        meshData.vec2.push_back(v);
      }
    }
    else
      return errorMsg("Invalid VEC2 component type " + std::to_string(meshData.componentType));
  }
  // 3 components
  else if (meshData.type == "VEC3") {
      if (byteStride <= 0)
        byteStride = 3*4;

    if (meshData.componentType == Constants::TYPE_FLOAT) {
      if (byteLength < meshData.count*byteStride)
        return errorMsg("Invalid VEC3 count");

      int id1 = 0;

      for (long i = 0; i < meshData.count; ++i) {
        Vec3 v;

        v.x = readFloat(data, id1    );
        v.y = readFloat(data, id1 + 4);
        v.z = readFloat(data, id1 + 8); id1 += byteStride;

        meshData.vec3.push_back(v);
      }
    }
    else
      return errorMsg("Invalid VEC3 component type " + std::to_string(meshData.componentType));
  }
  // 4 components
  else if (meshData.type == "VEC4") {
    if      (meshData.componentType == Constants::TYPE_UNSIGNED_SHORT) {
      if (byteStride <= 0)
        byteStride = 4*2;

      if (byteLength < meshData.count*byteStride)
        return errorMsg("Invalid VEC4 count");

      int id1 = 0;

      for (long i = 0; i < meshData.count; ++i) {
        Vec4 v;

        v.x = readShort(data, id1    );
        v.y = readShort(data, id1 + 2);
        v.z = readShort(data, id1 + 4);
        v.w = readShort(data, id1 + 6); id1 += byteStride;

        meshData.vec4.push_back(v);
      }
    }
    else if (meshData.componentType == Constants::TYPE_FLOAT) {
      if (byteStride <= 0)
        byteStride = 4*4;

      if (byteLength < meshData.count*byteStride)
        return errorMsg("Invalid VEC4 count");

      int id1 = 0;

      for (long i = 0; i < meshData.count; ++i) {
        Vec4 v;

        v.x = readFloat(data, id1     );
        v.y = readFloat(data, id1 +  4);
        v.z = readFloat(data, id1 +  8);
        v.w = readFloat(data, id1 + 12); id1 += byteStride;

        meshData.vec4.push_back(v);
      }
    }
    else if (meshData.componentType == Constants::TYPE_UNSIGNED_BYTE) {
      if (byteStride <= 0)
        byteStride = 4*1;

      if (byteLength < meshData.count*byteStride)
        return errorMsg("Invalid VEC4 count");

      int id1 = 0;

      for (long i = 0; i < meshData.count; ++i) {
        Vec4 v;

        v.x = readByte(data, id1     );
        v.y = readByte(data, id1 +  4);
        v.z = readByte(data, id1 +  8);
        v.w = readByte(data, id1 + 12); id1 += byteStride;

        meshData.vec4.push_back(v);
      }
    }
    else
      return errorMsg("Invalid VEC4 component type " + std::to_string(meshData.componentType));
  }
  // 4 componenents
  else if (meshData.type == "MAT2") {
    // TODO: 4 values
    TODO("Unhandled MAT2 meshData type");
  }
  // 9 componenents
  else if (meshData.type == "MAT3") {
    // TODO:  9 values
    TODO("Unhandled MAT3 meshData type");
  }
  // 16 componenents
  else if (meshData.type == "MAT4") {
    if      (meshData.componentType == Constants::TYPE_FLOAT) {
      if (byteStride <= 0)
        byteStride = 4*16;

      if (byteLength < meshData.count*byteStride)
        return errorMsg("Invalid MAT4 count");

      int id1 = 0;

      for (long i = 0; i < meshData.count; ++i) {
        Mat4 m;

        for (int j = 0; j < 16; ++j)
          m.v[j] = readFloat(data, id1 + j*4);

        m.calcMatrix();

        meshData.mat4.push_back(m);

        id1 += byteStride;
      }
    }
    else
      TODO("Unhandled MAT4 meshData type");
  }
  else {
    //std::cerr << "  byteOffset: " << bufferView.byteOffset << "\n";
    //std::cerr << "  byteLength: " << byteLength << "\n";

    return errorMsg("Invalid type '" + meshData.type + "'");
  }

  if (isDebugData()) {
    printVector<CImportGLTF::Vec4 >("VEC4"  , meshData.vec4);
    printVector<CImportGLTF::Vec3 >("VEC3"  , meshData.vec3);
    printVector<CImportGLTF::Vec2 >("VEC2"  , meshData.vec2);
    printVector<long              >("SCALAR", meshData.iscalars);
    printVector<float             >("SCALAR", meshData.fscalars);
  }

  return true;
}

const uchar *
CImportGLTF::
getAccessorBufferData(const IndName &accessorBufferView, long accessorByteOffset,
                      long &byteLength, int &byteStride)
{
  // get accessor buffer view
  BufferView bufferView;

  if (! getBufferView(accessorBufferView, bufferView)) {
    (void) errorMsg("Invalid accessor buffer view"); return nullptr;
  }

  Buffer buffer;

  if (! getBuffer(bufferView.buffer, buffer)) {
    (void) errorMsg("Invalid accessor buffer"); return nullptr;
  }

  long byteOffset1 = 0;

  if (bufferView.byteOffset > 0)
    byteOffset1 += bufferView.byteOffset;

  long byteOffset = byteOffset1;

  if (accessorByteOffset > 0)
    byteOffset += accessorByteOffset;

  const uchar *data = nullptr;

  byteLength = bufferView.byteLength;
  byteStride = int(bufferView.byteStride);

  if (byteStride < 0)
    byteStride = 0;

  Chunk *chunk { nullptr };

  if (binary_) {
    if (version_ == 1) {
      if (! getChunk(0, chunk)) {
        (void) errorMsg("Invalid buffer chunk"); return nullptr;
      }
    }
    else {
      if (! getChunk(bufferView.buffer.ind, chunk)) {
        (void) errorMsg("Invalid buffer chunk"); return nullptr;
      }
    }

    if (byteOffset1 < 0 || byteOffset1 + byteLength > long(chunk->buffer.data.size())) {
      (void) errorMsg("Invalid buffer chunk data"); return nullptr;
    }

    data = &chunk->buffer.data[byteOffset];
  }
  else {
    if (buffer.uri != "") {
      auto pi = uriDataMap_.find(buffer.uri);

      if (pi == uriDataMap_.end()) {
        static std::string uri_base64 = "data:application/octet-stream;base64,";
        static size_t      uri_base64_len = uri_base64.size();

        if (buffer.uri.size() > uri_base64_len &&
            buffer.uri.substr(0, uri_base64_len) == uri_base64) {
          auto str1 = CEncode64Inst->decode(buffer.uri.substr(uri_base64_len));

          auto len1 = str1.size();

          uchar *data1 = new uchar [len1 + 1];

          memcpy(data1, str1.c_str(), len1);
          data1[len1] = '\0';

          uriDataMap_[buffer.uri] = UriData(data1, uint(len1));
        }
        else {
          CFile file1(buffer.uri);

          uchar *data1;
          size_t len1;

          if (! file1.readAll(&data1, &len1)) {
            (void) errorMsg("Invalid buffer file '" + buffer.uri + "'"); return nullptr;
          }

          uriDataMap_[buffer.uri] = UriData(data1, uint(len1));
        }

        pi = uriDataMap_.find(buffer.uri);
      }

      auto &uriData = (*pi).second;

      if (byteOffset1 < 0 || byteOffset1 + byteLength > uriData.len) {
        (void) errorMsg("Invalid buffer file data"); return nullptr;
      }

      data = &uriData.data[byteOffset];
    }
    else {
      (void) errorMsg("Invalid buffer"); return nullptr;
    }
  }

  return data;
}

float
CImportGLTF::
readFloat(const uchar *data, long i) const
{
  const auto *fdata = &data[i];

  uint fi = ((fdata[0] & 0xFF)      ) |
            ((fdata[1] & 0xFF) <<  8) |
            ((fdata[2] & 0xFF) << 16) |
            ((fdata[3] & 0xFF) << 24);

  return *reinterpret_cast<float *>(&fi);
}

ushort
CImportGLTF::
readShort(const uchar *data, long i) const
{
   const auto *sdata = &data[i];

   ushort s = ((sdata[0] & 0xFF)      ) |
              ((sdata[1] & 0xFF) <<  8);

   return s;
}

uint
CImportGLTF::
readInt(const uchar *data, long i) const
{
  const auto *idata = &data[i];

  uint l = ((idata[0] & 0xFF)      ) |
           ((idata[1] & 0xFF) <<  8) |
           ((idata[2] & 0xFF) << 16) |
           ((idata[3] & 0xFF) << 24);

  return l;
}

uchar
CImportGLTF::
readByte(const uchar *data, long i) const
{
  const auto *bdata = &data[i];

  uchar c = bdata[0];

  return c;
}

bool
CImportGLTF::
getChunk(long ind, Chunk* &chunk) const
{
  if (ind >= 0) {
    if (ind >= long(jsonData_.chunks.size()))
      return false;

    chunk = const_cast<Chunk *>(&jsonData_.chunks[ind]);
  }
  else
    return false;

  return true;
}

void
CImportGLTF::
processSkins()
{
  // set inverseBindMatrix for node
  for (const auto &ps : jsonData_.skins) {
    const auto &skin = ps.second;

    auto *meshData = getMeshData(skin.inverseBindMatrices);
    if (! meshData) continue;

    if (isDebug())
      printMeshData(*meshData);

    size_t im = 0;
    auto   nm = meshData->mat4.size();

    for (const auto &joint : skin.joints) {
      Node *node;
      if (! getNode(joint, node))
        continue;

      if (im >= nm)
        continue;

      node->inverseBindMatrix = meshData->mat4[im];

      ++im;
    }
  }
}

bool
CImportGLTF::
getSkin(const IndName &indName, Skin &skin) const
{
  auto ps = jsonData_.skins.find(indName);
  if (ps == jsonData_.skins.end()) return false;

  skin = (*ps).second;

  return true;
}

bool
CImportGLTF::
processAnim()
{
  for (const auto &animation : jsonData_.animations) {
    if (isDebug())
      std::cerr << "Animation: " << animation.name << "\n";

    for (const auto &channels : animation.channelsArray) {
      if (isDebug())
        std::cerr << " Channels\n";

      for (const auto &channel : channels) {
        CGeomObject3D::AnimationData animationData;

        if (isDebug())
          std::cerr << "  Channel: " << channel.sampler << " " << channel.node << " " <<
                       channel.path << "\n";

        //---

        Node *node { nullptr };

        if (! channel.node.isEmpty()) {
          if (! getNode(channel.node, node)) {
            std::cerr << "   Node: <invalid>\n"; continue;
          }
        }
        else {
          std::cerr << "   No node in channel\n";
          continue;
        }

        auto ps = animation.samplers.find(channel.sampler);
        if (ps == animation.samplers.end()) {
          std::cerr << "   Sampler: <invalid>\n"; continue;
        }

        const auto &sampler = (*ps).second;

        Accessor iaccessor;
        if (! getAccessor(sampler.input, iaccessor)) {
          std::cerr << "    Input Accessor: <invalid>\n"; continue;
        }

        auto *imeshData = getMeshData(sampler.input);
        if (! imeshData) {
          std::cerr << "Invalid Mesh Data for sampler.input\n"; continue;
        }

        Accessor oaccessor;
        if (! getAccessor(sampler.output, oaccessor)) {
          std::cerr << "    Output Accessor: <invalid>\n"; continue;
        }

        auto *omeshData = getMeshData(sampler.output);
        if (! omeshData) {
          std::cerr << "Invalid Mesh Data for sampler.output\n"; continue;
        }

        //---

        if (isDebug()) {
          std::cerr << "   Sampler: " << sampler.input << " " << sampler.output << " " <<
                       sampler.interpolation << "\n";

          std::cerr << "    Input Accessor: ";
          printMeshData(*imeshData);

          std::cerr << "    Output Accessor: ";
          printMeshData(*omeshData);
        }

        if (imeshData->type != "SCALAR" || imeshData->componentType != Constants::TYPE_FLOAT ||
            imeshData->min.size() != 1 || imeshData->max.size() != 1) {
          std::cerr << "Invalid Type for sampler.input (" << imeshData->type << " " <<
                       imeshData->componentType << ")\n"; continue;
        }

        auto ni = imeshData->fscalars.size();

        for (const auto &f : imeshData->fscalars)
          animationData.range.push_back(f);

        if (! imeshData->min.empty())
          animationData.rangeMin = imeshData->min[0];

        if (! imeshData->max.empty())
          animationData.rangeMax = imeshData->max[0];

        animationData.interpolation = sampler.interpType;

        //---

        auto nodeInd = int(node->indName.ind);

        if      (channel.path == "rotation") {
          if (omeshData->type != "VEC4") {
            std::cerr << "Invalid Type for sampler.output rotation\n"; continue;
          }

          if (ni != omeshData->vec4.size()) {
            std::cerr << "Invalid Size for sampler.output rotation\n"; continue;
          }

          for (const auto &v : omeshData->vec4)
            animationData.rotation.push_back(CQuaternion(v.w, v.x, v.y, v.z));

          rootObject_->setNodeAnimationTransformData(nodeInd, animation.name,
            CGeomObject3D::Transform::ROTATION, animationData);
        }
        else if (channel.path == "translation") {
          if (omeshData->type != "VEC3") {
            std::cerr << "Invalid Type for sampler.output translation\n"; continue;
          }

          if (ni != omeshData->vec3.size()) {
            std::cerr << "Invalid Size for sampler.output translation\n"; continue;
          }

          for (const auto &v : omeshData->vec3)
            animationData.translation.push_back(CVector3D(v.x, v.y, v.z));

          rootObject_->setNodeAnimationTransformData(nodeInd, animation.name,
            CGeomObject3D::Transform::TRANSLATION, animationData);
        }

        else if (channel.path == "scale") {
          if (omeshData->type != "VEC3") {
            std::cerr << "Invalid Type for sampler.output scale\n"; continue;
          }

          if (ni != omeshData->vec3.size()) {
            std::cerr << "Invalid Size for sampler.output scale\n"; continue;
          }

          for (const auto &v : omeshData->vec3)
            animationData.scale.push_back(CVector3D(v.x, v.y, v.z));

          rootObject_->setNodeAnimationTransformData(nodeInd, animation.name,
            CGeomObject3D::Transform::SCALE, animationData);
        }
        else if (channel.path == "weights") {
          TODO("channel.path weights");
        }
        else {
          std::cerr << "Invalid channel.path : " << channel.path << "\n";
        }

        //node->anim_data[animation.name] = animationData;
      }
    }
  }

  return true;
}

bool
CImportGLTF::
updateAnim(const std::string &name, double t)
{
  auto name1 = name;

  if (name1 == "") {
    for (const auto &animation : jsonData_.animations) {
      if (animation.name != "") {
        name1 = animation.name;
        break;
      }
    }
  }

  rootObject_->updateNodesAnimationData(name1, t);

  return true;
}

bool
CImportGLTF::
processNodeTransform(Node *node, int depth)
{
  node->depth = depth;

  //---

  // calc node transform and local translate
  node->transform = calcNodeTransform(node);

  //---

  for (const auto &pc : node->children) {
    Node *node1;

    if (! getNode(pc, node1))
      continue;

    assert(! node1->parent);

    node1->parent = node;

    if (! processNodeTransform(node1, depth + 1))
      continue;
  }

  return true;
}

void
CImportGLTF::
processNodeHierTransform(Node *node)
{
  if (node->parent) {
    node->hierTranslate = node->parent->hierTranslate*node->ttransform;
    node->hierTransform = node->parent->hierTransform*node->transform;
  }
  else {
    node->hierTranslate = node->ttransform;
    node->hierTransform = node->transform;
  }

  for (const auto &pc : node->children) {
    Node *node1;

    if (! getNode(pc, node1))
      continue;

    processNodeHierTransform(node1);
  }
}

bool
CImportGLTF::
processNodeMesh(Node *node)
{
  auto parentHierTranslate = (node->parent ? node->parent->hierTranslate : CMatrix3D::identity());

  auto mt = node->transform*parentHierTranslate;

  if (! createNodeObject(node, mt))
    return false;

  return true;
}

bool
CImportGLTF::
createNodeObject(Node *node, const CMatrix3D & /*hierTranslate*/)
{
  // create object
  auto name = node->name;

  if (name == "")
    name = "node" + node->name;

  auto *object = CGeometryInst->createObject3D(scene_, name);

  scene_->addObject(object);

  rootObject_->addChild(object);

  //---

  // set mesh
  Mesh mesh;

  if (! getMesh(node->mesh, mesh))
    return false;

  if (! processMesh(object, mesh))
    return false;

//object->transform(hierTranslate);

  //---

  // add skin (bones/nodes)
  if (! node->skin.isEmpty()) {
    // TODO: support multiple skins ?
    Skin skin;
    if (getSkin(node->skin, skin)) {
      for (const auto &joint : skin.joints) {
        Node *node1;
        if (! getNode(joint, node1))
          continue;

        auto ind = node1->indName.ind;

        CGeomObject3D::NodeData nodeData;

        nodeData.parent = (node1->parent ? int(node1->parent->indName.ind) : -1);

        if (node1->inverseBindMatrix)
          nodeData.inverseBindMatrix = node1->inverseBindMatrix->matrix().toCMatrix();

        nodeData.children.resize(node1->children.size());

        nodeData.name = node1->name;

        for (size_t ic = 0; ic < node1->children.size(); ++ic)
          nodeData.children[ic] = int(node1->children[ic].ind);

        if (! rootObject_->hasNode(int(ind)))
          rootObject_->addNode(int(ind), nodeData);

        node1->added   = true;
        node1->skinned = true;
      }
    }

#if 0
    // TODO: add missing nodes ?
    for (auto &pn : jsonData_.nodes) {
      auto &node1 = pn.second;
      if (node1.added) continue;

      auto ind = pn.first.ind;

      CGeomObject3D::NodeData nodeData;

      nodeData.parent = (node1.parent ? int(node1.parent->indName.ind) : -1);

      if (node1.inverseBindMatrix)
        nodeData.inverseBindMatrix = node1.inverseBindMatrix->matrix().toCMatrix();

      nodeData.children.resize(node1.children.size());

      for (size_t ic = 0; ic < node1.children.size(); ++ic)
        nodeData.children[ic] = int(node1.children[ic].ind);

      nodeData.name = node1.name;

      rootObject_->addNode(int(ind), nodeData);

      node1.added = true;
    }
#else
    // process scenes (scene root nodes)
    std::vector<IndName> nodeIds;

    for (const auto &ps : jsonData_.scenes) {
      for (const auto &pn : ps.second.nodes)
        nodeIds.push_back(pn);
    }

    while (! nodeIds.empty()) {
      auto indName = nodeIds.back();
      nodeIds.pop_back();

      Node *node1;
      if (! getNode(indName, node1))
        continue;

      if (node1->added) continue;

      CGeomObject3D::NodeData nodeData;

      nodeData.parent = (node1->parent ? int(node1->parent->indName.ind) : -1);

      if (node1->inverseBindMatrix)
        nodeData.inverseBindMatrix = node1->inverseBindMatrix->matrix().toCMatrix();

      nodeData.children.resize(node1->children.size());

      for (size_t ic = 0; ic < node1->children.size(); ++ic) {
        nodeData.children[ic] = int(node1->children[ic].ind);

        nodeIds.push_back(node1->children[ic]);
      }

      nodeData.name = node1->name;

      rootObject_->addNode(int(indName.ind), nodeData);

      node1->added = true;
    }
#endif
  }
  else {
    for (auto &pn : jsonData_.nodes) {
      auto &node1 = pn.second;
      if (node1.added) continue;

      auto ind = pn.first.ind;

      CGeomObject3D::NodeData nodeData;

      nodeData.parent = (node1.parent ? int(node1.parent->indName.ind) : -1);

      if (node1.inverseBindMatrix)
        nodeData.inverseBindMatrix = node1.inverseBindMatrix->matrix().toCMatrix();

      nodeData.children.resize(node1.children.size());

      for (size_t ic = 0; ic < node1.children.size(); ++ic)
        nodeData.children[ic] = int(node1.children[ic].ind);

      nodeData.name = node1.name;

      rootObject_->addNode(int(ind), nodeData);

      node1.added = true;
    }
  }

  object->setMeshNode(int(node->indName.ind));

//object->transform(node->hierTransform.inverse());

  return true;
}

CMatrix3D
CImportGLTF::
calcNodeTransform(Node *node) const
{
  if (node->matrix)
    return node->matrix->toCMatrix();

  node->ttransform = CMatrix3D::identity();
  node->rtransform = CMatrix3D::identity();
  node->stransform = CMatrix3D::identity();

  if (node->translation)
    node->ttransform = CMatrix3D::translation(node->translation->x,
                                              node->translation->y,
                                              node->translation->z);

  if (node->rotation) {
    //auto v = CVector3D(node->rotation->x, node->rotation->y, node->rotation->z);
    //node->rtransform.setRotation(node->rotation->w, v);

    auto q = CQuaternion(node->rotation->w, node->rotation->x,
                         node->rotation->y, node->rotation->z);
    q.toRotationMatrix(node->rtransform);
  }

  if (node->scale)
    node->stransform = CMatrix3D::scale(node->scale->x, node->scale->y, node->scale->z);

  return node->ttransform*node->rtransform*node->stransform;
}

#if 0
CMatrix3D
CImportGLTF::
calcNodeGlobalTransform(Node *node) const
{
  auto *pnode = node->parent;

#if 0
  if (! pnode || ! pnode->skinned)
    return node->transform;
#else
  if (! pnode)
    return node->transform;
#endif

  return calcNodeGlobalTransform(pnode)*node->transform;
}
#endif

bool
CImportGLTF::
processMesh(CGeomObject3D *object, const Mesh &mesh)
{
  for (const auto &primitive : mesh.primitives) {
    std::vector<long> indices;
    std::vector<Vec3> positions;
    std::vector<Vec3> normals;
    std::vector<Vec2> textCoords0;
    std::vector<Vec4> joints0;
    std::vector<Vec4> weights0;

    if (! primitive.indices.isEmpty()) {
      auto *indMeshData = getMeshData(primitive.indices);
      if (! indMeshData)
        return errorMsg("Invalid indices mesh");

#if 0
      if (isDebug()) {
        std::cerr << " indices\n";
        printMeshData(*indMeshData);
      }
#endif

      indices = indMeshData->iscalars;
    }

    if (! primitive.position.isEmpty()) {
      auto *positionMeshData = getMeshData(primitive.position);
      if (! positionMeshData)
        return errorMsg("Invalid mesh POSITION");

#if 0
      if (isDebug()) {
        std::cerr << " POSITION\n";
        printMeshData(*positionMeshData);
      }
#endif

      positions = positionMeshData->vec3;
    }

    if (! primitive.normal.isEmpty()) {
      auto *normalMeshData = getMeshData(primitive.normal);
      if (! normalMeshData)
        return errorMsg("Invalid mesh NORMAL");

#if 0
      if (isDebug()) {
        std::cerr << " NORMAL\n";
        printMeshData(*normalMeshData);
      }
#endif

      normals = normalMeshData->vec3;
    }

    if (! primitive.texCoord0.isEmpty()) {
      auto *texCoord0MeshData = getMeshData(primitive.texCoord0);
      if (! texCoord0MeshData)
        return errorMsg("Invalid mesh TEXCOORD_0");

#if 0
      if (isDebug()) {
        std::cerr << " TEXCOORD_0\n";
        printMeshData(*texCoord0MeshData);
      }
#endif

      textCoords0 = texCoord0MeshData->vec2;
    }

    if (! primitive.joints0.isEmpty()) {
      auto *joints0MeshData = getMeshData(primitive.joints0);
      if (! joints0MeshData)
        return errorMsg("Invalid mesh JOINTS_0");

#if 0
      if (isDebug()) {
        std::cerr << " JOINTS_0\n";
        printMeshData(*joints0MeshData);
      }
#endif

      joints0 = joints0MeshData->vec4;
    }

    if (! primitive.weights0.isEmpty()) {
      auto *weights0MeshData = getMeshData(primitive.weights0);
      if (! weights0MeshData)
        return errorMsg("Invalid mesh WEIGHTS0");

#if 0
      if (isDebug()) {
        std::cerr << " WEIGHTS0\n";
        printMeshData(*weights0MeshData);
      }
#endif

      weights0 = weights0MeshData->vec4;
    }

    auto ni = indices    .size();
    auto np = positions  .size();
    auto nn = normals    .size();
    auto nt = textCoords0.size();
    auto nj = joints0    .size();
    auto nw = weights0   .size();

    CIMinMax indMinMax;
    for (int i = 0; i < int(ni); ++i)
      indMinMax.add(int(indices[i]));

#if 0
    auto setDefaultIndices = [&]() {
      indices.resize(np);

      for (int i = 0; i < int(np); ++i)
        indices[i] = short(i);

      indMinMax.reset();

      indMinMax.add(0);
      indMinMax.add(int(np - 1));

      ni = np;
    };
#endif

    if (np == 0)
      return errorMsg("No mesh positions");

    if (ni > 0 && indMinMax.max() >= int(np))
      warnMsg("Invalid mesh indices size");

    if (nn > 0 && np != nn)
      warnMsg("Invalid mesh normals size");

    if (nt > 0 && np != nt)
      warnMsg("Invalid mesh textCoords0 size");

    if (nj > 0 && np != nj)
      warnMsg("Invalid mesh joints0 size");

    if (nw > 0 && np != nw)
      warnMsg("Invalid mesh weights0 size");

    //---

    for (const auto &p : positions) {
      auto ind = object->addVertex(CPoint3D(p.x, p.y, p.z));

      if (isDebug())
        debugMsg("add vertex: " + std::to_string(p.x) + " " + std::to_string(p.y) +
                 " " + std::to_string(p.z));

      auto &v = object->getVertex(ind);

      if (ind < nn) {
        const auto &n = normals[ind];

        v.setNormal(CVector3D(n.x, n.y, n.z));

        if (isDebug())
          debugMsg("set normal: " + std::to_string(n.x) + " " + std::to_string(n.y) +
                   " " + std::to_string(n.z));
      }

      if (ind < nt) {
        const auto &t = textCoords0[ind];

        v.setTextureMap(CPoint2D(t.x, t.y));

        if (isDebug())
          debugMsg("set texture: " + std::to_string(t.x) + " " + std::to_string(t.y));
      }

      //---

      if (nj > 0) {
        JointData jointData;

        const auto &j = joints0[ind];

        if (nw > 0) {
          jointData.w = weights0[ind];

          jointData.nodes[0].ind = int(j.x); jointData.nodes[0].weight = jointData.w.x;
          jointData.nodes[1].ind = int(j.y); jointData.nodes[1].weight = jointData.w.y;
          jointData.nodes[2].ind = int(j.z); jointData.nodes[2].weight = jointData.w.z;
          jointData.nodes[3].ind = int(j.w); jointData.nodes[3].weight = jointData.w.w;

          Node *nodes[4];
          for (int i = 0; i < 4; ++i) {
            if (! getNode(jointData.nodes[i].ind, nodes[i]))
              continue; // joint 0

            jointData.nodes[i].inverseBindMatrix = nodes[i]->inverseBindMatrix;

            jointData.skinMatrix +=
              jointData.nodes[i].inverseBindMatrix->matrix()*jointData.nodes[i].weight;
          }
        }

        CGeomVertex3D::JointData jointData1;

        for (int i = 0; i < 4; ++i) {
          auto &nodeData = jointData1.nodeDatas[i];

          nodeData.node   = jointData.nodes[i].ind;
          nodeData.weight = jointData.nodes[i].weight;
        }

        v.setJointData(jointData1);

        objectJoints_[ind] = jointData;
      }
    }

    uint numTriangles = uint(ni/3);

    for (uint i = 0; i < numTriangles; ++i) {
      auto i1 = indices[i*3 + 0];
      auto i2 = indices[i*3 + 1];
      auto i3 = indices[i*3 + 2];

      if (i1 < 0 || i1 >= int(np) ||
          i2 < 0 || i2 >= int(np) ||
          i3 < 0 || i3 >= int(np))
        continue;

      object->addITriangle(uint(i1), uint(i2), uint(i3));

      if (isDebug())
        debugMsg("add triangle: " + std::to_string(i1) + " " + std::to_string(i2) +
                 " " + std::to_string(i3));
    }

    //---

    if (! primitive.material.isEmpty()) {
      Material material;

      if (! getMaterial(primitive.material, material))
        return errorMsg("Invalid mesh material");

      //printMaterial(material, primitive.material);

      //---

      if (material.diffuse) {
        object->setFaceDiffuse(colorRGBA(material.diffuse.value()));
      }

      if (material.specular) {
        object->setFaceSpecular(colorRGBA(material.specular.value()));
      }

      if (material.baseColorTexture.index >= 0) {
        Texture texture;

        auto textureName = IndName(material.baseColorTexture.index);

        if (! getTexture(textureName, texture))
          return errorMsg("Invalid base color texture");

        if (! texture.source.isEmpty()) {
          auto pi = jsonData_.images.find(texture.source);

          if (pi == jsonData_.images.end())
            return errorMsg("Invalid base color texture image");

          const auto &image = (*pi).second;

          if (! resolveImage(image))
            return false;

          auto *texture1 = CGeometryInst->createTexture(image.image);

          texture1->setName(textureName.to_string());

          object->setDiffuseTexture(texture1);
        }
        else {
          warnMsg("Unhandled base color texture " + material.baseColorTexture.index);
        }
      }

      if (material.normalTexture.index >= 0) {
        Texture texture;

        auto textureName = IndName(material.normalTexture.index);

        if (! getTexture(IndName(material.normalTexture.index), texture))
          return errorMsg("Invalid normal texture");

        if (! texture.source.isEmpty()) {
          auto pi = jsonData_.images.find(texture.source);

          if (pi == jsonData_.images.end())
            return errorMsg("Invalid normal texture source");

          const auto &image = (*pi).second;

          if (! resolveImage(image))
            return false;

          auto *texture1 = CGeometryInst->createTexture(image.image);

          texture1->setName(textureName.to_string());

          object->setNormalTexture(texture1);
        }
        else {
          warnMsg("Unhandled normal texture " + material.normalTexture.index);
        }
      }

      if (material.emissiveTexture.index >= 0) {
        Texture texture;

        auto textureName = IndName(material.emissiveTexture.index);

        if (! getTexture(IndName(material.emissiveTexture.index), texture))
          return errorMsg("Invalid emissive texture");

        if (! texture.source.isEmpty()) {
          auto pi = jsonData_.images.find(texture.source);

          if (pi == jsonData_.images.end())
            return errorMsg("Invalid emissive texture source");

          const auto &image = (*pi).second;

          if (! resolveImage(image))
            return false;

          auto *texture1 = CGeometryInst->createTexture(image.image);

          texture1->setName(textureName.to_string());

          object->setEmissiveTexture(texture1);
        }
        else {
          warnMsg("Unhandled emissive texture " + material.emissiveTexture.index);
        }
      }
    }
  }

  return true;
}

bool
CImportGLTF::
resolveImage(const Image &image)
{
  if (image.image)
    return true;

  const uchar* data;
  long         len;

  if (! getImageData(image, data, len))
    return false;

  if      (image.mimeType == "image/png") {
    auto image1 = CImageMgrInst->createImage();

    if (! image1->read(data, len, CFILE_TYPE_IMAGE_PNG))
      return errorMsg("Invalid image data");

    image.image = image1;
  }
  else if (image.mimeType == "image/jpeg") {
    auto image1 = CImageMgrInst->createImage();

    if (! image1->read(data, len, CFILE_TYPE_IMAGE_JPG))
      return errorMsg("Invalid image data");

    image.image = image1;
  }
  else if (image.mimeType == "image/webp") {
    auto image1 = CImageMgrInst->createImage();

    if (! image1->read(data, len, CFILE_TYPE_IMAGE_WEBP))
      return errorMsg("Invalid image data");

    image.image = image1;
  }
  else
    return errorMsg("Invalid image type '" + image.mimeType + "'");

  return true;
}

bool
CImportGLTF::
getImageData(const Image &image, const uchar* &data, long &len)
{
  if      (! image.bufferView.isEmpty()) {
    BufferView bufferView;

    if (! getBufferView(image.bufferView, bufferView))
      return errorMsg("Invalid image buffer view");

    Chunk *chunk = nullptr;

    if      (bufferView.buffer.ind >= 0) {
      if (bufferView.buffer.ind >= long(jsonData_.chunks.size()))
        return errorMsg("Invalid buffer chunk");

      chunk = &jsonData_.chunks[bufferView.buffer.ind];
    }
    else if (bufferView.buffer.name != "") {
      auto pb = jsonData_.buffers.find(bufferView.buffer);

      if (pb == jsonData_.buffers.end())
        return errorMsg("Invalid buffer chunk");

      //auto *buffer = &(*pb).second;

      if (version_ == 1)
        chunk = &jsonData_.chunks[0];
      else
        return errorMsg("Invalid buffer chunk");
    }
    else {
      return errorMsg("Invalid buffer chunk");
    }

    auto byteOffset = bufferView.byteOffset;

    if (byteOffset < 0)
      byteOffset = 0;

    if (byteOffset < 0 || byteOffset + bufferView.byteLength >= long(chunk->buffer.data.size()))
      return errorMsg("Invalid buffer chunk data");

    data = &chunk->buffer.data[byteOffset];
    len  = bufferView.byteLength;
  }
  else if (image.uri != "") {
    if (! image.image) {
      image.image = CImageMgrInst->createImage();

      if (! image.image->read(image.uri))
        return errorMsg("Invalid image file '" + image.uri + "'");
    }
  }
  else {
    return errorMsg("Invalid image data");
  }

  return true;
}

bool
CImportGLTF::
getBufferView(const IndName &indName, BufferView &bufferView) const
{
  auto pn = jsonData_.bufferViews.find(indName);
  if (pn == jsonData_.bufferViews.end()) return false;

  bufferView = (*pn).second;

  return true;
}

bool
CImportGLTF::
getBuffer(const IndName &indName, Buffer &buffer) const
{
  auto pn = jsonData_.buffers.find(indName);
  if (pn == jsonData_.buffers.end()) return false;

  buffer = (*pn).second;

  return true;
}

bool
CImportGLTF::
getMaterial(const IndName &indName, Material &material) const
{
  auto pm = jsonData_.materials.find(indName);
  if (pm == jsonData_.materials.end()) return false;

  material = (*pm).second;

  return true;
}

bool
CImportGLTF::
getTexture(const IndName &indName, Texture &texture) const
{
  auto pt = jsonData_.textures.find(indName);
  if (pt == jsonData_.textures.end()) return false;

  texture = (*pt).second;

  return true;
}

bool
CImportGLTF::
getNode(const IndName &indName, Node* &node) const
{
  auto pn = jsonData_.nodes.find(indName);
  if (pn == jsonData_.nodes.end()) return false;

  node = const_cast<Node *>(&(*pn).second);

  return true;
}

bool
CImportGLTF::
getAccessor(const IndName &indName, Accessor &accessor) const
{
  auto pn = jsonData_.accessors.find(indName);
  if (pn == jsonData_.accessors.end()) return false;

  accessor = (*pn).second;

  return true;
}

bool
CImportGLTF::
getMesh(const IndName &indName, Mesh &mesh) const
{
  auto pm = jsonData_.meshes.find(indName);
  if (pm == jsonData_.meshes.end()) return false;

  mesh = (*pm).second;

  return true;
}

const CImportGLTF::MeshData *
CImportGLTF::
getMeshData(const IndName &indName) const
{
  const MeshData *meshData = nullptr;

  auto pm = meshDatas_.find(indName);

  if (pm != meshDatas_.end())
    meshData = &(*pm).second;

  return meshData;
}

bool
CImportGLTF::
writeFile(const std::string &filename, const uchar *data, long len) const
{
  auto *fp = fopen(filename.c_str(), "w");
  if (! fp) return false;

  fwrite(data, len, 1, fp);

  fclose(fp);

  return true;
}

bool
CImportGLTF::
parseJson(const std::string &jsonStr, JsonData &jsonData) const
{
  auto valueNumber = [](const CJson::ValueP &value, double &number) {
    if (! value->isNumber())
      return false;

    auto *jnumber = value->cast<CJson::Number>();

    number = jnumber->value();

    return true;
  };

  auto valueLong = [](const CJson::ValueP &value, long &number) {
     if (! value->isNumber())
      return false;

    auto *jnumber = value->cast<CJson::Number>();

    number = long(jnumber->value());

    return true;
  };

  auto valueString = [](const CJson::ValueP &value, std::string &str) {
    if (! value->isString())
      return false;

    auto *jstr = value->cast<CJson::String>();

    str = jstr->value();

    return true;
  };

  auto valueBool = [](const CJson::ValueP &value, bool &b) {
    if (! value->isBool())
      return false;

    b = value->toBool();

    return true;
  };

  auto valueIndName = [&](const CJson::ValueP &value, IndName &indName) {
    if (value->isString()) {
      if (! valueString(value, indName.name))
        return false;
    }
    else {
      if (! valueLong(value, indName.ind))
        return false;
    }

    return true;
  };

  auto valueColor = [&](const CJson::ValueP &value, Color &color) {
    if      (value->isArray()) {
      auto *arr = value->cast<CJson::Array>();

      std::vector<float> rvalues;

      for (const auto &pv : arr->values()) {
        double r;
        if (! valueNumber(pv, r))
          return false;

        rvalues.push_back(float(r));
      }

      if (rvalues.size() != 4)
        return false;

      color = Color(rvalues[0], rvalues[1], rvalues[2], rvalues[3]);
    }
    else if (value->isString()) {
      std::string str;
      if (! valueString(value, str))
        return false;

      color = Color(str);
    }
    else
      return false;

    return true;
  };

  auto arrayNumbers = [&](const CJson::ValueP &value, std::vector<float> &values) {
   if (! value->isArray())
     return false;

    auto *arr = value->cast<CJson::Array>();

    for (const auto &pv : arr->values()) {
      double r;
      if (! valueNumber(pv, r))
        return false;

      values.push_back(float(r));
    }

    return true;
  };

  //---

  if (isDebug())
    debugMsg(jsonStr);

  CJson json;

  CJson::ValueP value;

  if (! json.loadString(jsonStr, value))
    return errorMsg("Invalid JSON");

  //value->print(); std::cout << "\n";

  if (! value->isObject())
    return errorMsg("Invalid JSON data");

  auto *obj = value->cast<CJson::Object>();

  for (const auto &pnv : obj->nameValueMap()) {
    if      (pnv.first == "accessors") {
      auto readAccessor = [&](CJson::Object *jobj, Accessor &accessor) {
        for (const auto &pnv1 : jobj->nameValueMap()) {
          if      (pnv1.first == "bufferView") {
            if (! valueIndName(pnv1.second, accessor.bufferView))
              return errorMsg("Invalid accessors bufferView ind/name");
          }
          else if (pnv1.first == "byteOffset") {
            if (! valueLong(pnv1.second, accessor.byteOffset))
              return errorMsg("Invalid accessors byteOffset number");
          }
          else if (pnv1.first == "byteStride") {
            if (! valueLong(pnv1.second, accessor.byteStride))
              return errorMsg("Invalid accessors byteStride number");
          }
          else if (pnv1.first == "type") {
            if (! valueString(pnv1.second, accessor.type))
              return errorMsg("Invalid accessors type string");
          }
          else if (pnv1.first == "componentType") {
            if (! valueLong(pnv1.second, accessor.componentType))
              return errorMsg("Invalid accessors componentType number");
          }
          else if (pnv1.first == "count") {
            if (! valueLong(pnv1.second, accessor.count))
              return errorMsg("Invalid accessors count number");
          }
          else if (pnv1.first == "min") {
            if (! pnv1.second->isArray())
              return errorMsg("Invalid JSON " + pnv.first + " max data");

            auto *maxArr = pnv1.second->cast<CJson::Array>();

            for (const auto &pv2 : maxArr->values()) {
              double r;
              if (! valueNumber(pv2, r))
                return errorMsg("Invalid min number");

              accessor.min.push_back(r);
            }
          }
          else if (pnv1.first == "max") {
            if (! pnv1.second->isArray())
              return errorMsg("Invalid JSON " + pnv.first + " max data");

            auto *maxArr = pnv1.second->cast<CJson::Array>();

            for (const auto &pv2 : maxArr->values()) {
              double r;
              if (! valueNumber(pv2, r))
                return errorMsg("Invalid max number");

              accessor.max.push_back(r);
            }
          }
          else if (pnv1.first == "name") {
            if (! valueString(pnv1.second, accessor.name))
              return errorMsg("Invalid accessor name");
          }
          else if (pnv1.first == "normalized") {
            // TODO
            TODOValue("Unhandled accessor normalized", pnv1.second);
          }
          else if (pnv1.first == "sparse") {
            accessor.sparse = true;

            if (! pnv1.second->isObject())
              return errorMsg("Invalid JSON " + pnv.first + " sparse data");

            auto *sparseObj = pnv1.second->cast<CJson::Object>();

            for (const auto &pnv2 : sparseObj->nameValueMap()) {
              auto name = pnv2.first;

              if      (name == "count") {
                if (! valueLong(pnv2.second, accessor.sparseCount))
                  return errorMsg("Invalid accessor sparse count");
              }
              else if (name == "indices") {
                if (! pnv2.second->isObject())
                  return errorMsg("Invalid JSON " + pnv.first + " indices data");

                auto *sparseIndicesObj = pnv2.second->cast<CJson::Object>();

                for (const auto &pnv3 : sparseIndicesObj->nameValueMap()) {
                  auto name1 = pnv3.first;

                  if      (name1 == "bufferView") {
                    if (! valueLong(pnv3.second, accessor.sparseIndicesBufferView))
                      return errorMsg("Invalid accessor sparse indices bufferView");
                  }
                  else if (name1 == "componentType") {
                    if (! valueLong(pnv3.second, accessor.sparseIndicesComponentType))
                      return errorMsg("Invalid accessor sparse indices componentType");
                  }
                  else
                    return errorMsg("Unhandled accessor sparse indices name " + name1);
                }
              }
              else if (name == "values") {
                if (! pnv2.second->isObject())
                  return errorMsg("Invalid JSON " + pnv.first + " values data");

                auto *sparseValuesObj = pnv2.second->cast<CJson::Object>();

                for (const auto &pnv3 : sparseValuesObj->nameValueMap()) {
                  auto name1 = pnv3.first;

                  if (name1 == "bufferView") {
                    if (! valueLong(pnv3.second, accessor.sparseValuesBufferView))
                      return errorMsg("Invalid accessor sparse values bufferView");
                  }
                  else
                    return errorMsg("Unhandled accessor sparse values name " + name1);
                }
              }
              else {
                return errorMsg("Invalid sparse name " + name);
              }
            }
          }
          else
            return errorMsg("Invalid accessors name " + pnv1.first);
        }

        return true;
      };

      if      (pnv.second->isArray()) {
        auto *arr1 = pnv.second->cast<CJson::Array>();

        for (const auto &pv1 : arr1->values()) {
          if (! pv1->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " data");

          auto *obj1 = pv1->cast<CJson::Object>();

          Accessor accessor;

          if (! readAccessor(obj1, accessor))
            return false;

          jsonData.accessors.add(accessor, IndName(-1));
        }
      }
      else if (pnv.second->isObject()) {
        auto *obj1 = pnv.second->cast<CJson::Object>();

        for (const auto &pnv1 : obj1->nameValueMap()) {
          auto name = pnv1.first;

          if (! pnv1.second->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " data");

          auto *obj2 = pnv1.second->cast<CJson::Object>();

          Accessor accessor;

          if (! readAccessor(obj2, accessor))
            return false;

          jsonData.accessors.add(accessor, IndName(name));
        }
      }
      else {
        return errorMsg("Invalid JSON " + pnv.first + " data");
      }
    }
    else if (pnv.first == "asset") {
      if (! pnv.second->isObject())
        return errorMsg("Invalid JSON " + pnv.first + " data");

      jsonData.asset.set = true;

      auto *obj1 = pnv.second->cast<CJson::Object>();

      for (const auto &pnv1 : obj1->nameValueMap()) {
        if      (pnv1.first == "generator") {
          if (! valueString(pnv1.second, jsonData.asset.generator))
            return errorMsg("Invalid generator string");
        }
        else if (pnv1.first == "version") {
          if (pnv1.second->isString()) {
            if (! valueString(pnv1.second, jsonData.asset.versionId))
              return errorMsg("Invalid version string");
          }
          else {
            if (! valueLong(pnv1.second, jsonData.asset.versionNumber))
              return errorMsg("Invalid version number");
          }
        }
        else if (pnv1.first == "premultipliedAlpha") {
          // TODO : bool
          TODOValue("Unhandled asset premultipliedAlpha", pnv1.second);
        }
        else if (pnv1.first == "profile") {
          // TODO : object
          TODOValue("Unhandled asset profile", pnv1.second);
        }
        else if (pnv1.first == "copyright") {
          if (! valueString(pnv1.second, jsonData.asset.copyright))
            return errorMsg("Invalid asset copyright");
        }
        else {
          debugMsg("  " + pnv.first + "/" + pnv1.first + " : " + pnv1.second->typeName());
        }
      }
    }
    else if (pnv.first == "bufferViews") {
      auto readBufferView = [&](CJson::Object *jobj, BufferView &bufferView) {
        for (const auto &pnv2 : jobj->nameValueMap()) {
          if      (pnv2.first == "buffer") {
            if (! valueIndName(pnv2.second, bufferView.buffer))
              return errorMsg("Invalid bufferViews buffer ind/name");
          }
          else if (pnv2.first == "byteOffset") {
            if (! valueLong(pnv2.second, bufferView.byteOffset))
              return errorMsg("Invalid bufferViews byteOffset number");
          }
          else if (pnv2.first == "byteLength") {
            if (! valueLong(pnv2.second, bufferView.byteLength))
              return errorMsg("Invalid bufferViews byteLength number");
          }
          else if (pnv2.first == "byteStride") {
            if (! valueLong(pnv2.second, bufferView.byteStride))
              return errorMsg("Invalid bufferViews byteStride number");
          }
          else if (pnv2.first == "target") {
            if (! valueLong(pnv2.second, bufferView.target))
              return errorMsg("Invalid bufferViews target number");
          }
          else if (pnv2.first == "name") {
            if (! valueString(pnv2.second, bufferView.name))
              return errorMsg("Unhandled bufferViews name");
          }
          else {
            return errorMsg("Invalid buffer name " + pnv2.first);
          }
        }

        return true;
      };

      if      (pnv.second->isArray()) {
        auto *arr1 = pnv.second->cast<CJson::Array>();

        for (const auto &pv1 : arr1->values()) {
          if (! pv1->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pv1->cast<CJson::Object>();

          BufferView bufferView;

          if (! readBufferView(obj2, bufferView))
            return false;

          jsonData.bufferViews.add(bufferView, IndName(-1));
        }
      }
      else if (pnv.second->isObject()) {
        auto *obj1 = pnv.second->cast<CJson::Object>();

        for (const auto &pnv1 : obj1->nameValueMap()) {
          auto name = pnv1.first;

          if (! pnv1.second->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " data");

          auto *obj2 = pnv1.second->cast<CJson::Object>();

          BufferView bufferView;

          if (! readBufferView(obj2, bufferView))
            return false;

          jsonData.bufferViews.add(bufferView, IndName(name));
        }
      }
      else {
        return errorMsg("Invalid JSON " + pnv.first + " data");
      }
    }
    else if (pnv.first == "buffers") {
      auto readBuffer = [&](CJson::Object *jobj, Buffer &buffer) {
        for (const auto &pnv2 : jobj->nameValueMap()) {
          if      (pnv2.first == "byteLength") {
            if (! valueLong(pnv2.second, buffer.byteLength))
              return errorMsg("Invalid buffers byteLength number");
          }
          else if (pnv2.first == "type") {
            if (! valueString(pnv2.second, buffer.type))
              return errorMsg("Invalid buffers type name");
          }
          else if (pnv2.first == "uri") {
            if (! valueString(pnv2.second, buffer.uri))
              return errorMsg("Invalid buffers uri name");
          }
          else
            debugMsg("  " + pnv.first + "/" + pnv2.first + " : " + pnv2.second->typeName());
        }

        return true;
      };

      if      (pnv.second->isArray()) {
        auto *arr1 = pnv.second->cast<CJson::Array>();

        for (const auto &pv1 : arr1->values()) {
          if (! pv1->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pv1->cast<CJson::Object>();

          Buffer buffer;

          if (! readBuffer(obj2, buffer))
            return false;

          jsonData.buffers.add(buffer, IndName(-1));
        }
      }
      else if (pnv.second->isObject()) {
        auto *obj1 = pnv.second->cast<CJson::Object>();

        for (const auto &pnv1 : obj1->nameValueMap()) {
          auto name = pnv1.first;

          if (! pnv1.second->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pnv1.second->cast<CJson::Object>();

          Buffer buffer;

          if (! readBuffer(obj2, buffer))
            return false;

          jsonData.buffers.add(buffer, IndName(name));
        }
      }
      else {
        return errorMsg("Invalid JSON " + pnv.first + " data");
      }
    }
    else if (pnv.first == "images") {
      auto readImage = [&](CJson::Object *jobj, Image &image) {
        auto processNameValue1 = [&](const std::string &name, const CJson::ValueP &nvalue) {
          if      (name == "bufferView") {
            if (! valueIndName(nvalue, image.bufferView))
              return errorMsg("Invalid images bufferView ind/name");
          }
          else if (name == "mimeType") {
            if (! valueString(nvalue, image.mimeType))
              return errorMsg("Invalid images mimeType string");
          }
          else if (name == "name") {
            if (! valueString(nvalue, image.name))
              return errorMsg("Invalid images name string");
          }
          else if (name == "url") {
            if (! valueString(nvalue, image.url))
              return errorMsg("Invalid images url string");
          }
          else if (name == "uri") {
            if (! valueString(nvalue, image.uri))
              return errorMsg("Invalid images uri string");
          }
          else if (name == "width") {
            if (! valueLong(nvalue, image.width))
              return errorMsg("Invalid images width value");
          }
          else if (name == "height") {
            if (! valueLong(nvalue, image.height))
              return errorMsg("Invalid images height value");
          }
          else {
            debugMsg("  " + pnv.first + "/" + name + " : " + nvalue->typeName());
            return false;
          }

          return true;
        };

        auto processNameValue = [&](const std::string &name, const CJson::ValueP &nvalue) {
          if      (name == "bufferView") {
            if (! valueIndName(nvalue, image.bufferView))
              return errorMsg("Invalid images bufferView ind/name");
          }
          else if (name == "mimeType") {
            if (! valueString(nvalue, image.mimeType))
              return errorMsg("Invalid images mimeType string");
          }
          else if (name == "name") {
            if (! valueString(nvalue, image.name))
              return errorMsg("Invalid images name string");
          }
          else if (name == "url") {
            if (! valueString(nvalue, image.url))
              return errorMsg("Invalid images url string");
          }
          else if (name == "uri") {
            if (! valueString(nvalue, image.uri))
              return errorMsg("Invalid images uri string");
          }
          else if (name == "width") {
            if (! valueLong(nvalue, image.width))
              return errorMsg("Invalid images width number");
          }
          else if (name == "height") {
            if (! valueLong(nvalue, image.height))
              return errorMsg("Invalid images height number");
          }
          else if (name == "extensions") {
            if (! nvalue->isObject())
              return errorMsg("Invalid image extensions");

            auto *obj2 = nvalue->cast<CJson::Object>();

            for (const auto &pnv3 : obj2->nameValueMap()) {
              if (pnv3.first == "KHR_binary_glTF") {
                if (! pnv3.second->isObject())
                  return errorMsg("Invalid image extensions/KHR_binary_glTF");

                auto *obj3 = pnv3.second->cast<CJson::Object>();

                for (const auto &pnv4 : obj3->nameValueMap()) {
                  if (! processNameValue1(pnv4.first, pnv4.second))
                    return false;
                }
              }
              else {
                debugMsg("  " + pnv.first + "/" + name + "/" + pnv3.first);
              }
            }
          }
          else {
            debugMsg("  " + pnv.first + "/" + name + " : " + nvalue->typeName());
            return false;
          }

          return true;
        };

        for (const auto &pnv2 : jobj->nameValueMap()) {
          if (! processNameValue(pnv2.first, pnv2.second))
            return false;
        }

        return true;
      };

      if      (pnv.second->isArray()) {
        auto *arr1 = pnv.second->cast<CJson::Array>();

        for (const auto &pv1 : arr1->values()) {
          if (! pv1->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pv1->cast<CJson::Object>();

          Image image;

          if (! readImage(obj2, image))
            return false;

          jsonData.images.add(image, IndName(-1));
        }
      }
      else if (pnv.second->isObject()) {
        auto *obj1 = pnv.second->cast<CJson::Object>();

        for (const auto &pnv1 : obj1->nameValueMap()) {
          auto name = pnv1.first;

          if (! pnv1.second->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pnv1.second->cast<CJson::Object>();

          Image image;

          if (! readImage(obj2, image))
            return false;

          jsonData.images.add(image, IndName(name));
        }
      }
      else {
        return errorMsg("Invalid JSON " + pnv.first + " data");
      }
    }
    else if (pnv.first == "materials") {
      auto readMaterial = [&](CJson::Object *jobj, Material &material) {
        for (const auto &pnv2 : jobj->nameValueMap()) {
          if      (pnv2.first == "name") {
            if (! valueString(pnv2.second, material.name))
              return errorMsg("Invalid materials name string");
          }
          else if (pnv2.first == "normalTexture") {
            if (! pnv2.second->isObject())
              return errorMsg("Invalid " + pnv.first + " normalTexture");

            auto *obj3 = pnv2.second->cast<CJson::Object>();

            for (const auto &pnv3 : obj3->nameValueMap()) {
              if      (pnv3.first == "index") {
                if (! valueLong(pnv3.second, material.normalTexture.index))
                  return errorMsg("Invalid materials normalTexture index number");
              }
              else if (pnv3.first == "texCoord") {
                if (! valueLong(pnv3.second, material.normalTexture.texCoord))
                  return errorMsg("Invalid materials normalTexture texCoord number");
              }
              else if (pnv3.first == "scale") {
                // TODO
                TODOValue("Unhandled materials normalTexture scale", pnv3.second);
              }
              else if (pnv3.first == "extensions") {
                // TODO
                TODOValue("Unhandled materials normalTexture extensions", pnv3.second);
              }
              else {
                warnMsg("Invalid materials normalTexture name " + pnv3.first);
              }
            }
          }
          else if (pnv2.first == "pbrMetallicRoughness") {
            if (! pnv2.second->isObject())
              return errorMsg("Invalid " + pnv.first + " pbrMetallicRoughness");

            auto *obj3 = pnv2.second->cast<CJson::Object>();

            for (const auto &pnv3 : obj3->nameValueMap()) {
              if      (pnv3.first == "metallicFactor") {
                if (! valueNumber(pnv3.second, material.metallicFactor))
                  return errorMsg("Invalid material pbrMetallicRoughness/metallicFactor number");
              }
              else if (pnv3.first == "roughnessFactor") {
                if (! valueNumber(pnv3.second, material.roughnessFactor))
                  return errorMsg("Invalid material pbrMetallicRoughness/roughnessFactor number");
              }
              else if (pnv3.first == "baseColorTexture") {
                if (! pnv3.second->isObject())
                  return errorMsg("Invalid " + pnv.first + " baseColorTexture");

                auto *obj4 = pnv3.second->cast<CJson::Object>();

                for (const auto &pnv4 : obj4->nameValueMap()) {
                  if      (pnv4.first == "index") {
                    if (! valueLong(pnv4.second, material.baseColorTexture.index))
                      return errorMsg("Invalid " + pnv.first + " baseColorTexture index number");
                  }
                  else if (pnv4.first == "texCoord") {
                    if (! valueLong(pnv4.second, material.baseColorTexture.texCoord))
                      return errorMsg("Invalid " + pnv.first + " baseColorTexture texCoord number");
                  }
                  else if (pnv4.first == "extensions") {
                    // TODO
                    TODOValue("Unhandled material baseColorTexture/extensions", pnv4.second);
                  }
                  else {
                    warnMsg("Invalid material baseColorTexture/name " + pnv4.first);
                  }
                }
              }
              else if (pnv3.first == "metallicRoughnessTexture") {
                if (! pnv3.second->isObject())
                  return errorMsg("Invalid " + pnv.first + " metallicRoughnessTexture");

                auto *obj4 = pnv3.second->cast<CJson::Object>();

                for (const auto &pnv4 : obj4->nameValueMap()) {
                  if      (pnv4.first == "index") {
                    if (! valueLong(pnv4.second, material.metallicRoughnessTexture.index))
                      return errorMsg("Invalid " + pnv.first + " " + pnv3.first + " index number");
                  }
                  else if (pnv4.first == "texCoord") {
                    if (! valueLong(pnv4.second, material.metallicRoughnessTexture.texCoord))
                      return errorMsg("Invalid " + pnv.first + " " + pnv3.first + " texCoord number");
                  }
                  else if (pnv4.first == "extensions") {
                    // TODO
                    TODOValue("Unhandled material metallicRoughnessTexture/extensions", pnv4.second);
                  }
                  else {
                    warnMsg("Invalid material metallicRoughnessTexture/name " + pnv4.first);
                  }
                }
              }
              else if (pnv3.first == "baseColorFactor") {
                Color color;
                if (! valueColor(pnv3.second, color))
                  return errorMsg("Invalid material pbrMetallicRoughness/baseColorFactor color");
                TODOValue("Unhandled material pbrMetallicRoughness/baseColorFactor", pnv3.second);
              }
              else {
                TODOValue("Invalid pbrMetallicRoughness name " + pnv3.first, pnv3.second);
              }
            }
          }
          else if (pnv2.first == "technique") {
            if (! valueString(pnv2.second, material.technique))
              return errorMsg("Invalid material technique number");
          }
          else if (pnv2.first == "values") {
            if (! pnv.second->isObject())
              return errorMsg("Invalid material values");

            auto *obj2 = pnv2.second->cast<CJson::Object>();

            for (const auto &pnv3 : obj2->nameValueMap()) {
              auto name = pnv3.first;

              if      (name == "ambient") {
                Color ambient;
                if (! valueColor(pnv3.second, ambient))
                  return errorMsg("Invalid nodes ambient color");
                material.ambient = ambient;
              }
              else if (name == "diffuse") {
                Color diffuse;
                if (! valueColor(pnv3.second, diffuse))
                  return errorMsg("Invalid nodes diffuse color");
                material.diffuse = diffuse;
              }
              else if (name == "emission") {
                Color emission;
                if (! valueColor(pnv3.second, emission))
                  return errorMsg("Invalid nodes emission color");
                material.emission = emission;
              }
              else if (name == "shininess") {
                double shininess;
                if (! valueNumber(pnv3.second, shininess))
                  return errorMsg("Invalid material shininess value");
                material.shininess = shininess;
              }
              else if (name == "specular") {
                Color specular;
                if (! valueColor(pnv3.second, specular))
                  return errorMsg("Invalid nodes specular color");
                material.specular = specular;
              }
              else if (name == "transparency") {
                double transparency;
                if (! valueNumber(pnv3.second, transparency))
                  return errorMsg("Invalid material transparency value");
                material.transparency = transparency;
              }
              else {
                debugMsg(std::string("  material values ") + name +
                         " : " + pnv3.second->typeName());
              }
            }
          }
          else if (pnv2.first == "occlusionTexture") {
            if (! pnv2.second->isObject())
              return errorMsg("Invalid " + pnv.first + " occlusionTexture");

            auto *obj3 = pnv2.second->cast<CJson::Object>();

            for (const auto &pnv3 : obj3->nameValueMap()) {
              if      (pnv3.first == "index") {
                if (! valueLong(pnv3.second, material.occlusionTexture.index))
                  return errorMsg("Invalid materials occlusionTexture index number");
              }
              else if (pnv3.first == "texCoord") {
                if (! valueLong(pnv3.second, material.occlusionTexture.texCoord))
                  return errorMsg("Invalid materials occlusionTexture texCoord number");
              }
              else if (pnv3.first == "scale") {
                // TODO
                TODOValue("Unhandled materials occlusionTexture scale", pnv3.second);
              }
              else if (pnv3.first == "extensions") {
                // TODO
                TODOValue("Unhandled materials occlusionTexture extensions", pnv3.second);
              }
              else {
                warnMsg("Invalid materials occlusionTexture name " + pnv3.first);
              }
            }
          }
          else if (pnv2.first == "extensions") {
            // TODO
            if (! pnv2.second->isObject())
              return errorMsg("Invalid material extensions values");

            auto *obj2 = pnv2.second->cast<CJson::Object>();

            for (const auto &pnv3 : obj2->nameValueMap()) {
              auto name = pnv3.first;

              if (name == "KHR_materials_anisotropy") {
                if (! pnv3.second->isObject())
                  return errorMsg("Invalid material extensions KHR_materials_anisotropy values");

                auto *obj3 = pnv3.second->cast<CJson::Object>();

                for (const auto &pnv4 : obj3->nameValueMap()) {
                  auto name1 = pnv4.first;

                  if      (name1 == "anisotropyStrength") {
                    double anisotropyStrength;
                    if (! valueNumber(pnv4.second, anisotropyStrength))
                      return errorMsg("Invalid material anisotropyStrength value");
                  }
                  else if (name1 == "anisotropyRotation") {
                    double anisotropyRotation;
                    if (! valueNumber(pnv4.second, anisotropyRotation))
                      return errorMsg("Invalid material anisotropyRotation value");
                  }
                  else if (name1 == "anisotropyTexture") {
                    TODO("material extension KHR_materials_anisotropy anisotropyTexture");
                  }
                  else
                    debugMsg(std::string("  material extension KHR_materials_anisotropy ") + name1 +
                             " : " + pnv4.second->typeName());
                }
              }
              else if (name == "KHR_materials_iridescence") {
                TODOValue("Invalid material KHR_materials_iridescence", pnv3.second);
              }
              else if (name == "KHR_materials_transmission") {
                TODOValue("Invalid material KHR_materials_transmission", pnv3.second);
              }
              else if (name == "KHR_materials_volume") {
                TODOValue("Invalid material KHR_materials_volume", pnv3.second);
              }
              else if (name == "KHR_materials_ior") {
                TODOValue("Invalid material KHR_materials_ior", pnv3.second);
              }
              else
                debugMsg(std::string("  material extension ") + name +
                           " : " + pnv3.second->typeName());
            }
          }
          else if (pnv2.first == "alphaCutoff") {
            // TODO
            TODOValue("Invalid material alphaCutoff", pnv2.second);
          }
          else if (pnv2.first == "alphaMode") {
            // TODO
            TODOValue("Invalid material alphaMode", pnv2.second);
          }
          else if (pnv2.first == "doubleSided") {
            if (! valueBool(pnv2.second, material.doubleSided))
              return errorMsg("Invalid material doubleSided value");
          }
          else if (pnv2.first == "emissiveFactor") {
            std::vector<float> rvalues;
            if (! arrayNumbers(pnv2.second, rvalues))
              return errorMsg("Invalid materials emissiveFactor array");

            if (rvalues.size() != 3)
              return errorMsg("Invalid materials emissiveFactor array size");

            material.emissiveFactor = Vec3(rvalues[0], rvalues[1], rvalues[2]);
          }
          else if (pnv2.first == "emissiveTexture") {
            if (! pnv2.second->isObject())
              return errorMsg("Invalid " + pnv.first + " emissiveTexture");

            auto *obj3 = pnv2.second->cast<CJson::Object>();

            for (const auto &pnv3 : obj3->nameValueMap()) {
              if      (pnv3.first == "index") {
                if (! valueLong(pnv3.second, material.emissiveTexture.index))
                  return errorMsg("Invalid materials emissiveTexture index number");
              }
              else if (pnv3.first == "texCoord") {
                if (! valueLong(pnv3.second, material.emissiveTexture.texCoord))
                  return errorMsg("Invalid materials emissiveTexture texCoord number");
              }
              else if (pnv3.first == "scale") {
                // TODO
                TODOValue("Unhandled materials emissiveTexture scale", pnv3.second);
              }
              else if (pnv3.first == "extensions") {
                // TODO
                TODOValue("Unhandled materials emissiveTexture extensions", pnv3.second);
              }
              else {
                warnMsg("Invalid materials emissiveTexture name " + pnv3.first);
              }
            }
          }
          else {
            warnMsg("Invalid materials name " + pnv2.first);
          }
        }

        return true;
      };

      if      (pnv.second->isArray()) {
        auto *arr1 = pnv.second->cast<CJson::Array>();

        for (const auto &pv1 : arr1->values()) {
          if (! pv1->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pv1->cast<CJson::Object>();

          Material material;

          if (! readMaterial(obj2, material))
            return false;

          jsonData.materials.add(material, IndName(-1));
        }
      }
      else if (pnv.second->isObject()) {
        auto *obj1 = pnv.second->cast<CJson::Object>();

        for (const auto &pnv1 : obj1->nameValueMap()) {
          auto name = pnv1.first;

          if (! pnv1.second->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pnv1.second->cast<CJson::Object>();

          Material material;

          if (! readMaterial(obj2, material))
            return false;

          jsonData.materials.add(material, name);
        }
      }
      else {
        return errorMsg("Invalid JSON " + pnv.first + " data");
      }
    }
    else if (pnv.first == "meshes") {
      auto readMesh = [&](CJson::Object *jobj, Mesh &mesh) {
        for (const auto &pnv2 : jobj->nameValueMap()) {
          if      (pnv2.first == "name") {
            if (! valueString(pnv2.second, mesh.name))
              return errorMsg("Invalid name string");
          }
          else if (pnv2.first == "primitives") {
            if (! pnv2.second->isArray())
              return errorMsg("Invalid JSON " + pnv.first + " primitives");

            auto *arr2 = pnv2.second->cast<CJson::Array>();

            Primitive primitive;

            for (const auto &pv2 : arr2->values()) {
              if (! pv2->isObject())
                return errorMsg("Invalid JSON " + pnv.first + " primitives data");

              auto *obj3 = pv2->cast<CJson::Object>();

              for (const auto &pnv3 : obj3->nameValueMap()) {
                if      (pnv3.first == "attributes") {
                  if (! pnv3.second->isObject())
                    return errorMsg("Invalid JSON " + pnv.first + " attributes");

                  auto *obj4 = pnv3.second->cast<CJson::Object>();

                  for (const auto &pnv4 : obj4->nameValueMap()) {
                    if      (pnv4.first == "POSITION") { // VEC3
                      if (! valueIndName(pnv4.second, primitive.position))
                        return errorMsg("Invalid POSITION ind/name");
                    }
                    else if (pnv4.first == "NORMAL") { // VEC3
                      if (! valueIndName(pnv4.second, primitive.normal))
                        return errorMsg("Invalid NORMAL ind/name");
                    }
                    else if (pnv4.first == "TANGENT") { // VEC4
                      // TODO
                      TODOValue("Unhandled primitive TANGENT", pnv4.second);
                    }
                    else if (pnv4.first == "TEXCOORD_0") { // VEC2
                      if (! valueIndName(pnv4.second, primitive.texCoord0))
                        return errorMsg("Invalid TEXCOORD_0 ind/name");
                    }
                    else if (pnv4.first == "TEXCOORD_1") { // VEC2
                      if (! valueIndName(pnv4.second, primitive.texCoord1))
                        return errorMsg("Invalid TEXCOORD_1 ind/name");
                    }
                    else if (pnv4.first == "COLOR_0") { // VEC3 or VEC4
                      // TODO
                      TODOValue("Unhandled primitive COLOR_0", pnv4.second);
                    }
                    else if (pnv4.first == "JOINTS_0") { // VEC4
                      if (! valueIndName(pnv4.second, primitive.joints0))
                        return errorMsg("Invalid JOINTS_0 number");
                    }
                    else if (pnv4.first == "WEIGHTS_0") { // VEC4
                      if (! valueIndName(pnv4.second, primitive.weights0))
                        return errorMsg("Invalid WEIGHTS_0 number");
                    }
                    else {
                      debugMsg("  " + pnv.first + "/" + pnv2.first + "/" + pnv3.first + "/" +
                               pnv4.first + " : " + pnv4.second->typeName());
                    }
                  }
                }
                else if (pnv3.first == "indices") {
                  if (! valueIndName(pnv3.second, primitive.indices))
                    return errorMsg("Invalid indices ind/name");
                }
                else if (pnv3.first == "material") {
                  if (! valueIndName(pnv3.second, primitive.material))
                    return errorMsg("Invalid material ind/name");
                }
                else if (pnv3.first == "mode") {
                  if (! valueLong(pnv3.second, primitive.mode))
                    return errorMsg("Invalid material mode");
                }
                else if (pnv3.first == "targets") {
                  if (! pnv3.second->isArray())
                    return errorMsg("Invalid JSON " + pnv.first + " targets");

                  auto *arr3 = pnv3.second->cast<CJson::Array>();

                  for (const auto &pv3 : arr3->values()) {
                    if (! pv3->isObject())
                      return errorMsg("Invalid JSON " + pnv.first + " targets data");

                    auto *obj4 = pv3->cast<CJson::Object>();

                    for (const auto &pnv4 : obj4->nameValueMap()) {
                      if      (pnv4.first == "POSITION") {
                        IndName position;
                        if (! valueIndName(pnv4.second, position))
                          return errorMsg("Invalid target POSITION ind/name");
                      }
                      else if (pnv4.first == "NORMAL") {
                        IndName normal;
                        if (! valueIndName(pnv4.second, normal))
                          return errorMsg("Invalid target NORMAL ind/name");
                      }
                      else if (pnv4.first == "TANGENT") {
                        IndName tangent;
                        if (! valueIndName(pnv4.second, tangent))
                          return errorMsg("Invalid target NORMAL ind/name");
                      }
                      else {
                        return errorMsg("Invalid targets name " + pnv4.first);
                      }
                    }
                  }
                }
                else {
                  debugMsg("  " + pnv.first + "/" + pnv3.first + " : " + pnv3.second->typeName());
                }
              }

              mesh.primitives.push_back(primitive);
            }
          }
          else if (pnv2.first == "weights") {
            if (! pnv2.second->isArray())
              return errorMsg("Invalid JSON " + pnv.first + " weights");

            auto *arr2 = pnv2.second->cast<CJson::Array>();

            for (const auto &pv2 : arr2->values()) {
              double r;
              if (! valueNumber(pv2, r))
                return errorMsg("Invalid primitives weights value");
            }
          }
          else {
            debugMsg("  " + pnv.first + "/" + pnv2.first + " : " + pnv2.second->typeName());
          }
        }

        return true;
      };

      if      (pnv.second->isArray()) {
        auto *arr1 = pnv.second->cast<CJson::Array>();

        for (const auto &pv1 : arr1->values()) {
          if (! pv1->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pv1->cast<CJson::Object>();

          Mesh mesh;

          if (! readMesh(obj2, mesh))
            return false;

          jsonData.meshes.add(mesh, IndName(-1));
        }
      }
      else if (pnv.second->isObject()) {
        auto *obj1 = pnv.second->cast<CJson::Object>();

        for (const auto &pnv1 : obj1->nameValueMap()) {
          auto name = pnv1.first;

          if (! pnv1.second->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pnv1.second->cast<CJson::Object>();

          Mesh mesh;

          if (! readMesh(obj2, mesh))
            return false;

          jsonData.meshes.add(mesh, name);
        }
      }
      else {
        return errorMsg("Invalid JSON " + pnv.first + " data");
      }
    }
    else if (pnv.first == "nodes") {
      auto readNode = [&](CJson::Object *jobj, Node &node) {
        for      (const auto &pnv2 : jobj->nameValueMap()) {
          if (pnv2.first == "name") {
            if (! valueString(pnv2.second, node.name))
              return errorMsg("Invalid nodes name string");
          }
          else if (pnv2.first == "mesh") {
            if (! valueIndName(pnv2.second, node.mesh))
              return errorMsg("Invalid nodes mesh number");
          }
          else if (pnv2.first == "matrix") {
            std::vector<float> rvalues;
            if (! arrayNumbers(pnv2.second, rvalues))
              return errorMsg("Invalid nodes matrix array");

            if (rvalues.size() != 16)
              return errorMsg("Invalid nodes matrix array size");

            CGLMatrix3D m;
            m.setData(&rvalues[0]);

            node.matrix = m;
          }
          else if (pnv2.first == "children") {
            if (! pnv2.second->isArray())
              return errorMsg("Invalid nodes children array");

            auto *arr2 = pnv2.second->cast<CJson::Array>();

            for (const auto &pv3 : arr2->values()) {
              IndName indName;
              if (! valueIndName(pv3, indName))
                return errorMsg("Invalid nodes child");

              node.children.push_back(indName);
            }
          }
          else if (pnv2.first == "meshes") {
            if (! pnv2.second->isArray())
              return errorMsg("Invalid nodes meshes array");

            auto *arr2 = pnv2.second->cast<CJson::Array>();

            for (const auto &pv3 : arr2->values()) {
              if (! pv3->isString())
                return errorMsg("Invalid nodes meshes string");

              std::string str;
              (void) valueString(pv3, str);
            }

            TODO("Unhandled nodes meshs");
          }
          else if (pnv2.first == "skin") {
            if (! valueIndName(pnv2.second, node.skin))
              return errorMsg("Invalid mesh skin number");
          }
          else if (pnv2.first == "rotation") {
            std::vector<float> rvalues;
            if (! arrayNumbers(pnv2.second, rvalues))
              return errorMsg("Invalid nodes rotation array");

            if (rvalues.size() != 4)
              return errorMsg("Invalid nodes rotation array size");

            node.rotation = Vec4(rvalues[0], rvalues[1], rvalues[2], rvalues[3]);
          }
          else if (pnv2.first == "scale") {
            std::vector<float> rvalues;
            if (! arrayNumbers(pnv2.second, rvalues))
              return errorMsg("Invalid nodes scale array");

            if (rvalues.size() != 3)
              return errorMsg("Invalid nodes scale array size");

            node.scale = Vec3(rvalues[0], rvalues[1], rvalues[2]);
          }
          else if (pnv2.first == "translation") {
            std::vector<float> rvalues;
            if (! arrayNumbers(pnv2.second, rvalues))
              return errorMsg("Invalid nodes translation array");

            if (rvalues.size() != 3)
              return errorMsg("Invalid nodes translation array size");

            node.translation = Vec3(rvalues[0], rvalues[1], rvalues[2]);
          }
          else if (pnv2.first == "camera") {
            if (! valueLong(pnv2.second, node.camera))
              return errorMsg("Invalid camera number");
          }
          else if (pnv2.first == "extensions") {
            TODOValue("Unhandled node extensions", pnv2.second);
          }
          else {
            debugMsg("  " + pnv.first + "/" + pnv2.first + " : " + pnv2.second->typeName());
          }
        }

        return true;
      };

      if      (pnv.second->isArray()) {
        auto *arr1 = pnv.second->cast<CJson::Array>();

        for (const auto &pv1 : arr1->values()) {
          if (! pv1->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pv1->cast<CJson::Object>();

          Node node;
          if (! readNode(obj2, node))
            return false;

          jsonData.nodes.add(node, IndName(-1));
        }
      }
      else if (pnv.second->isObject()) {
        auto *obj1 = pnv.second->cast<CJson::Object>();

        for (const auto &pnv1 : obj1->nameValueMap()) {
          auto name = pnv1.first;

          if (! pnv1.second->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pnv1.second->cast<CJson::Object>();

          Node node;
          if (! readNode(obj2, node))
            return false;

          jsonData.nodes.add(node, name);
        }
      }
      else {
        return errorMsg("Invalid JSON " + pnv.first + " data");
      }
    }
    else if (pnv.first == "samplers") {
      // GL_LINEAR               =  0x2601 (9729)
      // GL_LINEAR_MIPMAP_LINEAR =  0x2703 (9987)
      auto readSampler = [&](CJson::Object *jobj, Sampler &sampler) {
        for (const auto &pnv2 : jobj->nameValueMap()) {
          if      (pnv2.first == "magFilter") {
            if (! valueLong(pnv2.second, sampler.magFilter))
              return errorMsg("Invalid magFilter number");
          }
          else if (pnv2.first == "minFilter") {
            if (! valueLong(pnv2.second, sampler.minFilter))
              return errorMsg("Invalid minFilter number");
          }
          else if (pnv2.first == "wrapS") {
            if (! valueLong(pnv2.second, sampler.wrapS))
              return errorMsg("Invalid wrapS number");
          }
          else if (pnv2.first == "wrapT") {
            if (! valueLong(pnv2.second, sampler.wrapT))
              return errorMsg("Invalid wrapT number");
          }
          else {
            debugMsg("  " + pnv.first + "/" + pnv2.first + " : " + pnv2.second->typeName());
          }
        }

        return true;
      };

      if      (pnv.second->isArray()) {
        auto *arr1 = pnv.second->cast<CJson::Array>();

        for (const auto &pv1 : arr1->values()) {
          if (! pv1->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pv1->cast<CJson::Object>();

          Sampler sampler;

          if (! readSampler(obj2, sampler))
            return false;

          jsonData.samplers.add(sampler, IndName(-1));
        }
      }
      else if (pnv.second->isObject()) {
        auto *obj1 = pnv.second->cast<CJson::Object>();

        for (const auto &pnv1 : obj1->nameValueMap()) {
          auto name = pnv1.first;

          if (! pnv1.second->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pnv1.second->cast<CJson::Object>();

          Sampler sampler;

          if (! readSampler(obj2, sampler))
            return false;

          jsonData.samplers.add(sampler, IndName(name));
        }
      }
      else {
        return errorMsg("Invalid JSON " + pnv.first + " data");
      }
    }
    else if (pnv.first == "scene") {
      if (! valueIndName(pnv.second, jsonData.scene))
        return errorMsg("Invalid scene ind/name");
    }
    else if (pnv.first == "scenes") {
      auto readScene = [&](CJson::Object *jobj, Scene &scene) {
        for (const auto &pnv2 : jobj->nameValueMap()) {
          if      (pnv2.first == "name") {
            if (! valueString(pnv2.second, scene.name))
              return errorMsg("Invalid name string");
          }
          else if (pnv2.first == "nodes") {
            if (! pnv2.second->isArray())
              return errorMsg("Invalid JSON " + pnv.first + " nodes");

            auto *arr2 = pnv2.second->cast<CJson::Array>();

            for (const auto &pv2 : arr2->values()) {
              IndName indName;
              if (! valueIndName(pv2, indName))
                return errorMsg("Invalid scene node");

              scene.nodes.push_back(indName);
            }
          }
          else if (pnv2.first == "extensions") {
            TODOValue("Unhandled scene extensions", pnv2.second);
          }
          else {
            debugMsg("  " + pnv.first + "/" + pnv2.first + " : " + pnv2.second->typeName());
          }
        }

        return true;
      };

      if      (pnv.second->isArray()) {
        auto *arr1 = pnv.second->cast<CJson::Array>();

        for (const auto &pv1 : arr1->values()) {
          if (! pv1->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pv1->cast<CJson::Object>();

          Scene scene;

          if (! readScene(obj2, scene))
            return false;

          jsonData.scenes.add(scene, IndName(-1));
        }
      }
      else if (pnv.second->isObject()) {
        auto *obj1 = pnv.second->cast<CJson::Object>();

        for (const auto &pnv1 : obj1->nameValueMap()) {
          auto name = pnv1.first;

          if (! pnv1.second->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pnv1.second->cast<CJson::Object>();

          Scene scene;

          if (! readScene(obj2, scene))
            return false;

          jsonData.scenes.add(scene, name);
        }
      }
      else {
        return errorMsg("Invalid JSON " + pnv.first + " data");
      }
    }
    else if (pnv.first == "textures") {
      auto readTexture = [&](CJson::Object *jobj, Texture &texture) {
        for (const auto &pnv2 : jobj->nameValueMap()) {
          if      (pnv2.first == "source") {
            if (! valueIndName(pnv2.second, texture.source))
              return errorMsg("Invalid texture source ind/name");
          }
          else if (pnv2.first == "sampler") {
            if (! valueIndName(pnv2.second, texture.sampler))
              return errorMsg("Invalid texture sampler ind/name");
          }
          else if (pnv2.first == "format") {
            if (! valueLong(pnv2.second, texture.format))
              return errorMsg("Invalid texture format");
          }
          else if (pnv2.first == "internalFormat") {
            if (! valueLong(pnv2.second, texture.internalFormat))
              return errorMsg("Invalid texture internalFormat");
          }
          else if (pnv2.first == "target") {
            if (! valueLong(pnv2.second, texture.target))
              return errorMsg("Invalid texture target");
          }
          else if (pnv2.first == "type") {
            if (! valueLong(pnv2.second, texture.type))
              return errorMsg("Invalid texture type");
          }
          else if (pnv2.first == "name") {
            if (! valueString(pnv2.second, texture.name))
              return errorMsg("Invalid texture name");
          }
          else if (pnv2.first == "extensions") {
            if (! pnv2.second->isObject())
              return errorMsg("Invalid textures extensions type");

            auto *obj3 = pnv2.second->cast<CJson::Object>();

            for (const auto &pnv3 : obj3->nameValueMap()) {
              if (pnv3.first == "EXT_texture_webp") {
                if (! pnv3.second->isObject())
                  return errorMsg("Invalid textures extensions EXT_texture_webp");

                auto *obj4 = pnv3.second->cast<CJson::Object>();

                for (const auto &pnv4 : obj4->nameValueMap()) {
                  if (pnv4.first == "source") {
                    if (! valueIndName(pnv4.second, texture.source))
                      return errorMsg("Invalid texture extensions EXT_texture_webp source ind/name");
                  }
                  else {
                    debugMsg("Invalid textures extensions EXT_texture_webp name " +
                             pnv4.first + " : " + pnv4.second->typeName());
                  }
                }
              }
              else {
                debugMsg("Invalid textures extensions name " +
                         pnv3.first + " : " + pnv3.second->typeName());
              }
            }
            debugMsg(std::string("Invalid textures extensions : ") + pnv2.second->typeName());
          }
          else {
            debugMsg("Invalid textures name " + pnv2.first + " : " + pnv2.second->typeName());
          }
        }

        return true;
      };

      if      (pnv.second->isArray()) {
        auto *arr1 = pnv.second->cast<CJson::Array>();

        for (const auto &pv1 : arr1->values()) {
          if (! pv1->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pv1->cast<CJson::Object>();

          Texture texture;

          if (! readTexture(obj2, texture))
            return false;

          jsonData.textures.add(texture, IndName(-1));
        }
      }
      else if (pnv.second->isObject()) {
        auto *obj1 = pnv.second->cast<CJson::Object>();

        for (const auto &pnv1 : obj1->nameValueMap()) {
          auto name = pnv1.first;

          if (! pnv1.second->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pnv1.second->cast<CJson::Object>();

          Texture texture;

          if (! readTexture(obj2, texture))
            return false;

          jsonData.textures.add(texture, name);
        }
      }
      else {
        return errorMsg("Invalid JSON " + pnv.first + " data");
      }
    }
    else if (pnv.first == "animations") {
      if (! pnv.second->isArray())
        return errorMsg("Invalid JSON " + pnv.first + " data");

      auto *arr1 = pnv.second->cast<CJson::Array>();

      for (const auto &pv1 : arr1->values()) {
        Animation animation;

        if (! pv1->isObject())
          return errorMsg("Invalid JSON " + pnv.first + " array data");

        auto *obj2 = pv1->cast<CJson::Object>();

        for (const auto &pnv2 : obj2->nameValueMap()) {
          auto name = pnv2.first;

          if      (name == "name") {
            if (! valueString(pnv2.second, animation.name))
              return errorMsg("Invalid animations name");
          }
          else if (name == "channels") {
            if (! pnv2.second->isArray())
              return errorMsg("Invalid JSON " + pnv.first + " " + name + " data");

            auto *arr3 = pnv2.second->cast<CJson::Array>();

            Animation::Channels channels;

            for (const auto &pv3 : arr3->values()) {
              AnimationChannel channel;

              if (! pv3->isObject())
                return errorMsg("Invalid JSON " + pnv.first + " " + name + " array data");

              auto *obj3 = pv3->cast<CJson::Object>();

              for (const auto &pnv3 : obj3->nameValueMap()) {
                auto name1 = pnv3.first;

                if      (name1 == "target") {
                  if (! pnv3.second->isObject())
                    return errorMsg("Invalid JSON " + pnv.first + " " + name +
                                    " " + name1 + " data");

                  auto *obj4 = pnv3.second->cast<CJson::Object>();

                  for (const auto &pnv4 : obj4->nameValueMap()) {
                    auto name2 = pnv4.first;

                    if      (name2 == "node") {
                      if (! valueIndName(pnv4.second, channel.node))
                        return errorMsg("Invalid channel node value");
                    }
                    else if (name2 == "path") {
                      if (! valueString(pnv4.second, channel.path))
                        return errorMsg("Invalid channel path value");
                    }
                    else if (name2 == "extensions") {
                      TODOValue("Unhandled target extensions", pnv4.second);
                    }
                    else
                      TODOValue("Unhandled animations channels target name " + name2,
                                pnv4.second);
                  }
                }
                else if (name1 == "sampler") {
                  if (! valueLong(pnv3.second, channel.sampler))
                    return errorMsg("Invalid channel sampler value");
                }
                else
                  TODOValue("Unhandled animations channels name " + name1, pnv3.second);
              }

              channels.push_back(channel);
            }

            animation.channelsArray.push_back(channels);
          }
          else if (name == "samplers") {
            if (! pnv2.second->isArray())
              return errorMsg("Invalid JSON " + pnv.first + " " + name + " data");

            auto *arr3 = pnv2.second->cast<CJson::Array>();

            for (const auto &pv3 : arr3->values()) {
              AnimationSampler sampler;

              if (! pv3->isObject())
                return errorMsg("Invalid JSON " + pnv.first + " " + name + " array data");

              auto *obj3 = pv3->cast<CJson::Object>();

              for (const auto &pnv3 : obj3->nameValueMap()) {
                auto name1 = pnv3.first;

                if      (name1 == "input") {
                  if (! valueLong(pnv3.second, sampler.input))
                    return errorMsg("Invalid sampler input value");
                }
                else if (name1 == "output") {
                  if (! valueLong(pnv3.second, sampler.output))
                    return errorMsg("Invalid sampler output value");
                }
                else if (name1 == "interpolation") {
                  if (! valueString(pnv3.second, sampler.interpolation))
                    return errorMsg("Invalid sampler interpolation value");

                  if      (sampler.interpolation == "LINEAR")
                    sampler.interpType = AnimationInterpolation::LINEAR;
                  else if (sampler.interpolation == "STEP")
                    sampler.interpType = AnimationInterpolation::STEP;
                  else if (sampler.interpolation == "CUBICSPLINE")
                    sampler.interpType = AnimationInterpolation::CUBICSPLINE;
                  else
                    return errorMsg("Invalid sampler interpolation " + sampler.interpolation);
                }
                else
                  TODOValue("Unhandled animations samplers name " + name1, pnv3.second);
              }

              animation.samplers.add(sampler, IndName(-1));
            }
          }
          else
            TODOValue("Unhandled animations name " + name, pnv2.second);
        }

        if (animation.name == "")
          animation.name = "anim." + std::to_string(jsonData.animations.size() + 1);

        jsonData.animations.push_back(animation);
      }
    }
    else if (pnv.first == "cameras") {
      if      (pnv.second->isArray()) {
        auto *arr1 = pnv.second->cast<CJson::Array>();

        int ind = 0;

        for (const auto &pv1 : arr1->values()) {
          if (! pv1->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          Camera camera;

          auto *obj2 = pv1->cast<CJson::Object>();

          for (const auto &pnv2 : obj2->nameValueMap()) {
            auto name = pnv2.first;

            if      (name == "name") {
              if (! valueString(pnv2.second, camera.name))
                return errorMsg("Invalid camera name");
            }
            else if (name == "type") {
              if (! valueString(pnv2.second, camera.type))
                return errorMsg("Invalid camera type");
            }
            else if (name == "perspective") {
              if (! pnv2.second->isObject())
                return errorMsg("Invalid camera perspective");

              auto *obj3 = pnv2.second->cast<CJson::Object>();

              for (const auto &pnv3 : obj3->nameValueMap()) {
                auto name1 = pnv3.first;

                if      (name1 == "yfov") {
                  if (! valueNumber(pnv3.second, camera.yfov))
                    return errorMsg("Invalid camera yfov");
                }
                else if (name1 == "znear") {
                  if (! valueNumber(pnv3.second, camera.znear))
                    return errorMsg("Invalid camera znear");
                }
                else if (name1 == "zfar") {
                  if (! valueNumber(pnv3.second, camera.zfar))
                    return errorMsg("Invalid camera zfar");
                }
                else
                  TODOValue("Unhandled camera perspective " + name1, pnv3.second);
              }
            }
            else
              TODOValue("Unhandled camera name " + name, pnv2.second);
          }

          jsonData.cameras.add(camera, IndName(ind++));
        }
      }
      else if (pnv.second->isObject()) {
        return errorMsg("Invalid JSON " + pnv.first + " array data");
      }
      else
        return errorMsg("Invalid JSON " + pnv.first + " data");
    }
    else if (pnv.first == "extensionsUsed") {
      if (pnv.second->isArray()) {
        auto *arr1 = pnv.second->cast<CJson::Array>();

        for (const auto &pv1 : arr1->values()) {
          if (! pv1->isString())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          std::string name;
          if (! valueString(pv1, name))
            return errorMsg("Invalid extensionsUsed name");

          jsonData.extensionsUsed.push_back(name);
        }
      }
      else
        return errorMsg("Invalid JSON " + pnv.first + " array data");
    }
    else if (pnv.first == "programs") {
      // TODO
      TODOValue("Unhandled programs", pnv.second);
    }
    else if (pnv.first == "shaders") {
      // TODO
      TODOValue("Unhandled shaders", pnv.second);
    }
    else if (pnv.first == "skins") {
      if (! pnv.second->isArray())
        return errorMsg("Invalid JSON " + pnv.first + " array data");

      auto *arr1 = pnv.second->cast<CJson::Array>();

      int ind = 0;

      for (const auto &pv1 : arr1->values()) {
        Skin skin;

        if (! pv1->isObject())
          return errorMsg("Invalid JSON " + pnv.first + " array data");

        auto *obj2 = pv1->cast<CJson::Object>();

        for (const auto &pnv2 : obj2->nameValueMap()) {
          auto name = pnv2.first;

          if      (name == "inverseBindMatrices") {
            if (! valueLong(pnv2.second, skin.inverseBindMatrices))
              return errorMsg("Invalid sampler output value");
          }
          else if (name == "joints") {
            if (! pnv2.second->isArray())
              return errorMsg("Invalid JSON " + pnv.first + " " + pnv2.first + " array data");

            auto *arr3 = pnv2.second->cast<CJson::Array>();

            for (const auto &pv3 : arr3->values()) {
              long ijoint;
              if (! valueLong(pv3, ijoint))
                return errorMsg("Invalid " + pnv.first + " " + pnv2.first + " array value");

              skin.joints.push_back(ijoint);
            }
          }
          else if (name == "name") {
            if (! valueString(pnv2.second, skin.name))
              return errorMsg("Invalid " + pnv.first + " " + pnv2.first + " name");
          }
          else if (name == "skeleton") {
            long iroot;
            if (! valueLong(pnv2.second, iroot))
              return errorMsg("Invalid " + pnv.first + " " + pnv2.first + " skeleton");
          }
          else
            debugMsg(pnv2.first + " : " + pnv2.second->typeName());
        }

        jsonData.skins.add(skin, IndName(ind++));
      }
    }
    else if (pnv.first == "techniques") {
      // TODO
      TODO("Unhandled techniques");
      if (pnv.second->isObject()) {
        auto *obj1 = pnv.second->cast<CJson::Object>();

        for (const auto &pnv1 : obj1->nameValueMap()) {
          debugMsg(pnv1.first + " : " + pnv1.second->typeName());
        }
      }
      else if (pnv.second->isArray()) {
      }
      else
        return errorMsg("Invalid JSON " + pnv.first + " array data");
    }
    else if (pnv.first == "extensions") {
      TODOValue("Unhandled extensions", pnv.second);
    }
    else if (pnv.first == "extensionsRequired") {
      TODOValue("Unhandled extensionsRequired", pnv.second);
    }
    else if (pnv.first == "extensionsUsed") {
      TODOValue("Unhandled extensionsUsed", pnv.second);
    }
    else {
      debugMsg(pnv.first + " : " + pnv.second->typeName());

      return errorMsg("Unhandled name : " + pnv.first);
    }
  }

  //---

  if (isDebug())
    printJsonData(jsonData);

  return true;
}

void
CImportGLTF::
printJsonData(const JsonData &jsonData) const
{
  if (! jsonData.accessors.isEmpty()) {
    std::cerr << "accessors\n";

    for (const auto &pa : jsonData.accessors)
      printAccessor(pa.second, pa.first);
  }

  //---

  if (jsonData.asset.set) {
    std::cerr << "asset\n";
    std::cerr << "  generator : " << jsonData.asset.generator << "\n";

    if (jsonData.asset.versionId != "")
      std::cerr << "  version : " << jsonData.asset.versionId << "\n";
    else
      std::cerr << "  version : " << jsonData.asset.versionNumber << "\n";
  }

  //---

  if (! jsonData.bufferViews.isEmpty()) {
    std::cerr << "bufferViews\n";

    for (const auto &pb : jsonData.bufferViews)
      printBufferView(pb.second, pb.first);
  }

  //---

  if (! jsonData.buffers.isEmpty()) {
    std::cerr << "buffers\n";

    for (const auto &pb : jsonData.buffers)
      printBuffer(pb.second, pb.first);
  }

  //---

  if (! jsonData.images.isEmpty()) {
    std::cerr << "images\n";

    for (const auto &pi : jsonData.images)
      printImage(pi.second, pi.first);
  }

  //---

  if (! jsonData.materials.isEmpty()) {
    std::cerr << "materials\n";

    for (const auto &pm : jsonData.materials)
      printMaterial(pm.second, pm.first);
  }

  //---

  if (! jsonData.meshes.isEmpty()) {
    std::cerr << "meshes\n";

    for (const auto &pm : jsonData.meshes)
      printMesh(pm.second, pm.first);
  }

  //---

  if (! jsonData.nodes.isEmpty()) {
    std::cerr << "nodes\n";

    for (const auto &pn : jsonData.nodes)
      printNode(pn.second, pn.first);
  }

  if (! jsonData.samplers.isEmpty()) {
    std::cerr << "samplers\n";

    for (const auto &ps : jsonData.samplers)
      printSampler(ps.second, IndName(ps.first));
  }

  //---

  std::cerr << "scene: " << jsonData.scene << "\n";

  //---

  if (! jsonData.scenes.isEmpty()) {
    std::cerr << "scenes\n";

    for (const auto &ps : jsonData.scenes)
      printScene(ps.second, ps.first);
  }

  //---

  if (! jsonData.textures.isEmpty()) {
    std::cerr << "textures\n";

    for (const auto &pt : jsonData.textures)
      printTexture(pt.second, pt.first);
  }
}

void
CImportGLTF::
printMeshData(const MeshData &meshData) const
{
  if (! meshData.min.empty()) {
    std::cerr << "Min:";
    for (const auto &v : meshData.min) {
      std::cerr << " " << v;
    }
    std::cerr << ", ";
  }

  if (! meshData.max.empty()) {
    std::cerr << "Max:";
    for (const auto &v : meshData.max) {
      std::cerr << " " << v;
    }
    std::cerr << ", ";
  }

  if      (meshData.vec4.size() > 0) {
    std::cerr << "VEC4:";
    for (const auto &v : meshData.vec4) {
      std::cerr << " {" << v.x << "," << v.y << "," << v.z << "," << v.w << "}";
    }
  }
  else if (meshData.vec3.size() > 0) {
    std::cerr << "VEC3:";
    for (const auto &v : meshData.vec3) {
      std::cerr << " {" << v.x << "," << v.y << "," << v.z << "}";
    }
  }
  else if (meshData.vec2.size() > 0) {
    std::cerr << "VEC2:";
    for (const auto &v : meshData.vec2) {
      std::cerr << " {" << v.x << "," << v.y << "}";
    }
  }
  else if (meshData.iscalars.size() > 0) {
    std::cerr << "SCALAR (I):";
    for (const auto &s : meshData.iscalars) {
      std::cerr << " " << s;
    }
  }
  else if (meshData.fscalars.size() > 0) {
    std::cerr << "SCALAR (F):";
    for (const auto &s : meshData.fscalars) {
      std::cerr << " " << s;
    }
  }
  else if (meshData.mat4.size() > 0) {
    std::cerr << "MAT4:";
    for (const auto &s : meshData.mat4) {
      std::cerr << " " << s;
    }
  }
  else {
    std::cerr << "Unhandled type";
  }

  std::cerr << "\n";
}

void
CImportGLTF::
printAccessor(const Accessor &accessor, const IndName &indName) const
{
  auto componentTypeToString = [](const long &t) {
    switch (t) {
      case TYPE_SIGNED_BYTE   : return std::string("SIGNED_BYTE");
      case TYPE_UNSIGNED_BYTE : return std::string("UNSIGNED_BYTE");
      case TYPE_SIGNED_SHORT  : return std::string("SIGNED_SHORT");
      case TYPE_UNSIGNED_SHORT: return std::string("UNSIGNED_SHORT");
  //  case TYPE_SIGNED_INT    : return std::string("SIGNED_INT");
      case TYPE_UNSIGNED_INT  : return std::string("UNSIGNED_INT");
      case TYPE_FLOAT         : return std::string("FLOAT");
      default                 : return std::string("? (") + std::to_string(t) + std::string(")");
    }
  };

  std::cerr << " accessor[" << indName << "]\n";

  std::cerr << "  bufferView: " << accessor.bufferView << "\n";
  if (accessor.byteOffset >= 0)
    std::cerr << "  byteOffset: " << accessor.byteOffset << "\n";
  if (accessor.byteStride >= 0)
    std::cerr << "  byteStride: " << accessor.byteStride << "\n";
  if (accessor.type != "")
    std::cerr << "  type: " << accessor.type << "\n";
  std::cerr << "  componentType: " << componentTypeToString(accessor.componentType) << "\n";
  std::cerr << "  count: " << accessor.count << "\n";

  if (! accessor.min.empty()) {
    std::cerr << "  min:";
    for (auto r : accessor.min)
      std::cerr << " " << r;
    std::cerr << "\n";
  }

  if (! accessor.max.empty()) {
    std::cerr << "  max:";
    for (auto r : accessor.max)
      std::cerr << " " << r;
    std::cerr << "\n";
  }
}

void
CImportGLTF::
printSampler(const Sampler &sampler, const IndName &indName) const
{
  std::cerr << " sampler[" << indName << "]\n";

  std::cerr << "  magFilter: " << sampler.magFilter << "\n";
  std::cerr << "  minFilter: " << sampler.minFilter << "\n";
  std::cerr << "  wrapS: " << sampler.wrapS << "\n";
  std::cerr << "  wrapT: " << sampler.wrapT << "\n";
}

void
CImportGLTF::
printScene(const Scene &scene, const IndName &indName) const
{
  std::cerr << " scene[" << indName << "]\n";

  if (scene.name != "")
    std::cerr << "  name: " << scene.name << "\n";

  std::cerr << "  nodes:";
  for (const auto &node : scene.nodes)
    std::cerr << " " << node;
  std::cerr << "\n";
}

void
CImportGLTF::
printTexture(const Texture &texture, const IndName &indName) const
{
  std::cerr << " texture[" << indName << "]\n";

  std::cerr << "  source: " << texture.source << "\n";
  std::cerr << "  sampler: " << texture.sampler << "\n";
  if (texture.format >= 0)
    std::cerr << "  format: " << texture.format << "\n";
  if (texture.internalFormat >= 0)
    std::cerr << "  internalFormat: " << texture.internalFormat << "\n";
  if (texture.target >= 0)
    std::cerr << "  target: " << texture.target << "\n";
  if (texture.type >= 0)
    std::cerr << "  type: " << texture.type << "\n";
  if (texture.name != "")
    std::cerr << "  name: " << texture.name << "\n";
}

void
CImportGLTF::
printBufferView(const BufferView &bufferView, const IndName &indName) const
{
  std::cerr << " bufferView[" << indName << "]\n";

  std::cerr << "  buffer: " << bufferView.buffer << "\n";
  if (bufferView.byteOffset >= 0)
    std::cerr << "  byteOffset: " << bufferView.byteOffset << "\n";
  std::cerr << "  byteLength: " << bufferView.byteLength << "\n";
  if (bufferView.byteStride >= 0)
    std::cerr << "  byteStride: " << bufferView.byteStride << "\n";
  if (bufferView.target >= 0)
    std::cerr << "  target: " << bufferView.target << "\n";
}

void
CImportGLTF::
printBuffer(const Buffer &buffer, const IndName &indName) const
{
  std::cerr << " buffer[" << indName << "]\n";

  std::cerr << "  byteLength: " << buffer.byteLength << "\n";
  std::cerr << "  uri: " << buffer.uri << "\n";
  std::cerr << "  type: " << buffer.type << "\n";
};

void
CImportGLTF::
printImage(const Image &image, const IndName &indName) const
{
  std::cerr << " image[" << indName << "]\n";

  std::cerr << "  bufferView: " << image.bufferView << "\n";
  std::cerr << "  mimeType: " << image.mimeType << "\n";
  std::cerr << "  name: " << image.name << "\n";
  if (image.url != "")
    std::cerr << "  url: " << image.url << "\n";
  if (image.uri != "")
    std::cerr << "  uri: " << image.uri << "\n";
  if (image.width >= 0)
    std::cerr << "  width: " << image.width << "\n";
  if (image.height >= 0)
    std::cerr << "  height: " << image.height << "\n";
}

void
CImportGLTF::
printMaterial(const Material &material, const IndName &indName) const
{
  std::cerr << " material[" << indName << "]\n";

  std::cerr << "  name: " << material.name << "\n";
  std::cerr << "  metallicFactor: " << material.metallicFactor << "\n";
  std::cerr << "  roughnessFactor: " << material.roughnessFactor << "\n";
  std::cerr << "  normalTexture\n";
  if (material.normalTexture.index >= 0)
    std::cerr << "   index: " << material.normalTexture.index << "\n";
  if (material.normalTexture.texCoord >= 0)
    std::cerr << "   texCoord: " << material.normalTexture.texCoord << "\n";
  std::cerr << "  emissiveTexture\n";
  if (material.emissiveTexture.index >= 0)
    std::cerr << "   index: " << material.emissiveTexture.index << "\n";
  if (material.emissiveTexture.texCoord >= 0)
    std::cerr << "   texCoord: " << material.emissiveTexture.texCoord << "\n";
  std::cerr << "  baseColorTexture\n";
  if (material.baseColorTexture.index >= 0)
    std::cerr << "   index: " << material.baseColorTexture.index << "\n";
  if (material.baseColorTexture.texCoord >= 0)
    std::cerr << "   texCoord: " << material.baseColorTexture.texCoord << "\n";
  std::cerr << "  technique: " << material.technique << "\n";
  if (material.ambient)
    std::cerr << "  ambient: " << material.ambient.value() << "\n";
  if (material.diffuse)
    std::cerr << "  diffuse: " << material.diffuse.value() << "\n";
  if (material.emission)
    std::cerr << "  emission: " << material.emission.value() << "\n";
  if (material.specular)
    std::cerr << "  specular: " << material.specular.value() << "\n";
  if (material.shininess)
    std::cerr << "  shininess: " << material.shininess.value() << "\n";
  if (material.transparency)
    std::cerr << "  transparency: " << material.transparency.value() << "\n";
}

void
CImportGLTF::
printMesh(const Mesh &mesh, const IndName &indName) const
{
  std::cerr << " mesh[" << indName << "]\n";

  std::cerr << "  name: " << mesh.name << "\n";

  if (! mesh.primitives.empty()) {
    std::cerr << "  primitives\n";

    int ia1 = 0;

    for (const auto &primitive : mesh.primitives)
      printPrimitive(primitive, ia1++);
  }
}

void
CImportGLTF::
printNode(const Node &node, const IndName &indName) const
{
  std::cerr << " node[" << indName << "]\n";

  if (! node.mesh.isEmpty())
    std::cerr << "  mesh: " << node.mesh << "\n";
  if (node.name != "")
    std::cerr << "  name: " << node.name << "\n";
  if (node.matrix)
    std::cerr << "  matrix: " << *node.matrix << "\n";
  if (node.rotation)
    std::cerr << "  rotation: " << *node.rotation << "\n";
  if (node.scale)
    std::cerr << "  scale: " << *node.scale << "\n";
  if (node.translation)
    std::cerr << "  translation: " << *node.translation << "\n";
}

void
CImportGLTF::
printPrimitive(const Primitive &primitive, int ia) const
{
  std::cerr << "   primitive[" << ia << "]\n";
  if (primitive.mode >= 0)
    std::cerr << "    mode: " << primitive.mode << "\n";
  if (! primitive.indices.isEmpty())
    std::cerr << "    indices: " << primitive.indices << "\n";
  if (! primitive.normal.isEmpty())
    std::cerr << "    NORMAL: " << primitive.normal << "\n";
  if (! primitive.position.isEmpty())
    std::cerr << "    POSITION: " << primitive.position << "\n";
  if (! primitive.texCoord0.isEmpty())
    std::cerr << "    TEXCOORD_0: " << primitive.texCoord0 << "\n";
  if (! primitive.texCoord1.isEmpty())
    std::cerr << "    TEXCOORD_1: " << primitive.texCoord1 << "\n";
  if (! primitive.material.isEmpty())
    std::cerr << "    material: " << primitive.material << "\n";
}
