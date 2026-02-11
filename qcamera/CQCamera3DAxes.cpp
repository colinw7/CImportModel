#include <CQCamera3DAxes.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DShaderProgram.h>
#include <CQCamera3DCamera.h>
#include <CQCamera3DFont.h>
#include <CQCamera3DShapes.h>

#include <CQGLBuffer.h>
#include <CQGLUtil.h>
#include <CInterval.h>

CQCamera3DAxes::
CQCamera3DAxes(CQCamera3DCanvas *canvas) :
 CQCamera3DObject(canvas)
{
}

CQCamera3DShaderProgram *
CQCamera3DAxes::
shaderProgram()
{
  return getShader("axis.vs", "axis.fs");
}

void
CQCamera3DAxes::
updateGeometry()
{
  auto *buffer = initBuffer();

  //---

  auto bbox = canvas_->bbox();

  auto sceneCenter = bbox.getCenter();
  auto sceneSize   = bbox.getSize();
  auto sceneScale  = bbox.getMaxSize();

  //---

  auto lineWidth = lineWidth_;

  if (lineWidth < 0)
    lineWidth = sceneScale/1000.0;

  auto color = color_;

  //---

  auto addLine = [&](const CPoint3D &c1, const CPoint3D &c2, const CPoint3D & /*dp*/) {
    CQCamera3DShapes::ShapeData shapeData;

    shapeData.color = color;

    CQCamera3DShapes::addCylinder(buffer, c1, c2, lineWidth, shapeData, faceDataList_);
  };

  //---

  double xc = sceneCenter.x;
  double yc = sceneCenter.y;
  double zc = sceneCenter.z;

  double xp = xpos();
  double yp = ypos();
  double zp = zpos();

  //auto xsize = sceneSize.getX()/2.0;
  //auto ysize = sceneSize.getY()/2.0;
  //auto zsize = sceneSize.getZ()/2.0;
  auto xsize = sceneScale/2.0;
  auto ysize = sceneScale/2.0;
  auto zsize = sceneScale/2.0;

  auto x1 = xc - xsize, x2 = xc + xsize;
  auto y1 = yc - ysize, y2 = yc + ysize;
  auto z1 = zc - zsize, z2 = zc + zsize;

  CInterval xinterval(x1, x2);
  CInterval yinterval(y1, y2);
  CInterval zinterval(z1, z2);

  if (isShowX())
    (void) addLine(CPoint3D(x1, yp, zp), CPoint3D(x2, yp, zp), CPoint3D(0, 1, 0));

  if (isShowGrid()) {
    for (uint i = 0; i <= xinterval.calcNumMajor(); ++i) {
      double x = xinterval.interval(i);

      addLine(CPoint3D(x, yp, z1), CPoint3D(x, yp, z2), CPoint3D(0, 1, 0));
      addLine(CPoint3D(x, y1, zp), CPoint3D(x, y2, zp), CPoint3D(0, 0, 1));
    }
  }

  if (isShowY())
    (void) addLine(CPoint3D(xp, y1, zp), CPoint3D(xp, y2, zp), CPoint3D(0, 1, 0));

  if (isShowGrid()) {
    for (uint i = 0; i <= yinterval.calcNumMajor(); ++i) {
      double y = yinterval.interval(i);

      addLine(CPoint3D(x1, y, zp), CPoint3D(x2, y, zp), CPoint3D(0, 0, 1));
      addLine(CPoint3D(xp, y, z1), CPoint3D(xp, y, z2), CPoint3D(1, 0, 0));
    }
  }

  if (isShowZ())
    (void) addLine(CPoint3D(xp, xp, z1), CPoint3D(xp, yp, z1), CPoint3D(0, 1, 0));

  if (isShowGrid()) {
    for (uint i = 0; i <= zinterval.calcNumMajor(); ++i) {
      double z = zinterval.interval(i);

      addLine(CPoint3D(xp, y1, z), CPoint3D(xp, y2, z), CPoint3D(1, 0, 0));
      addLine(CPoint3D(x1, yp, z), CPoint3D(x2, yp, z), CPoint3D(0, 1, 0));
    }
  }

  //---

  buffer->load();

  //---

  // add text labels
  clearTexts();

  auto *font = canvas_->getFont();

  auto createText = [&](const QString &str, const CPoint3D &pos, double size) {
    auto *text = new CQCamera3DText(str);

    text->setFont(font);
    text->setColor(CQCamera3DFont::Color(1, 1, 1));
    text->setPosition(CGLVector3D(pos.x, pos.y, pos.z));
    text->setSize(size);

    addText(text);

    return text;
  };

  auto ts1 = sceneScale/10.0;
  auto ts2 = sceneScale/20.0;

  if (isShowX()) {
    (void) createText("X", CPoint3D(x2, yp - ts1, zp), ts1);

    for (uint i = 0; i <= xinterval.calcNumMajor(); ++i) {
      auto x      = xinterval.interval(i);
      auto xlabel = QString("%1").arg(x);
      auto dx     = xlabel.length()*ts2/2.0;

      (void) createText(xlabel, CPoint3D(x - dx, yp, zp), ts2);
    }
  }


  if (isShowY()) {
    (void) createText("Y", CPoint3D(xp, y2, zp + ts1), ts1);

    for (uint i = 0; i <= yinterval.calcNumMajor(); ++i) {
      auto y      = yinterval.interval(i);
      auto ylabel = QString("%1").arg(y);
      auto dy     = ylabel.length()*ts2/2.0;

      (void) createText(ylabel, CPoint3D(xp, y - dy, zp), ts2);
    }
  }

  if (isShowZ()) {
    (void) createText("Z", CPoint3D(xp, yp - ts1, z2), ts1);

    for (uint i = 0; i <= zinterval.calcNumMajor(); ++i) {
      auto z      = zinterval.interval(i);
      auto zlabel = QString("%1").arg(z);
      auto dz     = zlabel.length()*ts2/2.0;

      (void) createText(zlabel, CPoint3D(xp, yp, z - dz), ts2);
    }
  }

  updateTexts();
}

void
CQCamera3DAxes::
drawGeometry()
{
  auto *program = shaderProgram();

  program->bind();

  //---

  auto *camera = canvas_->getCurrentCamera();

  // camera projection
  auto projectionMatrix = camera->worldMatrix();
  program->setUniformValue("projection", CQGLUtil::toQMatrix(projectionMatrix));

  // camera/view transformation
  auto viewMatrix = camera->viewMatrix();
  program->setUniformValue("view", CQGLUtil::toQMatrix(viewMatrix));

  // model matrix
  auto modelMatrix = CMatrix3DH::identity();
  program->setUniformValue("model", CQGLUtil::toQMatrix(modelMatrix));

  program->setUniformValue("viewPos", CQGLUtil::toVector(camera->position()));

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

  glDisable(GL_CULL_FACE);

  drawTexts();

  glEnable(GL_CULL_FACE);
}
