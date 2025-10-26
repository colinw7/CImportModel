#include <CQNewGLAxes.h>
#include <CQNewGLShaderProgram.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLFont.h>
#include <CQGLBuffer.h>
#include <CQGLUtil.h>
#include <CInterval.h>

CQNewGLShaderProgram* CQNewGLAxes::shaderProgram_;

CQNewGLAxes::
CQNewGLAxes(CQNewGLCanvas *canvas) :
 CQNewGLObject(canvas)
{
}

void
CQNewGLAxes::
initShader(CQNewGLCanvas *canvas)
{
  shaderProgram_ = new CQNewGLShaderProgram(canvas);

  shaderProgram_->addShaders("axis.vs", "axis.fs");
}

void
CQNewGLAxes::
updateGeometry()
{
  initBuffer();

  //---

  auto lineWidth = lineWidth_;

  if (lineWidth < 0)
    lineWidth = canvas_->sceneScale()/500.0;

  auto color = color_;

  //---

  int pos = 0;

#if 0
  auto addPoint = [&](const CPoint3D &p, const CPoint3D &n, const CRGBA &c) {
    buffer_->addPoint(p.x, p.y, p.z);
    buffer_->addNormal(n.x, n.y, n.z);
    buffer_->addColor(c);
  };
#endif

  auto addLine = [&](const CPoint3D &c1, const CPoint3D &c2, const CPoint3D & /*dp*/) {
#if 0
    CQNewGLFaceData faceData;

    faceData.pos = pos;
    faceData.len = 4;

    auto dp1 = lineWidth*dp;

    auto p1 = c1 - dp1;
    auto p2 = c1 + dp1;
    auto p3 = c2 + dp1;
    auto p4 = c2 - dp1;
    auto n  = CPoint3D(0, 0, 1);

    addPoint(p1, n, color);
    addPoint(p2, n, color);
    addPoint(p3, n, color);
    addPoint(p4, n, color);

    faceDatas_.push_back(faceData);

    pos += faceData.len;
#else
    CQNewGLCanvas::ShapeData shapeData;

    shapeData.color = color;

    canvas_->addCylinder(buffer_, c1, c2, lineWidth, shapeData, faceDatas_, pos);
#endif
  };

  //---

  auto sceneScale = canvas_->sceneScale();

  CInterval interval(-sceneScale, sceneScale);

#if 0
  addLine(CPoint3D(-sceneSize_.getX(), 0, 0), CPoint3D(sceneSize_.getX(), 0, 0));
  addLine(CPoint3D(0, -sceneSize_.getY(), 0), CPoint3D(0, sceneSize_.getY(), 0));
  addLine(CPoint3D(0, 0, -sceneSize_.getZ()), CPoint3D(0, 0, sceneSize_.getZ()));
#else
  for (uint i = 0; i <= interval.calcNumMajor(); ++i) {
    double x = interval.interval(i);

    addLine(CPoint3D(x, 0, -sceneScale), CPoint3D(x, 0, sceneScale), CPoint3D(0, 1, 0));
    addLine(CPoint3D(x, -sceneScale, 0), CPoint3D(x, sceneScale, 0), CPoint3D(0, 0, 1));

    addLine(CPoint3D(-sceneScale, x, 0), CPoint3D(sceneScale, x, 0), CPoint3D(0, 0, 1));
    addLine(CPoint3D(0, x, -sceneScale), CPoint3D(0, x, sceneScale), CPoint3D(1, 0, 0));

    addLine(CPoint3D(0, -sceneScale, x), CPoint3D(0, sceneScale, x), CPoint3D(1, 0, 0));
    addLine(CPoint3D(-sceneScale, 0, x), CPoint3D(sceneScale, 0, x), CPoint3D(0, 1, 0));
  }
#endif

  //---

  buffer_->load();

  //---

  // add text labels
  clearTexts();

  auto *font = canvas_->getFont();

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

  (void) createText("X", CPoint3D(sceneScale, -ts1, 0), ts1);
  (void) createText("Y", CPoint3D(0, sceneScale, ts1), ts1);
  (void) createText("Z", CPoint3D(0, ts1, sceneScale), ts1);

  for (uint i = 0; i <= interval.calcNumMajor(); ++i) {
    double x = interval.interval(i);

    auto label = QString("%1").arg(x);

    auto d = label.length()*ts2/2.0;

    (void) createText(label, CPoint3D(x - d, 0, 0), ts2);
    (void) createText(label, CPoint3D(0, x - d, 0), ts2);
    (void) createText(label, CPoint3D(0, 0, x - d), ts2);
  }

  updateTexts();
}

void
CQNewGLAxes::
drawGeometry()
{
  buffer_->bind();

  shaderProgram_->bind();

  //---

  auto modelMatrix = CMatrix3D::identity();
  canvas_->addShaderMVP(shaderProgram_, modelMatrix);

  //---

  // render axis
  for (const auto &faceData : faceDatas_) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
  }

  buffer_->unbind();

  //---

  shaderProgram_->release();

  //---

  drawTexts();
}
