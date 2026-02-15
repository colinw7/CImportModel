#include <TextureArea.h>
#include <App.h>
#include <UI.h>

#include <CQIntegerSpin.h>

#include <QPainter>
#include <QVBoxLayout>

namespace CQTextureGen {

TextureArea::
TextureArea(App *app) :
 app_(app)
{
  setObjectName("textureArea");

  //setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  auto *layout = new QVBoxLayout(this);

  UI ui(this, layout);

  //---

  textureSizeEdit_ = ui.addLabelEdit("Size", new CQIntegerSpin);

  ui.addStretch();

  //---

  connectSlots(true);

  updateWidgets();
}

void
TextureArea::
updateWidgets()
{
  connectSlots(false);

  textureSizeEdit_->setValue(app_->textureSize());

  connectSlots(true);
}

void
TextureArea::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, textureSizeEdit_, SIGNAL(valueChanged(int)),
                            this, SLOT(textureSizeSlot(int)));
}

void
TextureArea::
textureSizeSlot(int s)
{
  app_->setTextureSize(s);
}

void
TextureArea::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  int ix = 0;
  int iy = textureSizeEdit_->height() + 16;

  auto drawImage = [&](const QImage &image) {
    painter.drawImage(ix, iy, image);
    iy += app_->textureSize() + 4;
  };

  drawImage(app_->diffuseImage ());
  drawImage(app_->normalImage  ());
  drawImage(app_->specularImage());
  drawImage(app_->emissionImage());
  drawImage(app_->metallicImage());
  drawImage(app_->ORMImage     ());
}

}
