#include <CQCamera3DOverview.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DCamera.h>
#include <CQCamera3DLight.h>
#include <CQCamera3DStatus.h>
#include <CQCamera3DGeomObject.h>
#include <CQCamera3DUtil.h>

#include <CQPixmapCache.h>
#include <CGeomScene3D.h>
#include <CGeomObject3D.h>
#include <CLine3D.h>
#include <CMatrix2D.h>

#include <QMouseEvent>

namespace {

double polygonArea(const QPolygonF &poly) {
  double area = 0.0;

  int n = poly.size();

  int i1 = n - 1;
  int i2 = 0;

  for ( ; i2 < n; i1 = i2++) {
    const auto &p1 = poly[i1];
    const auto &p2 = poly[i2];

    area += std::abs(p1.x()*p2.y() - p1.y()*p2.x());
  }

  return area;
}

}

//---

CQCamera3DOverview::
CQCamera3DOverview(CQCamera3DApp *app) :
 app_(app)
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  auto *canvas = app_->canvas();
  auto *camera = canvas->currentCamera();

  connect(camera, SIGNAL(stateChanged()), this, SLOT(update()));
  connect(canvas, SIGNAL(stateChanged()), this, SLOT(update()));

  setFocusPolicy(Qt::StrongFocus);

  setMouseTracking(true);

  lightPixmap_ = CQPixmapCacheInst->getSizedPixmap("LIGHT", QSize(32, 32));
}

void
CQCamera3DOverview::
resizeEvent(QResizeEvent *)
{
  updateRange();
}

void
CQCamera3DOverview::
updateRange()
{
#if 0
  int    w = width ();
  double h = height()/3.0;

  int y1 = 0;
  int y2 = y1 + h;
  int y3 = y2 + h;
  int y4 = y3 + h;

  xrange_.setPixelRange(0, y1, w, y2);
  yrange_.setPixelRange(0, y2, w, y3);
  zrange_.setPixelRange(0, y3, w, y4);
#else
  double w = width ()/2.0;
  double h = height()/2.0;

  int x1 = 0, x2 = x1 + w, x3 = x2 + w;
  int y1 = 0, y2 = y1 + h, y3 = y2 + h;

  xrange_.setPixelRange(x1, y1, x2, y2); // XY
  yrange_.setPixelRange(x2, y1, x3, y2); // ZY
  zrange_.setPixelRange(x1, y2, x2, y3); // XZ
  prange_.setPixelRange(x2, y2, x3, y3); // 3D

  xrange_.setEqualScale(isEqualScale());
  yrange_.setEqualScale(isEqualScale());
  zrange_.setEqualScale(isEqualScale());
#endif

  update();
}

void
CQCamera3DOverview::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter_ = &painter;

  //---

  auto *canvas = app_->canvas();
  auto *camera = canvas->currentCamera();

  projectionMatrix_ = CMatrix3DH(camera->perspectiveMatrix());
  viewMatrix_       = CMatrix3DH(camera->viewMatrix());

  //---

  // set window ranges from object bbox
  auto bbox = canvas->bbox();

  auto c = bbox.getCenter();

  xs_ = bbox.getXSize();
  ys_ = bbox.getYSize();
  zs_ = bbox.getZSize();

  xrange_.setWindowRange(c.x - xs_, c.y - ys_, c.x + xs_, c.y + ys_); // XY
  yrange_.setWindowRange(c.z + zs_, c.y - ys_, c.z - zs_, c.y + ys_); // ZY
  zrange_.setWindowRange(c.x - xs_, c.z + zs_, c.x + xs_, c.z - zs_); // XZ
  prange_.setWindowRange(-1, -1, 1, 1); // 3D

  //---

  // draw background
  painter.fillRect(rect(), QColor(240, 240, 240));

  //---

  auto drawPixelBorder = [&](const CDisplayRange2D &range, int ind, QRectF &r) {
    QPen pen;
    pen.setColor(ind == ind_ ? Qt::red : Qt::black);
    pen.setWidthF(ind == ind_ ? 2 : 0);
    painter.setPen(pen);

    QBrush brush(ind == ind_ ? QColor(220, 220, 240) : QColor(255, 255, 255));
    painter.setBrush(brush);

    double pxmin, pymin, pxmax, pymax;
    range.getPixelRange(&pxmin, &pymin, &pxmax, &pymax);

    r = QRectF(pxmin, pymin, pxmax - pxmin - 1, pymax - pymin - 1);
    painter.drawRect(r);

    pen.setWidthF(0);
    painter.setPen(pen);

    painter.setBrush(Qt::NoBrush);
  };

  //---

  // draw border (sets rect values)
  painter.setPen(Qt::black);
  painter.setBrush(Qt::NoBrush);

  drawPixelBorder(xrange_, 0, xrect_);
  drawPixelBorder(yrange_, 1, yrect_);
  drawPixelBorder(zrange_, 2, zrect_);
  drawPixelBorder(prange_, 3, prect_);

  //---

  drawModel();

  //---

  drawCamera();

  //---

  drawLights();

  //---

  // draw labels
  painter.setPen(Qt::black);

  QFontMetrics fm(font());

  auto drawTitle = [&](const CDisplayRange2D &range, const QRectF &rect, const QString &text) {
    painter.setClipRect(rect);

    double pxmin, pymin, pxmax, pymax;
    range.getPixelRange(&pxmin, &pymin, &pxmax, &pymax);

    auto r = QRectF(pxmin + 4, pymin + 4, fm.horizontalAdvance(text) + 4, fm.height() + 4);
    painter.drawText(r, text);
  };

  drawTitle(xrange_, xrect_, "XY");
  drawTitle(yrange_, yrect_, "ZY");
  drawTitle(zrange_, zrect_, "XZ");
}

void
CQCamera3DOverview::
drawModel()
{
  if (modelType() == ModelType::NONE)
    return;

  bool filled = (modelType() == ModelType::SOLID);

  // draw model
  faces_.clear();

  painter_->setPen(QColor(0, 0, 0, 32));
  painter_->setBrush(Qt::NoBrush);

  auto *scene = app_->getScene();

  for (auto *object : scene->getObjects()) {
    if (! object->getVisible())
      continue;

    auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);
    assert(object1);

    modelMatrix_ = CMatrix3DH(object1->getHierTransform());

    auto *objectMaterial = object->getMaterialP();

    //---

    const auto &faces = object->getFaces();

    for (const auto *face : faces) {
      if (! face->getVisible())
        continue;

      faces_.push_back(FaceData());

      auto &faceData = faces_.back();

      faceData.face = const_cast<CGeomFace3D *>(face);

      const auto &vertices = face->getVertices();

      for (const auto &v : vertices) {
        const auto &vertex = object->getVertex(v);
        const auto &model  = vertex.getModel();

        faceData.points.push_back(model);
      }

      //---

      if (filled) {
        auto *faceMaterial = faceData.face->getMaterialP();

        if (! faceMaterial && objectMaterial)
          faceMaterial = objectMaterial;

        auto color = face->color().value_or(CRGBA(1, 1, 1));

        if (faceMaterial && faceMaterial->diffuse())
          color = faceMaterial->diffuse().value();

        painter_->setBrush(RGBAToQColor(color));
      }

      //---

      bool selected = (object->getHierSelected() || face->getSelected());

      if (selected)
        painter_->setPen(QColor(255, 0, 0, 255));

      drawPolygon(faceData.points);

      if (selected)
        painter_->setPen(QColor(0, 0, 0, 32));
    }
  }
}

void
CQCamera3DOverview::
drawCamera()
{
  if (! isCameraVisible())
    return;

  auto *canvas = app_->canvas();
  auto *camera = canvas->currentCamera();

  auto pos    = camera->position();
  auto origin = camera->origin();

  // draw camera position, origin and direction vectors

//auto o = CVector3D(0, 0, 0);

  painter_->setPen(QColor(0, 0, 255, 255));
  painter_->setBrush(Qt::NoBrush);

  auto front = camera->front();
  auto up    = camera->up();
  auto right = camera->right();

  drawPoint(pos   , "P");
  drawPoint(origin, "O");

  drawVector(pos, front, "F");
  drawVector(pos, up   , "U");
  drawVector(pos, right, "R");

  //---

  const auto &eyeLine = canvas->eyeLine();

  if (canvas->isShowEyeLine() && eyeLine.isSet) {
    const auto &xyPlane = canvas->xyPlane();

    CLine3D line(eyeLine.pv1, eyeLine.pv2);
    double t;
    (void) xyPlane.intersectLine(line, &t);
    auto pi = line.point(t);

    drawVector(CVector3D(eyeLine.pv1), CVector3D(pi), "E1");
    drawVector(CVector3D(eyeLine.pv2), CVector3D(pi), "E2");
  }

  //---

  // draw camera orbit

  auto r = CVector3D(origin, pos).length();

  painter_->setPen(Qt::red);

  drawCircle(origin, r);
}

void
CQCamera3DOverview::
drawLights()
{
  if (! isLightsVisible())
    return;

  painter_->setPen(QColor(0, 0, 0, 255));
  painter_->setBrush(Qt::NoBrush);

  auto *canvas = app_->canvas();

  // draw lights
  for (auto *light : canvas->lights()) {
    auto p = light->getPosition();

    drawPixmap(p, lightPixmap_);

    if      (light->getType() == CGeomLight3DType::DIRECTIONAL) {
      auto d = light->getDirection();

      drawVector(CVector3D(p), d, "D");
    }
    else if (light->getType() == CGeomLight3DType::POINT) {
      auto r = light->getPointRadius();

      drawCircle(CVector3D(p), r);
    }
    else if (light->getType() == CGeomLight3DType::SPOT) {
      auto d = light->getSpotDirection();
      auto a = light->getSpotCutOffAngle();

      auto a1 = CMathGen::DegToRad(a)/2.0;

      drawCone(CVector3D(p), d, a1);
    }
  }
}

void
CQCamera3DOverview::
drawPolygon(const std::vector<CPoint3D> &points) const
{
  auto drawPolygon2D = [&](const CDisplayRange2D &range, const QRectF &rect,
                           const std::vector<CPoint2D> &points) {
    painter_->setClipRect(rect);

    std::vector<QPointF> ppoints;

    for (const auto &p : points) {
      double px, py;
      range.windowToPixel(p.x, p.y, &px, &py);
      ppoints.push_back(QPointF(px, py));
    }

    painter_->drawPolygon(&ppoints[0], ppoints.size());
  };

  std::vector<CPoint2D> xpoints, ypoints, zpoints, ppoints;

  for (const auto &p : points) {
    auto p1 = modelMatrix_*p;

    xpoints.push_back(CPoint2D(p1.getX(), p1.getY())); // XY
    ypoints.push_back(CPoint2D(p1.getZ(), p1.getY())); // ZY
    zpoints.push_back(CPoint2D(p1.getX(), p1.getZ())); // XZ

    auto p2 = projectionMatrix_*viewMatrix_*p1;

    ppoints.push_back(CPoint2D(p2.getX(), p2.getY()));
  }

  drawPolygon2D(xrange_, xrect_, xpoints);
  drawPolygon2D(yrange_, yrect_, ypoints);
  drawPolygon2D(zrange_, zrect_, zpoints);
  drawPolygon2D(prange_, prect_, ppoints);
}

void
CQCamera3DOverview::
drawCone(const CVector3D &p, const CVector3D &d, double a) const
{
  auto m1 = CMatrix2D::rotation( a);
  auto m2 = CMatrix2D::rotation(-a);

  auto drawCone2D = [&](const CDisplayRange2D &range, const QRectF &rect, const CPoint2D &p,
                        const CPoint2D &d, double sx, double sy) {
    painter_->setClipRect(rect);

    auto s = std::sqrt(sx*sx + sy*sy)/3.0;

    auto d1 = m1*d;
    auto d2 = m2*d;

    double px1, py1;
    range.windowToPixel(p.x, p.y, &px1, &py1);
    double px2, py2;
    range.windowToPixel(p.x + s*d1.x, p.y + s*d1.y, &px2, &py2);
    double px3, py3;
    range.windowToPixel(p.x + s*d2.x, p.y + s*d2.y, &px3, &py3);

    painter_->drawLine(px1, py1, px2, py2);
    painter_->drawLine(px1, py1, px3, py3);
  };

  auto x1 = p.getX(), y1 = p.getY(), z1 = p.getZ();
  auto x2 = d.getX(), y2 = d.getY(), z2 = d.getZ();

  drawCone2D(xrange_, xrect_, CPoint2D(x1, y1), CPoint2D(x2, y2), xs_, ys_); // XY
  drawCone2D(yrange_, yrect_, CPoint2D(z1, y1), CPoint2D(z2, y2), zs_, ys_); // ZY
  drawCone2D(zrange_, zrect_, CPoint2D(x1, z1), CPoint2D(x2, z2), xs_, zs_); // XZ
}

void
CQCamera3DOverview::
drawVector(const CVector3D &p, const CVector3D &d, const QString &label) const
{
  auto drawVector2D = [&](const CDisplayRange2D &range, const QRectF &rect, const CPoint2D &p,
                          const CPoint2D &d, double sx, double sy, const QString &label) {
    painter_->setClipRect(rect);

    auto s = std::sqrt(sx*sx + sy*sy)/3.0;

    double px1, py1;
    range.windowToPixel(p.x, p.y, &px1, &py1);
    double px2, py2;
    range.windowToPixel(p.x + s*d.x, p.y + s*d.y, &px2, &py2);

    painter_->drawLine(px1, py1, px2, py2);

    painter_->drawText(px2, py2, label);
  };

  auto x1 = p.getX(), y1 = p.getY(), z1 = p.getZ();
  auto x2 = d.getX(), y2 = d.getY(), z2 = d.getZ();

  drawVector2D(xrange_, xrect_, CPoint2D(x1, y1), CPoint2D(x2, y2), xs_, ys_, label); // XY
  drawVector2D(yrange_, yrect_, CPoint2D(z1, y1), CPoint2D(z2, y2), zs_, ys_, label); // ZY
  drawVector2D(zrange_, zrect_, CPoint2D(x1, z1), CPoint2D(x2, z2), xs_, zs_, label); // XZ
}

void
CQCamera3DOverview::
drawCircle(const CVector3D &origin, double r)
{
  auto drawCircle2D = [&](CDisplayRange2D &range, const QRectF &rect,
                          const CPoint2D &o, double r) {
    painter_->setClipRect(rect);

    double px1, py1;
    range.windowToPixel(o.x - r, o.y - r, &px1, &py1);
    double px2, py2;
    range.windowToPixel(o.x + r, o.y + r, &px2, &py2);

    painter_->drawEllipse(QRectF(px1, py1, px2 - px1, py2 - py1));
  };

  drawCircle2D(xrange_, xrect_, CPoint2D(origin.x(), origin.y()), r); // XY
  drawCircle2D(yrange_, yrect_, CPoint2D(origin.z(), origin.y()), r); // ZY
  drawCircle2D(zrange_, zrect_, CPoint2D(origin.x(), origin.z()), r); // XZ
}

void
CQCamera3DOverview::
drawPoint(const CVector3D &p, const QString &label) const
{
  auto drawPoint2D = [&](const CDisplayRange2D &range, const QRectF &rect,
                         const CPoint2D &p, const QString &label) {
    painter_->setClipRect(rect);

    double px, py;
    range.windowToPixel(p.x, p.y, &px, &py);

    painter_->drawLine(px - 4, py, px + 4, py);
    painter_->drawLine(px, py - 4, px, py + 4);

    painter_->drawText(px, py, label);
  };

  drawPoint2D(xrange_, xrect_, CPoint2D(p.getX(), p.getY()), label); // XY
  drawPoint2D(yrange_, yrect_, CPoint2D(p.getZ(), p.getY()), label); // ZY
  drawPoint2D(zrange_, zrect_, CPoint2D(p.getX(), p.getZ()), label); // XZ
}

void
CQCamera3DOverview::
drawPixmap(const CPoint3D &p, const QPixmap &pixmap) const
{
  auto s = pixmap.width();

  auto drawPixmap2D = [&](const CDisplayRange2D &range, const QRectF &rect,
                          const CPoint2D &p, const QPixmap &pixmap) {
    painter_->setClipRect(rect);

    double px, py;
    range.windowToPixel(p.x, p.y, &px, &py);

    painter_->drawPixmap(px - s/2, py - s/2, pixmap);
  };

  drawPixmap2D(xrange_, xrect_, CPoint2D(p.getX(), p.getY()), pixmap); // XY
  drawPixmap2D(yrange_, yrect_, CPoint2D(p.getZ(), p.getY()), pixmap); // ZY
  drawPixmap2D(zrange_, zrect_, CPoint2D(p.getX(), p.getZ()), pixmap); // XZ
}

void
CQCamera3DOverview::
mousePressEvent(QMouseEvent *e)
{
  bool isShift   = (e->modifiers() & Qt::ShiftModifier);
//bool isControl = (e->modifiers() & Qt::ControlModifier);

  if (e->button() == Qt::LeftButton) {
    if      (mouseType() == CQCamera3DMouseType::CAMERA) {
      setCameraPosition(e->x(), e->y(), isShift);
    }
    else if (mouseType() == CQCamera3DMouseType::OBJECT) {
      selectObject(e->x(), e->y());
    }
    else if (mouseType() == CQCamera3DMouseType::LIGHT) {
      setLightPosition(e->x(), e->y(), isShift);
    }
  }

  pressed_     = true;
  mouseButton_ = e->button();
}

void
CQCamera3DOverview::
mouseReleaseEvent(QMouseEvent *)
{
  pressed_ = false;
}

void
CQCamera3DOverview::
mouseMoveEvent(QMouseEvent *e)
{
  int x = e->x();
  int y = e->y();

  bool isShift   = (e->modifiers() & Qt::ShiftModifier);
//bool isControl = (e->modifiers() & Qt::ControlModifier);

  if (pressed_) {
    if (mouseButton_ == Qt::LeftButton) {
      if      (mouseType() == CQCamera3DMouseType::CAMERA) {
        setCameraPosition(x, y, isShift);
      }
      else if (mouseType() == CQCamera3DMouseType::OBJECT) {
      }
      else if (mouseType() == CQCamera3DMouseType::LIGHT) {
        setLightPosition(e->x(), e->y(), isShift);
      }
    }
  }

  auto showPoint = [&](const CPoint3D &p1) {
    auto posStr = QString("X=%1, Y=%2, Z=%3").arg(p1.x).arg(p1.y).arg(p1.z);
    app_->status()->setMouseLabel(posStr);
  };

  int ind = ind_;

  ind_ = -1;

  CPoint2D p;
  if (pressRange(xrange_, x, y, p)) { ind_ = 0; showPoint(CPoint3D(p.x, p.y, 0.0)); } // XY
  if (pressRange(yrange_, x, y, p)) { ind_ = 1; showPoint(CPoint3D(0.0, p.y, p.x)); } // ZY
  if (pressRange(zrange_, x, y, p)) { ind_ = 2; showPoint(CPoint3D(p.x, 0.0, p.y)); } // XZ

  if (! pressed_) {
    if (ind_ != ind)
      update();
  }
}

void
CQCamera3DOverview::
keyPressEvent(QKeyEvent *e)
{
  auto *canvas = app_->canvas();
  auto *camera = canvas->currentCamera();

  auto k = e->key();

//bool isShift   = (e->modifiers() & Qt::ShiftModifier);
//bool isControl = (e->modifiers() & Qt::ControlModifier);

  auto bbox = canvas->bbox();

  auto d = bbox.getMaxSize()/100.0;

  if      (mouseType() == CQCamera3DMouseType::CAMERA) {
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
  else if (mouseType() == CQCamera3DMouseType::OBJECT) {
  }
  else if (mouseType() == CQCamera3DMouseType::LIGHT) {
  }

  canvas->update();
  update();
}

void
CQCamera3DOverview::
selectObject(int x, int y)
{
  auto *canvas = app_->canvas();
  auto *camera = canvas->currentCamera();

  auto pos = camera->position();

  CPoint2D p;

  if (pressRange(xrange_, x, y, p))
    selectObjectXY(CPoint3D(p.x, p.y, pos.z())); // XY
  if (pressRange(yrange_, x, y, p))
    selectObjectZY(CPoint3D(pos.x(), p.y, p.x)); // ZY
  if (pressRange(zrange_, x, y, p))
    selectObjectXZ(CPoint3D(p.x, pos.y(), p.y)); // XZ
}

void
CQCamera3DOverview::
selectObjectXY(const CPoint3D &p)
{
  auto *canvas = app_->canvas();

  QPointF p1(p.x, p.y);

  std::map<double, CGeomFace3D *> faces;

  for (const auto &face : faces_) {
    QPolygonF poly;

    for (const auto &p : face.points)
      poly.push_back(QPointF(p.x, p.y));

    if (poly.containsPoint(p1, Qt::WindingFill))
      faces[polygonArea(poly)] = face.face;
  }

  if (faces.empty()) {
    canvas->deselectAll();
    return;
  }

  auto selectType = canvas->selectType();

  auto *face = faces.begin()->second;

  if      (selectType == CQCamera3DCanvas::SelectType::OBJECT)
    canvas->selectObject(face->getObject());
  else if (selectType == CQCamera3DCanvas::SelectType::FACE)
    canvas->selectFace(face);
}

void
CQCamera3DOverview::
selectObjectZY(const CPoint3D &p)
{
  auto *canvas = app_->canvas();

  QPointF p1(p.z, p.y);

  std::map<double, CGeomFace3D *> faces;

  for (const auto &face : faces_) {
    QPolygonF poly;

    for (const auto &p : face.points)
      poly.push_back(QPointF(p.z, p.y));

    if (poly.containsPoint(p1, Qt::WindingFill))
      faces[polygonArea(poly)] = face.face;
  }

  if (faces.empty()) {
    canvas->deselectAll();
    return;
  }

  auto selectType = canvas->selectType();

  auto *face = faces.begin()->second;

  if      (selectType == CQCamera3DCanvas::SelectType::OBJECT)
    canvas->selectObject(face->getObject());
  else if (selectType == CQCamera3DCanvas::SelectType::FACE)
    canvas->selectFace(face);
}

void
CQCamera3DOverview::
selectObjectXZ(const CPoint3D &p)
{
  auto *canvas = app_->canvas();

  QPointF p1(p.x, p.z);

  std::map<double, CGeomFace3D *> faces;

  for (const auto &face : faces_) {
    QPolygonF poly;

    for (const auto &p : face.points)
      poly.push_back(QPointF(p.x, p.z));

    if (poly.containsPoint(p1, Qt::WindingFill))
      faces[polygonArea(poly)] = face.face;
  }

  if (faces.empty()) {
    canvas->deselectAll();
    return;
  }

  auto selectType = canvas->selectType();

  auto *face = faces.begin()->second;

  if      (selectType == CQCamera3DCanvas::SelectType::OBJECT)
    canvas->selectObject(face->getObject());
  else if (selectType == CQCamera3DCanvas::SelectType::FACE)
    canvas->selectFace(face);
}

void
CQCamera3DOverview::
setCameraPosition(int x, int y, bool setOrigin)
{
  auto *canvas = app_->canvas();
  auto *camera = canvas->currentCamera();

  auto pos = camera->position();

  CPoint2D p;

  if (! setOrigin) {
    if (pressRange(xrange_, x, y, p)) camera->setPosition(CVector3D(p.x, p.y, pos.z())); // XY
    if (pressRange(yrange_, x, y, p)) camera->setPosition(CVector3D(pos.x(), p.y, p.x)); // ZY
    if (pressRange(zrange_, x, y, p)) camera->setPosition(CVector3D(p.x, pos.y(), p.y)); // XZ
  }
  else {
#if 0
    auto pointAngle = [&](double x, double y) {
      return CVector2D(CPoint2D(x, y), p).angle();
    };

    if (pressRange(xrange_, x, y, p)) camera->setRoll (pointAngle(pos.x(), pos.y())); // XY
    if (pressRange(yrange_, x, y, p)) camera->setPitch(pointAngle(pos.z(), pos.y())); // ZY
    if (pressRange(zrange_, x, y, p)) camera->setYaw  (pointAngle(pos.x(), pos.z()) - M_PI/2.0); // XZ
#else
    auto origin = camera->origin();

    if (pressRange(xrange_, x, y, p))
      origin = CVector3D(p.x, p.y, origin.y()); // XY

    if (pressRange(yrange_, x, y, p))
      origin = CVector3D(origin.x(), p.y, p.x); // ZY

    if (pressRange(zrange_, x, y, p))
      origin = CVector3D(p.x, origin.y(), p.y); // XZ

    camera->setOrigin(origin);
#endif
  }

  update();
}

void
CQCamera3DOverview::
setLightPosition(int x, int y, bool setDirection)
{
  auto *canvas = app_->canvas();
  auto *light  = canvas->currentLight();

  auto pos = CVector3D(light->getPosition());

  if (! setDirection) {
    CPoint2D p;

    if (pressRange(xrange_, x, y, p)) light->setPosition(CPoint3D(p.x, p.y, pos.z())); // XY
    if (pressRange(yrange_, x, y, p)) light->setPosition(CPoint3D(pos.x(), p.y, p.x)); // ZY
    if (pressRange(zrange_, x, y, p)) light->setPosition(CPoint3D(p.x, pos.y(), p.y)); // XZ
  }
  else {
    CVector3D dir { 1, 0, 0 };

    if      (light->getType() == CGeomLight3DType::DIRECTIONAL)
      dir = light->getDirection();
    else if (light->getType() == CGeomLight3DType::SPOT)
      dir = light->getSpotDirection();
    else
      return;

    auto pos1 = pos + dir;

    CPoint2D p;

    if (pressRange(xrange_, x, y, p)) pos1 = CPoint3D(p.x, p.y, pos1.z()); // XY
    if (pressRange(yrange_, x, y, p)) pos1 = CPoint3D(pos1.x(), p.y, p.x); // ZY
    if (pressRange(zrange_, x, y, p)) pos1 = CPoint3D(p.x, pos1.y(), p.y); // XZ

    auto dir1 = (pos1 - pos).normalized();

    if      (light->getType() == CGeomLight3DType::DIRECTIONAL) {
      light->setDirection(dir1);
    }
    else if (light->getType() == CGeomLight3DType::SPOT) {
      light->setSpotDirection(dir1);
    }
  }

  canvas->update();
  update();
}

bool
CQCamera3DOverview::
pressRange(const CDisplayRange2D &range, int x, int y, CPoint2D &p) const
{
  double xmin, ymin, xmax, ymax;
  range.getPixelRange(&xmin, &ymin, &xmax, &ymax);

  if (xmin > xmax) std::swap(xmin, xmax);
  if (ymin > ymax) std::swap(ymin, ymax);

  if (x < xmin || y < ymin || x > xmax || y > ymax)
    return false;

  double x1, y1;
  range.pixelToWindow(x, y, &x1, &y1);

  p = CPoint2D(x1, y1);

  return true;
}
