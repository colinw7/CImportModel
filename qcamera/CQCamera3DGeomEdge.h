#ifndef CQCamera3DGeomEdge_H
#define CQCamera3DGeomEdge_H

#include <CGeomEdge3D.h>
#include <CGeomEdge3D.h>

class CQCamera3DGeomEdge : public CGeomEdge3D {
 public:
  CQCamera3DGeomEdge();

  CQCamera3DGeomEdge(const CQCamera3DGeomEdge &line);

  CQCamera3DGeomEdge &operator=(const CQCamera3DGeomEdge &rhs) = delete;
};

#endif
