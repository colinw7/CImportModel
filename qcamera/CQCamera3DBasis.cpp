#include <CQCamera3DBasis.h>
#include <CQCamera3DShaderProgram.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DGeomObject.h>
#include <CQCamera3DFont.h>
#include <CQCamera3DShapes.h>
#include <CQCamera3DCamera.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DUtil.h>

#include <CQGLBuffer.h>
#include <CQGLUtil.h>
#include <CGeomObject3D.h>

CQCamera3DBasis::
CQCamera3DBasis(CQCamera3DCanvas *canvas) :
 CQCamera3DObject(canvas), canvas_(canvas)
{
}

CQCamera3DShaderProgram *
CQCamera3DBasis::
shaderProgram()
{
  return getShader("basis.vs", "basis.fs");
}

void
CQCamera3DBasis::
updateGeometry()
{
  initBuffer();

  //---

  auto *object = canvas_->currentGeomObject(/*includeRoot*/true);
  if (! object) return;

  auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);

  auto bbox = object1->bbox();

  //---

  auto lineWidth = this->lineWidth();
  auto lineSize  = this->lineSize();

  if (lineWidth < 0) {
    lineWidth = bbox.getMaxSize()/250.0;
  }

  if (lineSize < 0)
    lineSize = 1.0;

  auto color = QColorToRGBA(this->color());

  //---

  faceDataList_.clear();

  auto addLine = [&](const CPoint3D &c1, const CPoint3D &c2, const CPoint3D & /*dp*/) {
    CQCamera3DShapes::ShapeData shapeData;

    shapeData.color = color;

    CQCamera3DShapes::addCylinder(buffer_, c1, c2, lineWidth, shapeData, faceDataList_);
  };

  //---

  CPoint3D pu, pv, pw;

  if (object) {
    CVector3D u, v, w;
    canvas_->getBasis(object, u, v, w);

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

    auto *text = new CQCamera3DText(str);

    text->setFont(font);
    text->setColor(CQCamera3DFont::Color(1, 1, 1));
    text->setPosition(CGLVector3D(pos1.x, pos1.y, pos1.z));
    text->setSize(size);

    addText(text);

    return text;
  };

  if (object) {
    auto ts = bbox.getMaxSize()/10.0;

    (void) createText("U", pu, ts);
    (void) createText("V", pv, ts);
    (void) createText("W", pw, ts);

    updateTexts();
  }
}

void
CQCamera3DBasis::
drawGeometry()
{
  if (! isValid()) {
    updateGeometry();

    setValid(true);
  }

  //---

  auto *object = canvas_->currentGeomObject(/*includeRoot*/true);
  if (! object) return;

  //---

  auto *program = shaderProgram();

  buffer_->bind();

  program->bind();

  //---

  // camera projection
  auto *camera = canvas_->getCurrentCamera();

  auto projectionMatrix = camera->worldMatrix();
  program->setUniformValue("projection", CQGLUtil::toQMatrix(projectionMatrix));

  // camera/view transformation
  auto viewMatrix = camera->viewMatrix();
  program->setUniformValue("view", CQGLUtil::toQMatrix(viewMatrix));

  // view pos
  program->setUniformValue("viewPos", CQGLUtil::toVector(camera->position()));

  // model matrix
  auto modelMatrix = object->getHierTransform();
  program->setUniformValue("model", CQGLUtil::toQMatrix(modelMatrix));

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
