#ifndef CQNewGLEmitter_H
#define CQNewGLEmitter_H

#include <CQNewGLObject.h>

#include <CImageMgr.h>
#include <CPoint3D.h>
#include <CVector3D.h>

#include <QObject>
#include <QImage>

class CQNewGLEmitterParticleSystem;
class CQNewGLCanvas;
class CQNewGLModel;

class CFlocking;
class CFireworks;

class CQNewGLEmitter : public QObject, public CQNewGLObject {
  Q_OBJECT

 public:
  enum class Type {
    GENERATOR,
    FLOCKING,
    FIREWORKS
  };

 public:
  CQNewGLEmitter(CQNewGLCanvas *canvas);

  //---

  const Type &type() const { return type_; }
  void setType(const Type &t);

  const CPoint3D &position() const { return position_; }
  void setPosition(const CPoint3D &p);

  const CVector3D &minVelocity() const { return minVelocity_; }
  void setMinVelocity(const CVector3D &v);

  const CVector3D &maxVelocity() const { return maxVelocity_; }
  void setMaxVelocity(const CVector3D &v);

  const CRGBA &startColor() const { return startColor_; }
  void setStartColor(const CRGBA &v) { startColor_ = v; }

  const CRGBA &endColor() const { return endColor_; }
  void setEndColor(const CRGBA &v) { endColor_ = v; }

  int emitInterval() const { return emitInterval_; }
  void setEmitInterval(int i);

  int maxParticles() const { return maxParticles_; }
  void setMaxParticles(int i) { maxParticles_ = i; }

  int maxAge() const { return maxAge_; }
  void setMaxAge(int i) { maxAge_ = i; }

  double pointSize() const { return pointSize_; }
  void setPointSize(double r) { pointSize_ = r; }

  double step() const { return step_; }
  void setStep(double r) { step_ = r; }

  double gravity() const { return gravity_; }
  void setGravity(double r) { gravity_ = r; }

  double mass() const { return mass_; }
  void setMass(double r) { mass_ = r; }

  bool isEnabled() const { return enabled_; }
  void setEnabled(bool b) { enabled_ = b; }

  bool isRunning() const { return running_; }
  void setRunning(bool b) { running_ = b; }

  const QString &imageName() const { return imageName_; }
  void setImageName(const QString &s);

  //---

  void updateGeometry() override;

  void drawGeometry() override;

  //---

  CQNewGLShaderProgram *shaderProgram() override;

 private Q_SLOTS:
  void stepSlot();

 private:
  void updateTexture();

 private:
  CQNewGLCanvas* canvas_ { nullptr };

  Type type_ { Type::GENERATOR };

  CPoint3D position_ { 0.0, 0.0, 0.0 };

  CVector3D minVelocity_ { -1.0, -1.0, -1.0 };
  CVector3D maxVelocity_ {  1.0,  1.0,  1.0 };

  CRGBA startColor_ { 0.0, 1.0, 0.0 };
  CRGBA endColor_   { 1.0, 1.0, 1.0 };

  double gravity_ { 0.0 };

  double step_ { 0.01 };
  double mass_ { 1.0 };

  double pointSize_ { 1.0 };

  bool enabled_ { false };
  bool running_ { true };
  uint steps_   { 0 };

  int emitInterval_ { 10 };

  int maxParticles_ { 100 };
  int maxAge_       { 10 };

  QString imageName_ { "particle.png" };

  CQNewGLEmitterParticleSystem *particleSystem_ { nullptr };

  CQGLTexture* texture_ { nullptr };

  bool updateGeometry_ { true };

  CFlocking*  flocking_  { nullptr };
  CFireworks* fireworks_ { nullptr };
};

#endif
