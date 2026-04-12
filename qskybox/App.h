#ifndef App_H
#define App_H

#include <CGeom3DType.h>

#include <QFrame>

class CGeomScene3D;

namespace CQSkybox3D {

class Canvas;
class Overview;
class Control;
class Toolbar;

class App : public QFrame {
  Q_OBJECT

 public:
  App();

  //---

  Canvas *canvas() const { return canvas_; }

  Overview* overview() const { return overview_; }

  Control *control() const { return control_; }

  //---

  const QString &buildDir() const { return buildDir_; }
  void setBuildDir(const QString &s) { buildDir_ = s; }

  CGeomScene3D *scene() const { return scene_; }

  bool isReflectionMap() const { return reflectionMap_; }
  void setReflectionMap(bool b) { reflectionMap_ = b; }

  bool isRefractionMap() const { return refractionMap_; }
  void setRefractionMap(bool b) { refractionMap_ = b; }

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

  bool reflectionMap_ { false };
  bool refractionMap_ { false };
};

}

#endif
