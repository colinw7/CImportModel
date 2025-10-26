#include <CQNewGLStatusBar.h>
#include <CQNewGLModel.h>
#include <CQNewGLControl.h>
#include <CQNewGLCanvas.h>

#include <QLabel>
#include <QHBoxLayout>

CQNewGLStatusBar::
CQNewGLStatusBar(CQNewGLModel *app) :
 app_(app)
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  auto *layout = new QHBoxLayout(this);

#if 0
  auto addSpacer = [&](int w) {
    auto *spacer = new QFrame;
    spacer->setFixedWidth(w);
    layout->addWidget(spacer);
  };
#endif

  //---

  scaleLabel_ = new QLabel;

  layout->addWidget(scaleLabel_);
}

void
CQNewGLStatusBar::
setScaleLabel(const QString &label)
{
  scaleLabel_->setText(label);
}
