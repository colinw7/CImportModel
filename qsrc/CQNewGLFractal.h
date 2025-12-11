#ifndef CQNewGLFractal_H
#define CQNewGLFractal_H

#include <CQNewGLObject.h>

#include <CImageMgr.h>
#include <CPoint3D.h>
#include <CVector3D.h>

#include <QObject>
#include <QImage>

class CQNewGLFractalParticleSystem;
class CQNewGLLorenzCalc;
class CQNewGLWidget;
class CQNewGLModel;

class CQNewGLFractal : public QObject, public CQNewGLObject {
  Q_OBJECT

 public:
  enum class Type {
    LORENZ
  };

 public:
  CQNewGLFractal(CQNewGLWidget *widget);

  //---

  bool isActive() const { return active_; }
  void setActive(bool b);

  const Type &type() const { return type_; }
  void setType(const Type &t);

  bool isTextured() const { return textured_; }
  void setTextured(bool b);

  double pointSize() const { return pointSize_; }
  void setPointSize(double r) { pointSize_ = r; }

  const CBBox3D &bbox() const { return bbox_; }
  void setBBox(const CBBox3D &v) { bbox_ = v; }

  //---

  CQGLBuffer *initBuffer() override;

  //---

  void updateGeometry() override { }

  void addGeometry();

  void drawGeometry() override;

  //---

  CQNewGLShaderProgram *shaderProgram() override;

 private:
  bool    active_         { false };
  bool    textured_       { false };
  bool    updateGeometry_ { true };
  Type    type_           { Type::LORENZ };
  double  pointSize_      { 1.0 };
  CBBox3D bbox_;

  CQNewGLFractalParticleSystem *particleSystem_ { nullptr };
  CQNewGLLorenzCalc*            lorenz_         { nullptr };
  CQGLTexture*                  texture_        { nullptr };
};

#endif
