#include <CQCamera3DTextures.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DShape.h>
#include <CQCamera3DGeomObject.h>

#include <CQGLBuffer.h>
#include <CImportBase.h>
#include <CGeomScene3D.h>
#include <CGeomObject3D.h>
#include <CGeomTexture.h>
#include <CQImage.h>
#include <QPainter>

CQCamera3DTextures::
CQCamera3DTextures(CQCamera3DApp *app) :
 app_(app)
{
  auto *canvas = app_->canvas();

  connect(canvas, SIGNAL(stateChanged()), this, SLOT(update()));
}

void
CQCamera3DTextures::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.fillRect(rect(), bgColor_);

  int w = width ();
  int h = height();

  //---

  auto *canvas = app_->canvas();

  const auto &textures = canvas->glTextures();

  auto n = textures.size();
  if (n == 0) return;

  int nr = std::max(int(sqrt(n)), 1);
  int nc = (n + nr - 1)/nr;

  auto iw = double(w)/nc;
  auto ih = double(h)/nr;

  int ir = 0;
  int ic = 0;

  for (const auto &pt : textures) {
    const auto &textureData = pt.second;

    auto *texture = textureData.geomTexture;

    auto *qimage = dynamic_cast<CQImage *>(texture->image()->image().get());

    auto qimage1 = qimage->getQImage().scaled(int(iw), int(ih),
      Qt::KeepAspectRatio, Qt::FastTransformation);

    auto x = ic*iw;
    auto y = ir*ih;

    painter.drawImage(int(x), int(y), qimage1);

    iw = qimage1.width ();
    ih = qimage1.height();

    ++ic;

    if (ic >= nc) {
      ++ir;

      ic = 0;
    }
  }
}
