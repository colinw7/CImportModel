#include <CQCamera3DLightList.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DLight.h>

#include <CQUtil.h>

#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>

CQCamera3DLightList::
CQCamera3DLightList(CQCamera3DCanvas *canvas) :
 QFrame(nullptr), canvas_(canvas)
{
  setObjectName("lightList");

  auto *layout = new QVBoxLayout(this);

  list_ = new QListWidget;

  list_->setSelectionMode(QListWidget::SingleSelection);

  layout->addWidget(list_);

  //---

  connectSlots(true);
}

void
CQCamera3DLightList::
updateLights()
{
  if (valid_)
    return;

  valid_ = true;

  //---

  connectSlots(false);

  //---

  auto *canvas = this->canvas_;

  list_->clear();

  auto *item = new QListWidgetItem("");

  list_->addItem(item);

  item->setData(Qt::UserRole, -1);

  for (auto *light : canvas->lights()) {
    auto name = QString::fromStdString(light->name());

    auto *item = new QListWidgetItem(name);

    list_->addItem(item);

    item->setData(Qt::UserRole, light->id());
  }

  selectedInd_ = -1;

  //---

  connectSlots(true);
}

void
CQCamera3DLightList::
connectSlots(bool b)
{
  auto *canvas = this->canvas_;

  CQUtil::connectDisconnect(b, canvas, SIGNAL(lightAdded()), this, SLOT(invalidate()));

  if (b) {
    connect(list_, &QListWidget::currentItemChanged,
            this, &CQCamera3DLightList::itemSelectedSlot);
  }
  else {
    disconnect(list_, &QListWidget::currentItemChanged,
               this, &CQCamera3DLightList::itemSelectedSlot);
  }
}

void
CQCamera3DLightList::
itemSelectedSlot(QListWidgetItem *item, QListWidgetItem *)
{
  selectedInd_ = (item ? item->data(Qt::UserRole).toInt() : -1);
//std::cerr << selectedInd_ << "\n";

  Q_EMIT currentItemChanged();
}

CQCamera3DLight *
CQCamera3DLightList::
getCurrentLight() const
{
  if (selectedInd_ < 0)
    return nullptr;

  auto *canvas = this->canvas_;

  return canvas->getLightById(selectedInd_);
}

CQCamera3DLight *
CQCamera3DLightList::
getSelectedLight() const
{
  auto selectedItems = list_->selectedItems();

  int ind = -1;

  for (const auto *item : selectedItems) {
    int ind1 = item->data(Qt::UserRole).toInt();
    if (ind1 >= 0) {
      ind = ind1;
      break;
    }
  }

  if (ind < 0)
    return nullptr;

  auto *canvas = this->canvas_;

  return canvas->getLightById(ind);
}
