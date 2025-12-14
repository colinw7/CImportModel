#ifndef CQCamera3DApp_H
#define CQCamera3DApp_H

#include <QFrame>

#include <CGeom3DType.h>

class CQCamera3DCanvas;
class CQCamera3DOverview;
class CQCamera3DUVMap;
class CQCamera3DTextures;
class CQCamera3DControl;
class CQCamera3DStatus;

class CGeomScene3D;
class CGeomObject3D;

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

 public:
  CQCamera3DApp();

  //---

  CGeomScene3D* getScene() const { return scene_; }

  bool loadModel(const QString &fileName, CGeom3DType format, LoadData &loadData);

  //---

  const QString &buildDir() const { return buildDir_; }
  void setBuildDir(const QString &s) { buildDir_ = s; }

  //---

  CQCamera3DCanvas*   canvas  () const { return canvas_  ; }
  CQCamera3DOverview* overview() const { return overview_; }
  CQCamera3DUVMap*    uvMap   () const { return uvMap_   ; }
  CQCamera3DTextures* textures() const { return textures_; }
  CQCamera3DControl*  control () const { return control_ ; }
  CQCamera3DStatus*   status  () const { return status_  ; }

 Q_SIGNALS:
  void modelAdded();
  void textureAdded();

 private:
  QString buildDir_;

  // object data
  CGeomScene3D* scene_ { nullptr };

  //---

  // widgets
  CQCamera3DCanvas*   canvas_   { nullptr };
  CQCamera3DOverview* overview_ { nullptr };
  CQCamera3DUVMap*    uvMap_    { nullptr };
  CQCamera3DTextures* textures_ { nullptr };
  CQCamera3DControl*  control_  { nullptr };
  CQCamera3DStatus*   status_   { nullptr };
};

#endif
