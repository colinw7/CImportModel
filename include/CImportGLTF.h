#ifndef CImportGLTF_H
#define CImportGLTF_H

#include <CImportBase.h>
#include <CGeomObject3D.h>
#include <CGLMatrix3D.h>
#include <CFile.h>

#include <string>
#include <vector>
#include <optional>
#include <iostream>

class CImportGLTF : public CImportBase {
 public:
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

  struct Vec4 {
    float x { 0.0f };
    float y { 0.0f };
    float z { 0.0f };
    float w { 0.0f };

    Vec4() { }

    Vec4(float x1, float y1, float z1, float w1) :
     x(x1), y(y1), z(z1), w(w1) {
    }

    friend Vec4 operator+(const Vec4 &lhs, const Vec4 &rhs) {
      return Vec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
    }

    friend Vec4 operator-(const Vec4 &lhs, const Vec4 &rhs) {
      return Vec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
    }

    friend Vec4 operator*(float lhs, const Vec4 &rhs) {
      return Vec4(lhs*rhs.x, lhs*rhs.y, lhs*rhs.z, lhs*rhs.w);
    }

    friend std::ostream &operator<<(std::ostream &os, const Vec4 &v) {
      os << v.x << "," << v.y << "," << v.z << "," << v.w;
      return os;
    }
  };

  using OptVec4 = std::optional<Vec4>;

  struct Vec3 {
    float x { 0.0f };
    float y { 0.0f };
    float z { 0.0f };

    Vec3() { }

    Vec3(float x1, float y1, float z1) :
     x(x1), y(y1), z(z1) {
    }

    friend Vec3 operator+(const Vec3 &lhs, const Vec3 &rhs) {
      return Vec3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
    }

    friend Vec3 operator-(const Vec3 &lhs, const Vec3 &rhs) {
      return Vec3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
    }

    friend Vec3 operator*(float lhs, const Vec3 &rhs) {
      return Vec3(lhs*rhs.x, lhs*rhs.y, lhs*rhs.z);
    }

    friend std::ostream &operator<<(std::ostream &os, const Vec3 &v) {
      os << v.x << "," << v.y << "," << v.z;
      return os;
    }
  };

  using OptVec3 = std::optional<Vec3>;

  struct Vec2 {
    float x { 0.0f };
    float y { 0.0f };

    friend std::ostream &operator<<(std::ostream &os, const Vec2 &v) {
      os << v.x << "," << v.y;
      return os;
    }
  };

  struct Mat4 {
    float       v[16];
    CGLMatrix3D m;

    Mat4() { m.setIdentity(); }

    Mat4(const float v1[16]) {
      for (int i = 0; i < 16; ++i)
        v[i] = v1[i];

      calcMatrix();
    }

    void calcMatrix() {
      m = CGLMatrix3D(&v[0], 16);
    }

    const CGLMatrix3D &matrix() const { return m; }

    friend std::ostream &operator<<(std::ostream &os, const Mat4 &v) {
      for (int i = 0; i < 16; ++i)
        os << " " << v.v[i];
      return os;
    }

  };

  using OptMat4 = std::optional<Mat4>;

 public:
  CImportGLTF(CGeomScene3D *scene=nullptr, const std::string &name="gltf");

 ~CImportGLTF();

  bool isDebug() const { return debug_; }
  void setDebug(bool b) { debug_ = b; }

  bool isDebugData() const { return debugData_; }
  void setDebugData(bool b) { debugData_ = b; }

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
  using OptReal = std::optional<double>;
  using OptLong = std::optional<long>;

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

    std::string to_string() const {
      if (ind  >= 0 ) return std::to_string(ind);
      else if (name != "") return name;
      return "";
    }
  };

  struct IndData {
    IndName indName;
  };

  struct Accessor : IndData {
    std::string         name;
    IndName             bufferView;
    long                byteOffset    { -1 };
    long                byteStride    { -1 };
    std::string         type;
    long                componentType { -1 };
    long                count         { -1 };
    std::vector<double> min;
    std::vector<double> max;
    bool                normalized    { false };

    bool    sparse                     { false };
    long    sparseCount                { -1 };
    IndName sparseIndicesBufferView;
    long    sparseIndicesByteOffset    { -1 };
    long    sparseIndicesComponentType { -1 };
    IndName sparseValuesBufferView     { -1 };
    long    sparseValuesByteOffset     { -1 };
  };

  struct Asset {
    bool        set { false };
    std::string generator;
    std::string versionId;
    long        versionNumber { -1 };
    std::string copyright;
  };

  struct BufferView : IndData {
    std::string name;
    IndName     buffer;
    long        byteOffset { -1 };
    long        byteLength { -1 };
    long        byteStride { -1 };
    long        target     { -1 };
  };

  struct Buffer : IndData {
    std::string name;
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

    mutable CImagePtr image;
  };

  using OptColor = std::optional<Color>;

  struct MaterialTexture {
    long      index    { -1 };
    OptLong   texCoord;
    OptReal   scale;
    OptReal   strength;
    OptReal   rotation;
    CVector2D scaleVector;
    CVector2D offsetVector;
  };

  struct Material : IndData {
    std::string name;
    std::string technique;

    OptReal         metallicFactor;
    OptReal         roughnessFactor;
    MaterialTexture normalTexture;
    MaterialTexture occlusionTexture;
    MaterialTexture metallicRoughnessTexture;

    OptColor ambient;
    OptColor diffuse;
    OptReal  shininess;
    OptReal  transparency;

    // base color
    MaterialTexture baseColorTexture;
    OptColor        baseColorFactor;

    // specular
    MaterialTexture specularTexture;
    OptReal         specularFactor;
    OptColor        specular;
    OptColor        specularColorFactor;

    // emission
    MaterialTexture emissiveTexture;
    OptReal         emissiveStrength;
    OptVec3         emissiveFactor;
    OptColor        emission;

    // transmission
    OptReal         transmissionFactor;
    MaterialTexture transmissionTexture;

    // attenuation
    OptColor attenuationColor;
    OptReal  attenuationDistance;

    // thickness
    OptReal         thicknessFactor;
    MaterialTexture thicknessTexture;

    bool        doubleSided { false };
    std::string alphaMode;
    OptReal     alphaCutoff;

    OptReal         diffuseTransmissionFactor;
    MaterialTexture diffuseTransmissionTexture;
    OptColor        diffuseTransmissionColorFactor;
    MaterialTexture diffuseTransmissionColorTexture;

    // anisotropy
    OptReal         anisotropyStrength;
    OptReal         anisotropyRotation;
    MaterialTexture anisotropyTexture;

    // clearcoat
    OptReal         clearcoatFactor;
    OptReal         clearcoatRoughnessFactor;
    MaterialTexture clearcoatTexture;
    MaterialTexture clearcoatNormalTexture;
    MaterialTexture clearcoatRoughnessTexture;

    // sheen
    OptColor        sheenColorFactor;
    MaterialTexture sheenColorTexture;
    OptReal         sheenRoughnessFactor;
    MaterialTexture sheenRoughnessTexture;

    // iridescence
    OptReal         iridescenceFactor;
    OptReal         iridescenceIor;
    MaterialTexture iridescenceTexture;
    MaterialTexture iridescenceThicknessTexture;
    OptReal         iridescenceThicknessMinimum;
    OptReal         iridescenceThicknessMaximum;

    OptReal dispersion;
    OptReal ior;

    MaterialTexture diffuseTexture;
    MaterialTexture specularGlossinessTexture;
  };

  struct Primitive {
    long    mode      { -1 }; // default triangles
    IndName indices;
    IndName normal;
    IndName position;
    IndName texCoord0;
    IndName texCoord1;
    IndName texCoord2;
    IndName material;
    IndName color0;
    IndName joints0;
    IndName weights0;
    IndName tangent;
  };

  struct Mesh : IndData {
    std::string            name;
    std::vector<Primitive> primitives;
  };

  using OptGLMatrix = std::optional<CGLMatrix3D>;
  using OptMatrix   = std::optional<CMatrix3D>;

  struct Node : IndData {
    // values from "nodes"
    std::string          name;          // "name"
    IndName              mesh;          // "mesh"
    OptGLMatrix          matrix;        // "matrix"
    IndName              skin;          // "skin"
    OptVec4              rotation;      // "rotation"
    OptVec3              scale;         // "scale"
    OptVec3              translation;   // "translation"
    long                 camera { -1 }; // "camera"
    std::vector<IndName> children;      // "children"

    // inverseBindMatrix from skins
    OptMat4 inverseBindMatrix;

    // combined translation, rotation, scale matrix
    CMatrix3D transform     { CMatrix3D::identity() };
    CMatrix3D ttransform    { CMatrix3D::identity() };
    CMatrix3D rtransform    { CMatrix3D::identity() };
    CMatrix3D stransform    { CMatrix3D::identity() };
    CMatrix3D hierTranslate { CMatrix3D::identity() };
    CMatrix3D hierTransform { CMatrix3D::identity() };

    Node* parent { nullptr };
    int   depth  { -1 };

    bool isJoint { false };
    bool added   { false };
    bool skinned { false };

    CGeomObject3D *object { nullptr };
  };

  struct Sampler : IndData {
    std::string    name;
    long magFilter { 0 };
    long minFilter { 0 };
    long wrapS     { 0 };
    long wrapT     { 0 };
  };

  struct Scene : IndData {
    std::string          name;
    std::vector<IndName> nodes;
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

  struct Camera : IndData {
    std::string name;
    std::string type;
    double      xmag        { 0.0 };
    double      ymag        { 0.0 };
    double      yfov        { -1.0 };
    double      znear       { -1.0 };
    double      zfar        { -1.0 };
    double      aspectRatio { 1.0 };
  };

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

  struct AnimationChannel : IndData {
    IndName     node;
    std::string path;
    long        sampler { -1 };
  };

  using AnimationInterpolation = CGeomAnimationData::Interpolation;

  struct AnimationSampler : IndData {
    long                   input { -1 };
    long                   output { -1 };
    std::string            interpolation;
    AnimationInterpolation interpType;
  };

  struct Animation : IndData {
    using Channels      = std::vector<AnimationChannel>;
    using ChannelsArray = std::vector<Channels>;

    std::string                  name;
    IndNameMap<AnimationSampler> samplers;
    ChannelsArray                channelsArray;
  };

  using Animations = std::vector<Animation>;

  struct Skin : IndData {
    long              inverseBindMatrices { -1 };
    std::vector<long> joints;
    std::string       name;
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

  struct JsonData {
    IndNameMap<Accessor>     accessors;
    Asset                    asset;
    IndNameMap<BufferView>   bufferViews;
    IndNameMap<Buffer>       buffers;
    IndNameMap<Image>        images;
    IndNameMap<Material>     materials;
    IndNameMap<Mesh>         meshes;
    IndNameMap<Node>         nodes;
    IndNameMap<Sampler>      samplers;
    IndName                  scene;
    IndNameMap<Scene>        scenes;
    IndNameMap<Texture>      textures;
    IndNameMap<Camera>       cameras;
    std::vector<Chunk>       chunks;
    std::vector<std::string> extensionsUsed;
    Animations               animations;
    IndNameMap<Skin>         skins;
  };

  struct AccessorData {
    const uchar *data       { nullptr };
    int          byteStride { 0 };
    long         byteLength { 0 };
  };

  struct MeshData : IndData {
    std::string         type;
    long                componentType { -1 };
    long                count { 0 };
    std::vector<double> min;
    std::vector<double> max;
    std::vector<Vec4>   vec4;
    std::vector<Vec3>   vec3;
    std::vector<Vec2>   vec2;
    std::vector<long>   iscalars;
    std::vector<float>  fscalars;
    std::vector<Mat4>   mat4;
  };

 private:
  bool parseJson(const std::string &str, JsonData &json) const;

  bool processData();

  bool processSparseAccessor(const IndName &indName, const Accessor &accessor);
  bool processAccessor(const IndName &indName, const Accessor &accessor);

  bool readMeshData(const AccessorData &accessorData, MeshData &meshData) const;

  const uchar *getAccessorBufferData(const IndName &accessorBufferView, long accessorByteOffset,
                                     long &byteLength, int &byteStride);

  float  readFloat(const uchar *data, long i) const;
  ushort readShort(const uchar *data, long i) const;
  uint   readInt  (const uchar *data, long i) const;
  uchar  readByte (const uchar *data, long i) const;

  bool getChunk(long ind, Chunk* &chunk) const;

  void processSkins();

  bool getSkin(const IndName &indName, Skin &skin) const;

  void updateHier();
  void addNodeChildren(Node *node);

  bool processAnim();
  bool updateAnim(const std::string &name, double t);

  bool processNodeTransform(Node *node, int depth);
  void processNodeHierTransform(Node *node);

  bool processNodeMesh(Node *node);

  bool createNodeObject(Node *node, const CMatrix3D &m1);
  void createNodeObj(Node *node);

  CMatrix3D calcNodeTransform(Node *node) const;

  CMatrix3D calcNodeGlobalTransform(Node *node) const;

  bool processMesh(CGeomObject3D *object, const Mesh &mesh);

  bool resolveImage(const Image &image);
  bool getImageData(const Image &image, const uchar* &data, long &len);

  bool getBufferView(const IndName &indName, BufferView &bufferView) const;
  bool getBuffer    (const IndName &indName, Buffer &buffer) const;
  bool getMaterial  (const IndName &indName, Material &material) const;
  bool getTexture   (const IndName &indName, Texture &texture) const;
  bool getNode      (const IndName &indName, Node* &node) const;
  bool getAccessor  (const IndName &indName, Accessor &accessor) const;
  bool getMesh      (const IndName &indName, Mesh &mesh) const;

  const MeshData *getMeshData(const IndName &indName) const;

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
  void printSkin      (const Skin       &skin      , const IndName &indName) const;

  bool writeFile(const std::string &filename, const unsigned char *data, long len) const;

 private:
  CGeomScene3D*  scene_   { nullptr };
  SceneP         pscene_;
  CGeomObject3D* object_  { nullptr };
  ObjectP        pobject_;
  CFile*         file_    { nullptr };

  CGeomObject3D *rootObject_ { nullptr };

  bool binary_ { false };

  bool debug_     { false };
  bool debugData_ { false };
  bool saveImage_ { false };

  uint version_ { 0 };

  JsonData jsonData_;

  IndNameMap<MeshData> meshDatas_;

  struct UriData {
    uchar *data { nullptr };
    uint   len { 0 };

    UriData() { }

    UriData(uchar *d, uint l) :
     data(d), len(l) {
    }
  };

  using UriDataMap = std::map<std::string, UriData>;

  mutable UriDataMap uriDataMap_;

  struct JointNode {
    int     ind    { 0 };
    float   weight { 0.0f };
    OptMat4 inverseBindMatrix;
  };

  struct JointData {
    JointNode   nodes[4];
    CGLMatrix3D skinMatrix;
    Vec4        w;
  };

  using ObjectJoints = std::map<int, JointData>;

  ObjectJoints objectJoints_;
};

#endif
