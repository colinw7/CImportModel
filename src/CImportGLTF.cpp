#include <CImportGLTF.h>
#include <CGeometry3D.h>
#include <CJson.h>

namespace {

auto errorMsg(const std::string &msg) -> bool {
  std::cerr << "[33mError[0m: " << msg << "\n";
  return false;
}

auto debugMsg(const std::string &msg) -> void {
  std::cerr << "[31m" << msg << "[0m\n";
}

}

//---

CImportGLTF::
CImportGLTF(CGeomScene3D *scene, const std::string &name) :
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

CImportGLTF::
~CImportGLTF()
{
}

bool
CImportGLTF::
read(CFile &file)
{
  file_ = &file;

  //---

  uchar buffer[256];

  auto readInteger = [&](uint *integer) {
    if (! file.read(buffer, 4))
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

  // check magic
  if (! file.read(buffer, 4))
    return false;

  if (buffer[0] != 0x67 || buffer[1] != 0x6c || buffer[2] != 0x54 || buffer[3] != 0x46)
    return false;

  //---

  // get version (should be 1 or 2)
  if (! readInteger(&version_))
    return false;

  if (isDebug())
    std::cerr << "version: " << version_ << "\n";

  //---

  // get file length (including 12 byte header)
  uint length;
  if (! readInteger(&length))
    return false;

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

    if (! file.read(&chunk.buffer.data[0], chunk.length))
      return errorMsg("failed to read chunk");

    // parse json
    chunk.buffer.data[chunk.length] = '\0';

    std::string str(reinterpret_cast<char *>(&chunk.buffer.data[0]));

    if (! parseJson(str, jsonData_))
      return false;

    length -= chunk.length + 8;

    //---

    chunk.length = length;

    chunk.buffer.data.resize(chunk.length + 1);

    if (! file.read(&chunk.buffer.data[0], chunk.length))
      return errorMsg("failed to read chunk");

    jsonData_.chunks.push_back(chunk);
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

      if (! file.read(&chunk.buffer.data[0], chunk.length))
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
          std::cerr << "BIN: " << chunk.length << "\n";
      }
      else
        return errorMsg("Invalid chunk");

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
processData()
{
  auto getBufferView = [&](long ind, const std::string &name, BufferView &bufferView) {
    if      (ind >= 0) {
      if (ind >= long(jsonData_.bufferViews.array.size()))
        return false;

      bufferView = jsonData_.bufferViews.array[ind];
    }
    else if (name != "") {
      auto pn = jsonData_.bufferViews.map.find(name);

      if (pn == jsonData_.bufferViews.map.end())
        return false;

      bufferView = (*pn).second;
    }
    else
      return false;

    return true;
  };

  auto getChunk = [&](long ind, Chunk &chunk) {
    if (ind >= 0) {
      if (ind >= long(jsonData_.chunks.size()))
        return false;

      chunk = jsonData_.chunks[ind];
    }
    else
      return false;

    return true;
  };

  //---

  // get mesh data from accessors
  auto processAccessor = [&](long id, const std::string &name, const Accessor &accessor) {
    // get accessor buffer view
    BufferView bufferView;

    if (! getBufferView(accessor.bufferView.ind, accessor.bufferView.name, bufferView))
      return errorMsg("Invalid accessor buffer view");

    Chunk chunk;

    if (version_ == 1) {
      if (! getChunk(0, chunk))
        return errorMsg("Invalid buffer chunk");
    }
    else {
      if (! getChunk(bufferView.buffer.ind, chunk))
        return errorMsg("Invalid buffer chunk");
    }

    if (bufferView.byteOffset < 0 ||
        bufferView.byteOffset + bufferView.byteLength >= long(chunk.buffer.data.size()))
      return errorMsg("Invalid buffer chunk data");

    const uchar *data = &chunk.buffer.data[bufferView.byteOffset];

    auto readFloat = [&](long i) {
      const auto *fdata = &data[i];

      uint fi = ((fdata[0] & 0xFF)      ) |
                ((fdata[1] & 0xFF) <<  8) |
                ((fdata[2] & 0xFF) << 16) |
                ((fdata[3] & 0xFF) << 24);

      return *reinterpret_cast<float *>(&fi);
    };

     auto readShort = [&](long i) {
       const auto *sdata = &data[i];

       ushort s = ((sdata[0] & 0xFF)      ) |
                  ((sdata[1] & 0xFF) <<  8);

       return s;
    };

    MeshData meshData;

    if      (accessor.type == "VEC3") {
      if (accessor.componentType == 5126) { // GL_FLOAT
        if (bufferView.byteLength < accessor.count*3*4)
          return errorMsg("Invalid VEC3 count");

        int id1 = 0;

        for (long i = 0; i < accessor.count; ++i) {
          Vec3 v;

          v.x = readFloat(id1); id1 += 4;
          v.y = readFloat(id1); id1 += 4;
          v.z = readFloat(id1); id1 += 4;

          meshData.vec3.push_back(v);
        }
      }
      else
        return errorMsg("Invalid component type");
    }
    else if (accessor.type == "VEC2") {
      if (accessor.componentType == 5126) { // GL_FLOAT
        if (bufferView.byteLength < accessor.count*2*4)
          return errorMsg("Invalid VEC2 count");

        int id1 = 0;

        for (long i = 0; i < accessor.count; ++i) {
          Vec2 v;

          v.x = readFloat(id1); id1 += 4;
          v.y = readFloat(id1); id1 += 4;

          meshData.vec2.push_back(v);
        }
      }
      else
        return errorMsg("Invalid component type");
    }
    else if (accessor.type == "SCALAR") {
      if (accessor.componentType == 5123) { // GL_UNSIGNED_SHORT
        if (bufferView.byteLength < accessor.count*2)
          return errorMsg("Invalid SCALAR count");

        for (long i = 0; i < accessor.count; ++i) {
          auto s = readShort(i*2);

          meshData.scalars.push_back(s);
        }
      }
      else {
        //std::cerr << " " << accessor.count;
        //std::cerr << " " << accessor.componentType;
        //std::cerr << "\n";

        return errorMsg("Invalid component type");
      }
    }
    else {
      //std::cerr << "  byteOffset: " << bufferView.byteOffset << "\n";
      //std::cerr << "  byteLength: " << bufferView.byteLength << "\n";

      return errorMsg("Invalid type '" + accessor.type + "'");
    }

    if      (name != "")
      meshDataMap_[name] = meshData;
    else if (id >= 0)
      meshDatas_.push_back(meshData);

    return true;
  };

  int ia = 0;

  for (const auto &accessor : jsonData_.accessors.array)
    processAccessor(ia++, "", accessor);

  for (const auto &pa : jsonData_.accessors.map)
    processAccessor(-1, pa.first, pa.second);

  //---

  // get images

  auto processImage = [&](const Image &image) {
    const uchar* data;
    long         len;

    if (! getImageData(image, data, len))
      return false;

    if (image.mimeType == "image/png" || image.mimeType == "image/jpeg") {
      if (image.name == "")
        return errorMsg("Invalid image name");

      if (isSaveImage()) {
        if (! writeFile(image.name, data, len))
          return errorMsg("Image write failed");
      }
    }
    else {
      return errorMsg("Invalid image type");
    }

    return true;
  };

  for (const auto &image : jsonData_.images.array) {
    if (! processImage(image))
      return false;
  }

  for (const auto &pi : jsonData_.images.map) {
    if (! processImage(pi.second))
      return false;
  }

  //---

  // get meshes

  auto getMeshData = [&](const IndName &indName) {
    MeshData *meshData = nullptr;

    if      (indName.ind >= 0) {
      if (indName.ind < long(meshDatas_.size()))
        meshData = &meshDatas_[indName.ind];
    }
    else if (indName.name != "") {
      auto pm = meshDataMap_.find(indName.name);

      if (pm != meshDataMap_.end())
        meshData = &(*pm).second;
    }

    return meshData;
  };

  auto getMaterial = [&](const IndName &indName) {
    Material *material = nullptr;

    if      (indName.ind >= 0) {
      if (indName.ind < long(jsonData_.materials.array.size()))
        material = &jsonData_.materials.array[indName.ind];
    }
    else if (indName.name != "") {
      auto pm = jsonData_.materials.map.find(indName.name);

      if (pm != jsonData_.materials.map.end())
        material = &(*pm).second;
    }

    return material;
  };

  auto processMesh = [&](const Mesh &mesh) {
    for (const auto &primitive : mesh.primitives) {
      if (! primitive.indices.isEmpty()) {
        auto *meshData = getMeshData(primitive.indices);
        if (! meshData)
          return errorMsg("Invalid indices mesh");

        //std::cerr << " indices\n";
        //printMeshData(*meshData);

        const auto &indices = meshData->scalars;

        uint nt = uint(indices.size()/3);

        for (uint i = 0; i < nt; ++i) {
          int i1 = indices[i*3 + 0];
          int i2 = indices[i*3 + 1];
          int i3 = indices[i*3 + 2];

          object_->addITriangle(i1, i2, i3);
        }
      }

      if (primitive.position.ind >= 0 || primitive.position.name != "") {
        auto *meshData = getMeshData(primitive.position);
        if (! meshData)
          return errorMsg("Invalid mesh POSITION");

        //std::cerr << " POSITION\n";
        //printMeshData(*meshData);

        const auto &points = meshData->vec3;

        for (const auto &p : points)
          (void) object_->addVertex(CPoint3D(p.x, p.y, p.z));
      }

      if (primitive.normal.ind >= 0 || primitive.normal.name != "") {
        auto *meshData = getMeshData(primitive.normal);
        if (! meshData)
          return errorMsg("Invalid mesh NORMAL");

        //std::cerr << " NORMAL\n";
        //printMeshData(*meshData);

        const auto &points = meshData->vec3;

        uint i = 0;

        for (const auto &p : points) {
          auto &v = object_->getVertex(i);

          (void) v.setNormal(CVector3D(p.x, p.y, p.z));

          ++i;
        }
      }

      if (primitive.texCoord0.ind >= 0 || primitive.texCoord0.name != "") {
        auto *meshData = getMeshData(primitive.texCoord0);
        if (! meshData)
          return errorMsg("Invalid mesh TEXCOORD_0");

        //std::cerr << " TEXCOORD_0\n";
        //printMeshData(*meshData);

        const auto &points = meshData->vec2;

        uint i = 0;

        for (const auto &p : points) {
          auto &v = object_->getVertex(i);

          (void) v.setTextureMap(CPoint2D(p.x, p.y));

          ++i;
        }
      }

      if (primitive.material.ind >= 0 || primitive.material.name != "") {
        auto *material = getMaterial(primitive.material);
        if (! material)
          return errorMsg("Invalid mesh material");

        //printMaterial(*material, int(primitive.material.ind), "");

        //---

        if (material->baseColorTexture.index >= 0) {
          if (material->baseColorTexture.index >= long(jsonData_.textures.array.size()))
            return errorMsg("Invalid base color texture");

          const auto &texture = jsonData_.textures.array[material->baseColorTexture.index];

          if (texture.source.ind >= 0) {
            if (texture.source.ind >= int(jsonData_.images.array.size()))
              return errorMsg("Invalid base color texture image");

            const auto &image = jsonData_.images.array[texture.source.ind];

            const uchar* data;
            long         len;

            if (! getImageData(image, data, len))
              return false;

            if      (image.mimeType == "image/png") {
              auto image1 = CImageMgrInst->createImage();

              if (! image1->read(data, len, CFILE_TYPE_IMAGE_PNG))
                return false;

              object_->setDiffuseTexture(CGeometryInst->createTexture(image1));
            }
            else if (image.mimeType == "image/jpeg") {
              auto image1 = CImageMgrInst->createImage();

              if (! image1->read(data, len, CFILE_TYPE_IMAGE_JPG))
                return false;

              object_->setDiffuseTexture(CGeometryInst->createTexture(image1));
            }
            else
              return errorMsg("Invalid base color texture image type");
          }
        }

        if (material->normalTexture.index >= 0) {
          if (material->normalTexture.index >= long(jsonData_.textures.array.size()))
            return errorMsg("Invalid normal texture image");

          const auto &texture = jsonData_.textures.array[material->normalTexture.index];

          if (texture.source.ind >= 0) {
            if (texture.source.ind >= int(jsonData_.images.array.size()))
              return errorMsg("Invalid normal texture source");

            const auto &image = jsonData_.images.array[texture.source.ind];

            const uchar* data;
            long         len;

            if (! getImageData(image, data, len))
              return false;

            if      (image.mimeType == "image/png") {
              auto image1 = CImageMgrInst->createImage();

              if (! image1->read(data, len, CFILE_TYPE_IMAGE_PNG))
                return false;

              object_->setNormalTexture(CGeometryInst->createTexture(image1));
            }
            else if (image.mimeType == "image/jpeg") {
              auto image1 = CImageMgrInst->createImage();

              if (! image1->read(data, len, CFILE_TYPE_IMAGE_JPG))
                return false;

              object_->setNormalTexture(CGeometryInst->createTexture(image1));
            }
            else
              return errorMsg("Invalid notmal texture image type");
          }
        }
      }
    }

    return true;
  };

  for (const auto &mesh : jsonData_.meshes.array)
    processMesh(mesh);

  for (const auto &pm : jsonData_.meshes.map)
    processMesh(pm.second);

  //---

#if 0
  ia = 0;

  for (const auto &texture : jsonData_.textures.array) {
    std::cerr << "texture[" << ia << "]\n";

    if (texture.source.ind >= 0) {
      if (texture.source.ind < int(jsonData_.images.array.size())) {
        const auto &image = jsonData_.images.array[texture.source.ind];

        const uchar* data;
        long         len;

        if (! getImageData(image, data, len))
          return false;

        if (image.mimeType == "image/png") {
          auto image1 = CImageMgrInst->createImage();

          if (! image1->read(data, len, CFILE_TYPE_IMAGE_PNG))
            return false;

          object_->setDiffuseTexture(CGeometryInst->createTexture(image1));
        }
      }
      else
        return errorMsg("Invalid texture image");
    }

    if (texture.sampler.ind >= 0) {
      if (texture.sampler.ind < int(jsonData_.samplers.array.size())) {
        const auto &sampler = jsonData_.samplers.array[texture.sampler.ind];

        printSampler(sampler, ia, "");
      }
      else
        return errorMsg("Invalid texture image");
    }

    ++ia;
  }
#endif

  //---

  return true;
}

bool
CImportGLTF::
getImageData(const Image &image, const uchar* &data, long &len)
{
  BufferView *bufferView = nullptr;

  // get image buffer view
  if      (image.bufferView.ind >= 0) {
    if (image.bufferView.ind >= long(jsonData_.bufferViews.array.size()))
      return errorMsg("Invalid image buffer view");

    bufferView = &jsonData_.bufferViews.array[image.bufferView.ind];
  }
  else if (image.bufferView.name != "") {
    auto pb = jsonData_.bufferViews.map.find(image.bufferView.name);

    if (pb == jsonData_.bufferViews.map.end())
      return errorMsg("Invalid image buffer view");

    bufferView = &(*pb).second;
  }
  else {
    return errorMsg("Invalid image buffer view");
  }

  Chunk *chunk = nullptr;

  if      (bufferView->buffer.ind >= 0) {
    if (bufferView->buffer.ind >= long(jsonData_.chunks.size()))
      return errorMsg("Invalid buffer chunk");

    chunk = &jsonData_.chunks[bufferView->buffer.ind];
  }
  else if (bufferView->buffer.name != "") {
    auto pb = jsonData_.buffers.map.find(bufferView->buffer.name);

    if (pb == jsonData_.buffers.map.end())
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

  if (bufferView->byteOffset < 0 ||
      bufferView->byteOffset + bufferView->byteLength >= long(chunk->buffer.data.size()))
    return errorMsg("Invalid buffer chunk data");

  data = &chunk->buffer.data[bufferView->byteOffset];
  len  = bufferView->byteLength;

  return true;
}

void
CImportGLTF::
printMeshData(const MeshData &meshData) const
{
  if      (meshData.vec3.size() > 0) {
    std::cerr << "  VEC3:";
    for (const auto &v : meshData.vec3) {
      std::cerr << " {" << v.x << "," << v.y << "," << v.z << "}";
    }
    std::cerr << "\n";
  }
  else if (meshData.vec2.size() > 0) {
    std::cerr << "  VEC2:";
    for (const auto &v : meshData.vec2) {
      std::cerr << " {" << v.x << "," << v.y << "}";
    }
    std::cerr << "\n";
  }
  else if (meshData.scalars.size() > 0) {
    std::cerr << "  SCALAR:";
    for (const auto &s : meshData.scalars) {
      std::cerr << " " << s;
    }
    std::cerr << "\n";
  }
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
              return errorMsg("Invalid bufferView ind/name");
          }
          else if (pnv1.first == "byteOffset") {
            if (! valueLong(pnv1.second, accessor.byteOffset))
              return errorMsg("Invalid byteOffset number");
          }
          else if (pnv1.first == "byteStride") {
            if (! valueLong(pnv1.second, accessor.byteStride))
              return errorMsg("Invalid byteStride number");
          }
          else if (pnv1.first == "type") {
            if (! valueString(pnv1.second, accessor.type))
              return errorMsg("Invalid type string");
          }
          else if (pnv1.first == "componentType") {
            if (! valueLong(pnv1.second, accessor.componentType))
              return errorMsg("Invalid componentType number");
          }
          else if (pnv1.first == "count") {
            if (! valueLong(pnv1.second, accessor.count))
              return errorMsg("Invalid count number");
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

          jsonData.accessors.array.push_back(accessor);
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

          jsonData.accessors.map[name] = accessor;
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
          if (! valueString(pnv1.second, jsonData.asset.version))
            return errorMsg("Invalid version string");
        }
        else if (pnv1.first == "premultipliedAlpha") {
          // TODO
        }
        else if (pnv1.first == "profile") {
          // TODO
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
              return errorMsg("Invalid buffer ind/name");
          }
          else if (pnv2.first == "byteOffset") {
            if (! valueLong(pnv2.second, bufferView.byteOffset))
              return errorMsg("Invalid byteOffset number");
          }
          else if (pnv2.first == "byteLength") {
            if (! valueLong(pnv2.second, bufferView.byteLength))
              return errorMsg("Invalid byteLength number");
          }
          else if (pnv2.first == "byteStride") {
            if (! valueLong(pnv2.second, bufferView.byteStride))
              return errorMsg("Invalid byteStride number");
          }
          else if (pnv2.first == "target") {
            if (! valueLong(pnv2.second, bufferView.target))
              return errorMsg("Invalid target number");
          }
          else {
            return errorMsg("Invalid buffer name");
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

          jsonData.bufferViews.array.push_back(bufferView);
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

          jsonData.bufferViews.map[name] = bufferView;
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
              return errorMsg("Invalid byteLength number");
          }
          else if (pnv2.first == "type") {
            // TODO
          }
          else if (pnv2.first == "uri") {
            // TODO
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

          jsonData.buffers.array.push_back(buffer);
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

          jsonData.buffers.map[name] = buffer;
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
              return errorMsg("Invalid bufferView ind/name");
          }
          else if (name == "mimeType") {
            if (! valueString(nvalue, image.mimeType))
              return errorMsg("Invalid mimeType string");
          }
          else if (name == "name") {
            if (! valueString(nvalue, image.name))
              return errorMsg("Invalid name string");
          }
          else if (name == "url") {
            if (! valueString(nvalue, image.url))
              return errorMsg("Invalid url string");
          }
          else if (name == "uri") {
            if (! valueString(nvalue, image.uri))
              return errorMsg("Invalid uri string");
          }
          else if (name == "width") {
            // TODO
          }
          else if (name == "height") {
            // TODO
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
              return errorMsg("Invalid bufferView ind/name");
          }
          else if (name == "mimeType") {
            if (! valueString(nvalue, image.mimeType))
              return errorMsg("Invalid mimeType string");
          }
          else if (name == "name") {
            if (! valueString(nvalue, image.name))
              return errorMsg("Invalid name string");
          }
          else if (name == "url") {
            if (! valueString(nvalue, image.url))
              return errorMsg("Invalid url string");
          }
          else if (name == "uri") {
            if (! valueString(nvalue, image.uri))
              return errorMsg("Invalid uri string");
          }
          else if (name == "width") {
            // TODO
          }
          else if (name == "height") {
            // TODO
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

          jsonData.images.array.push_back(image);
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

          jsonData.images.map[name] = image;
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
              return errorMsg("Invalid name string");
          }
          else if (pnv2.first == "normalTexture") {
            if (! pnv2.second->isObject())
              return errorMsg("Invalid " + pnv.first + " normalTexture");

            auto *obj3 = pnv2.second->cast<CJson::Object>();

            for (const auto &pnv3 : obj3->nameValueMap()) {
              if      (pnv3.first == "index") {
                if (! valueLong(pnv3.second, material.normalTexture.index))
                  return errorMsg("Invalid index number");
              }
              else if (pnv3.first == "texCoord") {
                if (! valueLong(pnv3.second, material.normalTexture.texCoord))
                  return errorMsg("Invalid texCoord number");
              }
              else {
                errorMsg("Invalid normalTexture name " + pnv3.first + "\n");
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
                  return errorMsg("Invalid metallicFactor number");
              }
              else if (pnv3.first == "roughnessFactor") {
                if (! valueNumber(pnv3.second, material.roughnessFactor))
                  return errorMsg("Invalid roughnessFactor number");
              }
              else if (pnv3.first == "baseColorTexture") {
                if (! pnv3.second->isObject())
                  return errorMsg("Invalid " + pnv.first + " baseColorTexture");

                auto *obj4 = pnv3.second->cast<CJson::Object>();

                for (const auto &pnv4 : obj4->nameValueMap()) {
                  if      (pnv4.first == "index") {
                    if (! valueLong(pnv4.second, material.baseColorTexture.index))
                      return errorMsg("Invalid index number");
                  }
                  else if (pnv4.first == "texCoord") {
                    if (! valueLong(pnv4.second, material.baseColorTexture.texCoord))
                      return errorMsg("Invalid texCoord number");
                  }
                  else {
                    errorMsg("Invalid baseColorTexture name " + pnv4.first + "\n");
                  }
                }
              }
              else {
                errorMsg("Invalid pbrMetallicRoughness name " + pnv3.first + "\n");
              }
            }
          }
          else if (pnv2.first == "technique") {
            // TODO
          }
          else if (pnv2.first == "values") {
            // TODO
          }
          else {
            errorMsg("Invalid materials name " + pnv2.first + "\n");
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

          jsonData.materials.array.push_back(material);
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

          jsonData.materials.map[name] = material;
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
                    if      (pnv4.first == "NORMAL") {
                      if (! valueIndName(pnv4.second, primitive.normal))
                        return errorMsg("Invalid NORMAL ind/name");
                    }
                    else if (pnv4.first == "POSITION") {
                      if (! valueIndName(pnv4.second, primitive.position))
                        return errorMsg("Invalid POSITION ind/name");
                    }
                    else if (pnv4.first == "TEXCOORD_0") {
                      if (! valueIndName(pnv4.second, primitive.texCoord0))
                        return errorMsg("Invalid TEXCOORD_0 ind/name");
                    }
                    else {
                      debugMsg("  " + pnv.first + "/" + pnv2.first + "/" +
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
                else {
                  debugMsg("  " + pnv.first + "/" + pnv3.first + " : " + pnv3.second->typeName());
                }
              }

              mesh.primitives.push_back(primitive);
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

          jsonData.meshes.array.push_back(mesh);
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

          jsonData.meshes.map[name] = mesh;
        }
      }
      else {
        return errorMsg("Invalid JSON " + pnv.first + " data");
      }
    }
    else if (pnv.first == "nodes") {
      auto readNode = [&](CJson::Object *jobj, Node &node) {
        for (const auto &pnv2 : jobj->nameValueMap()) {
          if      (pnv2.first == "mesh") {
            if (! valueLong(pnv2.second, node.mesh))
              return errorMsg("Invalid mesh number");
          }
          else if (pnv2.first == "name") {
            if (! valueString(pnv2.second, node.name))
              return errorMsg("Invalid name string");
          }
          else if (pnv2.first == "matrix") {
            // TODO
          }
          else if (pnv2.first == "children") {
            // TODO
          }
          else if (pnv2.first == "meshes") {
            // TODO
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

          jsonData.nodes.array.push_back(node);
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

          jsonData.nodes.map[name] = node;
        }
      }
      else {
        return errorMsg("Invalid JSON " + pnv.first + " data");
      }
    }
    else if (pnv.first == "samplers") {
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

          jsonData.samplers.array.push_back(sampler);
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

          jsonData.samplers.map[name] = sampler;
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
              if (pv2->isString()) {
                std::string nodeName;
                if (! valueString(pv2, nodeName))
                  return errorMsg("Invalid scene node name");

                scene.nodeNames.push_back(nodeName);
              }
              else {
                long nodeId;
                if (! valueLong(pv2, nodeId))
                  return errorMsg("Invalid scene node number");

                scene.nodeIds.push_back(nodeId);
              }
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

          Scene scene;

          if (! readScene(obj2, scene))
            return false;

          jsonData.scenes.array.push_back(scene);
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

          jsonData.scenes.map[name] = scene;
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
            // TODO
          }
          else if (pnv2.first == "internalFormat") {
            // TODO
          }
          else if (pnv2.first == "target") {
            // TODO
          }
          else if (pnv2.first == "type") {
            // TODO
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

          Texture texture;

          if (! readTexture(obj2, texture))
            return false;

          jsonData.textures.array.push_back(texture);
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

          jsonData.textures.map[name] = texture;
        }
      }
      else {
        return errorMsg("Invalid JSON " + pnv.first + " data");
      }
    }
    else if (pnv.first == "animations") {
      if (! pnv.second->isObject())
        return errorMsg("Invalid JSON " + pnv.first + " array data");

      // TODO
    }
    else if (pnv.first == "cameras") {
      if (! pnv.second->isObject())
        return errorMsg("Invalid JSON " + pnv.first + " array data");

      // TODO
    }
    else if (pnv.first == "extensionsUsed") {
      if (! pnv.second->isArray())
        return errorMsg("Invalid JSON " + pnv.first + " array data");

      // TODO
    }
    else if (pnv.first == "programs") {
      // TODO
    }
    else if (pnv.first == "shaders") {
      // TODO
    }
    else if (pnv.first == "skins") {
      // TODO
    }
    else if (pnv.first == "techniques") {
      // TODO
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

    int ia = 0;

    for (const auto &accessor : jsonData.accessors.array)
      printAccessor(accessor, IndName(ia++));

    for (const auto &pa : jsonData.accessors.map)
      printAccessor(pa.second, IndName(pa.first));
  }

  //---

  if (jsonData.asset.set) {
    std::cerr << "asset\n";
    std::cerr << "  generator : " << jsonData.asset.generator << "\n";
    std::cerr << "  version : " << jsonData.asset.version << "\n";
  }

  //---

  if (! jsonData.bufferViews.isEmpty()) {
    std::cerr << "bufferViews\n";

    int ia = 0;

    for (const auto &bufferView : jsonData.bufferViews.array)
      printBufferView(bufferView, ia++, "");

    for (const auto &pb : jsonData.bufferViews.map)
      printBufferView(pb.second, -1, pb.first);
  }

  //---

  if (! jsonData.buffers.isEmpty()) {
    std::cerr << "buffers\n";

    int ia = 0;

    for (const auto &buffer : jsonData.buffers.array)
      printBuffer(buffer, ia++, "");

    for (const auto &pb : jsonData.buffers.map)
      printBuffer(pb.second, -1, pb.first);
  }

  //---

  if (! jsonData.images.isEmpty()) {
    std::cerr << "images\n";

    int ia = 0;

    for (const auto &image : jsonData.images.array)
      printImage(image, ia++, "");

    for (const auto &pi : jsonData.images.map)
      printImage(pi.second, -1, pi.first);
  }

  //---

  if (! jsonData.materials.isEmpty()) {
    std::cerr << "materials\n";

    int ia = 0;

    for (const auto &material : jsonData.materials.array)
      printMaterial(material, ia++, "");

    for (const auto &pm : jsonData.materials.map)
      printMaterial(pm.second, -1, pm.first);
  }

  //---

  if (! jsonData.meshes.isEmpty()) {
    std::cerr << "meshes\n";

    int ia = 0;

    for (const auto &mesh : jsonData.meshes.array)
      printMesh(mesh, ia++, "");

    for (const auto &pm : jsonData.meshes.map)
      printMesh(pm.second, -1, pm.first);
  }

  //---

  if (! jsonData.nodes.isEmpty()) {
    std::cerr << "nodes\n";

    int ia { 0 };

    for (const auto &node : jsonData.nodes.array)
      printNode(node, ia++, "");

    for (const auto &pn : jsonData.nodes.map)
      printNode(pn.second, -1, pn.first);
  }

  if (! jsonData.samplers.isEmpty()) {
    std::cerr << "samplers\n";

    int ia { 0 };

    for (const auto &sampler : jsonData.samplers.array)
      printSampler(sampler, ia++, "");

    for (const auto &ps : jsonData.samplers.map)
      printSampler(ps.second, -1, ps.first);
  }

  //---

  std::cerr << "scene: " << jsonData.scene << "\n";

  //---

  if (! jsonData.scenes.isEmpty()) {
    std::cerr << "scenes\n";

    int ia = 0;

    for (const auto &scene : jsonData.scenes.array)
      printScene(scene, ia++, "");

    for (const auto &ps : jsonData.scenes.map)
      printScene(ps.second, -1, ps.first);
  }

  //---

  if (! jsonData.textures.isEmpty()) {
    std::cerr << "textures\n";

    int ia = 0;

    for (const auto &texture : jsonData.textures.array)
      printTexture(texture, ia++, "");

    for (const auto &pt : jsonData.textures.map)
      printTexture(pt.second, -1, pt.first);
  }
}

void
CImportGLTF::
printAccessor(const Accessor &accessor, const IndName &indName) const
{
  std::cerr << " accessor[" << indName << "]\n";

  std::cerr << "  bufferView: " << accessor.bufferView << "\n";
  std::cerr << "  type: " << accessor.type << "\n";
  std::cerr << "  componentType: " << accessor.componentType << "\n";
  std::cerr << "  count: " << accessor.count << "\n";

  std::cerr << "  min:";
  for (auto r : accessor.min)
    std::cerr << " " << r;
  std::cerr << "\n";

  std::cerr << "  max:";
  for (auto r : accessor.max)
    std::cerr << " " << r;
  std::cerr << "\n";
}

void
CImportGLTF::
printSampler(const Sampler &sampler, int ia, const std::string &id) const
{
  std::cerr << " sampler[";
  if (ia >= 0)
    std::cerr << ia;
  else
    std::cerr << id;
  std::cerr << "]\n";

  std::cerr << "  magFilter: " << sampler.magFilter << "\n";
  std::cerr << "  minFilter: " << sampler.minFilter << "\n";
  std::cerr << "  wrapS: " << sampler.wrapS << "\n";
  std::cerr << "  wrapT: " << sampler.wrapT << "\n";
}

void
CImportGLTF::
printScene(const Scene &scene, int ia, const std::string &id) const
{
  std::cerr << " scene[";
  if (ia >= 0)
    std::cerr << ia;
  else
    std::cerr << id;
  std::cerr << "]\n";

  std::cerr << "  name: " << scene.name << "\n";

  std::cerr << "  nodes:";
  for (const auto &node : scene.nodeIds)
    std::cerr << " " << node;
  for (const auto &node : scene.nodeNames)
    std::cerr << " " << node;
  std::cerr << "\n";
}

void
CImportGLTF::
printTexture(const Texture &texture, int ia, const std::string &id) const
{
  std::cerr << " texture[";
  if (ia >= 0)
    std::cerr << ia;
  else
    std::cerr << id;
  std::cerr << "]\n";

  std::cerr << "  source: " << texture.source << "\n";
  std::cerr << "  sampler: " << texture.sampler << "\n";
}

void
CImportGLTF::
printBufferView(const BufferView &bufferView, int ia, const std::string &id) const
{
  std::cerr << " bufferView[";
  if (ia >= 0)
    std::cerr << ia;
  else
    std::cerr << id;
  std::cerr << "]\n";

  std::cerr << "  buffer: " << bufferView.buffer << "\n";
  std::cerr << "  byteOffset: " << bufferView.byteOffset << "\n";
  std::cerr << "  byteLength: " << bufferView.byteLength << "\n";
}


void
CImportGLTF::
printBuffer(const Buffer &buffer, int ia, const std::string &id) const
{
  std::cerr << " buffer[";
  if (ia >= 0)
    std::cerr << ia;
  else
    std::cerr << id;
  std::cerr << "]\n";

  std::cerr << "  byteLength: " << buffer.byteLength << "\n";
};

void
CImportGLTF::
printImage(const Image &image, int ia, const std::string &id) const
{
  std::cerr << " image[";
  if (ia >= 0)
    std::cerr << ia;
  else
    std::cerr << id;
  std::cerr << "]\n";

  std::cerr << "  bufferView: " << image.bufferView << "\n";
  std::cerr << "  mimeType: " << image.mimeType << "\n";
  std::cerr << "  name: " << image.name << "\n";
}

void
CImportGLTF::
printMaterial(const Material &material, int ia, const std::string &id) const
{
  std::cerr << " material[";
  if (ia >= 0)
    std::cerr << ia;
  else
    std::cerr << id;
  std::cerr << "]\n";

  std::cerr << "  name: " << material.name << "\n";
  std::cerr << "  normalTexture\n";
  std::cerr << "   index: " << material.normalTexture.index << "\n";
  std::cerr << "   texCoord: " << material.normalTexture.texCoord << "\n";
  std::cerr << "  metallicFactor: " << material.metallicFactor << "\n";
  std::cerr << "  roughnessFactor: " << material.roughnessFactor << "\n";
  std::cerr << "  baseColorTexture\n";
  std::cerr << "   index: " << material.baseColorTexture.index << "\n";
  std::cerr << "   texCoord: " << material.baseColorTexture.texCoord << "\n";
}

void
CImportGLTF::
printMesh(const Mesh &mesh, int ia, const std::string &id) const
{
  std::cerr << " mesh[";
  if (ia >= 0)
    std::cerr << ia;
  else
    std::cerr << id;
  std::cerr << "]\n";

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
printNode(const Node &node, int ia, const std::string &id) const
{
  std::cerr << " node[";
  if (ia >= 0)
    std::cerr << ia;
  else
    std::cerr << id;
  std::cerr << "]\n";

  std::cerr << "  mesh: " << node.mesh << "\n";
  std::cerr << "  name: " << node.name << "\n";
}

void
CImportGLTF::
printPrimitive(const Primitive &primitive, int ia) const
{
  std::cerr << "   primitive[" << ia << "]\n";
  std::cerr << "    indices: " << primitive.indices << "\n";
  std::cerr << "    material: " << primitive.material << "\n";
  std::cerr << "    NORMAL: " << primitive.normal << "\n";
  std::cerr << "    POSITION: " << primitive.position << "\n";
  std::cerr << "    TEXCOORD_0: " << primitive.texCoord0 << "\n";
}
