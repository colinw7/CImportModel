#ifndef CQNewGLCanvas_H
#define CQNewGLCanvas_H

#include <CQNewGLShaderProgram.h>
#include <CQNewGLFaceData.h>

#include <CGeom3DType.h>
#include <CGLVector3D.h>
#include <CGLCamera.h>
#include <CQGLBuffer.h>

#include <CPoint3D.h>
#include <CRGBA.h>
#include <CImagePtr.h>

#include <QGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFunctions_3_3_Core>
#include <QMatrix4x4>

class CQNewGLModelObject;
class CQNewGLModel;
class CQNewGLCamera;
class CQNewGLLight;
class CQNewGLFont;
class CQNewGLText;
class CQNewGLAxes;
class CQNewGLBBox;
class CQNewGLNormals;
class CQNewGLWireframe;
class CQNewGLHull;
class CQNewGLBasis;
class CQNewGLTerrain;
class CQNewGLMaze;
class CQNewGLSkybox;
class CQNewGLEmitter;
class CQNewGLFractal;
class CQNewGLDrawTree;
class CQNewGLShape;
class CQNewGLPath;
class CQNewGLObject;

class CQGLBuffer;
class CQGLTexture;
class CGeomScene3D;
class CGeomObject3D;
class CGeomTexture;
class CBBox3D;

class CDungeon;

//---

//class CQNewGLCanvas : public QGLWidget, public QOpenGLFunctions_3_3_Core {
class CQNewGLCanvas : public QGLWidget, public QOpenGLExtraFunctions {
  Q_OBJECT

  Q_PROPERTY(QColor bgColor READ bgColor WRITE setBgColor)

  Q_PROPERTY(bool depthTest READ isDepthTest WRITE setDepthTest)
  Q_PROPERTY(bool cullFace  READ isCullFace  WRITE setCullFace )
  Q_PROPERTY(bool frontFace READ isFrontFace WRITE setFrontFace)

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
    CQGLTexture*  glTexture   { nullptr };
    CGeomTexture* geomTexture { nullptr };
  };

  using GLTextures = std::map<int, TextureData>;

  //---

  using ObjectDatas = std::map<CGeomObject3D *, CQNewGLModelObject *>;

  using Cameras = std::vector<CQNewGLCamera *>;
  using Lights  = std::vector<CQNewGLLight  *>;
  using Shapes  = std::vector<CQNewGLShape  *>;

  //---

  struct LoadData {
    bool invertX { false };
    bool invertY { false };
    bool invertZ { false };

    std::vector<CGeomObject3D *> objects;

    LoadData() { }
  };

  //---

  using Emitters = std::vector<CQNewGLEmitter *>;
  using Paths    = std::vector<CQNewGLPath *>;

 public:
  CQNewGLCanvas(CQNewGLModel *app);

  CQNewGLModel* app() const { return app_; }

  CGeomScene3D* scene() const { return scene_; }

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

  //---

  // normals
  bool isShowNormals() const { return showNormals_; }
  void setShowNormals(bool b);

  double normalsSize() const { return normalsSize_; }
  void setNormalsSize(double r);

  const QColor &normalsColor() const { return normalsColor_; }
  void setNormalsColor(const QColor &c);

  double calcNormalsSize() const;

  bool isTangentSpaceNormal() const { return tangentSpaceNormal_; }
  void setTangentSpaceNormal(bool b);

  //---

  void enableDepthTest();
  void enableCullFace();
  void enableFrontFace();
  void enablePolygonLine();

  //---

  bool isTimerRunning() const { return timerRunning_; }
  void setTimerRunning(bool b);

  void startTimer();
  void stopTimer();

  //---

//const CVector3D &modelTranslate() const { return modelTranslate_; }
//void setModelTranslate(const CVector3D &v) { modelTranslate_ = v; }

//const CVector3D &modelRotate() const { return modelRotate_; }
//void setModelRotate(const CVector3D &v) { modelRotate_ = v; }

  //---

  // objects
  void setCurrentObject(CGeomObject3D *object);

  int currentObjectNum() const { return currentObjectNum_; }
  void setCurrentObjectNum(int i);

  CGeomObject3D *getCurrentObject() const;
  CGeomObject3D *getObject(int ind) const;

  CQNewGLModelObject *getCurrentObjectData() const;
  CQNewGLModelObject *getObjectData(int ind) const;
  CQNewGLModelObject *getObjectData(CGeomObject3D *object);

  CGeomObject3D *getSelectedObject() const;

  const ObjectDatas &getObjectDatas() { return objectDatas_; }

  //--

  // bones
//CGeomObject3D *getBonesObject() const;

  bool isShowBonePoints() const { return bonesData_.showPoints; }
  void setShowBonePoints(bool b);

  bool isShowBoneVertices() const { return bonesData_.showVertices; }
  void setShowBoneVertices(bool b);

  bool isBonesEnabled() const { return bonesData_.enabled; }
  void setBonesEnabled(bool b) { bonesData_.enabled = b; }

  const BonesTransform &bonesTransform() const { return bonesData_.transform; }
  void setBonesTransform(BonesTransform t) { bonesData_.transform = t; }

  const CRGBA &bonesColor() const { return bonesData_.color; }
  void setBonesColor(const CRGBA &c) { bonesData_.color = c; }

  const CRGBA &bonesSelectedColor() const { return bonesData_.selectedColor; }
  void setBonesSelectedColor(const CRGBA &c) { bonesData_.selectedColor = c; }

  int currentBone() const { return currentBone_; }
  void setCurrentBone(int i) { currentBone_ = i; }

  //---

  // cameras
  bool isShowCameras() const { return showCameras_; }
  void setShowCameras(bool b);

  const Cameras &cameras() { return cameras_; }

  int currentCamera() const { return currentCamera_; }
  void setCurrentCamera(int i);

  CQNewGLCamera *getCurrentCamera() const;
  CQNewGLCamera *getCamera(int i) const;

  void setCameraRotate(bool b);
  void setCameraRotateAt(const CGLCamera::RotateAt &at);

  //---

  // lights
  bool isShowLights() const { return showLights_; }
  void setShowLights(bool b);

  const Lights &lights() { return lights_; }

  int currentLight() const { return currentLight_; }
  void setCurrentLight(int i);

  CQNewGLLight *getCurrentLight() const;

  //---

  // font
  CQNewGLFont *getFont() const { return font_; }

  //---

  // axes
  CQNewGLAxes *getAxes() const { return axes_; }

  //---

  // bbox
  CQNewGLBBox *getBBox() const { return bbox_; }

  //---

  // normal
  CQNewGLNormals *getNormals() const { return normals_; }

  //---

  // normal
  CQNewGLWireframe *getWireframe() const { return wireframe_; }

  //---

  // hull
  CQNewGLHull *getHull() const { return hull_; }

  //---

  // basis
  CQNewGLBasis *getBasis() const { return basis_; }

  //---

  // terrain
  CQNewGLTerrain *getTerrain() const { return terrain_; }

  //---

  // maze
  CQNewGLMaze *getMaze() const { return maze_; }

  //---

  // skybox
  CQNewGLSkybox *getSkybox() const { return skybox_; }

  //---

  // fractal
  CQNewGLFractal *getFractal() const { return fractal_; }

  //---

  // draw tree
  CQNewGLDrawTree *getDrawTree() const { return drawTree_; }

  //---

  // shapes
  const Shapes &shapes() { return shapes_; }

  int currentShape() const { return currentShape_; }
  void setCurrentShape(int i);

  CQNewGLShape *getCurrentShape() const;

  //---

  // eye line
  bool isShowEyeline() const { return showEyeline_; }
  void setShowEyeline(bool b) { showEyeline_ = b; }

  //---

  bool isAnimEnabled() const { return animEnabled_; }
  void setAnimEnabled(bool b) { animEnabled_ = b; }

  const QString &animName() const { return animName_; }
  void setAnimName(const QString &s) { animName_ = s; }

  //---

  struct BoneData {
    std::string           name;
    int                   ind { 0 };
    std::vector<CPoint3D> vertices;
  };

  void getBoneData(CGeomObject3D *object, int boneId, BoneData &boneData) const;
  void getNodeVertices(CGeomObject3D *object, int nodeId, BoneData &boneData) const;

  //---

  const CGLMatrix3D &projectionMatrix() const { return paintData_.projection; }
  const CGLMatrix3D &viewMatrix() const { return paintData_.view; }

  const CGLVector3D &viewPos() const { return paintData_.viewPos; }

  CMatrix3D getModelMatrix() const;

  double sceneScale() const { return sceneScale_; }

  //---

  bool loadModel(const QString &modelName, CGeom3DType format, LoadData &loadData);

  void setInitTextures(const InitTextureDatas &textures);
  void setTextureMap(const std::string &map);

  void initializeGL() override;

  void paintGL() override;

  void resizeGL(int, int) override;

  void resetRange();

  //---

  void addCamera  (bool update=true);
  void resetCamera();

  void addLight (bool update=true);

  //---

  bool createFontTexture(uint *texture, int w, int h, uchar *data);

  //---

  void mousePressEvent  (QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent   (QMouseEvent *event) override;

  void wheelEvent(QWheelEvent *) override;

  void keyPressEvent(QKeyEvent *event) override;

  void setMousePos(double x, double y, bool add, bool show);

  void calcEyeLine(const CPoint2D &pos, CPoint3D &ep1, CPoint3D &ep2, CVector3D &ev);

  //---

  void updateObjectsData();
  void updateObjectData(CGeomObject3D *object);

  void updateModelData(CBBox3D &bbox);

  //---

  // bones
  void updateObjectBones(CBBox3D &bbox);
  void drawObjectsBones();
  void drawObjectBones(CQNewGLModelObject *objectData);

  void updateObjectBone(CBBox3D &bbox);
  void drawObjectBone(CQNewGLModelObject *objectData);

  void drawObjectAnnotation(CQNewGLModelObject *objectData);

  //---

  void updateCameraBuffer();

  //---

  // axes
  void updateAxes();
  void drawAxes();

  //---

  // bbox
  void updateBBox();
  void drawBBox();

  //---

  // normal
  void updateNormals();
  void drawNormals();

  //---

  // wireframe
  void updateWireframe();
  void drawWireframe();

  //---

  // hull
  void updateHull();
  void drawHull();

  //---

  // terrain
  void updateTerrain();
  void drawTerrain();

  //---

  // maze
  void updateMaze(CBBox3D &bbox);
  void drawMaze();

  //---

  // skybox
  void updateSkybox();
  void drawSkybox();

  //---

  // emitters
  const Emitters &getEmitters() const { return emitters_; }

  void addEmitter();

  CQNewGLEmitter *getCurrentEmitter() const;
  void setCurrentEmitterNum(int n) { emitterNum_ = n; }

  void updateEmitters();
  void drawEmitters();

  //---

  // fractal
  void updateFractal();
  void drawFractal();

  //---

  // draw tree
  void updateDrawTree();
  void drawDrawTree();

  //---

  // shape
  CQNewGLShape *addShape();
  void deleteShape(CQNewGLShape *shape);

  void drawShapes();

  //---

  void clearEyeLines();

  void drawPaths();

  //---

  // shader helpers
  void addShaderMVP(CQNewGLShaderProgram *program, const CMatrix3D &modelMatrix);
  void addShaderLightGlobals(CQNewGLShaderProgram *program);
  void addShaderLights(CQNewGLShaderProgram *program);
  void addShaderCurrentLight(CQNewGLShaderProgram *program);

  //---

  void updateLightBuffer();

  //---

  // bones
  void addBonesCube(CQNewGLModelObject *objectData, const CMatrix3D &m, double cubeSize, int nodeId,
                    const CRGBA &color, bool selected, int &pos, CBBox3D &bbox) const;
  void addBonesLine(CQNewGLModelObject *objectData, const CPoint3D &c1, const CPoint3D &c2,
                    double size, int boneId, int parentBoneId, const CRGBA &color, int &pos) const;

  void addBoneCube(CQNewGLModelObject *objectData, const CMatrix3D &m, double cubeSize, int nodeId,
                   const CRGBA &color, int &pos, CBBox3D &bbox) const;

  CPoint3D applyBonePointTransform(const CPoint3D &p, int *boneIds, double *boneWeights) const;
  CPoint3D applyBoneTransform(const CPoint3D &p, int boneId) const;

  //---

  void setSceneBBox(const CBBox3D &bbox);

  //---

  const GLTextures &glTextures() const { return glTextures_; }

  CQGLTexture  *getTextureByName    (const std::string &name) const;
  CGeomTexture *getGeomTextureByName(const std::string &name) const;

  CQGLTexture *makeTexture(const CImagePtr &image) const;

  CQGLTexture *getTexture(CGeomTexture *texture, bool add);

  //---

  void placeObjectCamera(CGeomObject3D *);

  //---

  struct ShapeData {
    CRGBA    color { 1, 1, 1};
    CPoint3D size  { 1, 1, 1};
  };

  void addCube(CQGLBuffer *buffer, const CBBox3D &bbox, const ShapeData &data,
               std::vector<CQNewGLFaceData> &faceDatas, int &pos) const;
  void addCube(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, const ShapeData &data,
               std::vector<CQNewGLFaceData> &faceDatas, int &pos) const;
  void addCone(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, double w,
               const ShapeData &data, std::vector<CQNewGLFaceData> &faceDatas, int &pos) const;
  void addCylinder(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, double r,
                   const ShapeData &data, std::vector<CQNewGLFaceData> &faceDatas,
                   int &pos) const;
  void addSphere(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, const ShapeData &data,
                 double angleStart, double angleDelta,
                 std::vector<CQNewGLFaceData> &faceDatas, int &pos) const;
  void addPyramid(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, double w,
                  const ShapeData &data, std::vector<CQNewGLFaceData> &faceDatas, int &pos) const;
  void addTorus(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, double r1,
                double power1, double power2, const ShapeData &data,
                std::vector<CQNewGLFaceData> &faceDatas, int &pos) const;

  CMatrix3D getShapeRotationMatrix(const CPoint3D &p1, const CPoint3D &p2) const;

  //---

  CBBox3D getObjectBBox(CGeomObject3D *object) const;
  CBBox3D getModelBBox(CGeomObject3D *object) const;

  //---

  void addAnnotationObject(CQNewGLObject *);
  std::vector<CQNewGLObject *> getAnnotationObjects() const;
  void updateAnnotationObjects();

  //---

  std::vector<CGeomObject3D *> getRootObjects() const;

  //---

  void getBasis(CGeomObject3D *object, CVector3D &u, CVector3D &v, CVector3D &w) const;

 Q_SIGNALS:
  void modelMatrixChanged();

  void timerStep();

  void currentObjectChanged();
  void currentCameraChanged();
  void currentLightChanged();
  void currentShapeChanged();

  void modelAdded();
  void textureAdded();
  void cameraAdded();
  void lightAdded();

  void shapeAdded();
  void shapeDeleted();

 private:
  void loadInitTextures();

  GLuint loadTexture(const std::string &name);

  void drawObjectModel(CQNewGLModelObject *objectData);

  void drawBasis();

  void drawCameras();
  void drawLights();

  void drawObjectNormals(CQNewGLModelObject *objectData);

  CMatrix3D getMeshGlobalTransform(CGeomObject3D *object, bool invert) const;
//CMatrix3D getMeshLocalTransform (CGeomObject3D *object, bool invert) const;

  void updateNodeMatrices(CQNewGLModelObject *objectData);

 public Q_SLOTS:
  void timerSlot();

 private:
  struct PaintData {
    CGLVector3D viewPos;
    CGLMatrix3D projection;
    CGLMatrix3D view;

    std::vector<CMatrix3D>  nodeMatrices;
    std::vector<QMatrix4x4> nodeQMatrices;
    int                     numNodeMatrices { 128 };
  };

  struct TextureMapData {
    std::string diffuse;
    std::string normal;
    std::string specular;
    std::string emissive;
  };

  using TextureMap = std::map<std::string, TextureMapData>;

  CQNewGLModel* app_            { nullptr };
  bool          initialized_    { false };
  CGeomScene3D* scene_;
  QColor        bgColor_        { 51, 76, 76 };
  QColor        ambientColor_   { 100, 100, 100 };
  QColor        diffuseColor_   { 255, 255, 255 };
  QColor        emissionColor_  { 0, 0, 0 };
  bool          depthTest_      { true };
  bool          cullFace_       { false };
  bool          frontFace_      { true };
  bool          polygonSolid_   { true };
  bool          polygonLine_    { false };
  bool          ortho_          { false };
  double        time_           { 0.0 };

  // scene
  CVector3D sceneSize_     { 1.0, 1.0, 1.0 };
  double    sceneScale_    { 1.0 };
  float     invSceneScale_ { 1.0f };
  CPoint3D  sceneCenter_   { 0, 0, 0 };

  PaintData paintData_;

  int pixelWidth_  { 0 };
  int pixelHeight_ { 0 };

  double aspect_ { 1.0 };

  // objects
  ObjectDatas objectDatas_;
  int         currentObjectNum_ { 0 };

//CVector3D modelRotate_    { 0.0, 0.0, 0.0 };
//CVector3D modelScale_     { 1.0, 1.0, 1.0 };
//CVector3D modelTranslate_ { 0.0, 0.0, 0.0 };

  CQNewGLShaderProgram* normalShaderProgram_ { nullptr };

  // camera
  bool                showCameras_    { false };
  Cameras             cameras_;
  int                 currentCamera_  { 0 };
  bool                cameraRotate_   { false };
  CGLCamera::RotateAt cameraRotateAt_ { CGLCamera::RotateAt::ORIGIN };

  // normals
  bool   showNormals_  { false };
  double normalsSize_  { -1.0 };
  QColor normalsColor_ { 255, 255, 255 };

  bool tangentSpaceNormal_ { true };

  // light
  bool   showLights_ { false };
  Lights lights_;
  int    currentLight_ { 0 };

  // anim
  bool    animEnabled_ { true };
  QString animName_;

  // bones
  struct BonesData {
    bool           enabled       { false };
    bool           showPoints    { false };
    bool           showVertices  { false };
    BonesTransform transform     { BonesTransform::INVERSE_BIND };
    CRGBA          color         { 1.0, 0.0, 0.0 };
    CRGBA          selectedColor { 1.0, 1.0, 1.0 };
  };

  BonesData bonesData_;

  CQNewGLShaderProgram* bonesShaderProgram_ { nullptr };
  CQNewGLShaderProgram* boneShaderProgram_  { nullptr };

  int currentBone_ { -1 };

  // basis
  CQNewGLBasis *basis_ { nullptr };

  // textures
  InitTextureDatas initTextures_;
  TextureMap       textureMap_;
  GLTextures       glTextures_;

  // font
  CQNewGLFont* font_ { nullptr };
//CQNewGLText* text_ { nullptr };

  // axes
  CQNewGLAxes* axes_ { nullptr };

  // bbox
  CQNewGLBBox* bbox_ { nullptr };

  // normal
  CQNewGLNormals* normals_ { nullptr };

  // wireframe
  CQNewGLWireframe* wireframe_ { nullptr };

  // hull
  CQNewGLHull* hull_ { nullptr };

  // terrain
  CQNewGLTerrain* terrain_ { nullptr };

  // maze
  CQNewGLMaze* maze_ { nullptr };

  // terrain
  CQNewGLSkybox* skybox_ { nullptr };

  // emitters
  Emitters emitters_;
  int      emitterNum_ { 0 };

  // fractal
  CQNewGLFractal* fractal_ { nullptr };

  // draw tree
  CQNewGLDrawTree* drawTree_ { nullptr };

  //---

  using AnnotationObjects = std::vector<CQNewGLObject *>;

  AnnotationObjects annotationObjects_;

  // shapes
  Shapes shapes_;
  int    currentShape_ { 0 };

  // paths
  Paths paths_;

  // eye line
  bool         showEyeline_ { false };
  CQNewGLPath* eyeLine1_    { nullptr };
  CQNewGLPath* eyeLine2_    { nullptr };

  CVector3D eyeVector1_;
  CVector3D eyeVector2_;

  // mouse
  struct MouseData {
    double          pressX { 0.0 };
    double          pressY { 0.0 };
    double          moveX  { 0.0 };
    double          moveY  { 0.0 };
    Qt::MouseButton button { Qt::NoButton };
  };

  MouseData mouseData_;

  // timer
  QTimer* timer_        { nullptr };
  bool    timerRunning_ { false };
};

#endif
