#include <Canvas.h>
#include <App.h>
#include <ShaderProgram.h>
#include <Camera.h>
#include <GeomObject.h>
#include <Texture.h>

#include <CQGLBuffer.h>
#include <CQGLTexture.h>
#include <CQGLUtil.h>

#include <CGeomScene3D.h>
#include <CGeomFace3D.h>
#include <CBBox3D.h>

#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>

namespace CQModel3DView {

Canvas::
Canvas(App *app) :
 app_(app)
{
  setObjectName("canvas");

  setFocusPolicy(Qt::StrongFocus);

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  //---

  camera_ = new Camera(app_);

  camera_->setOrigin(CVector3D(0, 0, 0));
  camera_->setDistance(5);

  //---

  connect(app_, SIGNAL(modelAdded()), this, SLOT(updateScene()));

  connect(camera_, SIGNAL(stateChangedSignal()), this, SLOT(update()));
}

//---

ShaderProgram *
Canvas::
shaderProgram()
{
  return getShader("scene.vs", "scene.fs");
}

//---

void
Canvas::
initializeGL()
{
  initializeOpenGLFunctions();
}

void
Canvas::
resizeGL(int width, int height)
{
  setPixelWidth (width);
  setPixelHeight(height);

  glViewport(0, 0, width, height);

  setAspect(double(width)/double(height));

  camera_->setAspect(aspect());
}

void
Canvas::
paintGL()
{
  if (invalid_) {
    addScene();

    invalid_ = false;
  }

  //---

  glClearColor(bgColor_.redF(), bgColor_.greenF(), bgColor_.blueF(), 1.0f);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //---

  // set GL state
  enableDepthTest  ();
  enableCullFace   ();
  enableFrontFace  ();
  enablePolygonLine();

  glPointSize(pointSize());
  glLineWidth(lineWidth());

  //---

  drawScene();
}

//---

void
Canvas::
updateScene()
{
  invalid_ = true;

  update();
}

void
Canvas::
addScene()
{
  bbox_ = CBBox3D();

  //---

  auto *scene = app_->scene();

  for (auto *object : scene->getObjects()) {
    auto *object1 = dynamic_cast<GeomObject *>(object);
    assert(object1);

    //---

    auto modelMatrix = CMatrix3DH(object1->getHierTransform());
    auto meshMatrix  = CMatrix3DH(object->getMeshGlobalTransform());

    //---

    auto *buffer = object1->initBuffer(this);

    //---

    auto *objectMaterial = object->getMaterialP();

    auto *diffuseTexture  = object->getDiffuseTexture();
    auto *normalTexture   = object->getNormalTexture();
    auto *specularTexture = object->getSpecularTexture();
    auto *emissiveTexture = object->getEmissiveTexture();

    //---

    CBBox3D bbox1;

    int pos = 0;

    auto addFaceData = [&](CGeomFace3D *face, bool reverse=false) {
      FaceData faceData;

      faceData.face = const_cast<CGeomFace3D *>(face);

      //---

      auto *faceMaterial = faceData.face->getMaterialP();

      if (! faceMaterial && objectMaterial)
        faceMaterial = objectMaterial;

      //---

      auto color = face->color().value_or(CRGBA(1, 1, 1));

      if (faceMaterial && faceMaterial->diffuse())
        color = faceMaterial->diffuse().value();

      //---

      // set face textures
      auto *diffuseTexture1  = face->getDiffuseTexture();
      auto *normalTexture1   = face->getNormalTexture();
      auto *specularTexture1 = face->getSpecularTexture();
      auto *emissiveTexture1 = face->getEmissiveTexture();

      if (! diffuseTexture1 ) diffuseTexture1  = diffuseTexture;
      if (! normalTexture1  ) normalTexture1   = normalTexture;
      if (! specularTexture1) specularTexture1 = specularTexture;
      if (! emissiveTexture1) emissiveTexture1 = emissiveTexture;

      if (faceMaterial) {
        if (faceMaterial->diffuseTexture ()) diffuseTexture1  = faceMaterial->diffuseTexture ();
        if (faceMaterial->normalTexture  ()) normalTexture1   = faceMaterial->normalTexture  ();
        if (faceMaterial->specularTexture()) specularTexture1 = faceMaterial->specularTexture();
        if (faceMaterial->emissiveTexture()) emissiveTexture1 = faceMaterial->emissiveTexture();
      }

      if (diffuseTexture1)
        faceData.diffuseTexture = getGLTexture(diffuseTexture1, /*add*/true);

      if (normalTexture1)
        faceData.normalTexture = getGLTexture(normalTexture1, /*add*/true);

      if (specularTexture1)
        faceData.specularTexture = getGLTexture(specularTexture1, /*add*/true);

      if (emissiveTexture1)
        faceData.emissiveTexture = getGLTexture(emissiveTexture1, /*add*/true);

      //---

      auto vertices = face->getVertices();

      if (reverse)
        std::reverse(vertices.begin(), vertices.end());

      //--

      // get face normal
      CVector3D normal;

      if (face->getNormalSet())
        normal = face->getNormal();
      else {
        for (const auto &v : vertices) {
          auto &vertex = object->getVertex(v);

          vertex.setViewed(vertex.getModel());
        }

        face->calcNormal(normal);
      }

      //---

      faceData.pos = pos;
      faceData.len = int(vertices.size());

      int iv = 0;

      for (const auto &v : vertices) {
        faceData.vertices.push_back(v);

        const auto &vertex = object->getVertex(v);
        const auto &model  = vertex.getModel();

        auto model1 = meshMatrix *model;
        auto model2 = modelMatrix*model1;

        //---

        // update color, normal for custom vertex value

        auto normal1 = normal;
        auto color1  = color;

        if (vertex.hasNormal())
          normal1 = vertex.getNormal();

        if (vertex.hasColor())
          color1 = vertex.getColor();

        //---

        if (faceData.normalTexture) {
          CPoint2D tpoint;

          if (vertex.hasTextureMap())
            tpoint = vertex.getTextureMap();
          else
            tpoint = face->getTexturePoint(vertex, iv);

          int tw = faceData.normalTexture->getWidth ();
          int th = faceData.normalTexture->getHeight();

          auto tx = CMathUtil::clamp(tpoint.x, 0.0, 1.0);
          auto ty = CMathUtil::clamp(tpoint.y, 0.0, 1.0);

          // get normal value from texture
          auto rgba = faceData.normalTexture->getImage().pixel(tx*(tw - 1), ty*(th - 1));
          auto tnormal = CVector3D(qRed(rgba)/255.0, qGreen(rgba)/255.0, qBlue(rgba)/255.0);

          // this normal is in tangent space
          normal1 = (tnormal*2.0 - CVector3D(1.0, 1.0, 1.0)).normalized();
        }

        //---

        buffer->addInd(vertex.getInd());

        buffer->addPoint(float(model.x), float(model.y), float(model.z));

        buffer->addNormal(float(normal1.getX()), float(normal1.getY()), float(normal1.getZ()));

        buffer->addColor(color1);

        //---

#if 0
        const auto &jointData = vertex.getJointData();

        if (jointData.nodeDatas[0].node >= 0) {
          for (int i = 0; i < 4; ++i) {
            boneNodeIds[i] = jointData.nodeDatas[i].node;
            boneWeights[i] = jointData.nodeDatas[i].weight;
          }

          buffer->addBoneIds    (boneNodeIds[0], boneNodeIds[1], boneNodeIds[2], boneNodeIds[3]);
          buffer->addBoneWeights(boneWeights[0], boneWeights[1], boneWeights[2], boneWeights[3]);
        }
#endif

        //---

        if (faceData.diffuseTexture) {
          const auto &tpoint = face->getTexturePoint(vertex, iv);

          buffer->addTexturePoint(float(tpoint.x), float(tpoint.y));
        }
        else
          buffer->addTexturePoint(0.0f, 0.0f);

        //---

        ++iv;

        bbox1 += model2;
      }

      pos += faceData.len;

      object1->addFaceData(faceData);
    };

    //---

    const auto &faces = object->getFaces();

    for (auto *face : faces) {
      addFaceData(face);

      auto *faceMaterial = const_cast<CGeomFace3D *>(face)->getMaterialP();

      if (! faceMaterial && objectMaterial)
        faceMaterial = objectMaterial;

      if (face->getTwoSided() || (faceMaterial && faceMaterial->isTwoSided()))
        addFaceData(face, /*reverse*/true);
    }

    //---

#if 0
    const auto &lines = object->getLines();

    for (const auto *line : lines) {
      FaceData faceData;

      faceData.line = const_cast<CGeomLine3D *>(line);

      //---

      auto color = line->getColor();

      //---

      auto v1 = line->getStartInd();
      auto v2 = line->getEndInd  ();

      std::vector<uint> vertices;

      vertices.push_back(v1);
      vertices.push_back(v2);

      //--

      faceData.pos = pos;
      faceData.len = int(vertices.size());

      int iv = 0;

      for (const auto &v : vertices) {
        faceData.vertices.push_back(v);

        const auto &vertex = object->getVertex(v);
        const auto &model  = vertex.getModel();

        auto model1 = meshMatrix *model;
        auto model2 = modelMatrix*model1;

        //---

        // update color, normal for custom vertex value

        auto color1 = color;

        if (vertex.hasColor())
          color1 = vertex.getColor();

        //---

        buffer->addInd(vertex.getInd());

        buffer->addPoint(float(model.x), float(model.y), float(model.z));

        buffer->addNormal(0, 0, 1);

        buffer->addColor(color1);

        buffer->addTexturePoint(0.0f, 0.0f);

        //---

        ++iv;

        bbox1 += model2;
      }

      pos += faceData.len;

      object1->addFaceData(faceData);
    }
#endif

    //---

    object1->setBBox(bbox1);

    bbox_ += bbox1;

    //---

    buffer->load();
  }

  //---

  if (! bbox_.isSet()) {
    bbox_.add(CPoint3D(-1, -1, -1));
    bbox_.add(CPoint3D( 1,  1,  1));
  }

  //---

  auto c = bbox_.getCenter();
  auto d = bbox_.getMaxSize();

  camera_->setOrigin(CVector3D(c));
  camera_->setDistance(std::sqrt(2.0)*d);
}

void
Canvas::
drawScene()
{
  auto *program = this->shaderProgram();

  program->bind();

  //---

  auto worldMatrix = camera_->perspectiveMatrix();
  auto viewMatrix  = camera_->viewMatrix();
  auto viewPos     = camera_->position();

  // camera projection
  program->setUniformValue("projection", CQGLUtil::toQMatrix(worldMatrix));

  // camera/view transformation
  program->setUniformValue("view", CQGLUtil::toQMatrix(viewMatrix));

  // view pos
  program->setUniformValue("viewPos", CQGLUtil::toVector(viewPos));

  //---

#if 0
  // add light data to shader program
  addShaderLights(program);
#else
  auto ambientColor = CRGBA::white();
  auto ambientStrength = 0.2;
  auto diffuseStrength = 1.0;
  auto emissiveColor = CRGBA::white();
  auto emissiveStrength = 0.0;
  auto specularColor = CRGBA::white();
  auto specularStrength = 0.2;

  program->setUniformValue("ambientColor", CQGLUtil::toVector(ambientColor));
  program->setUniformValue("ambientStrength", float(ambientStrength));

  program->setUniformValue("diffuseStrength", float(diffuseStrength));

  program->setUniformValue("emissionColor"   , CQGLUtil::toVector(emissiveColor));
  program->setUniformValue("emissiveStrength", float(emissiveStrength));

  program->setUniformValue("specularColor"   , CQGLUtil::toVector(specularColor));
  program->setUniformValue("specularStrength", float(specularStrength));

  program->setUniformValue("fixedDiffuse", false);
#endif

  //---

  glPointSize(pointSize());
  glLineWidth(lineWidth());

#if 0
  paintData_.reset();
#endif

  //---

#if 0
  bool isAnim = (app_->animName() != "");
#endif

  //---

#if 0
  auto wireframeColor = this->wireframeColor();

  auto wireframeTransparency = 0.5f;

  program->setUniformValue("wireframeColor", CQGLUtil::toVector(ColorToVector(wireframeColor)));

  program->setUniformValue("wireframeTransparency", wireframeTransparency);
#endif

  //---

  auto *scene = app_->scene();

  for (auto *object : scene->getObjects()) {
    if (! object->getVisible())
      continue;

    auto *object1 = dynamic_cast<GeomObject *>(object);
    assert(object1);

    //---

    // mesh matrix
    auto meshMatrix = object->getMeshGlobalTransform();
    program->setUniformValue("meshMatrix", CQGLUtil::toQMatrix(meshMatrix));

    //---

    // model matrix
    //auto modelMatrix = CMatrix3DH::identity();
    auto modelMatrix = object1->getHierTransform();
    program->setUniformValue("model", CQGLUtil::toQMatrix(modelMatrix));

    //---

#if 0
    // anim
    program->setUniformValue("useBonePoints", isAnim); // per object ?

    if (isAnim) {
      updateNodeMatrices(object);

      program->setUniformValueArray("globalBoneTransform",
        &paintData_.nodeQMatrices[0], PaintData::NUM_NODE_MATRICES);
    }
#endif

    //---

    object1->buffer()->bind();

    //---

    auto *objectMaterial = object->getMaterialP();

    //---

    auto drawFace = [&](const FaceData &faceData, double transparency) {
      bool useDiffuseTexture = (isTextured() ? !!faceData.diffuseTexture : false);

      program->setUniformValue("diffuseTexture.enabled", useDiffuseTexture);

      if (useDiffuseTexture) {
        glActiveTexture(GL_TEXTURE0);
        faceData.diffuseTexture->bind();

        program->setUniformValue("diffuseTexture.texture", 0);
      }

      //---

      bool useNormalTexture = (isTextured() ? !!faceData.normalTexture : false);

      program->setUniformValue("normalTexture.enabled", useNormalTexture);

      if (useNormalTexture) {
        glActiveTexture(GL_TEXTURE1);
        faceData.normalTexture->bind();

        program->setUniformValue("normalTexture.texture", 1);
      }

      //---

      bool useSpecularTexture = (isTextured() ? !!faceData.specularTexture : false);

      program->setUniformValue("specularTexture.enabled", useSpecularTexture);

      if (useSpecularTexture) {
        glActiveTexture(GL_TEXTURE2);
        faceData.specularTexture->bind();

        program->setUniformValue("specularTexture.texture", 2);
      }

      //---

      bool useEmissiveTexture = (isTextured() ? !!faceData.emissiveTexture : false);

      program->setUniformValue("emissiveTexture.enabled", useEmissiveTexture);

      if (useEmissiveTexture) {
        glActiveTexture(GL_TEXTURE3);
        faceData.emissiveTexture->bind();

        program->setUniformValue("emissiveTexture.texture", 3);
      }

      program->setUniformValue("emissionColor", CQGLUtil::toVector(faceData.emission));

      //---

      program->setUniformValue("shininess", float(faceData.shininess));

      program->setUniformValue("transparency", float(1.0 - transparency));

      //---

#if 0
      if (isWireframe()) {
        program->setUniformValue("isWireframe", true);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
      }

      if (isSolid()) {
        program->setUniformValue("isWireframe", false);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
      }

      if (isPoints()) {
        program->setUniformValue("isWireframe", true);

        glDrawArrays(GL_POINTS, faceData.pos, faceData.len);
      }
#else
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

      glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
#endif
    };

    //---

#if 0
    bool anyTransparent = false;
#endif

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);

    for (const auto &faceData : object1->faceDatas()) {
      if      (faceData.face) {
        auto *face = faceData.face;

        if (! face->getVisible())
          continue;

        auto *faceMaterial = face->getMaterialP();

        if (! faceMaterial && objectMaterial)
          faceMaterial = objectMaterial;

#if 0
        if (faceMaterial && faceMaterial->transparency() > 0.0) {
          anyTransparent = true;
          continue;
        }
#endif

        drawFace(faceData, 0.0);
      }
#if 0
      else if (faceData.line) {
        auto *line = faceData.line;

        if (! line->getVisible())
          continue;

        program->setUniformValue("isWireframe", true);

        glDrawArrays(GL_LINES, faceData.pos, faceData.len);
      }
#endif
      else
        assert(false);
    }

#if 0
    if (anyTransparent) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glDepthMask(GL_FALSE);

      for (const auto &faceData : object1->faceDatas()) {
        auto *face = faceData.face;

        if (! face->getVisible())
          continue;

        auto *faceMaterial = face->getMaterialP();

        if (! faceMaterial && objectMaterial)
          faceMaterial = objectMaterial;

        if (! faceMaterial || faceMaterial->transparency() <= 0.0)
          continue;

        drawFace(faceData, faceMaterial->transparency());
      }

      glDisable(GL_BLEND);
      glDepthMask(GL_TRUE);
    }
#endif

    //---

    object1->buffer()->unbind();
  }

  //---

  program->release();
}

//---

void
Canvas::
mousePressEvent(QMouseEvent *e)
{
  mouseData_.pressed = true;
  mouseData_.button  = e->button();
  mouseData_.press   = CPoint2D(e->x(), e->y());

  mouseData_.isShift   = (e->modifiers() & Qt::ShiftModifier);
  mouseData_.isControl = (e->modifiers() & Qt::ControlModifier);
}

void
Canvas::
mouseMoveEvent(QMouseEvent *e)
{
  mouseData_.move.x = e->x();
  mouseData_.move.y = e->y();

  mouseData_.isShift   = (e->modifiers() & Qt::ShiftModifier);
  mouseData_.isControl = (e->modifiers() & Qt::ControlModifier);

  //---

  auto dx = CMathUtil::sign(mouseData_.move.x - mouseData_.press.x);
  auto dy = CMathUtil::sign(mouseData_.move.y - mouseData_.press.y);

  if      (mouseData_.button == Qt::LeftButton) {
  }
  else if (mouseData_.button == Qt::MiddleButton) {
    auto da = M_PI/180.0;

    camera_->rotateY(-dx*da);
    camera_->rotateX(-dy*da);
  }
  else if (mouseData_.button == Qt::RightButton) {
    camera_->moveRight(-dx/10.0);
    camera_->moveUp   ( dy/10.0);
  }

  //---

  mouseData_.press = mouseData_.move;
}

void
Canvas::
mouseReleaseEvent(QMouseEvent *)
{
  mouseData_.pressed = false;
  mouseData_.button  = Qt::NoButton;
}

void
Canvas::
wheelEvent(QWheelEvent *e)
{
  auto d  = bbox_.getMaxSize()/100.0;
  auto dw = e->angleDelta().y()/250.0;

  camera_->setDistance(camera_->distance() - dw*d);
}

void
Canvas::
keyPressEvent(QKeyEvent *e)
{
  mouseData_.isShift   = (e->modifiers() & Qt::ShiftModifier);
  mouseData_.isControl = (e->modifiers() & Qt::ControlModifier);

  auto k = e->key();

  auto d  = bbox_.getMaxSize()/100.0;
  auto da = M_PI/60.0;

  if      (k == Qt::Key_Left) {
    camera_->moveRight(-d);
  }
  else if (k == Qt::Key_Right) {
    camera_->moveRight(d);
  }
  else if (k == Qt::Key_Up) {
    camera_->moveUp(d);
  }
  else if (k == Qt::Key_Down) {
    camera_->moveUp(-d);
  }
  else if (k == Qt::Key_Plus) {
    camera_->moveFront(d);
  }
  else if (k == Qt::Key_Minus) {
    camera_->moveFront(-d);
  }
  else if (k == Qt::Key_W) {
    camera_->rotateX(da);
  }
  else if (k == Qt::Key_S) {
    camera_->rotateX(-da);
  }
  else if (k == Qt::Key_A) {
    camera_->rotateY(da);
  }
  else if (k == Qt::Key_D) {
    camera_->rotateY(-da);
  }
  else if (k == Qt::Key_Q) {
    camera_->rotateZ(-da);
  }
  else if (k == Qt::Key_E) {
    camera_->rotateZ(da);
  }
  else if (k == Qt::Key_Space) {
    camera_->printMatrices();
  }
}

//---

void
Canvas::
enableDepthTest()
{
  if (isDepthTest())
    glEnable(GL_DEPTH_TEST);
  else
    glDisable(GL_DEPTH_TEST);
}

void
Canvas::
enableCullFace()
{
  if (isCullFace())
    glEnable(GL_CULL_FACE);
  else
    glDisable(GL_CULL_FACE);
}

void
Canvas::
enableFrontFace()
{
  glFrontFace(isFrontFace() ? GL_CW : GL_CCW);
}

void
Canvas::
enablePolygonLine()
{
  if (isPolygonLine())
    glEnable(GL_POLYGON_OFFSET_LINE);
  else
    glDisable(GL_POLYGON_OFFSET_LINE);

  glPolygonOffset(-1.0f, -1.0f);
}

//---

ShaderProgram *
Canvas::
getShader(const QString &vertex, const QString &fragment)
{
  auto id = QString("V:%1,F:%2").arg(vertex).arg(fragment);

  auto ps = shaders_.find(id);

  if (ps == shaders_.end()) {
    auto *shaderProgram = new ShaderProgram(app_);

    shaderProgram->addShaders(vertex, fragment);

    ps = shaders_.insert(ps, Shaders::value_type(id, shaderProgram));
  }

  return (*ps).second;
}

CQGLTexture *
Canvas::
getGLTexture(CGeomTexture *texture, bool /*add*/)
{
  auto *texture1 = dynamic_cast<Texture *>(texture);
  assert(texture1);

  if (! texture1->glTexture(this)) {
    //if (! add) return nullptr;

    initGLTexture(texture1);

    Q_EMIT textureAdded();
  }

  return texture1->glTexture(this);
}

void
Canvas::
initGLTexture(Texture *texture)
{
  const auto &image = texture->image()->image();

  auto flippedImage = image->dup();

  flippedImage->flipH();

  auto *t1 = makeTexture(image);
  auto *t2 = makeTexture(flippedImage);

  t1->setName(texture->name());
  t2->setName(texture->name() + ".flip");

  texture->setGlTextures(this, t1, t2);
}

CQGLTexture *
Canvas::
makeTexture(const CImagePtr &image) const
{
  auto *texture = new CQGLTexture(image);

  texture->setFunctions(const_cast<Canvas *>(this));

  return texture;
}

}
