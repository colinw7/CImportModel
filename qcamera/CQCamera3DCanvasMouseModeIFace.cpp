#include <CQCamera3DCanvasMouseModeIFace.h>
#include <CQCamera3DCanvas.h>

CQCamera3DCanvasMouseModeIFace::
CQCamera3DCanvasMouseModeIFace(CQCamera3DCanvas *canvas) :
 canvas_(canvas)
{
}

const CPoint3D &
CQCamera3DCanvasMouseModeIFace::
cursor() const
{
  return canvas_->cursor();
}

void
CQCamera3DCanvasMouseModeIFace::
setCursor(const CPoint3D &c)
{
  canvas_->setCursor(c);
}

void
CQCamera3DCanvasMouseModeIFace::
moveCursorToMouse(const CPoint3D &c)
{
  canvas_->moveCursorToMouse(c);
}

CQCamera3DSelectData
CQCamera3DCanvasMouseModeIFace::
getSelection() const
{
  return canvas_->getSelection();
}

const CBBox3D &
CQCamera3DCanvasMouseModeIFace::
bbox() const
{
  return canvas_->bbox();
}

const CQCamera3DCanvasMouseModeIFace::MoveDirection &
CQCamera3DCanvasMouseModeIFace::
moveDirection() const
{
  return canvas_->moveDirection();
}

void
CQCamera3DCanvasMouseModeIFace::
setMoveDirection(const MoveDirection &d)
{
  canvas_->setMoveDirection(d);
}

void
CQCamera3DCanvasMouseModeIFace::
moveObject(CGeomObject3D *object, const CVector3D &d)
{
  canvas_->moveObject(object, d);
  canvas_->update();
}

void
CQCamera3DCanvasMouseModeIFace::
moveFace(CGeomFace3D *face, const CVector3D &d)
{
  canvas_->moveFace(face, d);
  canvas_->update();
}

void
CQCamera3DCanvasMouseModeIFace::
scaleObject(CGeomObject3D *object, const CVector3D &d)
{
  canvas_->scaleObject(object, d);
  canvas_->update();
}

void
CQCamera3DCanvasMouseModeIFace::
rotateObject(CGeomObject3D *object, double a, const CVector3D &d)
{
  canvas_->rotateObject(object, a, d);
  canvas_->update();
}

void
CQCamera3DCanvasMouseModeIFace::
extrudeMove(double d)
{
  canvas_->extrudeMove(d);
  canvas_->update();
}

void
CQCamera3DCanvasMouseModeIFace::
endUpdateObjects()
{
  canvas_->addScene();
  canvas_->update();
}

void
CQCamera3DCanvasMouseModeIFace::
setOptions(CQCamera3DOptions *options)
{
  canvas_->setOptions(options);
}

void
CQCamera3DCanvasMouseModeIFace::
resetOptions()
{
  canvas_->setOptions(nullptr);

  canvas_->setMoveDirection(MoveDirection::NONE);

  canvas_->setEditType(EditType::SELECT);
}

void
CQCamera3DCanvasMouseModeIFace::
showOptions()
{
  canvas_->showOptions();
}

void
CQCamera3DCanvasMouseModeIFace::
hideOptions()
{
  canvas_->hideOptions();
}
