#include <CQCamera3DTextureChooser.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DApp.h>

#include <CGeomScene3D.h>
#include <CQGLTexture.h>
#include <CQUtil.h>

CQCamera3DTextureChooser::
CQCamera3DTextureChooser(CQCamera3DApp *app) :
 app_(app)
{
  setObjectName("textureChooser");

  connectSlots(true);

  connect(app_, SIGNAL(textureAdded()), this, SLOT(needsUpdateSlot()));

  auto w = QFontMetrics(font()).horizontalAdvance("X")*32;
  setFixedWidth(w);
}

void
CQCamera3DTextureChooser::
needsUpdateSlot()
{
  needsUpdate_ = true;

  updateWidgets();
}

void
CQCamera3DTextureChooser::
setTextureName(const QString &name)
{
  auto *canvas = app_->canvas();
  if (! canvas) return;

  auto *scene = canvas->app()->getScene();

  //---

  int ind = 0;

  for (auto *texture : scene->textures()) {
    if (QString::fromStdString(texture->name()) == name) {
      setCurrentIndex(ind + 1);
      return;
    }

    ++ind;
  }

  setCurrentIndex(0);
}

void
CQCamera3DTextureChooser::
updateWidgets()
{
  auto *canvas = app_->canvas();
  if (! canvas) return;

  if (! needsUpdate_)
    return;

  auto *scene = canvas->app()->getScene();

  //---

  connectSlots(false);

  //---

  int currentInd = currentIndex();

  clear();

  addItem("", QVariant(-1));

  int ind = 0;

  for (auto *texture : scene->textures()) {
    auto textureName = QString::fromStdString(texture->name());

    addItem(textureName, QVariant(ind++));
  }

  setCurrentIndex(currentInd);

  //---

  connectSlots(true);
}

void
CQCamera3DTextureChooser::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, this, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(currentIndexChanged(int)));
}

void
CQCamera3DTextureChooser::
currentIndexChanged(int ind)
{
  auto *canvas = app_->canvas();
  if (! canvas) return;

  auto *scene = canvas->app()->getScene();

  //---

  textureName_ = "";

  int ind1 = 1;

  for (auto *texture : scene->textures()) {
    if (ind == ind1) {
      textureName_ = QString::fromStdString(texture->name());
      break;
    }

    ++ind1;
  }

  Q_EMIT textureChanged();
}
