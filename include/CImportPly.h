#ifndef CIMPORT_Ply_H
#define CIMPORT_Ply_H

#include <CImportBase.h>
#include <CGeomObject3D.h>
#include <CFile.h>

#include <set>

class CImportPly : public CImportBase {
 public:
  enum class FormatType {
    NONE,
    CHAR,
    UCHAR,
    SHORT,
    USHORT,
    INT,
    UINT,
    FLOAT,
    DOUBLE
  };

 public:
  static bool stringToFormat(const std::string &format, FormatType &formatType);

  CImportPly(CGeomScene3D *scene=nullptr, const std::string &name="ply");

 ~CImportPly();

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
  struct Vertex {
    double x  { 0.0 };
    double y  { 0.0 };
    double z  { 0.0 };
    double r  { 1.0 };
    double g  { 1.0 };
    double b  { 1.0 };
    double a  { 0.0 };
    double nx { 0.0 };
    double ny { 0.0 };
    double nz { 0.0 };
    double s  { 0.0 };
    double t  { 0.0 };
  };

  struct Face {
    std::vector<int> inds;
  };

  struct TexturePoint {
    uchar tx { 0 };
    float u  { 0.0f };
    float v  { 0.0f };
  };

  struct TextureFace {
    uchar            tx { 0 };
    uint             tn { 0 };
    std::vector<int> inds;
  };

  using Vertices      = std::vector<Vertex>;
  using Faces         = std::vector<Face>;
  using TexturePoints = std::vector<TexturePoint>;
  using TextureFaces  = std::vector<TextureFace>;

  struct Property {
    bool        list  { false };
    std::string name;
    std::string typeStr;
    FormatType  type  { FormatType::NONE };
    FormatType  type1 { FormatType::NONE };

    Property() { }

    Property(const std::string &n, const std::string &st, const FormatType &t) :
     name(n), typeStr(st), type(t) {
    }

    Property(bool l, const std::string &n, const std::string &st,
             const FormatType &t, const FormatType &t1) :
     list(l), name(n), typeStr(st), type(t), type1(t1) {
    }
  };

  using Properties = std::vector<Property>;

  using ElementNames      = std::vector<std::string>;
  using ElementCount      = std::map<std::string, int>;
  using ElementProperties = std::map<std::string, Properties>;

 private:
  bool readAscii();
  bool readBinary();

  Properties &getElementProperties(const std::string &name);

  bool readFloat(float *f) const;
  bool readInt  (int   *i) const;
  bool readUInt (uint  *i) const;
  bool readUChar(uchar *i) const;

 private:
  CGeomScene3D*  scene_   { nullptr };
  SceneP         pscene_;
  CGeomObject3D* object_  { nullptr };
  ObjectP        pobject_;
  CFile*         file_    { nullptr };

  ElementNames      elementNames_;
  ElementCount      elementCount_;
  ElementProperties elementProperties_;

  bool big_endian_ { false };

  Vertices      vertices_;
  Faces         faces_;
  TexturePoints texturePoints_;
  TextureFaces  textureFaces_;
};

#endif
