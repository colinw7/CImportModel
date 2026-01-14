#include <CQNewGLPath.h>
#include <CQNewGLShaderProgram.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLModel.h>

#include <CQGLBuffer.h>
#include <CQGLUtil.h>
#include <CBBox3D.h>

CQNewGLPath::
CQNewGLPath(CQNewGLCanvas *canvas) :
 CQNewGLObject(canvas), canvas_(canvas)
{
}

CQNewGLShaderProgram *
CQNewGLPath::
shaderProgram()
{
  return getShader("path.vs", "path.fs");
}

void
CQNewGLPath::
updateGeometry()
{
  initBuffer();

  //---

  auto color = CRGBA(0.8, 0.8, 0.8);

  int pos = 0;

  auto addPoint = [&](const CPoint3D &p, const CRGBA &c) {
    buffer_->addPoint(p.getX(), p.getY(), p.getZ());
    buffer_->addColor(c);
  };

  auto addLine = [&](const CPoint3D &p1, const CPoint3D &p2) {
    CQNewGLFaceData faceData;

    faceData.pos = pos;
    faceData.len = 2;

    addPoint(p1, color);
    addPoint(p2, color);

    faceDataList_.faceDatas.push_back(faceData);

    pos += faceData.len;
  };

  int ind = 0;

  CPoint3D p1;

  for (const auto &element : path_.elements()) {
    auto p2 = element.pos;

    if (ind > 0)
      addLine(p1, p2);

    p1 = p2;

    ++ind;
  }

  buffer_->load();
}

void
CQNewGLPath::
drawGeometry()
{
  if (! buffer_)
    return;

  auto *program = shaderProgram();

  buffer_->bind();

  program->bind();

  //---

  auto modelMatrix = CMatrix3D::identity();
  canvas_->addShaderMVP(program, modelMatrix);

  //---

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glPointSize(8.0);
  glLineWidth(4.0);

  for (const auto &faceData : faceDataList_.faceDatas) {
    glDrawArrays(GL_LINES, faceData.pos, faceData.len);
  }

  for (const auto &faceData : faceDataList_.faceDatas) {
    glDrawArrays(GL_POINTS, faceData.pos, faceData.len);
  }

  buffer_->unbind();

  //---

  program->release();
}
