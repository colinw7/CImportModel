#include <CTurtle3D.h>

CTurtle3D::
CTurtle3D()
{
  reset();
}

CTurtle3D::
~CTurtle3D()
{
}

void
CTurtle3D::
reset()
{
  prevPoint_     = CPoint3D(0.0, 0.0, 0.0);
  prevDirection_ = CVector3D(0.0, 0.0, 0.0);

  point_     = prevPoint_;
  direction_ = prevDirection_;
}

void
CTurtle3D::
setPoint(const CPoint3D &p)
{
  prevPoint_ = point_;
  point_     = p;
}

void
CTurtle3D::
setDirection(const CVector3D &d)
{
  prevDirection_ = direction_;
  direction_     = d;
}

void
CTurtle3D::
step(double length)
{
  prevPoint_ = point_;

  point_ += length*direction_;
}
