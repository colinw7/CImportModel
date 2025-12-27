#include <CQCamera3DMaterialList.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DGeomObject.h>

#include <CQUtil.h>
#include <CGeomObject3D.h>
#include <CGeomScene3D.h>

#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>

CQCamera3DMaterialList::
CQCamera3DMaterialList(CQCamera3DCanvas *canvas) :
 QFrame(nullptr), canvas_(canvas)
{
  setObjectName("materialList");

  auto *layout = new QVBoxLayout(this);

  list_ = new QListWidget;

  list_->setSelectionMode(QListWidget::SingleSelection);

  layout->addWidget(list_);

  //---

  connectSlots(true);
}

void
CQCamera3DMaterialList::
updateMaterials()
{
  if (valid_)
    return;

  valid_ = true;

  //---

  connectSlots(false);

  //---

  auto *canvas = this->canvas_;

  list_->clear();

  auto *scene = canvas->app()->getScene();

  auto *item = new QListWidgetItem("");

  list_->addItem(item);

  item->setData(Qt::UserRole, -1);

  for (auto *material : scene->getMaterials()) {
    auto name = QString::fromStdString(material->name());

    auto *item = new QListWidgetItem(name);

    list_->addItem(item);

    item->setData(Qt::UserRole, material->id());
  }

  selectedInd_ = -1;

  //---

  connectSlots(true);
}

void
CQCamera3DMaterialList::
connectSlots(bool b)
{
  auto *canvas = this->canvas_;

  CQUtil::connectDisconnect(b, canvas->app(), SIGNAL(modelAdded()), this, SLOT(invalidate()));

  if (b) {
    connect(list_, &QListWidget::currentItemChanged,
            this, &CQCamera3DMaterialList::itemSelectedSlot);
  }
  else {
    disconnect(list_, &QListWidget::currentItemChanged,
               this, &CQCamera3DMaterialList::itemSelectedSlot);
  }
}

void
CQCamera3DMaterialList::
itemSelectedSlot(QListWidgetItem *item, QListWidgetItem *)
{
  selectedInd_ = (item ? item->data(Qt::UserRole).toInt() : -1);
//std::cerr << selectedInd_ << "\n";

  Q_EMIT currentItemChanged();
}

CGeomMaterial *
CQCamera3DMaterialList::
getCurrentMaterial() const
{
  if (selectedInd_ < 0)
    return nullptr;

  auto *canvas = this->canvas_;
  auto *scene  = canvas->app()->getScene();

  return scene->getMaterialById(selectedInd_);
}

CGeomMaterial *
CQCamera3DMaterialList::
getSelectedMaterial() const
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
  auto *scene  = canvas->app()->getScene();

  return scene->getMaterialById(ind);
}
