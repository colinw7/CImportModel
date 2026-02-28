#ifndef Camera_H
#define Camera_H

#include <CGLCameraIFace.h>

#include <QObject>

namespace CQTextureGen {

class Camera : public QObject, public CGLCameraIFace {
  Q_OBJECT

 public:
  Camera();

 ~Camera() override;

  //---

  // get perspective matrix
  CMatrix3DH perspectiveMatrix() const override;

  CMatrix3DH orthoMatrix() const override;

  // get view matrix
  CMatrix3DH viewMatrix() const override;

  //---

  CVector3D front() const override;
  CVector3D up   () const override;
  CVector3D right() const override;

  void moveAroundX(double) override { }
  void moveAroundY(double) override { }
  void moveAroundZ(double) override { }

 private:
  void stateChanged() override { Q_EMIT stateChangedSignal(); }

 Q_SIGNALS:
  // send when state (data values) changed
  void stateChangedSignal();
};

}

#endif
