#include <CQCamera3DStatus.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DUI.h>

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
  layout->setMargin(2); layout->setSpacing(2);

  //---

  CQCamera3DUI ui(this, layout);

  //---

  tipLabel_ = new CQDocumentLabel(); tipLabel_->setObjectName("tip");

  ui.addWidget(tipLabel_);

  stateLabel_ = new CQTextLabel(" "); stateLabel_->setObjectName("state");
  modelLabel_ = new CQTextLabel(" "); modelLabel_->setObjectName("model");
  mouseLabel_ = new CQTextLabel(" "); mouseLabel_->setObjectName("mouse");

  ui.addWidget(stateLabel_);
  ui.addWidget(modelLabel_);

  ui.addStretch();

  ui.addWidget(mouseLabel_);

  viewFollowButton_ =
    ui.addIconCheckButton("viewFollow", "VIEW_FOLLOW", "View Follow");

  CQUtil::connectDisconnect(true,
    viewFollowButton_, SIGNAL(toggled(bool)), this, SLOT(viewFollowSlot(bool)));
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

void
CQCamera3DStatus::
viewFollowSlot(bool b)
{
  app_->setSyncView(b);
}
