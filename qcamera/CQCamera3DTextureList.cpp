#include <CQCamera3DTextureList.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DApp.h>

#include <CQGLTexture.h>
#include <CQUtil.h>
#include <CGeomScene3D.h>
#include <CGeomTexture.h>

#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>

CQCamera3DTextureList::
CQCamera3DTextureList(CQCamera3DCanvas *canvas) :
 QFrame(nullptr), canvas_(canvas)
{
  setObjectName("textureList");

  connect(canvas_->app(), SIGNAL(textureAdded()), this, SLOT(needsUpdateSlot()));

  //---

  auto *layout = new QVBoxLayout(this);

  list_ = new QListWidget;

  list_->setSelectionMode(QListWidget::SingleSelection);

  layout->addWidget(list_);

  //---

  connectSlots(true);
}

void
CQCamera3DTextureList::
needsUpdateSlot()
{
  valid_ = false;

  updateTextures();
}

void
CQCamera3DTextureList::
updateTextures()
{
  if (valid_)
    return;

  valid_ = true;

  //---

  connectSlots(false);

  //---

  auto *canvas = this->canvas_;

  auto *scene = canvas->app()->getScene();

  //---

  list_->clear();

  auto *item = new QListWidgetItem("");

  list_->addItem(item);

  item->setData(Qt::UserRole, -1);

  for (auto *texture : scene->textures()) {
    auto textureName = QString::fromStdString(texture->name());

    auto *item = new QListWidgetItem(textureName);

    list_->addItem(item);

    item->setData(Qt::UserRole, texture->id());
  }

  selectedInd_ = -1;

  //---

  connectSlots(true);
}

void
CQCamera3DTextureList::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b,
    list_, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
    this, SLOT(itemSelectedSlot(QListWidgetItem *, QListWidgetItem *)));
}

void
CQCamera3DTextureList::
itemSelectedSlot(QListWidgetItem *item, QListWidgetItem *)
{
  selectedInd_ = (item ? item->data(Qt::UserRole).toInt() : -1);

  Q_EMIT currentItemChanged();
}

CGeomTexture *
CQCamera3DTextureList::
getCurrentTexture() const
{
  if (selectedInd_ < 0)
    return nullptr;

  auto *canvas = this->canvas_;
  auto *scene  = canvas->app()->getScene();

  return scene->getTextureById(selectedInd_);
}

CGeomTexture *
CQCamera3DTextureList::
getSelectedTexture() const
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

  return scene->getTextureById(ind);
}
