#include <CQCamera3DOverview.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DCamera.h>
#include <CQCamera3DLight.h>
#include <CQCamera3DStatus.h>
#include <CQCamera3DGeomObject.h>
#include <CQCamera3DGeomFace.h>
#include <CQCamera3DUtil.h>

#include <CQPixmapCache.h>
#include <CQRubberBand.h>
#include <CQUtilGeom.h>
#include <CGeomScene3D.h>
#include <CGeomObject3D.h>
#include <CGeomNodeData.h>
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
  setObjectName("overview");

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  auto *canvas = app_->canvas();
  auto *camera = canvas->getCurrentCamera();

  setFocusPolicy(Qt::StrongFocus);

  setMouseTracking(true);

  //---

  lightPixmap_ = CQPixmapCacheInst->getSizedPixmap("LIGHT", QSize(32, 32));

  rubberBand_ = new CQRubberBand(this);

  //---

  xview_.ind = 0; xview_.type = ViewType::XY    ; xview_.name = "XY";
  yview_.ind = 1; yview_.type = ViewType::ZY    ; yview_.name = "ZY";
  zview_.ind = 2; zview_.type = ViewType::XZ    ; zview_.name = "XZ";
  pview_.ind = 3; pview_.type = ViewType::THREED; pview_.name = "3D";

  views_.push_back(&xview_);
  views_.push_back(&yview_);
  views_.push_back(&zview_);
  views_.push_back(&pview_);

  views2_.push_back(&xview_);
  views2_.push_back(&yview_);
  views2_.push_back(&zview_);

  //---

  connect(app_, SIGNAL(animNameChanged()), this, SLOT(update()));
  connect(app_, SIGNAL(animTimeChanged()), this, SLOT(update()));

  connect(camera, SIGNAL(stateChanged()), this, SLOT(update()));

  connect(canvas, SIGNAL(stateChanged()), this, SLOT(update()));
  connect(canvas, SIGNAL(objectAdded()), this, SLOT(update()));
  connect(canvas, SIGNAL(eyeLineChanged()), this, SLOT(update()));
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
  double w = width ()/2.0;
  double h = height()/2.0;

  int x1 = 0, x2 = x1 + w, x3 = x2 + w;
  int y1 = 0, y2 = y1 + h, y3 = y2 + h;

  xview_.range.setPixelRange(x1, y1, x2, y2); // XY
  yview_.range.setPixelRange(x2, y1, x3, y2); // ZY
  zview_.range.setPixelRange(x1, y2, x2, y3); // XZ
  pview_.range.setPixelRange(x2, y2, x3, y3); // 3D

  for (auto *view : views2_)
    view->range.setEqualScale(isEqualScale());

  update();
}

void
CQCamera3DOverview::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  drawData_.painter = &painter;

  //---

  auto *canvas = app_->canvas();
  auto *camera = canvas->getCurrentCamera();

  drawData_.projectionMatrix = CMatrix3DH(camera->worldMatrix());
  drawData_.viewMatrix       = CMatrix3DH(camera->viewMatrix());

  //---

  // set window ranges from object bbox
  if (! bboxSet_) {
    auto bbox = canvas->bbox();

    auto c = bbox.getCenter();

    xs_ = bbox.getXSize();
    ys_ = bbox.getYSize();
    zs_ = bbox.getZSize();

    xview_.range.setWindowRange(c.x - xs_, c.y - ys_, c.x + xs_, c.y + ys_); // XY
    yview_.range.setWindowRange(c.z + zs_, c.y - ys_, c.z - zs_, c.y + ys_); // ZY
    zview_.range.setWindowRange(c.x - xs_, c.z + zs_, c.x + xs_, c.z - zs_); // XZ
    pview_.range.setWindowRange(-1, -1, 1, 1); // 3D

    bboxSet_ = true;
  }

  //---

  // draw background
  painter.fillRect(rect(), QColor(240, 240, 240));

  //---

  auto drawPixelBorder = [&](ViewData &view) {
    bool current = (view.ind == ind_);

    QPen pen;
    pen.setColor(current ? Qt::red : Qt::black);
    pen.setWidthF(current ? 2 : 0);
    painter.setPen(pen);

    QBrush brush(current ? QColor(220, 220, 240) : QColor(255, 255, 255));
    painter.setBrush(brush);

    double pxmin, pymin, pxmax, pymax;
    view.range.getPixelRange(&pxmin, &pymin, &pxmax, &pymax);

    view.rect = QRectF(pxmin, pymin, pxmax - pxmin - 1, pymax - pymin - 1);
    painter.drawRect(view.rect);

    pen.setWidthF(0);
    painter.setPen(pen);

    painter.setBrush(Qt::NoBrush);
  };

  //---

  // draw border (sets rect values)
  painter.setPen(Qt::black);
  painter.setBrush(Qt::NoBrush);

  for (auto *view : views_)
    drawPixelBorder(*view);

  //---

  drawModel();

  //---

  drawCameras();

  drawCursor();

  //---

  drawLights();

  //---

  // draw labels
  painter.setPen(Qt::black);

  QFontMetrics fm(font());

  auto drawTitle = [&](const ViewData &view) {
    painter.setClipRect(view.rect);

    double pxmin, pymin, pxmax, pymax;
    view.range.getPixelRange(&pxmin, &pymin, &pxmax, &pymax);

    auto r = QRectF(pxmin + 4, pymin + 4, fm.horizontalAdvance(view.name) + 4, fm.height() + 4);
    painter.drawText(r, view.name);
  };

  for (auto *view : views2_)
    drawTitle(*view);
}

void
CQCamera3DOverview::
drawModel()
{
  if (modelType() == ModelType::NONE)
    return;

  drawData_.filled = (modelType() == ModelType::SOLID);

  //---

  drawData_.useAnim = (app_->animName() != "");

  //---

  // draw model
  drawData_.objectFaces.clear();

  drawData_.painter->setPen(QColor(0, 0, 0, 32));
  drawData_.painter->setBrush(Qt::NoBrush);

  class SceneVisitor : public CGeomScene3DVisitor {
   public:
    SceneVisitor(CQCamera3DOverview *overview, CGeomScene3D *scene, DrawData &drawData) :
     CGeomScene3DVisitor(scene), overview_(overview), drawData_(drawData) {
      app_ = overview->app();
    }

    bool beginVisitObject(CGeomObject3D *object) override {
      auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);
      assert(object1);

      //---

      drawData_.modelMatrix = CMatrix3DH(object1->getHierTransform());
      drawData_.meshMatrix  = CMatrix3DH(object->getMeshGlobalTransform());

      //---

      auto *rootObject = object->getRootObject();

      nodeMatrices_ = &app_->getObjectNodeMatrices(rootObject);

      //---

      objectMaterial_ = object->getMaterialP();

      //---

      faceDatas_ = &drawData_.objectFaces[object];

      return true;
    }

    void endVisitObject(CGeomObject3D *) override { }

    bool beginVisitFace(CGeomFace3D *face) override {
      faceDatas_->push_back(FaceData());

      faceData_ = &faceDatas_->back();

      faceData_->face = const_cast<CGeomFace3D *>(face);

      return true;
    }

    void endVisitFace(CGeomFace3D *face) override {
      auto *face1 = dynamic_cast<const CQCamera3DGeomFace *>(face);

      const auto &selectedEdges = face1->selectedEdges();

      for (const auto &selectedEdge : selectedEdges) {
        const auto &edge = face1->edge(selectedEdge);

        auto p1 = vertexPoint(edge.v1);
        auto p2 = vertexPoint(edge.v2);

        drawData_.painter->setPen(QColor(255, 0, 0, 255));

        overview_->drawModelLine(p1, p2);

        drawData_.painter->setPen(QColor(0, 0, 0, 32));
      }

      //---

      if (drawData_.filled) {
        auto *faceMaterial = faceData_->face->getMaterialP();

        if (! faceMaterial && objectMaterial_)
          faceMaterial = objectMaterial_;

        auto color = face->color().value_or(CRGBA(1, 1, 1));

        if (faceMaterial && faceMaterial->diffuse())
          color = faceMaterial->diffuse().value();

        drawData_.painter->setBrush(RGBAToQColor(color));
      }

      //---

      bool selected = (object_->getHierSelected() || face->getSelected());

      if (selected)
        drawData_.painter->setPen(QColor(255, 0, 0, 255));

      overview_->drawModelPolygon(faceData_->points);

      if (selected)
        drawData_.painter->setPen(QColor(0, 0, 0, 32));
    }

    void visitFaceVertex(CGeomVertex3D *vertex) override {
      auto p = vertexPoint(vertex);

      faceData_->points.push_back(p);

      if (vertex->isSelected()) {
        drawData_.painter->setPen(QColor(255, 0, 0, 255));

        overview_->drawModelPoint(p, "");

        drawData_.painter->setPen(QColor(0, 0, 0, 32));
      }
    }

    void visitLine(CGeomLine3D *line) override {
      faceDatas_->push_back(FaceData());

      faceData_ = &faceDatas_->back();

      faceData_->line = const_cast<CGeomLine3D *>(line);

      auto v1 = line->getStartInd();
      auto v2 = line->getEndInd  ();

      std::vector<uint> vertices;

      vertices.push_back(v1);
      vertices.push_back(v2);

      //--

      for (const auto &v : vertices) {
        auto p = vertexPoint(v);

        faceData_->points.push_back(p);

        const auto &vertex = object_->getVertex(v);

        if (vertex.isSelected()) {
          drawData_.painter->setPen(QColor(255, 0, 0, 255));

          overview_->drawModelPoint(p, "");

          drawData_.painter->setPen(QColor(0, 0, 0, 32));
        }
      }

      //---

      bool selected = line->getSelected();

      if (selected)
        drawData_.painter->setPen(QColor(255, 0, 0, 255));

      overview_->drawModelLine(faceData_->points[0], faceData_->points[1]);

      if (selected)
        drawData_.painter->setPen(QColor(0, 0, 0, 32));
    }

    bool beginVisitSubFace(CGeomFace3D *face) override {
      return beginVisitFace(face);
    }

    void endVisitSubFace(CGeomFace3D *face) override {
      endVisitFace(face);
    }

    void visitSubFaceVertex(CGeomVertex3D *v) override {
      visitFaceVertex(v);
    }

    void visitSubLine(CGeomLine3D *line) override {
      visitLine(line);
    }

   private:
    CPoint3D vertexPoint(int v) const {
      auto *vertex = object_->getVertexP(v);

      return vertexPoint(vertex);
    }

    CPoint3D vertexPoint(const CGeomVertex3D *vertex) const {
      const auto &model  = vertex->getModel();

      auto p = model;

      //---

      if (drawData_.useAnim)
        p = app_->adjustAnimPoint(*vertex, p, *nodeMatrices_);

      return drawData_.meshMatrix*p;
    }

   private:
    CQCamera3DOverview*           overview_ { nullptr };
    CQCamera3DOverview::DrawData& drawData_;
    CQCamera3DApp*                app_      { nullptr };

    CGeomMaterial* objectMaterial_ { nullptr };

    CQCamera3DOverview::FaceDatas* faceDatas_ { nullptr };
    CQCamera3DOverview::FaceData*  faceData_  { nullptr };

    const CQCamera3DApp::NodeMatrices *nodeMatrices_ { nullptr };
  };

  auto *scene = app_->getScene();

  SceneVisitor visitor(this, scene, drawData_);

  visitor.visit();
}

void
CQCamera3DOverview::
drawCameras()
{
  if (! isCameraVisible())
    return;

  auto *canvas = app_->canvas();

  for (auto *camera : canvas->cameras())
    drawCamera(camera);

  drawEyeLine();
}

void
CQCamera3DOverview::
drawCamera(CQCamera3DCamera *camera)
{
  auto pos    = camera->position();
  auto origin = camera->origin();

  // draw camera position, origin and direction vectors

//auto o = CVector3D(0, 0, 0);

  drawData_.painter->setPen(QColor(0, 0, 255, 255));
  drawData_.painter->setBrush(Qt::NoBrush);

  auto front = camera->front();
  auto up    = camera->up();
  auto right = camera->right();

  drawPoint(pos   , "P");
  drawPoint(origin, "O");

  drawVector(pos, front, "F");
  drawVector(pos, up   , "U");
  drawVector(pos, right, "R");

  //---

  // draw camera orbit

  auto r = CVector3D(origin, pos).length();

  drawData_.painter->setPen(Qt::red);

  drawCircle(origin, r);
}

void
CQCamera3DOverview::
drawEyeLine()
{
  auto *canvas = app_->canvas();

  const auto &eyeLine = canvas->eyeLine();

  if (canvas->isShowEyeLine() && eyeLine.isSet) {
    const auto &xyPlane = canvas->xyPlane();

    CLine3D line(eyeLine.pv1, eyeLine.pv2);
    double t = 0.0;
    (void) xyPlane.intersectLine(line, &t);
    auto pi = line.point(t);

    drawVector(CVector3D(eyeLine.pv1), CVector3D(pi), "E1");
    drawVector(CVector3D(eyeLine.pv2), CVector3D(pi), "E2");
  }
}

void
CQCamera3DOverview::
drawCursor()
{
  auto *canvas = app_->canvas();

  auto p = canvas->cursor();

  auto bbox = canvas->bbox();

  auto r = bbox.getMaxSize()/100.0;

  drawData_.painter->setPen(QColor(255, 0, 0));

  drawCircle(CVector3D(p), r);
}

void
CQCamera3DOverview::
drawLights()
{
  if (! isLightsVisible())
    return;

  drawData_.painter->setPen(QColor(0, 0, 0, 255));
  drawData_.painter->setBrush(Qt::NoBrush);

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
drawModelPolygon(const std::vector<CPoint3D> &points) const
{
  auto drawPolygon2D = [&](const ViewData &view, const std::vector<CPoint2D> &points) {
    drawData_.painter->setClipRect(view.rect);

    std::vector<QPointF> ppoints;

    for (const auto &p : points) {
      double px, py;
      view.range.windowToPixel(p.x, p.y, &px, &py);

      ppoints.push_back(QPointF(px, py));
    }

    drawData_.painter->drawPolygon(&ppoints[0], ppoints.size());
  };

  std::vector<CPoint2D> xpoints, ypoints, zpoints, ppoints;

  for (const auto &p : points) {
    auto p1 = drawData_.modelMatrix*p;

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
}

void
CQCamera3DOverview::
drawModelLine(const CPoint3D &p1, const CPoint3D &p2) const
{
  auto drawLine2D = [&](const ViewData &view, const CPoint2D &p1, const CPoint2D &p2) {
    drawData_.painter->setClipRect(view.rect);

    double px1, py1;
    view.range.windowToPixel(p1.x, p1.y, &px1, &py1);
    double px2, py2;
    view.range.windowToPixel(p2.x, p2.y, &px2, &py2);

    drawData_.painter->drawLine(px1, py1, px2, py2);
  };

  auto pm1 = drawData_.modelMatrix*p1;
  auto pm2 = drawData_.modelMatrix*p2;

  drawLine2D(xview_, CPoint2D(pm1.getX(), pm1.getY()), CPoint2D(pm2.getX(), pm2.getY())); // XY
  drawLine2D(yview_, CPoint2D(pm1.getZ(), pm1.getY()), CPoint2D(pm2.getZ(), pm2.getY())); // ZY
  drawLine2D(zview_, CPoint2D(pm1.getX(), pm1.getZ()), CPoint2D(pm2.getX(), pm2.getZ())); // XZ

  pm1 = drawData_.projectionMatrix*drawData_.viewMatrix*p1;
  pm2 = drawData_.projectionMatrix*drawData_.viewMatrix*p2;

  drawLine2D(pview_, CPoint2D(pm1.getX(), pm1.getY()), CPoint2D(pm2.getX(), pm2.getY()));
}

void
CQCamera3DOverview::
drawModelPoint(const CPoint3D &p, const QString &label) const
{
  auto drawPoint2D = [&](const ViewData &view, const CPoint2D &p, const QString &label) {
    drawData_.painter->setClipRect(view.rect);

    double px, py;
    view.range.windowToPixel(p.x, p.y, &px, &py);

    drawData_.painter->drawLine(px - 4, py, px + 4, py);
    drawData_.painter->drawLine(px, py - 4, px, py + 4);

    if (label != "")
      drawData_.painter->drawText(px, py, label);
  };

  auto p1 = drawData_.modelMatrix*p;

  drawPoint2D(xview_, CPoint2D(p1.getX(), p1.getY()), label); // XY
  drawPoint2D(yview_, CPoint2D(p1.getZ(), p1.getY()), label); // ZY
  drawPoint2D(zview_, CPoint2D(p1.getX(), p1.getZ()), label); // XZ

  auto p2 = drawData_.projectionMatrix*drawData_.viewMatrix*p1;

  drawPoint2D(pview_, CPoint2D(p2.getX(), p2.getY()), label);
}

void
CQCamera3DOverview::
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
    view.range.windowToPixel(p1.x, p1.y, &px1, &py1);
    double px2, py2;
    view.range.windowToPixel(p1.x + s*d1.x, p1.y + s*d1.y, &px2, &py2);
    double px3, py3;
    view.range.windowToPixel(p1.x + s*d2.x, p1.y + s*d2.y, &px3, &py3);

    drawData_.painter->drawLine(px1, py1, px2, py2);
    drawData_.painter->drawLine(px1, py1, px3, py3);
  };

  drawCone2D(xview_, p, d, xs_, ys_); // XY
  drawCone2D(yview_, p, d, zs_, ys_); // ZY
  drawCone2D(zview_, p, d, xs_, zs_); // XZ
}

void
CQCamera3DOverview::
drawVector(const CVector3D &p, const CVector3D &d, const QString &label) const
{
  auto drawVector2D = [&](const ViewData &view, const CPoint2D &p,
                          const CPoint2D &d, double sx, double sy, const QString &label) {
    drawData_.painter->setClipRect(view.rect);

    auto s = std::sqrt(sx*sx + sy*sy)/3.0;

    double px1, py1;
    view.range.windowToPixel(p.x, p.y, &px1, &py1);
    double px2, py2;
    view.range.windowToPixel(p.x + s*d.x, p.y + s*d.y, &px2, &py2);

    drawData_.painter->drawLine(px1, py1, px2, py2);

    drawData_.painter->drawText(px2, py2, label);
  };

  auto x1 = p.getX(), y1 = p.getY(), z1 = p.getZ();
  auto x2 = d.getX(), y2 = d.getY(), z2 = d.getZ();

  drawVector2D(xview_, CPoint2D(x1, y1), CPoint2D(x2, y2), xs_, ys_, label); // XY
  drawVector2D(yview_, CPoint2D(z1, y1), CPoint2D(z2, y2), zs_, ys_, label); // ZY
  drawVector2D(zview_, CPoint2D(x1, z1), CPoint2D(x2, z2), xs_, zs_, label); // XZ
}

void
CQCamera3DOverview::
drawCircle(const CVector3D &origin, double r)
{
  auto drawCircle2D = [&](const ViewData &view, const CPoint2D &o, double r) {
    drawData_.painter->setClipRect(view.rect);

    double px1, py1;
    view.range.windowToPixel(o.x - r, o.y - r, &px1, &py1);
    double px2, py2;
    view.range.windowToPixel(o.x + r, o.y + r, &px2, &py2);

    drawData_.painter->drawEllipse(QRectF(px1, py1, px2 - px1, py2 - py1));
  };

  drawCircle2D(xview_, CPoint2D(origin.x(), origin.y()), r); // XY
  drawCircle2D(yview_, CPoint2D(origin.z(), origin.y()), r); // ZY
  drawCircle2D(zview_, CPoint2D(origin.x(), origin.z()), r); // XZ
}

void
CQCamera3DOverview::
drawPoint(const CVector3D &p, const QString &label) const
{
  auto drawPoint2D = [&](const ViewData &view, const CPoint2D &p, const QString &label) {
    drawData_.painter->setClipRect(view.rect);

    double px, py;
    view.range.windowToPixel(p.x, p.y, &px, &py);

    drawData_.painter->drawLine(px - 4, py, px + 4, py);
    drawData_.painter->drawLine(px, py - 4, px, py + 4);

    drawData_.painter->drawText(px, py, label);
  };

  drawPoint2D(xview_, CPoint2D(p.getX(), p.getY()), label); // XY
  drawPoint2D(yview_, CPoint2D(p.getZ(), p.getY()), label); // ZY
  drawPoint2D(zview_, CPoint2D(p.getX(), p.getZ()), label); // XZ
}

void
CQCamera3DOverview::
drawPixmap(const CPoint3D &p, const QPixmap &pixmap) const
{
  auto s = pixmap.width();

  auto drawPixmap2D = [&](const ViewData &view, const CPoint2D &p, const QPixmap &pixmap) {
    drawData_.painter->setClipRect(view.rect);

    double px, py;
    view.range.windowToPixel(p.x, p.y, &px, &py);

    drawData_.painter->drawPixmap(px - s/2, py - s/2, pixmap);
  };

  drawPixmap2D(xview_, CPoint2D(p.getX(), p.getY()), pixmap); // XY
  drawPixmap2D(yview_, CPoint2D(p.getZ(), p.getY()), pixmap); // ZY
  drawPixmap2D(zview_, CPoint2D(p.getX(), p.getZ()), pixmap); // XZ
}

void
CQCamera3DOverview::
mousePressEvent(QMouseEvent *e)
{
  mouseData_.pressed = true;
  mouseData_.button  = e->button();

  mouseData_.isShift   = (e->modifiers() & Qt::ShiftModifier);
  mouseData_.isControl = (e->modifiers() & Qt::ControlModifier);

  mouseData_.pressPixel = e->pos();
  mouseData_.movePixel  = mouseData_.pressPixel;

  //---

  if (mouseData_.button == Qt::LeftButton) {
    if      (editType() == EditType::SELECT) {
      rubberBand_->setBounds(mouseData_.pressPixel, mouseData_.movePixel);
      rubberBand_->show();
    }
    else if (editType() == EditType::CURSOR) {
      setCursorPosition(e->x(), e->y());
    }
    else if (editType() == EditType::MOVE) {
      if      (mouseType() == CQCamera3DMouseType::CAMERA) {
        setCameraPosition(e->x(), e->y(), mouseData_.isShift);
      }
      else if (mouseType() == CQCamera3DMouseType::LIGHT) {
        setLightPosition(e->x(), e->y(), mouseData_.isShift);
      }
    }
  }
}

void
CQCamera3DOverview::
mouseMoveEvent(QMouseEvent *e)
{
  mouseData_.movePixel = e->pos();

  //---

  int x = e->x();
  int y = e->y();

  if (mouseData_.pressed) {
    if (mouseData_.button == Qt::LeftButton) {
      if      (editType() == EditType::SELECT) {
        rubberBand_->setBounds(mouseData_.pressPixel, mouseData_.movePixel);
      }
      else if (editType() == EditType::MOVE) {
        if      (mouseType() == CQCamera3DMouseType::CAMERA) {
          setCameraPosition(x, y, mouseData_.isShift);
        }
        else if (mouseType() == CQCamera3DMouseType::LIGHT) {
          setLightPosition(x, y, mouseData_.isShift);
        }
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
  if (xview_.pressRange(x, y, p)) { ind_ = xview_.ind; showPoint(CPoint3D(p.x, p.y, 0.0)); } // XY
  if (yview_.pressRange(x, y, p)) { ind_ = yview_.ind; showPoint(CPoint3D(0.0, p.y, p.x)); } // ZY
  if (zview_.pressRange(x, y, p)) { ind_ = zview_.ind; showPoint(CPoint3D(p.x, 0.0, p.y)); } // XZ

  if (! mouseData_.pressed) {
    if (ind_ != ind)
      update();
  }
}

void
CQCamera3DOverview::
mouseReleaseEvent(QMouseEvent *e)
{
  mouseData_.movePixel = e->pos();

  if (mouseData_.button == Qt::LeftButton) {
    if (editType() == EditType::SELECT) {
      bool clear = ! mouseData_.isControl;

      if (mouseType() == CQCamera3DMouseType::OBJECT) {
        int dx = std::abs(mouseData_.pressPixel.x() - mouseData_.movePixel.x());
        int dy = std::abs(mouseData_.pressPixel.y() - mouseData_.movePixel.y());

        if (dx < 4 && dy < 4)
          selectObjectAt(mouseData_.pressPixel, clear);
        else
          selectObjectIn(mouseData_.pressPixel, rubberBand_->bounds(), clear);
      }

      rubberBand_->hide();
    }
  }

  mouseData_.pressed = false;
}

void
CQCamera3DOverview::
keyPressEvent(QKeyEvent *e)
{
  auto *canvas = app_->canvas();
  auto *camera = canvas->getCurrentCamera();

  auto k = e->key();

  mouseData_.isShift   = (e->modifiers() & Qt::ShiftModifier);
  mouseData_.isControl = (e->modifiers() & Qt::ControlModifier);

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
  else if (mouseType() == CQCamera3DMouseType::LIGHT) {
  }
  else if (mouseType() == CQCamera3DMouseType::OBJECT) {
  }

  if      (k == Qt::Key_Plus) {
    for (auto *v : views_)
      v->range.zoomIn(1.1);
  }
  else if (k == Qt::Key_Minus) {
    for (auto *v : views_)
      v->range.zoomOut(1.1);
  }
  else if (k == Qt::Key_Home) {
    for (auto *v : views_)
      v->range.reset();

    bboxSet_ = false;
  }

  canvas->update();
  update();
}

void
CQCamera3DOverview::
selectObjectAt(const QPoint &p, bool clear)
{
  auto *canvas = app_->canvas();
  auto *camera = canvas->getCurrentCamera();

  auto pos = camera->position();

  CPoint2D p1;

  if      (xview_.pressRange(p.x(), p.y(), p1))
    selectObjectAt1(xview_, CPoint3D(p1.x, p1.y, pos.z()), clear); // XY
  else if (yview_.pressRange(p.x(), p.y(), p1))
    selectObjectAt1(yview_, CPoint3D(pos.x(), p1.y, p1.x), clear); // ZY
  else if (zview_.pressRange(p.x(), p.y(), p1))
    selectObjectAt1(zview_, CPoint3D(p1.x, pos.y(), p1.y), clear); // XZ
}

void
CQCamera3DOverview::
selectObjectIn(const QPoint &p, const QRect &r, bool clear)
{
  auto *canvas = app_->canvas();

  ViewType viewType { ViewType::NONE };

  CPoint2D p1;

  if      (xview_.pressRange(p.x(), p.y(), p1))
    viewType = xview_.type;
  else if (yview_.pressRange(p.x(), p.y(), p1))
    viewType = yview_.type;
  else if (zview_.pressRange(p.x(), p.y(), p1))
    viewType = zview_.type;
  else
    return;

  auto pr1 = pixelToView(viewType, r.topLeft    ());
  auto pr2 = pixelToView(viewType, r.bottomRight());

  auto r1 = QRectF(QPointF(pr1.x, pr1.y), QPointF(pr2.x, pr2.y)).normalized();

  auto selectType = this->selectType();

  if      (selectType == SelectType::OBJECT) {
  }
  else if (selectType == SelectType::FACE) {
    std::vector<CGeomFace3D *> selectFaces;

    for (const auto &po : drawData_.objectFaces) {
      auto *object = po.first;

      auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);
      assert(object1);

      auto modelMatrix = CMatrix3DH(object1->getHierTransform());

      //---

      const auto &faceDatas = po.second;

      for (const auto &faceData : faceDatas) {
        QPolygonF poly;

        for (const auto &pf : faceData.points) {
          auto pf1 = viewQPoint(viewType, modelMatrix*pf);

          poly << pf1;
        }

        if (poly.intersects(QPolygonF(r1)))
          selectFaces.push_back(faceData.face);
      }
    }

    canvas->selectFaces(selectFaces, clear, /*update*/true);
  }
  else if (selectType == SelectType::EDGE) {
  }
  else if (selectType == SelectType::POINT) {
    CQCamera3DCanvas::ObjectSelectInds selectInds;

    for (const auto &po : drawData_.objectFaces) {
      auto *object = po.first;

      auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);
      assert(object1);

      auto modelMatrix = CMatrix3DH(object1->getHierTransform());

      //---

      const auto &faceDatas = po.second;

      for (const auto &faceData : faceDatas) {
        int i = 0;

        for (const auto &pf : faceData.points) {
          auto pf1 = viewQPoint(viewType, modelMatrix*pf);

          if (r1.contains(pf1)) {
            auto vInd = faceData.face->getVertex(i);

            selectInds[object].insert(vInd);
          }

          ++i;
        }
      }
    }

    canvas->selectVertices(selectInds);
  }
}

void
CQCamera3DOverview::
selectObjectAt1(const ViewData &view, const CPoint3D &p, bool clear)
{
  // get view point
  auto *canvas = app_->canvas();

  auto p1 = viewQPoint(view.type, p);

  //---

  // get inside faces
  std::map<double, FaceData *> atFaces;

  for (const auto &po : drawData_.objectFaces) {
    auto *object = po.first;

    auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);
    assert(object1);

    auto modelMatrix = CMatrix3DH(object1->getHierTransform());

    //---

    const auto &faceDatas = po.second;

    for (const auto &faceData : faceDatas) {
      QPolygonF poly;

      for (const auto &pf : faceData.points) {
        auto pf1 = viewQPoint(view.type, modelMatrix*pf);

        poly.push_back(pf1);
      }

      if (poly.containsPoint(p1, Qt::WindingFill))
        atFaces[polygonArea(poly)] = const_cast<FaceData *>(&faceData);
    }
  }

  if (atFaces.empty()) {
    canvas->deselectAll();
    return;
  }

  //---

  // select associated shape type
  auto selectType = this->selectType();

  if      (selectType == SelectType::OBJECT) {
    auto *face = atFaces.begin()->second->face;

    canvas->selectObject(face->getObject());
  }
  else if (selectType == SelectType::FACE) {
    auto *face = atFaces.begin()->second->face;

    canvas->selectFace(face, clear, /*update*/true);
  }
  else if (selectType == SelectType::EDGE) {
    auto pm = viewPoint(view.type, p);

    auto *faceData = atFaces.begin()->second;
    auto *face = faceData->face;

    auto *object = face->getObject();

    auto modelMatrix = CMatrix3DH(object->getHierTransform());

    auto *face1 = dynamic_cast<CQCamera3DGeomFace *>(face);

    const auto &vertices = face1->getVertices();

    auto nv = vertices.size();

    int    minInd  = -1;
    double minDist = 0.0;

    size_t i1 = nv - 1;

    for (size_t i2 = 0; i2 < nv; i1 = i2++) {
      auto pf1 = viewPoint(view.type, modelMatrix*faceData->points[i1]);
      auto pf2 = viewPoint(view.type, modelMatrix*faceData->points[i2]);

      double dist = 0.0;
      (void) CMathGeom2D::PointLineDistance(pm, CLine2D(pf1, pf2), &dist);

      if (minInd < 0 || dist < minDist) {
        minInd  = i1;
        minDist = dist;
      }
    }

    if (minInd >= 0)
      canvas->selectFaceEdge(face, minInd);
  }
  else if (selectType == SelectType::POINT) {
    auto pm = viewPoint(view.type, p);

    int    minInd  = -1;
    double minDist = 0.0;

    CGeomFace3D *face = nullptr;

    for (const auto &po : drawData_.objectFaces) {
      auto *object = po.first;

      auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);
      assert(object1);

      auto modelMatrix = CMatrix3DH(object1->getHierTransform());

      //---

      const auto &faceDatas = po.second;

      for (const auto &faceData : faceDatas) {
        int i = 0;

        for (const auto &pf : faceData.points) {
          auto pf1 = viewPoint(view.type, modelMatrix*pf);

          auto d = pf1.distanceTo(pm);

          if (! face || d < minDist) {
            face    = faceData.face;
            minInd  = i;
            minDist = d;
          }

          ++i;
        }
      }
    }

    auto  vInd = face->getVertex(minInd);
    auto &v    = face->getObject()->getVertex(vInd);

    canvas->selectVertex(&v);
  }
}

CPoint2D
CQCamera3DOverview::
pixelToView(ViewType viewType, const QPointF &p) const
{
  CPoint2D p1;

  if      (viewType == ViewType::XY) {
    (void) xview_.pressRange(p.x(), p.y(), p1);
  }
  else if (viewType == ViewType::ZY) {
    (void) yview_.pressRange(p.x(), p.y(), p1);
  }
  else if (viewType == ViewType::XZ) {
    (void) zview_.pressRange(p.x(), p.y(), p1);
  }
  else
    assert(false);

  return p1;
}

void
CQCamera3DOverview::
setCameraPosition(int x, int y, bool setOrigin)
{
  auto *canvas = app_->canvas();
  auto *camera = canvas->getCurrentCamera();

  auto pos = camera->position();

  CPoint2D p;

  if (! setOrigin) {
    if (xview_.pressRange(x, y, p)) camera->setPosition(CVector3D(p.x, p.y, pos.z())); // XY
    if (yview_.pressRange(x, y, p)) camera->setPosition(CVector3D(pos.x(), p.y, p.x)); // ZY
    if (zview_.pressRange(x, y, p)) camera->setPosition(CVector3D(p.x, pos.y(), p.y)); // XZ
  }
  else {
#if 0
    auto pointAngle = [&](double x, double y) {
      return CVector2D(CPoint2D(x, y), p).angle();
    };

    if (xview_.pressRange(x, y, p)) camera->setRoll (pointAngle(pos.x(), pos.y())); // XY
    if (yview_.pressRange(x, y, p)) camera->setPitch(pointAngle(pos.z(), pos.y())); // ZY
    if (zview_.pressRange(x, y, p)) camera->setYaw  (pointAngle(pos.x(), pos.z()) - M_PI/2.0); // XZ
#else
    auto origin = camera->origin();

    if (xview_.pressRange(x, y, p))
      origin = CVector3D(p.x, p.y, origin.y()); // XY

    if (yview_.pressRange(x, y, p))
      origin = CVector3D(origin.x(), p.y, p.x); // ZY

    if (zview_.pressRange(x, y, p))
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

    if (xview_.pressRange(x, y, p)) light->setPosition(CPoint3D(p.x, p.y, pos.z())); // XY
    if (yview_.pressRange(x, y, p)) light->setPosition(CPoint3D(pos.x(), p.y, p.x)); // ZY
    if (zview_.pressRange(x, y, p)) light->setPosition(CPoint3D(p.x, pos.y(), p.y)); // XZ
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

    if (xview_.pressRange(x, y, p)) pos1 = CPoint3D(p.x, p.y, pos1.z()); // XY
    if (yview_.pressRange(x, y, p)) pos1 = CPoint3D(pos1.x(), p.y, p.x); // ZY
    if (zview_.pressRange(x, y, p)) pos1 = CPoint3D(p.x, pos1.y(), p.y); // XZ

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

void
CQCamera3DOverview::
setCursorPosition(int x, int y)
{
  auto *canvas = app_->canvas();

  auto pos = canvas->cursor();

  CPoint2D p;

  if (xview_.pressRange(x, y, p)) canvas->setCursor(CPoint3D(p.x, p.y, pos.z)); // XY
  if (yview_.pressRange(x, y, p)) canvas->setCursor(CPoint3D(pos.x, p.y, p.x)); // ZY
  if (zview_.pressRange(x, y, p)) canvas->setCursor(CPoint3D(p.x, pos.y, p.y)); // XZ

  update();
}

QPointF
CQCamera3DOverview::
viewQPoint(ViewType viewType, const CPoint3D &p) const
{
  auto p1 = viewPoint(viewType, p);

  return QPointF(p1.x, p1.y);
}

CPoint2D
CQCamera3DOverview::
viewPoint(ViewType viewType, const CPoint3D &p) const
{
  CPoint2D p1;

  if      (viewType == ViewType::XY)
    p1 = CPoint2D(p.x, p.y);
  else if (viewType == ViewType::ZY)
    p1 = CPoint2D(p.z, p.y);
  else if (viewType == ViewType::XZ)
    p1 = CPoint2D(p.x, p.z);
  else
    assert(false);

  return p1;
}

#if 0
bool
CQCamera3DOverview::
pressRange(const ViewData &view, int x, int y, CPoint2D &p) const
{
  double xmin, ymin, xmax, ymax;
  view.range.getPixelRange(&xmin, &ymin, &xmax, &ymax);

  if (xmin > xmax) std::swap(xmin, xmax);
  if (ymin > ymax) std::swap(ymin, ymax);

  if (x < xmin || y < ymin || x > xmax || y > ymax)
    return false;

  double x1, y1;
  view.range.pixelToWindow(x, y, &x1, &y1);

  p = CPoint2D(x1, y1);

  return true;
}
#endif
