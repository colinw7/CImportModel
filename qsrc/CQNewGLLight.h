#ifndef CQNewGLLight_H
#define CQNewGLLight_H

#include <CQNewGLObject.h>
#include <CGLVector3D.h>

#include <QColor>

class CQNewGLShaderProgram;

class CQNewGLLight : public CQNewGLObject {
 public:
  enum class Type {
    DIRECTIONAL,
    POINT,
    SPOT
  };

 public:
  static void initShader(CQNewGLCanvas *canvas);

  //---

  CQNewGLLight(CQNewGLCanvas *canvas);

  //---

  const Type &type() const { return type_; }
  void setType(const Type &v) { type_ = v; }

  bool isEnabled() const { return enabled_; }
  void setEnabled(bool b) { enabled_ = b; }

  const CGLVector3D &position() const { return position_; }
  void setPosition(const CGLVector3D &p) { position_ = p; }

  const CGLVector3D &direction() const { return direction_; }
  void setDirection(const CGLVector3D &p) { direction_ = p; }

  const QColor &color() const { return color_; }
  void setColor(const QColor &c) { color_ = c; }

  double radius() const { return radius_; }
  void setRadius(double r) { radius_ = r; }

  double cutoff() const { return cutoff_; }
  void setCutoff(double r) { cutoff_ = r; }

  //---

  void flipYZ() { position_.flipYZ(); }

  //---

  void updateGeometry() override { }

  void addGeometry();

  void drawGeometry() override;

  //---

  void setShaderData(CQNewGLShaderProgram *program, const QString &lightName) const;

  CQNewGLShaderProgram *shaderProgram() override { return shaderProgram_; }

 private:
  static CQNewGLShaderProgram* shaderProgram_;

  Type        type_      { Type::POINT };
  bool        enabled_   { true };
  CGLVector3D position_  { 0.4f, 0.4f, 0.4f };
  CGLVector3D direction_ { 0.0f, 0.0f, 1.0f };
  QColor      color_     { 255, 255, 255 };
  double      radius_    { 100.0 };
  double      cutoff_    { 0.1 };
};

#endif
