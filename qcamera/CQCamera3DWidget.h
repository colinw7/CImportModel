#ifndef CQCamera3DWidget_H
#define CQCamera3DWidget_H

#include <QGLWidget>
#include <QOpenGLExtraFunctions>

#include <CImagePtr.h>
#include <CPoint2D.h>

class CQCamera3DApp;
class CQCamera3DTexture;
class CQCamera3DShaderProgram;

class CQGLTexture;
class CGeomTexture;

class CQCamera3DWidget : public QGLWidget, public QOpenGLExtraFunctions {
  Q_OBJECT

 public:
  CQCamera3DWidget(CQCamera3DApp *app);
 ~CQCamera3DWidget() override;

  //---

  CQCamera3DApp *app() const { return app_; }

  uint ind() const { return ind_; }

  //---

  int pixelWidth() const { return pixelWidth_; }
  void setPixelWidth(int i) { pixelWidth_ = i; }

  int pixelHeight() const { return pixelHeight_; }
  void setPixelHeight(int i) { pixelHeight_ = i; }

  double aspect() const { return aspect_; }
  void setAspect(double r) { aspect_ = r; }

  //---

  bool isDepthTest() const { return depthTest_; }
  void setDepthTest(bool b) { depthTest_ = b; }

  bool isCullFace() const { return cullFace_; }
  void setCullFace(bool b) { cullFace_ = b; }

  bool isFrontFace() const { return frontFace_; }
  void setFrontFace(bool b) { frontFace_ = b; }

  bool isPolygonLine() const { return polygonLine_; }
  void setPolygonLine(bool b) { polygonLine_ = b; }

  double pointSize() const { return pointSize_; }
  void setPointSize(double r) { pointSize_ = r; update(); }

  double lineWidth() const { return lineWidth_; }
  void setLineWidth(double r) { lineWidth_ = r; update(); }

  //---

  void enableDepthTest();
  void enableCullFace();
  void enableFrontFace();
  void enablePolygonLine();

  //---

  const QColor &bgColor() const { return bgColor_; }
  void setBgColor(const QColor &c) { bgColor_ = c; }

  //---

  virtual CQCamera3DShaderProgram *shaderProgram() = 0;

  //---

  CQGLTexture *getGLTexture(CGeomTexture *texture, bool add);

  void initGLTexture(CQCamera3DTexture *texture);

 protected:
  CQGLTexture *makeTexture(const CImagePtr &image) const;

 Q_SIGNALS:
  void textureAdded();

 protected:
  struct MouseData {
    bool            pressed   { false };
    bool            isShift   { false };
    bool            isControl { false };
    Qt::MouseButton button    { Qt::NoButton };
    CPoint2D        press     { 0.0, 0.0 };
    CPoint2D        move      { 0.0, 0.0 };
  };

  //---

  CQCamera3DApp* app_ { nullptr };
  uint           ind_ { 0 };

  // state
  int pixelWidth_  { 2000 };
  int pixelHeight_ { 1500 };

  double aspect_ { 1.0 };

  // Open GL globals

  bool depthTest_   { true };
  bool cullFace_    { true };
  bool frontFace_   { false };
  bool polygonLine_ { false };

  double pointSize_ { 4.0 };
  double lineWidth_ { 2.0 };

  // globals
  QColor bgColor_ { 140, 140, 140 };

  // interaction
  MouseData mouseData_;
};

#endif
