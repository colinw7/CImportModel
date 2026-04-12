#ifndef Light_H
#define Light_H

#include <CGeomLight3D.h>
#include <Camera.h>

#include <QObject>

namespace CQSkybox3D {

class App;

class Light : public Camera, public CGeomLight3D {
  Q_OBJECT

 public:
  Light(App *app, const std::string &name="");

  void setPosition(const CPoint3D &point) override {
    Camera::setPosition(CVector3D(point));
    CGeomLight3D::setPosition(point);
  }

  const CVector3D &getDirection() const override {
    static CVector3D dir;
    dir = Camera::front();
    return dir;
  }
};

}

#endif
