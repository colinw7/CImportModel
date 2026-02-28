#ifndef CIMPORT_DAE_H
#define CIMPORT_DAE_H

#include <CImportBase.h>
#include <CFile.h>
#include <CMatrix3D.h>

#include <cassert>
#include <memory>

class CXMLTag;

class CImportDAE : public CImportBase {
 public:
  CImportDAE(CGeomScene3D *scene=nullptr, const std::string &name="dae");

 ~CImportDAE() override;

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
  struct Image {
    std::string id;
    std::string name;
    std::string from;
    int         depth { -1 };

    void print() const {
      std::cerr << "Image: id=" << id << " name=" << name << " from=" << from << "\n";
    }
  };

  using ImageP = std::unique_ptr<Image>;

  struct Material {
    std::string              id;
    std::string              name;
    std::vector<std::string> effects;

    void print() const {
      std::cerr << "Image: id=" << id << " name=" << name << " effects=" << effects.size() << "\n";
    }
  };

  using MaterialP = std::unique_ptr<Material>;

  struct Surface {
    std::string type;
    std::string from;
  };

  struct Sampler2D {
    std::string sid;
    std::string source;
    std::string minfilter;
    std::string magfilter;
  };

  struct TextureWrap {
    std::string sid;
  };

  struct TextureTechnique {
    std::string profile;
  };

  struct Texture {
    std::string texture;
    std::string texcoord;

    std::string extra_type;

    double offsetU { 0.0 };
    double offsetV { 0.0 };
    double scaleU  { 0.0 };
    double scaleV  { 0.0 };
    double angle   { 0.0 };
  };

  struct LibraryEffect {
    std::string id;
    std::string name;

    std::map<std::string, Surface>   surfaces;
    std::map<std::string, Sampler2D> samplers;

    Texture diffuseTexture;
    Texture normalTexture;
    Texture emissionTexture;
    Texture specularTexture;
    Texture transparentTexture;

    std::string transparentOpaque;

    std::string technique_profile;

    void print() {
      std::cerr << "LibraryEffect: id=" << id << " name=" << name <<
                   " samplers=" <<samplers.size() <<
                   " diffuseTexture: " << diffuseTexture.texture <<
                   " normalTexture: " << normalTexture.texture <<
                   " emissionTexture:: " << emissionTexture.texture <<
                   " specularTexture: " << specularTexture.texture <<
                   " transparentTexture: " << transparentTexture.texture << "\n";
    }
  };

  using LibraryEffectP = std::unique_ptr<LibraryEffect>;

  struct NewParam {
    std::string sid;
  };

  struct Matrix {
    std::string sid;
    CMatrix3D   matrix;
  };

  struct Node {
    std::string id;
    std::string name;
    std::string sid;
    std::string type;

    int ind { -1 };

    std::string instanceController;
    std::string skeleton;

    Matrix matrix;

    CMatrix3D inverseBindMatrix;

    Node*               parent { nullptr };
    std::vector<Node *> children;

    void print(const std::string &indent) {
      std::cerr << indent << "Node: id=" << id << " name=" << name <<
                   " sid=" << sid << " type=" << type << " matrix=" << matrix.matrix << "\n";

      for (auto *node : children)
        node->print(indent + "  ");
    }
  };

  struct VisualScene {
    std::string id;
    std::string name;

    std::map<std::string, Node *> nodes;

    void print() {
      std::cerr << "VisualScene: id=" << id << " name=" << name << "\n";

      for (const auto &pn : nodes)
        pn.second->print("  ");
    }
  };

  struct Input {
    std::string semantic;
    std::string source;
    int         set    { -1 };
    int         offset { -1 };
  };

  struct Float {
    std::string sid;
    double      value;
  };

  struct Color {
    std::string sid;
    double      r;
    double      g;
    double      b;
    double      a;
  };

  struct FloatArray {
    std::string         id;
    int                 count { -1 };
    std::vector<double> values;
  };

  struct NameArray {
    std::string              id;
    int                      count { -1 };
    std::vector<std::string> values;
  };

  struct Param {
    std::string name;
    std::string type;
  };

  struct TechniqueCommon {
    int accessorCount  { -1 };
    int accessorStride { -1 };
    int accessorOffset { -1 };

    std::vector<Param> params;
    std::string        source;
  };

  struct Source {
    std::string id;
    std::string name;

    int count { -1 };

    FloatArray valueArray;
    NameArray  nameArray;

    TechniqueCommon techniqueCommon;
  };

  struct MeshSource : public Source {
    void print() const {
      std::cerr << "MeshSource: id=" << id << " values=" << valueArray.values.size() << "\n";
    //std::cerr << "count=" << count << "\n";
    //std::cerr << "stride=" << stride << "\n";
    //std::cerr << "params=" << techniqueCommon.params.size() << "\n";

      assert(int(valueArray.values.size()) ==
        techniqueCommon.accessorCount*techniqueCommon.accessorStride);
      assert(int(techniqueCommon.params.size()) == techniqueCommon.accessorStride);
    }
  };

  using MeshSourceP = std::unique_ptr<MeshSource>;

  enum class MeshVerticesInputType {
    NONE,
    POSITION,
    NORMAL
  };

  struct MeshVerticesInput : public Input {
    MeshVerticesInputType type { MeshVerticesInputType::NONE };
  };

  struct MeshVertices {
    std::string                    id;
    std::vector<MeshVerticesInput> inputs;

    void print() const {
      std::cerr << "MeshVertices: id=" << id << " inputs=" << inputs.size() << "\n";
    }
  };

  using MeshVerticesP = std::unique_ptr<MeshVertices>;

  enum class MeshPolyListInputType {
    NONE,
    VERTEX,
    NORMAL,
    TEXCOORD,
    COLOR
  };

  struct MeshPolyListInput : public Input {
    MeshPolyListInputType type { MeshPolyListInputType::NONE };
  };

  struct MeshTriangles {
    std::string                    material;
    int                            count      { -1 };
    std::vector<MeshPolyListInput> inputs;
    std::vector<int>               p;
    int                            max_offset { -1 };
    int                            max_set    { -1 };

    void print() const {
      std::cerr << "MeshTriangles: material=" << material << " count=" << count <<
        " inputs=" << inputs.size() << " p=" << p.size() <<
        " max_offset=" << max_offset << " max_set=" << max_set <<
        " p1=" << p.size()/(max_offset + 1) << "\n";
    }
  };

  using MeshTrianglesP = std::unique_ptr<MeshTriangles>;

  struct MeshPolyList {
    std::string                    material;
    int                            count      { -1 };
    std::vector<MeshPolyListInput> inputs;
    std::vector<int>               vcount;
    std::vector<int>               p;
    uint                           vsum       { 0 };
    int                            max_offset { -1 };
    int                            max_set    { -1 };

    void print() const {
      std::cerr << "MeshPolyList: material=" << material << " count=" << count <<
        " inputs=" << inputs.size() << " vcount=" << vcount.size() << " p=" << p.size() <<
        " max_offset=" << max_offset << " max_set=" << max_set <<
        " p1=" << p.size()/(max_offset + 1) << "\n";
    }
  };

  using MeshPolyListP = std::unique_ptr<MeshPolyList>;

  using IdMeshSource       = std::map<std::string, MeshSourceP>;
  using IdMeshVertices     = std::map<std::string, MeshVerticesP>;
  using MeshPolyLists      = std::vector<MeshPolyListP>;
  using MeshTrianglesArray = std::vector<MeshTrianglesP>;

  struct Geometry {
    std::string id;
    std::string name;

    IdMeshSource       idMeshSource;
    IdMeshVertices     idMeshVertices;
    MeshPolyLists      meshPolyLists;
    MeshTrianglesArray meshTrianglesArray;

    CGeomObject3D *object { nullptr };
  };

  struct SkinJoint {
    std::vector<Input> inputs;
  };

  struct SkinWeights {
    int                count;
    std::vector<Input> inputs;
    std::vector<int>   vcount;
    std::vector<int>   v;
  };

  struct Skin {
    std::string                   source;
    CMatrix3D                     bind_shape_matrix;
    std::map<std::string, Source> sources;
    std::vector<SkinJoint>        joints;
    std::vector<SkinWeights>      weights;
  };

  struct LibraryController {
    std::string id;
    std::string name;

    Skin skin;
  };

  struct AnimationChannel {
    std::string source;
    std::string target;
  };

  struct AnimationSampler {
    std::string id;

    std::vector<Input> inputs;
  };

  enum class AnimationInterpolation {
      NONE,
      LINEAR
    };

  struct AnimationData {
    std::vector<double>                 range;
    std::vector<CMatrix3D>              matrices;
    std::vector<AnimationInterpolation> interpolations;

    void clear() {
      range         .clear();
      matrices      .clear();
      interpolations.clear();
    }
  };

  struct Animation {
    std::string id;
    std::string name;

    std::vector<AnimationChannel>           channels;
    std::vector<Source>                     sources;
    std::map<std::string, AnimationSampler> samplers;

    AnimationData data;

    struct NodeData {
      Node*       node { nullptr };
      std::string data;
    };

    std::vector<NodeData> nodeDatas;
  };

  struct Scene {
    std::string url;
  };

 private:
  void addGeometry();
  void addGeometry(Geometry *geometry);

  bool processAnimation();
  bool processAnimation(LibraryController *controller);

  LibraryEffect* getMaterialEffect(const std::string &name) const;
  std::string    getEffectTexture(LibraryEffect *effect, const std::string &texture);

  bool readAsset(const std::string &parentName, CXMLTag *tag);

  bool readLibraryImages(const std::string &parentName, CXMLTag *tag);
  bool readLibraryImage(const std::string &parentName, CXMLTag *tag, ImageP &image);

  bool readLibraryMaterials(const std::string &parentName, CXMLTag *tag);
  bool readLibraryMaterial(const std::string &parentName, CXMLTag *tag, MaterialP &material);

  bool readLibraryEffects(const std::string &parentName, CXMLTag *tag);
  bool readLibraryEffect(const std::string &parentName, CXMLTag *tag, LibraryEffectP &effect);
  bool readLibraryEffectProfileCommon(const std::string &parentName, CXMLTag *tag,
                                      LibraryEffectP &effect);
  bool readLibraryEffectProfileParam(const std::string &parentName, CXMLTag *tag, NewParam &param,
                                     LibraryEffectP &effect);
  bool readLibraryEffectProfileTechnique(const std::string &parentName, CXMLTag *tag,
                                         LibraryEffectP &effect);
  bool readLibraryEffectProfilePhong(const std::string &parentName, CXMLTag *tag,
                                     LibraryEffectP &effect);
  bool readLibraryEffectProfileExtra(const std::string &parentName, CXMLTag *tag,
                                     LibraryEffectP &effect);
  bool readLibraryEffectProfileTechExtra(const std::string &parentName, CXMLTag *tag,
                                         LibraryEffectP &effect);
  bool readLibraryEffectProfileDisplacement(const std::string &parentName, CXMLTag *tag,
                                            LibraryEffectP &effect);
  bool readLibraryEffectProfileSurface(const std::string &parentName, CXMLTag *tag,
                                       Surface &surface);
  bool readLibraryEffectSampler2D(const std::string &parentName, CXMLTag *tag, NewParam &param,
                                  Sampler2D &sampler);

  bool readAmbient(const std::string &parentName, CXMLTag *tag);
  bool readDiffuse(const std::string &parentName, CXMLTag *tag, LibraryEffectP &effect);
  bool readEmission(const std::string &parentName, CXMLTag *tag, LibraryEffectP &effect);
  bool readSpecular(const std::string &parentName, CXMLTag *tag, LibraryEffectP &effect);
  bool readShininess(const std::string &parentName, CXMLTag *tag);
  bool readReflective(const std::string &parentName, CXMLTag *tag);
  bool readReflectivity(const std::string &parentName, CXMLTag *tag);
  bool readTransparent(const std::string &parentName, CXMLTag *tag, LibraryEffectP &effect);
  bool readTransparency(const std::string &parentName, CXMLTag *tag);

  bool readTexture(const std::string &parentName, CXMLTag *tag, Texture &texture);
  bool readTextureExtra(const std::string &parentName, CXMLTag *tag, Texture &texture);
  bool readTextureTechnique(const std::string &parentName, CXMLTag *tag, Texture &texture);
  bool readTextureWrapU(const std::string &parentName, CXMLTag *tag, Texture &texture);
  bool readTextureWrapV(const std::string &parentName, CXMLTag *tag, Texture &texture);
  bool readTextureBlendMode(const std::string &parentName, CXMLTag *tag, Texture &texture);
  bool readTextureOffsets(const std::string &parentName, CXMLTag *tag, Texture &texture);
  bool readTextureAmount(const std::string &parentName, CXMLTag *tag, Texture &texture);

  bool readLibraryGeometries(const std::string &parentName, CXMLTag *tag);
  bool readLibraryGeometry(const std::string &parentName, CXMLTag *tag);

  bool readGeometryMesh(const std::string &parentName, CXMLTag *tag, Geometry *geometry);
  bool readGeometryMeshSource(const std::string &parentName, CXMLTag *tag, MeshSourceP &meshSource);
  bool readGeometryMeshVertices(const std::string &parentName, CXMLTag *tag,
                                MeshVerticesP &meshVertices);
  bool readGeometryMeshPolylist(const std::string &parentName, CXMLTag *tag,
                                MeshPolyListP &polyList);
  bool readGeometryMeshTriangles(const std::string &parentName, CXMLTag *tag,
                                MeshTrianglesP &triangles);

  bool readTechniqueCommon(const std::string &parentName, CXMLTag *tag,
                           TechniqueCommon &techniqueCommon);
  bool readTechniqueCommonAccessor(const std::string &parentName, CXMLTag *tag,
                                   TechniqueCommon &techniqueCommon);

  bool readLibraryControllers(const std::string &parentName, CXMLTag *tag);
  bool readLibraryController(const std::string &parentName, CXMLTag *tag);
  bool readLibraryControllerSkin(const std::string &parentName, CXMLTag *tag, Skin &skin);
  bool readLibraryControllerSkinSource(const std::string &parentName, CXMLTag *tag, Skin &skin);
  bool readLibraryControllerSkinJoints(const std::string &parentName, CXMLTag *tag, Skin &skin);
  bool readLibraryControllerSkinWeights(const std::string &parentName, CXMLTag *tag, Skin &skin);

  bool readLibraryAnimations(const std::string &parentName, CXMLTag *tag);
  bool readLibraryAnimation(const std::string &parentName, CXMLTag *tag);
  bool readLibraryAnimationSource(const std::string &parentName, CXMLTag *tag,
                                  Animation &animation);
  bool readLibraryAnimationSampler(const std::string &parentName, CXMLTag *tag,
                                  Animation &animation);

  bool readLibraryVisualScenes(const std::string &parentName, CXMLTag *tag);
  bool readLibraryVisualScene(const std::string &parentName, CXMLTag *tag);

  bool readInput(const std::string &parentName, CXMLTag *tag, Input &input);
  bool readFloatArray(const std::string &parentName, CXMLTag *tag, FloatArray &array);
  bool readNameArray(const std::string &parentName, CXMLTag *tag, NameArray &array);
  bool readFloat(const std::string &parentName, CXMLTag *tag, Float &f);
  bool readColor(const std::string &parentName, CXMLTag *tag, Color &c);
  bool readNode(const std::string &parentName, CXMLTag *tag, VisualScene &scene, Node *node);
  bool readNodeInstanceController(const std::string &parentName, CXMLTag *tag, Node *node);
  bool readNodeBindMaterial(const std::string &parentName, CXMLTag *tag, Node *node);
  bool readNodeBindMaterialCommon(const std::string &parentName, CXMLTag *tag, Node *node);
  bool readScene(const std::string &parentName, CXMLTag *tag, Scene &scene);

  bool errorMsg(const std::string &name) const;

  void TODOOption(const std::string &name) const;
  void TODO(const std::string &name) const;
  void SKIP(const std::string &name) const;

 private:
  CGeomScene3D*  scene_   { nullptr };
  SceneP         pscene_;
  CGeomObject3D* object_  { nullptr };
  ObjectP        pobject_;

  CFile* file_ { nullptr };

  //---

  using IdImage             = std::map<std::string, ImageP>;
  using IdMaterialNameMap   = std::map<std::string, std::string>;
  using IdMaterial          = std::map<std::string, MaterialP>;
  using IdLibraryEffect     = std::map<std::string, LibraryEffectP>;
  using IdVisualScene       = std::map<std::string, VisualScene>;
  using IdGeometry          = std::map<std::string, Geometry *>;
  using IdAnimation         = std::map<std::string, Animation>;
  using IdLibraryController = std::map<std::string, LibraryController *>;

  // <library_images>
  IdImage idLibraryImage_;

  // <library_materials>
  IdMaterial        idLibraryMaterial_;
  IdMaterialNameMap idMaterialNameMap_;

  // <library_effects>
  IdLibraryEffect idLibraryEffect_;

  // <library_geometries>
  IdGeometry idLibraryGeometry_;

  // <library_controllers >
  IdLibraryController idLibraryController_;

  // <library_animations>
  IdAnimation idLibraryAnimation_;

  // <library_visual_scenes>
  IdVisualScene idLibraryVisualScene_;

  // <scene>
  Scene daeScene_;
};

#endif
