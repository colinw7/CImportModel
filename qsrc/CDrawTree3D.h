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

  double treeLeftAlpha() const { return treeLeftAlpha_; }
  void setTreeLeftAlpha(double r) { treeLeftAlpha_ = r; }

  double treeRightAlpha() const { return treeRightAlpha_; }
  void setTreeRightAlpha(double r) { treeRightAlpha_ = r; }

  const CVector3D &treeLeftDirection() const { return treeLeftDirection_; }
  void setTreeLeftDirection(const CVector3D &v) { treeLeftDirection_ = v; }

  const CVector3D &treeRightDirection() const { return treeRightDirection_; }
  void setTreeRightDirection(const CVector3D &v) { treeRightDirection_ = v; }

  int treeDepth() const { return treeDepth_; }
  void setTreeDepth(int i) { treeDepth_ = i; }

  CBBox3D getBBox() const { return CBBox3D(canvasXMin_, canvasYMin_, canvasZMin_,
                                           canvasXMax_, canvasYMax_, canvasZMax_); }

 protected:
  void addBranch(Line *, double, const CVector3D &, int);

  Line *addLine(const CPoint3D &p, double width, int depth);

  void updateRange(Line *line);

  CVector3D lineDirection(Line *line) const;

 protected:
  static const char *colors_[];

  CTurtle3D *turtle_ { nullptr };

  Line *root_ { nullptr };

  // tree size
  double treeHeight_ { 10.0 };
  double treeWidth_  { 1.0 };

  // tree branch reduction factor
  double treeLeftAlpha_  { 1.1 };
  double treeRightAlpha_ { 1.1 };

  // tree branch direction
  CVector3D treeLeftDirection_  { 1.0, 2.0, 0.0 };
  CVector3D treeRightDirection_ { 1.0, 2.0, 0.0 };

  // tree branching depth
  int treeDepth_ { 0 };

  // generated data range
  double canvasXMin_ { 0.0 };
  double canvasYMin_ { 0.0 };
  double canvasZMin_ { 0.0 };
  double canvasXMax_ { 1.0 };
  double canvasYMax_ { 1.0 };
  double canvasZMax_ { 1.0 };

  double treeLeftWidthFactor_   { 1.0 };
  double treeLeftHeightFactor_  { 1.0 };
  double treeRightWidthFactor_  { 1.0 };
  double treeRightHeightFactor_ { 1.0 };
};

#endif
