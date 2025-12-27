#include <CQNewGLCamerasList.h>
#include <CQNewGLControl.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLCamera.h>

#include <CQUtil.h>

CQNewGLCamerasList::
CQNewGLCamerasList(CQNewGLControl *control) :
 control_(control)
{
  setObjectName("camerasList");

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
CQNewGLCamerasList::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, list_,
    SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
    this, SLOT(currentItemSlot(QListWidgetItem *, QListWidgetItem *)));
}

void
CQNewGLCamerasList::
updateWidgets()
{
  connectSlots(false);

  //---

  auto *canvas = control_->canvas();

  list_->clear();

  int ind = 0;

  for (auto *camera : canvas->cameras()) {
    const auto &name = camera->name();

    auto *item = new QListWidgetItem(name);

    list_->addItem(item);

    item->setData(Qt::UserRole, ind++);
  }

  //---

  connectSlots(true);
}

void
CQNewGLCamerasList::
currentItemSlot(QListWidgetItem *item, QListWidgetItem *)
{
  auto *canvas = control_->canvas();

  currentCamera_ = item->data(Qt::UserRole).toInt();

  if (isUpdateCurrent())
    canvas->setCurrentCamera(currentCamera_);

  //updateWidgets();

  canvas->update();

  Q_EMIT currentItemChanged();
}

CQNewGLCamera *
CQNewGLCamerasList::
getCurrentCamera() const
{
  auto *canvas = control_->canvas();

  return canvas->getCamera(currentCamera_);
}
