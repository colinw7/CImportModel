#ifndef CQNewGLWidget_H
#define CQNewGLWidget_H

#include <CPoint3D.h>
#include <CGLMatrix3D.h>

#include <CImagePtr.h>

#include <QGLWidget>
#include <QOpenGLExtraFunctions>

class CQNewGLModelObject;
class CQNewGLModel;
class CQNewGLCamera;
class CQNewGLFont;
class CQNewGLShaderProgram;

class CQGLTexture;
class CGeomTexture;
class CGeomObject3D;

class CQNewGLWidget : public QGLWidget, public QOpenGLExtraFunctions {
  Q_OBJECT

  Q_PROPERTY(QColor bgColor READ bgColor WRITE setBgColor)

 public:
  struct CameraData {
    CGLVector3D position;
    CGLVector3D origin;
    CGLVector3D up;
    QColor      color;
  };

  struct TextureData {
    CQGLTexture*  glTexture   { nullptr };
    CGeomTexture* geomTexture { nullptr };
  };

  using ObjectDatas = std::map<CGeomObject3D *, CQNewGLModelObject *>;
  using GLTextures  = std::map<int, TextureData>;

 public:
  CQNewGLWidget(CQNewGLModel *app);
 ~CQNewGLWidget() override;

  CQNewGLModel* app() const { return app_; }

  const QColor &bgColor() const { return bgColor_; }
  void setBgColor(const QColor &c);

  virtual CPoint3D  sceneCenter() const = 0;
  virtual CVector3D sceneSize  () const = 0;
  virtual double    sceneScale () const = 0;

  virtual CGeomObject3D *getCurrentObject() const { return nullptr; }
  virtual CQNewGLCamera *getCurrentCamera() const = 0;

  virtual const CGLMatrix3D &projectionMatrix() const = 0;
  virtual const CGLMatrix3D &viewMatrix() const = 0;

  //---

  int pixelWidth() const { return pixelWidth_; }
  void setPixelWidth(int i) { pixelWidth_ = i; }

  int pixelHeight() const { return pixelHeight_; }
  void setPixelHeight(int i) { pixelHeight_ = i; }

  double aspect() const { return aspect_; }
  void setAspect(double r) { aspect_ = r; }

  //---

  CQNewGLShaderProgram *getShader(const QString &vertex, const QString &fragment);

  //---

  const ObjectDatas &getObjectDatas() { return objectDatas_; }

  CQNewGLModelObject *getObjectData(CGeomObject3D *object);
  CQNewGLModelObject *getObjectData(int ind) const;

  CGeomObject3D *getSelectedObject() const;

  //---

  // font
  void initFont();

  CQNewGLFont *getFont() const { return font_; }

  bool createFontTexture(uint *texture, int w, int h, uchar *data);

  //---

  void calcCameraData(int ic, CameraData &cameraData) const;

  //---

  void addShaderMVP(CQNewGLShaderProgram *program, const CMatrix3D &modelMatrix);

  //---

  // textures
  const GLTextures &glTextures() const { return glTextures_; }

  CQGLTexture  *getTextureByName    (const std::string &name) const;
  CGeomTexture *getGeomTextureByName(const std::string &name) const;

  CQGLTexture *getTexture(CGeomTexture *texture, bool add);

  CQGLTexture *makeTexture(const CImagePtr &image) const;

  //---

  void enableBlend();
  void disableBlend();

 Q_SIGNALS:
  void modelAdded();

  void textureAdded();

 protected:
  struct MouseData {
    bool            pressed   { false };
    bool            isShift   { false };
    bool            isControl { false };
    Qt::MouseButton button    { Qt::NoButton };
    CPoint2D        press     { 0.0, 0.0 };
    CPoint2D        move      { 0.0, 0.0 };
  };

  using Shaders = std::map<QString, CQNewGLShaderProgram *>;

  CQNewGLModel* app_ { nullptr };

  QColor bgColor_ { 51, 76, 76 };

  Shaders shaders_;

  // widget object data
  ObjectDatas objectDatas_;

  // font
  CQNewGLFont* font_ { nullptr };

  // textures
  GLTextures glTextures_;

  int pixelWidth_  { 2000 };
  int pixelHeight_ { 1500 };

  double aspect_ { 1.0 };

  bool blend_ { false };

  // mouse
  MouseData mouseData_;
};

#endif
