#include <CQNewGLFractalWidget.h>
#include <CQNewGLFractal.h>
#include <CQNewGLModel.h>
#include <CQNewGLCamera.h>
#include <CQNewGLAxes.h>
#include <CQNewGLUtil.h>

#include <CQGLBuffer.h>
#include <CQGLUtil.h>

#include <QMouseEvent>

//---

CQNewGLFractalWidget::
CQNewGLFractalWidget(CQNewGLModel *app) :
 CQNewGLWidget(app)
{
}

void
CQNewGLFractalWidget::
initializeGL()
{
  initialized_ = true;

  //---

  initializeOpenGLFunctions();

  glClearColor(bgColor_.redF(), bgColor_.greenF(), bgColor_.blueF(), 1.0f);

  //---

  initFont();

  //---

  axes_ = new CQNewGLAxes(this);
  axes_->setVisible(false);

  //---

  fractal_ = new CQNewGLFractal(this);

  //---

  // cameras
  addCamera();
}

void
CQNewGLFractalWidget::
resizeGL(int width, int height)
{
  setPixelWidth (width);
  setPixelHeight(height);

  glViewport(0, 0, width, height);

  setAspect(double(width)/double(height));
}

void
CQNewGLFractalWidget::
paintGL()
{
  // clear canvas
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0, 0, pixelWidth(), pixelHeight());

  //---

  // set GL state
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glFrontFace(GL_CW);

  //---

  // set camera
  auto *camera = getCurrentCamera();

  camera->setAspect(aspect());

  //---

  // set view state
  paintData_.projection = camera->getPerspectiveMatrix();
  paintData_.view       = camera->getViewMatrix();
  paintData_.viewPos    = camera->position();

  //---

  drawFractal();

  //---

  if (axes_->isVisible())
    drawAxes();
}

//---

void
CQNewGLFractalWidget::
addCamera()
{
  CGLVector3D position, up;
  QColor      color;

  CameraData cameraData;
  calcCameraData(0, cameraData);

  //---

  camera_ = new CQNewGLCamera(this, 0, cameraData.origin, cameraData.position, cameraData.up);

  camera_->setColor(cameraData.color);

  camera_->setLastPos(float(pixelWidth())/2.0f, float(pixelHeight())/2.0f);
}

void
CQNewGLFractalWidget::
resetCamera()
{
  sceneScale_ = std::max(sceneSize_.maxComponent(), 1.0);

  //---

  auto f = std::sqrt(2.0);

  CGLVector3D position, origin;

  auto f1 = f*sceneScale_;

  position = CGLVector3D(f1, f1, f1);
  origin   = CGLVector3D(sceneCenter_.x, sceneCenter_.y, sceneCenter_.z);

  auto *camera = getCurrentCamera();

  camera->setOrigin(origin);
  camera->setPosition(position);
}

//---

void
CQNewGLFractalWidget::
updateFractal()
{
  if (! fractal_)
    return;

  fractal_->addGeometry();

  auto bbox = fractal_->bbox();

  sceneSize_   = bbox.getSize();
  sceneCenter_ = bbox.getCenter();

  resetCamera();

  axes_->setValid(false);

  update();
}

//---

void
CQNewGLFractalWidget::
drawFractal()
{
  if (! fractal_)
    return;

  fractal_->drawGeometry();
}

//---

void
CQNewGLFractalWidget::
updateAxes()
{
  axes_->updateGeometry();

  axes_->setValid(true);
}

void
CQNewGLFractalWidget::
drawAxes()
{
  if (! axes_->isValid())
    updateAxes();

  axes_->drawGeometry();
}

//---

void
CQNewGLFractalWidget::
mousePressEvent(QMouseEvent *e)
{
  mouseData_.pressed = true;
  mouseData_.button  = e->button();

  mouseData_.press.x = e->x();
  mouseData_.press.y = e->y();

  mouseData_.isShift   = (e->modifiers() & Qt::ShiftModifier);
  mouseData_.isControl = (e->modifiers() & Qt::ControlModifier);

  //---

  bool isLeftButton = (mouseData_.button == Qt::LeftButton);

  if (isLeftButton) {
    auto type = app_->type();

    if (type == CQNewGLModel::Type::CAMERA) {
      auto *camera = getCurrentCamera();

      camera->setLastPos(mouseData_.press.x, mouseData_.press.y);
    }
  }

  //---

  update();
}

void
CQNewGLFractalWidget::
mouseMoveEvent(QMouseEvent *e)
{
  mouseData_.move.x = e->x();
  mouseData_.move.y = e->y();

  mouseData_.isShift   = (e->modifiers() & Qt::ShiftModifier);
  mouseData_.isControl = (e->modifiers() & Qt::ControlModifier);

  //---

  bool isLeftButton = (mouseData_.button == Qt::LeftButton);

  if (isLeftButton) {
    auto type = app_->type();

    if (type == CQNewGLModel::Type::CAMERA) {
      auto *camera = getCurrentCamera();

      float xoffset, yoffset;
      camera->deltaPos(mouseData_.move.x, mouseData_.move.y, xoffset, yoffset);

      camera->setLastPos(mouseData_.move.x, mouseData_.move.y);

      camera->processMouseMovement(xoffset, yoffset, /*constrainPitch*/true);
      updateCameraBuffer();
    }
  }

  //---

  mouseData_.press = mouseData_.move;

  update();
}

void
CQNewGLFractalWidget::
mouseReleaseEvent(QMouseEvent *)
{
  mouseData_.pressed = false;
  mouseData_.button  = Qt::NoButton;

  update();
}

void
CQNewGLFractalWidget::
wheelEvent(QWheelEvent *e)
{
  auto dw = e->angleDelta().y()/250.0;

  auto type = app_->type();

  if (type == CQNewGLModel::Type::CAMERA) {
    auto *camera = getCurrentCamera();

    camera->processMouseScroll(dw);
    updateCameraBuffer();
  }

  update();
}

void
CQNewGLFractalWidget::
keyPressEvent(QKeyEvent *e)
{
  CGLCamera::ProcessKeyData processKeyData;

  bool isShift   = (e->modifiers() & Qt::ShiftModifier);
  bool isControl = (e->modifiers() & Qt::ControlModifier);

  auto *camera = getCurrentCamera();

  auto dt = 0.1 /* camera->deltaTime() */;
//auto da = M_PI/180.0;

#if 0
  if (isShift) {
    dt = -dt;
    da = -da;
  }
#endif

  processKeyData.deltaTime = dt;
  processKeyData.rotate    = false;
  processKeyData.strafe    = isShift;

  auto type = app_->type();

  if      (e->key() == Qt::Key_Q) {
    if      (type == CQNewGLModel::Type::CAMERA) {
      processKeyData.rotateAt =
        (isControl ? CGLCamera::RotateAt::POSITION : CGLCamera::RotateAt::ORIGIN);
      camera->processKeyboard(CGLCamera::Movement::ROTATE_LEFT, processKeyData);
      updateCameraBuffer();
    }
  }
  else if (e->key() == Qt::Key_E) {
    if      (type == CQNewGLModel::Type::CAMERA) {
      processKeyData.rotateAt =
        (isControl ? CGLCamera::RotateAt::POSITION : CGLCamera::RotateAt::ORIGIN);
      camera->processKeyboard(CGLCamera::Movement::ROTATE_RIGHT, processKeyData);
      updateCameraBuffer();
    }
  }
  else if (e->key() == Qt::Key_W) {
    if      (type == CQNewGLModel::Type::CAMERA) {
      camera->processKeyboard(CGLCamera::Movement::FORWARD, processKeyData);
      updateCameraBuffer();
    }
  }
  else if (e->key() == Qt::Key_S) {
    if      (type == CQNewGLModel::Type::CAMERA) {
      camera->processKeyboard(CGLCamera::Movement::BACKWARD, processKeyData);
      updateCameraBuffer();
    }
  }
  else if (e->key() == Qt::Key_A) {
    if      (type == CQNewGLModel::Type::CAMERA) {
      camera->processKeyboard(CGLCamera::Movement::STRAFE_LEFT, processKeyData);
      updateCameraBuffer();
    }
  }
  else if (e->key() == Qt::Key_D) {
    if      (type == CQNewGLModel::Type::CAMERA) {
      camera->processKeyboard(CGLCamera::Movement::STRAFE_RIGHT, processKeyData);
      updateCameraBuffer();
    }
  }
  else if (e->key() == Qt::Key_Up) {
    if      (type == CQNewGLModel::Type::CAMERA) {
      if (! isControl)
        camera->processKeyboard(CGLCamera::Movement::UP, processKeyData);
      else
        camera->processKeyboard(CGLCamera::Movement::FORWARD, processKeyData);

      updateCameraBuffer();
    }
  }
  else if (e->key() == Qt::Key_Down) {
    if      (type == CQNewGLModel::Type::CAMERA) {
      if (! isControl)
        camera->processKeyboard(CGLCamera::Movement::DOWN, processKeyData);
      else
        camera->processKeyboard(CGLCamera::Movement::BACKWARD, processKeyData);

      updateCameraBuffer();
    }
  }
  else if (e->key() == Qt::Key_Left) {
    if      (type == CQNewGLModel::Type::CAMERA) {
    }
  }
  else if (e->key() == Qt::Key_Right) {
    if      (type == CQNewGLModel::Type::CAMERA) {
    }
  }

  update();
}

void
CQNewGLFractalWidget::
updateCameraBuffer()
{
  auto *camera = getCurrentCamera();

  camera->addGeometry();
}
