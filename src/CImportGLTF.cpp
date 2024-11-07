#include <CImportGLTF.h>
#include <CGeometry3D.h>
#include <CJson.h>
#include <CMinMax.h>

namespace {

auto errorMsg(const std::string &msg) -> bool {
  std::cerr << "[33mError[0m: " << msg << "\n";
  return false;
}

auto warnMsg(const std::string &msg) -> void {
  std::cerr << "[35mWarn[0m: " << msg << "\n";
}

auto debugMsg(const std::string &msg) -> void {
  std::cerr << "[31m" << msg << "[0m\n";
}

enum Constants {
  TYPE_SIGNED_BYTE    = 5120, // GL_SIGNED_BYTE
  TYPE_UNSIGNED_BYTE  = 5121, // GL_UNSIGNED_BYTE
  TYPE_SIGNED_SHORT   = 5122, // GL_SIGNED_SHORT
  TYPE_UNSIGNED_SHORT = 5123, // GL_UNSIGNED_SHORT
//TYPE_SIGNED_INT     = 5124, // GL_SIGNED_INT
  TYPE_UNSIGNED_INT   = 5125, // GL_UNSIGNED_INT
  TYPE_FLOAT          = 5126, // GL_FLOAT

  MODE_POINTS         = 0,
  MODE_LINE           = 1,
  MODE_LINE_LOOP      = 2,
  MODE_TRIANGLES      = 4,
  MODE_TRIANGLE_STRIP = 5,
  MODE_TRIANGLE_FAN   = 6,
};

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

  // check magic for bin file
  uchar buffer[5];

  if (file_->read(buffer, 4)) {
    // 'glTF'
    if (buffer[0] == 0x67 && buffer[1] == 0x6c && buffer[2] == 0x54 && buffer[3] == 0x46)
      return readBin();
  }

  //---

  file_->rewind();

  return readJson();
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

    if (! file_->read(&chunk.buffer.data[0], chunk.length))
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

    if (! file_->read(&chunk.buffer.data[0], chunk.length))
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
readJson()
{
  auto str = file_->toString();

  if (! parseJson(str, jsonData_))
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
  auto processAccessor = [&](const IndName &indName, const Accessor &accessor) {
    // get accessor buffer view
    BufferView bufferView;

    if (! getBufferView(accessor.bufferView, bufferView))
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

    // 1 component
    if      (accessor.type == "SCALAR") {
      if (accessor.componentType == Constants::TYPE_UNSIGNED_SHORT) {
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

        return errorMsg("Invalid SCALAR component type " + std::to_string(accessor.componentType));
      }
    }
    // 2 components
    else if (accessor.type == "VEC2") {
      if (accessor.componentType == Constants::TYPE_FLOAT) {
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
        return errorMsg("Invalid VEC2 component type " + std::to_string(accessor.componentType));
    }
    // 3 components
    else if (accessor.type == "VEC3") {
      if (accessor.componentType == Constants::TYPE_FLOAT) {
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
        return errorMsg("Invalid VEC3 component type " + std::to_string(accessor.componentType));
    }
    // 4 components
    else if (accessor.type == "VEC4") {
      if (accessor.componentType == Constants::TYPE_FLOAT) {
        if (bufferView.byteLength < accessor.count*4*4)
          return errorMsg("Invalid VEC4 count");

        int id1 = 0;

        for (long i = 0; i < accessor.count; ++i) {
          Vec4 v;

          v.x = readFloat(id1); id1 += 4;
          v.y = readFloat(id1); id1 += 4;
          v.z = readFloat(id1); id1 += 4;
          v.w = readFloat(id1); id1 += 4;

          meshData.vec4.push_back(v);
        }
      }
      else
        return errorMsg("Invalid VEC4 component type " + std::to_string(accessor.componentType));
    }
    // 4 componenents
    else if (accessor.type == "MAT2") {
      // TODO
    }
    // 9 componenents
    else if (accessor.type == "MAT3") {
      // TODO
    }
    // 16 componenents
    else if (accessor.type == "MAT4") {
      // TODO
    }
    else {
      //std::cerr << "  byteOffset: " << bufferView.byteOffset << "\n";
      //std::cerr << "  byteLength: " << bufferView.byteLength << "\n";

      return errorMsg("Invalid type '" + accessor.type + "'");
    }

    meshDatas_.add(meshData, indName);

    return true;
  };

  for (const auto &pa : jsonData_.accessors)
    processAccessor(pa.first, pa.second);

  //---

  // get images

  auto processImage = [&](const Image &image) {
    const uchar* data;
    long         len;

    if (! getImageData(image, data, len))
      return false;

    if (image.mimeType == "image/png" || image.mimeType == "image/jpeg") {
      if (isSaveImage() && image.name != "") {
        if (! writeFile(image.name, data, len))
          return errorMsg("Image write failed");
      }
    }
    else {
      return errorMsg("Invalid image type");
    }

    return true;
  };

  for (const auto &pi : jsonData_.images) {
    if (! processImage(pi.second))
      return false;
  }

  //---

  // get meshes

  auto getMeshData = [&](const IndName &indName) {
    MeshData *meshData = nullptr;

    auto pm = meshDatas_.find(indName);

    if (pm != meshDatas_.end())
      meshData = &(*pm).second;

    return meshData;
  };

  auto processMesh = [&](const Mesh &mesh) {
    for (const auto &primitive : mesh.primitives) {
      std::vector<ushort> indices;
      std::vector<Vec3>   positions;
      std::vector<Vec3>   normals;
      std::vector<Vec2>   textCoords0;

      if (! primitive.indices.isEmpty()) {
        auto *indMeshData = getMeshData(primitive.indices);
        if (! indMeshData)
          return errorMsg("Invalid indices mesh");
        //std::cerr << " indices\n";
        //printMeshData(*indMeshData);
        indices = indMeshData->scalars;
      }

      if (! primitive.position.isEmpty()) {
        auto *positionMeshData = getMeshData(primitive.position);
        if (! positionMeshData)
          return errorMsg("Invalid mesh POSITION");
        //std::cerr << " POSITION\n";
        //printMeshData(*positionMeshData);
        positions = positionMeshData->vec3;
      }

      if (! primitive.normal.isEmpty()) {
        auto *normalMeshData = getMeshData(primitive.normal);
        if (! normalMeshData)
          return errorMsg("Invalid mesh NORMAL");
        //std::cerr << " NORMAL\n";
        //printMeshData(*normalMeshData);
        normals = normalMeshData->vec3;
      }

      if (! primitive.texCoord0.isEmpty()) {
        auto *texCoord0MeshData = getMeshData(primitive.texCoord0);
        if (! texCoord0MeshData)
          return errorMsg("Invalid mesh TEXCOORD_0");
        //std::cerr << " TEXCOORD_0\n";
        //printMeshData(*texCoord0MeshData);
        textCoords0 = texCoord0MeshData->vec2;
      }

      CIMinMax indMinMax;
      for (auto i : indices)
        indMinMax.add(i);

      auto ni = indices    .size();
      auto np = positions  .size();
      auto nn = normals    .size();
      auto nt = textCoords0.size();

      if (ni == 0) {
        indices.resize(np);

        for (int i = 0; i < int(np); ++i)
          indices[i] = short(i);

        indMinMax.add(0);
        indMinMax.add(int(np - 1));

        ni = np;
      }

      if (np == 0)
        return errorMsg("No mesh positions");

      if (indMinMax.max() >= int(np))
        warnMsg("Invalid mesh indices size");

      if (nn > 0 && np != nn)
        warnMsg("Invalid mesh normals size");

      if (nt > 0 && np != nt)
        warnMsg("Invalid mesh textCoords0 size");

      //---

      for (const auto &p : positions) {
        auto ind = object_->addVertex(CPoint3D(p.x, p.y, p.z));

        auto &v = object_->getVertex(ind);

        if (ind < nn) {
          const auto &n = normals[ind];

          v.setNormal(CVector3D(n.x, n.y, n.z));
        }

        if (ind < nt) {
          const auto &t = textCoords0[ind];

          v.setTextureMap(CPoint2D(t.x, t.y));
        }
      }

      uint numTriangles = uint(indices.size()/3);

      for (uint i = 0; i < numTriangles; ++i) {
        int i1 = indices[i*3 + 0];
        int i2 = indices[i*3 + 1];
        int i3 = indices[i*3 + 2];

        if (i1 >= int(np) || i2 >= int(np) || i3 >= int(np))
          continue;

        indMinMax.add(i1);
        indMinMax.add(i2);
        indMinMax.add(i3);

        object_->addITriangle(i1, i2, i3);
      }

      //---

      if (! primitive.material.isEmpty()) {
        Material material;

        if (! getMaterial(primitive.material, material))
          return errorMsg("Invalid mesh material");

        //printMaterial(material, primitive.material);

        //---

        if (material.baseColorTexture.index >= 0) {
          Texture texture;

          if (! getTexture(IndName(material.baseColorTexture.index), texture))
            return errorMsg("Invalid base color texture");

          if (! texture.source.isEmpty()) {
            auto pi = jsonData_.images.find(texture.source);

            if (pi == jsonData_.images.end())
              return errorMsg("Invalid base color texture image");

            const auto &image = (*pi).second;

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
          else {
            warnMsg("Unhandled base color texture " + material.baseColorTexture.index);
          }
        }

        if (material.normalTexture.index >= 0) {
          Texture texture;

          if (! getTexture(IndName(material.normalTexture.index), texture))
            return errorMsg("Invalid normal texture");

          if (! texture.source.isEmpty()) {
            auto pi = jsonData_.images.find(texture.source);

            if (pi == jsonData_.images.end())
              return errorMsg("Invalid normal texture source");

            const auto &image = (*pi).second;

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
              return errorMsg("Invalid normal texture image type");
          }
          else {
            warnMsg("Unhandled normal texture " + material.baseColorTexture.index);
          }
        }
      }
    }

    return true;
  };

  for (const auto &pm : jsonData_.meshes) {
    object_ = CGeometryInst->createObject3D(scene_, pm.first.name);

    scene_->addObject(object_);

    processMesh(pm.second);
  }

  //---

#if 0
  int ia = 0;

  for (const auto &texture : jsonData_.textures) {
    std::cerr << "texture[" << texture.indName << "]\n";

    if (! texture.source.isEmpty()) {
      auto pi = jsonData_.images.find(texture.source);

      if (pi == jsonData_.images.end())
        return errorMsg("Invalid texture image");

      const auot &image = (*pi).second;

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

    if (texture.sampler.isEmpty()) {
      auto ps = jsonData_.samplers.find(texture.sampler);

      if (ps == jsonData_.samplers.end())
        return errorMsg("Invalid texture image");

      const auto &sampler = (*ps).second;

      printSampler(sampler, (*ps).first);
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

  if (bufferView.byteOffset < 0 ||
      bufferView.byteOffset + bufferView.byteLength >= long(chunk->buffer.data.size()))
    return errorMsg("Invalid buffer chunk data");

  data = &chunk->buffer.data[bufferView.byteOffset];
  len  = bufferView.byteLength;

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
            // TODO
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
        }
        else if (pnv1.first == "profile") {
          // TODO : object
        }
        else if (pnv1.first == "copyright") {
          // TODO : string
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
            // TODO
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
                  return errorMsg("Invalid materials index number");
              }
              else if (pnv3.first == "texCoord") {
                if (! valueLong(pnv3.second, material.normalTexture.texCoord))
                  return errorMsg("Invalid materials texCoord number");
              }
              else if (pnv3.first == "scale") {
                // TODO
              }
              else if (pnv3.first == "extensions") {
                // TODO
              }
              else {
                errorMsg("Invalid normalTexture name " + pnv3.first);
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
                  return errorMsg("Invalid material pbrMetallicRoughness metallicFactor number");
              }
              else if (pnv3.first == "roughnessFactor") {
                if (! valueNumber(pnv3.second, material.roughnessFactor))
                  return errorMsg("Invalid material pbrMetallicRoughness roughnessFactor number");
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
                  }
                  else {
                    errorMsg("Invalid baseColorTexture name " + pnv4.first);
                  }
                }
              }
              else if (pnv3.first == "metallicRoughnessTexture") {
                // TODO
              }
              else if (pnv3.first == "baseColorFactor") {
                // TODO
              }
              else {
                errorMsg("Invalid pbrMetallicRoughness name " + pnv3.first);
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
                if (! valueColor(pnv3.second, material.ambient))
                  return errorMsg("Invalid nodes ambient color");
              }
              else if (name == "diffuse") {
                if (! valueColor(pnv3.second, material.diffuse))
                  return errorMsg("Invalid nodes diffuse color");
              }
              else if (name == "emission") {
                if (! valueColor(pnv3.second, material.emission))
                  return errorMsg("Invalid nodes diffuse color");
              }
              else if (name == "shininess") {
                if (! valueNumber(pnv3.second, material.shininess))
                  return errorMsg("Invalid material values shininess");
              }
              else if (name == "specular") {
                if (! valueColor(pnv3.second, material.specular))
                  return errorMsg("Invalid nodes specular color");
              }
              else if (name == "transparency") {
                if (! valueNumber(pnv3.second, material.transparency))
                  return errorMsg("Invalid material values transparency");
              }
              else {
                debugMsg(std::string("  material values ") + name +
                         " : " + pnv3.second->typeName());
              }
            }
          }
          else if (pnv2.first == "occlusionTexture") {
            // TODO
          }
          else if (pnv2.first == "extensions") {
            // TODO
          }
          else if (pnv2.first == "alphaCutoff") {
            // TODO
          }
          else if (pnv2.first == "alphaMode") {
            // TODO
          }
          else if (pnv2.first == "doubleSided") {
            // TODO
          }
          else if (pnv2.first == "emissiveFactor") {
            // TODO
          }
          else {
            errorMsg("Invalid materials name " + pnv2.first);
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
                    }
                    else if (pnv4.first == "JOINTS_0") { // VEC4
                      // TODO
                    }
                    else if (pnv4.first == "WEIGHTS_0") { // VEC4
                      // TODO
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
        for (const auto &pnv2 : jobj->nameValueMap()) {
          if      (pnv2.first == "mesh") {
            if (! valueLong(pnv2.second, node.mesh))
              return errorMsg("Invalid nodes mesh number");
          }
          else if (pnv2.first == "name") {
            if (! valueString(pnv2.second, node.name))
              return errorMsg("Invalid nodes name string");
          }
          else if (pnv2.first == "matrix") {
            if (! pnv2.second->isArray())
              return errorMsg("Invalid nodes matrix array");

            auto *arr2 = pnv2.second->cast<CJson::Array>();

            std::vector<float> rvalues;

            for (const auto &pv3 : arr2->values()) {
              double r;
              if (! valueNumber(pv3, r))
                return errorMsg("Invalid nodes matrix array number");

              rvalues.push_back(float(r));
            }

            if (rvalues.size() != 16)
              return errorMsg("Invalid nodes matrix array size");

            node.matrix = CGLMatrix3D(&rvalues[0], 16);
          }
          else if (pnv2.first == "children") {
            // TODO
            if (! pnv2.second->isArray())
              return errorMsg("Invalid nodes children array");

            auto *arr2 = pnv2.second->cast<CJson::Array>();

            for (const auto &pv3 : arr2->values()) {
              if (pv3->isString()) {
                std::string str;
                (void) valueString(pv3, str);
              }
              else {
                double r;
                if (! valueNumber(pv3, r))
                  return errorMsg(std::string("Invalid nodes children type : ") + pv3->typeName());
              }
            }
          }
          else if (pnv2.first == "meshes") {
            // TODO
            if (! pnv2.second->isArray())
              return errorMsg("Invalid nodes meshes array");

            auto *arr2 = pnv2.second->cast<CJson::Array>();

            for (const auto &pv3 : arr2->values()) {
              if (! pv3->isString())
                return errorMsg("Invalid nodes meshes string");

              std::string str;
              (void) valueString(pv3, str);
            }
          }
          else if (pnv2.first == "skin") {
            // TODO
          }
          else if (pnv2.first == "rotation") {
            // TODO
          }
          else if (pnv2.first == "scale") {
            // TODO
          }
          else if (pnv2.first == "translation") {
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
          else if (pnv2.first == "extensions") {
            // TODO : object
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
      // TODO
      if      (pnv.second->isObject()) {
      }
      else if (pnv.second->isArray()) {
      }
      else
        return errorMsg("Invalid JSON " + pnv.first + " array data");

    }
    else if (pnv.first == "cameras") {
      // TODO
      if      (pnv.second->isObject()) {
      }
      else if (pnv.second->isArray()) {
      }
      else
        return errorMsg("Invalid JSON " + pnv.first + " array data");
    }
    else if (pnv.first == "extensionsUsed") {
      // TODO
      if (! pnv.second->isArray())
        return errorMsg("Invalid JSON " + pnv.first + " array data");
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
    else if (pnv.first == "extensions") {
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
  if      (meshData.vec4.size() > 0) {
    std::cerr << "  VEC4:";
    for (const auto &v : meshData.vec4) {
      std::cerr << " {" << v.x << "," << v.y << "," << v.z << "," << v.w << "}";
    }
    std::cerr << "\n";
  }
  else if (meshData.vec3.size() > 0) {
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

void
CImportGLTF::
printAccessor(const Accessor &accessor, const IndName &indName) const
{
  std::cerr << " accessor[" << indName << "]\n";

  std::cerr << "  bufferView: " << accessor.bufferView << "\n";
  std::cerr << "  byteOffset: " << accessor.byteOffset << "\n";
  std::cerr << "  byteStride: " << accessor.byteStride << "\n";
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
printTexture(const Texture &texture, const IndName &indName) const
{
  std::cerr << " texture[" << indName << "]\n";

  std::cerr << "  source: " << texture.source << "\n";
  std::cerr << "  sampler: " << texture.sampler << "\n";
  std::cerr << "  format: " << texture.format << "\n";
  std::cerr << "  internalFormat: " << texture.internalFormat << "\n";
  std::cerr << "  target: " << texture.target << "\n";
  std::cerr << "  type: " << texture.type << "\n";
  std::cerr << "  name: " << texture.name << "\n";
}

void
CImportGLTF::
printBufferView(const BufferView &bufferView, const IndName &indName) const
{
  std::cerr << " bufferView[" << indName << "]\n";

  std::cerr << "  buffer: " << bufferView.buffer << "\n";
  std::cerr << "  byteOffset: " << bufferView.byteOffset << "\n";
  std::cerr << "  byteLength: " << bufferView.byteLength << "\n";
  std::cerr << "  byteStride: " << bufferView.byteStride << "\n";
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
  std::cerr << "  url: " << image.url << "\n";
  std::cerr << "  uri: " << image.uri << "\n";
  std::cerr << "  width: " << image.width << "\n";
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
  std::cerr << "   index: " << material.normalTexture.index << "\n";
  std::cerr << "   texCoord: " << material.normalTexture.texCoord << "\n";
  std::cerr << "  baseColorTexture\n";
  std::cerr << "   index: " << material.baseColorTexture.index << "\n";
  std::cerr << "   texCoord: " << material.baseColorTexture.texCoord << "\n";
  std::cerr << "  technique: " << material.technique << "\n";
  std::cerr << "  ambient: " << material.ambient << "\n";
  std::cerr << "  diffuse: " << material.diffuse << "\n";
  std::cerr << "  emission: " << material.emission << "\n";
  std::cerr << "  specular: " << material.specular << "\n";
  std::cerr << "  shininess: " << material.shininess << "\n";
  std::cerr << "  transparency: " << material.transparency << "\n";
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

  std::cerr << "  mesh: " << node.mesh << "\n";
  std::cerr << "  name: " << node.name << "\n";
  std::cerr << "  matrix: " << node.matrix << "\n";
}

void
CImportGLTF::
printPrimitive(const Primitive &primitive, int ia) const
{
  std::cerr << "   primitive[" << ia << "]\n";
  std::cerr << "    mode: " << primitive.mode << "\n";
  std::cerr << "    indices: " << primitive.indices << "\n";
  std::cerr << "    NORMAL: " << primitive.normal << "\n";
  std::cerr << "    POSITION: " << primitive.position << "\n";
  std::cerr << "    TEXCOORD_0: " << primitive.texCoord0 << "\n";
  std::cerr << "    TEXCOORD_1: " << primitive.texCoord1 << "\n";
  std::cerr << "    material: " << primitive.material << "\n";
}
