#ifndef CQCamera3DOverview_H
#define CQCamera3DOverview_H

//#include <CDisplayRange2D.h>
#include <CWindowRange2D.h>
#include <CPoint3D.h>

#include <QFrame>

class CQCamera3DApp;
class CGeomFace3D;

class CQCamera3DOverview : public QFrame {
 public:
  CQCamera3DOverview(CQCamera3DApp *app);

  bool isEqualScale() const { return equalScale_; }
  void setEqualScale(bool b) { equalScale_ = b; updateRange(); }

  void resizeEvent(QResizeEvent *) override;

  void paintEvent(QPaintEvent *) override;

  void mousePressEvent  (QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;
  void mouseMoveEvent   (QMouseEvent *e) override;

  void keyPressEvent(QKeyEvent *e) override;

 private:
  void updateRange();

  void setCameraPosition(int x, int y, bool angle);

  void selectObject(int x, int y) ;

  void selectObjectXY(const CPoint3D &p);
  void selectObjectZY(const CPoint3D &p);
  void selectObjectXZ(const CPoint3D &p);

  bool pressRange(const CDisplayRange2D &range, int x, int y, CPoint2D &p) const;

 private:
  struct FaceData {
    CGeomFace3D*          face { nullptr };
    std::vector<CPoint3D> points;
  };

  CQCamera3DApp*  app_   { nullptr };
  CDisplayRange2D xrange_;
  CDisplayRange2D yrange_;
  CDisplayRange2D zrange_;
  CDisplayRange2D prange_;

  int ind_ { -1 };

  bool equalScale_ { true };

  bool pressed_     { false };
  int  mouseButton_ { 0 };

  std::vector<FaceData> faces_;
};

#endif
