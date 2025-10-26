#include <CDrawTree3D.h>
#include <CTurtle3D.h>
#include <CFile.h>
#include <CStrUtil.h>
#include <CMathRand.h>

CDrawTree3D::
CDrawTree3D()
{
  turtle_ = nullptr;

  canvasXMin_ = 0.0; canvasYMin_ = 0.0; canvasZMin_ = 0.0;
  canvasXMax_ = 1.0; canvasYMax_ = 1.0; canvasZMax_ = 0.0;

  treeHeight_ = 10.0;
  treeWidth_  = 1.0;

  leftAlpha_  = 1.1;
  rightAlpha_ = 1.1;

  leftDirection_  = CVector3D( 1.0, 2.0, 0.0).normalized();
  rightDirection_ = CVector3D(-1.0, 2.0, 0.0).normalized();

  treeDepth_ = 6;

  leftWidthFactor_   = 1.0;
  leftHeightFactor_  = 1.0;
  rightWidthFactor_  = 1.0;
  rightHeightFactor_ = 1.0;
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

  leftAlpha_      = lalpha;
  rightAlpha_     = ralpha;
  leftDirection_  = langle;
  rightDirection_ = rangle;
  treeDepth_      = depth;

  return true;
}
#endif

void
CDrawTree3D::
generate()
{
  canvasXMin_ = 0.0; canvasYMin_ = 0.0; canvasZMin_ = 0.0;
  canvasXMax_ = 0.0; canvasYMax_ = 0.0; canvasZMax_ = 0.0;

  leftWidthFactor_   = std::pow(2.0, -1.0/     leftAlpha_  );
  leftHeightFactor_  = std::pow(2.0, -2.0/(3.0*leftAlpha_ ));
  rightWidthFactor_  = std::pow(2.0, -1.0/     rightAlpha_ );
  rightHeightFactor_ = std::pow(2.0, -2.0/(3.0*rightAlpha_));

  turtle_ = new CTurtle3D;

  turtle_->setPoint(CPoint3D(0.0, 0.0, 0.0));
  turtle_->setDirection(CVector3D(0.0, 1.0, 0.0));

  turtle_->step(treeHeight_);

  int depth = 0;

  root_ = addLine(turtle_->prevPoint(), treeWidth_, depth);

  root_->p2 = turtle_->point();

  updateRange(root_);

  turtle_->setDirection(lineDirection(root_) + perturb(leftDirection_));

  root_->left = addLine(root_->p2, root_->width*leftWidthFactor_, depth + 1);

  addBranch(root_->left, leftHeightFactor_*treeHeight_, leftDirection_, depth + 1);

  turtle_->setDirection(lineDirection(root_) + perturb(rightDirection_));

  root_->right = addLine(root_->p2, root_->width*rightWidthFactor_, depth + 1);

  addBranch(root_->right, rightHeightFactor_*treeHeight_, rightDirection_, depth + 1);

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

  turtle_->setDirection(lineDirection(line) + perturb(leftDirection_));

  line->left = addLine(line->p2, line->width*leftWidthFactor_, depth);

  addBranch(line->left, leftHeightFactor_*height, leftDirection_, depth + 1);

  turtle_->setDirection(lineDirection(line) + perturb(rightDirection_));

  line->right = addLine(line->p2, line->width*rightWidthFactor_, depth);

  addBranch(line->right, rightHeightFactor_*height, rightDirection_, depth + 1);
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

CVector3D
CDrawTree3D::
perturb(const CVector3D &v) const
{
  double xp = CMathRand::randInRange(-0.1, 0.1);
  double yp = CMathRand::randInRange(-0.1, 0.1);
  double zp = CMathRand::randInRange(-0.1, 0.1);

  return (v + CVector3D(xp, yp, zp)).normalize();
}
