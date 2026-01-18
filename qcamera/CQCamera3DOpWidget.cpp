#include <CQCamera3DOpWidget.h>
#include <CQCamera3DUI.h>
#include <CQCamera3DOptions.h>

#include <CQRealSpin.h>

CQCamera3DOpWidget::
CQCamera3DOpWidget(CQCamera3DCanvas *canvas) :
 CQWinWidget(canvas), canvas_(canvas)
{
  setObjectName("opWidget");

  setResizable(false);
  setCollapsable(true);

  contents_ = new CQCamera3DOpContents(this);

  setChild(contents_);

  connect(this, SIGNAL(closed()), this, SLOT(closeSlot()));
}

void
CQCamera3DOpWidget::
setOptions(CQCamera3DOptions *options)
{
  contents_->setOptions(options);

  fitChild();
}

void
CQCamera3DOpWidget::
closeSlot()
{
  canvas_->endMouseMode();
}

//---

CQCamera3DOpContents::
CQCamera3DOpContents(CQCamera3DOpWidget *widget) :
 QFrame(widget), widget_(widget)
{
  setObjectName("contents");

  layout_ = new QVBoxLayout(this);
  layout_->setMargin(0); layout_->setSpacing(0);

  frame_ = new QFrame;
  layout_->addWidget(frame_);

  frameLayout_ = new QVBoxLayout(frame_);
  frameLayout_->setMargin(0); frameLayout_->setSpacing(0);
}

CQCamera3DOpContents::
~CQCamera3DOpContents()
{
  delete options_;
}

void
CQCamera3DOpContents::
setOptions(CQCamera3DOptions *options)
{
  delete options_;

  options_ = options;

  delete frame_;

  if (options) {
    frame_ = new QFrame;
    layout_->addWidget(frame_);

    frameLayout_ = new QVBoxLayout(frame_);
    frameLayout_->setMargin(0); frameLayout_->setSpacing(0);

    CQCamera3DUI ui(this, frameLayout_);

    options->addOptions(&ui);

    layout_->invalidate();

    widget_->setWindowTitle(options->title());
  }
  else
    frame_ = nullptr;
}

QSize
CQCamera3DOpContents::
sizeHint() const
{
  if (frame_)
    return frame_->sizeHint();
  else
    return QFrame::sizeHint();
}
