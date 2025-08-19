#ifndef CQNewGLCanvas_H
#define CQNewGLCanvas_H

#include <CQNewGLShaderProgram.h>

#include <CGeom3DType.h>
#include <CGLCamera.h>
#include <CGLVector3D.h>
#include <CPoint3D.h>
#include <CRGBA.h>

#include <QGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QMatrix4x4>

class CQNewGLModel;
class CQNewGLCamera;
class CQNewGLFont;
class CQNewGLText;

class CImportBase;
class CQGLBuffer;
class CGLTexture;
class CGeomObject3D;
class CGeomTexture;

class CQNewGLCanvas : public QGLWidget, protected QOpenGLFunctions_3_3_Core {
  Q_OBJECT

  Q_PROPERTY(QColor bgColor READ bgColor WRITE setBgColor)

  Q_PROPERTY(bool depthTest   READ isDepthTest   WRITE setDepthTest  )
  Q_PROPERTY(bool cullFace    READ isCullFace    WRITE setCullFace   )
  Q_PROPERTY(bool frontFace   READ isFrontFace   WRITE setFrontFace  )
  Q_PROPERTY(bool polygonLine READ isPolygonLine WRITE setPolygonLine)
  Q_PROPERTY(bool showNormals READ isShowNormals WRITE setShowNormals)

 public:
  enum class BonesTransform {
    INVERSE_BIND,
    LOCAL,
    GLOBAL
  };

  struct InitTextureData {
    std::string name;
    bool        flipV { false };
  };

  using InitTextureDatas = std::vector<InitTextureData>;

  struct TextureData {
    CGLTexture   *glTexture   { nullptr };
    CGeomTexture *geomTexture { nullptr };
  };

  using GLTextures = std::map<int, TextureData>;

  struct FaceData {
    int         boneId          { -1 };
    int         parentBoneId    { -1 };
    int         pos             { 0 };
    int         len             { 0 };
    CGLTexture *diffuseTexture  { nullptr };
    CGLTexture *specularTexture { nullptr };
    CGLTexture *normalTexture   { nullptr };
    CGLTexture *emissiveTexture { nullptr };
    CRGBA       ambient         { 0, 0, 0 };
    CRGBA       diffuse         { 1, 1, 1 };
    CRGBA       specular        { 0, 0, 0 };
    CRGBA       emission        { 0, 0, 0, 0 };
    double      shininess       { 1.0 };
  };

  using FaceDatas = std::vector<FaceData>;

  struct ObjectDrawData {
    CQGLBuffer *buffer { nullptr };
    FaceDatas   faceDatas;
  };

  struct ObjectData {
    CGeomObject3D* object { nullptr };
    ObjectDrawData modelData;
    ObjectDrawData normalData;
    ObjectDrawData bonesData;
    ObjectDrawData boneData;
  };

  struct Light {
    QString     name;
    CGLVector3D pos    { 0.4f, 0.4f, 0.4f };
    QColor      color  { 255, 255, 255 };
    CQGLBuffer* buffer { nullptr };
  };

  using Lights = std::vector<Light *>;

  struct LoadData {
    bool invertX { false };
    bool invertY { false };
    bool invertZ { false };

    LoadData() { }
  };

 public:
  CQNewGLCanvas(CQNewGLModel *app);

  CQNewGLModel* app() const { return app_; }

  CImportBase* importBase() const { return import_; }

  CQNewGLCamera *camera() const { return camera_; }

  bool isFlipYZ() const { return flipYZ_; }
  void setFlipYZ(bool b);

  bool isInvertDepth() const { return invertDepth_; }
  void setInvertDepth(bool b);

  bool isShowAxis() const { return showAxis_; }
  void setShowAxis(bool b);

  bool isOrtho() const { return ortho_; }
  void setOrtho(bool b);

  double time() const { return time_; }
  void setTime(double t) { time_ = t; }

  const QColor &bgColor() const { return bgColor_; }
  void setBgColor(const QColor &c);

  const QColor &ambientColor() const { return ambientColor_; }
  void setAmbientColor(const QColor &c);

  const QColor &diffuseColor() const { return diffuseColor_; }
  void setDiffuseColor(const QColor &c);

  const QColor &emissionColor() const { return emissionColor_; }
  void setEmissionColor(const QColor &c);

  //---

  bool isDepthTest() const { return depthTest_; }
  void setDepthTest(bool b);

  bool isCullFace() const { return cullFace_; }
  void setCullFace(bool b);

  bool isFrontFace() const { return frontFace_; }
  void setFrontFace(bool b);

  bool isPolygonLine() const { return polygonLine_; }
  void setPolygonLine(bool b);

  bool isShowNormals() const { return showNormals_; }
  void setShowNormals(bool b);

  //---

  const CVector3D &modelTranslate() const { return modelTranslate_; }
  void setModelTranslate(const CVector3D &v) { modelTranslate_ = v; }

  const CVector3D &modelRotate() const { return modelRotate_; }
  void setModelRotate(const CVector3D &v) { modelRotate_ = v; }

  //---

  const Lights &lights() { return lights_; }

  int currentLight() const { return currentLight_; }
  void setCurrentLight(int i);

  CGLVector3D lightPos(int i) const;
  QColor lightColor(int i) const;

  CGLVector3D currentLightPos() const { return lightPos(currentLight()); }
  void setCurrentLightPos(const CGLVector3D &v);

  QColor currentLightColor() const { return lightColor(currentLight()); }
  void setCurrentLightColor(const QColor &c);

  //---

  bool isAnimEnabled() const { return animEnabled_; }
  void setAnimEnabled(bool b) { animEnabled_ = b; }

  const QString &animName() const { return animName_; }
  void setAnimName(const QString &s) { animName_ = s; }

  bool isBonesEnabled() const { return bonesEnabled_; }
  void setBonesEnabled(bool b) { bonesEnabled_ = b; }

  const BonesTransform &bonesTransform() const { return bonesTransform_; }
  void setBonesTransform(BonesTransform t) { bonesTransform_ = t; }

  //---

  const CGLMatrix3D &projectionMatrix() const { return paintData_.projection; }
  const CGLMatrix3D &viewMatrix() const { return paintData_.view; }

  CMatrix3D getModelMatrix() const;

  //---

  bool loadModel(const QString &modelName, CGeom3DType format,
                 const LoadData &loadData=LoadData());

  void setInitTextures(const InitTextureDatas &textures);
  void setTextureMap(const std::string &map);

  void initializeGL() override;

  void paintGL() override;

  void resizeGL(int, int) override;

  void resetRange();

  //---

  void addLight(bool update=true);

  bool createFontTexture(uint *texture, int w, int h, uchar *data);

  //---

  void mousePressEvent  (QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent   (QMouseEvent *event) override;

  void wheelEvent(QWheelEvent *) override;

  void keyPressEvent(QKeyEvent *event) override;

  void updateObjectData();
  void updateModelData();
  void updateBonesData();
  void updateBoneData();
  void updateAxis();

  const GLTextures &glTextures() const { return glTextures_; }

  CGLTexture   *getTextureByName    (const std::string &name) const;
  CGeomTexture *getGeomTextureByName(const std::string &name) const;

  CGLTexture *getTexture(CGeomTexture *texture, bool add);

 Q_SIGNALS:
  void modelMatrixChanged();

 private:
  void loadInitTextures();

  GLuint loadTexture(const std::string &name);

  void updateLightBuffer();

  void drawObjectModel(ObjectData *objectData);
  void drawObjectBones(ObjectData *objectData);
  void drawObjectBone (ObjectData *objectData);

  void drawAxis();
  void drawLight();

  void drawNormals(ObjectData *objectData);

  CMatrix3D getMeshGlobalTransform(ObjectData *objectData, bool invert) const;
  CMatrix3D getMeshLocalTransform (ObjectData *objectData, bool invert) const;

  void updateNodeMatrices(ObjectData *objectData);

  CGeomObject3D *getRootObject(CGeomObject3D *object) const;

 private:
  struct PaintData {
    CGLVector3D viewPos;
    CGLMatrix3D projection;
    CGLMatrix3D view;

    std::vector<QMatrix4x4> nodeMatrices;
    int                     numNodeMatrices { 128 };
  };

  struct TextureMapData {
    std::string diffuse;
    std::string normal;
    std::string specular;
    std::string emissive;
  };

  using ObjectDatas = std::map<CGeomObject3D*, ObjectData *>;
  using TextureMap  = std::map<std::string, TextureMapData>;
  using Texts       = std::vector<CQNewGLText *>;

  CQNewGLModel* app_           { nullptr };
  bool          initialized_   { false };
  CImportBase*  import_        { nullptr };
  QColor        bgColor_       { 51, 76, 76 };
  QColor        ambientColor_  { 100, 100, 100 };
  QColor        diffuseColor_  { 255, 255, 255 };
  QColor        emissionColor_ { 0, 0, 0 };
  bool          depthTest_     { true };
  bool          cullFace_      { false };
  bool          frontFace_     { true };
  bool          polygonLine_   { false };
  bool          showNormals_   { false };
  bool          flipYZ_        { false };
  bool          showAxis_      { false };
  bool          invertDepth_   { false };
  bool          ortho_         { false };
  double        time_          { 0.0 };

  // scene
  CVector3D sceneSize_     { 1.0, 1.0, 1.0 };
  double    sceneScale_    { 1.0 };
  float     invSceneScale_ { 1.0f };
  CPoint3D  sceneCenter_   { 0, 0, 0 };

  PaintData paintData_;

  // camera
  CQNewGLCamera* camera_ { nullptr };

  // model
  CVector3D modelRotate_    { 0.0, 0.0, 0.0 };
  CVector3D modelScale_     { 1.0, 1.0, 1.0 };
  CVector3D modelTranslate_ { 1.0, 1.0, 1.0 };

  CQNewGLShaderProgram* modelShaderProgram_  { nullptr };
  CQNewGLShaderProgram* normalShaderProgram_ { nullptr };

  ObjectDatas objectDatas_;

  // light
  CQNewGLShaderProgram* lightShaderProgram_ { nullptr };

  Lights lights_;
  int    currentLight_ { 0 };

  // anim
  bool    animEnabled_ { true };
  QString animName_;

  // bones
  bool           bonesEnabled_   { false };
  BonesTransform bonesTransform_ { BonesTransform::INVERSE_BIND };

  CQNewGLShaderProgram* bonesShaderProgram_ { nullptr };
  CQNewGLShaderProgram* boneShaderProgram_  { nullptr };

  // axis
  CQNewGLShaderProgram* axisShaderProgram_ { nullptr };
  FaceDatas             axisFaceDatas_;

  Texts axisTexts_;

  // textures
  InitTextureDatas initTextures_;
  TextureMap       textureMap_;
  GLTextures       glTextures_;

  // font
  CQNewGLFont* font_ { nullptr };
//CQNewGLText* text_ { nullptr };

  // mouse
  double mousePressX_ { 0.0 };
  double mousePressY_ { 0.0 };
  double mouseMoveX_  { 0.0 };
  double mouseMoveY_  { 0.0 };
};

//---

class CQNewGLCamera : public CGLCamera {
 public:
  CQNewGLCamera(CQNewGLCanvas *canvas, const CGLVector3D &v);

  void viewChanged() override;

  void reset();

 private:
  CQNewGLCanvas* canvas_ { nullptr };
  CGLVector3D    v_;
};

#endif
