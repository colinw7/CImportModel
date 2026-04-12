#ifndef Canvas_H
#define Canvas_H

#include <FaceData.h>

#include <CImagePtr.h>
#include <CBBox3D.h>
#include <CPoint2D.h>

#include <QGLWidget>
#include <QOpenGLExtraFunctions>

#include <optional>

class CQGLBuffer;
class CQGLTexture;
class CGeomTexture;
class CGLCameraIFace;

namespace CQShadow3D {

class App;
class Camera;
class Light;
class FaceData;
class ShaderProgram;
class Texture;

class Canvas : public QGLWidget, public QOpenGLExtraFunctions {
  Q_OBJECT

 public:
  enum class EditType {
    CAMERA,
    LIGHT
  };

  enum class ShaderType {
    SCENE,
    SHADOW,
    SHADOW_CUBE_MAP
  };

 private:
  struct TextureBuffer;

 public:
  Canvas(App *app);

  //---

  App *app() const { return app_; }

  int ind() const { return ind_; }
  void setInd(int i) { ind_ = i; }

  //---

  Camera *camera() const { return camera_; }

  //---

  // lights
  bool isFixedDiffuse() const { return fixedDiffuse_; }
  void setFixedDiffuse(bool b) { fixedDiffuse_ = b; }

  Light *light() const { return light_; }

  //---

  ShaderProgram *shaderProgram();

  ShaderProgram *sceneShaderProgram();
  ShaderProgram *skyboxShaderProgram();
  ShaderProgram *shadowShaderProgram();
  ShaderProgram *shadowCubeMapShaderProgram();
  ShaderProgram *textureShaderProgram();
  ShaderProgram *textureCubeMapShaderProgram();

  //---

  int pixelWidth() const { return pixelWidth_; }
  void setPixelWidth(int i) { pixelWidth_ = i; }

  int pixelHeight() const { return pixelHeight_; }
  void setPixelHeight(int i) { pixelHeight_ = i; }

  double aspect() const { return aspect_; }
  void setAspect(double r) { aspect_ = r; }

  //---

  const QColor &bgColor() const { return bgColor_; }
  void setBgColor(const QColor &c) { bgColor_ = c; }

  //---

  bool isDepthTest() const { return depthTest_; }
  void setDepthTest(bool b) { depthTest_ = b; }

  bool isCullFace() const { return cullFace_; }
  void setCullFace(bool b) { cullFace_ = b; }

  bool isFrontFace() const { return frontFace_; }
  void setFrontFace(bool b) { frontFace_ = b; }

  bool isPolygonLine() const { return polygonLine_; }
  void setPolygonLine(bool b) { polygonLine_ = b; }

  double pointSize() const { return pointSize_; }
  void setPointSize(double r) { pointSize_ = r; update(); }

  double lineWidth() const { return lineWidth_; }
  void setLineWidth(double r) { lineWidth_ = r; update(); }

  //---

  bool isTextured() const { return textured_; }
  void setTextured(bool b) { textured_ = b; }

  //---

  const EditType &editType() const { return editType_; }
  void setEditType(const EditType &v) { editType_ = v; }

  const ShaderType &shaderType() const { return shaderType_; }
  void setShaderType(const ShaderType &t) { shaderType_ = t; }

  //---

  bool isSkybox() const { return isSkybox_; }
  void setSkybox(bool b) { isSkybox_ = b; }

  //---

  int isShadowed() const { return shadowed_; }
  void setShadowed(int i) { shadowed_ = i; }

  int isTextureBuffer() const { return isTextureBuffer_; }
  void setTextureBuffer(int i) { isTextureBuffer_ = i; }

  int isLightBuffer() const { return lightBuffer_; }
  void setLightBuffer(int i) { lightBuffer_ = i; }

  double shadowBias() const { return shadowBias_; }
  void setShadowBias(double r) { shadowBias_ = r; }

#if 0
  double shadowScale() const { return shadowScale_; }
  void setShadowScale(double r) { shadowScale_ = r; }
#endif

  //---

  const CRGBA &ambientColor() const { return ambientColor_; }
  void setAmbientColor(const CRGBA &c) { ambientColor_ = c; update(); }

  double ambientStrength() const { return ambientStrength_; }
  void setAmbientStrength(double r) { ambientStrength_ = r; update(); }

  double diffuseStrength() const { return diffuseStrength_; }
  void setDiffuseStrength(double r) { diffuseStrength_ = r; update(); }

  const CRGBA &specularColor() const { return specularColor_; }
  void setSpecularColor(const CRGBA &c) { specularColor_ = c; update(); }

  double specularStrength() const { return specularStrength_; }
  void setSpecularStrength(double r) { specularStrength_ = r; update(); }

  const CRGBA &emissiveColor() const { return emissiveColor_; }
  void setEmissiveColor(const CRGBA &c) { emissiveColor_ = c; update(); }

  double emissiveStrength() const { return emissiveStrength_; }
  void setEmissiveStrength(double r) { emissiveStrength_ = r; update(); }

  double shininess() const { return shininess_; }
  void setShininess(double r) { shininess_ = r; update(); }

  //---

  const CBBox3D &bbox() const { return bbox_; }

  //---

  void initializeGL() override;

  void resizeGL(int, int) override;

  void paintGL() override;

  //---

  void mousePressEvent  (QMouseEvent *e) override;
  void mouseMoveEvent   (QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;

  void wheelEvent(QWheelEvent *e) override;

  void keyPressEvent(QKeyEvent *event) override;

  //---

  QSize sizeHint() const override { return QSize(1536, 1536); }

  //---

  void addSkybox();
  void drawSkybox();

  void addScene();

  void drawScene(CGLCameraIFace *camera);

  void drawTexture(TextureBuffer &texture, bool isDepth);

  void drawCubeMapTexture(TextureBuffer &texture, bool isDepth);

  void writeTexture(TextureBuffer &textureBuffer, const char *filename, bool isDepth);

  void addShaderLights(ShaderProgram *program);

 private:
  void connectSlots(bool);

  ShaderProgram *getShader(const QString &name, const QString &vertexFile,
                           const QString &fragmentFile);
  ShaderProgram *getShader(const QString &name, const QString &vertexFile,
                           const QString &geometryFile, const QString &fragmentFile);

  void enableDepthTest();
  void enableCullFace();
  void enableFrontFace();
  void enablePolygonLine();

  CQGLTexture *getGLTexture(CGeomTexture *texture, bool /*add*/);

  void initGLTexture(Texture *texture);

  CQGLTexture *makeTexture(const CImagePtr &image) const;

  CGLCameraIFace *getEditCamera() const;

 Q_SIGNALS:
  void textureAdded();

 public Q_SLOTS:
  void updateScene();

 private:
  struct MouseData {
    bool            pressed   { false };
    bool            isShift   { false };
    bool            isControl { false };
    Qt::MouseButton button    { Qt::NoButton };
    CPoint2D        press     { 0.0, 0.0 };
    CPoint2D        move      { 0.0, 0.0 };
  };

  using Shaders = std::map<QString, ShaderProgram *>;

  //---

  App* app_ { nullptr };

  int ind_ { 0 };

  Camera* camera_ { nullptr };
  Light*  light_  { nullptr };

  // state
  int pixelWidth_  { 2000 };
  int pixelHeight_ { 1500 };

  double aspect_ { 1.0 };

  // Open GL globals

  bool depthTest_   { true };
  bool cullFace_    { true };
  bool frontFace_   { false };
  bool polygonLine_ { false };

  double pointSize_ { 4.0 };
  double lineWidth_ { 2.0 };

  // textures
  bool textured_ { true };

  // lights
  CRGBA  ambientColor_     { CRGBA::white() };
  double ambientStrength_  { 0.1 };

  double diffuseStrength_  { 1.0 };

  CRGBA  specularColor_    { CRGBA::white() };
  double specularStrength_ { 0.2 };

  CRGBA  emissiveColor_    { CRGBA::white() };
  double emissiveStrength_ { 0.1 };

  double shininess_ { 32.0 };

  bool fixedDiffuse_ { false };

  //---

  // globals
  QColor bgColor_ { 140, 140, 140 };

  EditType editType_ { EditType::CAMERA };

  ShaderType shaderType_ { ShaderType::SCENE };

  bool isSkybox_ { true };

  bool shadowed_        { false };
  bool isTextureBuffer_ { false };
  bool lightBuffer_     { false };

  double shadowBias_     { 0.01 };
//double shadowScale_    { -50.0 };
  double shadowFarPlane_ { 1 };

  // interaction
  MouseData mouseData_;

  CBBox3D bbox_;

  Shaders shaders_;

  bool invalid_ { true };

  //---

  struct TextureBuffer {
    CGLCameraIFace* camera        { nullptr };
    CQGLTexture*    texture       { nullptr };
    ShaderProgram*  shaderProgram { nullptr };
    CQGLBuffer*     buffer        { nullptr };
    FaceDataList    faceDataList;
  };

  TextureBuffer textureBuffer_;
  TextureBuffer skyboxTextureBuffer_;
  TextureBuffer cubeMapTextureBuffer_;
  TextureBuffer shadowTextureBuffer_;
  TextureBuffer shadowCubeMapTextureBuffer_;
};

}

#endif
