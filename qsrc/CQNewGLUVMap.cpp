#include <CQNewGLUVMap.h>
#include <CQNewGLModel.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLShape.h>
#include <CQGLBuffer.h>
#include <CImportBase.h>
#include <CGeomScene3D.h>
#include <CGeomObject3D.h>
#include <CGeomTexture.h>
#include <CQImage.h>
#include <QPainter>

CQNewGLUVMap::
CQNewGLUVMap(CQNewGLModel *model) :
 model_(model)
{
}

void
CQNewGLUVMap::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.fillRect(rect(), bgColor_);

  int iw = width ();
  int ih = height();

  //---

  if (object_) {
    CGeomTexture *texture = nullptr;

    if      (textureType_ == TextureType::DIFFUSE)
      texture = object_->getDiffuseTexture();
    else if (textureType_ == TextureType::NORMAL)
      texture = object_->getNormalTexture();
    else if (textureType_ == TextureType::SPECULAR)
      texture = object_->getSpecularTexture();
    else if (textureType_ == TextureType::EMISSIVE)
      texture = object_->getEmissiveTexture();

    if (texture) {
      auto *qimage = dynamic_cast<CQImage *>(texture->image()->image().get());

      auto qimage1 = qimage->getQImage().scaled(width(), height(),
        Qt::KeepAspectRatio, Qt::FastTransformation);

      painter.drawImage(0, 0, qimage1);

      iw = qimage1.width ();
      ih = qimage1.height();
    }
  }

  //---

  painter.setPen(lineColor_);

  //---

  if (object_) {
    const auto &faces = object_->getFaces();

    for (const auto *face : faces) {
      const auto &vertices = face->getVertices();

      std::vector<QPointF> tpoints;

      int iv = 0;

      for (const auto &v : vertices) {
        auto &vertex = object_->getVertex(v);

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

  //---

  auto *canvas = model_->canvas();

  auto *shape = canvas->getCurrentShape();

  if (shape) {
    auto *buffer = shape->buffer();

    const auto &faceDatas = shape->faceDatas();

    for (const auto &faceData : faceDatas) {
      std::vector<QPointF> tpoints;

      for (int ip = 0; ip < faceData.len; ++ip) {
        CQGLBuffer::PointData pointData;

        buffer->getPointData(faceData.pos + ip, pointData);

        auto x1 = pointData.texturePoint.x*iw;
        auto y1 = pointData.texturePoint.y*ih;

        tpoints.push_back(QPointF(x1, y1));
      }

      painter.drawPolygon(&tpoints[0], tpoints.size());
    }
  }
}
