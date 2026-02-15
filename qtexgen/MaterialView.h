#ifndef MaterialView_H
#define MaterialView_H

#include <FaceData.h>

#include <CGeomObject3D.h>
#include <CImagePtr.h>

#include <QGLWidget>
#include <QOpenGLExtraFunctions>

class CGLCameraIFace;
class CQGLBuffer;
class CQGLTexture;
class CGeomTexture;
class CGeomMaterial;

namespace CQTextureGen {

class App;
class Light;
class ShaderProgram;
class Texture;

class MaterialView : public QGLWidget, public QOpenGLExtraFunctions {
  Q_OBJECT

 public:
  using App       = CQTextureGen::App;
  using ShapeType = CGeomObject3D::ShapeType;
  using Lights    = std::vector<Light *>;

 public:
  MaterialView(App *app);

  //---

  int pixelWidth() const { return pixelWidth_; }
  void setPixelWidth(int i) { pixelWidth_ = i; }

  int pixelHeight() const { return pixelHeight_; }
  void setPixelHeight(int i) { pixelHeight_ = i; }

  double aspect() const { return aspect_; }
  void setAspect(double r) { aspect_ = r; }

  //---

  const ShapeType &shapeType() const { return shapeType_; }
  void setShapeType(const ShapeType &v);

  const QColor &bgColor() const { return bgColor_; }
  void setBgColor(const QColor &c) { bgColor_ = c; }

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

  const CRGBA &emissionColor() const { return emissionColor_; }
  void setEmissionColor(const CRGBA &c) { emissionColor_ = c; update(); }

  double emissionStrength() const { return emissionStrength_; }
  void setEmissionStrength(double r) { emissionStrength_ = r; update(); }

  double shininess() const { return shininess_; }
  void setShininess(double r) { shininess_ = r; update(); }

  //---

  ShaderProgram *shaderProgram() { return shaderProgram_; }

  CGeomMaterial *material() const { return material_; }

  CGLCameraIFace* camera() const { return camera_; }

  //---

  const Lights &lights() const { return lights_; }

  void addLight();

  Light *getLightById(int id) const;

  //---

  const FaceDataList &faceDataList() const { return faceDataList_; }

  CQGLBuffer *buffer() const { return buffer_; }

  //---

  void initializeGL() override;

  void resizeGL(int, int) override;

  void paintGL() override;

  //---

  void mousePressEvent  (QMouseEvent *e) override;
  void mouseMoveEvent   (QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;

  void wheelEvent(QWheelEvent *) override;

  void keyPressEvent(QKeyEvent *event) override;

  //---

  CQGLTexture *getGLTexture(CGeomTexture *texture);

  void initGLTexture(Texture *texture, const QImage &image);

  CQGLTexture *makeTexture(const QImage &image) const;

 Q_SIGNALS:
  void lightAdded();
  void lightChanged();

 private Q_SLOTS:
  void invalidateSlot();

 private:
  void addGeometry();
  void drawGeometry();

  CQGLBuffer *initBuffer();

 private:
  struct MouseData {
    bool            pressed   { false };
    bool            isShift   { false };
    bool            isControl { false };
    Qt::MouseButton button    { Qt::NoButton };
    CPoint2D        press     { 0.0, 0.0 };
    CPoint2D        move      { 0.0, 0.0 };
  };

  //---

  App* app_ { nullptr };

  //---

  // Material
  CGeomMaterial *material_ { nullptr };

  Texture *diffuseTexture_  { nullptr };
  Texture *normalTexture_   { nullptr };
  Texture *specularTexture_ { nullptr };
  Texture *emissionTexture_ { nullptr };
  Texture *metallicTexture_ { nullptr };
  Texture *ORMTexture_      { nullptr };

  //---

  // Camera
  CGLCameraIFace* camera_ { nullptr };

  //---

  // Lighting
  CRGBA  ambientColor_    { 1.0, 1.0, 1.0 };
  double ambientStrength_ { 0.1 };

  double diffuseStrength_ { 1.0 };

  CRGBA  specularColor_    { 1.0, 1.0, 1.0 };
  double specularStrength_ { 0.2 };

  CRGBA  emissionColor_    { 1.0, 1.0, 1.0 };
  double emissionStrength_ { 0.01 };

  double shininess_ { 32.0 };

  Lights lights_;

  //---

  ShaderProgram *shaderProgram_ { nullptr };

  CQGLBuffer *buffer_ { nullptr };

  // state
  bool valid_ { false };

  //---

  // geometry
  FaceDataList faceDataList_;

  //---

  // state
  int pixelWidth_  { 2000 };
  int pixelHeight_ { 1500 };

  double aspect_ { 1.0 };

  ShapeType shapeType_ { ShapeType::CUBE };

  // globals
  QColor bgColor_ { 140, 140, 140 };

  // interaction
  MouseData mouseData_;
};

}

#endif
