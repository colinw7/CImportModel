#ifndef CQCamera3DShape_H
#define CQCamera3DShape_H

#include <CQCamera3DObject.h>
#include <CQCamera3DShapes.h>
#include <CPlane3D.h>

class CGLCameraIFace;
class CQCamera3DLight;

class CQCamera3DShape : public CQCamera3DObject {
 public:
  using ShapeData = CQCamera3DShapes::ShapeData;

 public:
  CQCamera3DShape(CQCamera3DCanvas *canvas);

  //---

  bool isWireframe() const { return wireframe_; }
  void setWireframe(bool b) { wireframe_ = b; }

  //---

  void updateGeometry();

  void drawGeometry();

  //---

  CQCamera3DShaderProgram *shaderProgram() override;

 private:
  void addCameras();
  void addCamera(CGLCameraIFace *camera);

  void addLights();
  void addLight(CQCamera3DLight *light);

  void addPlanes();

  void addEyeLine();
  void addEyeLine1();

  void addSphere(const CPoint3D &c, double r);

  void addCube(const CPoint3D &c, double r, const ShapeData &shapeData);

  void addCylinder(const CPoint3D &p1, const CPoint3D &p2, double r, const ShapeData &shapeData);

  CPlane3D addPlane(const CPoint3D &p1, const CPoint3D &p2, const CPoint3D &p3, const CPoint3D &p4,
                    const CVector3D &n, const CRGBA &c, bool draw=false);

  void addTriangle(const CPoint3D &p1, const CPoint3D &p2, const CPoint3D &p3,
                   const CVector3D &n, const ShapeData &shapeData);

 private:
  bool wireframe_ { true };

  CPlane3D xyPlane_;
  CPlane3D leftPlane_, rightPlane_;
  CPlane3D topPlane_, bottomPlane_;

  CQGLBuffer* buffer_ { nullptr };
};

#endif
