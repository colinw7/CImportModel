#ifndef App_H
#define App_H

#include <CGeom3DType.h>

#include <QFrame>

class CGeomScene3D;

namespace CQShadow3D {

class Canvas;
class Overview;
class Control;
class Toolbar;
class Status;

class App : public QFrame {
  Q_OBJECT

 public:
  App();

  //---

  Canvas *canvas() const { return canvas_; }

  Overview* overview() const { return overview_; }

  Control *control() const { return control_; }

  Status *status() const { return status_; }

  //---

  const QString &buildDir() const { return buildDir_; }
  void setBuildDir(const QString &s) { buildDir_ = s; }

  CGeomScene3D *scene() const { return scene_; }

  //---

  bool isShadowMapDebug() const { return shadowMapDebug_; }
  void setShadowMapDebug(bool b) { shadowMapDebug_ = b; }

  //---

  bool loadModel(const QString &fileName, CGeom3DType format);

 Q_SIGNALS:
  void modelAdded();

 private:
  QString buildDir_;

  // object data
  CGeomScene3D* scene_ { nullptr };

  Canvas*   canvas_   { nullptr };
  Overview* overview_ { nullptr };
  Control*  control_  { nullptr };
  Toolbar*  toolbar_  { nullptr };
  Status*   status_   { nullptr };

  bool shadowMapDebug_ { false };
};

}

#endif
