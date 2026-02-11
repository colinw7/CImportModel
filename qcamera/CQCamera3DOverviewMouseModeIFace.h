#ifndef CQCamera3DOverviewMouseModeIFace_H
#define CQCamera3DOverviewMouseModeIFace_H

#include <CQCamera3DMouseModeIFace.h>
#include <CQCamera3DOverview.h>

class CQCamera3DCanvas;

class CQCamera3DOverviewMouseModeIFace : public CQCamera3DMouseModeIFace {
 public:
  using ViewType = CQCamera3DOverview::ViewType;

 public:
  CQCamera3DOverviewMouseModeIFace(CQCamera3DOverview *overview);

  const CPoint3D &cursor() const override;
  void setCursor(const CPoint3D &c) override;

  void moveCursorToMouse(const CPoint3D &c) override;

  CQCamera3DSelectData getSelection() const override;

  const CBBox3D &bbox() const override;

  const MoveDirection &moveDirection() const override;
  void setMoveDirection(const MoveDirection &d) override;

  bool is2D() const override { return true; }

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

  const ViewType &viewType() const { return viewType_; }
  void setViewType(const ViewType &v) { viewType_ = v; }

 private:
  CQCamera3DOverview *overview_ { nullptr };
  CQCamera3DCanvas*   canvas_   { nullptr };

  ViewType viewType_ { ViewType::NONE };
};

#endif
