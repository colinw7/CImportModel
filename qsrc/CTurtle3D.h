#ifndef CTURTLE_3D_H
#define CTURTLE_3D_H

#include <CPoint3D.h>
#include <CVector3D.h>

class CTurtle3D {
 public:
  CTurtle3D();

 ~CTurtle3D();

  const CPoint3D &prevPoint() const { return prevPoint_; }

  const CPoint3D &point() const { return point_; }
  void setPoint(const CPoint3D &p);

  const CVector3D &direction() const { return direction_; }
  void setDirection(const CVector3D &p);

  void reset();

  void step(double length);

 private:
  CPoint3D  prevPoint_;
  CVector3D prevDirection_;
  CPoint3D  point_;
  CVector3D direction_;
};

#endif
