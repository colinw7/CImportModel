#include <CQNewGLDrawTree.h>
#include <CQNewGLShaderProgram.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLModel.h>
#include <CQNewGLUtil.h>

#include <CQGLBuffer.h>
#include <CQGLUtil.h>
#include <CQGLTexture.h>
#include <CGeometry3D.h>
#include <CGeomTexture.h>

#include <CDrawTree3D.h>

class CQNewGLDrawTreeImpl  : public CDrawTree3D {
 public:
  CQNewGLDrawTreeImpl(CQNewGLDrawTree *tree) :
   tree_(tree) {
  }

 private:
  CQNewGLDrawTree* tree_ { nullptr };
};

//---

CQNewGLShaderProgram *CQNewGLDrawTree::shaderProgram_;

void
CQNewGLDrawTree::
initShader(CQNewGLCanvas *canvas)
{
  shaderProgram_ = new CQNewGLShaderProgram(canvas);
  shaderProgram_->addShaders("tree.vs", "tree.fs");
}

CQNewGLDrawTree::
CQNewGLDrawTree(CQNewGLCanvas *canvas) :
 CQNewGLObject(canvas)
{
}

void
CQNewGLDrawTree::
setActive(bool b)
{
  active_ = b;

  updateGeometry_ = true;

  canvas_->update();
}

void
CQNewGLDrawTree::
addGeometry()
{
  if (! updateGeometry_)
    return;

  initBuffer();

  //---

  // generate tree
  CQNewGLDrawTreeImpl draw(this);

  draw.setWidth (width_);
  draw.setHeight(height_);

  draw.setLeftAlpha (leftAlpha_);
  draw.setRightAlpha(rightAlpha_);

  draw.setLeftDirection (leftDirection_);
  draw.setRightDirection(rightDirection_);

  draw.setTreeDepth(depth_);

  draw.generate();

  double h = draw.getBBox().getYSize();

  //---

  // add geometry

  int pos = 0;

#if 0
  auto addPoint = [&](const CPoint3D &p, const CRGBA &c) {
    buffer_->addPoint(p.x, p.y, p.z);
    buffer_->addColor(c);
  };
#endif

  auto addLine = [&](const CPoint3D &p1, const CPoint3D &p2, double r, int depth) {
#if 0
    CQNewGLFaceData faceData;

    faceData.pos = pos;
    faceData.len = 2;

    addPoint(p1, color_);
    addPoint(p2, color_);

    faceDatas_.push_back(faceData);

    pos += faceData.len;
#else
    auto color = color1_.blended(color2_, CMathUtil::map(depth, 0.0, depth_ - 1.0, 1.0, 0.0));

    CQNewGLCanvas::ShapeData shapeData;

    shapeData.color = color;

    canvas_->addCylinder(buffer_, p1 + CPoint3D(0, -h/2.0, 0), p2 + CPoint3D(0, -h/2.0, 0),
                         r, shapeData, faceDatas_, pos);
#endif
  };

  //---

  auto *root = draw.root();

  addLine(root->p1, root->p2, root->width, root->depth);

  std::vector<CDrawTree3D::Line *> workLines;

  if (root->left ) workLines.push_back(root->left );
  if (root->right) workLines.push_back(root->right);

  while (! workLines.empty()) {
    auto *line = workLines.back();

    workLines.pop_back();

    addLine(line->p1, line->p2, line->width, line->depth);

    if (line->left ) workLines.push_back(line->left );
    if (line->right) workLines.push_back(line->right);
  }

  buffer_->load();
}

void
CQNewGLDrawTree::
drawGeometry()
{
  if (! active_)
    return;

  if (updateGeometry_) {
    addGeometry();

    updateGeometry_ = false;
  }

  //---

  auto *program = CQNewGLDrawTree::shaderProgram();

  program->bind();

  buffer_->bind();

  //---

  program->setUniformValue("viewPos", CQGLUtil::toVector(canvas_->viewPos()));

  auto modelMatrix = CMatrix3D::identity();
  canvas_->addShaderMVP(program, modelMatrix);

  //---

  canvas_->addShaderLights(program);

  //---

  canvas_->addShaderLightGlobals(program);

  program->setUniformValue("shininess", 10.0f);
  program->setUniformValue("emissionColor", CQGLUtil::toVector(QColor(255, 255, 255)));

  program->setUniformValue("isSelected", isSelected());

  //---

  for (const auto &faceData : faceDatas()) {
    if (isWireframe()) {
      program->setUniformValue("isWireframe", true);

      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

      glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
    }

    if (isSolid()) {
      program->setUniformValue("isWireframe", false);

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

      glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
    }
  }

  //---

  buffer_->unbind();

  program->release();
}
