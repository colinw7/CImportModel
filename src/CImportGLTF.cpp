#include <CImportGLTF.h>
#include <CGeometry3D.h>
#include <CGeomNodeData.h>
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
  std::cerr << "[31mDBG[0m: " << msg << "\n";
}

auto debugValueMsg(const std::string &msg, const CJson::ValueP &value) -> void {
  std::cerr << "[31mDBG[0m: " << msg << " : " + std::string(value->typeName()) << "\n";
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

CRGBA vectorRGBA(const CImportGLTF::Vec3 &c) {
  return CRGBA(c.x, c.y, c.z);
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
    scene_  = CGeometry3DInst->createScene3D();
    pscene_ = SceneP(scene_);
  }

  auto name1 = name;

  if (name1 == "")
    name1 = "gltf";

  object_  = CGeometry3DInst->createObject3D(scene_, name1);
  pobject_ = ObjectP(object_);

  scene_->addObject(object_);

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

    bool rc;

    if (accessor.sparse)
      rc = processSparseAccessor(pa.first, accessor);
    else
      rc = processAccessor(pa.first, accessor);

    if (! rc)
      std::cerr << "process Accessor failed\n";
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
      rootObject_->setNodeLocalTransforms(nodeInd, node.mtranslate, node.mrotate, node.mscale);
    //rootObject_->setNodeLocalTransform(nodeInd, node.transform);

      rootObject_->setNodeGlobalTransform(nodeInd, node.hierTransform);
//    rootObject_->setNodeGlobalTransform(nodeInd, calcNodeGlobalTransform(&node));

#if 0
      if (node.object) {
        auto transform = node.mtranslate.matrix()*node.mrotate.matrix()*node.mscale.matrix();

        node.object->setLocalTransform (transform);
        node.object->setGlobalTransform(node.hierTransform);
      }
#endif
    }
    else
      std::cerr << "No node for " << nodeInd << "\n";
  }

  //---

  //updateHier();

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
    if      (meshData.componentType == Constants::TYPE_UNSIGNED_BYTE) {
      if (byteStride <= 0)
        byteStride = 1;

      if (byteLength < meshData.count*byteStride)
        return errorMsg("Invalid " + meshData.type + " count");

      int id1 = 0;

      for (long i = 0; i < meshData.count; ++i) {
        auto l = readByte(data, id1); id1 += byteStride;

        meshData.iscalars.push_back(l);
      }
    }
    else if (meshData.componentType == Constants::TYPE_UNSIGNED_SHORT) {
      if (byteStride <= 0)
        byteStride = 2;

      if (byteLength < meshData.count*byteStride)
        return errorMsg("Invalid " + meshData.type + " count");

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
        return errorMsg("Invalid " + meshData.type + " count");

      int id1 = 0;

      for (long i = 0; i < meshData.count; ++i) {
        auto l = readInt(data, id1); id1 += byteStride;

        meshData.iscalars.push_back(l);
      }
    }
    else if (meshData.componentType == Constants::TYPE_FLOAT) {
      if (byteStride <= 0)
        byteStride = 4;

      if (byteLength < meshData.count*byteStride)
        return errorMsg("Invalid " + meshData.type + " count");

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

      return errorMsg("Invalid " + meshData.type + " component type " +
                      std::to_string(meshData.componentType));
    }
  }
  // 2 components
  else if (meshData.type == "VEC2") {
    int dim = 2;

    if (meshData.componentType == Constants::TYPE_FLOAT) {
      if (byteStride <= 0)
        byteStride = dim*4;

      if (byteLength < meshData.count*byteStride)
        return errorMsg("Invalid " + meshData.type + " count");

      int id1 = 0;

      for (long i = 0; i < meshData.count; ++i) {
        Vec2 v;

        v.x = readFloat(data, id1    );
        v.y = readFloat(data, id1 + 4); id1 += byteStride;

        meshData.vec2.push_back(v);
      }
    }
    else
      return errorMsg("Invalid " + meshData.type + " component type " +
                      std::to_string(meshData.componentType));
  }
  // 3 components
  else if (meshData.type == "VEC3") {
    int dim = 3;

    if (meshData.componentType == Constants::TYPE_FLOAT) {
      if (byteStride <= 0)
        byteStride = dim*4;

      if (byteLength < meshData.count*byteStride)
        return errorMsg("Invalid " + meshData.type + " count");

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
      return errorMsg("Invalid " + meshData.type + " component type " +
                      std::to_string(meshData.componentType));
  }
  // 4 components
  else if (meshData.type == "VEC4") {
    int dim = 4;

    if      (meshData.componentType == Constants::TYPE_UNSIGNED_BYTE) {
      if (byteStride <= 0)
        byteStride = dim*1;

      if (byteLength < meshData.count*byteStride)
        return errorMsg("Invalid " + meshData.type + " count");

      int id1 = 0;

      for (long i = 0; i < meshData.count; ++i) {
        Vec4 v;

        v.x = readByte(data, id1    );
        v.y = readByte(data, id1 + 1);
        v.z = readByte(data, id1 + 2);
        v.w = readByte(data, id1 + 3); id1 += byteStride;

        meshData.vec4.push_back(v);
      }
    }
    else if (meshData.componentType == Constants::TYPE_UNSIGNED_SHORT) {
      if (byteStride <= 0)
        byteStride = dim*2;

      if (byteLength < meshData.count*byteStride)
        return errorMsg("Invalid " + meshData.type + " count");

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
        byteStride = dim*4;

      if (byteLength < meshData.count*byteStride)
        return errorMsg("Invalid " + meshData.type + " count");

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
    else
      return errorMsg("Invalid " + meshData.type + " component type " +
                      std::to_string(meshData.componentType));
  }
  // 4 componenents
  else if (meshData.type == "MAT2") {
    // TODO: 4 values
    TODO("Unhandled " + meshData.type + " meshData type");
  }
  // 9 componenents
  else if (meshData.type == "MAT3") {
    // TODO:  9 values
    TODO("Unhandled " + meshData.type + " meshData type");
  }
  // 16 componenents
  else if (meshData.type == "MAT4") {
    int dim = 16;

    if      (meshData.componentType == Constants::TYPE_FLOAT) {
      if (byteStride <= 0)
        byteStride = dim*4;

      if (byteLength < meshData.count*byteStride)
        return errorMsg("Invalid " + meshData.type + " count");

      int id1 = 0;

      for (long i = 0; i < meshData.count; ++i) {
        Mat4 m;

        for (int j = 0; j < dim; ++j)
          m.v[j] = readFloat(data, id1 + j*4);

        m.calcMatrix();

        meshData.mat4.push_back(m);

        id1 += byteStride;
      }
    }
    else
      TODO("Unhandled " + meshData.type + " meshData type");
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
  int is = 0;

  for (const auto &ps : jsonData_.skins) {
    const auto &skin = ps.second;

    auto *meshData = getMeshData(skin.inverseBindMatrices);
    if (! meshData) continue;

    if (isDump()) {
      std::cerr << " Skin Inverse Bind " << is << ": ";
      printMeshData(*meshData);
    }

    size_t im = 0;
    auto   nm = meshData->mat4.size();

    for (const auto &joint : skin.joints) {
      Node *node;
      if (! getNode(joint, node))
        continue;

      if (im >= nm)
        continue;

      node->isJoint           = true;
      node->order             = int(im);
      node->inverseBindMatrix = meshData->mat4[im];

      ++im;
    }

    ++is;
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

long
CImportGLTF::
mapSkinNode(const Skin &skin, int node) const
{
  assert(node >= 0 && node < int(skin.joints.size()));

  return skin.joints[node];
}

void
CImportGLTF::
updateHier()
{
  for (const auto &ps : jsonData_.skins) {
    const auto &skin = ps.second;

    for (const auto &joint : skin.joints) {
      Node *node;
      if (! getNode(joint, node))
        continue;

      if (! node->object)
        createNodeObj(node);

      addNodeChildren(node);
    }
  }
}

void
CImportGLTF::
addNodeChildren(Node *node)
{
  auto *object = (node ? node->object : nullptr);
  if (! object) return;

  object->resetChildren();

  for (const auto &pc : node->children) {
    Node *childNode;
    if (! getNode(pc, childNode))
      continue;

    if (! childNode->object)
      createNodeObj(childNode);

    object->addChild(childNode->object);

    //---

    addNodeChildren(childNode);
  }
}

bool
CImportGLTF::
processAnim()
{
  enum class ChannelType {
    NONE,
    TRANSLATION,
    ROTATION,
    SCALE,
    WEIGHTS,
  };

  for (const auto &animation : jsonData_.animations) {
    if (isDebug())
      debugMsg("Animation: " + animation.name);

    for (const auto &channels : animation.channelsArray) {
      if (isDebug())
        debugMsg(" Channels");

      for (const auto &channel : channels) {
        ChannelType channelType { ChannelType::NONE };

        if      (channel.path == "translation")
          channelType = ChannelType::TRANSLATION;
        else if (channel.path == "rotation")
          channelType = ChannelType::ROTATION;
        else if (channel.path == "scale")
          channelType = ChannelType::SCALE;
        else if (channel.path == "weights")
          channelType = ChannelType::WEIGHTS;

        //---

        CGeomAnimationData animationData;

        if (isDebug())
          debugMsg("  Channel: Sampler=" + std::to_string(channel.sampler) + " Node=" +
                   channel.node.to_string() + " Path=" + channel.path);

        //---

        Node *node { nullptr };

        if (! channel.node.isEmpty()) {
          if (! getNode(channel.node, node)) {
            errorMsg("   Node: <invalid>"); continue;
          }
        }
        else {
          errorMsg("   No node in channel");
          continue;
        }

        auto ps = animation.samplers.find(channel.sampler);
        if (ps == animation.samplers.end()) {
          errorMsg("   Sampler: <invalid>"); continue;
        }

        const auto &sampler = (*ps).second;

        Accessor iaccessor;
        if (! getAccessor(sampler.input, iaccessor)) {
          errorMsg("    Input Accessor: <invalid>"); continue;
        }

        auto *imeshData = getMeshData(sampler.input);
        if (! imeshData) {
          errorMsg("Invalid Mesh Data for sampler.input"); continue;
        }

        Accessor oaccessor;
        if (! getAccessor(sampler.output, oaccessor)) {
          errorMsg("    Output Accessor: <invalid>"); continue;
        }

        auto *omeshData = getMeshData(sampler.output);
        if (! omeshData) {
          errorMsg("Invalid Mesh Data for sampler.output"); continue;
        }

        //---

        if (isDebug()) {
          debugMsg("   Sampler: Input=" + std::to_string(sampler.input) + " Output=" +
                   std::to_string(sampler.output) + " Interpolation=" + sampler.interpolation);

          if (isDump()) {
            std::cerr << "    Input Accessor: ";
            printMeshData(*imeshData);
          }

          if (isDump()) {
            std::cerr << "    Output Accessor: ";
            printMeshData(*omeshData);
          }
        }

        if (imeshData->type != "SCALAR" || imeshData->componentType != Constants::TYPE_FLOAT ||
            imeshData->min.size() != 1 || imeshData->max.size() != 1) {
          errorMsg("Invalid Type for sampler.input (" + imeshData->type + " " +
                   std::to_string(imeshData->componentType) + ")"); continue;
        }

        auto nodeInd = int(node->indName.ind);

        auto ni = imeshData->fscalars.size();

        if      (channelType == ChannelType::TRANSLATION) {
          for (const auto &f : imeshData->fscalars)
            animationData.addTranslationRangeValue(f);

          if (! imeshData->min.empty())
            animationData.setTranslationRangeMin(imeshData->min[0]);

          if (! imeshData->max.empty())
            animationData.setTranslationRangeMax(imeshData->max[0]);

          //---

          animationData.setTranslationInterpolation(sampler.interpType);

          //---

          if (omeshData->type != "VEC3") {
            errorMsg("Invalid Type for animation '" + animation.name + "' sampler " +
                     std::to_string(channel.sampler) + " translation");
            continue;
          }

          if (ni != omeshData->vec3.size()) {
            errorMsg("Invalid Size for animation '" + animation.name + "' sampler " +
                     std::to_string(channel.sampler) + " translation " +
                     "(Expected " + std::to_string(ni) +
                     " Actual " + std::to_string(omeshData->vec3.size()) + ")");
            continue;
          }

          for (const auto &v : omeshData->vec3)
            animationData.addTranslation(CVector3D(v.x, v.y, v.z));

          rootObject_->setNodeAnimationTransformData(nodeInd, animation.name,
            CGeomObject3D::Transform::TRANSLATION, animationData);
        }
        else if (channelType == ChannelType::ROTATION) {
          for (const auto &f : imeshData->fscalars)
            animationData.addRotationRangeValue(f);

          if (! imeshData->min.empty())
            animationData.setRotationRangeMin(imeshData->min[0]);

          if (! imeshData->max.empty())
            animationData.setRotationRangeMax(imeshData->max[0]);

          //---

          animationData.setRotationInterpolation(sampler.interpType);

          //---

          if (omeshData->type != "VEC4") {
            errorMsg("Invalid Type for animation '" + animation.name + "' sampler " +
                     std::to_string(channel.sampler) + " rotation");
            continue;
          }

          if (ni != omeshData->vec4.size()) {
            errorMsg("Invalid Size for animation '" + animation.name + "' sampler " +
                     std::to_string(channel.sampler) + " rotation " +
                     "(Expected " + std::to_string(ni) +
                     " Actual " + std::to_string(omeshData->vec4.size()) + ")");
            continue;
          }

          for (const auto &v : omeshData->vec4)
            animationData.addRotation(CQuaternion(v.w, v.x, v.y, v.z));

          rootObject_->setNodeAnimationTransformData(nodeInd, animation.name,
            CGeomObject3D::Transform::ROTATION, animationData);
        }
        else if (channelType == ChannelType::SCALE) {
          for (const auto &f : imeshData->fscalars)
            animationData.addScaleRangeValue(f);

          if (! imeshData->min.empty())
            animationData.setScaleRangeMin(imeshData->min[0]);

          if (! imeshData->max.empty())
            animationData.setScaleRangeMax(imeshData->max[0]);

          //---

          animationData.setScaleInterpolation(sampler.interpType);

          //---

          if (omeshData->type != "VEC3") {
            errorMsg("Invalid Type for animation '" + animation.name + "' sampler " +
                     std::to_string(channel.sampler) + " scale");
            continue;
          }

          if (ni != omeshData->vec3.size()) {
            errorMsg("Invalid Size for animation '" + animation.name + "' sampler " +
                     std::to_string(channel.sampler) + " scale " +
                     "(Expected " + std::to_string(ni) +
                     " Actual " + std::to_string(omeshData->vec3.size()) + ")");
            continue;
          }

          for (const auto &v : omeshData->vec3)
            animationData.addScale(CVector3D(v.x, v.y, v.z));

          rootObject_->setNodeAnimationTransformData(nodeInd, animation.name,
            CGeomObject3D::Transform::SCALE, animationData);
        }
        else if (channelType == ChannelType::WEIGHTS) {
          TODO("channel.path weights");
        }
        else {
          errorMsg("Invalid channel.path : " + channel.path);
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
    node->hierTranslate = node->parent->hierTranslate*node->mtranslate.matrix();
    node->hierTransform = node->parent->hierTransform*node->transform;
  }
  else {
    node->hierTranslate = node->mtranslate.matrix();
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
  createNodeObj(node);

  rootObject_->addChild(node->object);

  //---

  // set mesh
  Mesh mesh;

  if (! getMesh(node->mesh, mesh))
    return false;

  if (! processMesh(node, mesh))
    return false;

//node->object->transform(hierTranslate);

  node->object->setMeshName(mesh.name);

  //---

  // add skin (bones/nodes)
  if (! node->skin.isEmpty()) {
    int maxIndex = -1;

    // TODO: support multiple skins ?
    Skin skin;
    if (getSkin(node->skin, skin)) {
      for (const auto &joint : skin.joints) {
        Node *node1;
        if (! getNode(joint, node1))
          continue;

        auto ind = node1->indName.ind;

        CGeomNodeData nodeData;

        nodeData.setParent(node1->parent ? int(node1->parent->indName.ind) : -1);

        nodeData.setJoint(node1->isJoint);

        if (node1->inverseBindMatrix)
          nodeData.setInverseBindMatrix(node1->inverseBindMatrix->matrix().toCMatrix());

        nodeData.resizeChildren(uint(node1->children.size()));

        nodeData.setName(node1->name);

        for (size_t ic = 0; ic < node1->children.size(); ++ic)
          nodeData.setChild(uint(ic), int(node1->children[ic].ind));

        auto ind1 = int(ind);

        if (! rootObject_->hasNode(ind1))
          rootObject_->addNode(ind1, nodeData);

        auto &nodeData1 = rootObject_->editNode(ind1);

        if (nodeData1.index() != node1->order) {
          if (nodeData1.index() == -1) {
            nodeData1.setIndex(node1->order);

            maxIndex = std::max(maxIndex, node1->order);
          }
          else
            (void) errorMsg("node index/order mismatch");
        }

        nodeData1.setObject(node1->object);

        node1->added   = true;
        node1->skinned = true;
      }
    }

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

      CGeomNodeData nodeData;

      nodeData.setParent(node1->parent ? int(node1->parent->indName.ind) : -1);

      if (node1->inverseBindMatrix)
        nodeData.setInverseBindMatrix(node1->inverseBindMatrix->matrix().toCMatrix());

      nodeData.resizeChildren(uint(node1->children.size()));

      for (size_t ic = 0; ic < node1->children.size(); ++ic) {
        nodeData.setChild(uint(ic), int(node1->children[ic].ind));

        nodeIds.push_back(node1->children[ic]);
      }

      nodeData.setName(node1->name);

      auto ind1 = int(indName.ind);

      rootObject_->addNode(ind1, nodeData);

      auto &nodeData1 = rootObject_->editNode(ind1);

      assert(nodeData1.index() == -1);

      nodeData1.setIndex(++maxIndex);

      nodeData1.setObject(node1->object);

      node1->added = true;
    }
  }
  else {
    for (auto &pn : jsonData_.nodes) {
      auto &node1 = pn.second;
      if (node1.added) continue;

      auto ind = pn.first.ind;

      CGeomNodeData nodeData;

      nodeData.setParent(node1.parent ? int(node1.parent->indName.ind) : -1);

      nodeData.setJoint(node1.isJoint);

      if (node1.inverseBindMatrix)
        nodeData.setInverseBindMatrix(node1.inverseBindMatrix->matrix().toCMatrix());

      nodeData.resizeChildren(uint(node1.children.size()));

      for (size_t ic = 0; ic < node1.children.size(); ++ic)
        nodeData.setChild(uint(ic), int(node1.children[ic].ind));

      nodeData.setName(node1.name);

      rootObject_->addNode(int(ind), nodeData);

      if (node1.object) {
        auto &nodeData1 = rootObject_->editNode(int(ind));

        nodeData1.setObject(node1.object);
      }

      node1.added = true;
    }
  }

  return true;
}

void
CImportGLTF::
createNodeObj(Node *node)
{
  assert(! node->object);

  auto name = node->name;

  if (name == "")
    name = "node" + node->indName.to_string();

  node->object = CGeometry3DInst->createObject3D(scene_, node->name);

  scene_->addObject(node->object);

  node->object->setMeshNode(int(node->indName.ind));

//node->object->transform(node->hierTransform.inverse());

  auto ind = int(node->indName.ind);

  auto &nodeData1 = rootObject_->editNode(ind);

  nodeData1.setObject(node->object);
}

CMatrix3D
CImportGLTF::
calcNodeTransform(Node *node) const
{
  if (node->matrix)
    return node->matrix->toCMatrix();

  node->mtranslate = CTranslate3D();
  node->mrotate    = CRotate3D();
  node->mscale     = CScale3D();

  if (node->translation)
    node->mtranslate = CTranslate3D(node->translation->x,
                                    node->translation->y,
                                    node->translation->z);

  if (node->rotation)
    node->mrotate = CRotate3D(node->rotation->w, node->rotation->x,
                              node->rotation->y, node->rotation->z);

  if (node->scale)
    node->mscale = CScale3D(node->scale->x, node->scale->y, node->scale->z);

  return node->mtranslate.matrix()*node->mrotate.matrix()*node->mscale.matrix();
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
processMesh(Node *node, const Mesh &mesh)
{
  auto *object = node->object;

  Skin skin;

  if (! node->skin.isEmpty())
    (void) getSkin(node->skin, skin);

  uint ip = 0;

  for (const auto &primitive : mesh.primitives) {
    auto ipStart = ip;

    //---

    std::vector<long> indices;
    std::vector<Vec3> positions;
    std::vector<Vec3> normals;
    std::vector<Vec2> texCoords0;
    std::vector<Vec4> joints0;
    std::vector<Vec4> weights0;

    if (! primitive.indices.isEmpty()) {
      auto *indMeshData = getMeshData(primitive.indices);
      if (! indMeshData)
        return errorMsg("Invalid indices mesh");

      if (isDump()) {
        debugMsg(" INDICES");
        printMeshData(*indMeshData);
      }

      indices = indMeshData->iscalars;
    }

    if (! primitive.position.isEmpty()) {
      auto *positionMeshData = getMeshData(primitive.position);
      if (! positionMeshData)
        return errorMsg("Invalid mesh POSITION");

      if (isDump()) {
        debugMsg(" POSITION");
        printMeshData(*positionMeshData);
      }

      positions = positionMeshData->vec3;
    }

    if (! primitive.normal.isEmpty()) {
      auto *normalMeshData = getMeshData(primitive.normal);
      if (! normalMeshData)
        return errorMsg("Invalid mesh NORMAL");

      if (isDump()) {
        debugMsg(" NORMAL");
        printMeshData(*normalMeshData);
      }

      normals = normalMeshData->vec3;
    }

    if (! primitive.texCoord0.isEmpty()) {
      auto *texCoord0MeshData = getMeshData(primitive.texCoord0);
      if (! texCoord0MeshData)
        return errorMsg("Invalid mesh TEXCOORD_0");

      if (isDump()) {
        debugMsg(" TEXCOORD_0");
        printMeshData(*texCoord0MeshData);
      }

      texCoords0 = texCoord0MeshData->vec2;
    }

    if (! primitive.joints0.isEmpty()) {
      auto *joints0MeshData = getMeshData(primitive.joints0);
      if (! joints0MeshData)
        return errorMsg("Invalid mesh JOINTS_0");

      if (isDump()) {
        debugMsg(" JOINTS_0");
        printMeshData(*joints0MeshData);
      }

      joints0 = joints0MeshData->vec4;
    }

    if (! primitive.weights0.isEmpty()) {
      auto *weights0MeshData = getMeshData(primitive.weights0);
      if (! weights0MeshData)
        return errorMsg("Invalid mesh WEIGHTS_0");

      if (isDump()) {
        debugMsg(" WEIGHTS_0");
        printMeshData(*weights0MeshData);
      }

      weights0 = weights0MeshData->vec4;
    }

    auto ni = indices   .size();
    auto np = positions .size();
    auto nn = normals   .size();
    auto nt = texCoords0.size();
    auto nj = joints0   .size();
    auto nw = weights0  .size();

    CIMinMax indMinMax;
    for (int i = 0; i < int(ni); ++i)
      indMinMax.add(int(indices[i]));

    auto setDefaultIndices = [&]() {
      indices.resize(np);

      for (int i = 0; i < int(np); ++i)
        indices[i] = short(i);

      indMinMax.reset();

      indMinMax.add(0);
      indMinMax.add(int(np - 1));

      ni = np;
    };

    if (ni == 0)
      setDefaultIndices();

    if (np == 0)
      return errorMsg("No mesh positions");

    if (ni > 0 && indMinMax.max() >= int(np))
      warnMsg("Invalid mesh indices size");

    if (nn > 0 && np != nn)
      warnMsg("Invalid mesh normals size");

    if (nt > 0 && np != nt)
      warnMsg("Invalid mesh texCoords0 size");

    if (nj > 0 && np != nj)
      warnMsg("Invalid mesh joints0 size");

    if (nw > 0 && np != nw)
      warnMsg("Invalid mesh weights0 size");

    //---

    for (const auto &p : positions) {
      auto ind = object->addVertex(CPoint3D(p.x, p.y, p.z));
      assert(ind == ip); ++ip;

#if 0
      if (isDebug())
        debugMsg("add vertex: " + std::to_string(p.x) + " " + std::to_string(p.y) +
                 " " + std::to_string(p.z));
#endif

      auto &v = object->getVertex(ind);

      if (ind < nn) {
        const auto &n = normals[ind];

        v.setNormal(CVector3D(n.x, n.y, n.z));

#if 0
        if (isDebug())
          debugMsg("set normal: " + std::to_string(n.x) + " " + std::to_string(n.y) +
                   " " + std::to_string(n.z));
#endif
      }

      if (ind < nt) {
        const auto &t = texCoords0[ind];

        v.setTextureMap(CPoint2D(t.x, t.y));

#if 0
        if (isDebug())
          debugMsg("set texture: " + std::to_string(t.x) + " " + std::to_string(t.y));
#endif
      }

      //---

      if (nj > 0) {
        auto ind1 = ind - ipStart;

        JointData jointData;

        Vec4 j, w;

        if (ind1 < nj)
          j = joints0[ind1];
        else
          std::cerr << "Invalid joint index\n";

        if (ind1 < nw)
          w = weights0[ind1];
        else
          std::cerr << "Invalid weight index\n";

        jointData.nodes[0].ind = int(j.x); jointData.nodes[0].weight = w.x;
        jointData.nodes[1].ind = int(j.y); jointData.nodes[1].weight = w.y;
        jointData.nodes[2].ind = int(j.z); jointData.nodes[2].weight = w.z;
        jointData.nodes[3].ind = int(j.w); jointData.nodes[3].weight = w.w;

#if 0
        Node *nodes[4];
        for (int i = 0; i < 4; ++i) {

#if 0
          if (! getNode(jointData.nodes[i].ind, nodes[i]))
            continue; // joint 0
#else
          if (! getOrderedNode(jointData.nodes[i].ind, nodes[i]))
            continue; // joint 0
#endif

          jointData.nodes[i].inverseBindMatrix = nodes[i]->inverseBindMatrix;

          jointData.skinMatrix +=
            jointData.nodes[i].inverseBindMatrix->matrix()*jointData.nodes[i].weight;
        }
#endif

        CGeomVertex3D::JointData jointData1;

        for (int i = 0; i < 4; ++i) {
          auto &nodeData = jointData1.nodeDatas[i];

          nodeData.node   = jointData.nodes[i].ind;
          nodeData.weight = jointData.nodes[i].weight;
        }

        v.setJointData(jointData1);

//      objectJoints_[ind1] = jointData;

        if (isDebug()) {
          std::cerr << "Vertex " << v.getInd() << " Joints:\n";

          for (int i = 0; i < 4; ++i) {
            const auto &nodeData = jointData1.nodeDatas[i];

            std::cerr << "  Node: " << nodeData.node;

#if 1
            auto node1 = mapSkinNode(skin, nodeData.node);

            Node *skinNode = nullptr;
            getNode(node1, skinNode);
#else
            Node *skinNode = nullptr;
            getNode(nodeData.node, skinNode);
#endif

            if (skinNode)
              std::cerr << " (" << skinNode->indName << " '" << skinNode->name << "')";
            else
              std::cerr << " (no node)";

            std::cerr << " = " << nodeData.weight << "\n";
          }
        }
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

      object->addITriangle(uint(i1 + ipStart), uint(i2 + ipStart), uint(i3 + ipStart));

#if 0
      if (isDebug())
        debugMsg("add triangle: " + std::to_string(i1) + " " + std::to_string(i2) +
                 " " + std::to_string(i3));
#endif
    }

    //---

    if (! primitive.material.isEmpty()) {
      Material *material { nullptr };

      if (! getMaterial(primitive.material, material))
        return errorMsg("Invalid mesh material");

      bool updateMaterial = false;

      if (! material->materialP) {
        auto *materialP = scene_->getMaterial(material->name);

        if (! materialP) {
          materialP = CGeometry3DInst->createMaterial();

          materialP->setName(material->name);

          scene_->addMaterial(materialP);

          updateMaterial = true;
        }

        material->materialP = materialP;
      }

#if 0
      if (isDebug())
        printMaterial(material, primitive.material);
#endif

      //---

      // diffuse color
      std::optional<CRGBA> diffuseColor;

      if (material->diffuse)
        diffuseColor = colorRGBA(material->diffuse.value());

      if (material->baseColorFactor)
        diffuseColor = colorRGBA(material->baseColorFactor.value());

      if (diffuseColor) {
        object->setFaceDiffuse(*diffuseColor);

        if (updateMaterial)
          material->materialP->setDiffuse(*diffuseColor);
      }

      //---

      // diffuse texture
      if (material->baseColorTexture.index >= 0) {
        Texture *texture { nullptr };

        auto textureName = IndName(material->baseColorTexture.index);

        if (! getTexture(textureName, texture))
          return errorMsg("Invalid base color texture");

        resolveTexture(textureName, texture);

        object->setDiffuseTexture(texture->texture);

        if (updateMaterial)
          material->materialP->setDiffuseTexture(texture->texture);
      }

      //---

      // normal texture
      if (material->normalTexture.index >= 0) {
        Texture *texture { nullptr };

        auto textureName = IndName(material->normalTexture.index);

        if (! getTexture(IndName(material->normalTexture.index), texture))
          return errorMsg("Invalid normal texture");

        resolveTexture(textureName, texture);

        object->setNormalTexture(texture->texture);

        if (updateMaterial)
          material->materialP->setNormalTexture(texture->texture);
      }

      //---

      // specular color
      std::optional<CRGBA> specularColor;

      if (material->specular)
        specularColor = colorRGBA(material->specular.value());

      if (specularColor) {
        object->setFaceSpecular(*specularColor);

        if (updateMaterial)
          material->materialP->setSpecular(*specularColor);
      }

      //---

      // specular texture
      if (material->specularTexture.index >= 0) {
        Texture *texture { nullptr };

        auto textureName = IndName(material->specularTexture.index);

        if (! getTexture(IndName(material->specularTexture.index), texture))
          return errorMsg("Invalid specular texture");

        resolveTexture(textureName, texture);

        object->setSpecularTexture(texture->texture);

        if (updateMaterial)
          material->materialP->setSpecularTexture(texture->texture);
      }

      //---

      // emissive color
      std::optional<CRGBA> emissionColor;

      if (material->emissiveFactor)
        emissionColor = vectorRGBA(material->emissiveFactor.value());

      if (emissionColor) {
        object->setFaceEmission(*emissionColor);

        if (updateMaterial)
          material->materialP->setEmission(*emissionColor);
      }

      //---

      // emissive texture
      if (material->emissiveTexture.index >= 0) {
        Texture *texture { nullptr };

        auto textureName = IndName(material->emissiveTexture.index);

        if (! getTexture(IndName(material->emissiveTexture.index), texture))
          return errorMsg("Invalid emissive texture");

        resolveTexture(textureName, texture);

        object->setEmissiveTexture(texture->texture);

        if (updateMaterial)
          material->materialP->setEmissiveTexture(texture->texture);
      }

      //----

      if (updateMaterial)
        material->materialP->setTwoSided(material->doubleSided);

      //---

      object->setMaterialP(material->materialP);
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

  std::string prefix;

  if      (image.mimeType == "image/png") {
    auto image1 = CImageMgrInst->createImage();

    if (! image1->read(data, len, CFILE_TYPE_IMAGE_PNG))
      return errorMsg("Invalid image data");

    image.image = image1;

    prefix = "png";
  }
  else if (image.mimeType == "image/jpeg") {
    auto image1 = CImageMgrInst->createImage();

    if (! image1->read(data, len, CFILE_TYPE_IMAGE_JPG))
      return errorMsg("Invalid image data");

    image.image = image1;

    prefix = "jpg";
  }
  else if (image.mimeType == "image/webp") {
    auto image1 = CImageMgrInst->createImage();

    if (! image1->read(data, len, CFILE_TYPE_IMAGE_WEBP))
      return errorMsg("Invalid image data");

    image.image = image1;

    prefix = "webp";
  }
  else
    return errorMsg("Invalid image type '" + image.mimeType + "'");

  if (isSaveImage()) {
    auto name = "image." + image.indName.to_string() + "." + prefix;

    if (! writeFile(name, data, len))
      return errorMsg("Image write failed");
  }

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
getMaterial(const IndName &indName, Material* &material) const
{
  auto pm = jsonData_.materials.find(indName);
  if (pm == jsonData_.materials.end()) return false;

  material = const_cast<Material *>(&(*pm).second);

  return true;
}

bool
CImportGLTF::
getTexture(const IndName &indName, Texture* &texture) const
{
  auto pt = jsonData_.textures.find(indName);
  if (pt == jsonData_.textures.end()) return false;

  texture = const_cast<Texture *>(&(*pt).second);

  return true;
}

bool
CImportGLTF::
resolveTexture(const IndName &textureName, Texture *texture)
{
  if (texture->texture)
    return true;

  if (! texture->source.isEmpty()) {
    auto pi = jsonData_.images.find(texture->source);

    if (pi == jsonData_.images.end())
      return errorMsg("Invalid base color texture image");

    const auto &image = (*pi).second;

    if (! resolveImage(image))
      return false;

    texture->texture = CGeometry3DInst->createTexture(image.image);

    texture->texture->setName(textureName.to_string());

    scene_->addTexture(texture->texture);
  }
  else {
    warnMsg("Unhandled base color texture " + textureName.ind);
    return false;
  }

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
getOrderedNode(int order, Node* &node) const
{
  for (const auto &pn : jsonData_.nodes) {
    const auto &node1 = pn.second;

    if (node1.order == order) {
      node = const_cast<Node *>(&node1);
      return true;
    }
  }

  return false;
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

  auto valueOptNumber = [&](const CJson::ValueP &value, OptReal &optNumber) {
    double number;
    if (! valueNumber(value, number))
      return false;
    optNumber = number;
    return true;
  };

  auto valueLong = [](const CJson::ValueP &value, long &number) {
     if (! value->isNumber())
      return false;

    auto *jnumber = value->cast<CJson::Number>();

    number = long(jnumber->value());

    return true;
  };

  auto valueOptLong = [&](const CJson::ValueP &value, OptLong &optNumber) {
    long number;
    if (! valueLong(value, number))
      return false;
    optNumber = number;
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

      if      (rvalues.size() == 3)
        color = Color(rvalues[0], rvalues[1], rvalues[2], 1.0);
      else if (rvalues.size() == 4)
        color = Color(rvalues[0], rvalues[1], rvalues[2], rvalues[3]);
      else
        return false;
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

  auto valueOptColor = [&](const CJson::ValueP &value, OptColor &optColor) {
    Color color;
    if (! valueColor(value, color))
      return false;
    optColor = color;
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

  //---

  auto readSparseAccessor = [&](CJson::Object *sparseObj, Accessor &accessor,
                                const std::string &id) {
    accessor.sparse = true;

    for (const auto &pnv2 : sparseObj->nameValueMap()) {
      if      (pnv2.first == "count") {
        if (! valueLong(pnv2.second, accessor.sparseCount))
          return errorMsg("Invalid " + id + "/" + pnv2.first);
      }
      else if (pnv2.first == "indices") {
        if (! pnv2.second->isObject())
          return errorMsg("Invalid JSON " + id + "/" + pnv2.first + " data");

        auto *sparseIndicesObj = pnv2.second->cast<CJson::Object>();

        for (const auto &pnv3 : sparseIndicesObj->nameValueMap()) {
          if      (pnv3.first == "bufferView") {
            if (! valueIndName(pnv3.second, accessor.sparseIndicesBufferView))
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" + pnv3.first);
          }
          else if (pnv3.first == "byteOffset") {
            if (! valueLong(pnv3.second, accessor.sparseIndicesByteOffset))
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" + pnv3.first);
          }
          else if (pnv3.first == "componentType") {
            if (! valueLong(pnv3.second, accessor.sparseIndicesComponentType))
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" + pnv3.first);
          }
          else
            return errorMsg("Unhandled " + id + "/" + pnv2.first  + " name '" +
                            pnv3.first + "'");
        }
      }
      else if (pnv2.first == "values") {
        if (! pnv2.second->isObject())
          return errorMsg("Invalid JSON " + id + "/" + pnv2.first + " data");

        auto *sparseValuesObj = pnv2.second->cast<CJson::Object>();

        for (const auto &pnv3 : sparseValuesObj->nameValueMap()) {
          if      (pnv3.first == "bufferView") {
            if (! valueIndName(pnv3.second, accessor.sparseValuesBufferView))
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" + pnv3.first);
          }
          else if (pnv3.first == "byteOffset") {
            if (! valueLong(pnv3.second, accessor.sparseValuesByteOffset))
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" + pnv3.first);
          }
          else
            return errorMsg("Unhandled " + id + "/" + pnv2.first + " name '" +
                            pnv3.first + "'");
        }
      }
      else {
        return errorMsg("Invalid " + id + " name '" + pnv2.first + "'");
      }
    }

    return true;
  };

  auto readAccessor = [&](CJson::Object *jobj, Accessor &accessor, const std::string &id) {
    for (const auto &pnv1 : jobj->nameValueMap()) {
      if      (pnv1.first == "bufferView") {
        if (! valueIndName(pnv1.second, accessor.bufferView))
          return errorMsg("Invalid " + id + "/" + pnv1.first + " ind/name");
      }
      else if (pnv1.first == "byteOffset") {
        if (! valueLong(pnv1.second, accessor.byteOffset))
          return errorMsg("Invalid " + id + "/" + pnv1.first + " number");
      }
      else if (pnv1.first == "byteStride") {
        if (! valueLong(pnv1.second, accessor.byteStride))
          return errorMsg("Invalid " + id + "/" + pnv1.first + " number");
      }
      else if (pnv1.first == "type") {
        if (! valueString(pnv1.second, accessor.type))
          return errorMsg("Invalid " + id + "/" + pnv1.first + " string");
      }
      else if (pnv1.first == "componentType") {
        if (! valueLong(pnv1.second, accessor.componentType))
          return errorMsg("Invalid " + id + "/" + pnv1.first + " number");
      }
      else if (pnv1.first == "count") {
        if (! valueLong(pnv1.second, accessor.count))
          return errorMsg("Invalid " + id + "/" + pnv1.first + " number");
      }
      else if (pnv1.first == "min") {
        if (! pnv1.second->isArray())
          return errorMsg("Invalid JSON " + id + "/" + pnv1.first + " data");

        auto *maxArr = pnv1.second->cast<CJson::Array>();

        for (const auto &pv2 : maxArr->values()) {
          double r;
          if (! valueNumber(pv2, r))
            return errorMsg("Invalid " + id + "/" + pnv1.first + " number");

          accessor.min.push_back(r);
        }
      }
      else if (pnv1.first == "max") {
        if (! pnv1.second->isArray())
          return errorMsg("Invalid JSON " + id + "/" + pnv1.first + " data");

        auto *maxArr = pnv1.second->cast<CJson::Array>();

        for (const auto &pv2 : maxArr->values()) {
          double r;
          if (! valueNumber(pv2, r))
            return errorMsg("Invalid " + id + "/" + pnv1.first + " number");

          accessor.max.push_back(r);
        }
      }
      else if (pnv1.first == "name") {
        if (! valueString(pnv1.second, accessor.name))
          return errorMsg("Invalid " + id + "/" + pnv1.first + " string");
      }
      else if (pnv1.first == "normalized") {
        if (! valueBool(pnv1.second, accessor.normalized))
          return errorMsg("Invalid " + id + "/" + pnv1.first + " bool");
      }
      else if (pnv1.first == "sparse") {
        if (! pnv1.second->isObject())
          return errorMsg("Invalid JSON " + id + "/" + pnv1.first + " data");

        auto *sparseObj = pnv1.second->cast<CJson::Object>();

        readSparseAccessor(sparseObj, accessor, id + "/" + pnv1.first);
      }
      else
        return errorMsg("Invalid " + id + " name '" + pnv1.first + "'");
    }

    return true;
  };

  auto readMaterialTexture = [&](CJson::Object *obj, MaterialTexture &texture,
                                 const std::string &id) {
    for (const auto &pnv : obj->nameValueMap()) {
      if      (pnv.first == "index") {
        if (! valueLong(pnv.second, texture.index))
          return errorMsg("Invalid " + id + "/" + pnv.first + " number");
      }
      else if (pnv.first == "texCoord") {
        if (! valueOptLong(pnv.second, texture.texCoord))
          return errorMsg("Invalid " + id + "/" + pnv.first + " number");
      }
      else if (pnv.first == "scale") {
        if (! valueOptNumber(pnv.second, texture.scale))
          return errorMsg("Invalid " + id + "/" + pnv.first + " number");

        //TODOValue("Unhandled " + pnv.first + " " + id + " scale", pnv.second);
      }
      else if (pnv.first == "strength") {
        if (! valueOptNumber(pnv.second, texture.strength))
          return errorMsg("Invalid " + id + "/" + pnv.first + " number");

        //TODOValue("Unhandled " + id + "/" + pnv.first, pnv.second);
      }
      else if (pnv.first == "extensions") {
        if (! pnv.second->isObject())
          return errorMsg("Invalid JSON " + id + "/" + pnv.first + " data");

        auto *obj1 = pnv.second->cast<CJson::Object>();

        for (const auto &pnv1 : obj1->nameValueMap()) {
          if (pnv1.first == "KHR_texture_transform") {
            if (! pnv1.second->isObject())
              return errorMsg("Invalid JSON " + id + "/" + pnv.first + "/" + pnv1.first + " data");

            auto *obj2 = pnv1.second->cast<CJson::Object>();

            for (const auto &pnv2 : obj2->nameValueMap()) {
              if      (pnv2.first == "rotation") {
                if (! valueOptNumber(pnv2.second, texture.rotation))
                  return errorMsg("Invalid " + id + "/" + pnv.first + "/" + pnv1.first +
                                  "/" + pnv2.first + " number");
              }
              else if (pnv2.first == "scale") {
                std::vector<float> values;
                if (! arrayNumbers(pnv2.second, values) || values.size() != 2)
                  return errorMsg("Invalid " + id + "/" + pnv.first + "/" + pnv1.first +
                                  "/" + pnv2.first + " values");

                texture.scaleVector = CVector2D(values[0], values[1]);
              }
              else if (pnv2.first == "offset") {
                std::vector<float> values;
                if (! arrayNumbers(pnv2.second, values) || values.size() != 2)
                  return errorMsg("Invalid " + id + "/" + pnv.first + "/" + pnv1.first +
                                  "/" + pnv2.first + " values");

                texture.offsetVector = CVector2D(values[0], values[1]);
              }
              else if (pnv2.first == "texCoord") {
                if (! valueOptLong(pnv2.second, texture.texCoord))
                  return errorMsg("Invalid " + id + "/" + pnv.first + "/" + pnv1.first +
                                  "/" + pnv2.first + " number");

                //TODOValue("Unhandled " + id + "/" + pnv.first + "/" + pnv1.first +
                //          "/" + pnv2.first, pnv2.second);
              }
              else
                TODOValue("Unhandled " + id + "/" + pnv.first + "/" + pnv1.first +
                          "/" + pnv2.first, pnv2.second);
            }
          }
          else
            TODOValue("Unhandled " + id + "/" + pnv.first + "/" + pnv1.first, pnv1.second);
        }
      }
      else {
        warnMsg("Invalid " + id + " name " + pnv.first);
      }
    }

    return true;
  };

  auto readBuffer = [&](CJson::Object *jobj, Buffer &buffer, const std::string &id) {
    for (const auto &pnv2 : jobj->nameValueMap()) {
      if      (pnv2.first == "byteLength") {
        if (! valueLong(pnv2.second, buffer.byteLength))
          return errorMsg("Invalid " + id + "/" + pnv2.first + " number");
      }
      else if (pnv2.first == "type") {
        if (! valueString(pnv2.second, buffer.type))
          return errorMsg("Invalid " + id + "/" + pnv2.first + " string");
      }
      else if (pnv2.first == "uri") {
        if (! valueString(pnv2.second, buffer.uri))
          return errorMsg("Invalid " + id + "/" + pnv2.first + " string");
      }
      else if (pnv2.first == "name") {
        if (! valueString(pnv2.second, buffer.name))
          return errorMsg("Invalid " + id + "/" + pnv2.first + " string");
      }
      else
        debugValueMsg("  " + id + "/" + pnv2.first, pnv2.second);
    }

    return true;
  };

  auto readBufferView = [&](CJson::Object *jobj, BufferView &bufferView, const std::string &id) {
    for (const auto &pnv2 : jobj->nameValueMap()) {
      if      (pnv2.first == "buffer") {
        if (! valueIndName(pnv2.second, bufferView.buffer))
          return errorMsg("Invalid " + id + "/" + pnv2.first + " ind/name");
      }
      else if (pnv2.first == "byteOffset") {
        if (! valueLong(pnv2.second, bufferView.byteOffset))
          return errorMsg("Invalid " + id + "/" + pnv2.first + " number");
      }
      else if (pnv2.first == "byteLength") {
        if (! valueLong(pnv2.second, bufferView.byteLength))
          return errorMsg("Invalid " + id + "/" + pnv2.first + " number");
      }
      else if (pnv2.first == "byteStride") {
        if (! valueLong(pnv2.second, bufferView.byteStride))
          return errorMsg("Invalid " + id + "/" + pnv2.first + " number");
      }
      else if (pnv2.first == "target") {
        if (! valueLong(pnv2.second, bufferView.target))
          return errorMsg("Invalid " + id + "/" + pnv2.first + " number");
      }
      else if (pnv2.first == "name") {
        if (! valueString(pnv2.second, bufferView.name))
          return errorMsg("Unhandled " + id + "/" + pnv2.first);
      }
      else {
        return errorMsg("Invalid buffer name " + pnv2.first);
      }
    }

    return true;
  };

  auto readMesh = [&](CJson::Object *jobj, Mesh &mesh, const std::string &id) {
    for (const auto &pnv2 : jobj->nameValueMap()) {
      if      (pnv2.first == "name") {
        if (! valueString(pnv2.second, mesh.name))
          return errorMsg("Invalid name string");
      }
      else if (pnv2.first == "primitives") {
        if (! pnv2.second->isArray())
          return errorMsg("Invalid JSON " + id + "/" + pnv2.first);

        auto *arr2 = pnv2.second->cast<CJson::Array>();

        Primitive primitive;

        for (const auto &pv2 : arr2->values()) {
          if (! pv2->isObject())
            return errorMsg("Invalid JSON " + id + "/" + pnv2.first + " data");

          auto *obj3 = pv2->cast<CJson::Object>();

          for (const auto &pnv3 : obj3->nameValueMap()) {
            if      (pnv3.first == "attributes") {
              if (! pnv3.second->isObject())
                return errorMsg("Invalid JSON " + id + " attributes");

              auto *obj4 = pnv3.second->cast<CJson::Object>();

              for (const auto &pnv4 : obj4->nameValueMap()) {
                if      (pnv4.first == "POSITION") { // VEC3
                  if (! valueIndName(pnv4.second, primitive.position))
                    return errorMsg("Invalid " + pnv4.first + " ind/name");
                }
                else if (pnv4.first == "NORMAL") { // VEC3
                  if (! valueIndName(pnv4.second, primitive.normal))
                    return errorMsg("Invalid " + pnv4.first + " ind/name");
                }
                else if (pnv4.first == "TANGENT") { // VEC4
                  if (! valueIndName(pnv4.second, primitive.tangent))
                    return errorMsg("Invalid " + pnv4.first + " ind/name");
                }
                else if (pnv4.first == "TEXCOORD_0") { // VEC2
                  if (! valueIndName(pnv4.second, primitive.texCoord0))
                    return errorMsg("Invalid " + pnv4.first + " ind/name");
                }
                else if (pnv4.first == "TEXCOORD_1") { // VEC2
                  if (! valueIndName(pnv4.second, primitive.texCoord1))
                    return errorMsg("Invalid " + pnv4.first + " ind/name");
                }
                else if (pnv4.first == "TEXCOORD_2") { // VEC2
                  if (! valueIndName(pnv4.second, primitive.texCoord2))
                    return errorMsg("Invalid " + pnv4.first + " ind/name");
                }
                else if (pnv4.first == "COLOR_0") { // VEC3 or VEC4
                  if (! valueIndName(pnv4.second, primitive.color0))
                    return errorMsg("Invalid " + pnv4.first + " ind/name");
                }
                else if (pnv4.first == "JOINTS_0") { // VEC4
                  if (! valueIndName(pnv4.second, primitive.joints0))
                    return errorMsg("Invalid " + pnv4.first + " ind/name");
                }
                else if (pnv4.first == "WEIGHTS_0") { // VEC4
                  if (! valueIndName(pnv4.second, primitive.weights0))
                    return errorMsg("Invalid " + pnv4.first + " ind/name");
                }
                else {
                  debugValueMsg("  " + id + "/" + pnv2.first + "/" + pnv3.first + "/" +
                                pnv4.first, pnv4.second);
                }
              }
            }
            else if (pnv3.first == "indices") {
              if (! valueIndName(pnv3.second, primitive.indices))
                return errorMsg("Invalid " + pnv3.first + " ind/name");
            }
            else if (pnv3.first == "material") {
              if (! valueIndName(pnv3.second, primitive.material))
                return errorMsg("Invalid " + pnv3.first + " ind/name");
            }
            else if (pnv3.first == "mode") {
              if (! valueLong(pnv3.second, primitive.mode))
                return errorMsg("Invalid " + pnv3.first + " value");
            }
            else if (pnv3.first == "targets") {
              if (! pnv3.second->isArray())
                return errorMsg("Invalid JSON " + id + "/" + pnv3.first);

              auto *arr3 = pnv3.second->cast<CJson::Array>();

              for (const auto &pv3 : arr3->values()) {
                if (! pv3->isObject())
                  return errorMsg("Invalid JSON " + id + "/" + pnv3.first + " data");

                auto *obj4 = pv3->cast<CJson::Object>();

                for (const auto &pnv4 : obj4->nameValueMap()) {
                  if      (pnv4.first == "POSITION") {
                    IndName position;
                    if (! valueIndName(pnv4.second, position))
                      return errorMsg("Invalid " + id + "/" + pnv4.first + " ind/name");
                  }
                  else if (pnv4.first == "NORMAL") {
                    IndName normal;
                    if (! valueIndName(pnv4.second, normal))
                      return errorMsg("Invalid " + id + "/" + pnv4.first + " ind/name");
                  }
                  else if (pnv4.first == "TANGENT") {
                    IndName tangent;
                    if (! valueIndName(pnv4.second, tangent))
                      return errorMsg("Invalid " + id + "/" + pnv4.first + " ind/name");
                  }
                  else {
                    return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                    "/" + pnv3.first + "/" + pnv4.first);
                  }
                }
              }
            }
            else if (pnv3.first == "extensions") {
              if (! pnv3.second->isObject())
                return errorMsg("Invalid JSON " + id + "/" + pnv2.first + "/" +
                                pnv3.first);

              auto *obj4 = pnv3.second->cast<CJson::Object>();

              for (const auto &pnv4 : obj4->nameValueMap()) {
                if (pnv4.first == "KHR_materials_variants") {
                  if (! pnv4.second->isObject())
                    return errorMsg("Invalid JSON " + id + "/" + pnv2.first + "/" +
                                    pnv3.first + "/" + pnv4.first + " data type");

                  auto *obj5 = pnv4.second->cast<CJson::Object>();

                  for (const auto &pnv5 : obj5->nameValueMap()) {
                    if (pnv5.first == "mappings") {
                      if (! pnv5.second->isArray())
                        return errorMsg("Invalid JSON " + id + "/" + pnv2.first + "/" +
                                        pnv3.first + "/" + pnv4.first + "/" + pnv5.first +
                                        " value type");

                      auto *arr6 = pnv5.second->cast<CJson::Array>();

                      for (const auto &pv6 : arr6->values()) {
                        if (! pv6->isObject())
                          return errorMsg("Invalid JSON " + id + "/" + pnv2.first + "/" +
                                          pnv3.first + "/" + pnv4.first + "/" + pnv5.first +
                                          " data type");

                        auto *obj7 = pv6->cast<CJson::Object>();

                        for (const auto &pnv7 : obj7->nameValueMap()) {
                          if      (pnv7.first == "material") {
                            OptReal material;
                            if (! valueOptNumber(pnv7.second, material))
                              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                              pnv3.first + "/" + pnv4.first + "/" +
                                              pnv5.first + "/" + pnv7.first + " number");
                          }
                          else if (pnv7.first == "variants") {
                            if (! pnv7.second->isArray())
                              return errorMsg("Invalid JSON " + id + "/" + pnv2.first +
                                              "/" + pnv3.first + "/" + pnv4.first + "/" +
                                              pnv5.first + "/" + pnv7.first + " value type");

                             auto *arr8 = pnv7.second->cast<CJson::Array>();

                             for (const auto &pv8 : arr8->values()) {
                                OptReal variant;
                                if (! valueOptNumber(pv8, variant))
                                  return errorMsg("Invalid " + id + "/" + pnv2.first +
                                                  "/" + pnv3.first + "/" + pnv4.first + "/" +
                                                  pnv5.first + "/" + pnv7.first + " number");
                             }
                          }
                          else
                            return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                            "/" + pnv3.first + "/" + pnv4.first + "/" +
                                            pnv5.first + "/" + pnv7.first);
                        }
                      }
                    }
                    else {
                      return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                      "/" + pnv3.first + "/" + pnv4.first + "/" + pnv5.first);
                    }
                  }
                }
                else {
                  TODOValue("Unhandled " + id + "/" + pnv2.first + "/" +
                            pnv3.first + "/" + pnv4.first, pnv4.second);
                }
              }
            }
            else {
              debugValueMsg("  " + id + "/" + pnv3.first, pnv3.second);
            }
          }

          mesh.primitives.push_back(primitive);
        }
      }
      else if (pnv2.first == "weights") {
        if (! pnv2.second->isArray())
          return errorMsg("Invalid JSON " + id + "/" + pnv2.first);

        auto *arr2 = pnv2.second->cast<CJson::Array>();

        for (const auto &pv2 : arr2->values()) {
          double r;
          if (! valueNumber(pv2, r))
            return errorMsg("Invalid " + id + "/" + pnv2.first + " value");
        }
      }
      else if (pnv2.first == "extras") {
        if (! pnv2.second->isObject())
          return errorMsg("Invalid JSON " + id + "/" + pnv2.first + " data");

        auto *obj3 = pnv2.second->cast<CJson::Object>();

        for (const auto &pnv3 : obj3->nameValueMap()) {
          if (pnv3.first == "targetNames") {
            auto *arr3 = pnv3.second->cast<CJson::Array>();

            for (const auto &pv3 : arr3->values()) {
              std::string name;
              if (! valueString(pv3, name))
                return errorMsg("Invalid " + id + "/" + pnv2.first + "/" + pnv3.first + " value");
            }
          }
          else
            debugValueMsg("  " + id + "/" + pnv2.first + "/" + pnv3.first, pnv3.second);
        }
      }
      else if (pnv2.first == "extensions") {
        if (! pnv2.second->isObject())
          return errorMsg("Invalid JSON " + id + "/" + pnv2.first + " data");

        auto *obj3 = pnv2.second->cast<CJson::Object>();

        for (const auto &pnv3 : obj3->nameValueMap()) {
          debugValueMsg("  " + id + "/" + pnv2.first + "/" + pnv3.first, pnv3.second);
        }
      }
      else {
        debugValueMsg("  " + id + "/" + pnv2.first, pnv2.second);
      }
    }

    return true;
  };

  auto readNode = [&](CJson::Object *jobj, Node &node, const std::string &id) {
    for      (const auto &pnv2 : jobj->nameValueMap()) {
      if      (pnv2.first == "name") {
        if (! valueString(pnv2.second, node.name))
          return errorMsg("Invalid " + id + "/" + pnv2.first + " string");
      }
      else if (pnv2.first == "mesh") {
        if (! valueIndName(pnv2.second, node.mesh))
          return errorMsg("Invalid " + id + "/" + pnv2.first + " number");
      }
      else if (pnv2.first == "matrix") {
        std::vector<float> rvalues;
        if (! arrayNumbers(pnv2.second, rvalues))
          return errorMsg("Invalid " + id + "/" + pnv2.first + " array");

        if (rvalues.size() != 16)
          return errorMsg("Invalid " + id + "/" + pnv2.first + " array size");

        CGLMatrix3D m;
        m.setData(&rvalues[0]);

        node.matrix = m;
      }
      else if (pnv2.first == "children") {
        if (! pnv2.second->isArray())
          return errorMsg("Invalid " + id + "/" + pnv2.first + " array");

        auto *arr2 = pnv2.second->cast<CJson::Array>();

        for (const auto &pv3 : arr2->values()) {
          IndName indName;
          if (! valueIndName(pv3, indName))
            return errorMsg("Invalid " + id + "/" + pnv2.first + " child");

          node.children.push_back(indName);
        }
      }
      else if (pnv2.first == "meshes") {
        if (! pnv2.second->isArray())
          return errorMsg("Invalid " + id + "/" + pnv2.first + " array");

        auto *arr2 = pnv2.second->cast<CJson::Array>();

        for (const auto &pv3 : arr2->values()) {
          if (! pv3->isString())
            return errorMsg("Invalid " + id + "/" + pnv2.first + " string");

          std::string str;
          (void) valueString(pv3, str);
        }

        TODO("Unhandled " + id + " " + pnv2.first);
      }
      else if (pnv2.first == "skin") {
        if (! valueIndName(pnv2.second, node.skin))
          return errorMsg("Invalid " + id + "/" + pnv2.first + " number");
      }
      else if (pnv2.first == "rotation") {
        std::vector<float> rvalues;
        if (! arrayNumbers(pnv2.second, rvalues))
          return errorMsg("Invalid " + id + "/" + pnv2.first + " array");

        if (rvalues.size() != 4)
          return errorMsg("Invalid " + id + "/" + pnv2.first + " array size");

        node.rotation = Vec4(rvalues[0], rvalues[1], rvalues[2], rvalues[3]);
      }
      else if (pnv2.first == "scale") {
        std::vector<float> rvalues;
        if (! arrayNumbers(pnv2.second, rvalues))
          return errorMsg("Invalid " + id + "/" + pnv2.first + " array");

        if (rvalues.size() != 3)
          return errorMsg("Invalid " + id + "/" + pnv2.first + " array size");

        node.scale = Vec3(rvalues[0], rvalues[1], rvalues[2]);
      }
      else if (pnv2.first == "translation") {
        std::vector<float> rvalues;
        if (! arrayNumbers(pnv2.second, rvalues))
          return errorMsg("Invalid " + id + "/" + pnv2.first + " array");

        if (rvalues.size() != 3)
          return errorMsg("Invalid " + id + "/" + pnv2.first + " array size");

        node.translation = Vec3(rvalues[0], rvalues[1], rvalues[2]);
      }
      else if (pnv2.first == "camera") {
        if (! valueLong(pnv2.second, node.camera))
          return errorMsg("Invalid " + id + "/" + pnv2.first + " number");
      }
      else if (pnv2.first == "extensions") {
        //TODOValue("Unhandled " + id + "/" + pnv2.first, pnv2.second);

        if (! pnv2.second->isObject())
          return errorMsg("Invalid " + id + "/" + pnv2.first + " type");

        auto *obj3 = pnv2.second->cast<CJson::Object>();

        for (const auto &pnv3 : obj3->nameValueMap()) {
          if      (pnv3.first == "KHR_lights_punctual") {
            if (! pnv3.second->isObject())
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" + pnv3.first);

            auto *obj4 = pnv3.second->cast<CJson::Object>();

            for (const auto &pnv4 : obj4->nameValueMap()) {
              if (pnv4.first == "light") {
                IndName ind;
                if (! valueIndName(pnv4.second, ind))
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" + pnv3.first +
                                  "/" + pnv4.first + " name");
              }
              else {
                debugValueMsg("Invalid name " + id + "/" + pnv2.first + "/" +
                              pnv3.first + "/" + pnv4.first, pnv4.second);
              }
            }
          }
          else if (pnv3.first == "EXT_mesh_gpu_instancing") {
            if (! pnv3.second->isObject())
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" + pnv3.first);

            auto *obj4 = pnv3.second->cast<CJson::Object>();

            for (const auto &pnv4 : obj4->nameValueMap()) {
              if (pnv4.first == "attributes") {
                if (! pnv4.second->isObject())
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" + pnv3.first +
                                  "/" + pnv4.first);

                auto *obj5 = pnv4.second->cast<CJson::Object>();

                for (const auto &pnv5 : obj5->nameValueMap()) {
                  if      (pnv5.first == "TRANSLATION") {
                    long ind;
                    if (! valueLong(pnv5.second, ind))
                      return errorMsg("Invalid " + id + "/" + pnv2.first + "/" + pnv3.first +
                                      "/" + pnv4.first + "/" + pnv5.first + " value");
                  }
                  else if (pnv5.first == "ROTATION") {
                    long ind;
                    if (! valueLong(pnv5.second, ind))
                      return errorMsg("Invalid " + id + "/" + pnv2.first + "/" + pnv3.first +
                                      "/" + pnv4.first + "/" + pnv5.first + " value");
                  }
                  else if (pnv5.first == "SCALE") {
                    long ind;
                    if (! valueLong(pnv5.second, ind))
                      return errorMsg("Invalid " + id + "/" + pnv2.first + "/" + pnv3.first +
                                      "/" + pnv4.first + "/" + pnv5.first + " value");
                  }
                  else
                    debugValueMsg("Invalid name " + id + "/" + pnv2.first + "/" + pnv3.first +
                                  "/" + pnv4.first + "/" + pnv5.first, pnv5.second);
                }
              }
              else {
                debugValueMsg("Invalid name " + id + "/" + pnv2.first + "/" +
                              pnv3.first + "/" + pnv4.first, pnv4.second);
              }
            }
          }
          else {
            debugValueMsg("Invalid name " + id + "/" + pnv2.first + "/" +
                          pnv3.first, pnv3.second);
          }
        }
      }
      else if (pnv2.first == "extras") {
        //TODOValue("Unhandled " + id + "/" + pnv2.first, pnv2.second);

        if (! pnv2.second->isObject())
          return errorMsg("Invalid " + id + "/" + pnv2.first);

        auto *obj3 = pnv2.second->cast<CJson::Object>();

        for (const auto &pnv3 : obj3->nameValueMap()) {
          if (pnv3.first == "name") {
            std::string name;
            if (! valueString(pnv3.second, name))
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" + pnv3.first + " string");
          }
          else {
            debugValueMsg("Invalid name " + id + "/" + pnv2.first + "/" + pnv3.first, pnv3.second);
          }
        }
      }
      else {
        debugValueMsg("  " + id + "/" + pnv2.first, pnv2.second);
      }
    }

    return true;
  };

  auto readImage = [&](CJson::Object *jobj, Image &image, const std::string &id) {
    auto processNameValue1 = [&](const std::string &name, const CJson::ValueP &nvalue) {
      if      (name == "bufferView") {
        if (! valueIndName(nvalue, image.bufferView))
          return errorMsg("Invalid " + id + "/" + name + " ind/name");
      }
      else if (name == "mimeType") {
        if (! valueString(nvalue, image.mimeType))
          return errorMsg("Invalid " + id + "/" + name + " string");
      }
      else if (name == "name") {
        if (! valueString(nvalue, image.name))
          return errorMsg("Invalid " + id + "/" + name + " string");
      }
      else if (name == "url") {
        if (! valueString(nvalue, image.url))
          return errorMsg("Invalid " + id + "/" + name + " string");
      }
      else if (name == "uri") {
        if (! valueString(nvalue, image.uri))
          return errorMsg("Invalid " + id + "/" + name + " string");
      }
      else if (name == "width") {
        if (! valueLong(nvalue, image.width))
          return errorMsg("Invalid " + id + "/" + name + " value");
      }
      else if (name == "height") {
        if (! valueLong(nvalue, image.height))
          return errorMsg("Invalid " + id + "/" + name + " value");
      }
      else {
        debugMsg("  " + id + "/" + name + " : " + nvalue->typeName());
        return false;
      }

      return true;
    };

    auto processNameValue = [&](const std::string &name, const CJson::ValueP &nvalue) {
      if      (name == "bufferView") {
        if (! valueIndName(nvalue, image.bufferView))
          return errorMsg("Invalid images/" + name + " ind/name");
      }
      else if (name == "mimeType") {
        if (! valueString(nvalue, image.mimeType))
          return errorMsg("Invalid images/" + name + " string");
      }
      else if (name == "name") {
        if (! valueString(nvalue, image.name))
          return errorMsg("Invalid images/" + name + " string");
      }
      else if (name == "url") {
        if (! valueString(nvalue, image.url))
          return errorMsg("Invalid images/" + name + " string");
      }
      else if (name == "uri") {
        if (! valueString(nvalue, image.uri))
          return errorMsg("Invalid images/" + name + " string");
      }
      else if (name == "width") {
        if (! valueLong(nvalue, image.width))
          return errorMsg("Invalid images/" + name + " number");
      }
      else if (name == "height") {
        if (! valueLong(nvalue, image.height))
          return errorMsg("Invalid images/" + name + " number");
      }
      else if (name == "extensions") {
        if (! nvalue->isObject())
          return errorMsg("Invalid images/" + name);

        auto *obj2 = nvalue->cast<CJson::Object>();

        for (const auto &pnv3 : obj2->nameValueMap()) {
          if (pnv3.first == "KHR_binary_glTF") {
            if (! pnv3.second->isObject())
              return errorMsg("Invalid image " + name + "/" + pnv3.first);

            auto *obj3 = pnv3.second->cast<CJson::Object>();

            for (const auto &pnv4 : obj3->nameValueMap()) {
              if (! processNameValue1(pnv4.first, pnv4.second))
                return false;
            }
          }
          else {
            debugMsg("  " + id + "/" + name + "/" + pnv3.first);
          }
        }
      }
      else {
        debugMsg("  " + id + "/" + name + " : " + nvalue->typeName());
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

  auto readMaterial = [&](CJson::Object *jobj, Material &material, const std::string &id) {
    for (const auto &pnv2 : jobj->nameValueMap()) {
      if      (pnv2.first == "name") {
        if (! valueString(pnv2.second, material.name))
          return errorMsg("Invalid " + id + " name string");
      }
      else if (pnv2.first == "normalTexture") {
        if (! pnv2.second->isObject())
          return errorMsg("Invalid " + id + "/" + pnv2.first);

        auto *obj3 = pnv2.second->cast<CJson::Object>();

        readMaterialTexture(obj3, material.normalTexture, id + "/" + pnv2.first);
      }
      else if (pnv2.first == "pbrMetallicRoughness") {
        if (! pnv2.second->isObject())
          return errorMsg("Invalid " + id + "/" + pnv2.first);

        auto *obj3 = pnv2.second->cast<CJson::Object>();

        for (const auto &pnv3 : obj3->nameValueMap()) {
          if      (pnv3.first == "metallicFactor") {
            if (! valueOptNumber(pnv3.second, material.metallicFactor))
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                              pnv3.first + " number");
          }
          else if (pnv3.first == "roughnessFactor") {
            if (! valueOptNumber(pnv3.second, material.roughnessFactor))
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                              pnv3.first + " number");
          }
          else if (pnv3.first == "metallicRoughnessTexture") {
            if (! pnv3.second->isObject())
              return errorMsg("Invalid " + id + "/" + pnv3.first);

            auto *obj4 = pnv3.second->cast<CJson::Object>();

            readMaterialTexture(obj4, material.metallicRoughnessTexture,
                                id + "/" + pnv2.first + "/" + pnv3.first);
          }
          else if (pnv3.first == "baseColorFactor") {
            if (! valueOptColor(pnv3.second, material.baseColorFactor))
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                              pnv3.first + " color");
          }
          else if (pnv3.first == "baseColorTexture") {
            if (! pnv3.second->isObject())
              return errorMsg("Invalid " + id + " baseColorTexture");

            auto *obj4 = pnv3.second->cast<CJson::Object>();

            readMaterialTexture(obj4, material.baseColorTexture,
                                id + "/" + pnv2.first + "/" + pnv3.first);
          }
          else {
            TODOValue("Invalid " + id + "/" + pnv2.first + " name '" +
                      pnv3.first + "'", pnv3.second);
          }
        }
      }
      else if (pnv2.first == "technique") {
        if (! valueString(pnv2.second, material.technique))
          return errorMsg("Invalid " + id + "/technique number");
      }
      else if (pnv2.first == "values") {
        if (! pnv2.second->isObject())
          return errorMsg("Invalid " + id + "/values");

        auto *obj2 = pnv2.second->cast<CJson::Object>();

        for (const auto &pnv3 : obj2->nameValueMap()) {
          if      (pnv3.first == "ambient") {
            if (! valueOptColor(pnv3.second, material.ambient))
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                              pnv3.first + " color");
          }
          else if (pnv3.first == "diffuse") {
            if (! valueOptColor(pnv3.second, material.diffuse))
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                              pnv3.first + " color");
          }
          else if (pnv3.first == "emission") {
            if (! valueOptColor(pnv3.second, material.emission))
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                              pnv3.first + " color");
          }
          else if (pnv3.first == "shininess") {
            if (! valueOptNumber(pnv3.second, material.shininess))
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                              pnv3.first + "/ value");
          }
          else if (pnv3.first == "specular") {
            if (! valueOptColor(pnv3.second, material.specular))
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                              pnv3.first + " color");
          }
          else if (pnv3.first == "transparency") {
            if (! valueOptNumber(pnv3.second, material.transparency))
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                              pnv3.first + " value");
          }
          else {
            debugValueMsg("  " + id + "/" + pnv2.first + "/" + pnv3.first, pnv3.second);
          }
        }
      }
      else if (pnv2.first == "occlusionTexture") {
        if (! pnv2.second->isObject())
          return errorMsg("Invalid " + id + "/" + pnv2.first);

        auto *obj3 = pnv2.second->cast<CJson::Object>();

        readMaterialTexture(obj3, material.occlusionTexture, id + "/" + pnv2.first);
      }
      else if (pnv2.first == "alphaCutoff") {
        // TODO
        if (! valueOptNumber(pnv2.second, material.alphaCutoff))
          return errorMsg("Invalid " + id + "/alphaCutoff value");

        //TODOValue("Invalid " + id + "/alphaCutoff", pnv2.second);
      }
      else if (pnv2.first == "alphaMode") {
        // TODO
        if (! valueString(pnv2.second, material.alphaMode))
          return errorMsg("Invalid " + id + "/" + pnv2.first + " string");

        //TODOValue("Invalid " + id + "/alphaMode", pnv2.second);
      }
      else if (pnv2.first == "doubleSided") {
        if (! valueBool(pnv2.second, material.doubleSided))
          return errorMsg("Invalid " + id + "/" + pnv2.first + " value");
      }
      else if (pnv2.first == "emissiveFactor") {
        std::vector<float> rvalues;
        if (! arrayNumbers(pnv2.second, rvalues))
          return errorMsg("Invalid " + id + "/" + pnv2.first + " array");

        if (rvalues.size() != 3)
          return errorMsg("Invalid " + id + "/" + pnv2.first + " array size");

        material.emissiveFactor = Vec3(rvalues[0], rvalues[1], rvalues[2]);
      }
      else if (pnv2.first == "emissiveTexture") {
        if (! pnv2.second->isObject())
          return errorMsg("Invalid " + id + "/" + pnv2.first);

        auto *obj3 = pnv2.second->cast<CJson::Object>();

        readMaterialTexture(obj3, material.emissiveTexture, id + "/" + pnv2.first);
      }
      else if (pnv2.first == "extensions") {
        if (! pnv2.second->isObject())
          return errorMsg("Invalid " + id + "/" + pnv2.first + " values");

        auto *obj2 = pnv2.second->cast<CJson::Object>();

        for (const auto &pnv3 : obj2->nameValueMap()) {
          if      (pnv3.first == "KHR_materials_anisotropy") {
            if (! pnv3.second->isObject())
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                              pnv3.first + " values");

            auto *obj3 = pnv3.second->cast<CJson::Object>();

            for (const auto &pnv4 : obj3->nameValueMap()) {
              if      (pnv4.first == "anisotropyStrength") {
                if (! valueOptNumber(pnv4.second, material.anisotropyStrength))
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first + " value");
              }
              else if (pnv4.first == "anisotropyRotation") {
                if (! valueOptNumber(pnv4.second, material.anisotropyRotation))
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first + " value");
              }
              else if (pnv4.first == "anisotropyTexture") {
                auto *obj4 = pnv4.second->cast<CJson::Object>();

                readMaterialTexture(obj4, material.anisotropyTexture,
                                    id + "/" + pnv2.first + "/" +
                                    pnv3.first + "/" + pnv4.first);
              }
              else
                debugValueMsg("  " + id + "/" + pnv2.first + "/" + pnv3.first + "/" +
                              pnv4.first, pnv4.second);
            }
          }
          else if (pnv3.first == "KHR_materials_clearcoat") {
            if (! pnv3.second->isObject())
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                              pnv3.first + " values");

            auto *obj3 = pnv3.second->cast<CJson::Object>();

            for (const auto &pnv4 : obj3->nameValueMap()) {
              if      (pnv4.first == "clearcoatFactor") {
                if (! valueOptNumber(pnv4.second, material.clearcoatFactor))
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first + " value");
              }
              else if (pnv4.first == "clearcoatRoughnessFactor") {
                if (! valueOptNumber(pnv4.second, material.clearcoatRoughnessFactor))
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first + " value");
              }
              else if (pnv4.first == "clearcoatTexture") {
                if (! pnv4.second->isObject())
                  return errorMsg("Invalid " + id + "/" + pnv3.first);

                auto *obj4 = pnv4.second->cast<CJson::Object>();

                readMaterialTexture(obj4, material.clearcoatTexture,
                                    id + "/" + pnv2.first + "/" +
                                    pnv3.first + "/" + pnv4.first);
              }
              else if (pnv4.first == "clearcoatNormalTexture") {
                if (! pnv4.second->isObject())
                  return errorMsg("Invalid " + id + "/" + pnv3.first);

                auto *obj4 = pnv4.second->cast<CJson::Object>();

                readMaterialTexture(obj4, material.clearcoatNormalTexture,
                                    id + "/" + pnv2.first + "/" +
                                    pnv3.first + "/" + pnv4.first);
              }
              else if (pnv4.first == "clearcoatRoughnessTexture") {
                if (! pnv4.second->isObject())
                  return errorMsg("Invalid " + id + "/" + pnv3.first);

                auto *obj4 = pnv4.second->cast<CJson::Object>();

                readMaterialTexture(obj4, material.clearcoatRoughnessTexture,
                                    id + "/" + pnv2.first + "/" + pnv3.first);
              }
              else
                debugValueMsg("  " + id + "/" + pnv2.first + "/" + pnv3.first + "/" +
                              pnv4.first, pnv4.second);
            }
          }
          else if (pnv3.first == "KHR_materials_dispersion") {
            if (! pnv3.second->isObject())
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                              pnv3.first + " values");

            auto *obj3 = pnv3.second->cast<CJson::Object>();

            for (const auto &pnv4 : obj3->nameValueMap()) {
              if (pnv4.first == "dispersion") {
                if (! valueOptNumber(pnv4.second, material.dispersion))
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first + " value");
              }
              else
                debugValueMsg("  " + id + "/" + pnv2.first + "/" + pnv3.first + "/" +
                              pnv4.first, pnv4.second);
            }
          }
          else if (pnv3.first == "KHR_materials_ior") {
            if (! pnv3.second->isObject())
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                              pnv3.first + " values");

            auto *obj3 = pnv3.second->cast<CJson::Object>();

            for (const auto &pnv4 : obj3->nameValueMap()) {
              if (pnv4.first == "ior") {
                if (! valueOptNumber(pnv4.second, material.ior))
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first + " value");
              }
              else
                debugValueMsg("  " + id + "/" + pnv2.first + "/" + pnv3.first + "/" +
                              pnv4.first, pnv4.second);
            }
          }
          else if (pnv3.first == "KHR_materials_iridescence") {
            if (! pnv3.second->isObject())
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                              pnv3.first + " values");

            auto *obj3 = pnv3.second->cast<CJson::Object>();

            for (const auto &pnv4 : obj3->nameValueMap()) {
              if      (pnv4.first == "iridescenceFactor") {
                if (! valueOptNumber(pnv4.second, material.iridescenceFactor))
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first + " value");
              }
              else if (pnv4.first == "iridescenceIor") {
                if (! valueOptNumber(pnv4.second, material.iridescenceIor))
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first + " value");
              }
              else if (pnv4.first == "iridescenceTexture") {
                if (! pnv4.second->isObject())
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first);

                auto *obj5 = pnv4.second->cast<CJson::Object>();

                readMaterialTexture(obj5, material.iridescenceTexture,
                                    id + "/" + pnv2.first + "/" + pnv3.first + "/" +
                                    pnv4.first);
              }
              else if (pnv4.first == "iridescenceThicknessTexture") {
                if (! pnv4.second->isObject())
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first);

                auto *obj5 = pnv4.second->cast<CJson::Object>();

                readMaterialTexture(obj5, material.iridescenceThicknessTexture,
                                    id + "/" + pnv2.first + "/" + pnv3.first + "/" +
                                    pnv4.first);
              }
              else if (pnv4.first == "iridescenceThicknessMinimum") {
                if (! valueOptNumber(pnv4.second, material.iridescenceThicknessMinimum))
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first + " value");
              }
              else if (pnv4.first == "iridescenceThicknessMaximum") {
                if (! valueOptNumber(pnv4.second, material.iridescenceThicknessMaximum))
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first + " value");
              }
              else
                debugValueMsg("  " + id + "/" + pnv2.first + "/" + pnv3.first + "/" +
                              pnv4.first, pnv4.second);
            }
          }
          else if (pnv3.first == "KHR_materials_transmission") {
            if (! pnv3.second->isObject())
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                              pnv3.first + " values");

            auto *obj3 = pnv3.second->cast<CJson::Object>();

            for (const auto &pnv4 : obj3->nameValueMap()) {
              if      (pnv4.first == "transmissionFactor") {
                if (! valueOptNumber(pnv4.second, material.transmissionFactor))
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first + " number");
              }
              else if (pnv4.first == "transmissionTexture") {
                if (! pnv4.second->isObject())
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first);

                auto *obj5 = pnv4.second->cast<CJson::Object>();

                readMaterialTexture(obj5, material.transmissionTexture,
                                    id + "/" + pnv2.first + "/" + pnv3.first + "/" +
                                    pnv4.first);
              }
              else
                debugValueMsg("  " + id + "/" + pnv2.first + "/" + pnv3.first + "/" +
                              pnv4.first, pnv4.second);
            }
          }
          else if (pnv3.first == "KHR_materials_diffuse_transmission") {
            if (! pnv3.second->isObject())
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                              pnv3.first + " values");

            auto *obj3 = pnv3.second->cast<CJson::Object>();

            for (const auto &pnv4 : obj3->nameValueMap()) {
              if      (pnv4.first == "diffuseTransmissionFactor") {
                if (! valueOptNumber(pnv4.second, material.diffuseTransmissionFactor))
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first + " number");
              }
              else if (pnv4.first == "diffuseTransmissionTexture") {
                if (! pnv4.second->isObject())
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first);

                auto *obj5 = pnv4.second->cast<CJson::Object>();

                readMaterialTexture(obj5, material.diffuseTransmissionTexture,
                                    id + "/" + pnv2.first + "/" + pnv3.first + "/" +
                                    pnv4.first);
              }
              else if (pnv4.first == "diffuseTransmissionColorFactor") {
                if (! valueOptColor(pnv4.second, material.diffuseTransmissionColorFactor))
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first + " color");

              }
              else if (pnv4.first == "diffuseTransmissionColorTexture") {
                if (! pnv4.second->isObject())
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first);

                auto *obj5 = pnv4.second->cast<CJson::Object>();

                readMaterialTexture(obj5, material.diffuseTransmissionColorTexture,
                                    id + "/" + pnv2.first + "/" + pnv3.first + "/" +
                                    pnv4.first);
              }
              else
                debugValueMsg("  " + id + "/" + pnv2.first + "/" + pnv3.first + "/" +
                              pnv4.first, pnv4.second);
            }
          }
          else if (pnv3.first == "KHR_materials_volume") {
            if (! pnv3.second->isObject())
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                              pnv3.first + " values");

            auto *obj3 = pnv3.second->cast<CJson::Object>();

            for (const auto &pnv4 : obj3->nameValueMap()) {
              if      (pnv4.first == "attenuationColor") {
                if (! valueOptColor(pnv4.second, material.attenuationColor))
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first + " number");
              }
              else if (pnv4.first == "attenuationDistance") {
                if (! valueOptNumber(pnv4.second, material.attenuationDistance))
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first + " number");
              }
              else if (pnv4.first == "thicknessFactor") {
                if (! valueOptNumber(pnv4.second, material.thicknessFactor))
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first + " number");
              }
              else if (pnv4.first == "thicknessTexture") {
                if (! pnv4.second->isObject())
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first);

                auto *obj5 = pnv4.second->cast<CJson::Object>();

                readMaterialTexture(obj5, material.thicknessTexture,
                                    id + "/" + pnv2.first + "/" + pnv3.first + "/" +
                                    pnv4.first);
              }
              else
                debugValueMsg("  " + id + "/" + pnv2.first + "/" + pnv3.first + "/" +
                              pnv4.first, pnv4.second);
            }
          }
          else if (pnv3.first == "KHR_materials_ior") {
            TODOValue("Invalid " + id + "/" + pnv2.first + "/" + pnv3.first,
                      pnv3.second);
          }
          else if (pnv3.first == "KHR_materials_emissive_strength") {
            if (! pnv3.second->isObject())
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                              pnv3.first + " values");

            auto *obj3 = pnv3.second->cast<CJson::Object>();

            for (const auto &pnv4 : obj3->nameValueMap()) {
              if      (pnv4.first == "emissiveStrength") {
                if (! valueOptNumber(pnv4.second, material.emissiveStrength))
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first + " number");
              }
              else
                debugValueMsg("  " + id + "/" + pnv2.first + "/" + pnv3.first + "/" +
                              pnv4.first, pnv4.second);
            }
          }
          else if (pnv3.first == "KHR_materials_sheen") {
            if (! pnv3.second->isObject())
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                              pnv3.first + " values");

            auto *obj3 = pnv3.second->cast<CJson::Object>();

            for (const auto &pnv4 : obj3->nameValueMap()) {
              if      (pnv4.first == "sheenColorFactor") {
                if (! valueOptColor(pnv4.second, material.sheenColorFactor))
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first + " color");
              }
              else if (pnv4.first == "sheenRoughnessFactor") {
                if (! valueOptNumber(pnv4.second, material.sheenRoughnessFactor))
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first + " number");
              }
              else if (pnv4.first == "sheenColorTexture") {
                if (! pnv4.second->isObject())
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first);

                auto *obj5 = pnv4.second->cast<CJson::Object>();

                readMaterialTexture(obj5, material.sheenColorTexture,
                                    id + "/" + pnv2.first + "/" + pnv3.first + "/" +
                                    pnv4.first);
              }
              else if (pnv4.first == "sheenRoughnessTexture") {
                if (! pnv4.second->isObject())
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first);

                auto *obj5 = pnv4.second->cast<CJson::Object>();

                readMaterialTexture(obj5, material.sheenRoughnessTexture,
                                    id + "/" + pnv2.first + "/" + pnv3.first + "/" +
                                    pnv4.first);
              }
              else
                debugValueMsg("  " + id + "/" + pnv2.first + "/" + pnv3.first + "/" +
                              pnv4.first, pnv4.second);
            }
          }
          else if (pnv3.first == "KHR_materials_specular") {
            if (! pnv3.second->isObject())
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                              pnv3.first + " values");

            auto *obj3 = pnv3.second->cast<CJson::Object>();

            for (const auto &pnv4 : obj3->nameValueMap()) {
              if      (pnv4.first == "specularTexture" ||
                       pnv4.first == "specularColorTexture") {
                if (! pnv4.second->isObject())
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first);

                auto *obj5 = pnv4.second->cast<CJson::Object>();

                readMaterialTexture(obj5, material.specularTexture,
                                    id + "/" + pnv2.first + "/" + pnv3.first + "/" +
                                    pnv4.first);
              }
              else if (pnv4.first == "specularColorFactor") {
                if (! valueOptColor(pnv4.second, material.specularColorFactor))
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first + " color");
              }
              else if (pnv4.first == "specularFactor") {
                if (! valueOptNumber(pnv4.second, material.specularFactor))
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first + " number");
              }
              else
                debugValueMsg("  " + id + "/" + pnv2.first + "/" + pnv3.first + "/" +
                              pnv4.first, pnv4.second);
            }
          }
          else if (pnv3.first == "KHR_materials_pbrSpecularGlossiness") {
            if (! pnv3.second->isObject())
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                              pnv3.first + " values");

            auto *obj3 = pnv3.second->cast<CJson::Object>();

            for (const auto &pnv4 : obj3->nameValueMap()) {
              if      (pnv4.first == "diffuseTexture") {
                if (! pnv4.second->isObject())
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first);

                auto *obj5 = pnv4.second->cast<CJson::Object>();

                readMaterialTexture(obj5, material.diffuseTexture,
                                    id + "/" + pnv2.first + "/" + pnv3.first + "/" +
                                    pnv4.first);
              }
              else if (pnv4.first == "specularGlossinessTexture") {
                if (! pnv4.second->isObject())
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                                  pnv3.first + "/" + pnv4.first);

                auto *obj5 = pnv4.second->cast<CJson::Object>();

                readMaterialTexture(obj5, material.specularGlossinessTexture,
                                    id + "/" + pnv2.first + "/" + pnv3.first + "/" +
                                    pnv4.first);
              }
            }
          }
          else if (pnv3.first == "KHR_materials_unlit") {
            if (! pnv3.second->isObject())
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" +
                              pnv3.first + " values");

            auto *obj3 = pnv3.second->cast<CJson::Object>();

            for (const auto &pnv4 : obj3->nameValueMap()) {
              debugValueMsg("  " + id + "/" + pnv2.first + "/" + pnv3.first + "/" +
                            pnv4.first, pnv4.second);
            }
          }
          else
            debugValueMsg("  " + id + "/" + pnv2.first + "/" + pnv3.first, pnv3.second);
        }
      }
      else {
        warnMsg("Invalid " + id + " pnv3.first " + pnv2.first);
      }
    }

    return true;
  };

  // GL_LINEAR               =  0x2601 (9729)
  // GL_LINEAR_MIPMAP_LINEAR =  0x2703 (9987)
  auto readSampler = [&](CJson::Object *jobj, Sampler &sampler, const std::string &id) {
    for (const auto &pnv2 : jobj->nameValueMap()) {
      if      (pnv2.first == "name") {
        if (! valueString(pnv2.second, sampler.name))
          return errorMsg("Invalid " + id + "/" + pnv2.first + " string");
      }
      else if (pnv2.first == "magFilter") {
        if (! valueLong(pnv2.second, sampler.magFilter))
          return errorMsg("Invalid " + id + "/" + pnv2.first + " number");
      }
      else if (pnv2.first == "minFilter") {
        if (! valueLong(pnv2.second, sampler.minFilter))
          return errorMsg("Invalid " + id + "/" + pnv2.first + " number");
      }
      else if (pnv2.first == "wrapS") {
        if (! valueLong(pnv2.second, sampler.wrapS))
          return errorMsg("Invalid " + id + "/" + pnv2.first + " number");
      }
      else if (pnv2.first == "wrapT") {
        if (! valueLong(pnv2.second, sampler.wrapT))
          return errorMsg("Invalid " + id + "/" + pnv2.first + " number");
      }
      else {
        debugValueMsg("  " + id + "/" + pnv2.first, pnv2.second);
      }
    }

    return true;
  };

  auto readScene = [&](CJson::Object *jobj, Scene &scene, const std::string &id) {
    for (const auto &pnv2 : jobj->nameValueMap()) {
      if      (pnv2.first == "name") {
        if (! valueString(pnv2.second, scene.name))
          return errorMsg("Invalid " + id + "/" + pnv2.first + " string");
      }
      else if (pnv2.first == "nodes") {
        if (! pnv2.second->isArray())
          return errorMsg("Invalid JSON " + id + "/" + pnv2.first + " value");

        auto *arr2 = pnv2.second->cast<CJson::Array>();

        for (const auto &pv2 : arr2->values()) {
          IndName indName;
          if (! valueIndName(pv2, indName))
            return errorMsg("Invalid " + id + "/" + pnv2.first);

          scene.nodes.push_back(indName);
        }
      }
      else if (pnv2.first == "extensions") {
        if (! pnv2.second->isObject())
          return errorMsg("Invalid " + id + "/" + pnv2.first + " type");

        auto *obj3 = pnv2.second->cast<CJson::Object>();

        for (const auto &pnv3 : obj3->nameValueMap()) {
          debugValueMsg("Invalid " + id + "/" + pnv2.first + " name " +
                        pnv3.first, pnv3.second);
        }
      }
      else {
        debugValueMsg("  " + id + "/" + pnv2.first, pnv2.second);
      }
    }

    return true;
  };

  auto readTexture = [&](CJson::Object *jobj, Texture &texture, const std::string &id) {
    for (const auto &pnv2 : jobj->nameValueMap()) {
      if      (pnv2.first == "source") {
        if (! valueIndName(pnv2.second, texture.source))
          return errorMsg("Invalid " + id + "/" + pnv2.first + " ind/name");
      }
      else if (pnv2.first == "sampler") {
        if (! valueIndName(pnv2.second, texture.sampler))
          return errorMsg("Invalid " + id + "/" + pnv2.first + " ind/name");
      }
      else if (pnv2.first == "format") {
        if (! valueLong(pnv2.second, texture.format))
          return errorMsg("Invalid " + id + "/" + pnv2.first);
      }
      else if (pnv2.first == "internalFormat") {
        if (! valueLong(pnv2.second, texture.internalFormat))
          return errorMsg("Invalid " + id + "/" + pnv2.first);
      }
      else if (pnv2.first == "target") {
        if (! valueLong(pnv2.second, texture.target))
          return errorMsg("Invalid " + id + "/" + pnv2.first);
      }
      else if (pnv2.first == "type") {
        if (! valueLong(pnv2.second, texture.type))
          return errorMsg("Invalid " + id + "/" + pnv2.first);
      }
      else if (pnv2.first == "name") {
        if (! valueString(pnv2.second, texture.name))
          return errorMsg("Invalid " + id + "/" + pnv2.first);
      }
      else if (pnv2.first == "extensions") {
        if (! pnv2.second->isObject())
          return errorMsg("Invalid " + id + "/" + pnv2.first + " type");

        auto *obj3 = pnv2.second->cast<CJson::Object>();

        for (const auto &pnv3 : obj3->nameValueMap()) {
          if (pnv3.first == "EXT_texture_webp") {
            if (! pnv3.second->isObject())
              return errorMsg("Invalid " + id + "/" + pnv2.first + "/" + pnv3.first);

            auto *obj4 = pnv3.second->cast<CJson::Object>();

            for (const auto &pnv4 : obj4->nameValueMap()) {
              if (pnv4.first == "source") {
                if (! valueIndName(pnv4.second, texture.source))
                  return errorMsg("Invalid " + id + "/" + pnv2.first + "/" + pnv3.first +
                                  "/" + pnv4.first + " ind/name");
              }
              else {
                debugValueMsg("Invalid " + id + "/" + pnv2.first + "/" + pnv3.first +
                              " name " + pnv4.first, pnv4.second);
              }
            }
          }
          else {
            debugValueMsg("Invalid " + id + "/" + pnv2.first + " name " +
                          pnv3.first, pnv3.second);
          }
        }

        //debugValueMsg("Invalid " + id + "/" + pnv2.first, pnv2.second);
      }
      else {
        debugValueMsg("Invalid " + id + " name " + pnv2.first, pnv2.second);
      }
    }

    return true;
  };

  auto readAnimationChannel = [&](CJson::Object *jobj, AnimationChannel &channel,
                                  const std::string &id) {
    for (const auto &pnv3 : jobj->nameValueMap()) {
      if      (pnv3.first == "target") {
        auto id1 = id + "/" + pnv3.first;

        if (! pnv3.second->isObject())
          return errorMsg("Invalid JSON " + id1 + " data");

        auto *obj4 = pnv3.second->cast<CJson::Object>();

        for (const auto &pnv4 : obj4->nameValueMap()) {
          if      (pnv4.first == "node") {
            if (! valueIndName(pnv4.second, channel.node))
              return errorMsg("Invalid " + id1 + "/" + pnv4.first + " value");
          }
          else if (pnv4.first == "path") {
            if (! valueString(pnv4.second, channel.path))
              return errorMsg("Invalid " + id1 + "/" + pnv4.first + " value");
          }
          else if (pnv4.first == "extensions") {
            if (! pnv4.second->isObject())
              return errorMsg("Invalid JSON " + id1 + "/" + pnv4.first + " data");

            auto *obj5 = pnv4.second->cast<CJson::Object>();

            for (const auto &pnv5 : obj5->nameValueMap()) {
              auto id2 = id1 + "/" + pnv4.first;

              if (pnv5.first == "KHR_animation_pointer") {
                if (! pnv5.second->isObject())
                  return errorMsg("Invalid JSON " + id2 + " data");

                auto *obj6 = pnv5.second->cast<CJson::Object>();

                for (const auto &pnv6 : obj6->nameValueMap()) {
                  auto id3 = id2 + "/" + pnv5.first;

                  if (pnv6.first == "pointer") {
                    TODOValue("Unhandled " + id3 + "/" + pnv6.first, pnv6.second);
                  }
                  else
                    TODOValue("Unhandled " + id3 + "/" + pnv6.first, pnv6.second);
                }
              }
              else
                TODOValue("Unhandled " + id2 + "/" + pnv5.first, pnv5.second);
            }
          }
          else
            TODOValue("Unhandled " + id + " target name " + pnv4.first, pnv4.second);
        }
      }
      else if (pnv3.first == "sampler") {
        if (! valueLong(pnv3.second, channel.sampler))
          return errorMsg("Invalid " + id + "/" + pnv3.first + " value");
      }
      else
        TODOValue("Unhandled " + id + "/" + pnv3.first, pnv3.second);
    }

    return true;
  };

  auto readAsset = [&](CJson::Object *obj1, const std::string &id) {
    jsonData.asset.set = true;

    for (const auto &pnv1 : obj1->nameValueMap()) {
      if      (pnv1.first == "generator") {
        if (! valueString(pnv1.second, jsonData.asset.generator))
          return errorMsg("Invalid " + id + "/" + pnv1.first + " string");
      }
      else if (pnv1.first == "minVersion") {
        std::string minVersion;
        if (! valueString(pnv1.second, minVersion))
          return errorMsg("Invalid " + id + "/" + pnv1.first + " string");
      }
      else if (pnv1.first == "version") {
        if (pnv1.second->isString()) {
          if (! valueString(pnv1.second, jsonData.asset.versionId))
            return errorMsg("Invalid " + id + "/" + pnv1.first + " string");
        }
        else {
          if (! valueLong(pnv1.second, jsonData.asset.versionNumber))
            return errorMsg("Invalid " + id + "/" + pnv1.first + " number");
        }
      }
      else if (pnv1.first == "premultipliedAlpha") {
        // TODO : bool
        TODOValue("Unhandled " + id + "/" + pnv1.first, pnv1.second);
      }
      else if (pnv1.first == "profile") {
        // TODO : object
        TODOValue("Unhandled " + id + "/" + pnv1.first, pnv1.second);
      }
      else if (pnv1.first == "copyright") {
        if (! valueString(pnv1.second, jsonData.asset.copyright))
          return errorMsg("Invalid " + id + "/" + pnv1.first);
      }
      else if (pnv1.first == "extras") {
        //TODOValue("Unhandled " + id + "/" + pnv1.first, pnv1.second);

        if (! pnv1.second->isObject())
          return errorMsg("Invalid " + id + "/" + pnv1.first);

        auto *obj2 = pnv1.second->cast<CJson::Object>();

        for (const auto &pnv2 : obj2->nameValueMap()) {
          if      (pnv2.first == "title") {
            std::string title;
            if (! valueString(pnv2.second, title))
              return errorMsg("Invalid " + id + "/" + pnv1.first + "/" +
                              pnv2.first + " string");
          }
          else if (pnv2.first == "author") {
            std::string author;
            if (! valueString(pnv2.second, author))
              return errorMsg("Invalid " + id + "/" + pnv1.first + "/" +
                              pnv2.first + " string");
          }
          else if (pnv2.first == "license") {
            std::string license;
            if (! valueString(pnv2.second, license))
              return errorMsg("Invalid " + id + "/" + pnv1.first + "/" +
                              pnv2.first + " string");
          }
          else if (pnv2.first == "source") {
            std::string source;
            if (! valueString(pnv2.second, source))
              return errorMsg("Invalid " + id + "/" + pnv1.first + "/" +
                              pnv2.first + " string");
          }
          else {
            debugValueMsg("Invalid name " + id + "/" + pnv1.first + "/" +
                          pnv2.first, pnv2.second);
          }
        }
      }
      else if (pnv1.first == "extensions") {
        if (! pnv1.second->isObject())
          return errorMsg("Invalid " + id + "/" + pnv1.first);

        auto *obj2 = pnv1.second->cast<CJson::Object>();

        for (const auto &pnv2 : obj2->nameValueMap()) {
          if (pnv2.first == "KHR_xmp") {
            if (! pnv2.second->isObject())
              return errorMsg("Invalid " + id + "/" + pnv1.first + "/" +
                              pnv2.first + " type");

            auto *obj3 = pnv2.second->cast<CJson::Object>();

            for (const auto &pnv3 : obj3->nameValueMap()) {
              if (pnv3.first == "packet") {
                long packet;
                if (! valueLong(pnv3.second, packet))
                  return errorMsg("Invalid " + id + "/" + pnv1.first + "/" + pnv2.first +
                                  pnv3.first + " value");
              }
              else
                debugValueMsg(id + "/" + pnv1.first + "/" + pnv2.first + "/" +
                              pnv3.first, pnv3.second);
            }
          }
        }
      }
      else {
        debugValueMsg("  " + id + "/" + pnv1.first, pnv1.second);
      }
    }

    return true;
  };

  //---

  auto *obj = value->cast<CJson::Object>();

  for (const auto &pnv : obj->nameValueMap()) {
    if      (pnv.first == "accessors") {
      if      (pnv.second->isArray()) {
        auto *arr1 = pnv.second->cast<CJson::Array>();

        for (const auto &pv1 : arr1->values()) {
          if (! pv1->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " data");

          auto *obj1 = pv1->cast<CJson::Object>();

          Accessor accessor;

          if (! readAccessor(obj1, accessor, pnv.first))
            return false;

          jsonData.accessors.add(accessor, IndName(-1));
        }
      }
      else if (pnv.second->isObject()) {
        auto *obj1 = pnv.second->cast<CJson::Object>();

        for (const auto &pnv1 : obj1->nameValueMap()) {
          if (! pnv1.second->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " data");

          auto *obj2 = pnv1.second->cast<CJson::Object>();

          Accessor accessor;

          if (! readAccessor(obj2, accessor, pnv.first))
            return false;

          jsonData.accessors.add(accessor, IndName(pnv1.first));
        }
      }
      else {
        return errorMsg("Invalid JSON " + pnv.first + " data");
      }
    }
    else if (pnv.first == "asset") {
      if (! pnv.second->isObject())
        return errorMsg("Invalid JSON " + pnv.first + " data");

      auto *obj1 = pnv.second->cast<CJson::Object>();

      readAsset(obj1, pnv.first);
    }
    else if (pnv.first == "bufferViews") {
      if      (pnv.second->isArray()) {
        auto *arr1 = pnv.second->cast<CJson::Array>();

        for (const auto &pv1 : arr1->values()) {
          if (! pv1->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pv1->cast<CJson::Object>();

          BufferView bufferView;

          if (! readBufferView(obj2, bufferView, pnv.first))
            return false;

          jsonData.bufferViews.add(bufferView, IndName(-1));
        }
      }
      else if (pnv.second->isObject()) {
        auto *obj1 = pnv.second->cast<CJson::Object>();

        for (const auto &pnv1 : obj1->nameValueMap()) {
          if (! pnv1.second->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " data");

          auto *obj2 = pnv1.second->cast<CJson::Object>();

          BufferView bufferView;

          if (! readBufferView(obj2, bufferView, pnv.first))
            return false;

          jsonData.bufferViews.add(bufferView, IndName(pnv1.first));
        }
      }
      else {
        return errorMsg("Invalid JSON " + pnv.first + " data");
      }
    }
    else if (pnv.first == "buffers") {
      if      (pnv.second->isArray()) {
        auto *arr1 = pnv.second->cast<CJson::Array>();

        for (const auto &pv1 : arr1->values()) {
          if (! pv1->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pv1->cast<CJson::Object>();

          Buffer buffer;

          if (! readBuffer(obj2, buffer, pnv.first))
            return false;

          jsonData.buffers.add(buffer, IndName(-1));
        }
      }
      else if (pnv.second->isObject()) {
        auto *obj1 = pnv.second->cast<CJson::Object>();

        for (const auto &pnv1 : obj1->nameValueMap()) {
          if (! pnv1.second->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pnv1.second->cast<CJson::Object>();

          Buffer buffer;

          if (! readBuffer(obj2, buffer, pnv.first))
            return false;

          jsonData.buffers.add(buffer, IndName(pnv1.first));
        }
      }
      else {
        return errorMsg("Invalid JSON " + pnv.first + " data");
      }
    }
    else if (pnv.first == "images") {
      if      (pnv.second->isArray()) {
        auto *arr1 = pnv.second->cast<CJson::Array>();

        for (const auto &pv1 : arr1->values()) {
          if (! pv1->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pv1->cast<CJson::Object>();

          Image image;

          if (! readImage(obj2, image, pnv.first))
            return false;

          jsonData.images.add(image, IndName(-1));
        }
      }
      else if (pnv.second->isObject()) {
        auto *obj1 = pnv.second->cast<CJson::Object>();

        for (const auto &pnv1 : obj1->nameValueMap()) {
          if (! pnv1.second->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pnv1.second->cast<CJson::Object>();

          Image image;

          if (! readImage(obj2, image, pnv.first))
            return false;

          jsonData.images.add(image, IndName(pnv1.first));
        }
      }
      else {
        return errorMsg("Invalid JSON " + pnv.first + " data");
      }
    }
    else if (pnv.first == "materials") {
      if      (pnv.second->isArray()) {
        auto *arr1 = pnv.second->cast<CJson::Array>();

        for (const auto &pv1 : arr1->values()) {
          if (! pv1->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pv1->cast<CJson::Object>();

          Material material;

          if (! readMaterial(obj2, material, pnv.first))
            return false;

          jsonData.materials.add(material, IndName(-1));
        }
      }
      else if (pnv.second->isObject()) {
        auto *obj1 = pnv.second->cast<CJson::Object>();

        for (const auto &pnv1 : obj1->nameValueMap()) {
          if (! pnv1.second->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pnv1.second->cast<CJson::Object>();

          Material material;

          if (! readMaterial(obj2, material, pnv.first))
            return false;

          jsonData.materials.add(material, pnv1.first);
        }
      }
      else {
        return errorMsg("Invalid JSON " + pnv.first + " data");
      }
    }
    else if (pnv.first == "meshes") {
      if      (pnv.second->isArray()) {
        auto *arr1 = pnv.second->cast<CJson::Array>();

        for (const auto &pv1 : arr1->values()) {
          if (! pv1->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pv1->cast<CJson::Object>();

          Mesh mesh;

          if (! readMesh(obj2, mesh, pnv.first))
            return false;

          jsonData.meshes.add(mesh, IndName(-1));
        }
      }
      else if (pnv.second->isObject()) {
        auto *obj1 = pnv.second->cast<CJson::Object>();

        for (const auto &pnv1 : obj1->nameValueMap()) {
          if (! pnv1.second->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pnv1.second->cast<CJson::Object>();

          Mesh mesh;

          if (! readMesh(obj2, mesh, pnv.first))
            return false;

          jsonData.meshes.add(mesh, pnv1.first);
        }
      }
      else {
        return errorMsg("Invalid JSON " + pnv.first + " data");
      }
    }
    else if (pnv.first == "nodes") {
      if      (pnv.second->isArray()) {
        auto *arr1 = pnv.second->cast<CJson::Array>();

        for (const auto &pv1 : arr1->values()) {
          if (! pv1->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pv1->cast<CJson::Object>();

          Node node;
          if (! readNode(obj2, node, pnv.first))
            return false;

          jsonData.nodes.add(node, IndName(-1));
        }
      }
      else if (pnv.second->isObject()) {
        auto *obj1 = pnv.second->cast<CJson::Object>();

        for (const auto &pnv1 : obj1->nameValueMap()) {
          if (! pnv1.second->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pnv1.second->cast<CJson::Object>();

          Node node;
          if (! readNode(obj2, node, pnv.first))
            return false;

          jsonData.nodes.add(node, pnv1.first);
        }
      }
      else {
        return errorMsg("Invalid JSON " + pnv.first + " data");
      }
    }
    else if (pnv.first == "samplers") {
      if      (pnv.second->isArray()) {
        auto *arr1 = pnv.second->cast<CJson::Array>();

        for (const auto &pv1 : arr1->values()) {
          if (! pv1->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pv1->cast<CJson::Object>();

          Sampler sampler;

          if (! readSampler(obj2, sampler, pnv.first))
            return false;

          jsonData.samplers.add(sampler, IndName(-1));
        }
      }
      else if (pnv.second->isObject()) {
        auto *obj1 = pnv.second->cast<CJson::Object>();

        for (const auto &pnv1 : obj1->nameValueMap()) {
          if (! pnv1.second->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pnv1.second->cast<CJson::Object>();

          Sampler sampler;

          if (! readSampler(obj2, sampler, pnv.first))
            return false;

          jsonData.samplers.add(sampler, IndName(pnv1.first));
        }
      }
      else {
        return errorMsg("Invalid JSON " + pnv.first + " data");
      }
    }
    else if (pnv.first == "scene") {
      if (! valueIndName(pnv.second, jsonData.scene))
        return errorMsg("Invalid " + pnv.first + " ind/name");
    }
    else if (pnv.first == "scenes") {
      if      (pnv.second->isArray()) {
        auto *arr1 = pnv.second->cast<CJson::Array>();

        for (const auto &pv1 : arr1->values()) {
          if (! pv1->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pv1->cast<CJson::Object>();

          Scene scene;

          if (! readScene(obj2, scene, pnv.first))
            return false;

          jsonData.scenes.add(scene, IndName(-1));
        }
      }
      else if (pnv.second->isObject()) {
        auto *obj1 = pnv.second->cast<CJson::Object>();

        for (const auto &pnv1 : obj1->nameValueMap()) {
          if (! pnv1.second->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pnv1.second->cast<CJson::Object>();

          Scene scene;

          if (! readScene(obj2, scene, pnv.first))
            return false;

          jsonData.scenes.add(scene, pnv1.first);
        }
      }
      else {
        return errorMsg("Invalid JSON " + pnv.first + " data");
      }
    }
    else if (pnv.first == "textures") {
      if      (pnv.second->isArray()) {
        auto *arr1 = pnv.second->cast<CJson::Array>();

        for (const auto &pv1 : arr1->values()) {
          if (! pv1->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pv1->cast<CJson::Object>();

          Texture texture;

          if (! readTexture(obj2, texture, pnv.first))
            return false;

          jsonData.textures.add(texture, IndName(-1));
        }
      }
      else if (pnv.second->isObject()) {
        auto *obj1 = pnv.second->cast<CJson::Object>();

        for (const auto &pnv1 : obj1->nameValueMap()) {
          if (! pnv1.second->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pnv1.second->cast<CJson::Object>();

          Texture texture;

          if (! readTexture(obj2, texture, pnv.first))
            return false;

          jsonData.textures.add(texture, pnv1.first);
        }
      }
      else {
        return errorMsg("Invalid JSON " + pnv.first + " data");
      }
    }
    else if (pnv.first == "animations") {
      if (pnv.second->isArray()) {
        auto *arr1 = pnv.second->cast<CJson::Array>();

        for (const auto &pv1 : arr1->values()) {
          Animation animation;

          if (! pv1->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pv1->cast<CJson::Object>();

          for (const auto &pnv2 : obj2->nameValueMap()) {
            if      (pnv2.first == "name") {
              if (! valueString(pnv2.second, animation.name))
                return errorMsg("Invalid " + pnv.first + "/" + pnv2.first);
            }
            else if (pnv2.first == "channels") {
              if (! pnv2.second->isArray())
                return errorMsg("Invalid JSON " + pnv.first + "/" + pnv2.first + " data");

              auto *arr3 = pnv2.second->cast<CJson::Array>();

              Animation::Channels channels;

              for (const auto &pv3 : arr3->values()) {
                AnimationChannel channel;

                if (! pv3->isObject())
                  return errorMsg("Invalid JSON " + pnv.first + "/" + pnv2.first + " array data");

                auto *obj3 = pv3->cast<CJson::Object>();

                readAnimationChannel(obj3, channel, pnv.first + "/" + pnv2.first);

                channels.push_back(channel);
              }

              animation.channelsArray.push_back(channels);
            }
            else if (pnv2.first == "samplers") {
              if (! pnv2.second->isArray())
                return errorMsg("Invalid JSON " + pnv.first + "/" + pnv2.first + " data");

              auto *arr3 = pnv2.second->cast<CJson::Array>();

              for (const auto &pv3 : arr3->values()) {
                AnimationSampler sampler;

                if (! pv3->isObject())
                  return errorMsg("Invalid JSON " + pnv.first + "/" + pnv2.first + " array data");

                auto *obj3 = pv3->cast<CJson::Object>();

                for (const auto &pnv3 : obj3->nameValueMap()) {
                  if      (pnv3.first == "input") {
                    if (! valueLong(pnv3.second, sampler.input))
                      return errorMsg("Invalid " + pnv2.first + "/" + pnv3.first + " value");
                  }
                  else if (pnv3.first == "output") {
                    if (! valueLong(pnv3.second, sampler.output))
                      return errorMsg("Invalid " + pnv2.first + "/" + pnv3.first + " value");
                  }
                  else if (pnv3.first == "interpolation") {
                    if (! valueString(pnv3.second, sampler.interpolation))
                      return errorMsg("Invalid " + pnv2.first + "/" + pnv3.first + " value");

                    if      (sampler.interpolation == "LINEAR")
                      sampler.interpType = AnimationInterpolation::LINEAR;
                    else if (sampler.interpolation == "STEP")
                      sampler.interpType = AnimationInterpolation::STEP;
                    else if (sampler.interpolation == "CUBICSPLINE")
                      sampler.interpType = AnimationInterpolation::CUBICSPLINE;
                    else
                      return errorMsg("Invalid " + pnv2.first + "/" + pnv3.first + " value '" +
                                      sampler.interpolation + "'");
                  }
                  else
                    TODOValue("Unhandled " + pnv.first + "/" + pnv2.first + " name " +
                              pnv3.first, pnv3.second);
                }

                animation.samplers.add(sampler, IndName(-1));
              }
            }
            else
              TODOValue("Unhandled " + pnv.first + " name " + pnv2.first, pnv2.second);
          }

          if (animation.name == "")
            animation.name = "anim." + std::to_string(jsonData.animations.size() + 1);

          jsonData.animations.push_back(animation);
        }
      }
      else if (pnv.second->isObject()) {
        (void) errorMsg("Invalid JSON " + pnv.first + " array data");
      }
      else {
        (void) errorMsg("Invalid JSON " + pnv.first + " data");
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
            if      (pnv2.first == "name") {
              if (! valueString(pnv2.second, camera.name))
                return errorMsg("Invalid " + pnv.first + " name");
            }
            else if (pnv2.first == "type") {
              if (! valueString(pnv2.second, camera.type))
                return errorMsg("Invalid " + pnv.first + " type");
            }
            else if (pnv2.first == "perspective") {
              if (! pnv2.second->isObject())
                return errorMsg("Invalid " + pnv.first + "/" + pnv2.first);

              auto *obj3 = pnv2.second->cast<CJson::Object>();

              for (const auto &pnv3 : obj3->nameValueMap()) {
                if      (pnv3.first == "yfov") {
                  if (! valueNumber(pnv3.second, camera.yfov))
                    return errorMsg("Invalid " + pnv.first + "/" + pnv3.first);
                }
                else if (pnv3.first == "znear") {
                  if (! valueNumber(pnv3.second, camera.znear))
                    return errorMsg("Invalid " + pnv.first + "/" + pnv3.first);
                }
                else if (pnv3.first == "zfar") {
                  if (! valueNumber(pnv3.second, camera.zfar))
                    return errorMsg("Invalid " + pnv.first + "/" + pnv3.first);
                }
                else if (pnv3.first == "aspectRatio") {
                  if (! valueNumber(pnv3.second, camera.aspectRatio))
                    return errorMsg("Invalid " + pnv.first + "/" + pnv3.first);
                }
                else
                  TODOValue("Unhandled " + pnv.first + "/" + pnv2.first + "/" +
                            pnv3.first, pnv3.second);
              }
            }
            else if (pnv2.first == "orthographic") {
              if (! pnv2.second->isObject())
                return errorMsg("Invalid " + pnv.first + "/" + pnv2.first);

              auto *obj3 = pnv2.second->cast<CJson::Object>();

              for (const auto &pnv3 : obj3->nameValueMap()) {
                if      (pnv3.first == "xmag") {
                  if (! valueNumber(pnv3.second, camera.xmag))
                    return errorMsg("Invalid " + pnv.first + "/" + pnv3.first);
                }
                else if (pnv3.first == "ymag") {
                  if (! valueNumber(pnv3.second, camera.ymag))
                    return errorMsg("Invalid " + pnv.first + "/" + pnv3.first);
                }
                else if (pnv3.first == "znear") {
                  if (! valueNumber(pnv3.second, camera.znear))
                    return errorMsg("Invalid " + pnv.first + "/" + pnv3.first);
                }
                else if (pnv3.first == "zfar") {
                  if (! valueNumber(pnv3.second, camera.zfar))
                    return errorMsg("Invalid " + pnv.first + "/" + pnv3.first);
                }
                else
                  TODOValue("Unhandled " + pnv.first + "/" + pnv2.first + "/" +
                            pnv3.first, pnv3.second);
              }
            }
            else
              TODOValue("Unhandled " + pnv.first + " name " + pnv2.first, pnv2.second);
          }

          jsonData.cameras.add(camera, IndName(ind++));
        }
      }
      else if (pnv.second->isObject()) {
        (void) errorMsg("Invalid JSON " + pnv.first + " array data");
      }
      else {
        (void) errorMsg("Invalid JSON " + pnv.first + " data");
      }
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
      if (pnv.second->isArray()) {
        auto *arr1 = pnv.second->cast<CJson::Array>();

        int ind = 0;

        for (const auto &pv1 : arr1->values()) {
          Skin skin;

          if (! pv1->isObject())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          auto *obj2 = pv1->cast<CJson::Object>();

          for (const auto &pnv2 : obj2->nameValueMap()) {
            if      (pnv2.first == "inverseBindMatrices") {
              if (! valueLong(pnv2.second, skin.inverseBindMatrices))
                return errorMsg("Invalid sampler output value");
            }
            else if (pnv2.first == "joints") {
              if (! pnv2.second->isArray())
                return errorMsg("Invalid JSON " + pnv.first + "/" + pnv2.first + " array data");

              auto *arr3 = pnv2.second->cast<CJson::Array>();

              for (const auto &pv3 : arr3->values()) {
                long ijoint;
                if (! valueLong(pv3, ijoint))
                  return errorMsg("Invalid " + pnv.first + "/" + pnv2.first + " array value");

                skin.joints.push_back(ijoint);
              }
            }
            else if (pnv2.first == "name") {
              if (! valueString(pnv2.second, skin.name))
                return errorMsg("Invalid " + pnv.first + "/" + pnv2.first + " string");
            }
            else if (pnv2.first == "skeleton") {
              long iroot;
              if (! valueLong(pnv2.second, iroot))
                return errorMsg("Invalid " + pnv.first + "/" + pnv2.first + " number");
            }
            else
              debugValueMsg(pnv.first + "/" + pnv2.first, pnv2.second);
          }

          jsonData.skins.add(skin, IndName(ind++));
        }
      }
      else if (pnv.second->isObject()) {
        (void) errorMsg("Invalid JSON " + pnv.first + " array data");
      }
      else {
        (void) errorMsg("Invalid JSON " + pnv.first + " data");
      }
    }
    else if (pnv.first == "techniques") {
      // TODO
      TODO("Unhandled techniques");
      if (pnv.second->isObject()) {
        auto *obj1 = pnv.second->cast<CJson::Object>();

        for (const auto &pnv1 : obj1->nameValueMap()) {
          debugValueMsg(pnv.first + "/" + pnv1.first, pnv1.second);
        }
      }
      else if (pnv.second->isArray()) {
      }
      else
        return errorMsg("Invalid JSON " + pnv.first + " array data");
    }
    else if (pnv.first == "extensions") {
      if (pnv.second->isObject()) {
        auto *obj1 = pnv.second->cast<CJson::Object>();

        for (const auto &pnv1 : obj1->nameValueMap()) {
          if      (pnv1.first == "KHR_lights_punctual") {
            if (! pnv1.second->isObject())
              return errorMsg("Invalid " + pnv.first + "/" + pnv1.first + " type");

            auto *obj2 = pnv1.second->cast<CJson::Object>();

            for (const auto &pnv2 : obj2->nameValueMap()) {
              if (pnv2.first == "lights") {
                if (! pnv2.second->isArray())
                  return errorMsg("Invalid " + pnv.first + "/" + pnv2.first);

                auto *arr3 = pnv2.second->cast<CJson::Array>();

                for (const auto &pv3 : arr3->values()) {
                  if (! pv3->isObject())
                    return errorMsg("Invalid JSON " + pnv.first + "/" + pnv2.first +
                                    " array data");

                  auto *obj4 = pv3->cast<CJson::Object>();

                  for (const auto &pnv4 : obj4->nameValueMap()) {
                    if      (pnv4.first == "color") {
                      OptColor color;
                      if (! valueOptColor(pnv4.second, color))
                        return errorMsg("Invalid " + pnv.first + "/" + pnv2.first + "/" +
                                        pnv4.first + " color");
                    }
                    else if (pnv4.first == "intensity") {
                      OptReal real;
                      if (! valueOptNumber(pnv4.second, real))
                        return errorMsg("Invalid " + pnv.first + "/" + pnv2.first + "/" +
                                        pnv4.first + " number");
                    }
                    else if (pnv4.first == "name") {
                      std::string name;
                      if (! valueString(pnv4.second, name))
                        return errorMsg("Invalid " + pnv.first + "/" + pnv2.first + "/" +
                                        pnv4.first + " string");
                    }
                    else if (pnv4.first == "type") {
                      std::string type;
                      if (! valueString(pnv4.second, type))
                        return errorMsg("Invalid " + pnv.first + "/" + pnv2.first + "/" +
                                        pnv4.first + " string");
                    }
                    else
                      debugValueMsg("Invalid name " + pnv.first + "/" + pnv2.first + "/" +
                                    pnv4.first, pnv4.second);
                  }
                }
              }
              else
                debugValueMsg(pnv.first + "/" + pnv1.first + "/" + pnv2.first, pnv2.second);
            }
          }
          else if (pnv1.first == "KHR_materials_variants") {
            if (! pnv1.second->isObject())
              return errorMsg("Invalid " + pnv.first + "/" + pnv1.first + " type");

            auto *obj2 = pnv1.second->cast<CJson::Object>();

            for (const auto &pnv2 : obj2->nameValueMap()) {
              if (pnv2.first == "variants") {
                if (! pnv2.second->isArray())
                  return errorMsg("Invalid " + pnv.first + "/" + pnv2.first);

                auto *arr3 = pnv2.second->cast<CJson::Array>();

                for (const auto &pv3 : arr3->values()) {
                  if (! pv3->isObject())
                    return errorMsg("Invalid JSON " + pnv.first + "/" + pnv2.first + " type");

                  auto *obj4 = pv3->cast<CJson::Object>();

                  for (const auto &pnv4 : obj4->nameValueMap()) {
                    if (pnv4.first == "name") {
                      std::string name;
                      if (! valueString(pnv4.second, name))
                        return errorMsg("Invalid " + pnv.first + "/" + pnv2.first + "/" +
                                        pnv4.first + " string");
                    }
                    else
                      debugValueMsg("Invalid name " + pnv.first + "/" + pnv2.first + "/" +
                                    pnv4.first, pnv4.second);
                  }
                }
              }
              else
                debugValueMsg(pnv.first + "/" + pnv1.first + "/" + pnv2.first, pnv2.second);
            }
          }
          else if (pnv1.first == "KHR_xmp") {
            if (! pnv1.second->isObject())
              return errorMsg("Invalid " + pnv.first + "/" + pnv1.first + " type");

            auto *obj2 = pnv1.second->cast<CJson::Object>();

            for (const auto &pnv2 : obj2->nameValueMap()) {
              if      (pnv2.first == "@context") {
                if (! pnv2.second->isObject())
                  return errorMsg("Invalid JSON " + pnv.first + "/" + pnv1.first + "/" +
                                  pnv2.first + " type");

                auto *obj3 = pnv2.second->cast<CJson::Object>();

                for (const auto &pnv3 : obj3->nameValueMap()) {
                  if (pnv3.first == "dc") {
                    std::string name;
                    if (! valueString(pnv3.second, name))
                      return errorMsg("Invalid " + pnv.first + "/" + pnv2.first + "/" +
                                      pnv3.first + " string");
                  }
                  else
                    debugValueMsg(pnv.first + "/" + pnv1.first + "/" + pnv2.first + "/" +
                                  pnv3.first, pnv3.second);
                }

              }
              else if (pnv2.first == "packets") {
                if (! pnv2.second->isArray())
                  return errorMsg("Invalid " + pnv.first + "/" + pnv2.first);

                auto *arr3 = pnv2.second->cast<CJson::Array>();

                for (const auto &pv3 : arr3->values()) {
                  if (! pv3->isObject())
                    return errorMsg("Invalid JSON " + pnv.first + "/" + pnv2.first + " type");

                  auto *obj4 = pv3->cast<CJson::Object>();

                  for (const auto &pnv4 : obj4->nameValueMap()) {
                    if      (pnv4.first == "dc:date") {
                      std::string name;
                      if (! valueString(pnv4.second, name))
                        return errorMsg("Invalid " + pnv.first + "/" + pnv2.first + "/" +
                                        pnv4.first + " string");
                    }
                    else if (pnv4.first == "dc:title") {
                      std::string name;
                      if (! valueString(pnv4.second, name))
                        return errorMsg("Invalid " + pnv.first + "/" + pnv2.first + "/" +
                                        pnv4.first + " string");
                    }
                    else if (pnv4.first == "xmp:CreatorTool") {
                      std::string name;
                      if (! valueString(pnv4.second, name))
                        return errorMsg("Invalid " + pnv.first + "/" + pnv2.first + "/" +
                                        pnv4.first + " string");
                    }
                    else
                      debugValueMsg(pnv.first + "/" + pnv1.first + "/" + pnv2.first + "/" +
                                    pnv4.first, pnv4.second);
                  }
                }
              }
              else
                debugValueMsg(pnv.first + "/" + pnv1.first + "/" + pnv2.first, pnv2.second);
            }
          }
          else if (pnv1.first == "KHR_xmp_json_ld") {
            debugValueMsg(pnv.first + "/" + pnv1.first, pnv1.second);
          }
          else
            debugValueMsg(pnv.first + "/" + pnv1.first, pnv1.second);
        }
      }
      else if (pnv.second->isArray()) {
        TODOValue("Unhandled " + pnv.first, pnv.second);
      }
      else
        return errorMsg("Invalid JSON " + pnv.first + " array data");
    }
    else if (pnv.first == "extensionsUsed") {
      //TODOValue("Unhandled extensionsUsed", pnv.second);
      if (pnv.second->isArray()) {
        auto *arr1 = pnv.second->cast<CJson::Array>();

        for (const auto &pv1 : arr1->values()) {
          if (! pv1->isString())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          std::string name;
          if (! valueString(pv1, name))
            return errorMsg("Invalid " + pnv.first + " name");

          jsonData.extensionsUsed.push_back(name);
        }
      }
      else
        return errorMsg("Invalid JSON " + pnv.first + " array data");
    }
    else if (pnv.first == "extensionsRequired") {
      //TODOValue("Unhandled extensionsRequired", pnv.second);
      if (pnv.second->isArray()) {
        auto *arr1 = pnv.second->cast<CJson::Array>();

        for (const auto &pv1 : arr1->values()) {
          if (! pv1->isString())
            return errorMsg("Invalid JSON " + pnv.first + " array data");

          std::string name;
          if (! valueString(pv1, name))
            return errorMsg("Invalid " + pnv.first + " name");

          jsonData.extensionsUsed.push_back(name);
        }
      }
      else
        return errorMsg("Invalid JSON " + pnv.first + " array data");
    }
    else {
      debugValueMsg(pnv.first, pnv.second);

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

  //---

  if (! jsonData.skins.isEmpty()) {
    std::cerr << "skins\n";

    for (const auto &ps : jsonData.skins)
      printSkin(ps.second, ps.first);
  }
}

void
CImportGLTF::
printMeshData(const MeshData &meshData) const
{
  std::cerr << "Id: " << meshData.indName << ", ";
  std::cerr << "Type: " << meshData.type << ", ";
  std::cerr << "Component: " << meshData.componentType << ", ";

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
    std::cerr << "VEC4 (#" << meshData.vec4.size() << ") :";
    for (const auto &v : meshData.vec4) {
      std::cerr << " {" << v.x << "," << v.y << "," << v.z << "," << v.w << "}";
    }
  }
  else if (meshData.vec3.size() > 0) {
    std::cerr << "VEC3 (#" << meshData.vec3.size() << ") :";
    for (const auto &v : meshData.vec3) {
      std::cerr << " {" << v.x << "," << v.y << "," << v.z << "}";
    }
  }
  else if (meshData.vec2.size() > 0) {
    std::cerr << "VEC2 (#" << meshData.vec2.size() << ") :";
    for (const auto &v : meshData.vec2) {
      std::cerr << " {" << v.x << "," << v.y << "}";
    }
  }
  else if (meshData.iscalars.size() > 0) {
    std::cerr << "SCALAR (I) (#" << meshData.iscalars.size() << ") :";
    for (const auto &s : meshData.iscalars) {
      std::cerr << " " << s;
    }
  }
  else if (meshData.fscalars.size() > 0) {
    std::cerr << "SCALAR (F) (#" << meshData.fscalars.size() << ") :";
    for (const auto &s : meshData.fscalars) {
      std::cerr << " " << s;
    }
  }
  else if (meshData.mat4.size() > 0) {
    std::cerr << "MAT4 (#" << meshData.mat4.size() << ") :";
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
  auto printOptValue = [](const std::string &name, const auto &value) {
    if (value)
      std::cerr << "  " << name << ": " << value.value() << "\n";
  };

  std::cerr << " material[" << indName << "]\n";

  std::cerr << "  name: " << material.name << "\n";

  if (material.technique != "")
    std::cerr << "  technique: " << material.technique << "\n";

  printOptValue("metallicFactor" , material.metallicFactor);
  printOptValue("roughnessFactor", material.roughnessFactor);

  auto printTexture = [&](const MaterialTexture &texture, const std::string &id) {
    if (texture.index < 0) return;

    std::cerr << "  " << id << "\n";
    std::cerr << "   index: " << texture.index << "\n";

    if (texture.texCoord)
      std::cerr << "   texCoord: " << texture.texCoord.value() << "\n";
    if (texture.scale)
      std::cerr << "   scale: " << texture.scale.value() << "\n";
    if (texture.strength)
      std::cerr << "   strength: " << texture.strength.value() << "\n";
    if (texture.rotation)
      std::cerr << "   rotation: " << texture.rotation.value() << "\n";
    if (texture.scaleVector)
      std::cerr << "   scaleVector: " << texture.scaleVector.value() << "\n";
    if (texture.offsetVector)
      std::cerr << "   offsetVector: " << texture.offsetVector.value() << "\n";
  };

  printTexture(material.normalTexture           , "normalTexture");
  printTexture(material.occlusionTexture        , "occlusionTexture");
  printTexture(material.metallicRoughnessTexture, "metallicRoughnessTexture");

  printOptValue("ambient"     , material.ambient);
  printOptValue("diffuse"     , material.diffuse);
  printOptValue("shininess"   , material.shininess);
  printOptValue("transparency", material.transparency);

  printTexture(material.baseColorTexture, "baseColorTexture");
  printOptValue("baseColorFactor", material.baseColorFactor);

  printTexture(material.specularTexture, "specularTexture");
  printOptValue("specularFactor"     , material.specularFactor);
  printOptValue("specular"           , material.specular);
  printOptValue("specularColorFactor", material.specularColorFactor);

  printTexture(material.emissiveTexture, "emissiveTexture");
  printOptValue("emissiveStrength", material.emissiveStrength);
  printOptValue("emissiveFactor"  , material.emissiveFactor);
  printOptValue("emission"        , material.emission);

  printOptValue("transmissionFactor", material.transmissionFactor);
  printTexture(material.transmissionTexture, "transmissionTexture");

  printOptValue("attenuationColor", material.attenuationColor);
  printOptValue("attenuationDistance", material.attenuationDistance);

  printOptValue("thicknessFactor", material.thicknessFactor);
  printTexture(material.thicknessTexture, "thicknessTexture");

  if (material.doubleSided)
    std::cerr << "  doubleSided: true\n";

  if (material.alphaMode != "")
    std::cerr << "  alphaMode: " << material.alphaMode << "\n";

  printOptValue("alphaCutoff", material.alphaCutoff);

  printOptValue("diffuseTransmissionFactor", material.diffuseTransmissionFactor);
  printTexture(material.diffuseTransmissionTexture, "diffuseTransmissionTexture");
  printOptValue("diffuseTransmissionColorFactor", material.diffuseTransmissionColorFactor);
  printTexture(material.diffuseTransmissionColorTexture, "diffuseTransmissionColorTexture");

  // anisotropy
  printOptValue("anisotropyStrength", material.anisotropyStrength);
  printOptValue("anisotropyRotation", material.anisotropyRotation);
  printTexture(material.anisotropyTexture, "anisotropyTexture");

  // clearcoat
  printOptValue("clearcoatFactor", material.clearcoatFactor);
  printOptValue("clearcoatRoughnessFactor", material.clearcoatRoughnessFactor);
  printTexture(material.clearcoatTexture, "clearcoatTexture");
  printTexture(material.clearcoatNormalTexture, "clearcoatNormalTexture");
  printTexture(material.clearcoatRoughnessTexture, "clearcoatRoughnessTexture");

  // sheen
  printOptValue("sheenColorFactor", material.sheenColorFactor);
  printTexture(material.sheenColorTexture, "sheenColorTexture");
  printOptValue("sheenRoughnessFactor", material.sheenRoughnessFactor);
  printTexture(material.sheenRoughnessTexture, "sheenRoughnessTexture");

  // iridescence
  printOptValue("iridescenceFactor", material.iridescenceFactor);
  printOptValue("iridescenceIor", material.iridescenceIor);
  printTexture(material.iridescenceTexture, "iridescenceTexture");
  printTexture(material.iridescenceThicknessTexture, "iridescenceThicknessTexture");
  printOptValue("iridescenceThicknessMinimum", material.iridescenceThicknessMinimum);
  printOptValue("iridescenceThicknessMaximum", material.iridescenceThicknessMaximum);

  printOptValue("dispersion", material.dispersion);
  printOptValue("ior", material.ior);

  printTexture(material.diffuseTexture, "diffuseTexture");
  printTexture(material.specularGlossinessTexture, "specularGlossinessTexture");
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

  if (node.name != "")
    std::cerr << "  name: " << node.name << "\n";
  if (! node.mesh.isEmpty())
    std::cerr << "  mesh: " << node.mesh << "\n";
  if (node.matrix)
    std::cerr << "  matrix: " << *node.matrix << "\n";
  if (! node.skin.isEmpty())
    std::cerr << "  skin: " << node.skin << "\n";
  if (node.rotation)
    std::cerr << "  rotation: " << *node.rotation << "\n";
  if (node.scale)
    std::cerr << "  scale: " << *node.scale << "\n";
  if (node.translation)
    std::cerr << "  translation: " << *node.translation << "\n";
  if (node.camera >= 0)
    std::cerr << "  camera: " << node.camera << "\n";

  if (! node.children.empty()) {
    std::cerr << "  children: ";
    for (const auto &c : node.children)
      std::cerr << "  " << c;
    std::cerr << "\n";
  }
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
  if (! primitive.texCoord2.isEmpty())
    std::cerr << "    TEXCOORD_2: " << primitive.texCoord2 << "\n";
  if (! primitive.material.isEmpty())
    std::cerr << "    material: " << primitive.material << "\n";
  if (! primitive.color0.isEmpty())
    std::cerr << "    COLOR_0: " << primitive.color0 << "\n";
  if (! primitive.joints0.isEmpty())
    std::cerr << "    JOINTS_0: " << primitive.joints0 << "\n";
  if (! primitive.weights0.isEmpty())
    std::cerr << "    WEIGHTS_0: " << primitive.weights0 << "\n";
  if (! primitive.tangent.isEmpty())
    std::cerr << "    TANGENT: " << primitive.tangent << "\n";
}

void
CImportGLTF::
printSkin(const Skin &skin, const IndName &indName) const
{
  std::cerr << " skin[" << indName << "]\n";

  std::cerr << "  inverseBindMatrices: Mesh #" << skin.inverseBindMatrices << "\n";

  if (isDump()) {
    auto *meshData = getMeshData(skin.inverseBindMatrices);

    if (meshData) {
      std::cerr << "  Inverse Bind: ";
      printMeshData(*meshData);
    }
  }

  if (! skin.joints.empty()) {
    std::cerr << "  joints (#" << skin.joints.size() << ") :";
    for (auto i : skin.joints)
      std::cerr << " " << i;
    std::cerr << "\n";
  }

  std::cerr << "  name: '" << skin.name << "'\n";
}
