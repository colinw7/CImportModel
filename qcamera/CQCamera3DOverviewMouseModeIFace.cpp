#include <CQCamera3DOverviewMouseModeIFace.h>
#include <CQCamera3DOverview.h>
#include <CQCamera3DCanvas.h>

CQCamera3DOverviewMouseModeIFace::
CQCamera3DOverviewMouseModeIFace(CQCamera3DOverview *overview) :
 overview_(overview)
{
  canvas_ = overview_->app()->canvas();
}

const CPoint3D &
CQCamera3DOverviewMouseModeIFace::
cursor() const
{
  return canvas_->cursor();
}

void
CQCamera3DOverviewMouseModeIFace::
setCursor(const CPoint3D &c)
{
  canvas_->setCursor(c);
}

void
CQCamera3DOverviewMouseModeIFace::
moveCursorToMouse(const CPoint3D &c)
{
  canvas_->moveCursorToMouse(c);
}

CQCamera3DSelectData
CQCamera3DOverviewMouseModeIFace::
getSelection() const
{
  return canvas_->getSelection();
}

const CBBox3D &
CQCamera3DOverviewMouseModeIFace::
bbox() const
{
  return canvas_->bbox();
}

const CQCamera3DOverviewMouseModeIFace::MoveDirection &
CQCamera3DOverviewMouseModeIFace::
moveDirection() const
{
  return canvas_->moveDirection();
}

void
CQCamera3DOverviewMouseModeIFace::
setMoveDirection(const MoveDirection &d)
{
  canvas_->setMoveDirection(d);
}

void
CQCamera3DOverviewMouseModeIFace::
moveObject(CGeomObject3D *object, const CVector3D &d)
{
  auto dx = d.getX();
  auto dy = d.getY();

  if      (viewType_ == ViewType::XY)
    canvas_->moveObject(object, CVector3D(dx, dy, 0));
  else if (viewType_ == ViewType::ZY)
    canvas_->moveObject(object, CVector3D(0, dy, dx));
  else if (viewType_ == ViewType::XZ)
    canvas_->moveObject(object, CVector3D(dx, 0, -dy));

  overview_->update();
}

void
CQCamera3DOverviewMouseModeIFace::
moveFace(CGeomFace3D *face, const CVector3D &d)
{
  canvas_->moveFace(face, d);
  overview_->update();
}

void
CQCamera3DOverviewMouseModeIFace::
scaleObject(CGeomObject3D *object, const CVector3D &d)
{
  canvas_->scaleObject(object, d);
  overview_->update();
}

void
CQCamera3DOverviewMouseModeIFace::
rotateObject(CGeomObject3D *object, double a, const CVector3D &d)
{
  canvas_->rotateObject(object, a, d);
  overview_->update();
}

void
CQCamera3DOverviewMouseModeIFace::
extrudeMove(double d)
{
  canvas_->extrudeMove(d);
  overview_->update();
}

void
CQCamera3DOverviewMouseModeIFace::
endUpdateObjects()
{
  canvas_->addObjectsData();
  overview_->update();
}

void
CQCamera3DOverviewMouseModeIFace::
setOptions(CQCamera3DOptions *options)
{
  overview_->setOptions(options);
}

void
CQCamera3DOverviewMouseModeIFace::
resetOptions()
{
  overview_->setOptions(nullptr);

  canvas_->setMoveDirection(MoveDirection::NONE);

  overview_->setEditType(EditType::SELECT);
}

void
CQCamera3DOverviewMouseModeIFace::
showOptions()
{
  overview_->showOptions();
}

void
CQCamera3DOverviewMouseModeIFace::
hideOptions()
{
  overview_->hideOptions();
}
