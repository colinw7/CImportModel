#ifndef CQCamera3DCanvas_H
#define CQCamera3DCanvas_H

#include <CQCamera3DWidget.h>
#include <CQCamera3DApp.h>

#include <CGLMatrix3D.h>
#include <CMatrix3DH.h>
#include <CBBox3D.h>
#include <CPlane3D.h>
#include <CRGBA.h>

#include <QMatrix4x4>

#include <set>

class CQCamera3DShaderProgram;
class CQCamera3DObjectData;
class CQCamera3DFaceData;
class CQCamera3DCamera;
class CQCamera3DLight;
class CQCamera3DShape;
class CQCamera3DAnnotation;
class CQCamera3DBillboard;
class CQCamera3DNormals;
class CQCamera3DBasis;
class CQCamera3DBBox;
class CQCamera3DOverlay;
class CQCamera3DOverlay2D;
class CQCamera3DFont;
class CQCamera3DAxes;
class CQCamera3DGeomObject;
class CQCamera3DGeomFace;
class CQCamera3DOpWidget;
class CQCamera3DMouseModeMgr;
class CQCamera3DOptions;

class CQGLBuffer;
class CGeomObject3D;
class CGeomFace3D;
class CGeomVertex3D;

class CQCamera3DCanvas : public CQCamera3DWidget {
  Q_OBJECT

 public:
  using SelectType = CQCamera3DSelectType;
  using MouseType  = CQCamera3DMouseType;
  using EditType   = CQCamera3DEditType;

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

  using SelectInds       = std::set<int>;
  using ObjectSelectInds = std::map<CGeomObject3D *, SelectInds>;

  using Cameras     = std::vector<CQCamera3DCamera *>;
  using Lights      = std::vector<CQCamera3DLight *>;
  using ObjectDatas = std::vector<CQCamera3DObjectData *>;
  using Objects     = std::vector<CGeomObject3D *>;
  using Faces       = std::vector<CGeomFace3D *>;

  using Edges     = std::vector<int>;
  using FaceEdges = std::map<CQCamera3DGeomFace *, Edges>;

  enum class AddObjectType {
    NONE,
    CIRCLE,
    CUBE,
    CYLINDER,
    PLANE,
    PYRAMID,
    SPHERE,
    TORUS
  };

  enum class ViewType {
    PERSPECTIVE,
    TOP,
    BOTTOM,
    LEFT,
    RIGHT,
    FRONT,
    BACK
  };

  enum class MoveDirection {
    NONE,
    X,
    Y,
    Z
  };

 public:
  CQCamera3DCanvas(CQCamera3DApp *app);

  //---

  bool isQuadView() const { return quadView_; }
  void setQuadView(bool b) { quadView_ = b; updateQuadCameras(); update(); }

  const CBBox3D &bbox() const { return bbox_; }

  const CPoint3D &cursor() const { return cursor_; }
  void setCursor(const CPoint3D &v);

  //---

  void moveObject  (CGeomObject3D *object, const CVector3D &d);
  void scaleObject (CGeomObject3D *object, const CVector3D &d);
  void rotateObject(CGeomObject3D *object, double da, const CVector3D &axis);

  //---

  CQCamera3DShaderProgram *shaderProgram();

  //---

  // light
  const Lights &lights() const { return lights_; }

  CQCamera3DLight* currentLight() const;
  void setCurrentLight(CQCamera3DLight *);

  CQCamera3DLight* getLightById(uint id) const;

  bool isFixedDiffuse() const { return fixedDiffuse_; }
  void setFixedDiffuse(bool b) { fixedDiffuse_ = b; }

  // font/text
  CQCamera3DFont* getFont() const { return font_; }

  //---

  // cameras
  const Cameras &cameras() const { return cameras_; }

  CQCamera3DCamera* getCurrentCamera() const;
  void setCurrentCamera(CQCamera3DCamera *camera);

  void setCurrentCameraInd(uint ind);
  uint getCurrentCameraInd() const { return cameraInd_; }

  CQCamera3DCamera* getCameraById(uint id) const;

  bool isShowCamera() const { return showCamera_; }
  void setShowCamera(bool b) { showCamera_ = b; }

  void updateQuadCameras();

  //---

  // axes
  CQCamera3DAxes *axes() const { return axes_; }

  bool isShowAxes() const;
  void setShowAxes(bool b);

  //---

  // normals
  CQCamera3DNormals *getNormals() const { return normals_; }

  double normalsSize() const { return normalsSize_; }
  void setNormalsSize(double r) { normalsSize_ = r; }

  const QColor &normalsColor() const { return normalsColor_; }
  void setNormalsColor(const QColor &c) { normalsColor_ = c; }

  //---

  // basis
  CQCamera3DBasis *getBasis() const { return basis_; }

  bool isShowBasis() const { return showBasis_; }
  void setShowBasis(bool b) { showBasis_ = b; }

  //---

  // bbox
  CQCamera3DBBox *getBBoxOverlay() const { return bboxOverlay_; }

  bool isShowBBox() const { return showBBox_; }
  void setShowBBox(bool b) { showBBox_ = b; }

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

  bool isTextured() const { return textured_; }
  void setTextured(bool b) { textured_ = b; }

  bool isPoints() const { return points_; }
  void setPoints(bool b) { points_ = b; }

  //---

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

  const MouseType &mouseType() const { return mouseType_; }
  void setMouseType(const MouseType &v) { mouseType_ = v; }

  const EditType &editType() const { return editType_; }
  void setEditType(const EditType &v);

  double mouseScale() const { return mouseScale_; }
  void setMouseScale(double r) { mouseScale_ = r; }

  bool isMouseBasis() const { return mouseBasis_; }
  void setMouseBasis(bool b) { mouseBasis_ = b; }

  //---

  const SelectType &selectType() const { return selectType_; }
  void setSelectType(const SelectType &v) { selectType_ = v; }

  void selectAllObjects();
  bool selectObject(CGeomObject3D *object, bool update=true);

  bool selectFaces(const Faces &faces, bool clear, bool update);
  bool selectFace(CGeomFace3D *face, bool clear, bool update);

  void selectFaceEdge(CGeomFace3D *face, int ind);

  bool selectVertex(CGeomVertex3D *vertex, bool update=true);
  bool selectVertices(const ObjectSelectInds &vertices, bool update=true);

  bool deselectAll(bool update=true);

  Objects          getSelectedObjects () const;
  Faces            getSelectedFaces   () const;
  ObjectSelectInds getSelectedVertices() const;

  //---

  const CPoint3D &intersectPoint1() const { return intersectPoint1_; }
  const CPoint3D &intersectPoint2() const { return intersectPoint2_; }

  void setIntersectPoints(const CPoint3D &p1, const CPoint3D &p2) {
    intersectPoint1_ = p1; intersectPoint2_ = p2;
  }

  //---

  void addLight();
  void resetLight(CQCamera3DLight *light);

  //---

  void updateShapes();

  void updateAnnotation();

  void updateBillboard();

  void updateOverlay();
  void updateOverlay2D();

  //---

  CQCamera3DGeomObject *currentObject(bool includeRoot=false) const;
  CQCamera3DGeomObject *currentGeomObject(bool includeRoot=false) const;

  void setCurrentObject(CQCamera3DGeomObject *object, bool update);

  CGeomFace3D *currentFace() const { return currentFace_; }
  void setCurrentFace(CGeomFace3D *face, bool update);

  CGeomVertex3D *currentVertex() const { return currentVertex_; }
  void setCurrentVertex(CGeomVertex3D *vertex, bool update);

  FaceEdges getSelectedFaceEdges() const;

  //---

  CQCamera3DGeomObject *defaultRootObject() const;

  //---

  const ObjectDatas &getObjectDatas() const { return objectDatas_; }

  CGeomObject3D *getObject(int i) const;

  CQCamera3DObjectData *getObjectData(CGeomObject3D *object) const;

  CGeomObject3D *getGeomObject(CGeomObject3D *object) const;

  //---

  // textures

  void setInitTextureMap(const std::string &s) { initTextureMap_ = s; }

  bool loadTextureMap(const std::string &fileName);

  void addTextureFile(const std::string &fileName);

  //---

  // materials

  void setInitMaterialMap(const std::string &s) { initMaterialMap_ = s; }

  bool loadMaterialMap(const std::string &fileName);

  //---

  const ViewType &viewType() const { return viewType_; }
  void setViewType(const ViewType &v);

  CQCamera3DCamera *getViewCamera() const;

  //---

  const MoveDirection &moveDirection() const { return moveDirection_; }
  void setMoveDirection(const MoveDirection &v);

  //---

  void addCircle();
  void addCube();
  void addCylinder();
  void addPlane();
  void addPyramid();
  void addSphere();
  void addTorus();

  void addObject(const std::string &typeName, const AddObjectType &type);

  //---

  void extrudeMode();

  void extrude();
  void extrudeMove(double d);

  double extrudeMoveDelta() const { return extrudeMoveDelta_; }
  void setExtrudeMoveDelta(double r) { extrudeMoveDelta_ = r; }

  void loopCut();

  //---

  void setOptions(CQCamera3DOptions *options);
  void showOptions();
  void hideOptions();

  void endMouseMode();

  //---

  bool createFontTexture(uint *texture, int w, int h, uchar *data);

  //---

  void getBasis(CGeomObject3D *object, CVector3D &u, CVector3D &v, CVector3D &w) const;

  //---

  void initializeGL() override;

  void resizeGL(int, int) override;

  void paintGL() override;

  QSize sizeHint() const override { return QSize(1536, 1536); }

  //--

  void drawContents(CQCamera3DCamera *camera);

 private:
  void drawObjectsData();

  void addShaderLights(CQCamera3DShaderProgram *program);

  void updateNodeMatrices(CGeomObject3D *object);

  void initCamera();

  CQCamera3DObjectData *initObjectData(int i, CGeomObject3D *o);

  //---

  void mousePressEvent  (QMouseEvent *e) override;
  void mouseMoveEvent   (QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;

  void wheelEvent(QWheelEvent *) override;

  void selectObjectAtMouse();
  void moveCursorToMouse();
  void showEyelineAtMouse();

  void setEyeLineLabel();

  //---

  void keyPressEvent(QKeyEvent *event) override;

  void cameraKeyPress(QKeyEvent *event);
  void lightKeyPress (QKeyEvent *event);
  void objectKeyPress(QKeyEvent *event);
  void faceKeyPress  (QKeyEvent *event);
  void edgeKeyPress  (QKeyEvent *event);
  void pointKeyPress (QKeyEvent *event);

  void calcEyeLine(const CPoint3D &pos, EyeLine &eyeLine, bool verbose=false) const;

 public Q_SLOTS:
  void updateObjectsData();

  void addObjectsData();

 private Q_SLOTS:
  void updateStatus();

  void cameraChanged();

 Q_SIGNALS:
  void stateChanged();

  void materialAdded();

  void cameraAdded();

  void lightAdded();

  void objectAdded();
  void objectsChanged();

  void eyeLineChanged();

  void editTypeChanged();

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

  //---

  // fonts
  CQCamera3DFont *font_ { nullptr };

  //---

  // axis
  CQCamera3DAxes *axes_ { nullptr };

  //---

  // cameras
  Cameras cameras_;
  uint    cameraInd_  { 0 };
  bool    showCamera_ { false };

  struct CameraData {
    CQCamera3DCamera* camera      { nullptr };
    CMatrix3DH        worldMatrix { CMatrix3DH::identity() };
    CMatrix3DH        viewMatrix  { CMatrix3DH::identity() };
    CVector3D         viewPos     { 0, 0, 0 };
  };

  CameraData cameraData_;

  CQCamera3DCamera *perspectiveCamera_ { nullptr };
  CQCamera3DCamera *topCamera_         { nullptr };
  CQCamera3DCamera *bottomCamera_      { nullptr };
  CQCamera3DCamera *frontCamera_       { nullptr };
  CQCamera3DCamera *backCamera_        { nullptr };
  CQCamera3DCamera *leftCamera_        { nullptr };
  CQCamera3DCamera *rightCamera_       { nullptr };

  //---

  // lighting
  CRGBA  ambientColor_    { 1, 1, 1 };
  double ambientStrength_ { 0.1 };

  double diffuseStrength_  { 1.0 };
  double specularStrength_ { 0.2 };
  double emissiveStrength_ { 0.1 };

  double shininess_ { 32.0 };

  Lights lights_;
  int    lightInd_ { 0 };
  bool   fixedDiffuse_ { false };

  //---

  // normals
  CQCamera3DNormals* normals_      { nullptr};
  double             normalsSize_  { -1.0 };
  QColor             normalsColor_ { 255, 255, 255 };

  // basis
  CQCamera3DBasis* basis_     { nullptr};
  bool             showBasis_ { false };

  // bbox
  CQCamera3DBBox* bboxOverlay_ { nullptr};
  bool            showBBox_    { false };

  //---

  // views

  CQCamera3DShape* shape_ { nullptr };

  CQCamera3DAnnotation* annotation_ { nullptr };

  CQCamera3DBillboard* billboard_ { nullptr };

  CQCamera3DOverlay*   overlay_   { nullptr };
  CQCamera3DOverlay2D* overlay2D_ { nullptr };

  //----

  // globals
  CQCamera3DShaderProgram *shaderProgram_ { nullptr };

  PaintData paintData_;

   //---

  // objects

  ObjectDatas objectDatas_;

  //---

  // textures
  std::string initTextureMap_;

  //---

  // materials

  std::string initMaterialMap_;

  //---

  // state
  bool    quadView_ { false };
  CBBox3D bbox_;

  CPoint3D cursor_ { 0, 0, 0 };

  ViewType viewType_ { ViewType::PERSPECTIVE };

  MoveDirection moveDirection_ { MoveDirection::NONE };

  double extrudeMoveDelta_ { 0.0 };

  //---

  // draw types

  bool wireframe_ { false };
  bool solid_     { true };
  bool textured_  { true };
  bool points_    { false };

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

  MouseType mouseType_  { MouseType::OBJECT };
  EditType  editType_   { EditType::SELECT };
  double    mouseScale_ { 0.05 };
  bool      mouseBasis_ { false };

  SelectType selectType_ { SelectType::OBJECT };

  CQCamera3DGeomObject* currentObject_ { nullptr };
  CGeomFace3D*          currentFace_   { nullptr };
  CGeomVertex3D*        currentVertex_ { nullptr };

  CQCamera3DOpWidget* opWidget_ { nullptr };

  CQCamera3DMouseModeMgr* mouseModeMgr_ { nullptr };
};

#endif
