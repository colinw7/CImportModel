#include <CQCamera3DOverlay2D.h>
#include <CQCamera3DShaderProgram.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DCamera.h>

#include <CGeomObject3D.h>
#include <CGeomSphere3D.h>
#include <CGeomCylinder3D.h>
#include <CQGLBuffer.h>
#include <CQGLUtil.h>

CQCamera3DOverlay2D::
CQCamera3DOverlay2D(CQCamera3DCanvas *canvas) :
 CQCamera3DObject(canvas)
{
}

CQCamera3DShaderProgram *
CQCamera3DOverlay2D::
shaderProgram()
{
  return getShader("overlay2d.vs", "overlay2d.fs");
}

void
CQCamera3DOverlay2D::
updateGeometry()
{
  auto *buffer = initBuffer();

  //---

  auto addPoint = [&](const CPoint3D &p, const CRGBA &c) {
    buffer->addPoint(p.x, p.y, p.z);
    buffer->addColor(c);
  };

  auto addRect = [&](const CPoint3D &p1, const CPoint3D &p2,
                     const CPoint3D &p3, const CPoint3D &p4,
                     const CRGBA &c) {
    CQCamera3DFaceData faceData;

    faceData.pos = faceDataList_.pos;
    faceData.len = 4;

    addPoint(p1, c);
    addPoint(p2, c);
    addPoint(p3, c);
    addPoint(p4, c);

    faceDataList_.faceDatas.push_back(faceData);

    faceDataList_.pos += faceData.len;
  };

  //---

  auto s =  0.10;
  auto x = -0.95;
  auto y =  0.85;

  addRect(CPoint3D(x    , y    , 0), CPoint3D(x + s, y    , 0),
          CPoint3D(x + s, y + s, 0), CPoint3D(x    , y + s, 0),
          CRGBA(1, 0, 0));

  //---

  buffer->load();
}

void
CQCamera3DOverlay2D::
drawGeometry()
{
  glDisable(GL_DEPTH_TEST);

  //---

  auto *program = shaderProgram();

  program->bind();

  //---

  auto *camera = canvas_->getCurrentCamera();

  // camera projection
  auto projectionMatrix = camera->perspectiveMatrix();
  program->setUniformValue("projection", CQGLUtil::toQMatrix(projectionMatrix));

  // model matrix
  auto modelMatrix = CMatrix3DH::identity();
  program->setUniformValue("model", CQGLUtil::toQMatrix(modelMatrix));

  //---

  // render shapes
  buffer_->bind();

  for (const auto &faceData : faceDataList_.faceDatas) {
    if (isWireframe()) {
      program->setUniformValue("isWireframe", true);

      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

      glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
    }

    program->setUniformValue("isWireframe", false);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
  }

  buffer_->unbind();

  //---

  program->release();
}
