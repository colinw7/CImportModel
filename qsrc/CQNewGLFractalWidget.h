#ifndef CQNewGLFractalWidget_H
#define CQNewGLFractalWidget_H

#include <CQNewGLShaderProgram.h>
#include <CQNewGLFaceData.h>
#include <CQNewGLWidget.h>

#include <CGLVector3D.h>
#include <CGLCamera.h>
#include <CQGLBuffer.h>

class CQNewGLModel;
class CQNewGLCamera;
class CQNewGLAxes;
class CQNewGLFractal;

//---

class CQNewGLFractalWidget : public CQNewGLWidget {
  Q_OBJECT

 public:
  enum class BonesTransform {
    INVERSE_BIND,
    LOCAL,
    GLOBAL
  };

 public:
  CQNewGLFractalWidget(CQNewGLModel *app);

  //---

  CQNewGLFractal *fractal() const { return fractal_; }

  CQNewGLCamera *getCurrentCamera() const override { return camera_; }

  //---

  // axes
  CQNewGLAxes *getAxes() const { return axes_; }

  //---

  const CGLMatrix3D &projectionMatrix() const override { return paintData_.projection; }
  const CGLMatrix3D &viewMatrix() const override { return paintData_.view; }

  const CGLVector3D &viewPos() const { return paintData_.viewPos; }

  CMatrix3D getModelMatrix() const;

  CPoint3D  sceneCenter() const override { return sceneCenter_; }
  CVector3D sceneSize  () const override { return sceneSize_; }
  double    sceneScale () const override { return sceneScale_; }

  //---

  void initializeGL() override;

  void paintGL() override;

  void resizeGL(int, int) override;

  //---

  void addCamera();
  void resetCamera();

  //---

  void mousePressEvent  (QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent   (QMouseEvent *event) override;

  void wheelEvent(QWheelEvent *) override;

  void keyPressEvent(QKeyEvent *event) override;

  void setMousePos(double x, double y, bool add, bool show);

  //---

  void updateFractal();

  void drawFractal();

  //---

  // axes
  void updateAxes();
  void drawAxes();

  void updateCameraBuffer();

 private:
  struct PaintData {
    CGLVector3D viewPos;
    CGLMatrix3D projection;
    CGLMatrix3D view;
  };

  bool initialized_ { false };

  CQNewGLFractal* fractal_ { nullptr };

  // scene
  CVector3D sceneSize_   { 1.0, 1.0, 1.0 };
  double    sceneScale_  { 1.0 };
  CPoint3D  sceneCenter_ { 0, 0, 0 };

  PaintData paintData_;

  int pixelWidth_  { 0 };
  int pixelHeight_ { 0 };

  double aspect_ { 1.0 };

  // camera
  CQNewGLCamera* camera_;

  // axes
  CQNewGLAxes* axes_ { nullptr };

  //---

  // mouse
  struct MouseData {
    double          pressX { 0.0 };
    double          pressY { 0.0 };
    double          moveX  { 0.0 };
    double          moveY  { 0.0 };
    Qt::MouseButton button { Qt::NoButton };
  };

  MouseData mouseData_;
};

#endif
