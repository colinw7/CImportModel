#include <CQCamera3DUVMap.h>
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

CQCamera3DUVMap::
CQCamera3DUVMap(CQCamera3DApp *app) :
 app_(app)
{
  auto *canvas = app_->canvas();

  connect(canvas, SIGNAL(stateChanged()), this, SLOT(update()));
}

void
CQCamera3DUVMap::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.fillRect(rect(), bgColor_);

  int iw = width ();
  int ih = height();

  //---

  auto *canvas = app_->canvas();

  auto *object = object_;

  if (! object)
    object = canvas->currentObject();

  CGeomFace3D *face = nullptr;

  if (! object)
    face = canvas->currentFace();

  CGeomTexture *texture = nullptr;

  if      (object) {
    if      (textureType_ == TextureType::DIFFUSE)
      texture = object->getDiffuseTexture();
    else if (textureType_ == TextureType::NORMAL)
      texture = object->getNormalTexture();
    else if (textureType_ == TextureType::SPECULAR)
      texture = object->getSpecularTexture();
    else if (textureType_ == TextureType::EMISSIVE)
      texture = object->getEmissiveTexture();
  }
  else if (face) {
    if      (textureType_ == TextureType::DIFFUSE)
      texture = face->getDiffuseTexture();
    else if (textureType_ == TextureType::NORMAL)
      texture = face->getNormalTexture();
    else if (textureType_ == TextureType::SPECULAR)
      texture = face->getSpecularTexture();
    else if (textureType_ == TextureType::EMISSIVE)
      texture = face->getEmissiveTexture();
  }

  if (texture) {
    auto *qimage = dynamic_cast<CQImage *>(texture->image()->image().get());

    auto qimage1 = qimage->getQImage().scaled(width(), height(),
      Qt::KeepAspectRatio, Qt::FastTransformation);

    painter.drawImage(0, 0, qimage1);

    iw = qimage1.width ();
    ih = qimage1.height();
  }

  //---

  painter.setPen(lineColor_);

  //---

  if (object) {
    const auto &faces = object->getFaces();

    for (const auto *face : faces) {
      const auto &vertices = face->getVertices();

      std::vector<QPointF> tpoints;

      int iv = 0;

      for (const auto &v : vertices) {
        auto &vertex = object->getVertex(v);

        auto tpoint = face->getTexturePoint(vertex, iv);

        if (tpoint.x > 1.0)
          tpoint.x -= int(tpoint.x);

        if (tpoint.y > 1.0)
          tpoint.y -= int(tpoint.y);

        auto x1 = tpoint.x*iw;
        auto y1 = tpoint.y*ih;

        tpoints.push_back(QPointF(x1, y1));

        ++iv;
      }

      painter.drawPolygon(&tpoints[0], tpoints.size());
    }
  }
  else if (face) {
    auto *object = face->getObject();

    const auto &vertices = face->getVertices();

    std::vector<QPointF> tpoints;

    int iv = 0;

    for (const auto &v : vertices) {
      auto &vertex = object->getVertex(v);

      auto tpoint = face->getTexturePoint(vertex, iv);

      if (tpoint.x > 1.0)
        tpoint.x -= int(tpoint.x);

      if (tpoint.y > 1.0)
        tpoint.y -= int(tpoint.y);

      auto x1 = tpoint.x*iw;
      auto y1 = tpoint.y*ih;

      tpoints.push_back(QPointF(x1, y1));

      ++iv;
    }

    painter.drawPolygon(&tpoints[0], tpoints.size());
  }
}
