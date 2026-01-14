#include <CQCamera3DTextures.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DShape.h>
#include <CQCamera3DGeomObject.h>
#include <CQCamera3DTexture.h>

#include <CQGLBuffer.h>
#include <CQGLTexture.h>
#include <CImportBase.h>
#include <CGeomScene3D.h>
#include <CGeomObject3D.h>
#include <CGeomTexture.h>
#include <CQImage.h>

#include <QPainter>
#include <QMouseEvent>

CQCamera3DTextures::
CQCamera3DTextures(CQCamera3DApp *app) :
 app_(app)
{
  setObjectName("textures");

  auto *canvas = app_->canvas();

  connect(canvas, SIGNAL(stateChanged()), this, SLOT(invalidateSlot()));

  connect(app_, SIGNAL(textureAdded()), this, SLOT(invalidateSlot()));
}

void
CQCamera3DTextures::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.fillRect(rect(), bgColor_);

  if (textureDatas_.empty())
    placeTextures();

  auto selectColor = QColor(40, 40, 40);

  for (const auto &textureData : textureDatas_) {
    auto iw = textureData.qimage.width ();
    auto ih = textureData.qimage.height();

    auto x1 = textureData.rect.left() + (textureData.rect.width () - iw)/2;
    auto y1 = textureData.rect.top () + (textureData.rect.height() - ih)/2;

    if (textureData.texture->isSelected()) {
      painter.fillRect(textureData.rect, selectColor);
    }

    painter.drawImage(x1, y1, textureData.qimage);
  }
}

void
CQCamera3DTextures::
resizeEvent(QResizeEvent *)
{
  w_ = width ();
  h_ = height();

  invalidateSlot();
}

void
CQCamera3DTextures::
invalidateSlot()
{
  textureDatas_.clear();

  update();
}

void
CQCamera3DTextures::
placeTextures()
{
  auto *scene  = app_->getScene();
  auto *canvas = app_->canvas();

  const auto &textures = scene->textures();

  auto n = textures.size();
  if (n == 0) return;

  int nr = std::max(int(sqrt(n)), 1);
  int nc = (n + nr - 1)/nr;

  if (w_ >= h_) {
    if (nc < nr)
      std::swap(nr, nc);
  }
  else {
    if (nr < nc)
      std::swap(nr, nc);
  }

  auto iw = double(w_)/nc;
  auto ih = double(h_)/nr;

  int ir = 0;
  int ic = 0;

  textureDatas_.clear();

  for (auto *texture : textures) {
    auto *texture1 = dynamic_cast<CQCamera3DTexture *>(texture);

    auto *glTexture = texture1->glTexture(canvas);

    TextureData textureData1;

    textureData1.texture = texture;

    auto qimage = glTexture->getImage();

    auto iw1 = int(iw - border_);
    auto ih1 = int(ih - border_);

    if (! qimage.isNull())
      textureData1.qimage = qimage.scaled(iw1, ih1, Qt::KeepAspectRatio, Qt::FastTransformation);

    auto x = ic*iw;
    auto y = ir*ih;

    textureData1.rect = QRect(x, y, iw, ih);

    textureDatas_.push_back(textureData1);

    ++ic;

    if (ic >= nc) {
      ++ir;

      ic = 0;
    }
  }
}

void
CQCamera3DTextures::
mousePressEvent(QMouseEvent *e)
{
  auto p = e->pos();

  CGeomTexture *texture = nullptr;

  for (const auto &textureData : textureDatas_) {
    if (textureData.rect.contains(p)) {
      texture = textureData.texture;
      break;
    }
  }

  for (const auto &textureData : textureDatas_)
    textureData.texture->setSelected(textureData.texture == texture);

  update();

  if (texture)
    app_->setCurrentTexture(QString::fromStdString(texture->name()));

  Q_EMIT selectionChanged();
}
