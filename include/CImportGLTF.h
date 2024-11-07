#ifndef CImportGLTF_H
#define CImportGLTF_H

#include <CImportBase.h>
#include <CGeomObject3D.h>
#include <CGLMatrix3D.h>
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
  bool readBin ();
  bool readJson();

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

    friend bool operator<(const IndName &lhs, const IndName &rhs) {
      if (lhs.ind  >= 0  && rhs.ind  >= 0 ) return (lhs.ind  < rhs.ind );
      if (lhs.name != "" && rhs.name != "") return (lhs.name < rhs.name);
      return (lhs.ind >= 0);
    }

    friend std::ostream &operator<<(std::ostream &os, const IndName &i) {
      if      (i.ind  >= 0 ) os << i.ind;
      else if (i.name != "") os << i.name;
      return os;
    }

    bool isEmpty() const { return (ind < 0 && name == ""); }
  };

  struct IndData {
    IndName indName;
  };

  struct Accessor : IndData {
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
    std::string versionId;
    long        versionNumber { -1 };
  };

  struct BufferView : IndData {
    IndName buffer;
    long    byteOffset { -1 };
    long    byteLength { -1 };
    long    byteStride { -1 };
    long    target     { -1 };
  };

  struct Buffer : IndData {
    long        byteLength { 0 };
    std::string uri;
    std::string type;
  };

  struct Image : IndData {
    IndName     bufferView;
    std::string mimeType;
    std::string name;
    std::string url;
    std::string uri;
    long        width { -1 };
    long        height { -1 };
  };

  struct Color {
    double r { 0.0 };
    double g { 0.0 };
    double b { 0.0 };
    double a { 1.0 };

    std::string name;

    Color() { }

    Color(double r1, double g1, double b1, double a1) :
     r(r1), g(g1), b(b1), a(a1) {
    }

    Color(const std::string &s) :
     name(s) {
    }

    friend std::ostream &operator<<(std::ostream &os, const Color &c) {
      os << c.r << "," << c.g << "," << c.b << "," << c.a;
      return os;
    }
  };

  struct MaterialTexture {
    long index    { -1 };
    long texCoord { -1 };
  };

  struct Material : IndData {
    std::string     name;
    double          metallicFactor { 0 };
    double          roughnessFactor { 0 };
    MaterialTexture normalTexture;
    MaterialTexture baseColorTexture;
    std::string     technique;
    Color           ambient;
    Color           diffuse;
    Color           emission;
    Color           specular;
    double          shininess { 0.0 };
    double          transparency { 0.0 };
  };

  struct Primitive {
    long    mode      { -1 }; // default triangles
    IndName indices;
    IndName normal;
    IndName position;
    IndName texCoord0;
    IndName texCoord1;
    IndName material;
  };

  struct Mesh : IndData {
    std::string            name;
    std::vector<Primitive> primitives;
  };

  struct Node : IndData {
    long        mesh { 0 };
    std::string name;
    CGLMatrix3D matrix;
  };

  struct Sampler : IndData {
    long magFilter { 0 };
    long minFilter { 0 };
    long wrapS     { 0 };
    long wrapT     { 0 };
  };

  struct Scene : IndData {
    std::string              name;
    std::vector<long>        nodeIds;
    std::vector<std::string> nodeNames;
  };

  struct Texture : IndData {
    IndName     source;
    IndName     sampler;
    long        format { -1 };
    long        internalFormat { -1 };
    long        target { -1 };
    long        type { -1 };
    std::string name;
  };

#if 0
  template<typename T>
  struct ArrayMap {
    std::vector<T>           array;
    std::map<std::string, T> map;

    bool isEmpty() const { return (array.empty() && map.empty()); }

    void add(T &d, const IndName &indName) {
      d.indName = indName;

      if (indName.name != "")
        map[indName.name] = d;
      else {
        d.indName.ind = array.size();

        array.push_back(d);
      }
    }
  };
#endif

  template<typename T>
  struct IndNameMap : public std::map<IndName, T> {
    int lastInd { 0 };

    bool isEmpty() const { return (*this).empty(); }

    void add(T &d, const IndName &indName) {
      d.indName = indName;

      if (indName.name == "")
        d.indName.ind = lastInd++;

      (*this)[d.indName] = d;
    }
  };

  struct JsonData {
    IndNameMap<Accessor>   accessors;
    Asset                  asset;
    IndNameMap<BufferView> bufferViews;
    IndNameMap<Buffer>     buffers;
    IndNameMap<Image>      images;
    IndNameMap<Material>   materials;
    IndNameMap<Mesh>       meshes;
    IndNameMap<Node>       nodes;
    IndNameMap<Sampler>    samplers;
    IndName                scene;
    IndNameMap<Scene>      scenes;
    IndNameMap<Texture>    textures;
    std::vector<Chunk>     chunks;
  };

  struct Vec4 {
    float x;
    float y;
    float z;
    float w;
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

  struct MeshData : IndData {
    std::vector<Vec4>   vec4;
    std::vector<Vec3>   vec3;
    std::vector<Vec2>   vec2;
    std::vector<ushort> scalars;
  };

 private:
  bool parseJson(const std::string &str, JsonData &json) const;

  bool processData();

  bool getImageData(const Image &image, const uchar* &data, long &len);

  bool getBufferView(const IndName &indName, BufferView &bufferView) const;
  bool getMaterial  (const IndName &indName, Material &material) const;
  bool getTexture   (const IndName &indName, Texture &texture) const;

  void printAccessor  (const Accessor   &accessor  , const IndName &indName) const;
  void printSampler   (const Sampler    &sampler   , const IndName &indName) const;
  void printScene     (const Scene      &scene     , const IndName &indName) const;
  void printTexture   (const Texture    &texture   , const IndName &indName) const;
  void printMeshData  (const MeshData   &meshData                          ) const;
  void printJsonData  (const JsonData   &jsonData                          ) const;
  void printBufferView(const BufferView &bufferView, const IndName &indName) const;
  void printBuffer    (const Buffer     &buffer    , const IndName &indName) const;
  void printImage     (const Image      &image     , const IndName &indName) const;
  void printMaterial  (const Material   &material  , const IndName &indName) const;
  void printMesh      (const Mesh       &mesh      , const IndName &indName) const;
  void printNode      (const Node       &node      , const IndName &indName) const;
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

  bool debug_     { false };
  bool saveImage_ { false };

  uint version_ { 0 };

  JsonData jsonData_;

  IndNameMap<MeshData> meshDatas_;
};

#endif
