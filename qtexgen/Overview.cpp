#include <Overview.h>
#include <MaterialView.h>
#include <Camera.h>
#include <Light.h>
#include <LightControl.h>
#include <Status.h>
#include <App.h>

#include <Camera.h>
#include <Light.h>
#include <CQGLBuffer.h>

#include <CGeomScene3D.h>
#include <CGeomObject3D.h>

#include <QPainter>
#include <QMouseEvent>

namespace CQTextureGen {

Overview::
Overview(App *app) :
 app_(app)
{
  setObjectName("overview");

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  setFocusPolicy(Qt::StrongFocus);

  setMouseTracking(true);

  //---

  xview_.range = new CDisplayRange2D; xview_.type = ViewType::XY;
  yview_.range = new CDisplayRange2D; yview_.type = ViewType::ZY;
  zview_.range = new CDisplayRange2D; zview_.type = ViewType::XZ;
  pview_.range = new CDisplayRange2D; pview_.type = ViewType::THREED;

  views_.push_back(&xview_);
  views_.push_back(&yview_);
  views_.push_back(&zview_);
  views_.push_back(&pview_);

  views2d_.push_back(&xview_);
  views2d_.push_back(&yview_);
  views2d_.push_back(&zview_);

  //---

  auto *camera = app_->materialView()->camera();

  connect(dynamic_cast<Camera *>(camera), SIGNAL(stateChanged()), this, SLOT(invalidate()));

  //---

 connect(app_->materialView(), SIGNAL(lightAdded()), this, SLOT(invalidate()));
 connect(app_->materialView(), SIGNAL(lightChanged()), this, SLOT(invalidate()));
}

Overview::
~Overview()
{
}

void
Overview::
invalidate()
{
  facesValid_ = false;

  update();
}

void
Overview::
resizeEvent(QResizeEvent *)
{
  w_ = width ();
  h_ = height();

  xview_.range->setPixelRange(0, 0, w_/2, h_/2);
  yview_.range->setPixelRange(w_/2, 0, w_ - 1, h_/2);
  zview_.range->setPixelRange(0, h_/2, w_/2, h_ - 1);
  pview_.range->setPixelRange(w_/2, h_/2, w_ - 1, h_ - 1);
}

void
Overview::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  drawData_.painter = &painter;

  //---

  auto *camera = dynamic_cast<Camera *>(app_->materialView()->camera());

  drawData_.projectionMatrix = camera->perspectiveMatrix();
  drawData_.viewMatrix       = camera->viewMatrix();

  //---

  if (! facesValid_) {
    updateModel();

    facesValid_ = true;
  }

  updateRange();

  //---

  // draw background
  painter.fillRect(rect(), QColor(200, 200, 200));

  //---

  // draw view borders (sets view rect)

  auto drawPixelBorder = [&](ViewData &viewData) {
    painter.setPen(Qt::black);

    QBrush brush(QColor(255, 255, 255));
    painter.setBrush(brush);

    double pxmin, pymin, pxmax, pymax;
    viewData.range->getPixelRange(&pxmin, &pymin, &pxmax, &pymax);

    viewData.rect = QRectF(pxmin, pymin, pxmax - pxmin - 1, pymax - pymin - 1);
    painter.drawRect(viewData.rect);

    painter.setBrush(Qt::NoBrush);
  };

  painter.setPen(Qt::black);
  painter.setBrush(Qt::NoBrush);

  drawPixelBorder(xview_);
  drawPixelBorder(yview_);
  drawPixelBorder(zview_);
  drawPixelBorder(pview_);

  //---

  painter.save();

  drawModel();

  painter.restore();

  //---

  painter.save();

  drawCamera(camera);

  painter.restore();

  //---

  painter.save();

  const auto &lights = app_->materialView()->lights();

  for (auto *light : lights) {
    auto *light1 = dynamic_cast<Light *>(light);

    drawLight(light1);
  }

  painter.restore();
}

void
Overview::
updateModel()
{
  drawData_.bbox = CBBox3D();

  drawData_.faces.clear();

  const auto &faceDataList = app_->materialView()->faceDataList();
  auto       *buffer       = app_->materialView()->buffer();

  for (const auto &faceData : faceDataList.faceDatas) {
    Face face;

    for (int i = 0; i < faceData.len; ++i) {
      CQGLBuffer::PointData data;
      buffer->getPointData(faceData.pos + i, data);

      face.points.push_back(data.point->point());

      drawData_.bbox += data.point->point();
    }

    drawData_.faces.push_back(face);
  }
}

void
Overview::
updateRange()
{
  if (rangeValid_)
    return;

  rangeValid_ = true;

  bbox_ = drawData_.bbox;

  bbox_ += CPoint3D(-3, -3, -3);
  bbox_ += CPoint3D( 3,  3,  3);

  xview_.range->setWindowRange(bbox_.getXMin(), bbox_.getYMin(), bbox_.getXMax(), bbox_.getYMax());
  yview_.range->setWindowRange(bbox_.getZMax(), bbox_.getYMin(), bbox_.getZMin(), bbox_.getYMax());
  zview_.range->setWindowRange(bbox_.getXMin(), bbox_.getZMin(), bbox_.getXMax(), bbox_.getZMax());

  pview_.range->setWindowRange(-1, -1, 1, 1); // 3D

  std::cerr << "Overview BBox: " << bbox_ << "\n";
}

void
Overview::
drawModel()
{
  auto drawPolygon2D = [&](const ViewData &view, const std::vector<CPoint2D> &points) {
    drawData_.painter->setClipRect(view.rect);

    std::vector<QPointF> ppoints;

    for (const auto &p : points) {
      double px, py;
      view.range->windowToPixel(p.x, p.y, &px, &py);

      ppoints.push_back(QPointF(px, py));
    }

    drawData_.painter->drawPolygon(&ppoints[0], ppoints.size());
  };

  auto drawPolygon = [&](const std::vector<CPoint3D> &points) {
    std::vector<CPoint2D> xpoints, ypoints, zpoints, ppoints;

    for (const auto &p : points) {
      xpoints.push_back(CPoint2D(p.getX(), p.getY())); // XY
      ypoints.push_back(CPoint2D(p.getZ(), p.getY())); // ZY
      zpoints.push_back(CPoint2D(p.getX(), p.getZ())); // XZ

      auto p1 = drawData_.projectionMatrix*drawData_.viewMatrix*p;

      ppoints.push_back(CPoint2D(p1.getX(), p1.getY()));
    }

    drawPolygon2D(xview_, xpoints);
    drawPolygon2D(yview_, ypoints);
    drawPolygon2D(zview_, zpoints);
    drawPolygon2D(pview_, ppoints);
  };

  for (auto &face : drawData_.faces) {
    drawData_.painter->setPen(Qt::black);

    drawData_.painter->setBrush(Qt::NoBrush);

    drawPolygon(face.points);
  }
}

void
Overview::
drawCamera(Camera *camera)
{
  drawData_.painter->setPen(Qt::black);
  drawData_.painter->setBrush(Qt::NoBrush);

  //---

  auto p = camera->position();
  auto o = camera->origin  ();

  drawLine(drawData_.painter, p.point(), o.point());

  //---

  drawPoint(drawData_.painter, p.point(), "P");
  drawPoint(drawData_.painter, o.point(), "O");
}

void
Overview::
drawLight(Light *light)
{
  auto p = light->position();

  drawPoint(drawData_.painter, p, QString::fromStdString(light->name()));
}

void
Overview::
drawLine(QPainter *painter, const CPoint3D &p1, const CPoint3D &p2) const
{
  auto px1 = windowToPixelX(CPoint2D(p1.x, p1.y));
  auto px2 = windowToPixelX(CPoint2D(p2.x, p2.y));

  drawData_.painter->setClipRect(xview_.rect);
  painter->drawLine(px1.x, px1.y, px2.x, px2.y);

  auto py1 = windowToPixelY(CPoint2D(p1.z, p1.y));
  auto py2 = windowToPixelY(CPoint2D(p2.z, p2.y));

  drawData_.painter->setClipRect(yview_.rect);
  painter->drawLine(py1.x, py1.y, py2.x, py2.y);

  auto pz1 = windowToPixelZ(CPoint2D(p1.x, p1.z));
  auto pz2 = windowToPixelZ(CPoint2D(p2.x, p2.z));

  drawData_.painter->setClipRect(zview_.rect);
  painter->drawLine(pz1.x, pz1.y, pz2.x, pz2.y);

  auto pv1 = drawData_.projectionMatrix*drawData_.viewMatrix*p1;
  auto pv2 = drawData_.projectionMatrix*drawData_.viewMatrix*p2;

  auto pp1 = windowToPixelP(CPoint2D(pv1.x, pv1.y));
  auto pp2 = windowToPixelP(CPoint2D(pv2.x, pv2.y));

  drawData_.painter->setClipRect(pview_.rect);
  painter->drawLine(pp1.x, pp1.y, pp2.x, pp2.y);
}

void
Overview::
drawPoint(QPainter *painter, const CPoint3D &p, const QString &text) const
{
  double s = 8.0;

  painter->setBrush(Qt::red);

  auto px = windowToPixelX(CPoint2D(p.x, p.y));
  auto py = windowToPixelY(CPoint2D(p.z, p.y));
  auto pz = windowToPixelZ(CPoint2D(p.x, p.z));

  drawData_.painter->setClipRect(xview_.rect);
  painter->drawEllipse(QRectF(px.x - s/2, px.y - s/2, s, s));
  painter->drawText(px.x, px.y, text);

  drawData_.painter->setClipRect(yview_.rect);
  painter->drawEllipse(QRectF(py.x - s/2, py.y - s/2, s, s));
  painter->drawText(py.x, py.y, text);

  drawData_.painter->setClipRect(zview_.rect);
  painter->drawEllipse(QRectF(pz.x - s/2, pz.y - s/2, s, s));
  painter->drawText(pz.x, pz.y, text);

  auto pv = drawData_.projectionMatrix*drawData_.viewMatrix*p;
  auto pp = windowToPixelP(CPoint2D(pv.x, pv.y));

  drawData_.painter->setClipRect(pview_.rect);
  painter->drawEllipse(QRectF(pp.x - s/2, pp.y - s/2, s, s));
  painter->drawText(pp.x, pp.y, text);
}

CPoint2D
Overview::
windowToPixelX(const CPoint2D &w) const
{
  CPoint2D p;
  xview_.range->windowToPixel(w, p);
  return p;
}

CPoint2D
Overview::
windowToPixelY(const CPoint2D &w) const
{
  CPoint2D p;
  yview_.range->windowToPixel(w, p);
  return p;
}

CPoint2D
Overview::
windowToPixelZ(const CPoint2D &w) const
{
  CPoint2D p;
  zview_.range->windowToPixel(w, p);
  return p;
}

CPoint2D
Overview::
windowToPixelP(const CPoint2D &w) const
{
  CPoint2D p;
  pview_.range->windowToPixel(w, p);
  return p;
}

void
Overview::
mousePressEvent(QMouseEvent *e)
{
  mouseData_.pressPos = CPoint2D(e->x(), e->y());
  mouseData_.button   = e->button();
  mouseData_.pressed  = true;

  auto *camera = app_->materialView()->camera();

  if      (mouseData_.button == Qt::LeftButton) {
    if      (editType_ == EditType::SELECT) {
    }
    else if (editType_ == EditType::CAMERA) {
      auto position = camera->position();

      CPoint2D p;
      if (xview_.pressRange(mouseData_.pressPos, p))
        camera->setPosition(CVector3D(p.x, p.y, position.z())); // XY
      if (yview_.pressRange(mouseData_.pressPos, p))
        camera->setPosition(CVector3D(position.x(), p.y, p.x)); // ZY
      if (zview_.pressRange(mouseData_.pressPos, p))
        camera->setPosition(CVector3D(p.x, position.y(), p.y)); // XZ
    }
    else if (editType_ == EditType::LIGHT) {
      auto *light = app_->lightControl()->currentLight();

      auto origin = light->position();

      CPoint2D p;
      if (xview_.pressRange(mouseData_.pressPos, p))
        light->setPosition(CPoint3D(p.x, p.y, origin.z)); // XY
      if (yview_.pressRange(mouseData_.pressPos, p))
        light->setPosition(CPoint3D(origin.x, p.y, p.x)); // ZY
      if (zview_.pressRange(mouseData_.pressPos, p))
        light->setPosition(CPoint3D(p.x, origin.y, p.y)); // XZ
    }
  }
  else if (mouseData_.button == Qt::MiddleButton) {
    if      (editType_ == EditType::SELECT) {
    }
    else if (editType_ == EditType::CAMERA) {
      auto origin = camera->origin();

      CPoint2D p;
      if (xview_.pressRange(mouseData_.pressPos, p))
        camera->setOrigin(CVector3D(p.x, p.y, origin.z())); // XY
      if (yview_.pressRange(mouseData_.pressPos, p))
        camera->setOrigin(CVector3D(origin.x(), p.y, p.x)); // ZY
      if (zview_.pressRange(mouseData_.pressPos, p))
        camera->setOrigin(CVector3D(p.x, origin.y(), p.y)); // XZ
    }
  }
  else if (mouseData_.button == Qt::RightButton) {
  }

  update();
}

void
Overview::
mouseReleaseEvent(QMouseEvent *)
{
  mouseData_.pressed = false;
}

void
Overview::
mouseMoveEvent(QMouseEvent *e)
{
  mouseData_.movePos = CPoint2D(e->x(), e->y());

  auto *camera = app_->materialView()->camera();

  CPoint2D p;
  if (xview_.pressRange(mouseData_.movePos, p))
    app_->status()->setText(QString("X=%1 Y=%2").arg(p.x).arg(p.y));
  if (yview_.pressRange(mouseData_.movePos, p))
    app_->status()->setText(QString("Z=%1 Y=%2").arg(p.x).arg(p.y));
  if (zview_.pressRange(mouseData_.movePos, p))
    app_->status()->setText(QString("X=%1 Z=%2").arg(p.x).arg(p.y));

  //---

  if (! mouseData_.pressed)
    return;

  if      (mouseData_.button == Qt::LeftButton) {
    if      (editType_ == EditType::SELECT) {
    }
    else if (editType_ == EditType::CAMERA) {
      auto position = camera->position();

      CPoint2D p;
      if (xview_.pressRange(mouseData_.movePos, p))
        camera->setPosition(CVector3D(p.x, p.y, position.z())); // XY
      if (yview_.pressRange(mouseData_.movePos, p))
        camera->setPosition(CVector3D(position.x(), p.y, p.x)); // ZY
      if (zview_.pressRange(mouseData_.movePos, p))
        camera->setPosition(CVector3D(p.x, position.y(), p.y)); // XZ
    }
    else if (editType_ == EditType::LIGHT) {
      auto *light = app_->lightControl()->currentLight();

      auto origin = light->position();

      CPoint2D p;
      if (xview_.pressRange(mouseData_.movePos, p))
        light->setPosition(CPoint3D(p.x, p.y, origin.z)); // XY
      if (yview_.pressRange(mouseData_.movePos, p))
        light->setPosition(CPoint3D(origin.x, p.y, p.x)); // ZY
      if (zview_.pressRange(mouseData_.movePos, p))
        light->setPosition(CPoint3D(p.x, origin.y, p.y)); // XZ
    }
  }
  else if (mouseData_.button == Qt::MiddleButton) {
    if      (editType_ == EditType::SELECT) {
    }
    else if (editType_ == EditType::CAMERA) {
      auto origin = camera->origin();

      CPoint2D p;
      if (xview_.pressRange(mouseData_.movePos, p))
        camera->setOrigin(CVector3D(p.x, p.y, origin.z())); // XY
      if (yview_.pressRange(mouseData_.movePos, p))
        camera->setOrigin(CVector3D(origin.x(), p.y, p.x)); // ZY
      if (zview_.pressRange(mouseData_.movePos, p))
        camera->setOrigin(CVector3D(p.x, origin.y(), p.y)); // XZ
    }
  }
  else if (mouseData_.button == Qt::RightButton) {
  }

  update();
}

void
Overview::
wheelEvent(QWheelEvent *e)
{
  auto dw = e->angleDelta().y()/250.0;

  if      (dw < 0) {
    for (auto *v : views2d_)
      v->range->zoomOut(1.05);
  }
  else if (dw > 0) {
    for (auto *v : views2d_)
      v->range->zoomIn(1.05);
  }

  update();
}

void
Overview::
keyPressEvent(QKeyEvent *e)
{
  auto key = e->key();

  if (key == Qt::Key_R) {
    rangeValid_ = false;
    updateRange();
  }

  update();
}

}
