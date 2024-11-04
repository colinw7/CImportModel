#include <CQNewGLCanvas.h>
#include <CQNewGLModel.h>

#include <CImportBase.h>
#include <CQGLBuffer.h>
#include <CGLTexture.h>
#include <CGeomScene3D.h>
#include <CGeomTexture.h>
#include <CQGLUtil.h>
#include <CFile.h>

#include <CGLCamera.h>

#include <QOpenGLShaderProgram>
#include <QMouseEvent>

#define Q(x) #x
#define QUOTE(x) Q(x)

//---

CQNewGLCanvas::
CQNewGLCanvas(CQNewGLModel *app) :
 QGLWidget(), app_(app)
{
  setFocusPolicy(Qt::StrongFocus);

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void
CQNewGLCanvas::
setBgColor(const QColor &c)
{
  bgColor_ = c;

  glClearColor(bgColor_.redF(), bgColor_.greenF(), bgColor_.blueF(), 1.0f);
}

void
CQNewGLCanvas::
setDepthTest(bool b)
{
  depthTest_ = b;

  if (b)
    glEnable(GL_DEPTH_TEST);
  else
    glDisable(GL_DEPTH_TEST);
}

void
CQNewGLCanvas::
setCullFace(bool b)
{
  cullFace_ = b;

  if (b)
    glEnable(GL_CULL_FACE);
  else
    glDisable(GL_CULL_FACE);
}

void
CQNewGLCanvas::
setFrontFace(bool b)
{
  frontFace_ = b;

  if (b)
    glFrontFace(GL_CW);
  else
    glFrontFace(GL_CCW);
}

void
CQNewGLCanvas::
setPolygonLine(bool b)
{
  polygonLine_ = b;

  if (b)
    glEnable(GL_POLYGON_OFFSET_LINE);
  else
    glDisable(GL_POLYGON_OFFSET_LINE);

  glPolygonOffset(-1.0f, -1.0f);
}

bool
CQNewGLCanvas::
loadModel(const QString &modelName, CGeom3DType format)
{
  // import model
  import_ = CImportBase::createModel(format);

  if (! import_) {
    std::cerr << "Invalid format\n";
    return false;
  }

  CFile file(modelName.toStdString());

  if (! import_->read(file)) {
    std::cerr << "Failed to read model '" << modelName.toStdString() << "'\n";
    return false;
  }

  return true;
}

void
CQNewGLCanvas::
initializeGL()
{
  glClearColor(bgColor_.redF(), bgColor_.greenF(), bgColor_.blueF(), 1.0f);

  QString buildDir = QUOTE(BUILD_DIR);

  //---

  setDepthTest  (depthTest_);
  setCullFace   (cullFace_);
  setFrontFace  (frontFace_);
  setPolygonLine(polygonLine_);

  //---

  modelShaderProgram_ = new QOpenGLShaderProgram;

  if (! modelShaderProgram_->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                                     buildDir + "/shaders/model.vs"))
    std::cerr << modelShaderProgram_->log().toStdString() << "\n";

  if (! modelShaderProgram_->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                                     buildDir + "/shaders/model.fs"))
    std::cerr << modelShaderProgram_->log().toStdString() << "\n";

  modelShaderProgram_->link();

  //---

  lightShaderProgram_ = new QOpenGLShaderProgram;

  if (! lightShaderProgram_->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                                     buildDir + "/shaders/light.vs"))
    std::cerr << lightShaderProgram_->log().toStdString() << "\n";

  if (! lightShaderProgram_->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                                     buildDir + "/shaders/light.fs"))
    std::cerr << lightShaderProgram_->log().toStdString() << "\n";

  lightShaderProgram_->link();

  //---

  // camera
  camera_ = new CGLCamera(CGLVector3D(0.0f, 0.0f, 1.0f));

  camera_->setLastPos(float(app_->windowWidth())/2.0f, float(app_->windowHeight())/2.0f);

  //---

  updateObjectData();

  //---

  // set up vertex data (and buffer(s)) and configure vertex attributes
  lightBuffer_ = new CQGLBuffer(lightShaderProgram_);

  lightBuffer_->addPoint(-0.5f, -0.5f, -0.5f);
  lightBuffer_->addPoint( 0.5f, -0.5f, -0.5f);
  lightBuffer_->addPoint( 0.5f,  0.5f, -0.5f);

  lightBuffer_->addPoint( 0.5f,  0.5f, -0.5f);
  lightBuffer_->addPoint(-0.5f,  0.5f, -0.5f);
  lightBuffer_->addPoint(-0.5f, -0.5f, -0.5f);

  lightBuffer_->addPoint(-0.5f, -0.5f,  0.5f);
  lightBuffer_->addPoint( 0.5f, -0.5f,  0.5f);
  lightBuffer_->addPoint( 0.5f,  0.5f,  0.5f);

  lightBuffer_->addPoint( 0.5f,  0.5f,  0.5f);
  lightBuffer_->addPoint(-0.5f,  0.5f,  0.5f);
  lightBuffer_->addPoint(-0.5f, -0.5f,  0.5f);

  lightBuffer_->addPoint(-0.5f,  0.5f,  0.5f);
  lightBuffer_->addPoint(-0.5f,  0.5f, -0.5f);
  lightBuffer_->addPoint(-0.5f, -0.5f, -0.5f);

  lightBuffer_->addPoint(-0.5f, -0.5f, -0.5f);
  lightBuffer_->addPoint(-0.5f, -0.5f,  0.5f);
  lightBuffer_->addPoint(-0.5f,  0.5f,  0.5f);

  lightBuffer_->addPoint( 0.5f,  0.5f,  0.5f);
  lightBuffer_->addPoint( 0.5f,  0.5f, -0.5f);
  lightBuffer_->addPoint( 0.5f, -0.5f, -0.5f);

  lightBuffer_->addPoint( 0.5f, -0.5f, -0.5f);
  lightBuffer_->addPoint( 0.5f, -0.5f,  0.5f);
  lightBuffer_->addPoint( 0.5f,  0.5f,  0.5f);

  lightBuffer_->addPoint(-0.5f, -0.5f, -0.5f);
  lightBuffer_->addPoint( 0.5f, -0.5f, -0.5f);
  lightBuffer_->addPoint( 0.5f, -0.5f,  0.5f);

  lightBuffer_->addPoint( 0.5f, -0.5f,  0.5f);
  lightBuffer_->addPoint(-0.5f, -0.5f,  0.5f);
  lightBuffer_->addPoint(-0.5f, -0.5f, -0.5f);

  lightBuffer_->addPoint(-0.5f,  0.5f, -0.5f);
  lightBuffer_->addPoint( 0.5f,  0.5f, -0.5f);
  lightBuffer_->addPoint( 0.5f,  0.5f,  0.5f);

  lightBuffer_->addPoint( 0.5f,  0.5f,  0.5f);
  lightBuffer_->addPoint(-0.5f,  0.5f,  0.5f);
  lightBuffer_->addPoint(-0.5f,  0.5f, -0.5f);

  lightBuffer_->load();
}

void
CQNewGLCanvas::
updateObjectData()
{
  // set up vertex data (and buffer(s)) and configure vertex attributes
  CVector3D sceneSize(1, 1, 1);

  if (import_) {
    auto &scene = import_->getScene();

    CBBox3D bbox;

    scene.getBBox(bbox);

    sceneSize    = bbox.getSize();
    sceneCenter_ = bbox.getCenter();

    std::cerr << "Scene Center : " << sceneCenter_.getX() << " " <<
                 sceneCenter_.getY() << " " << sceneCenter_.getZ() << "\n";

    for (auto *object : scene.getObjects()) {
      ObjectData *objectData { nullptr };

      auto pd = objectDatas_.find(object);

      if (pd == objectDatas_.end())
        pd = objectDatas_.insert(pd, ObjectDatas::value_type(object, new ObjectData));

      objectData = (*pd).second;

      if (! objectData->buffer)
        objectData->buffer = new CQGLBuffer(modelShaderProgram_);

      //---

      auto *buffer = objectData->buffer;

      buffer->clearPoints();
      buffer->clearNormals();
      buffer->clearColors();
      buffer->clearTexturePoints();

      objectData->faceDatas.clear();

      //---

      auto *diffuseTexture  = object->getDiffuseTexture();
      auto *specularTexture = object->getSpecularTexture();
      auto *normalTexture   = object->getNormalTexture();

      //---

      const auto &faces = object->getFaces();

      int pos = 0;

      for (const auto *face : faces) {
        FaceData faceData;

        //---

        const auto &color = face->getColor();

        //---

        auto *diffuseTexture1 = face->getDiffuseTexture();

        if (! diffuseTexture1)
          diffuseTexture1 = diffuseTexture;

        auto *specularTexture1 = face->getSpecularTexture();

        if (! specularTexture1)
          specularTexture1 = specularTexture;

        auto *normalTexture1 = face->getNormalTexture();

        if (! normalTexture1)
          normalTexture1 = normalTexture;

        //---

        if (diffuseTexture1) {
          auto pt = glTextures_.find(diffuseTexture1->id());

          if (pt == glTextures_.end()) {
            const auto &image = diffuseTexture1->image()->image();

            auto *glTexture = new CGLTexture(image);

            pt = glTextures_.insert(pt, GLTextures::value_type(diffuseTexture1->id(), glTexture));
          }

          faceData.diffuseTexture = (*pt).second;
        }

        if (specularTexture1) {
          auto pt = glTextures_.find(specularTexture->id());

          if (pt == glTextures_.end()) {
            const auto &image = specularTexture->image()->image();

            auto *glTexture = new CGLTexture(image);

            pt = glTextures_.insert(pt, GLTextures::value_type(specularTexture->id(), glTexture));
          }

          faceData.specularTexture = (*pt).second;
        }

        if (normalTexture1) {
          auto pt = glTextures_.find(normalTexture->id());

          if (pt == glTextures_.end()) {
            const auto &image = normalTexture->image()->image();

            auto *glTexture = new CGLTexture(image);

            pt = glTextures_.insert(pt, GLTextures::value_type(normalTexture->id(), glTexture));
          }

          faceData.normalTexture = (*pt).second;
        }

      //const auto &ambient   = face->getMaterial().getAmbient  ();
      //const auto &diffuse   = face->getMaterial().getDiffuse  ();
      //const auto &specular  = face->getMaterial().getSpecular ();
      //double      shininess = face->getMaterial().getShininess();

        CVector3D normal;

        if (face->getNormalSet())
          normal = face->getNormal();
        else
          face->calcNormal(normal);

        const auto &vertices = face->getVertices();

        faceData.pos = pos;
        faceData.len = int(vertices.size());

        for (const auto &v : vertices) {
          auto &vertex = object->getVertex(v);

          const auto &model = vertex.getModel();

          if (! isFlipYZ()) {
            buffer->addPoint(float(model.x), float(model.y), float(model.z));
            buffer->addNormal(float(normal.getX()), float(normal.getY()), float(normal.getZ()));
          }
          else {
            buffer->addPoint(float(model.x), float(model.z), float(model.y));
            buffer->addNormal(float(normal.getX()), float(normal.getZ()), float(normal.getY()));
          }

          buffer->addColor(color.getRedF(), color.getGreenF(), color.getBlueF());

          if (faceData.diffuseTexture) {
            const auto &tpoint = vertex.getTextureMap();

            buffer->addTexturePoint(float(tpoint.x), float(tpoint.y));
          }
          else
            buffer->addTexturePoint(0.0f, 0.0f);
        }

        pos += faceData.len;

        objectData->faceDatas.push_back(faceData);
      }

      objectData->buffer->load();
    }
  }

  auto max3 = [](double x, double y, double z) {
    return std::max(std::max(x, y), z);
  };

  sceneScale_ = float(1.0/max3(sceneSize.getX(), sceneSize.getY(), sceneSize.getZ()));
  //std::cerr << "Scene Scale : " << sceneScale_ << "\n";
}

void
CQNewGLCanvas::
paintGL()
{
  // per-frame time logic
  camera_->updateFrameTime();

  //---

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //---

  auto aspect     = float(app_->windowWidth())/float(app_->windowHeight());
  auto projection = CGLMatrix3D::perspective(camera_->zoom(), aspect, 0.1f, 100.0f);

  auto view = camera_->getViewMatrix();

  //---

  // lighting
  CGLVector3D lightColor(1.0f, 1.0f, 1.0f);

  auto viewPos = camera_->position();

  //---

  // setup model shader
  for (auto &po : objectDatas_) {
    auto *objectData = po.second;

    objectData->buffer->bind();

    modelShaderProgram_->bind();

    modelShaderProgram_->setUniformValue("lightColor", CQGLUtil::toVector(lightColor));
    modelShaderProgram_->setUniformValue("lightPos"  , CQGLUtil::toVector(lightPos_));
    modelShaderProgram_->setUniformValue("viewPos"   , CQGLUtil::toVector(viewPos));

    modelShaderProgram_->setUniformValue("ambientStrength" , float(app_->ambient()));
    modelShaderProgram_->setUniformValue("diffuseStrength" , float(app_->diffuse()));
    modelShaderProgram_->setUniformValue("specularStrength", float(app_->specular()));
    modelShaderProgram_->setUniformValue("shininess"       , float(app_->shininess()));

    // pass projection matrix to shader (note that in this case it could change every frame)
    modelShaderProgram_->setUniformValue("projection", CQGLUtil::toQMatrix(projection));

    // camera/view transformation
    modelShaderProgram_->setUniformValue("view", CQGLUtil::toQMatrix(view));

    // model rotation
    auto modelMatrix = CGLMatrix3D::identity();
    modelMatrix.scaled(sceneScale_, sceneScale_, sceneScale_);
    modelMatrix.rotated(modelXAngle_, CGLVector3D(1.0, 0.0, 0.0));
    modelMatrix.rotated(modelYAngle_, CGLVector3D(0.0, 1.0, 0.0));
    modelMatrix.rotated(modelZAngle_, CGLVector3D(0.0, 0.0, 1.0));
    modelMatrix.translated(float(-sceneCenter_.getX()),
                           float(-sceneCenter_.getY()),
                           float(-sceneCenter_.getZ()));
    modelShaderProgram_->setUniformValue("model", CQGLUtil::toQMatrix(modelMatrix));

    // render model
    for (const auto &faceData : objectData->faceDatas) {
      bool useDiffuseTexture = faceData.diffuseTexture;

      modelShaderProgram_->setUniformValue("useDiffuseTexture", useDiffuseTexture);

      if (useDiffuseTexture) {
        glActiveTexture(GL_TEXTURE0);
        faceData.diffuseTexture->bind();

        modelShaderProgram_->setUniformValue("diffuseTexture", 0);
      }

      //---

      bool useSpecularTexture = faceData.specularTexture;

      modelShaderProgram_->setUniformValue("useSpecularTexture", useSpecularTexture);

      if (useSpecularTexture) {
        glActiveTexture(GL_TEXTURE1);
        faceData.specularTexture->bind();

        modelShaderProgram_->setUniformValue("specularTexture", 1);
      }

      //---

      bool useNormalTexture = faceData.normalTexture;

      modelShaderProgram_->setUniformValue("useNormalTexture", useNormalTexture);

      if (useNormalTexture) {
        glActiveTexture(GL_TEXTURE2);
        faceData.normalTexture->bind();

        modelShaderProgram_->setUniformValue("normalTexture", 2);
      }

      //---

      modelShaderProgram_->setUniformValue("isWireframe", 0);

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

      glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);

      if (isPolygonLine()) {
        modelShaderProgram_->setUniformValue("isWireframe", 1);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
      }
    }

    objectData->buffer->unbind();
  }

  //---

  // setup light shader
  lightBuffer_->bind();

  lightShaderProgram_->bind();

  lightShaderProgram_->setUniformValue("projection", CQGLUtil::toQMatrix(projection));
  lightShaderProgram_->setUniformValue("view", CQGLUtil::toQMatrix(view));

  auto lightMatrix = CGLMatrix3D::translation(lightPos_);
  lightMatrix.scaled(0.01f, 0.01f, 0.01f);
  lightShaderProgram_->setUniformValue("model", CQGLUtil::toQMatrix(lightMatrix));

  // draw light
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  //lightBuffer_->drawTriangles();

  lightBuffer_->unbind();
}

void
CQNewGLCanvas::
resizeGL(int width, int height)
{
  app_->setWindowWidth (width);
  app_->setWindowHeight(height);

  //---

  glViewport(0, 0, width, height);
}

void
CQNewGLCanvas::
mousePressEvent(QMouseEvent *e)
{
  float xpos = float(e->x());
  float ypos = float(e->y());

  auto type = app_->type();

  if (type == CQNewGLModel::Type::CAMERA)
    camera_->setLastPos(xpos, ypos);

  update();
}

void
CQNewGLCanvas::
mouseReleaseEvent(QMouseEvent *)
{
  update();
}

void
CQNewGLCanvas::
mouseMoveEvent(QMouseEvent *e)
{
  float xpos = float(e->x());
  float ypos = float(e->y());

  //---

  auto type = app_->type();

  if (type == CQNewGLModel::Type::CAMERA) {
    float xoffset, yoffset;

    camera_->deltaPos(xpos, ypos, xoffset, yoffset);

    camera_->setLastPos(xpos, ypos);

    camera_->processMouseMovement(xoffset, yoffset);
  }

  update();
}

void
CQNewGLCanvas::
wheelEvent(QWheelEvent *e)
{
  float y = float(e->angleDelta().y()/250.0);

  auto type = app_->type();

  if (type == CQNewGLModel::Type::CAMERA)
    camera_->processMouseScroll(y);

  update();
}

void
CQNewGLCanvas::
keyPressEvent(QKeyEvent *e)
{
  bool isShift = (e->modifiers() & Qt::ShiftModifier);

  auto dt = 0.01f; /* camera_->deltaTime(); */
  auto da = M_PI/180.0;

  if (isShift) {
    dt = -dt;
    da = -da;
  }

  auto type = app_->type();

  if      (e->key() == Qt::Key_W) {
    if      (type == CQNewGLModel::Type::CAMERA)
      camera_->processKeyboard(CGLCamera::Movement::FORWARD, dt);
    else if (type == CQNewGLModel::Type::LIGHT)
      lightPos_ += CGLVector3D(0.0f, 0.1f, 0.0f);
  }
  else if (e->key() == Qt::Key_S) {
    if      (type == CQNewGLModel::Type::CAMERA)
      camera_->processKeyboard(CGLCamera::Movement::BACKWARD, dt);
    else if (type == CQNewGLModel::Type::LIGHT)
      lightPos_ -= CGLVector3D(0.0f, 0.1f, 0.0f);
  }
  else if (e->key() == Qt::Key_A) {
    if      (type == CQNewGLModel::Type::CAMERA)
      camera_->processKeyboard(CGLCamera::Movement::LEFT, dt);
    else if (type == CQNewGLModel::Type::LIGHT)
      lightPos_ -= CGLVector3D(0.1f, 0.0f, 0.0f);
  }
  else if (e->key() == Qt::Key_D) {
    if      (type == CQNewGLModel::Type::CAMERA)
      camera_->processKeyboard(CGLCamera::Movement::RIGHT, dt);
    else if (type == CQNewGLModel::Type::LIGHT)
      lightPos_ += CGLVector3D(0.1f, 0.0f, 0.0f);
  }
  else if (e->key() == Qt::Key_X) {
    if (type == CQNewGLModel::Type::MODEL)
      modelXAngle_ += da;
  }
  else if (e->key() == Qt::Key_Y) {
    if (type == CQNewGLModel::Type::MODEL)
      modelYAngle_ += da;
  }
  else if (e->key() == Qt::Key_Z) {
    if (type == CQNewGLModel::Type::MODEL)
      modelZAngle_ += da;
  }
  else if (e->key() == Qt::Key_Up) {
    if (type == CQNewGLModel::Type::LIGHT)
      lightPos_ += CGLVector3D(0.0f, 0.0f, 0.1f);
  }
  else if (e->key() == Qt::Key_Down) {
    if (type == CQNewGLModel::Type::LIGHT)
      lightPos_ -= CGLVector3D(0.0f, 0.0f, 0.1f);
  }

  update();
}
