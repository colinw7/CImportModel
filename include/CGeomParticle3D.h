#ifndef CGEOM_PARTICLE_3D_H
#define CGEOM_PARTICLE_3D_H

#include <CPoint3D.h>
#include <CMatrix3D.h>
#include <CRGBA.h>
#include <CClipSide.h>
#include <COptVal.h>
#include <CGeomPoint3D.h>

class CGeomObject3D;
class CGeomZBuffer;
class CGeomCamera3D;
class CGeomFace3D;

class CGeomParticle3D : public CGeomPoint3D {
 protected:
  CRGBA     color_;
  CVector3D normal_;
  int       size_;

 public:
  CGeomParticle3D(const CPoint3D &point=CPoint3D(0,0,0)) :
   CGeomPoint3D(point), color_(0,0,0,1), normal_(), size_(1) {
  }

  CGeomParticle3D(const CGeomParticle3D &particle);

  virtual ~CGeomParticle3D() { }

  ACCESSOR(Color , CRGBA    , color )
  ACCESSOR(Normal, CVector3D, normal)
  ACCESSOR(Size  , int      , size  )

  void draw(CGeomZBuffer *zbuffer);

  void print(std::ostream &os) const {
    CGeomPoint3D::print(os);

    os << ", color=" << color_ <<
          ", size="  << size_;
  }

  friend std::ostream &operator<<(std::ostream &os, const CGeomParticle3D &particle) {
    particle.print(os);

   return os;
  }
};

#endif
