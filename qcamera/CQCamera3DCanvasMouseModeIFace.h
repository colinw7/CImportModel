#ifndef CQCamera3DCanvasMouseModeIFace_H
#define CQCamera3DCanvasMouseModeIFace_H

#include <CQCamera3DMouseModeIFace.h>

class CQCamera3DCanvasMouseModeIFace : public CQCamera3DMouseModeIFace {
 public:
  CQCamera3DCanvasMouseModeIFace(CQCamera3DCanvas *canvas);

  const CPoint3D &cursor() const override;
  void setCursor(const CPoint3D &c) override;

  void moveCursorToMouse(const CPoint3D &c) override;

  CQCamera3DSelectData getSelection() const override;

  const CBBox3D &bbox() const override;

  const MoveDirection &moveDirection() const override;
  void setMoveDirection(const MoveDirection &d) override;

  void moveObject(CGeomObject3D *object, const CVector3D &d) override;
  void moveFace(CGeomFace3D *face, const CVector3D &d) override;

  void scaleObject(CGeomObject3D *object, const CVector3D &d) override;

  void rotateObject(CGeomObject3D *object, double a, const CVector3D &d) override;

  void extrudeMove(double d) override;

  void endUpdateObjects() override;

  void setOptions(CQCamera3DOptions *options) override;
  void resetOptions() override;

  void showOptions() override;
  void hideOptions() override;

 private:
  CQCamera3DCanvas *canvas_ { nullptr };
};

#endif
