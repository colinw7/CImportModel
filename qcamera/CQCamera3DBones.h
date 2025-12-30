#ifndef CQCamera3DBones_H
#define CQCamera3DBones_H

#include <CQCamera3DApp.h>

#include <CWindowRange2D.h>
//#include <CDisplayRange2D.h>
#include <CWindowRange2D.h>
#include <CMatrix3DH.h>
#include <CPoint3D.h>

#include <QFrame>

class CQCamera3DApp;
class CGeomFace3D;
class CGeomNodeData;
class CVector3D;

class CQCamera3DBones : public QFrame {
  Q_OBJECT

 public:
  enum class ModelType {
    NONE,
    WIREFRAME,
    SOLID
  };

 public:
  CQCamera3DBones(CQCamera3DApp *app);

  //---

  bool isEqualScale() const { return equalScale_; }
  void setEqualScale(bool b) { equalScale_ = b; updateRange(); }

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

  void drawBones();
  void drawModel();

  void drawCube(const CPoint3D &c, double s) const;

  void drawPolygon(const std::vector<CPoint3D> &points) const;

  void drawLine(const CPoint3D &p1, const CPoint3D &p2, const QString &label="") const;

  void drawCircle(const CVector3D &o, double r);

  void drawPoint(const CVector3D &p, const QString &label) const;

  void drawPixmap(const CPoint3D &p, const QPixmap &pixmap) const;

  void selectNode(int x, int y);

  void selectNodeXY(const CPoint2D &p);
  void selectNodeZY(const CPoint2D &p);
  void selectNodeXZ(const CPoint2D &p);

  bool pressRange(const CDisplayRange2D &range, int x, int y, CPoint2D &p) const;

  CMatrix3D getNodeTransform(CGeomNodeData &nodeData) const;

 private:
  using NodeMatrices       = std::map<int, CMatrix3D>;
  using ObjectNodeMatrices = std::map<uint, NodeMatrices>;

  using NodeCenters = std::map<uint, CPoint3D>;
  using ObjectNodes = std::map<uint, NodeCenters>;

  //---

  CQCamera3DApp* app_ { nullptr };

  CDisplayRange2D xrange_;
  CDisplayRange2D yrange_;
  CDisplayRange2D zrange_;
  CDisplayRange2D prange_;

  int ind_ { -1 };

  bool equalScale_ { true };

  bool showModel_       { false };
  bool showBoneNodes_   { false };
  bool showPointJoints_ { false };
  bool onlyJoints_      { false };

  bool pressed_     { false };
  int  mouseButton_ { 0 };

  // draw data
  QPainter *painter_ { nullptr };

  QRectF xrect_, yrect_, zrect_, prect_;

  double xs_ { 0.0 }, ys_ { 0.0 }, zs_ { 0.0 };

  CMatrix3DH projectionMatrix_;
  CMatrix3DH viewMatrix_;
  CMatrix3DH modelMatrix_;

  std::string animName_;
  double      animTime_ { 0.0 };
  bool        useAnim_  { false };

  ObjectNodeMatrices objectNodeMatrices_;


  ObjectNodes objectNodes_;
};

#endif
