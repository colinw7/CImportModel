#include <MaterialView.h>
#include <ShaderProgram.h>
#include <Shapes.h>
#include <Camera.h>
#include <Light.h>
#include <Texture.h>
#include <App.h>
#include <Util.h>

#include <CQGLBuffer.h>
#include <CQGLTexture.h>
#include <CQGLUtil.h>

#include <QMouseEvent>

namespace CQTextureGen {

//---

MaterialView::
MaterialView(App *app) :
 QGLWidget(app), app_(app)
{
  setObjectName("materials");

  //---

  material_ = new CGeomMaterial;

  //---

  camera_ = new Camera;

  camera_->setName("camera");
  camera_->setPosition(CVector3D(2.9, 3.1, 3));

  //camera_->setPitch(-M_PI/5.0);
  //camera_->setYaw(2.0*M_PI/3.0);

  camera_->setOrigin(CVector3D(0, 0, 0));

  //camera_->moveAroundX(0.01);

  //---

  addLight();

  //---

  connect(app_, SIGNAL(materialChanged()), this, SLOT(invalidateSlot()));

  auto *camera1 = dynamic_cast<Camera *>(camera_);
  connect(camera1, SIGNAL(stateChanged()), this, SLOT(invalidateSlot()));
}

void
MaterialView::
addLight()
{
  int  id   = lights_.size() + 1;
  auto name = "light." + std::to_string(id);

  auto *light = new Light;

  light->setId(id);
  light->setName(name);

  light->setPosition (CPoint3D(1.0, 2.0, 3.0));
  light->setDirection(CVector3D(-1.0, -2.0, -3.0));

  lights_.push_back(light);

  connect(light, SIGNAL(stateChanged()), this, SIGNAL(lightChanged()));
  connect(light, SIGNAL(stateChanged()), this, SLOT(invalidateSlot()));

  Q_EMIT lightAdded();

  invalidateSlot();
}

Light *
MaterialView::
getLightById(int id) const
{
  for (auto *light : lights_)
    if (light->id() == id)
      return light;

  return nullptr;
}

void
MaterialView::
setShapeType(const ShapeType &v)
{
  shapeType_ = v;

  invalidateSlot();
}

void
MaterialView::
invalidateSlot()
{
  valid_ = false;

  update();
}

void
MaterialView::
initializeGL()
{
  initializeOpenGLFunctions();

  //---

  shaderProgram_ = new ShaderProgram(app_);

  shaderProgram_->addShaders("materials.vs", "materials.fs");
}

void
MaterialView::
resizeGL(int width, int height)
{
  setPixelWidth (width);
  setPixelHeight(height);

  glViewport(0, 0, width, height);

  setAspect(double(width)/double(height));

  camera_->setAspect(aspect());
}

void
MaterialView::
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
MaterialView::
addGeometry()
{
  if (! diffuseTexture_) {
    diffuseTexture_  = new Texture;
    normalTexture_   = new Texture;
    specularTexture_ = new Texture;
    emissionTexture_ = new Texture;
    metallicTexture_ = new Texture;
    ORMTexture_      = new Texture;

    initGLTexture(diffuseTexture_ , app_->diffuseImage ());
    initGLTexture(normalTexture_  , app_->normalImage  ());
    initGLTexture(specularTexture_, app_->specularImage());
    initGLTexture(emissionTexture_, app_->emissionImage());
    initGLTexture(metallicTexture_, app_->metallicImage());
    initGLTexture(ORMTexture_     , app_->ORMImage     ());
  }
  else {
    diffuseTexture_ ->glTexture()->setImage(app_->diffuseImage ());
    normalTexture_  ->glTexture()->setImage(app_->normalImage  ());
    specularTexture_->glTexture()->setImage(app_->specularImage());
    emissionTexture_->glTexture()->setImage(app_->emissionImage());
    metallicTexture_->glTexture()->setImage(app_->metallicImage());
    ORMTexture_     ->glTexture()->setImage(app_->ORMImage     ());
  }

  bool hasDiffuseTexture  = app_->isDiffuseImageSet ();
  bool hasNormalTexture   = app_->isNormalImageSet  ();
  bool hasSpecularTexture = app_->isSpecularImageSet();
  bool hasEmissionTexture = app_->isEmissionImageSet();
  bool hasMetallicTexture = app_->isMetallicImageSet();
  bool hasORMTexture      = app_->isORMImageSet     ();

  auto diffuseColor = app_->diffuseColor();

  //---

  material_->setDiffuseTexture (hasDiffuseTexture  ? diffuseTexture_  : nullptr);
  material_->setNormalTexture  (hasNormalTexture   ? normalTexture_   : nullptr);
  material_->setSpecularTexture(hasSpecularTexture ? specularTexture_ : nullptr);
  material_->setEmissiveTexture(hasEmissionTexture ? emissionTexture_ : nullptr);
  material_->setMetallicTexture(hasMetallicTexture ? metallicTexture_ : nullptr);
  material_->setORMTexture     (hasORMTexture      ? ORMTexture_      : nullptr);

  //---

  auto *buffer = initBuffer();

  faceDataList_.clear();

  //---

  Shapes::ShapeData shapeData;

  shapeData.color = (hasDiffuseTexture ? material_->getDiffuse() : diffuseColor);

  if      (shapeType_ == ShapeType::CUBE) {
    auto addCube = [&](const CPoint3D &c, double r) {
      auto p1 = c - CPoint3D(r, r, r);
      auto p2 = c + CPoint3D(r, r, r);

      FaceDataList faceDataList;
      faceDataList.pos = faceDataList_.pos;
      CBBox3D bbox(p1, p2);
      Shapes::addCube(buffer, bbox, shapeData, faceDataList);
      faceDataList_.add(faceDataList);
      faceDataList_.pos += faceDataList.pos;
    };

    addCube(CPoint3D(0, 0, 0), 1.0);
  }
  else if (shapeType_ == ShapeType::SPHERE) {
    auto addSphere = [&](const CPoint3D &c, double r) {
      auto p1 = c - CPoint3D(r, r, r);
      auto p2 = c + CPoint3D(r, r, r);

      FaceDataList faceDataList;
      faceDataList.pos = faceDataList_.pos;
      Shapes::addSphere(buffer, p1, p2, shapeData, 0, 2*M_PI, faceDataList);
      faceDataList_.add(faceDataList);
      faceDataList_.pos += faceDataList.pos;
    };

    addSphere(CPoint3D(0, 0, 0), 0.5);
  }
  else if (shapeType_ == ShapeType::PLANE) {
    auto addPlane = [&](const CPoint3D &c, double s) {
      auto p1 = c - CPoint3D(s, s, 0);
      auto p2 = c + CPoint3D(s, s, 0);

      FaceDataList faceDataList;
      faceDataList.pos = faceDataList_.pos;
      CBBox3D bbox(p1, p2);
      Shapes::addPlane(buffer, bbox, shapeData, faceDataList);
      faceDataList_.add(faceDataList);
      faceDataList_.pos += faceDataList.pos;
    };

    addPlane(CPoint3D(0, 0, 0), 1.0);
  }

  //---

  buffer->load();
}

void
MaterialView::
drawGeometry()
{
  auto *program = shaderProgram_;

  program->bind();

  //---

  // lighting

  program->setUniformValue("ambientColor", CQGLUtil::toVector(ambientColor()));
  program->setUniformValue("ambientStrength", float(ambientStrength()));

  program->setUniformValue("diffuseStrength" , float(diffuseStrength()));

  program->setUniformValue("specularColor", CQGLUtil::toVector(specularColor()));
  program->setUniformValue("specularStrength", float(specularStrength()));

  program->setUniformValue("emissionColor", CQGLUtil::toVector(emissionColor()));
  program->setUniformValue("emissionStrength", float(emissionStrength()));

  program->setUniformValue("shininess", float(shininess()));

  //---

  // max four active lights
  enum { MAX_LIGHTS = 4 };

  int il = 0;

  for (auto *light : lights_) {
    auto lightName = QString("lights[%1]").arg(il);

    program->setUniformValue(toCString(lightName + ".type"), int(light->type()));
    program->setUniformValue(toCString(lightName + ".enabled"), light->isEnabled());

    program->setUniformValue(toCString(lightName + ".position"),
      CQGLUtil::toVector(light->position()));

    program->setUniformValue(toCString(lightName + ".color"),
      CQGLUtil::toVector(ColorToVector(light->color())));

    if      (light->type() == CGeomLight3DType::DIRECTIONAL) {
      program->setUniformValue(toCString(lightName + ".direction"),
        CQGLUtil::toVector(light->direction()));
    }
    else if (light->type() == CGeomLight3DType::POINT) {
      program->setUniformValue(toCString(lightName + ".radius"), float(light->getPointRadius()));

      program->setUniformValue(toCString(lightName + ".attenuation0"),
        float(light->getConstantAttenuation()));
      program->setUniformValue(toCString(lightName + ".attenuation1"),
        float(light->getLinearAttenuation()));
      program->setUniformValue(toCString(lightName + ".attenuation2"),
        float(light->getQuadraticAttenuation()));
    }
    else if (light->type() == CGeomLight3DType::SPOT) {
      program->setUniformValue(toCString(lightName + ".direction"),
        CQGLUtil::toVector(light->getSpotDirection()));

      auto a = light->getSpotCutOffAngle();

      auto cosv = std::cos(CMathGen::DegToRad(a));

      program->setUniformValue(toCString(lightName + ".cutoff"), float(cosv));

      program->setUniformValue(toCString(lightName + ".exponent"),
        float(light->getSpotExponent()));
    }

    ++il;

    if (il >= MAX_LIGHTS)
      break;
  }

  for ( ; il < MAX_LIGHTS; ++il) {
    auto lightName = QString("lights[%1]").arg(il);

    program->setUniformValue(toCString(lightName + ".enabled"), false);
  }

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

  // material

  program->setUniformValue("albedo", CQGLUtil::toVector(ColorToVector(material_->getDiffuse())));

  program->setUniformValue("metallic" , float(material_->getMetallic()));
  program->setUniformValue("roughness", float(material_->getRoughness()));
  program->setUniformValue("occlusion", float(material_->getAmbientOcclusion()));

  auto *diffuseTexture  = material_->diffuseTexture ();
  auto *normalTexture   = material_->normalTexture  ();
  auto *specularTexture = material_->specularTexture();
  auto *emissionTexture = material_->emissiveTexture();
  auto *metallicTexture = material_->metallicTexture();
  auto *ORMTexture      = material_->ORMTexture     ();

  auto setTextureVars = [&](const QString &name, CGeomTexture *texture, int ind) {
    auto name1 = (name + "Texture.enabled").toStdString();
    auto name2 = (name + "Texture.texture").toStdString();

    CQGLTexture *glTexture = nullptr;

    if (texture)
      glTexture = getGLTexture(texture);

    program->setUniformValue(name1.c_str(), !!glTexture);

    if (glTexture) {
      glActiveTexture(GL_TEXTURE0 + ind);

      glTexture->bind();

      program->setUniformValue(name2.c_str(), ind);
    }
  };

  setTextureVars("diffuse" , diffuseTexture , 0);
  setTextureVars("normal"  , normalTexture  , 1);
  setTextureVars("specular", specularTexture, 2);
  setTextureVars("emission", emissionTexture, 3);
  setTextureVars("metallic", metallicTexture, 4);
  setTextureVars("ORM"     , ORMTexture     , 5);

  //---

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
MaterialView::
initBuffer()
{
  if (! buffer_)
    buffer_ = shaderProgram_->createBuffer();

  buffer_->clearAll();

  return buffer_;
}

//---

void
MaterialView::
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
MaterialView::
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
MaterialView::
mouseReleaseEvent(QMouseEvent *)
{
  mouseData_.pressed = false;
}

void
MaterialView::
wheelEvent(QWheelEvent *e)
{
  auto dw = e->angleDelta().y()/250.0;

  camera_->moveFront(dw);

  update();
}

void
MaterialView::
keyPressEvent(QKeyEvent *e)
{
  int key = e->key();

  if (key == Qt::Key_P) {
    auto *camera1 = dynamic_cast<Camera *>(camera_);

    camera1->print();
  }
}

//---

CQGLTexture *
MaterialView::
getGLTexture(CGeomTexture *texture)
{
  auto *texture1 = dynamic_cast<Texture *>(texture);
  assert(texture1);

  return texture1->glTexture();
}

void
MaterialView::
initGLTexture(Texture *texture, const QImage &image)
{
  auto *t = makeTexture(image);

  //t->setName(texture->name());

  texture->setGlTexture(t);
}

CQGLTexture *
MaterialView::
makeTexture(const QImage &image) const
{
  auto *texture = new CQGLTexture(image);

  texture->setFunctions(const_cast<MaterialView *>(this));

  return texture;
}

//---

}
