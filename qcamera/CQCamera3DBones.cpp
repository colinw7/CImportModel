#include <CQCamera3DBones.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DCamera.h>
#include <CQCamera3DLight.h>
#include <CQCamera3DStatus.h>
#include <CQCamera3DGeomObject.h>
#include <CQCamera3DUtil.h>

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
  auto *camera = canvas->currentCamera();

  connect(camera, SIGNAL(stateChanged()), this, SLOT(update()));
  connect(canvas, SIGNAL(stateChanged()), this, SLOT(update()));

  setFocusPolicy(Qt::StrongFocus);

  setMouseTracking(true);

  connect(app_, SIGNAL(animNameChanged()), this, SLOT(update()));
  connect(app_, SIGNAL(animTimeChanged()), this, SLOT(update()));
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

  xrange_.setPixelRange(x1, y1, x2, y2); // XY
  yrange_.setPixelRange(x2, y1, x3, y2); // ZY
  zrange_.setPixelRange(x1, y2, x2, y3); // XZ
  prange_.setPixelRange(x2, y2, x3, y3); // 3D

  xrange_.setEqualScale(isEqualScale());
  yrange_.setEqualScale(isEqualScale());
  zrange_.setEqualScale(isEqualScale());

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

  animName_ = app_->animName().toStdString();
  animTime_ = app_->animTime();

  useAnim_ = (animName_ != "");

  //---

  drawBones();

  if (isShowModel())
    drawModel();

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
CQCamera3DBones::
drawModel()
{
  // draw model
  painter_->setPen(QColor(0, 0, 0, 32));
  painter_->setBrush(Qt::NoBrush);

  auto *scene = app_->getScene();

  for (auto *object : scene->getObjects()) {
    if (! object->getVisible())
      continue;

    auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);
    assert(object1);

    modelMatrix_ = CMatrix3DH(object1->getHierTransform());

    auto &nodeMatrices = objectNodeMatrices_[object->getInd()];

    if (useAnim_)
      object->updateNodesAnimationData(animName_, animTime_);

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

        if (useAnim_) {
          const auto &jointData = vertex.getJointData();

          struct NodeWeight {
            int    nodeId { -1 };
            double weight { 0.0 };
          };

          std::vector<NodeWeight> nodeWeights;

          for (int i = 0; i < 4; ++i) {
            if (jointData.nodeDatas[i].node >= 0) {
              NodeWeight nodeWeight;

              nodeWeight.nodeId = jointData.nodeDatas[i].node;
              nodeWeight.weight = jointData.nodeDatas[i].weight;

              nodeWeights.push_back(nodeWeight);
            }
          }

          if (! nodeWeights.empty()) {
            auto p1 = CPoint3D(0, 0, 0);

            for (const auto &nodeWeight : nodeWeights) {
              auto boneTransform = nodeMatrices[nodeWeight.nodeId];

              p1 += nodeWeight.weight*(boneTransform*p);
            }

            p = p1;
          }
        }

        //---

        points.push_back(p);
      }

      //---

      drawPolygon(points);
    }
  }
}

void
CQCamera3DBones::
drawBones()
{
  int currentBoneNode = app_->currentBoneNode();

  auto getNodeTransform = [&](CGeomNodeData &nodeData) {
    //return nodeData.inverseBindMatrix().inverse();

    if (useAnim_)
      return nodeData.object()->getNodeAnimHierTransform(nodeData, animName_, animTime_);
    else
      return nodeData.object()->getNodeHierTransform(nodeData);
  };

  painter_->setPen(Qt::black);

  auto rootObjects = app_->getRootObjects();

  objectNodes_.clear();

  for (auto *rootObject : rootObjects) {
    if (! rootObject->getVisible())
      continue;

    auto *rootObject1 = dynamic_cast<CQCamera3DGeomObject *>(rootObject);
    assert(rootObject1);

    modelMatrix_ = CMatrix3DH(rootObject1->getHierTransform());

    auto &nodeMatrices = objectNodeMatrices_[rootObject->getInd()];

    if (useAnim_)
      rootObject->updateNodesAnimationData(animName_, animTime_);

    auto meshMatrix        = rootObject->getMeshGlobalTransform();
    auto inverseMeshMatrix = meshMatrix.inverse();

    //---

    const auto &nodeIds = rootObject->getNodeIds();

    for (const auto &nodeId : nodeIds) {
      auto &node = const_cast<CGeomNodeData &>(rootObject->getNode(nodeId));
      if (! node.isValid()) continue;

      if (! node.isJoint())
        continue;

      if (node.ind() == currentBoneNode)
        painter_->setBrush(Qt::red);
      else
        painter_->setBrush(Qt::white);

      //---

      auto m = getNodeTransform(node);

      auto c = m*CPoint3D(0.0, 0.0, 0.0);

      auto s = 0.1;

      drawCube(c, s);

      objectNodes_[rootObject->getInd()][node.ind()] = c;

      if (node.parent() >= 0) {
        auto &pnode = const_cast<CGeomNodeData &>(rootObject->getNode(node.parent()));

        if (pnode.isValid() && pnode.isJoint()) {
          auto m1 = getNodeTransform(pnode);
          auto c1 = m1*CPoint3D(0.0, 0.0, 0.0);

          drawLine(c, c1);
        }
      }

      //---

      nodeMatrices[node.index()] = node.calcNodeAnimMatrix(inverseMeshMatrix);
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
CQCamera3DBones::
drawLine(const CPoint3D &p1, const CPoint3D &p2, const QString &label) const
{
  auto drawLine2D = [&](const CDisplayRange2D &range, const QRectF &rect, const CPoint2D &p1,
                        const CPoint2D &p2, const QString &label) {
    painter_->setClipRect(rect);

    double px1, py1;
    range.windowToPixel(p1.x, p1.y, &px1, &py1);
    double px2, py2;
    range.windowToPixel(p2.x, p2.y, &px2, &py2);

    painter_->drawLine(px1, py1, px2, py2);

    if (label != "")
      painter_->drawText(px2, py2, label);
  };

  auto p11 = modelMatrix_*p1;
  auto p21 = modelMatrix_*p2;

  auto x1 = p11.getX(), y1 = p11.getY(), z1 = p11.getZ();
  auto x2 = p21.getX(), y2 = p21.getY(), z2 = p21.getZ();

  drawLine2D(xrange_, xrect_, CPoint2D(x1, y1), CPoint2D(x2, y2), label); // XY
  drawLine2D(yrange_, yrect_, CPoint2D(z1, y1), CPoint2D(z2, y2), label); // ZY
  drawLine2D(zrange_, zrect_, CPoint2D(x1, z1), CPoint2D(x2, z2), label); // XZ

  p11 = projectionMatrix_*viewMatrix_*modelMatrix_*p1;
  p21 = projectionMatrix_*viewMatrix_*modelMatrix_*p2;

  x1 = p11.getX(), y1 = p11.getY();
  x2 = p21.getX(), y2 = p21.getY();

  drawLine2D(prange_, prect_, CPoint2D(x1, y1), CPoint2D(x2, y2), label);
}

void
CQCamera3DBones::
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
CQCamera3DBones::
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
CQCamera3DBones::
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
CQCamera3DBones::
mousePressEvent(QMouseEvent *e)
{
//bool isShift   = (e->modifiers() & Qt::ShiftModifier);
//bool isControl = (e->modifiers() & Qt::ControlModifier);

  if (e->button() == Qt::LeftButton) {
    selectNode(e->x(), e->y());
  }

  pressed_     = true;
  mouseButton_ = e->button();
}

void
CQCamera3DBones::
mouseReleaseEvent(QMouseEvent *)
{
  pressed_ = false;
}

void
CQCamera3DBones::
mouseMoveEvent(QMouseEvent *e)
{
  int x = e->x();
  int y = e->y();

//bool isShift   = (e->modifiers() & Qt::ShiftModifier);
//bool isControl = (e->modifiers() & Qt::ControlModifier);

  if (pressed_) {
    if (mouseButton_ == Qt::LeftButton) {
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
CQCamera3DBones::
keyPressEvent(QKeyEvent *)
{
#if 0
  auto *canvas = app_->canvas();
  auto *camera = canvas->currentCamera();

  auto k = e->key();

//bool isShift   = (e->modifiers() & Qt::ShiftModifier);
//bool isControl = (e->modifiers() & Qt::ControlModifier);

  auto bbox = canvas->bbox();

  auto d = bbox.getMaxSize()/100.0;

  canvas->update();
  update();
#endif
}

void
CQCamera3DBones::
selectNode(int x, int y)
{
  CPoint2D p;

  if (pressRange(xrange_, x, y, p))
    selectNodeXY(CPoint2D(p.x, p.y)); // XY
  if (pressRange(yrange_, x, y, p))
    selectNodeZY(CPoint2D(p.x, p.y)); // ZY
  if (pressRange(zrange_, x, y, p))
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
