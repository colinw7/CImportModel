#include <CQNewGLBasis.h>
#include <CQNewGLShaderProgram.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLModelObject.h>
#include <CQNewGLFont.h>
#include <CQNewGLShapes.h>
#include <CQNewGLModel.h>
#include <CQNewGLUtil.h>

#include <CQGLBuffer.h>
#include <CQGLUtil.h>
#include <CGeomObject3D.h>

CQNewGLBasis::
CQNewGLBasis(CQNewGLCanvas *canvas) :
 CQNewGLObject(canvas), canvas_(canvas)
{
}

CQNewGLShaderProgram *
CQNewGLBasis::
shaderProgram()
{
  return getShader("basis.vs", "basis.fs");
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

  auto *app = canvas_->app();

  auto lineWidth = this->lineWidth();
  auto lineSize  = this->lineSize();

  if (lineWidth < 0) {
    auto bbox = app->getModelBBox(object);

    lineWidth = bbox.getMaxSize()/250.0;
  }

  if (lineSize < 0)
    lineSize = 1.0;

  auto color = QColorToRGBA(this->color());

  //---

  faceDataList_.clear();

  auto addLine = [&](const CPoint3D &c1, const CPoint3D &c2, const CPoint3D & /*dp*/) {
    CQNewGLShapes::ShapeData shapeData;

    shapeData.color = color;

    CQNewGLShapes::addCylinder(buffer_, c1, c2, lineWidth, shapeData, faceDataList_);
  };

  //---

  CPoint3D pu, pv, pw;

  if (object) {
    CVector3D u, v, w;
    canvas_->getBasis(object, u, v, w);

    auto bbox = app->getModelBBox(object);

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

  auto *program = shaderProgram();

  buffer_->bind();

  program->bind();

  //---

  // model matrix
//auto modelMatrix = canvas_->getModelMatrix();
  auto modelMatrix = object->getHierTransform();
  canvas_->addShaderMVP(program, modelMatrix);

  //---

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
