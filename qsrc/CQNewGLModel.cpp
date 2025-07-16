#include <CQNewGLModel.h>
#include <CQNewGLToolbar.h>
#include <CQNewGLControl.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLUVMap.h>

#include <CGeometry3D.h>

#include <QVBoxLayout>
#include <QSplitter>
#include <QStackedWidget>

CQNewGLModel::
CQNewGLModel() :
 QFrame()
{
  auto *layout = new QVBoxLayout(this);

  //---

  toolbar_ = new CQNewGLToolbar(this);

  layout->addWidget(toolbar_);

  //---

  auto *layout1 = new QHBoxLayout;

  layout->addLayout(layout1);

  auto *splitter = new QSplitter;

  layout1->addWidget(splitter);

  //---

  canvas_  = new CQNewGLCanvas (this);
  uvMap_   = new CQNewGLUVMap  (this);
  control_ = new CQNewGLControl(this);

  stack_ = new QStackedWidget(this);

  stack_->addWidget(canvas_);
  stack_->addWidget(uvMap_);

  splitter->addWidget(stack_);
  splitter->addWidget(control_);

  control_->setVisible(false);

  //---

  CGeometryInst->setShininess(shininess_);
}

void
CQNewGLModel::
setShininess(double r)
{
  shininess_ = r;

  CGeometryInst->setShininess(r);
}

void
CQNewGLModel::
setShowUVMap(bool b)
{
  showUVMap_ = b;

  stack_->setCurrentIndex(showUVMap_ ? 1 : 0);
}

void
CQNewGLModel::
setShowBone(bool b)
{
  showBone_ = b;
}

void
CQNewGLModel::
update()
{
  control_->updateWidgets();
}

void
CQNewGLModel::
updateCamera()
{
  control_->updateCamera();
}

void
CQNewGLModel::
updateLights()
{
  control_->updateLights();
}
