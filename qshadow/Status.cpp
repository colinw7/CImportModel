#include <Status.h>
#include <App.h>
#include <UI.h>

#include <CQDocumentLabel.h>
#include <CQTextLabel.h>

#include <QHBoxLayout>

namespace CQShadow3D {

Status::
Status(App *app) :
 app_(app)
{
  setObjectName("status");

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  auto *layout = new QHBoxLayout(this);
  layout->setMargin(2); layout->setSpacing(2);

  //---

  UI ui(this, layout);

  //---

  tipLabel_ = new CQDocumentLabel(); tipLabel_->setObjectName("tip");

  ui.addWidget(tipLabel_);

  stateLabel_ = new CQTextLabel(" "); stateLabel_->setObjectName("state");
  mouseLabel_ = new CQTextLabel(" "); mouseLabel_->setObjectName("mouse");

  ui.addWidget(stateLabel_);
  ui.addWidget(mouseLabel_);

  ui.addStretch();

  ui.addWidget(mouseLabel_);
}

void
Status::
setTipLabel(const QString &label)
{
  tipLabel_->setText(label);
}

void
Status::
setStateLabel(const QString &label)
{
  stateLabel_->setText(label);
}

void
Status::
setMouseLabel(const QString &label)
{
  mouseLabel_->setText(label);
}

}
