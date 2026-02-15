#include <Light.h>

namespace CQTextureGen {

Light::
Light()
{
}

Light::
~Light()
{
}

void
Light::
setPosition(const CPoint3D &p)
{
  position_ = p;

  Q_EMIT stateChanged();
}

void
Light::
setDirection(const CVector3D &p)
{
  direction_ = p;

  Q_EMIT stateChanged();
}

void
Light::
setColor(const CRGBA &c)
{
  color_ = c;

  Q_EMIT stateChanged();
}

}
