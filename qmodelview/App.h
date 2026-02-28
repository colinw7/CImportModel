#ifndef App_H
#define App_H

#include <CGeom3DType.h>

#include <QFrame>

class CGeomScene3D;

namespace CQModel3DView {

class Canvas;
class Overview;
class Control;

class App : public QFrame {
  Q_OBJECT

 public:
  App();

  //---

  Canvas *canvas() const { return canvas_; }

  Control *control() const { return control_; }

  //---

  const QString &buildDir() const { return buildDir_; }
  void setBuildDir(const QString &s) { buildDir_ = s; }

  CGeomScene3D *scene() const { return scene_; }

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
};

}

#endif
