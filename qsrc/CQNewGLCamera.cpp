#include <CQNewGLCamera.h>
#include <CQNewGLShaderProgram.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLModel.h>
#include <CQGLBuffer.h>
#include <CQGLUtil.h>

CQNewGLShaderProgram *CQNewGLCamera::shaderProgram_;

void
CQNewGLCamera::
initShader(CQNewGLCanvas *canvas)
{
  shaderProgram_ = new CQNewGLShaderProgram(canvas);
  shaderProgram_->addShaders("camera.vs", "camera.fs");
}

CQNewGLCamera::
CQNewGLCamera(CQNewGLCanvas *canvas, const CGLVector3D &origin, const CGLVector3D &position,
              const CGLVector3D &up) :
 CQNewGLObject(canvas), CGLCamera(origin, position, up)
{
  initOrigin_   = origin;
  initPosition_ = position;
  initUp_       = up;
}

void
CQNewGLCamera::
viewChanged()
{
  canvas_->app()->updateCamera();
}

void
CQNewGLCamera::
reset()
{
  // position, up, yaw, pitch
  //init(position(), up(), yaw(), pitch());

  // origin, position, up
  //init(origin(), position(), up());
  init(initOrigin_, initPosition_, initUp_);
}

void
CQNewGLCamera::
addGeometry()
{
  auto pointSize = canvas_->sceneScale()/100.0;

  // set up vertex data (and buffer(s)) and configure vertex attributes
  initBuffer();

  //---

  const auto &color1 = this->color();
  auto        color2 = QColor(255, 255, 255);

  //---

  int pos = 0;

  auto addPoint = [&](const CPoint3D &p, const CPoint3D &n, const QColor &c) {
    buffer_->addPoint(p.x, p.y, p.z);
    buffer_->addNormal(n.x, n.y, n.z);
    buffer_->addColor(c);
  };

  auto addLine = [&](const CPoint3D &c1, const CPoint3D &c2, const CPoint3D &dp) {
    CQNewGLFaceData faceData;

    faceData.pos = pos;
    faceData.len = 4;

    auto p1 = c1 - dp;
    auto p2 = c1 + dp;
    auto p3 = c2 + dp;
    auto p4 = c2 - dp;
    auto n  = CPoint3D(0, 0, 1);

    // eye
    addPoint(p1, n, color1);
    addPoint(p2, n, color1);

    // origin
    addPoint(p3, n, color2);
    addPoint(p4, n, color2);

    addFaceData(faceData);

    pos += faceData.len;
  };

  //---

  const auto &o = this->origin();
  const auto &p = this->position();

  auto dir = p - o;
  auto l   = dir.length()/2.0;
  auto p1  = o + l*dir.normalized();

  auto calcUp = [&]() {
    auto dx = std::abs(p1.x() - o.x());
    auto dy = std::abs(p1.y() - o.y());
    auto dz = std::abs(p1.z() - o.z());

    if      (dx < dy && dx < dz)
      return CPoint3D(pointSize, 0, 0);
    else if (dy < dz)
      return CPoint3D(0, pointSize, 0);
    else
      return CPoint3D(0, 0, pointSize);
  };

  addLine(CPoint3D(p1.x(), p1.y(), p1.z()), CPoint3D(o.x(), o.y(), o.z()), calcUp());

  buffer_->load();
}

void
CQNewGLCamera::
drawGeometry()
{
  auto *program = shaderProgram();

  // setup camera shader
  buffer_->bind();

  auto modelMatrix = CMatrix3D::identity();
//auto modelMatrix = getModelMatrix();
  canvas_->addShaderMVP(program, modelMatrix);

  //---

  for (const auto &faceData : faceDatas()) {
    // draw camera
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDrawArrays(GL_TRIANGLE_STRIP, faceData.pos, faceData.len);
  }

  buffer_->unbind();
}
