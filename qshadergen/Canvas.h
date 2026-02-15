#ifndef Canvas_H
#define Canvas_H

#include <vec3.h>

#include <QFrame>

namespace CQShaderGen {

class App;
class Camera;

class Canvas : public QFrame {
  Q_OBJECT

 public:
  enum class Type2D {
    None,
    Circle,
    Heart,
    Cross
  };

  enum class Type3D {
    None,
    Sphere,
    Box,
    RoundBox,
    BoxFrame,
    Torus,
    CappedTorus,
    Link,
    Cylinder,
    Cone,
    Plane,
    HexPrism,
    Capsule,
    RotateCross,
    ExtrudeCross,
    ElongateEllipsoid,
    ElongateTorus,
    OnionSphere
  };

 public:
  Canvas(App *app);

  bool is3D() const { return is3D_; }
  void set3D(bool b) { is3D_ = b; }

  const Type2D &type2D() const { return type2D_; }
  void setType2D(const Type2D &v) { type2D_ = v; }

  const Type3D &type3D() const { return type3D_; }
  void setType3D(const Type3D &v) { type3D_ = v; }

  const vec3 &cameraPos() const { return cameraPos_; }
  void setCameraPos(const vec3 &v) { cameraPos_ = v; }

  const vec3 &lightPos() const { return lightPos_; }
  void setLightPos(const vec3 &v) { lightPos_ = v; }

  void paintEvent(QPaintEvent *) override;

  void wheelEvent(QWheelEvent *e) override;

  QSize sizeHint() const override { return QSize(512, 512); }

 private:
  void draw2D();
  void draw3D(double a);

 private Q_SLOTS:
  void updateSlot();

 private:
  App* app_ { nullptr };

  Camera* camera_ { nullptr };

  bool   is3D_   { true };
  Type2D type2D_ { Type2D::Circle };
  Type3D type3D_ { Type3D::Sphere };

  double a_  { 0.0 };
  double da_ { 0.1 };

  double cameraZ_ { 1.5 };

  vec3 cameraPos_ { 0.0, 0.0, 1.5 };
  vec3 lightPos_  { 0.6, 0.2, 0.4 };
};

}

#endif
