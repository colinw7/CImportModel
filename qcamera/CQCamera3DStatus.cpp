#include <CQCamera3DStatus.h>
#include <CQCamera3DApp.h>

#include <CQDocumentLabel.h>
#include <CQTextLabel.h>

#include <QHBoxLayout>

CQCamera3DStatus::
CQCamera3DStatus(CQCamera3DApp *app) :
 app_(app)
{
  setObjectName("status");

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  auto *layout = new QHBoxLayout(this);

  tipLabel_ = new CQDocumentLabel(); tipLabel_->setObjectName("tip");

  layout->addWidget(tipLabel_);

  stateLabel_ = new CQTextLabel(" "); stateLabel_->setObjectName("state");
  modelLabel_ = new CQTextLabel(" "); modelLabel_->setObjectName("model");
  mouseLabel_ = new CQTextLabel(" "); mouseLabel_->setObjectName("mouse");

  layout->addWidget (stateLabel_);
  layout->addWidget (modelLabel_);
  layout->addStretch(1);
  layout->addWidget (mouseLabel_);
}

void
CQCamera3DStatus::
setTipLabel(const QString &label)
{
  tipLabel_->setText(label);
}

void
CQCamera3DStatus::
setStateLabel(const QString &label)
{
  stateLabel_->setText(label);
}

void
CQCamera3DStatus::
setModelLabel(const QString &label)
{
  modelLabel_->setText(label);
}

void
CQCamera3DStatus::
setMouseLabel(const QString &label)
{
  mouseLabel_->setText(label);
}
