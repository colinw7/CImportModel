#include <CQCamera3DStatus.h>
#include <CQCamera3DApp.h>

#include <CQTextLabel.h>

#include <QHBoxLayout>

CQCamera3DStatus::
CQCamera3DStatus(CQCamera3DApp *app) :
 app_(app)
{
  setObjectName("status");

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  auto *layout = new QHBoxLayout(this);

  modelLabel_ = new CQTextLabel(" ");
  mouseLabel_ = new CQTextLabel(" ");

  layout->addWidget (modelLabel_);
  layout->addStretch(1);
  layout->addWidget (mouseLabel_);
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
