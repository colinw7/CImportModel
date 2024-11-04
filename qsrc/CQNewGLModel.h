#ifndef CQNewGLModel_H
#define CQNewGLModel_H

#include <QFrame>

class CQNewGLCanvas;
class CQNewGLControl;

class CQNewGLModel : public QFrame {
  Q_OBJECT

 public:
  enum class Type {
    CAMERA = 0,
    MODEL  = 1,
    LIGHT  = 2
  };

 public:
  CQNewGLModel();

  CQNewGLCanvas *canvas() const { return canvas_; }

  int windowWidth() const { return windowWidth_; }
  void setWindowWidth(int i) { windowWidth_ = i; }

  int windowHeight() const { return windowHeight_; }
  void setWindowHeight(int i) { windowHeight_ = i; }

  double ambient() const { return ambient_; }
  void setAmbient(double r) { ambient_ = r; }

  double diffuse() const { return diffuse_; }
  void setDiffuse(double r) { diffuse_ = r; }

  double specular() const { return specular_; }
  void setSpecular(double r) { specular_ = r; }

  double shininess() const { return shininess_; }
  void setShininess(double r) { shininess_ = r; }

  const Type &type() const { return type_; }
  void setType(const Type &t) { type_ = t; }

 private:
  CQNewGLCanvas*  canvas_  { nullptr };
  CQNewGLControl* control_ { nullptr };

  int windowWidth_  { 1024 };
  int windowHeight_ { 1024 };

  double ambient_   { 0.5 };
  double diffuse_   { 0.5 };
  double specular_  { 1.0 };
  double shininess_ { 32.0 };

  Type type_ { Type::CAMERA };
};

#endif
