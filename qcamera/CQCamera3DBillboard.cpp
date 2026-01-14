#include <CQCamera3DBillboard.h>
#include <CQCamera3DShaderProgram.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DCamera.h>

#include <CQGLBuffer.h>
#include <CQGLUtil.h>

CQCamera3DBillboard::
CQCamera3DBillboard(CQCamera3DCanvas *canvas) :
 CQCamera3DObject(canvas)
{
}

CQCamera3DShaderProgram *
CQCamera3DBillboard::
shaderProgram()
{
  return getShader("billboard.vs", "billboard.fs");
}

void
CQCamera3DBillboard::
updateGeometry()
{
  buffer_ = initBuffer();

  //---

  addCursor();

  //---

  buffer_->load();
}

void
CQCamera3DBillboard::
addCursor()
{
  auto bbox = canvas_->bbox();

  int n = 32;

  auto da = 2*M_PI/n;

  auto a = 0.0;
  auto r = bbox.getMaxSize()/100.0;

  for (int i = 0; i < n; ++i) {
    a += da;

    auto c = std::cos(a);
    auto s = std::sin(a);

    auto p = CPoint3D(r*c, r*s, 0.0);

    buffer_->addPoint(p.x, p.y, p.z);
    buffer_->addNormal(0, 0, 1);
    buffer_->addColor(i & 1 ? CRGBA::white() : CRGBA::red());
  }

  CQCamera3DFaceData faceData;

  faceData.wireframe = true;
  faceData.lines     = true;

  faceData.pos = faceDataList_.pos;
  faceData.len = n;

  faceDataList_.faceDatas.push_back(faceData);

  faceDataList_.pos += faceData.len;
}

void
CQCamera3DBillboard::
drawGeometry()
{
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);

  //---

  auto *program = shaderProgram();

  program->bind();

  //---

  auto p = canvas_->cursor();

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

  program->setUniformValue("center", CQGLUtil::toVector(p));

  program->setUniformValue("cameraUp", CQGLUtil::toVector(camera->up()));
  program->setUniformValue("cameraRight", CQGLUtil::toVector(camera->right()));

  //---

  // render shapes
  buffer_->bind();

  for (const auto &faceData : faceDataList_.faceDatas) {
    if (! faceData.lines) {
      if (isWireframe()) {
        program->setUniformValue("isWireframe", true);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
      }

      program->setUniformValue("isWireframe", false);

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

      glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
    }
    else {
      program->setUniformValue("isWireframe", false);

      glDrawArrays(GL_LINES, faceData.pos, faceData.len);
    }
  }

  buffer_->unbind();

  //---

  program->release();
}
