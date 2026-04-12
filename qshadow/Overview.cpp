#include <Overview.h>
#include <Canvas.h>
#include <Camera.h>
#include <Light.h>
#include <Status.h>
#include <App.h>
#include <GeomObject.h>

#include <CQGLBuffer.h>

#include <CQPixmapCache.h>
#include <CGeomScene3D.h>
#include <CGeomObject3D.h>

#include <QPainter>
#include <QMouseEvent>

namespace CQShadow3D {

Overview::
Overview(App *app) :
 app_(app)
{
  setObjectName("overview");

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  setFocusPolicy(Qt::StrongFocus);

  setMouseTracking(true);

  //---

  lightPixmap_ = CQPixmapCacheInst->getSizedPixmap("LIGHT", QSize(32, 32));

  //---

  xview_.ind = 0; xview_.range = new CDisplayRange2D; xview_.type = ViewType::XY;
  yview_.ind = 1; yview_.range = new CDisplayRange2D; yview_.type = ViewType::ZY;
  zview_.ind = 2; zview_.range = new CDisplayRange2D; zview_.type = ViewType::XZ;
  pview_.ind = 3; pview_.range = new CDisplayRange2D; pview_.type = ViewType::THREED;

  views_.push_back(&xview_);
  views_.push_back(&yview_);
  views_.push_back(&zview_);
  views_.push_back(&pview_);

  views2d_.push_back(&xview_);
  views2d_.push_back(&yview_);
  views2d_.push_back(&zview_);

  //---

  auto *canvas = app_->canvas();

  auto *camera = canvas->camera();
  auto *light  = canvas->light();

  connect(dynamic_cast<Camera *>(camera), SIGNAL(stateChangedSignal()), this, SLOT(invalidate()));
  connect(light, SIGNAL(stateChangedSignal()), this, SLOT(invalidate()));

  setEditType(editType_);
}

Overview::
~Overview()
{
}

void
Overview::
setEditType(const EditType &t)
{
  editType_ = t;

  QString tipStr;

  if      (editType_ == EditType::CAMERA) {
    tipStr = QString("Camera: Shift=Position, Control=Origin");
  }
  else if (editType_ == EditType::LIGHT) {
    tipStr = QString("Light: Shift=Position, Control=Origin, Alt=Direction");
  }

  app_->status()->setTipLabel(tipStr);

  update();
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
  auto zoomFactor = xview_.range->zoomFactor();

  w_ = width ();
  h_ = height();

  xview_.range->setPixelRange(   0,    0,   w_/2,   h_/2); // XY
  yview_.range->setPixelRange(w_/2,    0, w_ - 1,   h_/2); // ZY
  zview_.range->setPixelRange(   0, h_/2,   w_/2, h_ - 1); // XZ
  pview_.range->setPixelRange(w_/2, h_/2, w_ - 1, h_ - 1); // 3D

  for (auto *view : views2d_)
    view->range->setEqualScale(true);

  for (auto *v : views_)
    v->range->zoomOut(zoomFactor);

  invalidate();
}

void
Overview::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  drawData_.painter = &painter;

  //---

  auto *canvas = app_->canvas();

  auto *camera = dynamic_cast<Camera *>(canvas->camera());
  auto *light  = canvas->light();

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

  // draw view borders (sets view rect)
  painter.setPen(Qt::black);
  painter.setBrush(Qt::NoBrush);

  for (auto *view : views_)
    drawPixelBorder(*view);

  //---

  painter.save();

  drawModel();

  painter.restore();

  //---

  if (isShowCamera() || editType() == EditType::CAMERA) {
    painter.save();

    drawCamera(camera);

    painter.restore();
  }

  //---

  if (isShowLight() || editType() == EditType::LIGHT) {
    painter.save();

    drawLight(light);

    painter.restore();
  }
}

void
Overview::
updateModel()
{
  drawData_.bbox = CBBox3D();

  drawData_.faces.clear();

  auto *scene = app_->scene();

  for (auto *object : scene->getObjects()) {
    auto *object1 = dynamic_cast<GeomObject *>(object);
    assert(object1);

    auto modelMatrix = CMatrix3DH(object->getHierTransform());

    //---

    const auto &faceDatas = object1->faceDatas();
    auto       *buffer    = object1->buffer();

    for (const auto &faceData : faceDatas) {
      Face face;

      face.modelMatrix = modelMatrix;

      for (int i = 0; i < faceData.len; ++i) {
        CQGLBuffer::PointData data;
        buffer->getPointData(faceData.pos + i, data);

        face.points.push_back(data.point->point());

        auto p = modelMatrix*data.point->point();

        drawData_.bbox += p;
      }

      drawData_.faces.push_back(face);
    }
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

  xs_ = bbox_.getXSize();
  ys_ = bbox_.getYSize();
  zs_ = bbox_.getZSize();

  bbox_ += CPoint3D(-3, -3, -3);
  bbox_ += CPoint3D( 3,  3,  3);

  xview_.range->setWindowRange(bbox_.getXMin(), bbox_.getYMin(), bbox_.getXMax(), bbox_.getYMax());
  yview_.range->setWindowRange(bbox_.getZMax(), bbox_.getYMin(), bbox_.getZMin(), bbox_.getYMax());
  zview_.range->setWindowRange(bbox_.getXMin(), bbox_.getZMax(), bbox_.getXMax(), bbox_.getZMin());

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

  auto drawPolygon = [&](const CMatrix3DH &modelMatrix, const std::vector<CPoint3D> &points) {
    std::vector<CPoint2D> xpoints, ypoints, zpoints, ppoints;

    for (const auto &p : points) {
      auto p1 = modelMatrix*p;

      xpoints.push_back(CPoint2D(p1.getX(), p1.getY())); // XY
      ypoints.push_back(CPoint2D(p1.getZ(), p1.getY())); // ZY
      zpoints.push_back(CPoint2D(p1.getX(), p1.getZ())); // XZ

      auto p2 = drawData_.projectionMatrix*drawData_.viewMatrix*p1;

      ppoints.push_back(CPoint2D(p2.getX(), p2.getY()));
    }

    drawPolygon2D(xview_, xpoints);
    drawPolygon2D(yview_, ypoints);
    drawPolygon2D(zview_, zpoints);
    drawPolygon2D(pview_, ppoints);
  };

  for (auto &face : drawData_.faces) {
    drawData_.painter->setPen(Qt::black);

    drawData_.painter->setBrush(Qt::NoBrush);

    drawPolygon(face.modelMatrix, face.points);
  }
}

void
Overview::
drawCamera(Camera *camera)
{
  CameraShape shape;
  getCameraShape(camera, shape);

  auto pos    = camera->position();
  auto origin = camera->origin  ();

  //---

  drawData_.painter->setPen(Qt::black);
  drawData_.painter->setBrush(Qt::NoBrush);

  //---

  // draw camera position, origin and direction vectors
  auto drawCameraVector = [&](const CVector3D &v, const QColor &c, const QString &text) {
    drawData_.painter->setPen(c);
    drawVector(pos, v, text);
  };

  auto front = camera->front();
  auto up    = camera->up   ();
  auto right = camera->right();

  drawPoint(pos   , "P");
  drawPoint(origin, "O");

  drawCameraVector(front, QColor(255, 0, 0, 255), "F");
  drawCameraVector(up,    QColor(0, 255, 0, 255), "U");
  drawCameraVector(right, QColor(0, 0, 255, 255), "R");

  //---

  // draw view frustrum
  drawData_.painter->setPen(Qt::black);

  drawLine(shape.p11, shape.p12, "");
  drawLine(shape.p12, shape.p22, "");
  drawLine(shape.p22, shape.p21, "");
  drawLine(shape.p21, shape.p11, "");

  drawLine(pos.point(), shape.p11, "");
  drawLine(pos.point(), shape.p12, "");
  drawLine(pos.point(), shape.p22, "");
  drawLine(pos.point(), shape.p21, "");

  //---

#if 0
  // draw camera orbit
  drawData_.painter->setPen(QColor(100, 100, 100));

  drawSphere(origin.point(), pos.point());
#endif
}

void
Overview::
getCameraShape(Camera *camera, CameraShape &shape) const
{
  auto pos    = camera->position();
  auto origin = camera->origin();

  auto dist = CVector3D(pos, origin).length();

  auto front = camera->front();
  auto up    = camera->up   ();
  auto right = camera->right();

  auto fov = CMathGen::DegToRad(camera->fov());

  auto m1 = CMatrix3D::rotation(-fov/2.0, up   );
  auto m2 = CMatrix3D::rotation( fov/2.0, up   );
  auto m3 = CMatrix3D::rotation(-fov/2.0, right);
  auto m4 = CMatrix3D::rotation( fov/2.0, right);

  auto front1 = m1*front;
  auto front2 = m2*front;

  auto front11 = m3*front1;
  auto front12 = m4*front1;
  auto front21 = m3*front2;
  auto front22 = m4*front2;

  shape.p11 = (pos + dist*front11).point();
  shape.p12 = (pos + dist*front12).point();
  shape.p21 = (pos + dist*front21).point();
  shape.p22 = (pos + dist*front22).point();
}

void
Overview::
drawLight(Light *light)
{
  drawData_.painter->setPen(QColor(0, 0, 0, 255));
  drawData_.painter->setBrush(Qt::NoBrush);

  // draw lights
  auto p = light->getPosition();

  drawPixmap(p, lightPixmap_);

  if      (light->getType() == CGeomLight3DType::DIRECTIONAL) {
    auto d = light->getDirection();

    drawVector(CVector3D(p), d, "D");
  }
  else if (light->getType() == CGeomLight3DType::POINT) {
    auto r = light->getPointRadius();

    drawCircle(p, r, "P");
  }
  else if (light->getType() == CGeomLight3DType::SPOT) {
    auto d = light->getSpotDirection();
    auto a = light->getSpotCutOffAngle();

    auto a1 = CMathGen::DegToRad(a)/2.0;

    drawCone(CVector3D(p), d, a1);
  }
  else if (light->getType() == CGeomLight3DType::FLASHLIGHT) {
    // TODO
  }
}

void
Overview::
drawLine(const CPoint3D &p1, const CPoint3D &p2, const QString &) const
{
  auto px1 = windowToPixelX(CPoint2D(p1.x, p1.y));
  auto px2 = windowToPixelX(CPoint2D(p2.x, p2.y));

  drawData_.painter->setClipRect(xview_.rect);
  drawData_.painter->drawLine(px1.x, px1.y, px2.x, px2.y);

  auto py1 = windowToPixelY(CPoint2D(p1.z, p1.y));
  auto py2 = windowToPixelY(CPoint2D(p2.z, p2.y));

  drawData_.painter->setClipRect(yview_.rect);
  drawData_.painter->drawLine(py1.x, py1.y, py2.x, py2.y);

  auto pz1 = windowToPixelZ(CPoint2D(p1.x, p1.z));
  auto pz2 = windowToPixelZ(CPoint2D(p2.x, p2.z));

  drawData_.painter->setClipRect(zview_.rect);
  drawData_.painter->drawLine(pz1.x, pz1.y, pz2.x, pz2.y);

  auto pv1 = drawData_.projectionMatrix*drawData_.viewMatrix*p1;
  auto pv2 = drawData_.projectionMatrix*drawData_.viewMatrix*p2;

  auto pp1 = windowToPixelP(CPoint2D(pv1.x, pv1.y));
  auto pp2 = windowToPixelP(CPoint2D(pv2.x, pv2.y));

  drawData_.painter->setClipRect(pview_.rect);
  drawData_.painter->drawLine(pp1.x, pp1.y, pp2.x, pp2.y);
}

void
Overview::
drawVector(const CVector3D &p, const CVector3D &d, const QString &label) const
{
  auto drawVector2D = [&](const ViewData &view, const CPoint2D &p,
                          const CPoint2D &d, double sx, double sy, const QString &label) {
    drawData_.painter->setClipRect(view.rect);

    auto s = std::sqrt(sx*sx + sy*sy)/3.0;

    double px1, py1;
    view.range->windowToPixel(p.x, p.y, &px1, &py1);
    double px2, py2;
    view.range->windowToPixel(p.x + s*d.x, p.y + s*d.y, &px2, &py2);

    drawData_.painter->drawLine(px1, py1, px2, py2);

    if (label != "")
      drawData_.painter->drawText(px2, py2, label);
  };

  auto x1 = p.getX(), y1 = p.getY(), z1 = p.getZ();
  auto x2 = d.getX(), y2 = d.getY(), z2 = d.getZ();

  auto xs = drawData_.bbox.getXSize();
  auto ys = drawData_.bbox.getYSize();
  auto zs = drawData_.bbox.getZSize();

  drawVector2D(xview_, CPoint2D(x1, y1), CPoint2D(x2, y2), xs, ys, label); // XY
  drawVector2D(yview_, CPoint2D(z1, y1), CPoint2D(z2, y2), zs, ys, label); // ZY
  drawVector2D(zview_, CPoint2D(x1, z1), CPoint2D(x2, z2), xs, zs, label); // XZ
}

void
Overview::
drawCone(const CVector3D &p, const CVector3D &d, double a) const
{
  auto m1 = CMatrix2D::rotation( a);
  auto m2 = CMatrix2D::rotation(-a);

  auto drawCone2D = [&](const ViewData &view, const CVector3D &p,
                        const CVector3D &d, double sx, double sy) {
    auto p1 = view.viewPoint(p.point());
    auto dp = view.viewPoint(d.point());

    drawData_.painter->setClipRect(view.rect);

    auto s = std::sqrt(sx*sx + sy*sy)/3.0;

    auto d1 = m1*dp;
    auto d2 = m2*dp;

    double px1, py1;
    view.range->windowToPixel(p1.x, p1.y, &px1, &py1);
    double px2, py2;
    view.range->windowToPixel(p1.x + s*d1.x, p1.y + s*d1.y, &px2, &py2);
    double px3, py3;
    view.range->windowToPixel(p1.x + s*d2.x, p1.y + s*d2.y, &px3, &py3);

    drawData_.painter->drawLine(px1, py1, px2, py2);
    drawData_.painter->drawLine(px1, py1, px3, py3);
  };

  drawCone2D(xview_, p, d, xs_, ys_); // XY
  drawCone2D(yview_, p, d, zs_, ys_); // ZY
  drawCone2D(zview_, p, d, xs_, zs_); // XZ
}

void
Overview::
drawCircle(const CPoint3D &origin, double r, const QString &label) const
{
  auto drawCircle2D = [&](const ViewData &view, const CPoint2D &o, double r, const QString &label) {
    drawData_.painter->setClipRect(view.rect);

    double px1, py1;
    view.range->windowToPixel(o.x - r, o.y - r, &px1, &py1);
    double px2, py2;
    view.range->windowToPixel(o.x + r, o.y + r, &px2, &py2);

    drawData_.painter->drawEllipse(QRectF(px1, py1, px2 - px1, py2 - py1));

    if (label != "")
      drawData_.painter->drawText(px2, py2, label);
  };

  auto xo = CPoint2D(origin.x, origin.y);
  auto yo = CPoint2D(origin.z, origin.y);
  auto zo = CPoint2D(origin.x, origin.z);

  drawCircle2D(xview_, xo, r, label); // XY
  drawCircle2D(yview_, yo, r, label); // ZY
  drawCircle2D(zview_, zo, r, label); // XZ
}

void
Overview::
drawPoint(const CVector3D &v, const QString &text) const
{
  drawPoint(v.point(), text);
}

void
Overview::
drawPoint(const CPoint3D &p, const QString &text) const
{
  double s = 8.0;

  drawData_.painter->setBrush(Qt::red);

  auto px = windowToPixelX(CPoint2D(p.x, p.y));
  auto py = windowToPixelY(CPoint2D(p.z, p.y));
  auto pz = windowToPixelZ(CPoint2D(p.x, p.z));

  drawData_.painter->setClipRect(xview_.rect);
  drawData_.painter->drawEllipse(QRectF(px.x - s/2, px.y - s/2, s, s));
  drawData_.painter->drawText(px.x, px.y, text);

  drawData_.painter->setClipRect(yview_.rect);
  drawData_.painter->drawEllipse(QRectF(py.x - s/2, py.y - s/2, s, s));
  drawData_.painter->drawText(py.x, py.y, text);

  drawData_.painter->setClipRect(zview_.rect);
  drawData_.painter->drawEllipse(QRectF(pz.x - s/2, pz.y - s/2, s, s));
  drawData_.painter->drawText(pz.x, pz.y, text);

  auto pv = drawData_.projectionMatrix*drawData_.viewMatrix*p;
  auto pp = windowToPixelP(CPoint2D(pv.x, pv.y));

  drawData_.painter->setClipRect(pview_.rect);
  drawData_.painter->drawEllipse(QRectF(pp.x - s/2, pp.y - s/2, s, s));
  drawData_.painter->drawText(pp.x, pp.y, text);
}

void
Overview::
drawPixmap(const CPoint3D &p, const QPixmap &pixmap) const
{
  auto s = pixmap.width();

  auto drawPixmap2D = [&](const ViewData &view, const CPoint2D &p, const QPixmap &pixmap) {
    drawData_.painter->setClipRect(view.rect);

    double px, py;
    view.range->windowToPixel(p.x, p.y, &px, &py);

    drawData_.painter->drawPixmap(px - s/2, py - s/2, pixmap);
  };

  drawPixmap2D(xview_, CPoint2D(p.getX(), p.getY()), pixmap); // XY
  drawPixmap2D(yview_, CPoint2D(p.getZ(), p.getY()), pixmap); // ZY
  drawPixmap2D(zview_, CPoint2D(p.getX(), p.getZ()), pixmap); // XZ
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
  mouseData_.pressed  = true;
  mouseData_.button   = e->button();

  mouseData_.isShift   = (e->modifiers() & Qt::ShiftModifier);
  mouseData_.isControl = (e->modifiers() & Qt::ControlModifier);
  mouseData_.isAlt     = (e->modifiers() & Qt::AltModifier);

  mouseData_.pressPos = CPoint2D(e->x(), e->y());
  mouseData_.movePos  = mouseData_.pressPos;

  //---

  int x = e->x();
  int y = e->y();

  if      (mouseData_.button == Qt::LeftButton) {
    if      (editType_ == EditType::SELECT) {
#if 0
      rubberBand_->setBounds(mouseData_.pressPixel, mouseData_.movePixel1);
      rubberBand_->show();
#endif
    }
    else if (editType_ == EditType::CAMERA) {
      if      (mouseData_.isShift)
        setCameraPosition(x, y);
      else if (mouseData_.isControl)
        setCameraOrigin(x, y);
    }
    else if (editType() == EditType::LIGHT) {
      if      (mouseData_.isShift)
        setLightPosition(x, y);
      else if (mouseData_.isControl)
        setLightOrigin(x, y);
      else if (mouseData_.isAlt)
        setLightDirection(x, y);
    }
  }
  else if (mouseData_.button == Qt::MiddleButton) {
    if      (editType_ == EditType::SELECT) {
    }
    else if (editType_ == EditType::CAMERA) {
      setCameraOrigin(x, y);
    }
    else if (editType_ == EditType::LIGHT) {
      setLightOrigin(x, y);
    }
  }
  else if (mouseData_.button == Qt::RightButton) {
  }
}

void
Overview::
mouseMoveEvent(QMouseEvent *e)
{
  mouseData_.movePos = CPoint2D(e->x(), e->y());

#if 0
  CPoint2D p;
  if (xview_.pressRange(mouseData_.movePos, p))
    app_->status()->setText(QString("X=%1 Y=%2").arg(p.x).arg(p.y));
  if (yview_.pressRange(mouseData_.movePos, p))
    app_->status()->setText(QString("Z=%1 Y=%2").arg(p.x).arg(p.y));
  if (zview_.pressRange(mouseData_.movePos, p))
    app_->status()->setText(QString("X=%1 Z=%2").arg(p.x).arg(p.y));
#endif

  //---

  int x = e->x();
  int y = e->y();

  if (mouseData_.pressed) {
    if      (mouseData_.button == Qt::LeftButton) {
      if      (editType_ == EditType::SELECT) {
#if 0
        rubberBand_->setBounds(mouseData_.pressPixel, mouseData_.movePixel2);
#endif
      }
      else if (editType_ == EditType::CAMERA) {
        if      (mouseData_.isShift)
          setCameraPosition(x, y);
        else if (mouseData_.isControl)
          setCameraOrigin(x, y);
      }
      else if (editType_ == EditType::LIGHT) {
        if      (mouseData_.isShift)
          setLightPosition(x, y);
        else if (mouseData_.isControl)
          setLightOrigin(x, y);
        else if (mouseData_.isAlt)
          setLightDirection(x, y);
      }
    }
    else if (mouseData_.button == Qt::MiddleButton) {
      if      (editType_ == EditType::SELECT) {
      }
      else if (editType_ == EditType::CAMERA) {
        setCameraOrigin(x, y);
      }
      else if (editType_ == EditType::LIGHT) {
        setLightOrigin(x, y);
      }
    }
    else if (mouseData_.button == Qt::RightButton) {
    }
  }

  auto xy = CPoint2D(x, y);

  int ind = ind_;

  ind_ = -1;

  CPoint2D p;
  if (xview_.pressRange(xy, p)) { ind_ = xview_.ind; /*showPoint(CPoint3D(p.x, p.y, 0.0));*/ } // XY
  if (yview_.pressRange(xy, p)) { ind_ = yview_.ind; /*showPoint(CPoint3D(0.0, p.y, p.x));*/ } // ZY
  if (zview_.pressRange(xy, p)) { ind_ = zview_.ind; /*showPoint(CPoint3D(p.x, 0.0, p.y));*/ } // XZ

  if (! mouseData_.pressed) {
    if (ind_ != ind)
      invalidate();
  }
}

void
Overview::
mouseReleaseEvent(QMouseEvent *)
{
  mouseData_.pressed = false;
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

  invalidate();
}

void
Overview::
keyPressEvent(QKeyEvent *e)
{
  auto key = e->key();

  auto *canvas = app_->canvas();
  auto *camera = canvas->camera();

  auto k = e->key();

  mouseData_.isShift   = (e->modifiers() & Qt::ShiftModifier);
  mouseData_.isControl = (e->modifiers() & Qt::ControlModifier);

  auto bbox = canvas->bbox();

  auto d = bbox.getMaxSize()/100.0;

  if      (editType() == EditType::CAMERA) {
    if      (k == Qt::Key_A) {
      if      (ind_ == 0)
        camera->moveAroundZ(-3*d);
      else if (ind_ == 1)
        camera->moveAroundX(-3*d);
      else if (ind_ == 2)
        camera->moveAroundY(-3*d);
    }
    else if (k == Qt::Key_D) {
      if      (ind_ == 0)
        camera->moveAroundZ(3*d);
      else if (ind_ == 1)
        camera->moveAroundX(3*d);
      else if (ind_ == 2)
        camera->moveAroundY(3*d);
    }
  }
  else if (editType() == EditType::LIGHT) {
  }

  if      (k == Qt::Key_Plus) {
    for (auto *v : views_)
      v->range->zoomIn(1.1);
  }
  else if (k == Qt::Key_Minus) {
    for (auto *v : views_)
      v->range->zoomOut(1.1);
  }
  else if (k == Qt::Key_Home) {
    for (auto *v : views_)
      v->range->reset();

    rangeValid_ = false;
  }
#if 0
  else if (k == Qt::Key_1) {
    drawData_.pointLabels[0].show = ! drawData_.pointLabels[0].show;
  }
  else if (k == Qt::Key_2) {
    drawData_.pointLabels[1].show = ! drawData_.pointLabels[1].show;
  }
  else if (k == Qt::Key_3) {
    drawData_.pointLabels[2].show = ! drawData_.pointLabels[2].show;
  }
  else if (k == Qt::Key_4) {
    drawData_.pointLabels[3].show = ! drawData_.pointLabels[3].show;
  }
  else if (k == Qt::Key_5) {
    drawData_.pointLabels[4].show = ! drawData_.pointLabels[4].show;
  }
#endif
  if (key == Qt::Key_R) {
    rangeValid_ = false;
    updateRange();
  }

  canvas->update();

  invalidate();
}

void
Overview::
setCameraPosition(int x, int y)
{
  auto *canvas = app_->canvas();
  auto *camera = canvas->camera();

  auto pos = camera->position();

  auto xy = CPoint2D(x, y);

  CPoint2D p;

  if (xview_.pressRange(xy, p)) camera->setPosition(CVector3D(p.x, p.y, pos.z())); // XY
  if (yview_.pressRange(xy, p)) camera->setPosition(CVector3D(pos.x(), p.y, p.x)); // ZY
  if (zview_.pressRange(xy, p)) camera->setPosition(CVector3D(p.x, pos.y(), p.y)); // XZ

  invalidate();
}

void
Overview::
setCameraOrigin(int x, int y)
{
  auto *canvas = app_->canvas();
  auto *camera = canvas->camera();

  auto origin = camera->origin();

  auto xy = CPoint2D(x, y);

  CPoint2D p;

  if (xview_.pressRange(xy, p)) camera->setOrigin(CVector3D(p.x, p.y, origin.z())); // XY
  if (yview_.pressRange(xy, p)) camera->setOrigin(CVector3D(origin.x(), p.y, p.x)); // ZY
  if (zview_.pressRange(xy, p)) camera->setOrigin(CVector3D(p.x, origin.y(), p.y)); // XZ

  invalidate();
}

void
Overview::
setLightPosition(int x, int y)
{
  auto *canvas = app_->canvas();
  auto *light  = canvas->light();

  auto pos = light->position();

  auto xy = CPoint2D(x, y);

  CPoint2D p;

  if (xview_.pressRange(xy, p)) light->setPosition(CPoint3D(p.x, p.y, pos.z())); // XY
  if (yview_.pressRange(xy, p)) light->setPosition(CPoint3D(pos.x(), p.y, p.x)); // ZY
  if (zview_.pressRange(xy, p)) light->setPosition(CPoint3D(p.x, pos.y(), p.y)); // XZ

  canvas->update();

  invalidate();
}

void
Overview::
setLightOrigin(int x, int y)
{
  auto *canvas = app_->canvas();
  auto *light  = canvas->light();

  auto origin = light->origin();

  auto xy = CPoint2D(x, y);

  CPoint2D p;

  if (xview_.pressRange(xy, p)) light->setOrigin(CVector3D(p.x, p.y, origin.z())); // XY
  if (yview_.pressRange(xy, p)) light->setOrigin(CVector3D(origin.x(), p.y, p.x)); // ZY
  if (zview_.pressRange(xy, p)) light->setOrigin(CVector3D(p.x, origin.y(), p.y)); // XZ

  invalidate();
}

void
Overview::
setLightDirection(int x, int y)
{
  auto *canvas = app_->canvas();
  auto *light  = canvas->light();

  auto pos = CVector3D(light->getPosition());

  CVector3D dir { 1, 0, 0 };

  if      (light->getType() == CGeomLight3DType::DIRECTIONAL)
    dir = light->getDirection();
  else if (light->getType() == CGeomLight3DType::SPOT)
    dir = light->getSpotDirection();
  else if (light->getType() == CGeomLight3DType::FLASHLIGHT)
    dir = light->getSpotDirection();
  else
    return;

  auto pos1 = pos + dir;

  auto xy = CPoint2D(x, y);

  CPoint2D p;

  if (xview_.pressRange(xy, p)) pos1 = CPoint3D(p.x, p.y, pos1.z()); // XY
  if (yview_.pressRange(xy, p)) pos1 = CPoint3D(pos1.x(), p.y, p.x); // ZY
  if (zview_.pressRange(xy, p)) pos1 = CPoint3D(p.x, pos1.y(), p.y); // XZ

  auto dir1 = (pos1 - pos).normalized();

  if      (light->getType() == CGeomLight3DType::DIRECTIONAL) {
    light->setDirection(dir1);
  }
  else if (light->getType() == CGeomLight3DType::SPOT) {
    light->setSpotDirection(dir1);
  }
  else if (light->getType() == CGeomLight3DType::FLASHLIGHT) {
    light->setSpotDirection(dir1);
  }

  canvas->update();

  invalidate();
}

}
