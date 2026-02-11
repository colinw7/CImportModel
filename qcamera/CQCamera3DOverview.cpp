#include <CQCamera3DOverview.h>
#include <CQCamera3DOverviewMouseModeIFace.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DCamera.h>
#include <CQCamera3DLight.h>
#include <CQCamera3DStatus.h>
#include <CQCamera3DGeomObject.h>
#include <CQCamera3DGeomFace.h>
#include <CQCamera3DMouseMode.h>
#include <CQCamera3DOpWidget.h>
#include <CQCamera3DUtil.h>

#include <CQPixmapCache.h>
#include <CQRubberBand.h>
#include <CQUtilGeom.h>
#include <CGeometry3D.h>
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

CPoint3D pointsCenter(const std::vector<CPoint3D> &points) {
  CPoint3D c(0, 0, 0);

  if (! points.empty()) {
    for (const auto &p : points)
      c += p;

    c /= points.size();
  }

  return c;
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

  mouseModeIFace_ = new CQCamera3DOverviewMouseModeIFace(this);

  mouseModeMgr_ = new CQCamera3DMouseModeMgr(mouseModeIFace_);

  opWidget_ = new CQCamera3DOpWidget(this, mouseModeMgr_);

  hideOptions();

  //---

  connect(app_, SIGNAL(animNameChanged()), this, SLOT(invalidate()));
  connect(app_, SIGNAL(animTimeChanged()), this, SLOT(invalidate()));

  connect(canvas, SIGNAL(stateChanged()), this, SLOT(invalidate()));
  connect(canvas, SIGNAL(objectAdded()), this, SLOT(invalidate()));
  connect(canvas, SIGNAL(eyeLineChanged()), this, SLOT(invalidate()));
  connect(canvas, SIGNAL(cameraStateChanged()), this, SLOT(invalidate()));
}

void
CQCamera3DOverview::
invalidate()
{
  valid_ = false;

  update();
}

void
CQCamera3DOverview::
setEditType(const EditType &v)
{
  QString tipStr;

  editType_ = v;

  auto *canvas = app_->canvas();
  auto *object = canvas->currentObject();

  if      (editType_ == EditType::CURSOR) {
    auto *mode = new CQCamera3DCursorMouseMode(object);

    mouseModeMgr_->startMode(mode);
  }
  else if (editType_ == EditType::MOVE) {
    auto *mode = new CQCamera3DMoveMouseMode;

    mouseModeMgr_->startMode(mode);
  }
  else if (editType_ == EditType::SCALE) {
    auto *mode = new CQCamera3DScaleMouseMode(object);

    mouseModeMgr_->startMode(mode);
  }
  else if (editType_ == EditType::ROTATE) {
    auto *mode = new CQCamera3DRotateMouseMode(object);

    mouseModeMgr_->startMode(mode);
  }
  else if (editType_ == EditType::CAMERA) {
    tipStr = QString("Camera: Shift=Position, Control=Origin");
  }
  else if (editType_ == EditType::LIGHT) {
    tipStr = QString("Light: Shift=Position, Control=Direction");
  }

  app_->status()->setTipLabel(tipStr);

  invalidate();

  Q_EMIT editTypeChanged();
}

void
CQCamera3DOverview::
setSelectType(const SelectType &v)
{
  selectType_ = v;

  Q_EMIT selectTypeChanged();
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
  auto zoomFactor = xview_.range.zoomFactor();

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

  for (auto *v : views_)
    v->range.zoomOut(zoomFactor);

  invalidate();
}

void
CQCamera3DOverview::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  drawData_.painter = &painter;

  //---

  auto *canvas = app_->canvas();
  auto *camera = canvas->getInteractiveCamera();

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
  painter.fillRect(rect(), QColor(220, 220, 220));

  //---

  auto drawPixelBorder = [&](ViewData &view) {
    bool current = (view.ind == ind_);

    QPen pen;
    pen.setColor(current ? Qt::red : Qt::black);
    pen.setWidthF(current ? 2 : 0);
    painter.setPen(pen);

    QBrush brush(current ? QColor(220, 220, 240) : QColor(220, 220, 220));
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

  //---

  if (! valid_)
    updateModel();

  //---

  drawData_.viewSortedPolygon2DArray.clear();
  drawData_.viewSortedLine2DArray   .clear();
  drawData_.viewSortedPoint2DArray  .clear();

  // draw geom data
  for (auto &pg : drawData_.objectGeomData) {
    const auto &geomData = pg.second;

    for (auto *faceData : geomData.faceDatas) {
      if (faceData->filled)
        drawData_.painter->setBrush(RGBAToQColor(faceData->fillColor));
      else
        drawData_.painter->setBrush(Qt::NoBrush);

      drawData_.painter->setPen(RGBAToQColor(faceData->strokeColor));

      drawModelPolygon(faceData->points);
    }

    for (auto *lineData : geomData.lineDatas) {
      drawData_.painter->setPen(RGBAToQColor(lineData->color));

      size_t i = 0;

      for (size_t j = 1; j < lineData->points.size(); i = j++) {
        auto p1 = lineData->points[i];
        auto p2 = lineData->points[j];

        drawModelLine(p1, p2);
      }
    }

    for (auto *vertexData : geomData.vertexDatas) {
      drawData_.painter->setPen(RGBAToQColor(vertexData->color));

      drawModelPoint(vertexData->p);
    }
  }

  for (auto &pv : drawData_.viewSortedPolygon2DArray) {
    auto *view = views_[pv.first];

    drawData_.painter->setClipRect(view->rect);

    for (auto &pp : pv.second) {
      for (auto &ppoints : pp.second) {
        drawData_.painter->drawPolygon(&ppoints[0], int(ppoints.size()));
      }
    }
  }

  for (auto &pv : drawData_.viewSortedPoint2DArray) {
    auto *view = views_[pv.first];

    drawData_.painter->setClipRect(view->rect);

    for (auto &pp : pv.second) {
      for (auto &point : pp.second) {
        auto px = point.x();
        auto py = point.y();

        drawData_.painter->drawLine(px - 4, py, px + 4, py);
        drawData_.painter->drawLine(px, py - 4, px, py + 4);
      }
    }
  }
}

void
CQCamera3DOverview::
updateModel()
{
  drawData_.filled = (modelType() == ModelType::SOLID);

  //---

  drawData_.useAnim = (app_->animName() != "");

  //---

  // reset geom data
  for (auto &pg : drawData_.objectGeomData) {
    const auto &geomData = pg.second;

    for (auto *faceData : geomData.faceDatas)
      delete faceData;

    for (auto *lineData : geomData.lineDatas)
      delete lineData;

    for (auto *vertexData : geomData.vertexDatas)
      delete vertexData;
  }

  drawData_.objectGeomData.clear();

  //---

  // add geom data
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

      geomData_ = &drawData_.objectGeomData[object];

      bbox_ = CBBox3D();

      return true;
    }

    void endVisitObject(CGeomObject3D *object) override {
      auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);
      assert(object1);

      object1->setBBox(bbox_);
    }

    bool beginVisitFace(CGeomFace3D *face) override {
      if (faceData_) {
        endVisitFace(faceData_->face);

        faceData_ = nullptr;
      }

      faceData_ = new FaceData;

      faceData_->face = const_cast<CGeomFace3D *>(face);

      return true;
    }

    void endVisitFace(CGeomFace3D *) override {
      if (! faceData_)
        return;

      auto *face1 = dynamic_cast<const CQCamera3DGeomFace *>(faceData_->face);

      const auto &selectedEdges = face1->selectedEdges();

      for (const auto &selectedEdge : selectedEdges) {
        const auto &edge = face1->edge(selectedEdge);

        LineData lineData;

        lineData.points.push_back(vertexPoint(edge.v1));
        lineData.points.push_back(vertexPoint(edge.v2));

        lineData.color = CRGBA::red();
      }

      //---

      faceData_->filled = drawData_.filled;

      if (faceData_->filled) {
        auto *faceMaterial = faceData_->face->getMaterialP();

        if (! faceMaterial && objectMaterial_)
          faceMaterial = objectMaterial_;

        faceData_->fillColor = faceData_->face->color().value_or(CRGBA::white());

        if (faceMaterial && faceMaterial->diffuse())
          faceData_->fillColor = faceMaterial->diffuse().value();
      }

      //---

      faceData_->stroked = true;

      bool selected = (object_->getHierSelected() || faceData_->face->getSelected());

      if (selected)
        faceData_->strokeColor = CRGBA::red();
      else
        faceData_->strokeColor = CRGBA(0, 0, 0, 0.1);

      //---

      geomData_->faceDatas.push_back(faceData_);

      faceData_ = nullptr;
    }

    void visitFaceVertex(CGeomVertex3D *vertex) override {
      auto p = vertexPoint(vertex);

      faceData_->points.push_back(p);

      bbox_ += drawData_.modelMatrix*p;

      if (vertex->isSelected()) {
        auto *vertexData = new VertexData;

        geomData_->vertexDatas.push_back(vertexData);

        vertexData->p     = p;
        vertexData->color = CRGBA::red();
      }
    }

    void visitLine(CGeomLine3D *line) override {
      auto *lineData = new LineData;

      geomData_->lineDatas.push_back(lineData);

      lineData->line = const_cast<CGeomLine3D *>(line);

      auto v1 = line->getStartInd();
      auto v2 = line->getEndInd  ();

      std::vector<uint> vertices;

      vertices.push_back(v1);
      vertices.push_back(v2);

      //--

      for (const auto &v : vertices) {
        auto p = vertexPoint(v);

        lineData->points.push_back(p);

        bbox_ += drawData_.modelMatrix*p;

        const auto &vertex = object_->getVertex(v);

        if (vertex.isSelected()) {
          auto *vertexData = new VertexData;

          geomData_->vertexDatas.push_back(vertexData);

          vertexData->p     = p;
          vertexData->color = CRGBA::red();
        }
      }

      //---

      bool selected = line->getSelected();

      if (selected)
        lineData->color = CRGBA::red();
      else
        lineData->color = CRGBA(0, 0, 0, 0.2);
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
    using GeomData = CQCamera3DOverview::GeomData;
    using FaceData = CQCamera3DOverview::FaceData;

    CQCamera3DOverview*           overview_ { nullptr };
    CQCamera3DOverview::DrawData& drawData_;
    CQCamera3DApp*                app_      { nullptr };

    CGeomMaterial* objectMaterial_ { nullptr };

    GeomData* geomData_ { nullptr };
    FaceData* faceData_ { nullptr };

    CBBox3D bbox_;

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
  if (! isCameraVisible() && editType_ != EditType::CAMERA)
    return;

  auto *canvas = app_->canvas();

#if 0
  for (auto *camera : canvas->cameras()) {
    if (camera->isPerspective())
      drawCamera(camera);
  }
#else
  auto *camera = canvas->getInteractiveCamera();

  drawCamera(camera);
#endif

  drawEyeLine();
}

void
CQCamera3DOverview::
drawCamera(CGLCameraIFace *camera)
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

  drawData_.painter->setPen(Qt::red);

  drawSphere(origin.point(), pos.point());
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

  drawCircle(p, r);
}

void
CQCamera3DOverview::
drawLights()
{
  if (! isLightsVisible() && editType_ != EditType::LIGHT)
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

      drawCircle(p, r);
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
  auto drawPolygon2D = [&](const ViewData &view, double pos, const std::vector<CPoint2D> &points) {
    Polygon2D ppoints;

    for (const auto &p : points) {
      double px, py;
      view.range.windowToPixel(p.x, p.y, &px, &py);

      ppoints.push_back(QPointF(px, py));
    }

    drawData_.viewSortedPolygon2DArray[view.ind][-pos].push_back(ppoints);
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

  auto c = pointsCenter(points);

  drawPolygon2D(xview_, c.z, xpoints);
  drawPolygon2D(yview_, c.x, ypoints);
  drawPolygon2D(zview_, c.y, zpoints);
  drawPolygon2D(pview_, c.z, ppoints);
}

void
CQCamera3DOverview::
drawModelLine(const CPoint3D &p1, const CPoint3D &p2) const
{
  auto drawLine2D = [&](const ViewData &view, double pos, const CPoint2D &p1, const CPoint2D &p2) {
    double px1, py1;
    view.range.windowToPixel(p1.x, p1.y, &px1, &py1);
    double px2, py2;
    view.range.windowToPixel(p2.x, p2.y, &px2, &py2);

    std::vector<QPointF> ppoints;
    ppoints.push_back(QPointF(px1, py1));
    ppoints.push_back(QPointF(px2, py2));

    drawData_.viewSortedLine2DArray[view.ind][-pos].push_back(ppoints);
  };

  auto pm1 = drawData_.modelMatrix*p1;
  auto pm2 = drawData_.modelMatrix*p2;

  auto c = (p1 + p2)/2.0;

  drawLine2D(xview_, c.z, CPoint2D(pm1.getX(), pm1.getY()), CPoint2D(pm2.getX(), pm2.getY())); // XY
  drawLine2D(yview_, c.x, CPoint2D(pm1.getZ(), pm1.getY()), CPoint2D(pm2.getZ(), pm2.getY())); // ZY
  drawLine2D(zview_, c.y, CPoint2D(pm1.getX(), pm1.getZ()), CPoint2D(pm2.getX(), pm2.getZ())); // XZ

  pm1 = drawData_.projectionMatrix*drawData_.viewMatrix*p1;
  pm2 = drawData_.projectionMatrix*drawData_.viewMatrix*p2;

  drawLine2D(pview_, c.z, CPoint2D(pm1.getX(), pm1.getY()), CPoint2D(pm2.getX(), pm2.getY()));
}

void
CQCamera3DOverview::
drawModelPoint(const CPoint3D &p) const
{
  auto drawPoint2D = [&](const ViewData &view, double pos, const CPoint2D &p) {
    double px, py;
    view.range.windowToPixel(p.x, p.y, &px, &py);

    drawData_.viewSortedPoint2DArray[view.ind][-pos].push_back(QPointF(px, py));
  };

  auto p1 = drawData_.modelMatrix*p;

  drawPoint2D(xview_, p.z, CPoint2D(p1.getX(), p1.getY())); // XY
  drawPoint2D(yview_, p.x, CPoint2D(p1.getZ(), p1.getY())); // ZY
  drawPoint2D(zview_, p.y, CPoint2D(p1.getX(), p1.getZ())); // XZ

  auto p2 = drawData_.projectionMatrix*drawData_.viewMatrix*p1;

  drawPoint2D(pview_, p.z, CPoint2D(p2.getX(), p2.getY()));
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
drawCircle(const CPoint3D &origin, double r)
{
  auto drawCircle2D = [&](const ViewData &view, const CPoint2D &o, double r) {
    drawData_.painter->setClipRect(view.rect);

    double px1, py1;
    view.range.windowToPixel(o.x - r, o.y - r, &px1, &py1);
    double px2, py2;
    view.range.windowToPixel(o.x + r, o.y + r, &px2, &py2);

    drawData_.painter->drawEllipse(QRectF(px1, py1, px2 - px1, py2 - py1));
  };

  auto xo = CPoint2D(origin.x, origin.y);
  auto yo = CPoint2D(origin.z, origin.y);
  auto zo = CPoint2D(origin.x, origin.z);

  drawCircle2D(xview_, xo, r); // XY
  drawCircle2D(yview_, yo, r); // ZY
  drawCircle2D(zview_, zo, r); // XZ
}

void
CQCamera3DOverview::
drawSphere(const CPoint3D &origin, const CPoint3D &pos)
{
  auto drawCircle2D = [&](const ViewData &view, const CPoint2D &o, double r) {
    drawData_.painter->setClipRect(view.rect);

    double px1, py1;
    view.range.windowToPixel(o.x - r, o.y - r, &px1, &py1);
    double px2, py2;
    view.range.windowToPixel(o.x + r, o.y + r, &px2, &py2);

    drawData_.painter->drawEllipse(QRectF(px1, py1, px2 - px1, py2 - py1));
  };

  auto x1 = CPoint2D(origin.x, origin.y); auto x2 = CPoint2D(pos.x, pos.y);
  auto y1 = CPoint2D(origin.z, origin.y); auto y2 = CPoint2D(pos.z, pos.y);
  auto z1 = CPoint2D(origin.x, origin.z); auto z2 = CPoint2D(pos.x, pos.z);

  auto rx = x1.distanceTo(x2);
  auto ry = y1.distanceTo(y2);
  auto rz = z1.distanceTo(z2);

  drawCircle2D(xview_, x1, rx); // XY
  drawCircle2D(yview_, y1, ry); // ZY
  drawCircle2D(zview_, z1, rz); // XZ
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
  auto *mode = mouseModeMgr_->currentMode();

  if (mode)
    return mode->mousePress(e);

  //---

  mouseData_.pressed = true;
  mouseData_.button  = e->button();

  mouseData_.isShift   = (e->modifiers() & Qt::ShiftModifier);
  mouseData_.isControl = (e->modifiers() & Qt::ControlModifier);

  mouseData_.pressPixel = e->pos();
  mouseData_.movePixel  = mouseData_.pressPixel;

  //---

  int x = e->x();
  int y = e->y();

  if (mouseData_.button == Qt::LeftButton) {
    if      (editType() == EditType::SELECT) {
      rubberBand_->setBounds(mouseData_.pressPixel, mouseData_.movePixel);
      rubberBand_->show();
    }
    else if (editType() == EditType::CURSOR) {
      setCursorPosition(x, y);
    }
    else if (editType() == EditType::CAMERA) {
      if      (mouseData_.isShift)
        setCameraPosition(x, y);
      else if (mouseData_.isControl)
        setCameraOrigin(x, y);
    }
    else if (editType() == EditType::LIGHT) {
      if      (mouseData_.isShift)
        setLightPosition(x, y);
      else if (mouseData_.isControl)
        setLightDirection(x, y);
    }
  }
}

void
CQCamera3DOverview::
mouseMoveEvent(QMouseEvent *e)
{
  auto *mode = mouseModeMgr_->currentMode();

  if (mode)
    return mode->mouseMove(e);

  //---

  mouseData_.movePixel = e->pos();

  //---

  int x = e->x();
  int y = e->y();

  if (mouseData_.pressed) {
    if (mouseData_.button == Qt::LeftButton) {
      if      (editType() == EditType::SELECT) {
        rubberBand_->setBounds(mouseData_.pressPixel, mouseData_.movePixel);
      }
      else if (editType() == EditType::CAMERA) {
        if      (mouseData_.isShift)
          setCameraPosition(x, y);
        else if (mouseData_.isControl)
          setCameraOrigin(x, y);
      }
      else if (editType() == EditType::LIGHT) {
        if      (mouseData_.isShift)
          setLightPosition(x, y);
        else if (mouseData_.isControl)
          setLightDirection(x, y);
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
      invalidate();
  }
}

void
CQCamera3DOverview::
mouseReleaseEvent(QMouseEvent *e)
{
  auto *mode = mouseModeMgr_->currentMode();

  if (mode)
    return mode->mouseRelease(e);

  //---

  mouseData_.movePixel = e->pos();

  if (mouseData_.button == Qt::LeftButton) {
    if (editType() == EditType::SELECT) {
      bool clear = ! mouseData_.isControl;

      int dx = std::abs(mouseData_.pressPixel.x() - mouseData_.movePixel.x());
      int dy = std::abs(mouseData_.pressPixel.y() - mouseData_.movePixel.y());

      if (dx < 4 && dy < 4)
        selectObjectAt(mouseData_.pressPixel, clear);
      else
        selectObjectIn(mouseData_.pressPixel, rubberBand_->bounds(), clear);

      rubberBand_->hide();
    }
  }

  mouseData_.pressed = false;
}

void
CQCamera3DOverview::
wheelEvent(QWheelEvent *e)
{
  auto *mode = mouseModeMgr_->currentMode();

  if (mode)
    return mode->wheelEvent(e);

  //---

  auto dw = e->angleDelta().y()/250.0;

  if      (dw < 0) {
    for (auto *v : views_)
      v->range.zoomOut(1.05);
  }
  else if (dw > 0) {
    for (auto *v : views_)
      v->range.zoomIn(1.05);
  }

  invalidate();
}

void
CQCamera3DOverview::
keyPressEvent(QKeyEvent *e)
{
  if (e->key() == Qt::Key_Escape) {
    mouseModeMgr_->endAllModes();
    return;
  }

  //---

  auto *mode = mouseModeMgr_->currentMode();

  if (mode) {
    auto pos = this->mapFromGlobal(QCursor::pos());

    mouseModeIFace_->setViewType(pixelToViewType(pos));

    return mode->keyPress(e);
  }

  //---

  auto *canvas = app_->canvas();
  auto *camera = canvas->getInteractiveCamera();

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

  invalidate();
}

void
CQCamera3DOverview::
selectObjectAt(const QPoint &p, bool clear)
{
  auto *canvas = app_->canvas();
  auto *camera = canvas->getInteractiveCamera();

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
    std::set<CGeomObject3D *> selectObjectSet;

    for (auto &pg : drawData_.objectGeomData) {
      auto *object = pg.first;

      auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);
      assert(object1);

      auto modelMatrix = CMatrix3DH(object1->getHierTransform());

      const auto &geomData = pg.second;

      //---

      bool intersect = false;

      for (auto *faceData : geomData.faceDatas) {
        QPolygonF poly;

        for (const auto &pf : faceData->points) {
          auto pf1 = viewQPoint(viewType, modelMatrix*pf);

          poly << pf1;
        }

        if (poly.intersects(QPolygonF(r1))) {
          intersect = true;
          break;
        }
      }

      if (intersect)
        selectObjectSet.insert(object);
    }

    std::vector<CGeomObject3D *> selectObjects;

    for (auto *object : selectObjectSet)
      selectObjects.push_back(object);

    canvas->selectObjects(selectObjects, clear, /*update*/true);
  }
  else if (selectType == SelectType::FACE) {
    std::vector<CGeomFace3D *> selectFaces;

    for (auto &pg : drawData_.objectGeomData) {
      auto *object = pg.first;

      auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);
      assert(object1);

      auto modelMatrix = CMatrix3DH(object1->getHierTransform());

      const auto &geomData = pg.second;

      //---

      for (auto *faceData : geomData.faceDatas) {
        QPolygonF poly;

        for (const auto &pf : faceData->points) {
          auto pf1 = viewQPoint(viewType, modelMatrix*pf);

          poly << pf1;
        }

        if (poly.intersects(QPolygonF(r1)))
          selectFaces.push_back(faceData->face);
      }
    }

    canvas->selectFaces(selectFaces, clear, /*update*/true);
  }
  else if (selectType == SelectType::EDGE) {
  }
  else if (selectType == SelectType::POINT) {
    CQCamera3DCanvas::ObjectSelectInds selectInds;

    for (auto &pg : drawData_.objectGeomData) {
      auto *object = pg.first;

      auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);
      assert(object1);

      auto modelMatrix = CMatrix3DH(object1->getHierTransform());

      const auto &geomData = pg.second;

      //---

      for (auto *faceData : geomData.faceDatas) {
        int i = 0;

        for (const auto &pf : faceData->points) {
          auto pf1 = viewQPoint(viewType, modelMatrix*pf);

          if (r1.contains(pf1)) {
            auto vInd = faceData->face->getVertex(i);

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

  for (auto &pg : drawData_.objectGeomData) {
    auto *object = pg.first;

    auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);
    assert(object1);

    auto modelMatrix = CMatrix3DH(object1->getHierTransform());

    const auto &geomData = pg.second;

    //---

    for (auto *faceData : geomData.faceDatas) {
      QPolygonF poly;

      for (const auto &pf : faceData->points) {
        auto pf1 = viewQPoint(view.type, modelMatrix*pf);

        poly.push_back(pf1);
      }

      if (poly.containsPoint(p1, Qt::WindingFill))
        atFaces[polygonArea(poly)] = const_cast<FaceData *>(faceData);
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

    canvas->selectObject(face->getObject(), clear, /*update*/true);
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
      canvas->selectFaceEdge(face, minInd, clear, /*update*/true);
  }
  else if (selectType == SelectType::POINT) {
    auto pm = viewPoint(view.type, p);

    int    minInd  = -1;
    double minDist = 0.0;

    CGeomFace3D *face = nullptr;

    for (auto &pg : drawData_.objectGeomData) {
      auto *object = pg.first;

      auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);
      assert(object1);

      auto modelMatrix = CMatrix3DH(object1->getHierTransform());

      const auto &geomData = pg.second;

      //---

      for (auto *faceData : geomData.faceDatas) {
        int i = 0;

        for (const auto &pf : faceData->points) {
          auto pf1 = viewPoint(view.type, modelMatrix*pf);

          auto d = pf1.distanceTo(pm);

          if (! face || d < minDist) {
            face    = faceData->face;
            minInd  = i;
            minDist = d;
          }

          ++i;
        }
      }
    }

    auto  vInd = face->getVertex(minInd);
    auto &v    = face->getObject()->getVertex(vInd);

    canvas->selectVertex(&v, clear, /*update*/true);
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
setCameraPosition(int x, int y)
{
  auto *camera = app_->canvas()->getInteractiveCamera();

  auto pos = camera->position();

  CPoint2D p;

  if (xview_.pressRange(x, y, p)) camera->setPosition(CVector3D(p.x, p.y, pos.z())); // XY
  if (yview_.pressRange(x, y, p)) camera->setPosition(CVector3D(pos.x(), p.y, p.x)); // ZY
  if (zview_.pressRange(x, y, p)) camera->setPosition(CVector3D(p.x, pos.y(), p.y)); // XZ

  invalidate();
}

void
CQCamera3DOverview::
setCameraOrigin(int x, int y)
{
  auto *camera = app_->canvas()->getInteractiveCamera();

#if 0
  CPoint2D p;

  auto pointAngle = [&](double x, double y) { return CVector2D(CPoint2D(x, y), p).angle(); };

  auto pos = camera->position();

  if (xview_.pressRange(x, y, p)) camera->setRoll (pointAngle(pos.x(), pos.y())); // XY
  if (yview_.pressRange(x, y, p)) camera->setPitch(pointAngle(pos.z(), pos.y())); // ZY
  if (zview_.pressRange(x, y, p)) camera->setYaw  (pointAngle(pos.x(), pos.z()) - M_PI/2.0); // XZ
#else
  auto origin = camera->origin();

  CPoint2D p;

#if 0
  if (xview_.pressRange(x, y, p)) camera->setOriginXY(CVector3D(p.x, p.y, origin.z())); // XY
  if (yview_.pressRange(x, y, p)) camera->setOriginYZ(CVector3D(origin.x(), p.y, p.x)); // ZY
  if (zview_.pressRange(x, y, p)) camera->setOriginXZ(CVector3D(p.x, origin.y(), p.y)); // XZ
#else
  if (xview_.pressRange(x, y, p)) camera->setOrigin(CVector3D(p.x, p.y, origin.z())); // XY
  if (yview_.pressRange(x, y, p)) camera->setOrigin(CVector3D(origin.x(), p.y, p.x)); // ZY
  if (zview_.pressRange(x, y, p)) camera->setOrigin(CVector3D(p.x, origin.y(), p.y)); // XZ
#endif
#endif

  invalidate();
}

void
CQCamera3DOverview::
setLightPosition(int x, int y)
{
  auto *canvas = app_->canvas();
  auto *light  = canvas->currentLight();

  auto pos = CVector3D(light->getPosition());

  CPoint2D p;

  if (xview_.pressRange(x, y, p)) light->setPosition(CPoint3D(p.x, p.y, pos.z())); // XY
  if (yview_.pressRange(x, y, p)) light->setPosition(CPoint3D(pos.x(), p.y, p.x)); // ZY
  if (zview_.pressRange(x, y, p)) light->setPosition(CPoint3D(p.x, pos.y(), p.y)); // XZ

  canvas->update();

  invalidate();
}

void
CQCamera3DOverview::
setLightDirection(int x, int y)
{
  auto *canvas = app_->canvas();
  auto *light  = canvas->currentLight();

  auto pos = CVector3D(light->getPosition());

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

  canvas->update();

  invalidate();
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

  invalidate();
}

CQCamera3DOverview::ViewType
CQCamera3DOverview::
pixelToViewType(const QPointF &p) const
{
  ViewType viewType { ViewType::NONE };

  CPoint2D p1;

  if      (xview_.pressRange(p.x(), p.y(), p1))
    viewType = xview_.type;
  else if (yview_.pressRange(p.x(), p.y(), p1))
    viewType = yview_.type;
  else if (zview_.pressRange(p.x(), p.y(), p1))
    viewType = zview_.type;

  return viewType;
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

//---

void
CQCamera3DOverview::
setOptions(CQCamera3DOptions *options)
{
  opWidget_->setOptions(options);
}

void
CQCamera3DOverview::
showOptions()
{
  opWidget_->show();
}

void
CQCamera3DOverview::
hideOptions()
{
  opWidget_->hide();
}

//---

CGeomObject3D *
CQCamera3DOverview::
addCircle()
{
  return addObject("circle", AddObjectType::CIRCLE);
}

CGeomObject3D *
CQCamera3DOverview::
addCone()
{
  return addObject("cone", AddObjectType::CONE);
}

CGeomObject3D *
CQCamera3DOverview::
addCube()
{
  return addObject("cube", AddObjectType::CUBE);
}

CGeomObject3D *
CQCamera3DOverview::
addCylinder()
{
  return addObject("cylinder", AddObjectType::CYLINDER);
}

CGeomObject3D *
CQCamera3DOverview::
addPlane()
{
  return addObject("plane", AddObjectType::PLANE);
}

CGeomObject3D *
CQCamera3DOverview::
addPyramid()
{
  return addObject("pyramid", AddObjectType::PYRAMID);
}

CGeomObject3D *
CQCamera3DOverview::
addSphere()
{
  return addObject("sphere", AddObjectType::SPHERE);
}

CGeomObject3D *
CQCamera3DOverview::
addTorus()
{
  return addObject("torus", AddObjectType::TORUS);
}

CGeomObject3D *
CQCamera3DOverview::
addObject(const std::string &typeName, const AddObjectType &type)
{
  auto *scene = app_->getScene();

  auto n = scene->getObjects().size();
  std::string name = typeName + "." + std::to_string(n + 1);

  auto *object = CGeometry3DInst->createObject3D(scene, name);

  scene->addObject(object);

  auto *mode = new CQCamera3DAddMouseMode(type, object);

  mouseModeMgr_->startMode(mode);

  mode->updateObject();

  Q_EMIT objectAdded();

  auto *canvas = app_->canvas();

  canvas->selectObject(object, /*clear*/true, /*update*/true);

  return object;
}
