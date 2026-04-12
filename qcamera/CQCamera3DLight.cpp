#include <CQCamera3DLight.h>

CQCamera3DLight::
CQCamera3DLight(CGeomScene3D *pscene, const std::string &name) :
 CGeomLight3D(pscene, name)
{
  //setPerspective(false);

  //CGLCameraIFace::setPosition(CVector3D(CGeomLight3D::getPosition()));
}

#if 0
void
CQCamera3DLight::
calcVectors()
{
  static CVector3D defFront { 0, 0, 1 };
  static CVector3D defUp    { 0, 1, 0 };
  static CVector3D defRight { 1, 0, 0 };

  // TODO: FLASHLIGHT use eye direction
  if      (getType() == CGeomLight3DType::DIRECTIONAL)
    front_ = getDirection();
  else if (getType() == CGeomLight3DType::SPOT)
    front_ = getSpotDirection();
  else
    front_ = CVector3D(CVector3D(getPosition()), origin()).normalized();

  right_ = front_.crossProduct(defUp);

  if (right_.length() < 0.01)
    right_ = front_.crossProduct(defRight);

  if (right_.length() < 0.01)
    right_ = front_.crossProduct(defFront);

  up_ = front_.crossProduct(right_);
}
#endif
