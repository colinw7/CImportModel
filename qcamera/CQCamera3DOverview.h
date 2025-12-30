#ifndef CQCamera3DOverview_H
#define CQCamera3DOverview_H

#include <CQCamera3DApp.h>

#include <CWindowRange2D.h>
//#include <CDisplayRange2D.h>
#include <CWindowRange2D.h>
#include <CMatrix3DH.h>
#include <CPoint3D.h>

#include <QFrame>

class CQCamera3DApp;
class CGeomFace3D;
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

  using SelectType = CQCamera3DSelectType;
  using MouseType  = CQCamera3DMouseType;

 public:
  CQCamera3DOverview(CQCamera3DApp *app);

  //---

  bool isEqualScale() const { return equalScale_; }
  void setEqualScale(bool b) { equalScale_ = b; updateRange(); }

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

  CPoint2D pixelToView(ViewType viewType, const QPointF &p) const;

  QPointF viewQPoint(ViewType viewType, const CPoint3D &p) const;
  CPoint2D viewPoint(ViewType viewType, const CPoint3D &p) const;

  void setCameraPosition(int x, int y, bool angle);
  void setLightPosition(int x, int y, bool setDirection);

  void selectObjectAt(const QPoint &p) ;
  void selectObjectAt1(ViewType viewType, const CPoint3D &p);

  void selectObjectIn(const QPoint &p, const QRect &r);

  void selectObjectXY(const CPoint3D &p);
  void selectObjectZY(const CPoint3D &p);
  void selectObjectXZ(const CPoint3D &p);

  void drawModel();
  void updateNodeMatrices();

  void drawCamera();
  void drawLights();

  void drawPolygon(const std::vector<CPoint3D> &points) const;

  void drawCone(const CVector3D &p, const CVector3D &d, double a) const;

  void drawVector(const CVector3D &p, const CVector3D &d, const QString &label) const;

  void drawCircle(const CVector3D &o, double r);

  void drawPoint(const CVector3D &p, const QString &label) const;

  void drawPixmap(const CPoint3D &p, const QPixmap &pixmap) const;

  bool pressRange(const CDisplayRange2D &range, int x, int y, CPoint2D &p) const;

 private:
  struct FaceData {
    CGeomFace3D*          face { nullptr };
    std::vector<CPoint3D> points;
  };

  using NodeMatrices       = std::map<int, CMatrix3D>;
  using ObjectNodeMatrices = std::map<uint, NodeMatrices>;

  //---

  CQCamera3DApp* app_ { nullptr };

  CDisplayRange2D xrange_;
  CDisplayRange2D yrange_;
  CDisplayRange2D zrange_;
  CDisplayRange2D prange_;

  int ind_ { -1 };

  bool equalScale_ { true };

  SelectType selectType_ { SelectType::OBJECT };
  MouseType  mouseType_  { MouseType::CAMERA };

  ModelType modelType_     { ModelType::WIREFRAME };
  bool      cameraVisible_ { true };
  bool      lightsVisible_ { true };

  bool   pressed_     { false };
  int    mouseButton_ { 0 };
  QPoint pressPixel_;
  QPoint movePixel_;

  std::vector<FaceData> faces_;

  // draw data
  QPainter *painter_ { nullptr };

  QRectF xrect_, yrect_, zrect_, prect_;

  double xs_ { 0.0 }, ys_ { 0.0 }, zs_ { 0.0 };

  CMatrix3DH projectionMatrix_;
  CMatrix3DH viewMatrix_;
  CMatrix3DH modelMatrix_;

  ObjectNodeMatrices objectNodeMatrices_;

  QPixmap lightPixmap_;

  CQRubberBand* rubberBand_ { nullptr };
};

#endif
