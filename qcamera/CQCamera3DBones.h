#ifndef CQCamera3DBones_H
#define CQCamera3DBones_H

#include <CQCamera3DApp.h>

#include <CWindowRange2D.h>
//#include <CDisplayRange2D.h>
#include <CWindowRange2D.h>
#include <CMatrix3DH.h>
#include <CBBox3D.h>
#include <CPoint3D.h>

#include <QFrame>

class CQCamera3DApp;
class CGeomFace3D;
class CGeomNodeData;
class CQRubberBand;
class CVector3D;

class CQCamera3DBones : public QFrame {
  Q_OBJECT

 public:
  enum class ModelType {
    NONE,
    WIREFRAME,
    SOLID
  };

  enum class ViewType {
    NONE,
    XY,
    ZY,
    XZ,
    THREED
  };

  struct ViewData {
    CDisplayRange2D range;
    QRectF          rect;
    CBBox2D         bbox;
    int             ind  { -1 };
    ViewType        type { ViewType::NONE };
    QString         name;

    CPoint2D pixelToView(const QPointF &p) const {
      CPoint2D p1;

      if      (type == ViewType::XY) {
        (void) pressRange(p.x(), p.y(), p1);
      }
      else if (type == ViewType::ZY) {
        (void) pressRange(p.x(), p.y(), p1);
      }
      else if (type == ViewType::XZ) {
        (void) pressRange(p.x(), p.y(), p1);
      }
      else
        assert(false);

      return p1;
    }

    bool pressRange(int x, int y, CPoint2D &p) const {
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

    CPoint2D viewPoint(const CPoint3D &p) const {
      CPoint2D p1;

      if      (type == ViewType::XY)
        p1 = CPoint2D(p.x, p.y);
      else if (type == ViewType::ZY)
        p1 = CPoint2D(p.z, p.y);
      else if (type == ViewType::XZ)
        p1 = CPoint2D(p.x, p.z);
      else
        assert(false);

      return p1;
    }
  };

 public:
  CQCamera3DBones(CQCamera3DApp *app);

  //---

  bool isEqualScale() const { return equalScale_; }
  void setEqualScale(bool b) { equalScale_ = b; updateRange(); }

  bool isAutoFit() const { return autoFit_; }
  void setAutoFit(bool b) { autoFit_ = b; bboxSet_ = false; update(); }

  bool isShowModel() const { return showModel_; }
  void setShowModel(bool b) { showModel_ = b; update(); }

  bool isShowBoneNodes() const { return showBoneNodes_; }
  void setShowBoneNodes(bool b) { showBoneNodes_ = b; update(); }

  bool isShowPointJoints() const { return showPointJoints_; }
  void setShowPointJoints(bool b) { showPointJoints_ = b; update(); }

  bool isOnlyJoints() const { return onlyJoints_; }
  void setOnlyJoints(bool b) { onlyJoints_ = b; update(); }

  //---

  void resizeEvent(QResizeEvent *) override;

  void paintEvent(QPaintEvent *) override;

  void mousePressEvent  (QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;
  void mouseMoveEvent   (QMouseEvent *e) override;

  void keyPressEvent(QKeyEvent *e) override;

 private:
  void updateRange();

  void updateBonesBBox();

  void drawBones();
  void drawModel();
  void updateNodeMatrices();

  void drawCube(const CPoint3D &c, double s) const;

  void drawModelPolygon(const std::vector<CPoint3D> &points) const;
  void drawPolygon(const std::vector<CPoint3D> &points, bool model=false) const;

  void drawModelLine(const CPoint3D &p1, const CPoint3D &p2, const QString &label="") const;

  void drawCircle(const CVector3D &o, double r);

  void drawPoint(const CVector3D &p, const QString &label) const;

  void drawPixmap(const CPoint3D &p, const QPixmap &pixmap) const;

  void selectNode(int x, int y);

  void selectNodeXY(const CPoint2D &p);
  void selectNodeZY(const CPoint2D &p);
  void selectNodeXZ(const CPoint2D &p);

//bool pressRange(const CDisplayRange2D &range, int x, int y, CPoint2D &p) const;

  void updateBBox(const CPoint3D &p);

  CMatrix3D getNodeTransform(CGeomObject3D *rootObject, CGeomNodeData &nodeData) const;

 private:
  using ObjectNodeMatrices = CQCamera3DApp::ObjectNodeMatrices;

  using NodeCenters       = std::map<uint, CPoint3D>;
  using ObjectNodeCenters = std::map<uint, NodeCenters>;

  //---

  CQCamera3DApp* app_ { nullptr };

  ViewData xview_;
  ViewData yview_;
  ViewData zview_;
  ViewData pview_;

  std::vector<ViewData *> views_;  // all views
  std::vector<ViewData *> views2_; // 2d views

  int ind_ { -1 };

  bool equalScale_ { true };
  bool autoFit_    { false };

  bool showModel_       { false };
  bool showBoneNodes_   { false };
  bool showPointJoints_ { false };
  bool onlyJoints_      { false };

  bool   pressed_     { false };
  int    mouseButton_ { 0 };
  QPoint pressPixel_;
  QPoint movePixel_;

  // draw data
  QPainter *painter_ { nullptr };

  CBBox3D bbox_;
  bool    bboxSet_ { false };
  double  xs_ { 0.0 }, ys_ { 0.0 }, zs_ { 0.0 };

  CMatrix3DH projectionMatrix_;
  CMatrix3DH viewMatrix_;
  CMatrix3DH modelMatrix_;

  std::string animName_;
  double      animTime_ { 0.0 };
  bool        useAnim_  { false };

  ObjectNodeMatrices objectNodeMatrices_;
  ObjectNodeCenters  objectNodes_;

  CQRubberBand* rubberBand_ { nullptr };
};

#endif
