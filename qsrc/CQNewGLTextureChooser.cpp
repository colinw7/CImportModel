#include <CQNewGLTextureChooser.h>
#include <CQNewGLControl.h>
#include <CQNewGLCanvas.h>

#include <CQGLTexture.h>
#include <CQUtil.h>

CQNewGLTextureChooser::
CQNewGLTextureChooser(CQNewGLControl *control) :
 control_(control)
{
  connectSlots(true);

  auto *canvas = control_->canvas();

  connect(canvas, SIGNAL(textureAdded()), this, SLOT(needsUpdateSlot()));

  auto w = QFontMetrics(font()).horizontalAdvance("X")*32;
  setFixedWidth(w);
}

void
CQNewGLTextureChooser::
needsUpdateSlot()
{
  needsUpdate_ = true;

  updateWidgets();
}

void
CQNewGLTextureChooser::
setTextureName(const QString &name)
{
  auto *canvas = control_->canvas();

  const auto &textures = canvas->glTextures();

  int ind = 0;

  for (const auto &pr : textures) {
    const auto &textureData = pr.second;

    if (QString::fromStdString(textureData.glTexture->getName()) == name) {
      setCurrentIndex(ind + 1);
      return;
    }

    ++ind;
  }

  setCurrentIndex(0);
}

void
CQNewGLTextureChooser::
updateWidgets()
{
  if (! needsUpdate_)
    return;

  //---

  connectSlots(false);

  //---

  int currentInd = currentIndex();

  auto *canvas = control_->canvas();

  clear();

  addItem("", QVariant(-1));

  int ind = 0;

  const auto &textures = canvas->glTextures();

  for (const auto &pr : textures) {
    const auto &textureData = pr.second;

    auto objectName = QString::fromStdString(textureData.glTexture->getName());

    addItem(objectName, QVariant(ind++));
  }

  setCurrentIndex(currentInd);

  //---

  connectSlots(true);
}

void
CQNewGLTextureChooser::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, this, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(currentIndexChanged(int)));
}

void
CQNewGLTextureChooser::
currentIndexChanged(int ind)
{
  auto *canvas = control_->canvas();

  textureName_ = "";

  int ind1 = 1;

  const auto &textures = canvas->glTextures();

  for (const auto &pr : textures) {
    const auto &textureData = pr.second;

    if (ind == ind1) {
      textureName_ = QString::fromStdString(textureData.glTexture->getName());
      break;
    }

    ++ind1;
  }

  Q_EMIT textureChanged();
}
