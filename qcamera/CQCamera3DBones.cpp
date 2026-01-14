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

  auto *canvas = app_->canvas();
  auto *camera = canvas->getCurrentCamera();

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

  connect(app_, SIGNAL(animNameChanged()), this, SLOT(update()));
  connect(app_, SIGNAL(animTimeChanged()), this, SLOT(update()));

  connect(camera, SIGNAL(stateChanged()), this, SLOT(update()));
  connect(canvas, SIGNAL(stateChanged()), this, SLOT(update()));
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
CQCamera3DBones::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter_ = &painter;

  //---

  auto *canvas = app_->canvas();
  auto *camera = canvas->getCurrentCamera();

  projectionMatrix_ = CMatrix3DH(camera->worldMatrix());
  viewMatrix_       = CMatrix3DH(camera->viewMatrix());

  //---

  // set window ranges from object bbox
  if (! bboxSet_) {
    if (isAutoFit()) {
      updateBonesBBox();

      //---

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

  //useAnim_ = (app_->animName() != "");
  useAnim_ = true;

  //---

  drawBones();

  if (isShowModel())
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
  struct JointNode {
    CGeomObject3D* object { nullptr };
    int            nodeId { -1 };
  };

  std::vector<JointNode      > selectedJointNodes;
  std::vector<CGeomVertex3D *> selectedBoneVertices;

  int currentBoneNode   = app_->currentBoneNode();
  int currentBoneObject = app_->currentBoneObject();

  //---

  // draw model
  painter_->setPen(QColor(0, 0, 0, 32));
  painter_->setBrush(Qt::NoBrush);

  auto *scene = app_->getScene();

  for (auto *object : scene->getObjects()) {
    if (! object->getVisible())
      continue;

    auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);
    assert(object1);

    auto *rootObject = object->getRootObject();

    //---

    modelMatrix_ = CMatrix3DH(object1->getHierTransform());

    auto meshMatrix = CMatrix3DH(object->getMeshGlobalTransform());

    //---

    const auto &nodeMatrices = app_->getObjectNodeMatrices(rootObject);

    //---

    const auto &faces = object->getFaces();

    for (const auto *face : faces) {
      if (! face->getVisible())
        continue;

      std::vector<CPoint3D> points;

      const auto &vertices = face->getVertices();

      for (const auto &v : vertices) {
        const auto &vertex = object->getVertex(v);
        const auto &model  = vertex.getModel();

        auto p = model;

        //---

        if (useAnim_)
          p = app_->adjustAnimPoint(vertex, p, nodeMatrices);

        p = meshMatrix*p;

        //---

        points.push_back(p);

        //---

        // add vertex joint nodes
        const auto &jointData = vertex.getJointData();

        for (int i = 0; i < 4; ++i) {
          const auto &jointNodeData = jointData.nodeDatas[i];

          int nodeId = -1;

          if (jointNodeData.node >= 0)
            nodeId = rootObject->mapNodeIndex(jointNodeData.node);

          if (nodeId >= 0) {
            JointNode jointNode;

            jointNode.object = object;
            jointNode.nodeId = nodeId;

            if (vertex.isSelected())
              selectedJointNodes.push_back(jointNode);

            if (int(object->getInd()) == currentBoneObject && nodeId == currentBoneNode) {
              selectedBoneVertices.push_back(const_cast<CGeomVertex3D *>(&vertex));
            }
          }
        }
      }

      //---

      drawModelPolygon(points);
    }
  }

  //---

  // draw joint points
  if (isShowPointJoints()) {
    painter_->setPen(Qt::red);
    painter_->setBrush(Qt::white);

    for (auto &jointNode : selectedJointNodes) {
      auto *rootObject = jointNode.object->getRootObject();

      auto &node = rootObject->getNode(jointNode.nodeId);

      if (isOnlyJoints() && ! node.isJoint())
        continue;

      auto m = getNodeTransform(rootObject, const_cast<CGeomNodeData &>(node));

      auto c = m*CPoint3D(0.0, 0.0, 0.0);

      auto s = bbox_.getMaxSize()/100.0;

      drawCircle(CVector3D(c), s);
    }
  }

  //---

  // draw bone nodes
  if (isShowBoneNodes()) {
    painter_->setPen(Qt::green);
    painter_->setBrush(Qt::white);

    for (auto *v : selectedBoneVertices) {
      auto *object = v->getObject();
    //auto *object = scene->getObjectP(currentBoneObject);

      auto meshMatrix = CMatrix3DH(object->getMeshGlobalTransform());

      const auto &model = v->getModel();

      auto p = model;

      if (useAnim_) {
        auto *rootObject = object->getRootObject();

        auto &nodeMatrices = app_->getObjectNodeMatrices(rootObject);

        p = app_->adjustAnimPoint(*v, p, nodeMatrices);
      }

      p = meshMatrix*p;

      auto s = bbox_.getMaxSize()/100.0;

      drawCircle(CVector3D(p), s);
    }
  }
}

void
CQCamera3DBones::
updateBonesBBox()
{
  bbox_ = CBBox3D();

  xview_.bbox = CBBox2D();
  yview_.bbox = CBBox2D();
  zview_.bbox = CBBox2D();

  auto rootObjects = app_->getRootObjects();

  for (auto *rootObject : rootObjects) {
    if (! rootObject->getVisible())
      continue;

    for (const auto &pn : rootObject->getNodes()) {
      auto &node = const_cast<CGeomNodeData &>(pn.second);

      if (isOnlyJoints() && ! node.isJoint())
        continue;

      //---

    //auto *object = node.rootObject()->getMeshObject();

    //auto meshMatrix = CMatrix3DH(object->getMeshGlobalTransform());
      auto meshMatrix = CMatrix3DH::identity();

      //---

      auto m = rootObject->getNodeHierTransform(node);

      auto c = m*CPoint3D(0.0, 0.0, 0.0);

      c = meshMatrix*c;

      updateBBox(c);
    }
  }
}

void
CQCamera3DBones::
drawBones()
{
  int currentBoneNode   = app_->currentBoneNode();
  int currentBoneObject = app_->currentBoneObject();

  painter_->setPen(Qt::black);

  auto rootObjects = app_->getRootObjects();

  objectNodes_.clear();

  for (auto *rootObject : rootObjects) {
    if (! rootObject->getVisible())
      continue;

    auto *rootObject1 = dynamic_cast<CQCamera3DGeomObject *>(rootObject);
    assert(rootObject1);

    //---

    modelMatrix_ = CMatrix3DH(rootObject1->getHierTransform());

    //---

    for (const auto &pn : rootObject->getNodes()) {
      auto &node = const_cast<CGeomNodeData &>(pn.second);

      if (isOnlyJoints() && ! node.isJoint())
        continue;

      //---

      auto *meshObject = node.rootObject()->getMeshObject();

      auto meshMatrix = CMatrix3DH(meshObject->getMeshGlobalTransform());

      //---

      bool isSelected =
       (int(rootObject->getInd()) == currentBoneObject && node.ind() == currentBoneNode);

      painter_->setBrush(isSelected ? Qt::red: Qt::white);

      //---

      auto m = getNodeTransform(rootObject, node);

      auto c = m*CPoint3D(0.0, 0.0, 0.0);

      c = meshMatrix*c;

      auto s = bbox_.getMaxSize()/100.0;

      drawCube(c, s);

#if 0
      if (isSelected)
        drawModelPoint(CVector3D(c), QString::fromStdString(node.name()));
#endif

      objectNodes_[rootObject->getInd()][node.ind()] = c;

      if (node.parent() >= 0) {
        auto &pnode = const_cast<CGeomNodeData &>(rootObject->getNode(node.parent()));

        if (pnode.isValid() && (! isOnlyJoints() || pnode.isJoint())) {
          auto *pmeshObject = pnode.rootObject()->getMeshObject();

          auto pmeshMatrix = CMatrix3DH(pmeshObject->getMeshGlobalTransform());

          auto m1 = getNodeTransform(rootObject, pnode);

          auto c1 = m1*CPoint3D(0.0, 0.0, 0.0);

          c1 = pmeshMatrix*c1;

          drawModelLine(c, c1);
        }
      }
    }
  }
}

void
CQCamera3DBones::
drawCube(const CPoint3D &c, double s) const
{
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

  auto cubePoint = [&](int i, int j) {
    const auto &v1 = v[cube_faces[i][j]];

    return CPoint3D(c.x + s*v1[0], c.y + s*v1[1], c.z + s*v1[2]);
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
    painter_->setClipRect(view.rect);

    std::vector<QPointF> ppoints;

    for (const auto &p : points) {
      double px, py;
      view.range.windowToPixel(p.x, p.y, &px, &py);

      ppoints.push_back(QPointF(px, py));
    }

    painter_->drawPolygon(&ppoints[0], ppoints.size());
  };

  std::vector<CPoint2D> xpoints, ypoints, zpoints, ppoints;

  for (const auto &p : points) {
    CPoint3D p1;

    if (model)
      p1 = modelMatrix_*p;
    else
      p1 = p;

    xpoints.push_back(CPoint2D(p1.getX(), p1.getY())); // XY
    ypoints.push_back(CPoint2D(p1.getZ(), p1.getY())); // ZY
    zpoints.push_back(CPoint2D(p1.getX(), p1.getZ())); // XZ

    auto p2 = projectionMatrix_*viewMatrix_*p1;

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
    painter_->setClipRect(view.rect);

    double px1, py1;
    view.range.windowToPixel(p1.x, p1.y, &px1, &py1);
    double px2, py2;
    view.range.windowToPixel(p2.x, p2.y, &px2, &py2);

    painter_->drawLine(px1, py1, px2, py2);

    if (label != "")
      painter_->drawText(px2, py2, label);
  };

  auto p11 = modelMatrix_*p1;
  auto p21 = modelMatrix_*p2;

  auto x1 = p11.getX(), y1 = p11.getY(), z1 = p11.getZ();
  auto x2 = p21.getX(), y2 = p21.getY(), z2 = p21.getZ();

  drawLine2D(xview_, CPoint2D(x1, y1), CPoint2D(x2, y2), label); // XY
  drawLine2D(yview_, CPoint2D(z1, y1), CPoint2D(z2, y2), label); // ZY
  drawLine2D(zview_, CPoint2D(x1, z1), CPoint2D(x2, z2), label); // XZ

  p11 = projectionMatrix_*viewMatrix_*modelMatrix_*p1;
  p21 = projectionMatrix_*viewMatrix_*modelMatrix_*p2;

  x1 = p11.getX(), y1 = p11.getY();
  x2 = p21.getX(), y2 = p21.getY();

  drawLine2D(pview_, CPoint2D(x1, y1), CPoint2D(x2, y2), label);
}

void
CQCamera3DBones::
drawCircle(const CVector3D &origin, double r)
{
  auto drawCircle2D = [&](const ViewData &view, const CPoint2D &o, double r) {
    painter_->setClipRect(view.rect);

    double px1, py1;
    view.range.windowToPixel(o.x - r, o.y - r, &px1, &py1);
    double px2, py2;
    view.range.windowToPixel(o.x + r, o.y + r, &px2, &py2);

    painter_->drawEllipse(QRectF(px1, py1, px2 - px1, py2 - py1));
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
    painter_->setClipRect(view.rect);

    double px, py;
    view.range.windowToPixel(p.x, p.y, &px, &py);

    painter_->drawLine(px - 4, py, px + 4, py);
    painter_->drawLine(px, py - 4, px, py + 4);

    if (label != "")
      painter_->drawText(px, py, label);
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
    painter_->setClipRect(view.rect);

    double px, py;
    view.range.windowToPixel(p.x, p.y, &px, &py);

    painter_->drawPixmap(px - s/2, py - s/2, pixmap);
  };

  drawPixmap2D(xview_, CPoint2D(p.getX(), p.getY()), pixmap); // XY
  drawPixmap2D(yview_, CPoint2D(p.getZ(), p.getY()), pixmap); // ZY
  drawPixmap2D(zview_, CPoint2D(p.getX(), p.getZ()), pixmap); // XZ
}

void
CQCamera3DBones::
mousePressEvent(QMouseEvent *e)
{
//bool isShift   = (e->modifiers() & Qt::ShiftModifier);
//bool isControl = (e->modifiers() & Qt::ControlModifier);

  pressPixel_ = e->pos();
  movePixel_  = pressPixel_;

  if (e->button() == Qt::LeftButton) {
    rubberBand_->setBounds(pressPixel_, movePixel_);
    rubberBand_->show();
  }

  pressed_     = true;
  mouseButton_ = e->button();
}

void
CQCamera3DBones::
mouseReleaseEvent(QMouseEvent *e)
{
  movePixel_ = e->pos();

  if (mouseButton_ == Qt::LeftButton) {
    selectNode(pressPixel_.x(), pressPixel_.y());

    rubberBand_->hide();
  }

  pressed_ = false;
}

void
CQCamera3DBones::
mouseMoveEvent(QMouseEvent *e)
{
  int x = e->x();
  int y = e->y();

  movePixel_ = e->pos();

//bool isShift   = (e->modifiers() & Qt::ShiftModifier);
//bool isControl = (e->modifiers() & Qt::ControlModifier);

  if (pressed_) {
    if (mouseButton_ == Qt::LeftButton) {
      rubberBand_->setBounds(pressPixel_, movePixel_);
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

  if (! pressed_) {
    if (ind_ != ind)
      update();
  }
}

void
CQCamera3DBones::
keyPressEvent(QKeyEvent *e)
{
  auto *canvas = app_->canvas();
//auto *camera = canvas->getCurrentCamera();

  auto k = e->key();

//bool isShift   = (e->modifiers() & Qt::ShiftModifier);
//bool isControl = (e->modifiers() & Qt::ControlModifier);

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

  canvas->update();
  update();
}

void
CQCamera3DBones::
selectNode(int x, int y)
{
  CPoint2D p;

  if (xview_.pressRange(x, y, p))
    selectNodeXY(CPoint2D(p.x, p.y)); // XY
  if (yview_.pressRange(x, y, p))
    selectNodeZY(CPoint2D(p.x, p.y)); // ZY
  if (zview_.pressRange(x, y, p))
    selectNodeXZ(CPoint2D(p.x, p.y)); // XZ
}

void
CQCamera3DBones::
selectNodeXY(const CPoint2D &p)
{
  int    objId   = -1;
  int    nodeId  = -1;
  double minDist = 0.0;

  for (const auto &po : objectNodes_) {
    for (const auto &pn : po.second) {
      auto c = CPoint2D(pn.second.x, pn.second.y);

      auto d = p.distanceTo(c);

      if (objId < 0 || nodeId < 0 || d < minDist) {
        objId   = po.first;
        nodeId  = pn.first;
        minDist = d;
      }
    }
  }

  if (objId >= 0 && nodeId >= 0) {
    app_->setCurrentBoneObject(objId);
    app_->setCurrentBoneNode(nodeId);
  }
}

void
CQCamera3DBones::
selectNodeZY(const CPoint2D &p)
{
  int    objId   = -1;
  int    nodeId  = -1;
  double minDist = 0.0;

  for (const auto &po : objectNodes_) {
    for (const auto &pn : po.second) {
      auto c = CPoint2D(pn.second.z, pn.second.y);

      auto d = p.distanceTo(c);

      if (objId < 0 || nodeId < 0 || d < minDist) {
        objId   = po.first;
        nodeId  = pn.first;
        minDist = d;
      }
    }
  }

  if (objId >= 0 && nodeId >= 0) {
    app_->setCurrentBoneObject(objId);
    app_->setCurrentBoneNode(nodeId);
  }
}

void
CQCamera3DBones::
selectNodeXZ(const CPoint2D &p)
{
  int    objId   = -1;
  int    nodeId  = -1;
  double minDist = 0.0;

  for (const auto &po : objectNodes_) {
    for (const auto &pn : po.second) {
      auto c = CPoint2D(pn.second.x, pn.second.z);

      auto d = p.distanceTo(c);

      if (objId < 0 || nodeId < 0 || d < minDist) {
        objId   = po.first;
        nodeId  = pn.first;
        minDist = d;
      }
    }
  }

  if (objId >= 0 && nodeId >= 0) {
    app_->setCurrentBoneObject(objId);
    app_->setCurrentBoneNode(nodeId);
  }
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

void
CQCamera3DBones::
updateBBox(const CPoint3D &c)
{
  bbox_ += c;

  xview_.bbox += CPoint2D(c.x, c.y);
  yview_.bbox += CPoint2D(c.z, c.y);
  zview_.bbox += CPoint2D(c.x, c.z);
}

CMatrix3D
CQCamera3DBones::
getNodeTransform(CGeomObject3D *rootObject, CGeomNodeData &nodeData) const
{
  auto animName = rootObject->animName();
  auto animTime = app_->animTime();

  if (animName == "")
    animName = app_->animName().toStdString();

  auto *object = nodeData.object();

  if (! object)
    object = rootObject;

#if 0
  std::optional<CMatrix3D> m1, m2, m3;

  if (nodeData.isJoint())
    m1 = nodeData.inverseBindMatrix().inverse();

  if (animName != "")
    m2 = object->getNodeAnimHierTransform(nodeData, animName, animTime);
  else
    m2 = object->getNodeHierTransform(nodeData);

  auto &nodeMatrices = app_->getObjectNodeMatrices(rootObject);

  auto pn1 = nodeMatrices.find(nodeData.index());

  if (pn1 != nodeMatrices.end())
    m3 = (*pn1).second;

  if (m3)
    return (*m3).inverse();

  if (m1)
    return *m1;

  return *m2;
#else
  if (animName != "")
    return object->getNodeAnimHierTransform(nodeData, animName, animTime);
  else
    return object->getNodeHierTransform(nodeData);
#endif
}
