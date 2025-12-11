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
  setWindowWidth (width);
  setWindowHeight(height);

  //---

  glViewport(0, 0, width, height);
}

void
CQNewGLFractalWidget::
paintGL()
{
  // clear canvas
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //---

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glFrontFace(GL_CW);

  //---

  pixelWidth_  = width ();
  pixelHeight_ = height();

  // set view
  auto *camera = getCurrentCamera();

  aspect_ = float(pixelWidth_)/float(pixelHeight_);

  camera->setAspect(aspect_);

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

  camera_->setLastPos(float(windowWidth())/2.0f, float(windowHeight())/2.0f);
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
  mouseData_.pressX = e->x();
  mouseData_.pressY = e->y();
  mouseData_.button = e->button();

  bool isLeftButton = (mouseData_.button == Qt::LeftButton);

  //---

  if (isLeftButton) {
    auto type = app_->type();

    if (type == CQNewGLModel::Type::CAMERA) {
      auto *camera = getCurrentCamera();

      camera->setLastPos(mouseData_.pressX, mouseData_.pressY);
    }
  }

  //---

  update();
}

void
CQNewGLFractalWidget::
mouseReleaseEvent(QMouseEvent *)
{
  mouseData_.button = Qt::NoButton;

  update();
}

void
CQNewGLFractalWidget::
mouseMoveEvent(QMouseEvent *e)
{
  mouseData_.moveX = e->x();
  mouseData_.moveY = e->y();

  bool isLeftButton = (mouseData_.button == Qt::LeftButton);

  //---

  if (isLeftButton) {
    auto type = app_->type();

    if (type == CQNewGLModel::Type::CAMERA) {
      auto *camera = getCurrentCamera();

      float xoffset, yoffset;
      camera->deltaPos(mouseData_.moveX, mouseData_.moveY, xoffset, yoffset);

      camera->setLastPos(mouseData_.moveX, mouseData_.moveY);

      camera->processMouseMovement(xoffset, yoffset, /*constrainPitch*/true);
      updateCameraBuffer();
    }
  }

  //---

  mouseData_.pressX = mouseData_.moveX;
  mouseData_.pressY = mouseData_.moveY;

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
