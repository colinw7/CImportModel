#ifndef CQCamera3DLight_H
#define CQCamera3DLight_H

#include <CGeomLight3D.h>

class CQCamera3DLight : public CGeomLight3D {
 public:
  CQCamera3DLight(CGeomScene3D *scene, const std::string &name="");
};

#endif
