#ifndef CQCamera3DGeomLine_H
#define CQCamera3DGeomLine_H

#include <CGeomLine3D.h>
#include <CGeomLine3D.h>

class CQCamera3DGeomLine : public CGeomLine3D {
 public:
  CQCamera3DGeomLine();

  CQCamera3DGeomLine(const CQCamera3DGeomLine &line);

  CQCamera3DGeomLine &operator=(const CQCamera3DGeomLine &rhs) = delete;

  CQCamera3DGeomLine *dup() const override;
};

#endif
