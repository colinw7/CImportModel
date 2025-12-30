#ifndef CQCamera3DCanvas_H
#define CQCamera3DCanvas_H

#include <CQCamera3DApp.h>

#include <CGLMatrix3D.h>
#include <CBBox3D.h>
#include <CPlane3D.h>
#include <CImagePtr.h>
#include <CRGBA.h>

#include <QGLWidget>
#include <QOpenGLExtraFunctions>
#include <QMatrix4x4>

#include <set>

class CQCamera3DApp;
class CQCamera3DShaderProgram;
class CQCamera3DObjectData;
class CQCamera3DFaceData;
class CQCamera3DCamera;
class CQCamera3DLight;
class CQCamera3DShape;
class CQCamera3DAnnotation;
class CQCamera3DNormals;
class CQCamera3DOverlay;
class CQCamera3DOverlay2D;
class CQCamera3DFont;
class CQCamera3DAxes;
class CQCamera3DGeomObject;

class CQGLBuffer;
class CQGLTexture;
class CGeomObject3D;
class CGeomFace3D;
class CGeomVertex3D;
class CGeomTexture;

class CQCamera3DCanvas : public QGLWidget, public QOpenGLExtraFunctions {
  Q_OBJECT

 public:
  using SelectType = CQCamera3DSelectType;
  using MouseType  = CQCamera3DMouseType;

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

  using SelectInds       = std::set<int>;
  using ObjectSelectInds = std::map<CGeomObject3D *, SelectInds>;

 public:
  CQCamera3DCanvas(CQCamera3DApp *app);

  CQCamera3DApp *app() const { return app_; }

  CQCamera3DShaderProgram *shaderProgram();

  const CBBox3D &bbox() const { return bbox_; }

  // camera
  const std::vector<CQCamera3DCamera *> cameras() const { return cameras_; }

  CQCamera3DCamera* getCurrentCamera() const;

  // light
  const std::vector<CQCamera3DLight *> lights() const { return lights_; }

  CQCamera3DLight* currentLight() const;
  void setCurrentLight(CQCamera3DLight *);

  CQCamera3DLight* getLightById(uint id) const;

  bool isFixedDiffuse() const { return fixedDiffuse_; }
  void setFixedDiffuse(bool b) { fixedDiffuse_ = b; }

  // axes
  CQCamera3DAxes *axes() const { return axes_; }

  // font/text
  CQCamera3DFont* getFont() const { return font_; }

  // normals
  CQCamera3DNormals *getNormals() const { return normals_; }

  //---

  int pixelWidth() const { return pixelWidth_; }
  void setPixelWidth(int i) { pixelWidth_ = i; }

  int pixelHeight() const { return pixelHeight_; }
  void setPixelHeight(int i) { pixelHeight_ = i; }

  double aspect() const { return aspect_; }
  void setAspect(double r) { aspect_ = r; }

  //---

  // cameras
  int cameraInd() const { return cameraInd_; }
  void setCameraInd(int i);

  bool isShowCamera() const { return showCamera_; }
  void setShowCamera(bool b) { showCamera_ = b; }

  //---

  // axes
  bool isShowAxes() const;
  void setShowAxes(bool b);

  //---

  // normals

  bool isShowNormals() const { return showNormals_; }
  void setShowNormals(bool b) { showNormals_ = b; }

  double normalsSize() const { return normalsSize_; }
  void setNormalsSize(double r) { normalsSize_ = r; }

  const QColor &normalsColor() const { return normalsColor_; }
  void setNormalsColor(const QColor &c) { normalsColor_ = c; }

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

  bool isPoints() const { return points_; }
  void setPoints(bool b) { points_ = b; }

  double pointSize() const { return pointSize_; }
  void setPointSize(double r) { pointSize_ = r; update(); }

  double lineWidth() const { return lineWidth_; }
  void setLineWidth(double r) { lineWidth_ = r; update(); }

  //---

  const QColor &bgColor() const { return bgColor_; }
  void setBgColor(const QColor &c) { bgColor_ = c; }

  const CRGBA &ambientColor() const { return ambientColor_; }
  void setAmbientColor(const CRGBA &c) { ambientColor_ = c; update(); }

  double ambientStrength() const { return ambientStrength_; }
  void setAmbientStrength(double r) { ambientStrength_ = r; update(); }

  double diffuseStrength() const { return diffuseStrength_; }
  void setDiffuseStrength(double r) { diffuseStrength_ = r; update(); }

  double specularStrength() const { return specularStrength_; }
  void setSpecularStrength(double r) { specularStrength_ = r; update(); }

  double emissiveStrength() const { return emissiveStrength_; }
  void setEmissiveStrength(double r) { emissiveStrength_ = r; update(); }

  double shininess() const { return shininess_; }
  void setShininess(double r) { shininess_ = r; update(); }

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

  double mouseScale() const { return mouseScale_; }
  void setMouseScale(double r) { mouseScale_ = r; }

  //---

  const SelectType &selectType() const { return selectType_; }
  void setSelectType(const SelectType &v) { selectType_ = v; }

  bool selectObject(CGeomObject3D *object, bool update=true);

  bool selectFaces(const std::vector<CGeomFace3D *> &faces, bool update=true);
  bool selectFace(CGeomFace3D *face, bool update=true);

  bool selectVertex(CGeomVertex3D *vertex, bool update=true);
  bool selectVertices(const ObjectSelectInds &vertices, bool update=true);

  bool deselectAll(bool update=true);

  //---

  const CPoint3D &intersectPoint1() const { return intersectPoint1_; }
  const CPoint3D &intersectPoint2() const { return intersectPoint2_; }

  void setIntersectPoints(const CPoint3D &p1, const CPoint3D &p2) {
    intersectPoint1_ = p1; intersectPoint2_ = p2;
  }

  //---

  void enableDepthTest();
  void enableCullFace();
  void enableFrontFace();
  void enablePolygonLine();

  //---

  void addLight();
  void resetLight(CQCamera3DLight *light);

  //---

  void updateShapes();

  void updateAnnotation();

  void updateOverlay();
  void updateOverlay2D();

  //---

  CQCamera3DGeomObject *currentObject(bool includeRoot=false) const {
    if (currentObject_)
      return currentObject_;

    return (includeRoot ? rootObject() : nullptr);
  }

  void setCurrentObject(CQCamera3DGeomObject *object, bool update);

  CGeomFace3D *currentFace() const { return currentFace_; }
  void setCurrentFace(CGeomFace3D *face, bool update);

  CGeomVertex3D *currentVertex() const { return currentVertex_; }
  void setCurrentVertex(CGeomVertex3D *vertex, bool update);

  CQCamera3DGeomObject *rootObject() const;

  //---

  const std::vector<CQCamera3DObjectData *> &getObjectDatas() const { return objectDatas_; }

  CGeomObject3D *getObject(int i) const;

  CQCamera3DObjectData *getObjectData(CGeomObject3D *object) const;

  //---

  // textures

  void setInitTextureMap(const std::string &s) { initTextureMap_ = s; }

  bool loadTextureMap(const std::string &fileName);

  void addTextureFile(const std::string &fileName);

  const GLTextures &glTextures() const { return glTextures_; }

  CQGLTexture *getTextureByName(const std::string &name) const;

  CGeomTexture *getGeomTextureByName(const std::string &name) const;

  CQGLTexture *getTexture(CGeomTexture *texture, bool add);

  //---

  // materials

  void setInitMaterialMap(const std::string &s) { initMaterialMap_ = s; }

  bool loadMaterialMap(const std::string &fileName);

  //---

  bool createFontTexture(uint *texture, int w, int h, uchar *data);

  //---

  void initializeGL() override;

  void paintGL() override;

  void resizeGL(int, int) override;

  QSize sizeHint() const override { return QSize(1536, 1536); }

 private:
  void drawObjectsData();

  void addShaderLights(CQCamera3DShaderProgram *program);

  void updateNodeMatrices(CGeomObject3D *object);

  void initCamera();

  CQCamera3DObjectData *initObjectData(int i, CGeomObject3D *o);

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

 public Q_SLOTS:
  void addObjectsData();

 private Q_SLOTS:
  void updateStatus();

  void cameraChanged();

 Q_SIGNALS:
  void stateChanged();

  void textureAdded();

  void lightAdded();

  void objectsChanged();

 private:
  struct PaintData {
    enum { NUM_NODE_MATRICES = 128 };

    CGeomObject3D*          rootObject { nullptr };
    std::vector<CMatrix3D>  nodeMatrices;
    std::vector<QMatrix4x4> nodeQMatrices;

    void reset() {
      rootObject = nullptr;

      nodeMatrices .clear();
      nodeQMatrices.clear();
    }
  };

  struct MouseData {
    CPoint2D        press  { 0.0, 0.0 };
    CPoint2D        move   { 0.0, 0.0 };
    Qt::MouseButton button { Qt::NoButton };
  };

  CQCamera3DApp* app_ { nullptr };

  //---

  // fonts
  CQCamera3DFont *font_ { nullptr };

  //---

  // axis
  CQCamera3DAxes *axes_ { nullptr };

  //---

  // cameras
  std::vector<CQCamera3DCamera *> cameras_;
  int                             cameraInd_ { 0 };
  bool                            showCamera_ { false };

  //---

  // lighting
  CRGBA  ambientColor_    { 1, 1, 1 };
  double ambientStrength_ { 0.1 };

  double diffuseStrength_  { 1.0 };
  double specularStrength_ { 0.2 };
  double emissiveStrength_ { 0.1 };

  double shininess_ { 32.0 };

  std::vector<CQCamera3DLight *> lights_;
  int                            lightInd_ { 0 };
  bool                           fixedDiffuse_ { false };

  //---

  // normals
  bool   showNormals_  { false };
  double normalsSize_  { -1.0 };
  QColor normalsColor_ { 255, 255, 255 };

  //---

  // views

  CQCamera3DShape* shape_ { nullptr };

  CQCamera3DAnnotation* annotation_ { nullptr };

  CQCamera3DNormals *normals_ { nullptr};

  CQCamera3DOverlay*   overlay_   { nullptr };
  CQCamera3DOverlay2D* overlay2D_ { nullptr };

  //----

  // globals
  QColor bgColor_ { 200, 200, 255 };

  CQCamera3DShaderProgram *shaderProgram_ { nullptr };

  PaintData paintData_;

   //---

  // objects

  std::vector<CQCamera3DObjectData *> objectDatas_;

  //---

  // textures
  std::string initTextureMap_;

  GLTextures glTextures_;

  //---

  // materials

  std::string initMaterialMap_;

  //---

  // Open GL globals

  bool depthTest_   { true };
  bool cullFace_    { true };
  bool frontFace_   { false };
  bool polygonLine_ { false };

  //---

  // state
  int pixelWidth_  { 2000 };
  int pixelHeight_ { 1500 };

  double aspect_ { 1.0 };

  CBBox3D bbox_;

  MouseData mouseData_;

  //---

  // draw types

  bool wireframe_ { false };
  bool solid_     { true };
  bool points_    { false };

  double pointSize_ { 4.0 };
  double lineWidth_ { 2.0 };

  //---

  // eye line
  EyeLine eyeLine_;
  bool    showEyeLine_ { false };
  double  eyeLineZ1_   { -1.0 };
  double  eyeLineZ2_   { -2.0 };

  CPoint3D intersectPoint1_;
  CPoint3D intersectPoint2_;

  //---

  // planes

  bool     showPlanes_ { false };
  CPlane3D xyPlane_;

  //---

  // interaction

  MouseType mouseType_  { MouseType ::CAMERA };
  double    mouseScale_ { 0.05 };

  SelectType selectType_ { SelectType::OBJECT };

  CQCamera3DGeomObject* currentObject_ { nullptr };
  CGeomFace3D*          currentFace_   { nullptr };
  CGeomVertex3D*        currentVertex_ { nullptr };
};

//---

class CQCamera3DObjectData {
 public:
  using FaceDatas = std::vector<CQCamera3DFaceData>;

 public:
  CQCamera3DObjectData(CQCamera3DCanvas *canvas);

  int ind() const { return ind_; }
  void setInd(int i) { ind_ = i; }

  //---

  const CGeomObject3D *object() const { return object_; }
  void setObject(CGeomObject3D *o) { object_ = o; }

  //---

  const CBBox3D &bbox() const { return bbox_; }
  void setBBox(const CBBox3D &v) { bbox_ = v; }

  //---

  CQGLBuffer *buffer() const { return buffer_; }

  const FaceDatas &faceDatas() const { return faceDatas_; }

  CQGLBuffer *initBuffer();

  void addFaceData(const CQCamera3DFaceData &faceData);

 private:
  CQCamera3DCanvas* canvas_ { nullptr };
  CGeomObject3D*    object_ { nullptr };
  CQGLBuffer*       buffer_ { nullptr };

  int ind_ { 0 };

  FaceDatas faceDatas_;
  CBBox3D   bbox_;
};

#endif
