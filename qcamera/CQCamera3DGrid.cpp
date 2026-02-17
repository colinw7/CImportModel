#include <CQCamera3DGrid.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DShaderProgram.h>
#include <CQCamera3DCamera.h>
#include <CQCamera3DFont.h>
#include <CQCamera3DShapes.h>

#include <CQGLBuffer.h>
#include <CQGLUtil.h>
#include <CInterval.h>

CQCamera3DGrid::
CQCamera3DGrid(CQCamera3DCanvas *canvas) :
 CQCamera3DObject(canvas)
{
}

CQCamera3DShaderProgram *
CQCamera3DGrid::
shaderProgram()
{
  return getShader("grid.vs", "grid.fs");
}

void
CQCamera3DGrid::
updateGeometry()
{
  auto *buffer = initBuffer();

  buffer->addPoint(-1, -1, 0);
  buffer->addPoint( 1, -1, 0);
  buffer->addPoint( 1,  1, 0);

  buffer->addPoint(-1, -1, 0);
  buffer->addPoint( 1,  1, 0);
  buffer->addPoint(-1,  1, 0);

  buffer_->load();
}

void
CQCamera3DGrid::
drawGeometry()
{
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  auto *program = shaderProgram();

  program->bind();

  //---

  auto bbox = canvas_->bbox();
  auto size = bbox.getMaxSize();

  program->setUniformValue("gridSize", float(3*size));

  //---

  auto *camera = canvas_->getCurrentCamera();

  // camera projection
  auto projectionMatrix = camera->worldMatrix();
  program->setUniformValue("projection", CQGLUtil::toQMatrix(projectionMatrix));

  // camera/view transformation
  auto viewMatrix = camera->viewMatrix();
  program->setUniformValue("view", CQGLUtil::toQMatrix(viewMatrix));

  // camera pos
  program->setUniformValue("viewPos", CQGLUtil::toVector(camera->position()));

  //---

  // render grid
  buffer_->bind();

  //glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 6, 1, 0);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  buffer_->unbind();

  //---

  program->release();

  //---

  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
}
