#include <CQNewGLBasis.h>
#include <CQNewGLShaderProgram.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLModelObject.h>
#include <CQNewGLFont.h>
#include <CQNewGLUtil.h>
#include <CQGLBuffer.h>
#include <CQGLUtil.h>
#include <CGeomObject3D.h>

CQNewGLShaderProgram* CQNewGLBasis::shaderProgram_;

void
CQNewGLBasis::
initShader(CQNewGLCanvas *canvas)
{
  shaderProgram_ = new CQNewGLShaderProgram(canvas);

  shaderProgram_->addShaders("basis.vs", "basis.fs");
}

CQNewGLBasis::
CQNewGLBasis(CQNewGLCanvas *canvas) :
 CQNewGLObject(canvas)
{
}

void
CQNewGLBasis::
updateGeometry()
{
  initBuffer();

  //---

  objectData_ = canvas_->getCurrentObjectData();
  if (! objectData_) return;

  auto *object = objectData_->object();

  //---

  auto lineWidth = this->lineWidth();
  auto lineSize  = this->lineSize();

  if (lineWidth < 0) {
    auto bbox = canvas_->getModelBBox(object);

    lineWidth = bbox.getMaxSize()/250.0;
  }

  if (lineSize < 0)
    lineSize = 1.0;

  auto color = QColorToRGBA(this->color());

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

    addFaceData(faceData);

    pos += faceData.len;
#else
    CQNewGLCanvas::ShapeData shapeData;

    shapeData.color = color;

    canvas_->addCylinder(buffer_, c1, c2, lineWidth, shapeData, faceDatas_, pos);
#endif
  };

  //---

  CPoint3D pu, pv, pw;

  if (object) {
    CVector3D u, v, w;
    canvas_->getBasis(object, u, v, w);

    auto bbox = canvas_->getModelBBox(object);

    auto c = bbox.getCenter();
    auto s = lineSize*bbox.getRadius();

    pu = c + s*u;
    pv = c + s*v;
    pw = c + s*w;

    addLine(c, pu, CPoint3D(0, 1, 0));
    addLine(c, pv, CPoint3D(1, 0, 0));
    addLine(c, pw, CPoint3D(0, 1, 0));
  }

  //---

  buffer_->load();

  //---

  auto *font = canvas_->getFont();

  auto transform = object->getHierTransform();

  clearTexts();

  auto createText = [&](const QString &str, const CPoint3D &pos, double size) {
    auto pos1 = transform*pos;

    auto *text = new CQNewGLText(str);

    text->setFont(font);
    text->setColor(CQNewGLFont::Color(1, 1, 1));
    text->setPosition(CGLVector3D(pos1.x, pos1.y, pos1.z));
    text->setSize(size);

    addText(text);

    return text;
  };

  if (object) {
    auto ts = canvas_->sceneScale()/10.0;

    (void) createText("U", pu, ts);
    (void) createText("V", pv, ts);
    (void) createText("W", pw, ts);

    updateTexts();
  }
}

void
CQNewGLBasis::
drawGeometry()
{
  if (! isValid()) {
    updateGeometry();

    setValid(true);
  }

  //---

  objectData_ = canvas_->getCurrentObjectData();
  if (! objectData_) return;

  auto *object = objectData_->object();

  //---

  buffer_->bind();

  shaderProgram_->bind();

  //---

  // model matrix
//auto modelMatrix = canvas_->getModelMatrix();
  auto modelMatrix = object->getHierTransform();
  canvas_->addShaderMVP(shaderProgram_, modelMatrix);

  //---

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
