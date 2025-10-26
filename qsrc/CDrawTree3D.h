#ifndef CDRAW_TREE_3D_H
#define CDRAW_TREE_3D_H

#include <CPoint3D.h>
#include <CVector3D.h>
#include <CBBox3D.h>

class CTurtle3D;

class CDrawTree3D {
 public:
  struct Line {
    CPoint3D p1;
    CPoint3D p2;
    double   width { 1.0 };
    int      depth { 0 };
    Line*    left  { nullptr };
    Line*    right { nullptr };
  };

 public:
  CDrawTree3D();

  virtual ~CDrawTree3D() { }

#if 0
  bool read(const std::string &filename);
#endif

  void generate();

  Line *root() const { return root_; }

  double width() const { return treeWidth_; }
  void setWidth(double r) { treeWidth_ = r; }

  double height() const { return treeHeight_; }
  void setHeight(double r) { treeHeight_ = r; }

  double leftAlpha() const { return leftAlpha_; }
  void setLeftAlpha(double r) { leftAlpha_ = r; }

  double rightAlpha() const { return rightAlpha_; }
  void setRightAlpha(double r) { rightAlpha_ = r; }

  const CVector3D &leftDirection() const { return leftDirection_; }
  void setLeftDirection(const CVector3D &v) { leftDirection_ = v.normalized(); }

  const CVector3D &rightDirection() const { return rightDirection_; }
  void setRightDirection(const CVector3D &v) { rightDirection_ = v.normalized(); }

  int treeDepth() const { return treeDepth_; }
  void setTreeDepth(int i) { treeDepth_ = i; }

  CBBox3D getBBox() const { return CBBox3D(canvasXMin_, canvasYMin_, canvasZMin_,
                                           canvasXMax_, canvasYMax_, canvasZMax_); }

 protected:
  void addBranch(Line *, double, const CVector3D &, int);

  Line *addLine(const CPoint3D &p, double width, int depth);

  void updateRange(Line *line);

  CVector3D lineDirection(Line *line) const;

  CVector3D perturb(const CVector3D &v) const;

 protected:
  CTurtle3D *turtle_ { nullptr };

  Line *root_ { nullptr };

  // tree size
  double treeHeight_ { 10.0 };
  double treeWidth_  { 1.0 };

  // tree branch reduction factor
  double leftAlpha_  { 1.1 };
  double rightAlpha_ { 1.1 };

  // tree branch direction
  CVector3D leftDirection_  { 1.0, 2.0, 0.0 };
  CVector3D rightDirection_ { 1.0, 2.0, 0.0 };

  // tree branching depth
  int treeDepth_ { 0 };

  // generated data range
  double canvasXMin_ { 0.0 };
  double canvasYMin_ { 0.0 };
  double canvasZMin_ { 0.0 };
  double canvasXMax_ { 1.0 };
  double canvasYMax_ { 1.0 };
  double canvasZMax_ { 1.0 };

  double leftWidthFactor_   { 1.0 };
  double leftHeightFactor_  { 1.0 };
  double rightWidthFactor_  { 1.0 };
  double rightHeightFactor_ { 1.0 };
};

#endif
