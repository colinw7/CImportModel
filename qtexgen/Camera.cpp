#include <Camera.h>

namespace CQTextureGen {

Camera::
Camera()
{
}

Camera::
~Camera()
{
}

//---

// get perspective matrix
CMatrix3DH
Camera::
perspectiveMatrix() const
{
  CMatrix3DH matrix;

  double near = this->near();
  double far  = this->far();

  double tf2  = std::tan(0.5*degToRad(fov_));
  double itf2 = 1.0/tf2;

  double fmn  = far - near;
  double fpn  = far + near;
  double ftn  = far * near;
  double ifmn = 1.0/fmn;

  matrix =
    CMatrix3DH(itf2/aspect_, 0.0 ,       0.0,         0.0,
               0.0         , itf2,       0.0,         0.0,
               0.0         , 0.0 , -fpn*ifmn, -2*ftn*ifmn,
               0.0         , 0.0 ,      -1.0,         0.0);

  return matrix;
}

CMatrix3DH
Camera::
orthoMatrix() const
{
  return CMatrix3DH::identity();
}

// get view matrix
CMatrix3DH
Camera::
viewMatrix() const
{
  CMatrix3DH matrix;

  auto eye    = this->position();
  auto center = this->origin();

  auto dir = CVector3D(eye, center).normalized();
  auto up  = this->up().normalized();

  auto right = dir.crossProduct(up);
  auto newUp = right.crossProduct(dir);

  auto ex = right.dotProduct(eye);
  auto ey = newUp.dotProduct(eye);
  auto ez = dir  .dotProduct(eye);

  matrix.setTranslation(-ex, -ey, ez);

  matrix.setColumn(0, right);
  matrix.setColumn(1, newUp);
  matrix.setColumn(2, -dir );

  return matrix;
}

CVector3D
Camera::
front() const
{
  auto eye    = this->position();
  auto center = this->origin();

  return CVector3D(eye, center).normalized();
}

CVector3D
Camera::
up() const
{
  return CVector3D(0.0, 1.0,  0.0);
}

CVector3D
Camera::
right() const
{
  auto dir = this->front();
  auto up  = this->up();

  return dir.crossProduct(up);
}

}
