#include <CQCamera3DCameraList.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DCamera.h>

#include <CQUtil.h>

#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>

CQCamera3DCameraList::
CQCamera3DCameraList(CQCamera3DCanvas *canvas) :
 QFrame(nullptr), canvas_(canvas)
{
  setObjectName("cameraList");

  auto *layout = new QVBoxLayout(this);

  list_ = new QListWidget;

  list_->setSelectionMode(QListWidget::SingleSelection);

  layout->addWidget(list_);

  //---

  connectSlots(true);
}

void
CQCamera3DCameraList::
updateCameras()
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

  for (auto *camera : canvas->cameras()) {
    auto name = camera->name();

    auto *item = new QListWidgetItem(name);

    list_->addItem(item);

    item->setData(Qt::UserRole, camera->id());
  }

  selectedInd_ = -1;

  //---

  connectSlots(true);
}

void
CQCamera3DCameraList::
connectSlots(bool b)
{
  auto *canvas = this->canvas_;

  CQUtil::connectDisconnect(b, canvas, SIGNAL(cameraAdded()), this, SLOT(invalidate()));

  if (b) {
    connect(list_, &QListWidget::currentItemChanged,
            this, &CQCamera3DCameraList::itemSelectedSlot);
  }
  else {
    disconnect(list_, &QListWidget::currentItemChanged,
               this, &CQCamera3DCameraList::itemSelectedSlot);
  }
}

void
CQCamera3DCameraList::
itemSelectedSlot(QListWidgetItem *item, QListWidgetItem *)
{
  selectedInd_ = (item ? item->data(Qt::UserRole).toInt() : -1);
//std::cerr << selectedInd_ << "\n";

  Q_EMIT currentItemChanged();
}

CQCamera3DCamera *
CQCamera3DCameraList::
getCurrentCamera() const
{
  if (selectedInd_ < 0)
    return nullptr;

  auto *canvas = this->canvas_;

  return canvas->getCameraById(selectedInd_);
}

CQCamera3DCamera *
CQCamera3DCameraList::
getSelectedCamera() const
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

  return canvas->getCameraById(ind);
}
