#include <CQCamera3DBones.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DCamera.h>
#include <CQCamera3DLight.h>
#include <CQCamera3DStatus.h>
#include <CQCamera3DGeomObject.h>
#include <CQCamera3DUtil.h>

#include <CQRubberBand.h>
#include <CGeomScene3D.h>
#include <CGeomObject3D.h>
#include <CGeomNodeData.h>
#include <CLine3D.h>
#include <CMatrix2D.h>

#include <QMouseEvent>

CQCamera3DBones::
CQCamera3DBones(CQCamera3DApp *app) :
 app_(app)
{
  setObjectName("bones");

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  setFocusPolicy(Qt::StrongFocus);

  setMouseTracking(true);

  //---

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

  auto *canvas = app_->canvas();

  connect(app_, SIGNAL(animStateChanged()), this, SLOT(invalidate()));
  connect(app_, SIGNAL(animTimeChanged()), this, SLOT(invalidate()));

  connect(canvas, SIGNAL(stateChanged()), this, SLOT(invalidate()));
  connect(canvas, SIGNAL(objectAdded()), this, SLOT(invalidate()));
  connect(canvas, SIGNAL(cameraStateChanged()), this, SLOT(invalidate()));
}

void
CQCamera3DBones::
invalidate()
{
  bboxSet_ = false;

  update();
}

void
CQCamera3DBones::
resizeEvent(QResizeEvent *)
{
  updateRange();
}

void
CQCamera3DBones::
updateRange()
{
  auto zoomFactor = xview_.range.zoomFactor();
  auto xOffset    = xview_.range.getOffsetX();
  auto yOffset    = xview_.range.getOffsetY();
  auto zOffset    = yview_.range.getOffsetX();

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
    v->range.setZoomFactor(zoomFactor);

  xview_.range.setOffsetX(xOffset); // XY
  xview_.range.setOffsetY(yOffset);

  yview_.range.setOffsetX(zOffset); // ZY
  yview_.range.setOffsetY(yOffset);

  zview_.range.setOffsetX(xOffset); // XZ
  zview_.range.setOffsetY(zOffset);

  invalidate();
}

void
CQCamera3DBones::
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
    auto zoomFactor = xview_.range.zoomFactor();
    auto xOffset    = xview_.range.getOffsetX();
    auto yOffset    = xview_.range.getOffsetY();
    auto zOffset    = yview_.range.getOffsetX();

    if (isAutoFit() && updateBonesBBox()) {
      CPoint2D c, s;

      c = xview_.bbox.getCenter();
      s = xview_.bbox.getSize().point();
      xview_.range.setWindowRange(c.x - s.x, c.y - s.y, c.x + s.x, c.y + s.y); // XY

      c = yview_.bbox.getCenter();
      s = yview_.bbox.getSize().point();
      yview_.range.setWindowRange(c.x - s.x, c.y - s.y, c.x + s.x, c.y + s.y); // ZY

      c = zview_.bbox.getCenter();
      s = zview_.bbox.getSize().point();
      zview_.range.setWindowRange(c.x - s.x, c.y - s.y, c.x + s.x, c.y + s.y); // XZ
    }
    else {
      bbox_ = canvas->bbox();

      auto c = bbox_.getCenter();

      xs_ = bbox_.getXSize();
      ys_ = bbox_.getYSize();
      zs_ = bbox_.getZSize();

      xview_.range.setWindowRange(c.x - xs_, c.y - ys_, c.x + xs_, c.y + ys_); // XY
      yview_.range.setWindowRange(c.z + zs_, c.y - ys_, c.z - zs_, c.y + ys_); // ZY
      zview_.range.setWindowRange(c.x - xs_, c.z + zs_, c.x + xs_, c.z - zs_); // XZ
    }

    pview_.range.setWindowRange(-1, -1, 1, 1); // 3D

    for (auto *v : views_)
      v->range.setZoomFactor(zoomFactor);

    xview_.range.setOffsetX(xOffset); // XY
    xview_.range.setOffsetY(yOffset);

    yview_.range.setOffsetX(zOffset); // ZY
    yview_.range.setOffsetY(yOffset);

    zview_.range.setOffsetX(xOffset); // XZ
    zview_.range.setOffsetY(zOffset);

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

  drawBBox(bbox_);

  //---

  drawBones();

  if (isShowModel() || isShowBoneNodes())
    drawModel();

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
CQCamera3DBones::
drawModel()
{
  auto *object = app_->currentBoneObject();

  drawHierModel(object);

  if (! isShowModel() && isShowBoneNodes()) {
    int currentBoneNode = app_->currentBoneNode();

    auto *animObject = object->getAnimObject();

    if (animObject) {
      auto &node = animObject->getNode(currentBoneNode);

      if (node.object() && node.object() != animObject) {
        showModel_ = ! showModel_;

        drawHierModel(const_cast<CGeomObject3D *>(node.object()));

        showModel_ = ! showModel_;
      }
    }
  }
}

void
CQCamera3DBones::
drawHierModel(CGeomObject3D *object)
{
  drawObjectModel(object);

  for (auto *child : object->children())
    drawHierModel(child);
}

void
CQCamera3DBones::
drawObjectModel(CGeomObject3D *object)
{
  if (! object->getVisible())
    return;

  //---

  struct JointNode {
    CGeomObject3D* object { nullptr };
    int            nodeId { -1 };
  };

  std::vector<JointNode      > selectedJointNodes;
  std::vector<CGeomVertex3D *> selectedBoneVertices;

  //---

  int currentBoneNode = app_->currentBoneNode();

  //---

  // draw model
  drawData_.painter->setPen(QColor(0, 0, 0, 32));
  drawData_.painter->setBrush(Qt::NoBrush);

  //---

  drawData_.modelMatrix = CMatrix3DH(object->getHierTransform());
  drawData_.meshMatrix  = CMatrix3DH(object->getMeshGlobalTransform());

  //---

  auto *animObject = object->getAnimObject();

  auto animName = (animObject ? animObject->animName() : "");

  bool useAnim = false;

  if (app_->isAnimEnabled())
    useAnim = (animObject && animName != "");

  if (useAnim) {
    auto meshNodeId = object->getMeshNode();

    CGeomNodeData *node = nullptr;

    if (meshNodeId >= 0)
      node = const_cast<CGeomNodeData *>(&animObject->getNode(meshNodeId));

    auto isJointed = (node && object->isJointed());

    if (node && ! isJointed) {
      auto animTime = animObject->animTime();

      drawData_.meshMatrix =
        CMatrix3DH(object->getNodeAnimHierTransform(*node, animName, animTime));
    }
  }

  //---

  auto *nodeMatrices = (useAnim ? &app_->getObjectNodeMatrices(animObject) : nullptr);

  //---

  const auto &faces = object->getFaces();

  for (const auto *face : faces) {
    if (! face->getVisible())
      continue;

    // get face points
    std::vector<CPoint3D> points;

    const auto &vertices = face->getVertices();

    for (const auto &v : vertices) {
      const auto &vertex = object->getVertex(v);
      const auto &model  = vertex.getModel();

      auto p = model;

      //---

      if (useAnim) {
        if (vertex.hasJointData())
          p = app_->adjustAnimPoint(vertex, p, *nodeMatrices);
      }

      p = drawData_.meshMatrix*p;

      //---

      points.push_back(p);

      //---

      // process vertex joint data
      const auto &jointData = vertex.getJointData();

      for (int i = 0; i < 4; ++i) {
        const auto &jointNodeData = jointData.nodeDatas[i];

        int nodeId = -1;

        if (jointNodeData.node >= 0)
          nodeId = animObject->mapNodeIndex(jointNodeData.node);

        if (nodeId >= 0) {
          JointNode jointNode;

          jointNode.object = object;
          jointNode.nodeId = nodeId;

          // add selected vertex joint nodes
          if (vertex.isSelected())
            selectedJointNodes.push_back(jointNode);

          // add selected joint vertices
          if (nodeId == currentBoneNode) {
            selectedBoneVertices.push_back(const_cast<CGeomVertex3D *>(&vertex));
          }
        }
      }
    }

    //---

    if (isShowModel())
      drawModelPolygon(points);
  }

  //---

  // draw joint points
  if (isShowPointJoints()) {
    drawData_.painter->setPen(Qt::red);
    drawData_.painter->setBrush(Qt::white);

    for (auto &jointNode : selectedJointNodes) {
      auto *animObject = jointNode.object->getAnimObject();

      auto &node = animObject->getNode(jointNode.nodeId);

      if (isOnlyJoints() && ! node.isJoint())
        continue;

      auto m = getNodeTransform(animObject, const_cast<CGeomNodeData &>(node));

      auto c = m*CPoint3D(0.0, 0.0, 0.0);

      auto s = bbox_.getMaxSize()/100.0;

      drawCircle(CVector3D(c), s);
    }
  }

  //---

  // draw bone nodes
  if (isShowBoneNodes()) {
    // TODO: cache object data (update on change)

    drawData_.painter->setPen(Qt::green);
    drawData_.painter->setBrush(Qt::white);

    for (auto *v : selectedBoneVertices) {
      auto *object = v->getObject();

      auto *animObject = object->getAnimObject();

      bool useAnim = (animObject && animObject->animName() != "");

      drawData_.modelMatrix = CMatrix3DH(object->getHierTransform());
      drawData_.meshMatrix  = CMatrix3DH(object->getMeshGlobalTransform());

      const auto &model = v->getModel();

      auto p = model;

      if (useAnim) {
        if (v->hasJointData()) {
          auto *nodeMatrices = &app_->getObjectNodeMatrices(animObject);

          p = app_->adjustAnimPoint(*v, p, *nodeMatrices);
        }
      }

      p = drawData_.modelMatrix*drawData_.meshMatrix*p;

      auto s = bbox_.getMaxSize()/100.0;

      drawCircle(CVector3D(p), s);
    }
  }
}

bool
CQCamera3DBones::
updateBonesBBox()
{
  bbox_ = CBBox3D();

  xview_.bbox = CBBox2D();
  yview_.bbox = CBBox2D();
  zview_.bbox = CBBox2D();

  auto *animObject = app_->currentBoneObject();
  if (! animObject) return false;

  for (const auto &pn : animObject->getNodes()) {
    auto &node = const_cast<CGeomNodeData &>(pn.second);

    if (isOnlyJoints() && ! node.isJoint())
      continue;

    //---

#if 1
    auto *meshObject  = node.animObject()->getMeshObject();
    auto  meshMatrix  = CMatrix3DH(meshObject->getMeshGlobalTransform());
#else
    auto  meshMatrix  = CMatrix3DH::identity();
#endif
    auto  modelMatrix = CMatrix3DH(animObject->getHierTransform());

    //---

    // ignore anim for constant range
    //auto m = getNodeTransform(animObject, node);
    auto m = animObject->getNodeHierTransform(node);

    auto c = m*CPoint3D(0.0, 0.0, 0.0);

    //c = meshMatrix*c;
    c = modelMatrix*c;

    updateBBox(c);
  }

  return true;
}

void
CQCamera3DBones::
updateBBox(const CPoint3D &p)
{
  bbox_ += p;

  xview_.bbox += CPoint2D(p.x, p.y); // XY
  yview_.bbox += CPoint2D(p.z, p.y); // ZY
  zview_.bbox += CPoint2D(p.x, p.z); // XZ
}

void
CQCamera3DBones::
drawBones()
{
  auto *currentBoneObject = app_->currentBoneObject();

  if (currentBoneObject)
    drawBoneObject(currentBoneObject);
}

void
CQCamera3DBones::
drawBoneObject(CGeomObject3D *animObject)
{
  int currentBoneNode = app_->currentBoneNode();

  drawData_.painter->setPen(Qt::black);

  nodeCenters_.clear();

  drawData_.modelMatrix = CMatrix3DH(animObject->getHierTransform());

  //---

  for (const auto &pn : animObject->getNodes()) {
    auto &node = const_cast<CGeomNodeData &>(pn.second);

    if (isOnlyJoints() && ! node.isJoint())
      continue;

    //---

    auto *meshObject = node.animObject()->getMeshObject();
    auto  meshMatrix = CMatrix3DH(meshObject->getMeshGlobalTransform());

    //---

    bool isSelected = (node.ind() == currentBoneNode);

    drawData_.painter->setBrush(isSelected ? Qt::red: Qt::white);

    //---

    auto m = getNodeTransform(animObject, node);

    auto c = m*CPoint3D(0.0, 0.0, 0.0);

    //c = meshMatrix*c;

    auto s = bbox_.getMaxSize()/40.0;

    drawModelCube(c, s);

#if 0
    if (isSelected)
      drawModelPoint(CVector3D(c), QString::fromStdString(node.name()));
#endif

    nodeCenters_[node.ind()] = c;

    if (node.parent() >= 0) {
      auto &pnode = const_cast<CGeomNodeData &>(animObject->getNode(node.parent()));

      if (pnode.isValid() && (! isOnlyJoints() || pnode.isJoint())) {
        auto *pmeshObject = pnode.animObject()->getMeshObject();
        auto  pmeshMatrix = CMatrix3DH(pmeshObject->getMeshGlobalTransform());

        auto m1 = getNodeTransform(animObject, pnode);

        auto c1 = m1*CPoint3D(0.0, 0.0, 0.0);

        //c1 = pmeshMatrix*c1;

        drawModelLine(c, c1);
      }
    }
  }
}

void
CQCamera3DBones::
drawModelCube(const CPoint3D &c, double s) const
{
  auto c1 = drawData_.modelMatrix*c;

  //---

  static int cube_faces[6][4] = {
    {0, 1, 2, 3},
    {3, 2, 6, 7},
    {7, 6, 5, 4},
    {4, 5, 1, 0},
    {5, 6, 2, 1},
    {7, 4, 0, 3}
  };

  double v[8][3];

  v[0][0] = v[1][0] = v[2][0] = v[3][0] = -0.5;
  v[4][0] = v[5][0] = v[6][0] = v[7][0] =  0.5;
  v[0][1] = v[1][1] = v[4][1] = v[5][1] = -0.5;
  v[2][1] = v[3][1] = v[6][1] = v[7][1] =  0.5;
  v[0][2] = v[3][2] = v[4][2] = v[7][2] = -0.5;
  v[1][2] = v[2][2] = v[5][2] = v[6][2] =  0.5;

  auto cubePoint = [&](int iface, int ipoint) {
    const auto &v1 = v[cube_faces[iface][ipoint]];

    return CPoint3D(c1.x + s*v1[0], c1.y + s*v1[1], c1.z + s*v1[2]);
  };

  for (int i = 0; i < 6; ++i) {
    std::vector<CPoint3D> points;

    auto p1 = cubePoint(i, 0);
    auto p2 = cubePoint(i, 1);
    auto p3 = cubePoint(i, 2);
    auto p4 = cubePoint(i, 3);

    points.push_back(p1);
    points.push_back(p2);
    points.push_back(p3);
    points.push_back(p4);

    drawPolygon(points);
  }
}

void
CQCamera3DBones::
drawModelPolygon(const std::vector<CPoint3D> &points) const
{
  drawPolygon(points, /*model*/true);
}

void
CQCamera3DBones::
drawPolygon(const std::vector<CPoint3D> &points, bool model) const
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
    CPoint3D p1;

    if (model)
      p1 = drawData_.modelMatrix*p;
    else
      p1 = p;

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
CQCamera3DBones::
drawModelLine(const CPoint3D &p1, const CPoint3D &p2, const QString &label) const
{
  auto drawLine2D = [&](const ViewData &view, const CPoint2D &p1,
                        const CPoint2D &p2, const QString &label) {
    drawData_.painter->setClipRect(view.rect);

    double px1, py1;
    view.range.windowToPixel(p1.x, p1.y, &px1, &py1);
    double px2, py2;
    view.range.windowToPixel(p2.x, p2.y, &px2, &py2);

    drawData_.painter->drawLine(px1, py1, px2, py2);

    if (label != "")
      drawData_.painter->drawText(px2, py2, label);
  };

  auto p11 = drawData_.modelMatrix*p1;
  auto p21 = drawData_.modelMatrix*p2;

  auto x1 = p11.getX(), y1 = p11.getY(), z1 = p11.getZ();
  auto x2 = p21.getX(), y2 = p21.getY(), z2 = p21.getZ();

  drawLine2D(xview_, CPoint2D(x1, y1), CPoint2D(x2, y2), label); // XY
  drawLine2D(yview_, CPoint2D(z1, y1), CPoint2D(z2, y2), label); // ZY
  drawLine2D(zview_, CPoint2D(x1, z1), CPoint2D(x2, z2), label); // XZ

  p11 = drawData_.projectionMatrix*drawData_.viewMatrix*drawData_.modelMatrix*p1;
  p21 = drawData_.projectionMatrix*drawData_.viewMatrix*drawData_.modelMatrix*p2;

  x1 = p11.getX(), y1 = p11.getY();
  x2 = p21.getX(), y2 = p21.getY();

  drawLine2D(pview_, CPoint2D(x1, y1), CPoint2D(x2, y2), label);
}

void
CQCamera3DBones::
drawBBox(const CBBox3D &bbox)
{
  drawData_.painter->setPen(QColor(0, 0, 0, 10));

  auto drawBBox2D = [&](const ViewData &view, const CBBox2D &bbox) {
    drawData_.painter->setClipRect(view.rect);

    double px1, py1;
    view.range.windowToPixel(bbox.getXMin(), bbox.getYMin(), &px1, &py1);
    double px2, py2;
    view.range.windowToPixel(bbox.getXMax(), bbox.getYMax(), &px2, &py2);

    drawData_.painter->drawRect(QRectF(px1, py1, px2 - px1, py2 - py1));
  };

  drawBBox2D(xview_, CBBox2D(bbox.getXMin(), bbox.getYMin(),
                             bbox.getXMax(), bbox.getYMax())); // XY
  drawBBox2D(yview_, CBBox2D(bbox.getZMin(), bbox.getYMin(),
                             bbox.getZMax(), bbox.getYMax())); // ZY
  drawBBox2D(zview_, CBBox2D(bbox.getXMin(), bbox.getZMin(),
                             bbox.getXMax(), bbox.getZMax())); // XZ
}

void
CQCamera3DBones::
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
CQCamera3DBones::
drawPoint(const CVector3D &p, const QString &label) const
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

  drawPoint2D(xview_, CPoint2D(p.getX(), p.getY()), label); // XY
  drawPoint2D(yview_, CPoint2D(p.getZ(), p.getY()), label); // ZY
  drawPoint2D(zview_, CPoint2D(p.getX(), p.getZ()), label); // XZ
}

void
CQCamera3DBones::
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
CQCamera3DBones::
mousePressEvent(QMouseEvent *e)
{
  mouseData_.pressed = true;
  mouseData_.button  = e->button();

  mouseData_.isShift   = (e->modifiers() & Qt::ShiftModifier);
  mouseData_.isControl = (e->modifiers() & Qt::ControlModifier);

  mouseData_.pressPixel = e->pos();
  mouseData_.movePixel1 = mouseData_.pressPixel;

  //---

  if (mouseData_.button == Qt::LeftButton) {
    rubberBand_->setBounds(mouseData_.pressPixel, mouseData_.movePixel1);
    rubberBand_->show();
  }
}

void
CQCamera3DBones::
mouseMoveEvent(QMouseEvent *e)
{
  mouseData_.movePixel2 = e->pos();

  //---

  int x = e->x();
  int y = e->y();

  if (mouseData_.pressed) {
    if      (mouseData_.button == Qt::LeftButton) {
      rubberBand_->setBounds(mouseData_.pressPixel, mouseData_.movePixel2);
    }
    else if (mouseData_.button == Qt::MiddleButton) {
      double dx = 0.0;
      double dy = 0.0;
      double dz = 0.0;

      CPoint2D p2;
      if      (xview_.pressRange(mouseData_.movePixel2.x(), mouseData_.movePixel2.y(), p2)) { // XY
        CPoint2D p1;
        xview_.pressRange(mouseData_.movePixel1.x(), mouseData_.movePixel1.y(), p1);

        dx = p1.x - p2.x;
        dy = p1.y - p2.y;
      }
      else if (yview_.pressRange(mouseData_.movePixel2.x(), mouseData_.movePixel2.y(), p2)) { // ZY
        CPoint2D p1;
        yview_.pressRange(mouseData_.movePixel1.x(), mouseData_.movePixel1.y(), p1);

        dz = p1.x - p2.x;
        dy = p1.y - p2.y;
      }
      else if (zview_.pressRange(mouseData_.movePixel2.x(), mouseData_.movePixel2.y(), p2)) { // XZ
        CPoint2D p1;
        zview_.pressRange(mouseData_.movePixel1.x(), mouseData_.movePixel1.y(), p1);

        dx = p1.x - p2.x;
        dz = p1.y - p2.y;
      }

      xview_.range.scroll(dx, dy);
      yview_.range.scroll(dz, dy);
      zview_.range.scroll(dx, dz);

      invalidate();
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

  mouseData_.movePixel1 = mouseData_.movePixel2;
}

void
CQCamera3DBones::
mouseReleaseEvent(QMouseEvent *e)
{
   mouseData_.movePixel2 = e->pos();

  if (mouseData_.button == Qt::LeftButton) {
    selectNode(mouseData_.pressPixel.x(), mouseData_.pressPixel.y());

    rubberBand_->hide();
  }

  mouseData_.pressed = false;
}

void
CQCamera3DBones::
wheelEvent(QWheelEvent *e)
{
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
CQCamera3DBones::
keyPressEvent(QKeyEvent *e)
{
  auto *canvas = app_->canvas();
//auto *camera = canvas->getInteractiveCamera();

  auto k = e->key();

  mouseData_.isShift   = (e->modifiers() & Qt::ShiftModifier);
  mouseData_.isControl = (e->modifiers() & Qt::ControlModifier);

//auto bbox = canvas->bbox();

//auto d = bbox.getMaxSize()/100.0;

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
  else if (k == Qt::Key_F) {
    setAutoFit(! mouseData_.isShift);

    for (auto *v : views_)
      v->range.setZoomFactor(1.0);
  }

  canvas->update();

  invalidate();
}

void
CQCamera3DBones::
selectNode(int x, int y)
{
  CPoint2D p;

  if      (xview_.pressRange(x, y, p))
    selectNodeXY(CPoint2D(p.x, p.y)); // XY
  else if (yview_.pressRange(x, y, p))
    selectNodeZY(CPoint2D(p.x, p.y)); // ZY
  else if (zview_.pressRange(x, y, p))
    selectNodeXZ(CPoint2D(p.x, p.y)); // XZ
}

void
CQCamera3DBones::
selectNodeXY(const CPoint2D &p)
{
  int    nodeId  = -1;
  double minDist = 0.0;

  for (const auto &pn : nodeCenters_) {
    auto c = CPoint2D(pn.second.x, pn.second.y);

    auto d = p.distanceTo(c);

    if (nodeId < 0 || d < minDist) {
      nodeId  = pn.first;
      minDist = d;
    }
  }

  if (nodeId >= 0)
    app_->setCurrentBoneNode(nodeId);
}

void
CQCamera3DBones::
selectNodeZY(const CPoint2D &p)
{
  int    nodeId  = -1;
  double minDist = 0.0;

  for (const auto &pn : nodeCenters_) {
    auto c = CPoint2D(pn.second.z, pn.second.y);

    auto d = p.distanceTo(c);

    if (nodeId < 0 || d < minDist) {
      nodeId  = pn.first;
      minDist = d;
    }
  }

  if (nodeId >= 0)
    app_->setCurrentBoneNode(nodeId);
}

void
CQCamera3DBones::
selectNodeXZ(const CPoint2D &p)
{
  int    nodeId  = -1;
  double minDist = 0.0;

  for (const auto &pn : nodeCenters_) {
    auto c = CPoint2D(pn.second.x, pn.second.z);

    auto d = p.distanceTo(c);

    if (nodeId < 0 || d < minDist) {
      nodeId  = pn.first;
      minDist = d;
    }
  }

  if (nodeId >= 0)
    app_->setCurrentBoneNode(nodeId);
}

#if 0
bool
CQCamera3DBones::
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
#endif

CMatrix3D
CQCamera3DBones::
getNodeTransform(CGeomObject3D *animObject, const CGeomNodeData &nodeData) const
{
  auto animName = animObject->animName();
  auto animTime = animObject->animTime();

  auto *object = nodeData.object();

  if (! object)
    object = animObject;

  if (animName != "")
    return object->getNodeAnimHierTransform(nodeData, animName, animTime);
  else
    return object->getNodeHierTransform(nodeData);
}
