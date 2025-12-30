#include <CQCamera3DAnnotation.h>
#include <CQCamera3DShaderProgram.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DCamera.h>
#include <CQCamera3DShapes.h>

#include <CGeomObject3D.h>
#include <CGeomSphere3D.h>
#include <CGeomCylinder3D.h>
#include <CQGLBuffer.h>
#include <CQGLUtil.h>

CQCamera3DAnnotation::
CQCamera3DAnnotation(CQCamera3DCanvas *canvas) :
 CQCamera3DObject(canvas)
{
}

CQCamera3DShaderProgram *
CQCamera3DAnnotation::
shaderProgram()
{
  return getShader("annotation.vs", "annotation.fs");
}

void
CQCamera3DAnnotation::
updateGeometry()
{
  auto *buffer = initBuffer();

  //---

  CQCamera3DShapes::ShapeData shapeData;

  auto addCube = [&](const CPoint3D &c, double r) {
    auto p1 = c - CPoint3D(r, r, r);
    auto p2 = c + CPoint3D(r, r, r);

    CQCamera3DFaceDataList faceDataList;
    faceDataList.pos = faceDataList_.pos;
    CBBox3D bbox(p1, p2);
    CQCamera3DShapes::addCube(buffer, bbox, shapeData, faceDataList);
    faceDataList_.add(faceDataList);
    faceDataList_.pos += faceDataList.pos;
  };

  //---

  CPoint3D c(0, 0, 0);
  double   r { 0.1 };

  addCube(c, r);

  //---

  buffer->load();
}

void
CQCamera3DAnnotation::
drawGeometry()
{
  glDisable(GL_CULL_FACE);

  //---

  auto *program = shaderProgram();

  program->bind();

  //---

  auto *camera = canvas_->getCurrentCamera();

  // camera projection
  auto projectionMatrix = camera->perspectiveMatrix();
  program->setUniformValue("projection", CQGLUtil::toQMatrix(projectionMatrix));

  // camera/view transformation
  auto viewMatrix = camera->viewMatrix();
  program->setUniformValue("view", CQGLUtil::toQMatrix(viewMatrix));

  // model matrix
  auto modelMatrix = CMatrix3DH::identity();
  program->setUniformValue("model", CQGLUtil::toQMatrix(modelMatrix));

  program->setUniformValue("viewPos", CQGLUtil::toVector(camera->position()));

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
