#include <CQCamera3DMaterialChooser.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DCanvas.h>

#include <CGeomScene3D.h>
#include <CQUtil.h>

CQCamera3DMaterialChooser::
CQCamera3DMaterialChooser(CQCamera3DApp *app) :
 app_(app)
{
  setObjectName("materialChooser");

  connect(app_, SIGNAL(modelAdded()), this, SLOT(needsUpdateSlot()));
  connect(app_, SIGNAL(materialAdded()), this, SLOT(needsUpdateSlot()));

  auto w = QFontMetrics(font()).horizontalAdvance("X")*32;
  setFixedWidth(w);

  //---

  connectSlots(true);
}

void
CQCamera3DMaterialChooser::
needsUpdateSlot()
{
  needsUpdate_ = true;

  updateWidgets();
}

void
CQCamera3DMaterialChooser::
setMaterialName(const QString &name)
{
  int ind = 0;

  auto *scene = app_->getScene();

  for (auto *material : scene->getMaterials()) {
    auto name1 = QString::fromStdString(material->name());

    if (name1 == name) {
      setCurrentIndex(ind + 1);
      return;
    }

    ++ind;
  }

  setCurrentIndex(0);
}

void
CQCamera3DMaterialChooser::
updateWidgets()
{
  if (! needsUpdate_)
    return;

  //---

  connectSlots(false);

  //---

  int currentInd = currentIndex();

  clear();

  addItem("", QVariant(-1));

  int ind = 0;

  auto *scene = app_->getScene();

  for (auto *material : scene->getMaterials()) {
    auto name = QString::fromStdString(material->name());

    addItem(name, QVariant(ind++));
  }

  setCurrentIndex(currentInd);

  //---

  connectSlots(true);
}

void
CQCamera3DMaterialChooser::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, this, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(currentIndexChanged(int)));
}

void
CQCamera3DMaterialChooser::
currentIndexChanged(int ind)
{
  materialName_ = "";

  int ind1 = 1;

  auto *scene = app_->getScene();

  for (auto *material : scene->getMaterials()) {
    auto name = QString::fromStdString(material->name());

    if (ind == ind1) {
      materialName_ = name;
      break;
    }

    ++ind1;
  }

  Q_EMIT materialChanged();
}
