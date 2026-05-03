#include <CQCamera3DGeomFace.h>

CQCamera3DGeomFace::
CQCamera3DGeomFace()
{
}

CQCamera3DGeomFace::
CQCamera3DGeomFace(const CQCamera3DGeomFace &face) :
 CGeomFace3D(face)
{
}

CQCamera3DGeomFace *
CQCamera3DGeomFace::
dup() const
{
  return new CQCamera3DGeomFace(*this);
}
