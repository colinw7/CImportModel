#include <CDrawTree3D.h>
#include <CTurtle3D.h>
#include <CFile.h>
#include <CStrUtil.h>

CDrawTree3D::
CDrawTree3D()
{
  turtle_ = nullptr;

  canvasXMin_ = 0.0; canvasYMin_ = 0.0; canvasZMin_ = 0.0;
  canvasXMax_ = 1.0; canvasYMax_ = 1.0; canvasZMax_ = 0.0;

  treeHeight_ = 10.0;
  treeWidth_  = 1.0;

  treeLeftAlpha_  = 1.1;
  treeRightAlpha_ = 1.1;

  treeLeftDirection_  = CVector3D( 1.0, 2.0, 0.0).normalized();
  treeRightDirection_ = CVector3D(-1.0, 2.0, 0.0).normalized();

  treeDepth_ = 6;

  treeLeftWidthFactor_   = 1.0;
  treeLeftHeightFactor_  = 1.0;
  treeRightWidthFactor_  = 1.0;
  treeRightHeightFactor_ = 1.0;
}

#if 0
bool
CDrawTree3D::
read(const std::string &filename)
{
  CFile file(filename);

  std::vector<std::string> lines;

  file.toLines(lines);

  if (lines.size() != 7) {
    std::cerr << "Bad input file" << std::endl;
    return false;
  }

  double height;
  double width;
  double lalpha;
  double ralpha;
  double langle;
  double rangle;
  int    depth;

  if (! CStrUtil::toReal   (lines[0], &height) ||
      ! CStrUtil::toReal   (lines[1], &width ) ||
      ! CStrUtil::toReal   (lines[2], &lalpha) ||
      ! CStrUtil::toReal   (lines[3], &ralpha) ||
      ! CStrUtil::toReal   (lines[4], &langle) ||
      ! CStrUtil::toReal   (lines[5], &rangle) ||
      ! CStrUtil::toInteger(lines[6], &depth )) {
    std::cerr << "Bad input file" << std::endl;
    return false;
  }

  treeHeight_ = height;
  treeWidth_  = width;

  treeLeftAlpha_      = lalpha;
  treeRightAlpha_     = ralpha;
  treeLeftDirection_  = langle;
  treeRightDirection_ = rangle;
  treeDepth_          = depth;

  return true;
}
#endif

void
CDrawTree3D::
generate()
{
  canvasXMin_ = 0.0; canvasYMin_ = 0.0; canvasZMin_ = 0.0;
  canvasXMax_ = 0.0; canvasYMax_ = 0.0; canvasZMax_ = 0.0;

  treeLeftWidthFactor_   = std::pow(2.0, -1.0/     treeLeftAlpha_  );
  treeLeftHeightFactor_  = std::pow(2.0, -2.0/(3.0*treeLeftAlpha_ ));
  treeRightWidthFactor_  = std::pow(2.0, -1.0/     treeRightAlpha_ );
  treeRightHeightFactor_ = std::pow(2.0, -2.0/(3.0*treeRightAlpha_));

  turtle_ = new CTurtle3D;

  turtle_->setPoint(CPoint3D(0.0, 0.0, 0.0));
  turtle_->setDirection(CVector3D(0.0, 1.0, 0.0));

  turtle_->step(treeHeight_);

  int depth = 0;

  root_ = addLine(turtle_->prevPoint(), treeWidth_, depth);

  root_->p2 = turtle_->point();

  updateRange(root_);

  turtle_->setDirection(lineDirection(root_) + treeLeftDirection_);

  root_->left = addLine(root_->p2, root_->width*treeLeftWidthFactor_, depth + 1);

  addBranch(root_->left,
            treeLeftHeightFactor_*treeHeight_,
            treeLeftDirection_,
            depth + 1);

  turtle_->setDirection(lineDirection(root_) + treeRightDirection_);

  root_->right = addLine(root_->p2, root_->width*treeRightWidthFactor_, depth + 1);

  addBranch(root_->right,
            treeRightHeightFactor_*treeHeight_,
            treeRightDirection_,
            depth + 1);

  delete turtle_;

  turtle_ = nullptr;
}

void
CDrawTree3D::
addBranch(Line *line, double height, const CVector3D & /*angle*/, int depth)
{
  turtle_->setPoint(line->p1);

  turtle_->step(height);

  line->p2 = turtle_->point();

  updateRange(line);

  if (depth >= treeDepth_)
    return;

  turtle_->setDirection(lineDirection(line) + treeLeftDirection_);

  line->left = addLine(line->p2, line->width*treeLeftWidthFactor_, depth);

  addBranch(line->left,
            treeLeftHeightFactor_*height,
            treeLeftDirection_,
            depth + 1);

  turtle_->setDirection(lineDirection(line) + treeRightDirection_);

  line->right = addLine(line->p2, line->width*treeRightWidthFactor_, depth);

  addBranch(line->right,
            treeRightHeightFactor_*height,
            treeRightDirection_,
            depth + 1);
}

CDrawTree3D::Line *
CDrawTree3D::
addLine(const CPoint3D &p, double width, int depth)
{
  auto *line = new Line;

  line->p1    = p;
  line->depth = depth;
  line->width = width;
  line->left  = nullptr;
  line->right = nullptr;

  return line;
}

void
CDrawTree3D::
updateRange(Line *line)
{
  if (line->p2.x < canvasXMin_) canvasXMin_ = line->p2.x;
  if (line->p2.x > canvasXMax_) canvasXMax_ = line->p2.x;
  if (line->p2.y < canvasYMin_) canvasYMin_ = line->p2.y;
  if (line->p2.y > canvasYMax_) canvasYMax_ = line->p2.y;
  if (line->p2.z < canvasZMin_) canvasZMin_ = line->p2.z;
  if (line->p2.z > canvasZMax_) canvasZMax_ = line->p2.z;
}

CVector3D
CDrawTree3D::
lineDirection(Line *line) const
{
  return CVector3D(line->p2.x - line->p1.x, line->p2.y - line->p1.y, line->p2.z - line->p1.z);
}
