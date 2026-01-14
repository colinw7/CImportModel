#include <CQNewGLAxes.h>
#include <CQNewGLWidget.h>
#include <CQNewGLModel.h>
#include <CQNewGLShaderProgram.h>
#include <CQNewGLFont.h>
#include <CQNewGLShapes.h>
#include <CQNewGLUtil.h>

#include <CQGLBuffer.h>
#include <CQGLUtil.h>
#include <CInterval.h>

CQNewGLAxes::
CQNewGLAxes(CQNewGLWidget *widget) :
 CQNewGLObject(widget)
{
}

CQNewGLShaderProgram *
CQNewGLAxes::
shaderProgram()
{
  return getShader("axis.vs", "axis.fs");
}

void
CQNewGLAxes::
updateGeometry()
{
  initBuffer();

  //---

  auto sceneCenter = widget_->sceneCenter();
  auto sceneSize   = widget_->sceneSize();
  auto sceneScale  = widget_->sceneScale();

  //---

  auto lineWidth = lineWidth_;

  if (lineWidth < 0)
    lineWidth = sceneScale/500.0;

  auto color = color_;

  //---

  faceDataList_.clear();

  auto addLine = [&](const CPoint3D &c1, const CPoint3D &c2, const CPoint3D & /*dp*/) {
    CQNewGLShapes::ShapeData shapeData;

    shapeData.color = color;

    CQNewGLShapes::addCylinder(buffer_, c1, c2, lineWidth, shapeData, faceDataList_);
  };

  //---

  double xc = sceneCenter.x;
  double yc = sceneCenter.y;
  double zc = sceneCenter.z;

  auto xsize = sceneSize.getX()/2.0;
  auto ysize = sceneSize.getY()/2.0;
  auto zsize = sceneSize.getZ()/2.0;

  auto x1 = xc - xsize, x2 = xc + xsize;
  auto y1 = yc - ysize, y2 = yc + ysize;
  auto z1 = zc - zsize, z2 = zc + zsize;

  CInterval xinterval(x1, x2);
  CInterval yinterval(y1, y2);
  CInterval zinterval(z1, z2);

  for (uint i = 0; i <= xinterval.calcNumMajor(); ++i) {
    double x = xinterval.interval(i);

    addLine(CPoint3D(x, yc, z1), CPoint3D(x, yc, z2), CPoint3D(0, 1, 0));
    addLine(CPoint3D(x, y1, zc), CPoint3D(x, y2, zc), CPoint3D(0, 0, 1));
  }

  for (uint i = 0; i <= yinterval.calcNumMajor(); ++i) {
    double y = yinterval.interval(i);

    addLine(CPoint3D(x1, y, zc), CPoint3D(x2, y, zc), CPoint3D(0, 0, 1));
    addLine(CPoint3D(xc, y, z1), CPoint3D(xc, y, z2), CPoint3D(1, 0, 0));
  }

  for (uint i = 0; i <= zinterval.calcNumMajor(); ++i) {
    double z = zinterval.interval(i);

    addLine(CPoint3D(xc, y1, z), CPoint3D(xc, y2, z), CPoint3D(1, 0, 0));
    addLine(CPoint3D(x1, yc, z), CPoint3D(x2, yc, z), CPoint3D(0, 1, 0));
  }

  //---

  buffer_->load();

  //---

  // add text labels
  clearTexts();

  auto *font = widget_->getFont();

  auto createText = [&](const QString &str, const CPoint3D &pos, double size) {
    auto *text = new CQNewGLText(str);

    text->setFont(font);
    text->setColor(CQNewGLFont::Color(1, 1, 1));
    text->setPosition(CGLVector3D(pos.x, pos.y, pos.z));
    text->setSize(size);

    addText(text);

    return text;
  };

  auto ts1 = sceneScale/10.0;
  auto ts2 = sceneScale/20.0;

  (void) createText("X", CPoint3D(x2, yc - ts1, zc      ), ts1);
  (void) createText("Y", CPoint3D(xc, y2      , zc + ts1), ts1);
  (void) createText("Z", CPoint3D(xc, yc - ts1, z2      ), ts1);

  for (uint i = 0; i <= xinterval.calcNumMajor(); ++i) {
    auto x      = xinterval.interval(i);
    auto xlabel = QString("%1").arg(x);
    auto dx     = xlabel.length()*ts2/2.0;

    (void) createText(xlabel, CPoint3D(x - dx, yc, zc), ts2);
  }

  for (uint i = 0; i <= yinterval.calcNumMajor(); ++i) {
    auto y      = yinterval.interval(i);
    auto ylabel = QString("%1").arg(y);
    auto dy     = ylabel.length()*ts2/2.0;

    (void) createText(ylabel, CPoint3D(xc, y - dy, zc), ts2);
  }

  for (uint i = 0; i <= zinterval.calcNumMajor(); ++i) {
    auto z      = zinterval.interval(i);
    auto zlabel = QString("%1").arg(z);
    auto dz     = zlabel.length()*ts2/2.0;

    (void) createText(zlabel, CPoint3D(xc, yc, z - dz), ts2);
  }

  updateTexts();
}

void
CQNewGLAxes::
drawGeometry()
{
  auto *program = shaderProgram();

  program->bind();

  //---

  auto modelMatrix = CMatrix3D::identity();
  widget_->addShaderMVP(program, modelMatrix);

  //---

  // render axis
  buffer_->bind();

  for (const auto &faceData : faceDataList_.faceDatas) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
  }

  buffer_->unbind();

  //---

  program->release();

  //---

  drawTexts();
}
