#ifndef Canvas_H
#define Canvas_H

#include <FaceData.h>

#include <CImagePtr.h>
#include <CBBox3D.h>
#include <CPoint2D.h>

#include <QGLWidget>
#include <QOpenGLExtraFunctions>

#include <optional>

class CQGLBuffer;
class CQGLTexture;
class CGeomTexture;

namespace CQModel3DView {

class App;
class Camera;
class FaceData;
class ShaderProgram;
class Texture;

class Canvas : public QGLWidget, public QOpenGLExtraFunctions {
  Q_OBJECT

 public:
  Canvas(App *app);

  //---

  App *app() const { return app_; }

  int ind() const { return ind_; }
  void setInd(int i) { ind_ = i; }

  Camera *camera() const { return camera_; }

  ShaderProgram *shaderProgram();

  //---

  int pixelWidth() const { return pixelWidth_; }
  void setPixelWidth(int i) { pixelWidth_ = i; }

  int pixelHeight() const { return pixelHeight_; }
  void setPixelHeight(int i) { pixelHeight_ = i; }

  double aspect() const { return aspect_; }
  void setAspect(double r) { aspect_ = r; }

  //---

  const QColor &bgColor() const { return bgColor_; }
  void setBgColor(const QColor &c) { bgColor_ = c; }

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

  bool isTextured() const { return textured_; }
  void setTextured(bool b) { textured_ = b; }

  //---

  void initializeGL() override;

  void resizeGL(int, int) override;

  void paintGL() override;

  //---

  void mousePressEvent  (QMouseEvent *e) override;
  void mouseMoveEvent   (QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;

  void wheelEvent(QWheelEvent *e) override;

  void keyPressEvent(QKeyEvent *event) override;

  //---

  QSize sizeHint() const override { return QSize(1536, 1536); }

  //---

  void addScene ();
  void drawScene();

 private:
  ShaderProgram *getShader(const QString &vertex, const QString &fragment);

  void enableDepthTest();
  void enableCullFace();
  void enableFrontFace();
  void enablePolygonLine();

  CQGLTexture *getGLTexture(CGeomTexture *texture, bool /*add*/);

  void initGLTexture(Texture *texture);

  CQGLTexture *makeTexture(const CImagePtr &image) const;

 Q_SIGNALS:
  void textureAdded();

 public Q_SLOTS:
  void updateScene();

 private:
  struct MouseData {
    bool            pressed   { false };
    bool            isShift   { false };
    bool            isControl { false };
    Qt::MouseButton button    { Qt::NoButton };
    CPoint2D        press     { 0.0, 0.0 };
    CPoint2D        move      { 0.0, 0.0 };
  };

  using Shaders = std::map<QString, ShaderProgram *>;

  //---

  App* app_ { nullptr };

  int ind_ { 0 };

  Camera* camera_ { nullptr };

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

  // textures
  bool textured_ { true };

  // globals
  QColor bgColor_ { 140, 140, 140 };

  // interaction
  MouseData mouseData_;

  CBBox3D bbox_;

  Shaders shaders_;

  bool invalid_ { true };
};

}

#endif
