#include <CQNewGLLightsList.h>
#include <CQNewGLControl.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLLight.h>

#include <CQUtil.h>

CQNewGLLightsList::
CQNewGLLightsList(CQNewGLControl *control) :
 control_(control)
{
  setObjectName("lightsList");

  auto *layout = new QVBoxLayout(this);

  //--

  list_ = new QListWidget;

  list_->setSelectionMode(QListWidget::SingleSelection);

  layout->addWidget(list_);

  //---

  connectSlots(true);

  updateWidgets();
}

void
CQNewGLLightsList::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, list_,
    SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
    this, SLOT(currentItemSlot(QListWidgetItem *, QListWidgetItem *)));
}

void
CQNewGLLightsList::
updateWidgets()
{
  connectSlots(false);

  //---

  auto *canvas = control_->canvas();

  list_->clear();

  int ind = 0;

  for (auto *light : canvas->lights()) {
    const auto &name = light->name();

    auto *item = new QListWidgetItem(name);

    list_->addItem(item);

    item->setData(Qt::UserRole, ind++);
  }

  //---

  connectSlots(true);
}

void
CQNewGLLightsList::
currentItemSlot(QListWidgetItem *item, QListWidgetItem *)
{
  auto *canvas = control_->canvas();

  int ind = item->data(Qt::UserRole).toInt();

  canvas->setCurrentLight(ind);

  //updateWidgets();

  canvas->update();

  Q_EMIT currentItemChanged();
}
