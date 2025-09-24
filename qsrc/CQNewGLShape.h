#ifndef CQNewGLShape_H
#define CQNewGLShape_H

#include <CQNewGLObject.h>

#include <CPoint3D.h>

#include <QObject>
#include <QImage>

class CQNewGLShape : public QObject, public CQNewGLObject {
  Q_OBJECT

 public:
  enum class Type {
    NONE,
    BOX,
    CONE,
    CUBE,
    CYLINDER,
    HYPERBOLOID,
    PYRAMID,
    SPHERE,
    TORUS
  };

 public:
  static void initShader(CQNewGLCanvas *canvas);

  //---

  CQNewGLShape(CQNewGLCanvas *canvas);

  //---

  bool isActive() const { return active_; }
  void setActive(bool b);

  const Type &type() const { return type_; }
  void setType(const Type &v) { type_ = v; }

  const CPoint3D &start() const { return start_; }
  void setStart(const CPoint3D &p) { start_ = p; }

  const CPoint3D &end() const { return end_; }
  void setEnd(const CPoint3D &p) { end_ = p; }

  double width() const { return width_; }
  void setWidth(double r) { width_ = r; }

  const CRGBA &color() const { return color_; }
  void setColor(const CRGBA &v) { color_ = v; }

  const CQGLTexture *texture() const { return texture_; }
  void setTexture(CQGLTexture *t) { texture_ = t; }

  void invalidateGeometry() { updateGeometry_ = true; }

  //---

  void addGeometry();

  void drawGeometry();

  CQNewGLShaderProgram *shaderProgram() override { return shaderProgram_; }

 private:
  static CQNewGLShaderProgram* shaderProgram_;

  bool active_ { false };
  Type type_   { Type::SPHERE };

  CPoint3D start_ { 0.0, 0.0, 0.0 };
  CPoint3D end_   { 1.0, 1.0, 1.0 };
  double   width_ { 1.0 };
  CRGBA    color_ { 0.4, 0.4, 0.9 };

  CQGLTexture *texture_  { nullptr };

  bool updateGeometry_ { true };
};

#endif
