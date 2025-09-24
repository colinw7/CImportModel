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

class CQNewGLFractal : public QObject, public CQNewGLObject {
  Q_OBJECT

 public:
  enum class Type {
    LORENZ
  };

 public:
  static void initShader(CQNewGLCanvas *canvas);

  //---

  CQNewGLFractal(CQNewGLCanvas *canvas);

  //---

  bool isActive() const { return active_; }
  void setActive(bool b);

  const Type &type() const { return type_; }
  void setType(const Type &t);

  double pointSize() const { return pointSize_; }
  void setPointSize(double r) { pointSize_ = r; }

  //---

  void initBuffer() override;

  void addGeometry();

  void drawGeometry();

  CQNewGLShaderProgram *shaderProgram() override { return shaderProgram_; }

 private:
  static CQNewGLShaderProgram* shaderProgram_;

  bool active_ { false };

  Type type_ { Type::LORENZ };

  double pointSize_ { 1.0 };

  CQNewGLFractalParticleSystem *particleSystem_ { nullptr };

  CQGLTexture* texture_ { nullptr };

  bool updateGeometry_ { true };

  CQNewGLLorenzCalc* lorenz_ { nullptr };
};

#endif
