#include <CQCamera3DGeomLine.h>

CQCamera3DGeomLine::
CQCamera3DGeomLine()
{
}

CQCamera3DGeomLine::
CQCamera3DGeomLine(const CQCamera3DGeomLine &line) :
 CGeomLine3D(line)
{
}

CQCamera3DGeomLine *
CQCamera3DGeomLine::
dup() const
{
  return new CQCamera3DGeomLine(*this);
}
