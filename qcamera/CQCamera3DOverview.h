#ifndef CQCamera3DOverview_H
#define CQCamera3DOverview_H

#include <CQCamera3DApp.h>

#include <CWindowRange2D.h>
//#include <CDisplayRange2D.h>
#include <CWindowRange2D.h>
#include <CMatrix3DH.h>
#include <CPoint3D.h>

#include <QFrame>

#include <map>

class CQCamera3DApp;
class CQCamera3DCamera;

class CGeomFace3D;
class CGeomLine3D;
class CQRubberBand;
class CVector3D;

class CQCamera3DOverview : public QFrame {
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

  using EditType   = CQCamera3DEditType;
  using SelectType = CQCamera3DSelectType;
  using MouseType  = CQCamera3DMouseType;

  struct ViewData {
    CDisplayRange2D range;
    QRectF          rect;
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
  CQCamera3DOverview(CQCamera3DApp *app);

  //---

  CQCamera3DApp *app() const { return app_; }

  bool isEqualScale() const { return equalScale_; }
  void setEqualScale(bool b) { equalScale_ = b; updateRange(); }

  const EditType &editType() const { return editType_; }
  void setEditType(const EditType &v) { editType_ = v; }

  const SelectType &selectType() const { return selectType_; }
  void setSelectType(const SelectType &v) { selectType_ = v; }

  const MouseType &mouseType() const { return mouseType_; }
  void setMouseType(const MouseType &v) { mouseType_ = v; }

  const ModelType &modelType() const { return modelType_; }
  void setModelType(const ModelType &v) { modelType_ = v; update(); }

  bool isCameraVisible() const { return cameraVisible_; }
  void setCameraVisible(bool b) { cameraVisible_ = b; update(); }

  bool isLightsVisible() const { return lightsVisible_; }
  void setLightsVisible(bool b) { lightsVisible_ = b; update(); }

  //---

  void resizeEvent(QResizeEvent *) override;

  void paintEvent(QPaintEvent *) override;

  void mousePressEvent  (QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;
  void mouseMoveEvent   (QMouseEvent *e) override;

  void keyPressEvent(QKeyEvent *e) override;

 private:
  void updateRange();

  void setCameraPosition(int x, int y, bool angle);
  void setLightPosition(int x, int y, bool setDirection);
  void setCursorPosition(int x, int y);

  void selectObjectAt(const QPoint &p, bool clear) ;
  void selectObjectAt1(const ViewData &view, const CPoint3D &p, bool clear);

  void selectObjectIn(const QPoint &p, const QRect &r, bool clear);

  void selectObjectXY(const CPoint3D &p);
  void selectObjectZY(const CPoint3D &p);
  void selectObjectXZ(const CPoint3D &p);

  void drawModel();

  void drawCameras();
  void drawCamera(CQCamera3DCamera *camera);

  void drawEyeLine();

  void drawCursor();

  void drawLights();

  void drawModelPolygon(const std::vector<CPoint3D> &points) const;
  void drawModelLine(const CPoint3D &p1, const CPoint3D &p2) const;
  void drawModelPoint(const CPoint3D &p, const QString &label) const;

  void drawCone(const CVector3D &p, const CVector3D &d, double a) const;

  void drawVector(const CVector3D &p, const CVector3D &d, const QString &label) const;

  void drawCircle(const CVector3D &o, double r);

  void drawPoint(const CVector3D &p, const QString &label) const;

  void drawPixmap(const CPoint3D &p, const QPixmap &pixmap) const;

//bool pressRange(const ViewData &view, int x, int y, CPoint2D &p) const;

  CPoint2D pixelToView(ViewType viewType, const QPointF &p) const;

  QPointF viewQPoint(ViewType viewType, const CPoint3D &p) const;
  CPoint2D viewPoint(ViewType viewType, const CPoint3D &p) const;

 private:
  struct FaceData {
    CGeomFace3D*          face { nullptr };
    CGeomLine3D*          line { nullptr };
    std::vector<CPoint3D> points;
  };

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

  EditType   editType_   { EditType::SELECT };
  SelectType selectType_ { SelectType::OBJECT };
  MouseType  mouseType_  { MouseType::OBJECT };

  ModelType modelType_     { ModelType::WIREFRAME };
  bool      cameraVisible_ { true };
  bool      lightsVisible_ { true };

  struct MouseData {
    bool   pressed   { false };
    bool   isShift   { false };
    bool   isControl { false };
    int    button    { 0 };
    QPoint pressPixel;
    QPoint movePixel;
  };

  MouseData mouseData_;

  using FaceDatas       = std::vector<FaceData>;
  using ObjectFaceDatas = std::map<CGeomObject3D *, FaceDatas>;

  // draw data
  struct DrawData {
    QPainter*       painter { nullptr };
    CMatrix3DH      projectionMatrix;
    CMatrix3DH      viewMatrix;
    CMatrix3DH      modelMatrix;
    CMatrix3DH      meshMatrix;
    ObjectFaceDatas objectFaces;
    bool            filled  { true };
    bool            useAnim { false };
  };

  DrawData drawData_;

  bool   bboxSet_ { false };
  double xs_ { 0.0 }, ys_ { 0.0 }, zs_ { 0.0 };

  QPixmap lightPixmap_;

  CQRubberBand* rubberBand_ { nullptr };
};

#endif
