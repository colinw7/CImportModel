#ifndef Light_H
#define Light_H

#include <CGeomLight3D.h>
#include <CPoint3D.h>
#include <CVector3D.h>
#include <CRGBA.h>

#include <QObject>

namespace CQTextureGen {

class Light : public QObject {
  Q_OBJECT

 public:
  using Type = CGeomLight3DType;

 public:
  Light();

 ~Light() override;

  //---

  int id() const { return id_; }
  void setId(int i) { id_ = i; }

  const std::string &name() const { return name_; }
  void setName(const std::string &s) { name_ = s; }

  //---

  // enabled
  bool isEnabled() const { return enabled_; }
  virtual void setEnabled(bool enabled) { enabled_ = enabled; Q_EMIT stateChanged(); }

  //---

  // type
  const Type &type() const { return type_; }
  void setType(const Type &t) { type_ = t; Q_EMIT stateChanged(); }

  // color
  const CRGBA &color() const { return color_; }
  void setColor(const CRGBA &v);

  //---

  // position
  const CPoint3D &position() const { return position_; }
  void setPosition(const CPoint3D &v);

  // direction
  const CVector3D &direction() const { return direction_; }
  void setDirection(const CVector3D &v);

  //---

  // point light

  double getPointRadius() const { return pointData_.radius; }
  void setPointRadius(double r) { pointData_.radius = r; Q_EMIT stateChanged(); }

  //---

  // spot light

  const CVector3D &getSpotDirection() const { return spotData_.direction; }
  void setSpotDirection(const CVector3D &dir) {
    spotData_.direction = dir; Q_EMIT stateChanged(); }

  double getSpotExponent() const { return spotData_.exponent; }
  void setSpotExponent(double exponent) {
    spotData_.exponent = exponent; Q_EMIT stateChanged(); }

  double getSpotCutOffAngle() const { return spotData_.cutoffAngle; }
  void setSpotCutOffAngle(double a) {
    spotData_.cutoffAngle = a; Q_EMIT stateChanged(); }

  //---

  double getConstantAttenuation() const { return attenuation_.constant; }
  void setConstantAttenuation(double attenuation) {
    attenuation_.constant = attenuation; Q_EMIT stateChanged(); }

  double getLinearAttenuation() const { return attenuation_.linear; }
  void setLinearAttenuation(double attenuation) {
    attenuation_.linear = attenuation; Q_EMIT stateChanged(); }

  double getQuadraticAttenuation() const { return attenuation_.quadratic; }
  void setQuadraticAttenuation(double attenuation) {
    attenuation_.quadratic = attenuation; Q_EMIT stateChanged(); }

 Q_SIGNALS:
  void stateChanged();

 public:
  struct PointData {
    double radius { 100.0 };
  };

  struct SpotData {
    CVector3D direction   { 0.0, 0.0, -1.0 };
    double    exponent    { 1.0 };
    double    cutoffAngle { 180.0 };
  };

  struct Attenuation {
    double constant  { 1.0 };
    double linear    { 0.0 };
    double quadratic { 0.0 };

    Attenuation() { }
  };

  int         id_ { -1 };
  std::string name_;

  bool        enabled_   { true };
  CRGBA       color_     { CRGBA::white() };
  Type        type_      { Type::POINT };
  CPoint3D    position_  { 1, 1, 1 };
  CVector3D   direction_ { -1, -1, -1 };
  PointData   pointData_;
  SpotData    spotData_;
  Attenuation attenuation_;
};

}

#endif
