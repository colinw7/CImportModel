#include <CQNewGLShapesList.h>
#include <CQNewGLControl.h>
#include <CQNewGLCanvas.h>

#include <CQUtil.h>

CQNewGLShapesList::
CQNewGLShapesList(CQNewGLControl *control) :
 control_(control)
{
  setObjectName("bonesList");

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
CQNewGLShapesList::
connectSlots(bool b)
{
  auto *canvas = control_->canvas();

  CQUtil::connectDisconnect(b, canvas, SIGNAL(currentShapeChanged()),
                            this, SLOT(invalidateShapes()));

  CQUtil::connectDisconnect(b, canvas, SIGNAL(shapeAdded()),
                            this, SLOT(invalidateShapes()));
  CQUtil::connectDisconnect(b, canvas, SIGNAL(shapeDeleted()),
                            this, SLOT(invalidateShapes()));

  if (b) {
    connect(list_, &QListWidget::currentItemChanged,
            this, &CQNewGLShapesList::currentItemSlot);
  }
  else {
    disconnect(list_, &QListWidget::currentItemChanged,
               this, &CQNewGLShapesList::currentItemSlot);
  }
}

void
CQNewGLShapesList::
invalidateShapes()
{
  reload_ = true;

  updateWidgets();
}

void
CQNewGLShapesList::
updateWidgets()
{
  connectSlots(false);

  //---

  auto *canvas = control_->canvas();

  if (reload_) {
    reload_ = false;

    auto *currentShape =  canvas->getCurrentShape();

    list_->clear();

    uint ind        = 0;
    uint currentInd = 0;

    for (auto *shape : canvas->shapes()) {
      if (shape == currentShape)
        currentInd = ind;

      const auto &name = shape->name();

      auto *item = new QListWidgetItem(name);

      list_->addItem(item);

      item->setData(Qt::UserRole, ind++);
    }

    list_->setCurrentRow(currentInd);
  }

  //---

  connectSlots(true);
}

void
CQNewGLShapesList::
currentItemSlot(QListWidgetItem *item, QListWidgetItem *)
{
  auto *canvas = control_->canvas();

  int ind = item->data(Qt::UserRole).toInt();

  canvas->setCurrentShape(ind);
}
