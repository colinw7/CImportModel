#include <CQCamera3DTextureChooser.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DCanvas.h>

#include <CQGLTexture.h>
#include <CQUtil.h>

CQCamera3DTextureChooser::
CQCamera3DTextureChooser(CQCamera3DApp *app) :
 app_(app)
{
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

  const auto &textures = canvas->glTextures();

  int ind = 0;

  for (const auto &pt : textures) {
    const auto &textureData = pt.second;

    if (QString::fromStdString(textureData.glTexture->getName()) == name) {
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

  //---

  connectSlots(false);

  //---

  int currentInd = currentIndex();

  clear();

  addItem("", QVariant(-1));

  int ind = 0;

  const auto &textures = canvas->glTextures();

  for (const auto &pt : textures) {
    const auto &textureData = pt.second;

    auto objectName = QString::fromStdString(textureData.glTexture->getName());

    addItem(objectName, QVariant(ind++));
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

  textureName_ = "";

  int ind1 = 1;

  const auto &textures = canvas->glTextures();

  for (const auto &pt : textures) {
    const auto &textureData = pt.second;

    if (ind == ind1) {
      textureName_ = QString::fromStdString(textureData.glTexture->getName());
      break;
    }

    ++ind1;
  }

  Q_EMIT textureChanged();
}
