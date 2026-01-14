#include <CQCamera3DMaterials.h>
#include <CQCamera3DShaderProgram.h>
#include <CQCamera3DShapes.h>
#include <CQCamera3DCamera.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DUtil.h>

#include <CQGLBuffer.h>
#include <CQGLTexture.h>
#include <CQGLUtil.h>
#include <CGeomScene3D.h>

#include <QMouseEvent>

CQCamera3DMaterials::
CQCamera3DMaterials(CQCamera3DApp *app) :
 CQCamera3DWidget(app)
{
  setObjectName("materials");

  camera_ = new CQCamera3DCamera;

  camera_->setPosition(CVector3D(2.9, 3.1, 3));

  camera_->setPitch(-M_PI/5.0);
  camera_->setYaw(2.0*M_PI/3.0);

  camera_->setOrigin(CVector3D(0, 0, 0));

  camera_->moveAroundX(0.01);

  connect(app_, SIGNAL(currentMaterialChanged()), this, SLOT(invalidateSlot()));
  connect(app_, SIGNAL(materialChanged()), this, SLOT(invalidateSlot()));
}

void
CQCamera3DMaterials::
invalidateSlot()
{
  valid_ = false;

  update();
}

void
CQCamera3DMaterials::
initializeGL()
{
  initializeOpenGLFunctions();

  //---

  shaderProgram_ = new CQCamera3DShaderProgram(app_);

  shaderProgram_->addShaders("materials.vs", "materials.fs");
}

void
CQCamera3DMaterials::
resizeGL(int width, int height)
{
  setPixelWidth (width);
  setPixelHeight(height);

  glViewport(0, 0, width, height);

  setAspect(double(width)/double(height));

  camera_->setAspect(aspect());
}

void
CQCamera3DMaterials::
paintGL()
{
  // clear canvas
  glClearColor(bgColor_.redF(), bgColor_.greenF(), bgColor_.blueF(), 1.0f);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0, 0, pixelWidth(), pixelHeight());

  //---

  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glFrontFace(GL_CW);

  //---

  if (! valid_) {
    addGeometry();

    valid_ = true;
  }

  //---

  drawGeometry();
}

void
CQCamera3DMaterials::
addGeometry()
{
  auto *scene = app_->getScene();

  auto *material = scene->getMaterial(app_->currentMaterial().toStdString());

  //---

  auto *buffer = initBuffer();

  //---

  CQCamera3DShapes::ShapeData shapeData;

  shapeData.color = (material ? material->getDiffuse() : CRGBA::white());

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

  addCube(CPoint3D(0, 0, 0), 1.0);

  //---

  buffer->load();
}

void
CQCamera3DMaterials::
drawGeometry()
{
  auto *program = shaderProgram_;

  program->bind();

  //---

  // camera projection
  auto projectionMatrix = camera_->worldMatrix();
  program->setUniformValue("projection", CQGLUtil::toQMatrix(projectionMatrix));

  // camera/view transformation
  auto viewMatrix = camera_->viewMatrix();
  program->setUniformValue("view", CQGLUtil::toQMatrix(viewMatrix));

  // model matrix
  auto modelMatrix = CMatrix3DH::identity();
  program->setUniformValue("model", CQGLUtil::toQMatrix(modelMatrix));

  program->setUniformValue("viewPos", CQGLUtil::toVector(camera_->position()));

  //---

  auto *scene = app_->getScene();

  CGeomTexture *diffuseTexture = nullptr;
  CGeomTexture *normalTexture  = nullptr;

  auto *material = scene->getMaterial(app_->currentMaterial().toStdString());

  if (material) {
    diffuseTexture = material->diffuseTexture();
    normalTexture  = material->normalTexture();
  }

  auto setTextureVars = [&](const QString &name, CGeomTexture *texture, int ind) {
    auto name1 = (name + "Texture.enabled").toStdString();
    auto name2 = (name + "Texture.texture").toStdString();

    program->setUniformValue(name1.c_str(), !!texture);

    if (texture) {
      auto glTexture = getGLTexture(texture, /*add*/true);

      glActiveTexture(GL_TEXTURE0 + ind);

      glTexture->bind();

      program->setUniformValue(name2.c_str(), ind);
    }
  };

  setTextureVars("diffuse", diffuseTexture, 0);
  setTextureVars("normal" , normalTexture , 1);

  // render shapes
  buffer_->bind();

  for (const auto &faceData : faceDataList_.faceDatas) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
  }

  buffer_->unbind();

  //---

  program->release();
}

CQGLBuffer *
CQCamera3DMaterials::
initBuffer()
{
  if (! buffer_)
    buffer_ = shaderProgram_->createBuffer();

  buffer_->clearAll();

  return buffer_;
}

//---

void
CQCamera3DMaterials::
mousePressEvent(QMouseEvent *e)
{
  mouseData_.pressed = true;
  mouseData_.button  = e->button();

  mouseData_.press.x = e->x();
  mouseData_.press.y = e->y();
  mouseData_.button  = e->button();

  mouseData_.isShift   = (e->modifiers() & Qt::ShiftModifier);
  mouseData_.isControl = (e->modifiers() & Qt::ControlModifier);
}

void
CQCamera3DMaterials::
mouseMoveEvent(QMouseEvent *e)
{
  mouseData_.move.x = e->x();
  mouseData_.move.y = e->y();

  mouseData_.isShift   = (e->modifiers() & Qt::ShiftModifier);
  mouseData_.isControl = (e->modifiers() & Qt::ControlModifier);

  //---

  auto dx = CMathUtil::sign(mouseData_.move.x - mouseData_.press.x);
  auto dy = CMathUtil::sign(mouseData_.move.y - mouseData_.press.y);

  auto dx1 = 0.1*dx;
  auto dy1 = 0.1*dy;

  if      (mouseData_.button == Qt::MiddleButton) {
    if      (mouseData_.isShift) {
      camera_->movePosition(0, dx1);
      camera_->movePosition(1, dy1);
    }
    else if (mouseData_.isControl) {
      camera_->rotatePosition(2, dx*0.05);
    }
    else {
    //camera_->rotatePosition(1, dx*0.05);
      camera_->moveAroundY(-dx1);

    //camera_->rotatePosition(0, dy*0.05);
      camera_->moveAroundX(dy1);
    }
  }

  //---

  mouseData_.press.x = e->x();
  mouseData_.press.y = e->y();

  update();
}

void
CQCamera3DMaterials::
mouseReleaseEvent(QMouseEvent *)
{
  mouseData_.pressed = false;
}

void
CQCamera3DMaterials::
wheelEvent(QWheelEvent *)
{
}

void
CQCamera3DMaterials::
keyPressEvent(QKeyEvent *e)
{
  int key = e->key();

  if (key == Qt::Key_P)
    camera_->print();
}
