#include <CQCamera3DUVMap.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DShape.h>
#include <CQCamera3DGeomObject.h>
#include <CQCamera3DTexture.h>
#include <CQCamera3DStatus.h>

#include <CQGLBuffer.h>
#include <CGeomScene3D.h>
#include <CGeomObject3D.h>
#include <CQImage.h>

#include <QPainter>
#include <QMouseEvent>

CQCamera3DUVMap::
CQCamera3DUVMap(CQCamera3DApp *app) :
 app_(app)
{
  setObjectName("uvMap");

  auto *canvas = app_->canvas();

  connect(canvas, SIGNAL(stateChanged()), this, SLOT(update()));

  setFocusPolicy(Qt::StrongFocus);

  setMouseTracking(true);
}

void
CQCamera3DUVMap::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.fillRect(rect(), bgColor_);

  w_ = width ();
  h_ = height();

  //---

  auto *canvas = app_->canvas();

  auto *object = object_;

  if (! object) {
    object = canvas->currentObject();

    if (! object)
      object = canvas->currentGeomObject(/*includeRoot*/true);
  }

  CGeomTexture *texture = nullptr;

  auto updateTexture = [&](CGeomTexture *t) {
    if (t) texture = t;
  };

  if (object) {
    if      (textureType_ == TextureType::DIFFUSE ) updateTexture(object->getDiffuseTexture ());
    else if (textureType_ == TextureType::NORMAL  ) updateTexture(object->getNormalTexture  ());
    else if (textureType_ == TextureType::SPECULAR) updateTexture(object->getSpecularTexture());
    else if (textureType_ == TextureType::EMISSIVE) updateTexture(object->getEmissiveTexture());

    auto *objMat = object->getMaterialP();

    if (objMat) {
      if      (textureType_ == TextureType::DIFFUSE ) updateTexture(objMat->diffuseTexture ());
      else if (textureType_ == TextureType::NORMAL  ) updateTexture(objMat->normalTexture  ());
      else if (textureType_ == TextureType::SPECULAR) updateTexture(objMat->specularTexture());
      else if (textureType_ == TextureType::EMISSIVE) updateTexture(objMat->emissiveTexture());
    }
  }

  std::vector<CGeomFace3D *> faces;

  if (! texture) {
    auto selectData = canvas->getSelection();

    faces = selectData.faces;
  }

  if (! faces.empty()) {
    object = nullptr;

    for (auto *face : faces) {
      if      (textureType_ == TextureType::DIFFUSE ) updateTexture(face->getDiffuseTexture ());
      else if (textureType_ == TextureType::NORMAL  ) updateTexture(face->getNormalTexture  ());
      else if (textureType_ == TextureType::SPECULAR) updateTexture(face->getSpecularTexture());
      else if (textureType_ == TextureType::EMISSIVE) updateTexture(face->getEmissiveTexture());

      auto *faceMat = face->getMaterialP();

      if (faceMat) {
        if      (textureType_ == TextureType::DIFFUSE ) updateTexture(faceMat->diffuseTexture ());
        else if (textureType_ == TextureType::NORMAL  ) updateTexture(faceMat->normalTexture  ());
        else if (textureType_ == TextureType::SPECULAR) updateTexture(faceMat->specularTexture());
        else if (textureType_ == TextureType::EMISSIVE) updateTexture(faceMat->emissiveTexture());
      }

      if (texture)
        break;
    }
  }

  auto *texture1 = dynamic_cast<CQCamera3DTexture *>(texture);

  //---

  umin_ = 0.0; umax_ = 1.0;
  vmin_ = 0.0; vmax_ = 1.0;

  auto updateFaceTexture = [&](CGeomFace3D *face) {
    auto *object = face->getObject();

    const auto &vertices = face->getVertices();

    int iv = 0;

    for (const auto &v : vertices) {
      auto &vertex = object->getVertex(v);

      auto tpoint = face->getTexturePoint(vertex, iv);

      umin_ = std::min(umin_, tpoint.x);
      umax_ = std::max(umax_, tpoint.x);

      vmin_ = std::min(vmin_, tpoint.y);
      vmax_ = std::max(vmax_, tpoint.y);

      ++iv;
    }
  };

  double s = 1.0;

  if (! isWrapValues()) {
    if      (object) {
      const auto &objectFaces = object->getFaces();

      for (auto *face : objectFaces)
        updateFaceTexture(face);
    }
    else {
      for (auto *face : faces)
        updateFaceTexture(face);
    }

    //---

    double sx = (umax_ - umin_);
    double sy = (vmax_ - vmin_);

    double s = std::max(sx, sy);

    auto umid = (umin_ + umax_)/2.0;
    auto vmid = (vmin_ + vmax_)/2.0;

    umin_ = umid - s/2.0;
    umax_ = umid + s/2.0;

    vmin_ = vmid - s/2.0;
    vmax_ = vmid + s/2.0;
  }

  //---

  if (texture) {
    auto iw1 = width ()/s;
    auto ih1 = height()/s;

    auto *texture1 = dynamic_cast<CQCamera3DTexture *>(texture);

    auto *glTexture = texture1->glTexture(canvas);

    auto qimage = glTexture->getImage();

    if (! qimage.isNull()) {
      auto qimage1 = qimage.scaled(iw1, ih1,
        /*Qt::KeepAspectRatio*/Qt::IgnoreAspectRatio, Qt::FastTransformation);

      auto px = mapU(0.0);
      auto py = mapV(0.0);

      painter.drawImage(px, py, qimage1);
    }
  }

  //---

  texturePoints_.clear();

  painter.setPen(lineColor_);

  //---

  auto fixTValue = [&](double &t, double min, double max) {
    if (texture1 && ! texture1->isWrapped()) {
      t = std::min(std::max(t, min), max);
    }
    else {
      while (t > max)
        t -= (max - min);
      while (t < min)
        t += (max - min);
    }
  };

  auto fixUValue = [&](double &t) { fixTValue(t, umin_, umax_); };
  auto fixVValue = [&](double &t) { fixTValue(t, vmin_, vmax_); };

  auto drawFaceTexture = [&](CGeomFace3D *face) {
    auto *object = face->getObject();

    TexturePoint* selectedTexturePoint { nullptr };

    std::vector<QPointF> tpoints;

    int iv = 0;

    const auto &vertices = face->getVertices();

    for (const auto &v : vertices) {
      auto &vertex = object->getVertex(v);

      TexturePoint texturePoint;

      texturePoint.face   = face;
      texturePoint.ind    = iv;
      texturePoint.tpoint = face->getTexturePoint(vertex, iv);

      fixUValue(texturePoint.tpoint.x);
      fixVValue(texturePoint.tpoint.y);

      auto x1 = mapU(texturePoint.tpoint.x);
      auto y1 = mapV(texturePoint.tpoint.y);

      texturePoint.p = QPointF(x1, y1);

      tpoints.push_back(texturePoint.p);

      texturePoints_.push_back(texturePoint);

      if (face == selectedFace_ && iv == selectedInd_)
        selectedTexturePoint = &texturePoints_.back();

      ++iv;
    }

    painter.drawPolygon(&tpoints[0], tpoints.size());

    if (selectedTexturePoint) {
      int pw = 6;

      painter.setPen(Qt::yellow);

      painter.drawLine(selectedTexturePoint->p + QPointF(-pw, 0),
                       selectedTexturePoint->p + QPointF( pw, 0));
      painter.drawLine(selectedTexturePoint->p + QPointF(0, -pw),
                       selectedTexturePoint->p + QPointF(0,  pw));

      painter.setPen(lineColor_);
    }
  };

  if (object) {
    const auto &objectFaces = object->getFaces();

    for (auto *face : objectFaces) {
      if (! isWrapValues())
        updateFaceTexture(face);
    }

    for (auto *face : objectFaces)
      drawFaceTexture(face);
  }
  else {
    for (auto *face : faces) {
      if (! isWrapValues())
        updateFaceTexture(face);

      drawFaceTexture(face);
    }
  }
}

void
CQCamera3DUVMap::
mousePressEvent(QMouseEvent *e)
{
  auto pos = e->pos();

  auto p = CPoint2D(unmapU(pos.x()), unmapV(pos.y()));

  double minD;
  int    minI = -1;

  int i = 0;

  for (const auto &tp : texturePoints_) {
    auto d = tp.tpoint.distanceTo(p);

    if (minI < 0 || d < minD) {
      minI = i;
      minD = d;
    }

    ++i;
  }

  if (minI < 0)
    return;

  selectedFace_ = texturePoints_[minI].face;
  selectedInd_  = texturePoints_[minI].ind;

  update();
}

void
CQCamera3DUVMap::
mouseMoveEvent(QMouseEvent *e)
{
  auto pos = e->pos();

  auto x = unmapU(pos.x());
  auto y = unmapV(pos.y());

  app_->status()->setMouseLabel(QString("%1 %2").arg(x).arg(y));
}
