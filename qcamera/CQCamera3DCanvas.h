#ifndef CQCamera3DCanvas_H
#define CQCamera3DCanvas_H

#include <CGLMatrix3D.h>
#include <CBBox3D.h>
#include <CPlane3D.h>
#include <CImagePtr.h>

#include <QGLWidget>
#include <QOpenGLExtraFunctions>

class CQCamera3DApp;
class CQCamera3DShaderProgram;
class CQCamera3DObjectData;
class CQCamera3DFaceData;
class CQCamera3DCamera;
class CQCamera3DShape;
class CQCamera3DAnnotation;
class CQCamera3DOverlay;
class CQCamera3DOverlay2D;
class CQCamera3DFont;
class CQCamera3DAxes;
class CQCamera3DGeomObject;

class CQGLBuffer;
class CQGLTexture;
class CGeomObject3D;
class CGeomFace3D;
class CGeomTexture;

class CQCamera3DCanvas : public QGLWidget, public QOpenGLExtraFunctions {
  Q_OBJECT

 public:
  enum class SelectType {
    OBJECT,
    FACE,
    EDGE,
    POINT
  };

  enum class MouseType {
    CAMERA,
    OBJECT
  };

  struct EyeLine {
    bool      isSet { false };
    CPoint3D  px1;
    CPoint3D  px2;
    CPoint3D  pp1;
    CPoint3D  pp2;
    CPoint3D  pv1;
    CPoint3D  pv2;
    CVector3D v;
  };

  struct TextureData {
    CQGLTexture*  glTexture   { nullptr };
    CGeomTexture* geomTexture { nullptr };
  };

  using GLTextures = std::map<int, TextureData>;

 public:
  CQCamera3DCanvas(CQCamera3DApp *app);

  CQCamera3DApp *app() const { return app_; }

  CQCamera3DShaderProgram *shaderProgram() const { return shaderProgram_; }

  const CBBox3D &bbox() const { return bbox_; }

  const std::vector<CQCamera3DCamera *> cameras() const { return cameras_; }

  CQCamera3DAxes *axes() const { return axes_; }

  CQCamera3DFont* getFont() const { return font_; }

  CQCamera3DCamera* currentCamera() const;

  //---

  int pixelWidth() const { return pixelWidth_; }
  int pixelHeight() const { return pixelHeight_; }

  int cameraInd() const { return cameraInd_; }
  void setCameraInd(int i);

  bool isShowCamera() const { return showCamera_; }
  void setShowCamera(bool b) { showCamera_ = b; }

  bool isShowAxes() const;
  void setShowAxes(bool b);

  //---

  bool isShowEyeLine() const { return showEyeLine_; }
  void setShowEyeLine(bool b) { showEyeLine_ = b; }

  const EyeLine &eyeLine() const { return eyeLine_; }

  double eyeLineZ1() const { return eyeLineZ1_; }
  void setEyeLineZ1(double r) { eyeLineZ1_ = r; }

  double eyeLineZ2() const { return eyeLineZ2_; }
  void setEyeLineZ2(double r) { eyeLineZ2_ = r; }

  //---

  bool isShowPlanes() const { return showPlanes_; }
  void setShowPlanes(bool b) { showPlanes_ = b; }

  const CPlane3D &xyPlane() const { return xyPlane_; }
  void setXYPlane(const CPlane3D &v) { xyPlane_ = v; }

  //---

  bool isWireframe() const { return wireframe_; }
  void setWireframe(bool b) { wireframe_ = b; }

  bool isSolid() const { return solid_; }
  void setSolid(bool b) { solid_ = b; }

  //---

  const QColor &bgColor() const { return bgColor_; }
  void setBgColor(const QColor &c) { bgColor_ = c; }

  const QColor &ambientColor() const { return ambientColor_; }
  void setAmbientColor(const QColor &c) { ambientColor_ = c; }

  //---

  bool isDepthTest() const { return depthTest_; }
  void setDepthTest(bool b) { depthTest_ = b; }

  bool isCullFace() const { return cullFace_; }
  void setCullFace(bool b) { cullFace_ = b; }

  bool isFrontFace() const { return frontFace_; }
  void setFrontFace(bool b) { frontFace_ = b; }

  bool isPolygonLine() const { return polygonLine_; }
  void setPolygonLine(bool b) { polygonLine_ = b; }

  //---

  const MouseType &mouseType() const { return mouseType_; }
  void setMouseType(const MouseType &v) { mouseType_ = v; }

  //---

  const SelectType &selectType() const { return selectType_; }
  void setSelectType(const SelectType &v) { selectType_ = v; }

  bool selectObject(CGeomObject3D *object, bool update=true);
  bool selectFace(CGeomFace3D *face, bool update=true);

  bool deselectAll(bool update=true);

  //---

  const CPoint3D &intersectPoint1() const { return intersectPoint1_; }
  const CPoint3D &intersectPoint2() const { return intersectPoint2_; }

  void setIntersectPoints(const CPoint3D &p1, const CPoint3D &p2) {
    intersectPoint1_ = p1; intersectPoint2_ = p2;
  }

  //---

  void addObjectsData();

  void updateShapes();

  void updateAnnotation();

  void updateOverlay();
  void updateOverlay2D();

  //---

  CQCamera3DGeomObject *currentObject() const { return currentObject_; }
  void setCurrentObject(CQCamera3DGeomObject *object);

  CGeomFace3D *currentFace() const { return currentFace_; }
  void setCurrentFace(CGeomFace3D *face);

  //---

  const GLTextures &glTextures() const { return glTextures_; }

  CQGLTexture *getTextureByName(const std::string &name) const;

  CGeomTexture *getGeomTextureByName(const std::string &name) const;

  CQGLTexture *getTexture(CGeomTexture *texture, bool add);

  //---

  bool createFontTexture(uint *texture, int w, int h, uchar *data);

  void initializeGL() override;

  void paintGL() override;

  void resizeGL(int, int) override;

  QSize sizeHint() const override { return QSize(1536, 1536); }

 private:
  void initShader();

  void drawObjectsData();

  void initCamera();

  void updateStatus();

  CQCamera3DObjectData *getObjectData(int i, CGeomObject3D *o);

  void mousePressEvent  (QMouseEvent *e) override;
  void mouseMoveEvent   (QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;

  void wheelEvent(QWheelEvent *) override;

  void keyPressEvent(QKeyEvent *event) override;

  void cameraKeyPress(QKeyEvent *event);
  void objectKeyPress(QKeyEvent *event);

  void calcEyeLine(const CPoint3D &pos, EyeLine &eyeLine, bool verbose=false) const;

  //---

  CQGLTexture *makeTexture(const CImagePtr &image) const;

 private Q_SLOTS:
  void cameraChanged();

 Q_SIGNALS:
  void stateChanged();

  void textureAdded();

 private:
  struct MouseData {
    CPoint2D        press  { 0.0, 0.0 };
    CPoint2D        move   { 0.0, 0.0 };
    Qt::MouseButton button { Qt::NoButton };
  };

  CQCamera3DApp* app_ { nullptr };

  CQCamera3DFont *font_ { nullptr };
  CQCamera3DAxes *axes_ { nullptr };

  std::vector<CQCamera3DCamera *> cameras_;
  int                             cameraInd_ { 0 };
  bool                            showCamera_ { false };

  CQCamera3DShape* shape_ { nullptr };

  CQCamera3DAnnotation* annotation_ { nullptr };

  CQCamera3DOverlay*   overlay_   { nullptr };
  CQCamera3DOverlay2D* overlay2D_ { nullptr };

  QColor bgColor_      { 200, 200, 255 };
  QColor ambientColor_ { 50, 50, 50 };

  CQCamera3DShaderProgram *shaderProgram_ { nullptr };

  std::vector<CQCamera3DObjectData *> objectDatas_;

  // textures
  GLTextures glTextures_;

  bool depthTest_   { true };
  bool cullFace_    { true };
  bool frontFace_   { false };
  bool polygonLine_ { false };

  int pixelWidth_  { 100 };
  int pixelHeight_ { 100 };

  CBBox3D bbox_;

  MouseData mouseData_;

  bool wireframe_ { false };
  bool solid_     { true };

  EyeLine eyeLine_;
  bool    showEyeLine_ { false };
  double  eyeLineZ1_   { -1.0 };
  double  eyeLineZ2_   { -2.0 };

  bool     showPlanes_ { false };
  CPlane3D xyPlane_;

  MouseType mouseType_ { MouseType::CAMERA };

  SelectType selectType_ { SelectType::FACE };

  CPoint3D intersectPoint1_;
  CPoint3D intersectPoint2_;

  CQCamera3DGeomObject* currentObject_ { nullptr };
  CGeomFace3D*          currentFace_   { nullptr };
};

//---

class CQCamera3DObjectData {
 public:
  using FaceDatas = std::vector<CQCamera3DFaceData>;

 public:
  CQCamera3DObjectData(CQCamera3DCanvas *canvas);

  const CGeomObject3D *object() const { return object_; }
  void setObject(CGeomObject3D *o) { object_ = o; }

  const CBBox3D &bbox() const { return bbox_; }
  void setBBox(const CBBox3D &v) { bbox_ = v; }

  CQGLBuffer *buffer() const { return buffer_; }

  const FaceDatas &faceDatas() const { return faceDatas_; }

  CQGLBuffer *initBuffer();

  void addFaceData(const CQCamera3DFaceData &faceData);

 private:
  CQCamera3DCanvas* canvas_ { nullptr };
  CGeomObject3D*    object_ { nullptr };
  CQGLBuffer*       buffer_ { nullptr };

  FaceDatas faceDatas_;
  CBBox3D   bbox_;
};

#endif
