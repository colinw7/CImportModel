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
    bool invertX { false };
    bool invertY { false };
    bool invertZ { false };
    bool swapXY  { false };
    bool swapYZ  { false };
    bool swapZX  { false };

    Objects objects;
  };

  struct AnimData {
    QString name;
    double  tmin { 0.0 };
    double  tmax { 0.0 };
  };

  using NodeMatrices       = std::map<int, CMatrix3D>;
  using ObjectNodeMatrices = std::map<uint, NodeMatrices>;

 public:
  CQCamera3DApp();

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

  int currentBoneObject() const { return currentBoneObject_; }
  void setCurrentBoneObject(int i);

  int currentBoneNode() const { return currentBoneNode_; }
  void setCurrentBoneNode(int i);

  const QString &animName() const { return animName_; }
  void setAnimName(const QString &s);

  double animTime() const { return animTime_; }
  void setAnimTime(double r);

  double animTimeStep() const { return animTimeStep_; }
  void setAnimTimeStep(double r) { animTimeStep_ = r; }

  //---

  const NodeMatrices &getObjectNodeMatrices(CGeomObject3D *object) const;

  const ObjectNodeMatrices &getNodeMatrices() const;

  ObjectNodeMatrices calcNodeMatrices() const;

  CPoint3D adjustAnimPoint(const CGeomVertex3D &vertex, const CPoint3D &p,
                           const NodeMatrices &nodeMatrices) const;

  //---

  std::vector<AnimData> getAnimNames() const;

  std::vector<CGeomObject3D *> getRootObjects() const;

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

  void animNameChanged();
  void animTimeChanged();

 private:
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
  int currentBoneObject_ { -1 };
  int currentBoneNode_   { -1 };

  // anim
  QString animName_;
  double  animTime_     { 0.0 };
  double  animTimeStep_ { 100.0 };

  ObjectNodeMatrices objectNodeMatrices_;
  bool               objectNodeMatricesValid_ { false };
};

#endif
