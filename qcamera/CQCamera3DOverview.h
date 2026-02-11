#ifndef CQCamera3DOverview_H
#define CQCamera3DOverview_H

#include <CQCamera3DApp.h>

#include <CWindowRange2D.h>
#include <CMatrix3DH.h>
#include <CPoint3D.h>
#include <CRGBA.h>

#include <QFrame>

#include <map>

class CQCamera3DApp;
class CGLCameraIFace;
class CQCamera3DOpWidget;
class CQCamera3DOverviewMouseModeIFace;
class CQCamera3DMouseModeMgr;
class CQCamera3DOptions;

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

  using EditType      = CQCamera3DEditType;
  using SelectType    = CQCamera3DSelectType;
  using AddObjectType = CQCamera3DAddObjectType;

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
  void setEditType(const EditType &v);

  const SelectType &selectType() const { return selectType_; }
  void setSelectType(const SelectType &v);

  const ModelType &modelType() const { return modelType_; }
  void setModelType(const ModelType &v) { modelType_ = v; update(); }

  bool isCameraVisible() const { return cameraVisible_; }
  void setCameraVisible(bool b) { cameraVisible_ = b; update(); }

  bool isLightsVisible() const { return lightsVisible_; }
  void setLightsVisible(bool b) { lightsVisible_ = b; update(); }

  //---

  void setOptions(CQCamera3DOptions *options);
  void showOptions();
  void hideOptions();

  //---

  CGeomObject3D *addCircle();
  CGeomObject3D *addCone();
  CGeomObject3D *addCube();
  CGeomObject3D *addCylinder();
  CGeomObject3D *addPlane();
  CGeomObject3D *addPyramid();
  CGeomObject3D *addSphere();
  CGeomObject3D *addTorus();

  CGeomObject3D *addObject(const std::string &typeName, const AddObjectType &type);

  //---

  void resizeEvent(QResizeEvent *) override;

  void paintEvent(QPaintEvent *) override;

  void mousePressEvent  (QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;
  void mouseMoveEvent   (QMouseEvent *e) override;

  void wheelEvent(QWheelEvent *) override;

  void keyPressEvent(QKeyEvent *e) override;

 private:
  void updateRange();

  void setCameraPosition(int x, int y);
  void setCameraOrigin(int x, int y);

  void setLightPosition(int x, int y);
  void setLightDirection(int x, int y);

  void setCursorPosition(int x, int y);

  void selectObjectAt(const QPoint &p, bool clear) ;
  void selectObjectAt1(const ViewData &view, const CPoint3D &p, bool clear);

  void selectObjectIn(const QPoint &p, const QRect &r, bool clear);

  void selectObjectXY(const CPoint3D &p);
  void selectObjectZY(const CPoint3D &p);
  void selectObjectXZ(const CPoint3D &p);

  void drawModel();
  void updateModel();

  void drawCameras();
  void drawCamera(CGLCameraIFace *camera);

  void drawEyeLine();

  void drawCursor();

  void drawLights();

  void drawModelPolygon(const std::vector<CPoint3D> &points) const;
  void drawModelLine(const CPoint3D &p1, const CPoint3D &p2) const;
  void drawModelPoint(const CPoint3D &p) const;

  void drawCone(const CVector3D &p, const CVector3D &d, double a) const;

  void drawVector(const CVector3D &p, const CVector3D &d, const QString &label) const;

  void drawCircle(const CPoint3D &o, double r);
  void drawSphere(const CPoint3D &o, const CPoint3D &r);

  void drawPoint(const CVector3D &p, const QString &label) const;

  void drawPixmap(const CPoint3D &p, const QPixmap &pixmap) const;

//bool pressRange(const ViewData &view, int x, int y, CPoint2D &p) const;

  CPoint2D pixelToView(ViewType viewType, const QPointF &p) const;

  ViewType pixelToViewType(const QPointF &p) const;

  QPointF viewQPoint(ViewType viewType, const CPoint3D &p) const;
  CPoint2D viewPoint(ViewType viewType, const CPoint3D &p) const;

 public Q_SLOTS:
  void invalidate();

 Q_SIGNALS:
  void selectTypeChanged();
  void editTypeChanged();

  void objectAdded();

 private:
  struct FaceData {
    CGeomFace3D*          face        { nullptr };
    std::vector<CPoint3D> points;
    bool                  filled      { true };
    bool                  stroked     { false };
    CRGBA                 fillColor   { CRGBA::white() };
    CRGBA                 strokeColor { CRGBA::black() };
  };

  struct LineData {
    CGeomLine3D*          line  { nullptr };
    std::vector<CPoint3D> points;
    CRGBA                 color { CRGBA::white() };
  };

  struct VertexData {
    CPoint3D p;
    CRGBA    color { CRGBA::white() };
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
  bool valid_      { false };

  EditType   editType_   { EditType::SELECT };
  SelectType selectType_ { SelectType::OBJECT };

  ModelType modelType_     { ModelType::WIREFRAME };
  bool      cameraVisible_ { false };
  bool      lightsVisible_ { false };

  struct MouseData {
    bool   pressed   { false };
    bool   isShift   { false };
    bool   isControl { false };
    int    button    { 0 };
    QPoint pressPixel;
    QPoint movePixel;
  };

  MouseData mouseData_;

  using FaceDatas   = std::vector<FaceData *>;
  using LineDatas   = std::vector<LineData *>;
  using VertexDatas = std::vector<VertexData *>;

  struct GeomData {
    FaceDatas   faceDatas;
    LineDatas   lineDatas;
    VertexDatas vertexDatas;
  };

  using ObjectGeomData = std::map<CGeomObject3D *, GeomData>;

  using Polygon2D                = std::vector<QPointF>;
  using Polygon2DArray           = std::vector<Polygon2D>;
  using SortedPolygon2DArray     = std::map<double, Polygon2DArray>;
  using SortedPoint2DArray       = std::map<double, Polygon2D>;
  using ViewSortedPolygon2DArray = std::map<int, SortedPolygon2DArray>;
  using ViewSortedLine2DArray    = std::map<int, SortedPolygon2DArray>;
  using ViewSortedPoint2DArray   = std::map<int, SortedPoint2DArray>;

  // draw data
  struct DrawData {
    QPainter*      painter { nullptr };
    CMatrix3DH     projectionMatrix;
    CMatrix3DH     viewMatrix;
    CMatrix3DH     modelMatrix;
    CMatrix3DH     meshMatrix;
    ObjectGeomData objectGeomData;
    bool           filled  { true };
    bool           useAnim { false };

    mutable ViewSortedPolygon2DArray viewSortedPolygon2DArray;
    mutable ViewSortedLine2DArray    viewSortedLine2DArray;
    mutable ViewSortedPoint2DArray   viewSortedPoint2DArray;
  };

  DrawData drawData_;

  bool   bboxSet_ { false };
  double xs_ { 0.0 }, ys_ { 0.0 }, zs_ { 0.0 };

  QPixmap lightPixmap_;

  CQRubberBand* rubberBand_ { nullptr };

  CQCamera3DOpWidget* opWidget_ { nullptr };

  CQCamera3DOverviewMouseModeIFace* mouseModeIFace_ { nullptr };
  CQCamera3DMouseModeMgr*           mouseModeMgr_   { nullptr };
};

#endif
