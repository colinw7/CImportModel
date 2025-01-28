#include <CQNewGLModel.h>
#include <CQNewGLToolbar.h>
#include <CQNewGLControl.h>
#include <CQNewGLCanvas.h>

#include <QVBoxLayout>
#include <QSplitter>

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
  control_ = new CQNewGLControl(this);

  splitter->addWidget(canvas_);
  splitter->addWidget(control_);

  control_->setVisible(false);
}
