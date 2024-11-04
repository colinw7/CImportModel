#ifndef CImportGLTF_H
#define CImportGLTF_H

#include <CImportBase.h>
#include <CGeomObject3D.h>
#include <CFile.h>
#include <CAutoPtr.h>

#include <string>
#include <vector>
#include <iostream>

class CImportGLTF : public CImportBase {
 public:
  CImportGLTF(CGeomScene3D *scene=nullptr, const std::string &name="gltf");

 ~CImportGLTF();

  bool isDebug() const { return debug_; }
  void setDebug(bool b) { debug_ = b; }

  bool isSaveImage() const { return saveImage_; }
  void setSaveImage(bool b) { saveImage_ = b; }

  bool read(CFile &file) override;

  CGeomScene3D &getScene() override { return *scene_; }

  CGeomObject3D &getObject() { return *object_; }

  CGeomScene3D *releaseScene() override {
    pscene_ .release();
    pobject_.release();

    return scene_;
  }

  CGeomObject3D *releaseObject() {
    pobject_.release();

    return object_;
  }

 private:
  struct ChunkBuffer {
    std::vector<unsigned char> data;
  };

  struct Chunk {
    uint        length { 0 };
    uint        type   { 0 };
    ChunkBuffer buffer;
  };

  struct IndName {
    long        ind  { -1 };
    std::string name;

    IndName() { }

    IndName(long i) : ind(i) { }
    IndName(const std::string &n) : name(n) { }

    friend std::ostream &operator<<(std::ostream &os, const IndName &i) {
      if      (i.ind  >= 0 ) os << i.ind;
      else if (i.name != "") os << i.name;
      return os;
    }

    bool isEmpty() const { return (ind < 0 && name == ""); }
  };

  struct Accessor {
    IndName             bufferView;
    long                byteOffset    { -1 };
    long                byteStride    { -1 };
    std::string         type;
    long                componentType { -1 };
    long                count         { -1 };
    std::vector<double> min;
    std::vector<double> max;
  };

  struct Asset {
    bool        set { false };
    std::string generator;
    std::string version;
  };

  struct BufferView {
    IndName buffer;
    long    byteOffset { -1 };
    long    byteLength { -1 };
    long    byteStride { -1 };
    long    target     { -1 };
  };

  struct Buffer {
    long        byteLength { 0 };
    std::string uri;
  };

  struct Image {
    IndName     bufferView;
    std::string mimeType;
    std::string name;
    std::string url;
    std::string uri;
  };

  struct MaterialTexture {
    long index    { -1 };
    long texCoord { -1 };
  };

  struct Material {
    std::string     name;
    double          metallicFactor { 0 };
    double          roughnessFactor { 0 };
    MaterialTexture normalTexture;
    MaterialTexture baseColorTexture;
  };

  struct Primitive {
    long    mode      { -1 }; // default triangles
    IndName indices;
    IndName normal;
    IndName position;
    IndName texCoord0;
    IndName material;
  };

  struct Mesh {
    std::string            name;
    std::vector<Primitive> primitives;
  };

  struct Node {
    long        mesh { 0 };
    std::string name;
  };

  struct Sampler {
    long magFilter { 0 };
    long minFilter { 0 };
    long wrapS     { 0 };
    long wrapT     { 0 };
  };

  struct Scene {
    std::string              name;
    std::vector<long>        nodeIds;
    std::vector<std::string> nodeNames;
  };

  struct Texture {
    IndName source;
    IndName sampler;
  };

  template<typename T>
  struct ArrayMap {
    std::vector<T>           array;
    std::map<std::string, T> map;

    bool isEmpty() const { return (array.empty() && map.empty()); }
  };

  struct JsonData {
    ArrayMap<Accessor>   accessors;
    Asset                asset;
    ArrayMap<BufferView> bufferViews;
    ArrayMap<Buffer>     buffers;
    ArrayMap<Image>      images;
    ArrayMap<Material>   materials;
    ArrayMap<Mesh>       meshes;
    ArrayMap<Node>       nodes;
    ArrayMap<Sampler>    samplers;
    IndName              scene;
    ArrayMap<Scene>      scenes;
    ArrayMap<Texture>    textures;
    std::vector<Chunk>   chunks;
  };

  struct Vec3 {
    float x;
    float y;
    float z;
  };

  struct Vec2 {
    float x;
    float y;
  };

  struct MeshData {
    std::vector<Vec3>   vec3;
    std::vector<Vec2>   vec2;
    std::vector<ushort> scalars;
  };

 private:
  bool parseJson(const std::string &str, JsonData &json) const;

  bool processData();

  bool getImageData(const Image &image, const uchar* &data, long &len);

  void printAccessor  (const Accessor   &accessor  , const IndName &indName) const;
  void printSampler   (const Sampler    &sampler   , int ia, const std::string &id) const;
  void printScene     (const Scene      &scene     , int ia, const std::string &id) const;
  void printTexture   (const Texture    &texture   , int ia, const std::string &id) const;
  void printMeshData  (const MeshData   &meshData          ) const;
  void printJsonData  (const JsonData   &jsonData          ) const;
  void printBufferView(const BufferView &bufferView, int ia, const std::string &id) const;
  void printBuffer    (const Buffer     &buffer    , int ia, const std::string &id) const;
  void printImage     (const Image      &image     , int ia, const std::string &id) const;
  void printMaterial  (const Material   &material  , int ia, const std::string &id) const;
  void printMesh      (const Mesh       &mesh      , int ia, const std::string &id) const;
  void printNode      (const Node       &node      , int ia, const std::string &id) const;
  void printPrimitive (const Primitive  &primitive , int ia) const;

  bool writeFile(const std::string &filename, const unsigned char *data, long len) const;

 private:
  using SceneP  = CAutoPtr<CGeomScene3D>;
  using ObjectP = CAutoPtr<CGeomObject3D>;

  CGeomScene3D*  scene_   { nullptr };
  SceneP         pscene_;
  CGeomObject3D* object_  { nullptr };
  ObjectP        pobject_;
  CFile*         file_    { nullptr };

  bool debug_ { false };
  bool saveImage_ { false };

  uint version_ { 0 };

  JsonData jsonData_;

  std::vector<MeshData>           meshDatas_;
  std::map<std::string, MeshData> meshDataMap_;
};

#endif
