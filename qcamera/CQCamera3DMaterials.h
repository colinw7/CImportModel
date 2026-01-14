#ifndef CQCamera3DMaterials_H
#define CQCamera3DMaterials_H

#include <CQCamera3DWidget.h>
#include <CQCamera3DFaceData.h>

class CQCamera3DShaderProgram;
class CQCamera3DCamera;
class CQGLBuffer;

class CQCamera3DMaterials : public CQCamera3DWidget {
  Q_OBJECT

 public:
  CQCamera3DMaterials(CQCamera3DApp *app);

  //---

  int pixelWidth() const { return pixelWidth_; }
  void setPixelWidth(int i) { pixelWidth_ = i; }

  int pixelHeight() const { return pixelHeight_; }
  void setPixelHeight(int i) { pixelHeight_ = i; }

  double aspect() const { return aspect_; }
  void setAspect(double r) { aspect_ = r; }

  //---

  void initializeGL() override;

  void resizeGL(int, int) override;

  void paintGL() override;

  //---

  void mousePressEvent  (QMouseEvent *e) override;
  void mouseMoveEvent   (QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;

  void wheelEvent(QWheelEvent *) override;

  void keyPressEvent(QKeyEvent *event) override;

 private Q_SLOTS:
  void invalidateSlot();

 private:
  void addGeometry();
  void drawGeometry();

  CQGLBuffer *initBuffer();

 private:
  CQCamera3DCamera* camera_ { nullptr };

  CQCamera3DShaderProgram *shaderProgram_ { nullptr };

  CQGLBuffer *buffer_ { nullptr };

  // state
  bool valid_ { false };

  CQCamera3DFaceDataList faceDataList_;
};

#endif
