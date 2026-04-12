#ifndef CQCamera3DApp_H
#define CQCamera3DApp_H

#include <QFrame>

#include <CGeom3DType.h>
#include <CMatrix3D.h>
#include <CBBox3D.h>

#include <set>

class CQCamera3DCanvas;
class CQCamera3DToolbar;
class CQCamera3DSidebar;
class CQCamera3DOverview;
class CQCamera3DUVMap;
class CQCamera3DTextures;
class CQCamera3DMaterials;
class CQCamera3DBones;
class CQCamera3DControl;
class CQCamera3DStatus;

class CQTabSplit;

class CGeomScene3D;
class CGeomObject3D;
class CGeomFace3D;
class CGeomVertex3D;
class CGeomTexture;
class CGeomMaterial;

enum class CQCamera3DViewType {
  NONE,
  MODEL,
  OVERVIEW,
  UV,
  TEXTURES,
  MATERIALS,
  BONES,
  ANIMATION
};

enum class CQCamera3DSelectType {
  OBJECT,
  FACE,
  EDGE,
  POINT
};

enum class CQCamera3DEditType {
  SELECT,
  CURSOR,
  CAMERA,
  LIGHT,
  MOVE,
  ROTATE,
  SCALE
};

#if 0
enum class CQCamera3DMouseType {
  CAMERA,
  LIGHT,
  OBJECT,
  FACE,
  EDGE,
  POINT
};
#endif

enum class CQCamera3DAddObjectType {
  NONE,
  CIRCLE,
  CONE,
  CUBE,
  CYLINDER,
  PLANE,
  PYRAMID,
  SPHERE,
  TORUS
};

enum class CQCamera3DMoveDirection {
  NONE,
  X,
  Y,
  Z
};

enum class CQCamera3DTimeType {
  STEP,
  FRAMES
};

//---

struct CQCamera3DSelectData {
  using SelectType = CQCamera3DSelectType;

  using Objects   = std::vector<CGeomObject3D *>;
  using Faces     = std::vector<CGeomFace3D *>;
  using Edges     = std::vector<int>;
  using FaceEdges = std::map<CGeomFace3D *, Edges>;

  using SelectInds       = std::set<int>;
  using ObjectSelectInds = std::map<CGeomObject3D *, SelectInds>;

  SelectType       type { SelectType::OBJECT };
  Objects          objects;
  Faces            faces;
  FaceEdges        faceEdges;
  ObjectSelectInds vertices;
};

//---

class CQCamera3DApp : public QFrame {
  Q_OBJECT

 public:
  using Objects = std::vector<CGeomObject3D *>;

  struct LoadData {
    bool        invertX     { false };
    bool        invertY     { false };
    bool        invertZ     { false };
    bool        swapXY      { false };
    bool        swapYZ      { false };
    bool        swapZX      { false };
    bool        triangulate { false };
    std::string textureDir;

    Objects objects;
  };

  using NodeMatrices       = std::map<int, CMatrix3D>;
  using ObjectNodeMatrices = std::map<uint, NodeMatrices>;

 public:
  CQCamera3DApp();

  void init();

  //---

  bool isMultisample() const { return multisample_; }
  void setMultisample(bool b) { multisample_ = b; }

  //---

  CGeomScene3D* getScene() const { return scene_; }

  bool loadModel(const QString &fileName, CGeom3DType format, LoadData &loadData);

  //---

  const QString &buildDir() const { return buildDir_; }
  void setBuildDir(const QString &s) { buildDir_ = s; }

  //---

  CQCamera3DToolbar*   toolbar  () const { return toolbar_  ; }
  CQCamera3DSidebar*   sidebar  () const { return sidebar_  ; }
  CQCamera3DCanvas*    canvas   () const { return canvas_   ; }
  CQCamera3DOverview*  overview () const { return overview_ ; }
  CQCamera3DUVMap*     uvMap    () const { return uvMap_    ; }
  CQCamera3DTextures*  textures () const { return textures_ ; }
  CQCamera3DMaterials* materials() const { return materials_; }
  CQCamera3DBones*     bones    () const { return bones_    ; }
  CQCamera3DControl*   control  () const { return control_  ; }
  CQCamera3DStatus*    status   () const { return status_   ; }

  //---

  const CQCamera3DViewType &viewType() const { return viewType_; }

  void setCurrentView(const CQCamera3DViewType &viewType);

  int viewTypeToInd(const CQCamera3DViewType &viewType) const;
  CQCamera3DViewType indToViewType(int ind) const;

  bool isSyncView() const { return syncView_; }
  void setSyncView(bool b) { syncView_ = b; }

  //---

  bool isTimerRunning() const { return timerRunning_; }
  void setTimerRunning(bool b);

  void startTimer();
  void stopTimer();

  double time() const { return time_; }
  void setTime(double t) { time_ = t; }

  //---

  int currentTextureId() const { return currentTextureId_; }
  void setCurrentTextureId(int id);

  //---

  const QString &currentMaterial() const { return currentMaterial_; }
  void setCurrentMaterial(const QString &s);

  CGeomMaterial *getMaterialByName(const std::string &name) const;

  //---

  CGeomObject3D *currentBoneObject() const;

  int currentBoneObjectInd() const { return currentBoneObjectInd_; }
  void setCurrentBoneObjectInd(int ind);

  int currentBoneNode() const { return currentBoneNode_; }
  void setCurrentBoneNode(int i);

  //---

  bool isAnimEnabled() const { return animEnabled_; }
  void setAnimEnabled(bool b) { animEnabled_ = b; }

  const CQCamera3DTimeType &timeType() const { return timeType_; }
  void setTimeType(const CQCamera3DTimeType &v) { timeType_ = v; }

  int timeFrames() const { return timeFrames_; }
  void setTimeFrames(int i) { timeFrames_ = i; }

  //---

  const NodeMatrices &getObjectNodeMatrices(CGeomObject3D *object) const;

  const ObjectNodeMatrices &getNodeMatrices() const;

  ObjectNodeMatrices calcNodeMatrices() const;

  CPoint3D adjustAnimPoint(const CGeomVertex3D &vertex, const CPoint3D &p,
                           const NodeMatrices &nodeMatrices) const;

  bool getNodeMatrix(const NodeMatrices &nodeMatrices, int nodeId, CMatrix3D &m) const;

  void invalidateNodeMatrices() { objectNodeMatricesValid_ = false; }

  //---

  QStringList getAnimNames() const;

//std::vector<CGeomObject3D *> getRootObjects() const;
  std::vector<CGeomObject3D *> getAnimObjects() const;

  void stepAnimObjects();

  void signalAnimStateChange();
  void signalAnimTimeChange();

  //---

  CGeomTexture *getTextureByName(const std::string &name) const;
  CGeomTexture *getTextureById(int id) const;

  //---

  bool writeScene(CGeomScene3D *scene) const;

  //---

  CBBox3D transformBBox(const CBBox3D &bbox, const CMatrix3D &matrix) const;

  //---

  void showMetaEdit();
  void showPerfDialog();
  void showAppOptions();

 private:
  void connectSlots(bool b);

 public Q_SLOTS:
  void tabSlot(int i);

  void timerSlot();

 Q_SIGNALS:
  void viewTypeChanged();

  void timerStep();

  void modelAdded();

  void currentTextureChanged();
  void textureAdded();
  void textureChanged();

  void currentMaterialChanged();
  void materialAdded();
  void materialChanged();

  void boneNodeChanged();

  void animStateChanged();
  void animTimeChanged();

 private:
  bool multisample_ { false };

  QString buildDir_;

  // object data
  CGeomScene3D* scene_ { nullptr };

  CQCamera3DViewType viewType_ { CQCamera3DViewType::NONE };

  //---

  // widgets
  CQTabSplit* tab_ { nullptr };

  CQCamera3DToolbar*   toolbar_   { nullptr };
  CQCamera3DSidebar*   sidebar_   { nullptr };
  CQCamera3DCanvas*    canvas_    { nullptr };
  CQCamera3DOverview*  overview_  { nullptr };
  CQCamera3DUVMap*     uvMap_     { nullptr };
  CQCamera3DTextures*  textures_  { nullptr };
  CQCamera3DMaterials* materials_ { nullptr };
  CQCamera3DBones*     bones_     { nullptr };
  QFrame*              animation_ { nullptr };

  CQCamera3DControl* control_ { nullptr };

  CQCamera3DStatus* status_ { nullptr };

  bool syncView_ { false };

  // timer
  QTimer* timer_        { nullptr };
  bool    timerRunning_ { false };
  uint    ticks_        { 0 };
  double  time_         { 0.0 };

  // textures
  int currentTextureId_ { -1 };

  // material
  QString currentMaterial_;

  // bones
  int currentBoneObjectInd_ { -1 };
  int currentBoneNode_      { -1 };

  // anim
  bool animEnabled_ { true };

  CQCamera3DTimeType timeType_ { CQCamera3DTimeType::STEP };

  int timeFrames_ { 60 };

  ObjectNodeMatrices objectNodeMatrices_;
  bool               objectNodeMatricesValid_ { false };
};

#endif
