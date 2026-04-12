#include <Light.h>
#include <App.h>

namespace CQSkybox3D {

Light::
Light(App *app, const std::string &name) :
 Camera(app), CGeomLight3D(nullptr, name)
{
  //setPerspective(false);

  //setType(CGeomLight3DType::DIRECTIONAL);
  setType(CGeomLight3DType::POINT);
}

}
