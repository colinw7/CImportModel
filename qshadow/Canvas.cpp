#include <Canvas.h>
#include <App.h>
#include <ShaderProgram.h>
#include <Camera.h>
#include <Light.h>
#include <GeomObject.h>
#include <Texture.h>
#include <Util.h>

#include <CQGLBuffer.h>
#include <CQGLTexture.h>
#include <CQGLUtil.h>

#include <CGeomScene3D.h>
#include <CGeomFace3D.h>
#include <CBBox3D.h>

#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>

namespace CQShadow3D {

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

  light_ = new Light(app_);

  light_->setOrigin(CVector3D(0, 0, 0));
  light_->setPosition(CPoint3D(5, 5, 5));

  //---

  connectSlots(true);
}

void
Canvas::
connectSlots(bool b)
{
  if (b) {
    connect(app_, SIGNAL(modelAdded()), this, SLOT(updateScene()));

    connect(camera_, SIGNAL(stateChangedSignal()), this, SLOT(update()));

    connect(light_, SIGNAL(stateChangedSignal()), this, SLOT(update()));
  }
  else {
    disconnect(app_, SIGNAL(modelAdded()), this, SLOT(updateScene()));

    disconnect(camera_, SIGNAL(stateChangedSignal()), this, SLOT(update()));

    disconnect(light_, SIGNAL(stateChangedSignal()), this, SLOT(update()));
  }
}

//---

ShaderProgram *
Canvas::
sceneShaderProgram()
{
  return getShader("Scene", "scene.vs", "scene.fs");
}

ShaderProgram *
Canvas::
skyboxShaderProgram()
{
  return getShader("Skybox", "skybox.vs", "skybox.fs");
}

ShaderProgram *
Canvas::
shadowShaderProgram()
{
  return getShader("Shadow", "shadow.vs", "shadow.fs");
}

ShaderProgram *
Canvas::
shadowCubeMapShaderProgram()
{
  return getShader("ShadowCubeMap", "shadow_cube_map.vs", "shadow_cube_map.gs",
                   "shadow_cube_map.fs");
}

ShaderProgram *
Canvas::
textureShaderProgram()
{
  return getShader("Texture", "texture.vs", "texture.fs");
}

ShaderProgram *
Canvas::
textureCubeMapShaderProgram()
{
  return getShader("Texture Cube Map", "texture_cube_map.vs", "texture_cube_map.fs");
}

ShaderProgram *
Canvas::
shaderProgram()
{
  if      (shaderType_ == ShaderType::SCENE)
    return sceneShaderProgram();
  else if (shaderType_ == ShaderType::SHADOW)
    return shadowShaderProgram();
  else if (shaderType_ == ShaderType::SHADOW_CUBE_MAP)
    return shadowCubeMapShaderProgram();
  else
    return nullptr;
}

//---

void
Canvas::
initializeGL()
{
  initializeOpenGLFunctions();

  //---

  textureBuffer_.texture = new CQGLTexture;
  textureBuffer_.texture->setFunctions(this);

  skyboxTextureBuffer_.texture = new CQGLTexture;
  skyboxTextureBuffer_.texture->setFunctions(this);

  shadowTextureBuffer_.texture = new CQGLTexture;
  shadowTextureBuffer_.texture->setFunctions(this);

  shadowCubeMapTextureBuffer_.texture = new CQGLTexture;
  shadowCubeMapTextureBuffer_.texture->setFunctions(this);
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
  light_ ->setAspect(aspect());
}

void
Canvas::
paintGL()
{
  if (invalid_) {
    addSkybox();

    addScene();

    invalid_ = false;
  }

  //---

  // set GL state
  enableDepthTest  ();
  enableCullFace   ();
  enableFrontFace  ();
  enablePolygonLine();

  glPointSize(pointSize());
  glLineWidth(lineWidth());

  //---

  shadowFarPlane_ = camera_->far();

  //---

  if (isShadowed()) {
    auto *light = this->light();

    if      (light->getType() == CGeomLight3DType::DIRECTIONAL) {
      // draw to shadow frame buffer/texure
      if (! shadowTextureBuffer_.texture->setShadow(1024, 1024))
        std::cerr << "Set shadow texture failed\n";

      auto shaderType = ShaderType::SHADOW;
      std::swap(shaderType_, shaderType);

      shadowTextureBuffer_.texture->bind();

      if (isLightBuffer())
        shadowTextureBuffer_.camera = light_;
      else
        shadowTextureBuffer_.camera = camera_;

      if (isSkybox())
        drawSkybox();

      drawScene(shadowTextureBuffer_.camera);

      std::swap(shaderType_, shaderType);

      shadowTextureBuffer_.texture->unbind();

#if 0
      CQGLTexture::MinMax minMax;
      shadowTextureBuffer_.texture->getTextureRange(minMax);
      minMax.print("shadow");
#endif
    }
    else if (light->getType() == CGeomLight3DType::POINT) {
      // draw to shadow cube map frame buffer/texure
      if (! shadowCubeMapTextureBuffer_.texture->setShadowCubeMap(1024, 1024))
        std::cerr << "Set shadow map texture failed\n";

      auto shaderType = ShaderType::SHADOW_CUBE_MAP;
      std::swap(shaderType_, shaderType);

      shadowCubeMapTextureBuffer_.texture->bind();

      auto *program = shadowCubeMapShaderProgram();

      std::vector<CMatrix3DH> shadowTransforms;

      if (isLightBuffer())
        shadowCubeMapTextureBuffer_.camera = light_;
      else
        shadowCubeMapTextureBuffer_.camera = camera_;

      connectSlots(false);
      auto fov = shadowCubeMapTextureBuffer_.camera->fov();
      shadowCubeMapTextureBuffer_.camera->setFov(90.0);

      auto shadowProj = shadowCubeMapTextureBuffer_.camera->worldMatrix();

      auto addShadowTransform = [&](const CVector3D &front, const CVector3D &up) {
        CMatrix3DH viewMatrix;

        auto lightPos = shadowCubeMapTextureBuffer_.camera->position();

        auto right = front.crossProduct(up);

        viewMatrix.setLookAt(lightPos.point(), front, up, right);

        shadowTransforms.push_back(shadowProj*viewMatrix);
      };

      addShadowTransform(CVector3D( 1.0,  0.0,  0.0), CVector3D(0.0, -1.0,  0.0)); // left
      addShadowTransform(CVector3D(-1.0,  0.0,  0.0), CVector3D(0.0, -1.0,  0.0)); // right
      addShadowTransform(CVector3D( 0.0,  1.0,  0.0), CVector3D(0.0,  0.0,  1.0)); // up
      addShadowTransform(CVector3D( 0.0, -1.0,  0.0), CVector3D(0.0,  0.0, -1.0)); // down
      addShadowTransform(CVector3D( 0.0,  0.0,  1.0), CVector3D(0.0, -1.0,  0.0)); // front
      addShadowTransform(CVector3D( 0.0,  0.0, -1.0), CVector3D(0.0, -1.0,  0.0)); // back

      program->bind();

      for (uint i = 0; i < 6; ++i) {
        auto name = QString("shadowMatrices[%1]").arg(i);

        program->setUniformValue(toCString(name), CQGLUtil::toQMatrix(shadowTransforms[i]));
      }

      program->release();

      if (isSkybox())
        drawSkybox();

      drawScene(shadowCubeMapTextureBuffer_.camera);

      std::swap(shaderType_, shaderType);

      shadowCubeMapTextureBuffer_.texture->unbind();

  #if 0
      CQGLTexture::MinMax minMax;
      shadowCubeMapTextureBuffer_.texture->getTextureCubeMapRange(minMax);
      minMax.print("shadow cube map");
  #endif

      shadowCubeMapTextureBuffer_.camera->setFov(fov);
      connectSlots(true);
    }
  }

  //---

  if (isTextureBuffer()) {
    // draw to buffer/texure
    if (! textureBuffer_.texture->setTarget(pixelWidth(), pixelHeight()))
      std::cerr << "Set target texture failed\n";

    textureBuffer_.texture->bind();

    glViewport(0, 0, pixelWidth(), pixelHeight());

    glClearColor(bgColor_.redF(), bgColor_.greenF(), bgColor_.blueF(), 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (isLightBuffer())
      textureBuffer_.camera = light_;
    else
      textureBuffer_.camera = camera_;

    if (isSkybox())
      drawSkybox();

    drawScene(textureBuffer_.camera);

    textureBuffer_.texture->unbind();
  }

  //---

  glViewport(0, 0, pixelWidth(), pixelHeight());

  glClearColor(bgColor_.redF(), bgColor_.greenF(), bgColor_.blueF(), 1.0f);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // draw skybox
  if (isSkybox())
    drawSkybox();

  // draw scene (unbuffered)
  drawScene(camera_);

  //---

  const int textureAreaSize = 256;

  // draw shadow texture (for debugging)
  if      (isShadowed()) {
    auto *light = this->light();

    if      (light->getType() == CGeomLight3DType::DIRECTIONAL) {
      glViewport(0, 0, textureAreaSize, textureAreaSize); //  bottom left

      drawTexture(shadowTextureBuffer_, /*isDepth*/true);
    }
    else if (light->getType() == CGeomLight3DType::POINT) {
      glViewport(0, 0, textureAreaSize, textureAreaSize); //  bottom left

      drawCubeMapTexture(shadowCubeMapTextureBuffer_, /*isDepth*/true);
    }
  }

  // draw skybox texture (for debugging)
  if (isSkybox()) {
    glViewport(0, pixelHeight() - textureAreaSize, textureAreaSize, textureAreaSize);

    cubeMapTextureBuffer_.texture = skyboxTextureBuffer_.texture;

    drawCubeMapTexture(cubeMapTextureBuffer_, /*isDepth*/false);
  }

  // draw texture buffer (for debugging)
  if (isTextureBuffer()) {
    glViewport(pixelWidth() - textureAreaSize, 0, textureAreaSize, textureAreaSize); // bottom right

    drawTexture(textureBuffer_, /*isDepth*/false);
  }
}

void
Canvas::
addSkybox()
{
  std::vector<std::string> images = {
    "cube_map/right.png",
    "cube_map/left.png",
    "cube_map/top.png",
    "cube_map/bottom.png",
    "cube_map/front.png",
    "cube_map/back.png"
  };

  std::vector<QImage> qimages;

  qimages.resize(6);

  for (int i = 0; i < 6; ++i) {
    qimages[i] = QImage(QString::fromStdString(images[i]));

    if (qimages[i].isNull())
      std::cerr << "Invalid image '" << images[i] << "'\n";
  }

  skyboxTextureBuffer_.texture->setCubemap();

  skyboxTextureBuffer_.texture->setCubemapImages(qimages);

  //---

  // add plane for texture
  if (! skyboxTextureBuffer_.buffer)
    skyboxTextureBuffer_.buffer = skyboxShaderProgram()->createBuffer();

  //---

  auto width = 1000.0;

  std::vector<CPoint3D> cubePoints = {
    CPoint3D(-1.0,  1.0, -1.0),
    CPoint3D(-1.0, -1.0, -1.0),
    CPoint3D( 1.0, -1.0, -1.0),
    CPoint3D( 1.0, -1.0, -1.0),
    CPoint3D( 1.0,  1.0, -1.0),
    CPoint3D(-1.0,  1.0, -1.0),

    CPoint3D(-1.0, -1.0,  1.0),
    CPoint3D(-1.0, -1.0, -1.0),
    CPoint3D(-1.0,  1.0, -1.0),
    CPoint3D(-1.0,  1.0, -1.0),
    CPoint3D(-1.0,  1.0,  1.0),
    CPoint3D(-1.0, -1.0,  1.0),

    CPoint3D( 1.0, -1.0, -1.0),
    CPoint3D( 1.0, -1.0,  1.0),
    CPoint3D( 1.0,  1.0,  1.0),
    CPoint3D( 1.0,  1.0,  1.0),
    CPoint3D( 1.0,  1.0, -1.0),
    CPoint3D( 1.0, -1.0, -1.0),

    CPoint3D(-1.0, -1.0,  1.0),
    CPoint3D(-1.0,  1.0,  1.0),
    CPoint3D( 1.0,  1.0,  1.0),
    CPoint3D( 1.0,  1.0,  1.0),
    CPoint3D( 1.0, -1.0,  1.0),
    CPoint3D(-1.0, -1.0,  1.0),

    CPoint3D(-1.0,  1.0, -1.0),
    CPoint3D( 1.0,  1.0, -1.0),
    CPoint3D( 1.0,  1.0,  1.0),
    CPoint3D( 1.0,  1.0,  1.0),
    CPoint3D(-1.0,  1.0,  1.0),
    CPoint3D(-1.0,  1.0, -1.0),

    CPoint3D(-1.0, -1.0, -1.0),
    CPoint3D(-1.0, -1.0,  1.0),
    CPoint3D( 1.0, -1.0, -1.0),
    CPoint3D( 1.0, -1.0, -1.0),
    CPoint3D(-1.0, -1.0,  1.0),
    CPoint3D( 1.0, -1.0,  1.0),
  };

  auto addPoint = [&](const CPoint3D &p) {
    skyboxTextureBuffer_.buffer->addPoint(width*p.x, width*p.y, width*p.z);
  };

  for (const auto &p : cubePoints)
    addPoint(p);

  skyboxTextureBuffer_.buffer->load();
}

void
Canvas::
drawSkybox()
{
  auto *program = skyboxShaderProgram();

  glDepthMask(GL_FALSE);
//glDepthFunc(GL_LEQUAL);
  glDisable(GL_CULL_FACE);

  skyboxTextureBuffer_.buffer->bind();

  program->bind();

  //---

#if 0
  cubemap_->enable(/*enable*/true);

  cubemap_->bind();
#else
  skyboxTextureBuffer_.texture->bind();
#endif

  glActiveTexture(GL_TEXTURE0);

  //---

  program->setUniformValue("textureId", 0);

  // pass projection matrix to shader
  auto worldMatrix = camera_->worldMatrix();
  program->setUniformValue("projection", CQGLUtil::toQMatrix(worldMatrix));

  // camera/view transformation
  auto viewMatrix = camera_->viewMatrix();
  program->setUniformValue("view", CQGLUtil::toQMatrix(viewMatrix));

  // model matric
  auto modelMatrix = CMatrix3DH::identity();
  program->setUniformValue("model", CQGLUtil::toQMatrix(modelMatrix));

  //---

  program->setUniformValue("isWireframe", false);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glDrawArrays(GL_TRIANGLES, 0, 36);

  //---

#if 0
  program->setUniformValue("isWireframe", true);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glDrawArrays(GL_TRIANGLES, 0, 36);
#endif

  //---

#if 0
  cubemap_->enable(/*enable*/false);

  cubemap_->unbind();
#else
  skyboxTextureBuffer_.texture->unbind();
#endif

  //---

  skyboxTextureBuffer_.buffer->unbind();

  program->release();

  //---

  glDepthMask(GL_TRUE);
//glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);
}

void
Canvas::
drawTexture(TextureBuffer &textureBuffer, bool isDepth)
{
  textureBuffer.shaderProgram = textureShaderProgram();

#if 0
  textureBuffer.shaderProgram->setUniformValue("shadowScale", float(shadowScale()));
#endif

  textureBuffer.shaderProgram->setUniformValue("near_plane", float(textureBuffer.camera->near()));
  textureBuffer.shaderProgram->setUniformValue("far_plane", float(textureBuffer.camera->far()));

  // add plane for texture
  if (! textureBuffer.buffer) {
    textureBuffer.buffer = textureBuffer.shaderProgram->createBuffer();

    textureBuffer.buffer->clearBuffers();

    textureBuffer.faceDataList.clear();

    //---

    struct PointData {
      CPoint2D p;
      CPoint2D tp;

      PointData() { }

      PointData(const CPoint2D &p1, const CPoint2D &p2) :
       p(p1), tp(p2) {
      }
    };

    auto addPoint = [&](const PointData &p) {
      textureBuffer.buffer->addPoint(p.p.x, p.p.y, 0.0);
      textureBuffer.buffer->addTexturePoint(p.tp.x, p.tp.y);
    };

    auto addPolygon = [&](const std::vector<PointData> &points) {
      FaceData faceData;

      faceData.pos = textureBuffer.faceDataList.pos;
      faceData.len = points.size();

      for (const auto &p : points) {
        addPoint(p);
      }

      textureBuffer.faceDataList.faceDatas.push_back(faceData);

      textureBuffer.faceDataList.pos += faceData.len;
    };

    auto addRect = [&](const QRectF &rect) {
      std::vector<PointData> points;

      points.push_back(PointData(CPoint2D(rect.left (), rect.top   ()), CPoint2D(0, 0)));
      points.push_back(PointData(CPoint2D(rect.right(), rect.top   ()), CPoint2D(1, 0)));
      points.push_back(PointData(CPoint2D(rect.right(), rect.bottom()), CPoint2D(1, 1)));
      points.push_back(PointData(CPoint2D(rect.left (), rect.bottom()), CPoint2D(0, 1)));

      addPolygon(points);
    };

    addRect(QRectF(-1, -1, 2, 2));

    textureBuffer.buffer->load();
  }

  //---

  glDisable(GL_DEPTH_TEST);

  textureBuffer.shaderProgram->bind();

  //---

  // model matrix
  auto modelMatrix = CMatrix3DH::identity();
  textureBuffer.shaderProgram->setUniformValue("model", CQGLUtil::toQMatrix(modelMatrix));

  textureBuffer.shaderProgram->setUniformValue("textureId", 0);

  textureBuffer.shaderProgram->setUniformValue("isDepth", isDepth);

  //---

  textureBuffer.buffer->bind();

  glEnable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);

  textureBuffer.texture->bindBuffer();

  for (const auto &faceData : textureBuffer.faceDataList.faceDatas) {
    glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
  }

  textureBuffer.texture->unbindBuffer();

  glDisable(GL_TEXTURE_2D);

  textureBuffer.buffer->unbind();

  //---

  textureBuffer.shaderProgram->release();

  glEnable(GL_DEPTH_TEST);
}

void
Canvas::
drawCubeMapTexture(TextureBuffer &textureBuffer, bool isDepth)
{
  textureBuffer.shaderProgram = textureCubeMapShaderProgram();

  // add plane for texture
  if (! textureBuffer.buffer) {
    textureBuffer.buffer = textureBuffer.shaderProgram->createBuffer();

    textureBuffer.buffer->clearBuffers();

    textureBuffer.faceDataList.clear();

    //---

    struct PointData {
      CPoint2D p;
      CPoint2D tp;

      PointData() { }

      PointData(const CPoint2D &p1, const CPoint2D &p2) :
       p(p1), tp(p2) {
      }
    };

    auto addPoint = [&](const PointData &p) {
      textureBuffer.buffer->addPoint(p.p.x, p.p.y, 0.0);
      textureBuffer.buffer->addTexturePoint(p.tp.x, p.tp.y);
    };

    auto addPolygon = [&](const std::vector<PointData> &points) {
      FaceData faceData;

      faceData.pos = textureBuffer.faceDataList.pos;
      faceData.len = points.size();

      for (const auto &p : points) {
        addPoint(p);
      }

      textureBuffer.faceDataList.faceDatas.push_back(faceData);

      textureBuffer.faceDataList.pos += faceData.len;
    };

    auto addRect = [&](const QRectF &rect) {
      std::vector<PointData> points;

      points.push_back(PointData(CPoint2D(rect.left (), rect.top   ()), CPoint2D(0, 0)));
      points.push_back(PointData(CPoint2D(rect.right(), rect.top   ()), CPoint2D(1, 0)));
      points.push_back(PointData(CPoint2D(rect.right(), rect.bottom()), CPoint2D(1, 1)));
      points.push_back(PointData(CPoint2D(rect.left (), rect.bottom()), CPoint2D(0, 1)));

      addPolygon(points);
    };

    addRect(QRectF(-1, -1, 2, 2));

    textureBuffer.buffer->load();
  }

  //---

  glDisable(GL_DEPTH_TEST);

  textureBuffer.shaderProgram->bind();

  //---

  // model matrix
  auto modelMatrix = CMatrix3DH::identity();
  textureBuffer.shaderProgram->setUniformValue("model", CQGLUtil::toQMatrix(modelMatrix));

  textureBuffer.shaderProgram->setUniformValue("textureId", 0);

  //---

  textureBuffer.buffer->bind();

  glEnable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);

  if (isDepth)
    textureBuffer.texture->bindBuffer();
  else
    textureBuffer.texture->bind();

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  for (const auto &faceData : textureBuffer.faceDataList.faceDatas) {
    glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
  }

  if (isDepth)
    textureBuffer.texture->unbindBuffer();
  else
    textureBuffer.texture->unbind();

  glDisable(GL_TEXTURE_2D);

  textureBuffer.buffer->unbind();

  //---

  textureBuffer.shaderProgram->release();

  glEnable(GL_DEPTH_TEST);
}

void
Canvas::
writeTexture(TextureBuffer &textureBuffer, const char *filename, bool /*isDepth*/)
{
  CQGLTexture::ImageData imageData;
  imageData.scale = true;
  imageData.debug = true;
  textureBuffer.texture->writeImage(filename, imageData);
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

        face->calcModelNormal(normal);
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

  auto dist = std::sqrt(2.0)*d;

  camera_->setOrigin(CVector3D(c));
  camera_->setDistance(dist);

  light_->setOrigin(CVector3D(c));
  light_->setPosition((light_->origin() + CVector3D(dist, dist, dist)).point());

  std::cerr << "Max Size " << bbox_ << "\n";
}

void
Canvas::
drawScene(CGLCameraIFace *camera)
{
  auto *program = this->shaderProgram();

  program->bind();

  //---

  program->setUniformValue("shadowMapDebug", app_->isShadowMapDebug());

  //---

#if 0
  program->setUniformValue("shadowScale", float(shadowScale()));
#endif

  // bias for directional light
  program->setUniformValue("shadowBias", float(shadowBias()));

  // for shadow perspective adjust
  program->setUniformValue("near_plane", float(camera->near()));
  program->setUniformValue("far_plane", float(camera->far()));

#if 1
  if      (shaderType_ == ShaderType::SCENE && isShadowed()) {
    auto *light = this->light();

    // draw scene using data from shadow buffer/texture (texture 4)
    if      (light->getType() == CGeomLight3DType::DIRECTIONAL) {
      glActiveTexture(GL_TEXTURE4);
      shadowTextureBuffer_.texture->bindBuffer();

      // shadow texture
      program->setUniformValue("shadowMap", 4);
      program->setUniformValue("shadowCubeMap", 5);

      program->setUniformValue("useShadowMap", true);
      program->setUniformValue("useShadowCubeMap", false);

      auto lightMatrix = light->worldMatrix()*light->viewMatrix();
      program->setUniformValue("lightSpaceMatrix", CQGLUtil::toQMatrix(lightMatrix));
    }
    // draw scene using data from shadow cube map buffer/texture (texture 4)
    else if (light->getType() == CGeomLight3DType::POINT) {
      glActiveTexture(GL_TEXTURE4);
      shadowCubeMapTextureBuffer_.texture->bindBuffer();

      // shadow cube map texture
      program->setUniformValue("shadowCubeMap", 4);
      program->setUniformValue("shadowMap", 5);

      program->setUniformValue("useShadowCubeMap", true);
      program->setUniformValue("useShadowMap", true);

      auto lightMatrix = light->worldMatrix()*light->viewMatrix();
      program->setUniformValue("lightSpaceMatrix", CQGLUtil::toQMatrix(lightMatrix));
    }
  }
  // draw scene from light to shadow buffer/texture
  else {
    program->setUniformValue("shadowMap", 4);
    program->setUniformValue("shadowCubeMap", 5);

    program->setUniformValue("useShadowMap", false);
    program->setUniformValue("useShadowCubeMap", false);

    auto lightMatrix = CMatrix3DH::identity();
    program->setUniformValue("lightSpaceMatrix", CQGLUtil::toQMatrix(lightMatrix));
  }
#else
  program->setUniformValue("useShadowMap", false);

  auto lightMatrix = CMatrix3DH::identity();
  program->setUniformValue("lightSpaceMatrix", CQGLUtil::toQMatrix(lightMatrix));
#endif

  //---

//auto worldMatrix = camera->perspectiveMatrix();
  auto worldMatrix = camera->worldMatrix();
  auto viewMatrix  = camera->viewMatrix();
  auto viewPos     = camera->position();

  // camera projection
  program->setUniformValue("projection", CQGLUtil::toQMatrix(worldMatrix));

  // camera/view transformation
  program->setUniformValue("view", CQGLUtil::toQMatrix(viewMatrix));

  // view pos
  program->setUniformValue("viewPos", CQGLUtil::toVector(viewPos));

  //---

  // add light data to shader program
  addShaderLights(program);

  //---

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

    if (shaderType_ == ShaderType::SHADOW ||
        shaderType_ == ShaderType::SHADOW_CUBE_MAP) {
      if (! object->getShadowed())
        continue;
    }

    //---

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

      //---

      if (useDiffuseTexture)
        faceData.diffuseTexture->unbind();

      if (useNormalTexture)
        faceData.normalTexture->unbind();

      if (useSpecularTexture)
        faceData.specularTexture->unbind();

      if (useEmissiveTexture)
        faceData.emissiveTexture->unbind();
    };

    //---

#if 0
    bool anyTransparent = false;
#endif

    //glDisable(GL_BLEND);
    //glDepthMask(GL_TRUE);

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

#if 1
  if (shaderType_ == ShaderType::SCENE && isShadowed()) {
    auto *light = this->light();

    // draw scene using data from shadow buffer/texture (texture 4)
    if      (light->getType() == CGeomLight3DType::DIRECTIONAL) {
      shadowTextureBuffer_.texture->unbindBuffer();
    }
    // draw scene using data from shadow cube map buffer/texture (texture 4)
    else if (light->getType() == CGeomLight3DType::POINT) {
      shadowCubeMapTextureBuffer_.texture->unbindBuffer();
    }
  }
#endif

  //---

  program->release();
}

void
Canvas::
addShaderLights(ShaderProgram *program)
{
  program->setUniformValue("ambientColor", CQGLUtil::toVector(ambientColor()));
  program->setUniformValue("ambientStrength", float(ambientStrength()));

  program->setUniformValue("diffuseStrength", float(diffuseStrength()));

  program->setUniformValue("specularColor"   , CQGLUtil::toVector(specularColor()));
  program->setUniformValue("specularStrength", float(specularStrength()));

  program->setUniformValue("emissionColor"   , CQGLUtil::toVector(emissiveColor()));
  program->setUniformValue("emissiveStrength", float(emissiveStrength()));

  program->setUniformValue("fixedDiffuse", isFixedDiffuse());

  //---

  auto *light = this->light();

  auto lightName = QString("lights[0]");

  program->setUniformValue(toCString(lightName + ".type"), int(light->getType()));

  program->setUniformValue(toCString(lightName + ".enabled"), true);

  program->setUniformValue(toCString(lightName + ".position"),
    CQGLUtil::toVector(light->getPosition()));

  program->setUniformValue(toCString(lightName + ".color"),
    CQGLUtil::toVector(light->getDiffuse()));

  if      (light->getType() == CGeomLight3DType::DIRECTIONAL) {
    program->setUniformValue(toCString(lightName + ".direction"),
      CQGLUtil::toVector(light->getDirection()));
  }
  else if (light->getType() == CGeomLight3DType::POINT) {
    program->setUniformValue(toCString(lightName + ".radius"), float(light->getPointRadius()));

    program->setUniformValue(toCString(lightName + ".attenuation0"),
      float(light->getConstantAttenuation()));
    program->setUniformValue(toCString(lightName + ".attenuation1"),
      float(light->getLinearAttenuation()));
    program->setUniformValue(toCString(lightName + ".attenuation2"),
      float(light->getQuadraticAttenuation()));
  }
  else if (light->getType() == CGeomLight3DType::SPOT) {
    program->setUniformValue(toCString(lightName + ".direction"),
      CQGLUtil::toVector(light->getSpotDirection()));

    auto cutOffCos      = std::cos(CMathGen::DegToRad(light->getSpotCutOffAngle()));
    auto outerCutOffCos = std::cos(CMathGen::DegToRad(light->getSpotOuterCutOffAngle()));

    program->setUniformValue(toCString(lightName + ".cutoff"), float(cutOffCos));
    program->setUniformValue(toCString(lightName + ".outerCutoff"), float(outerCutOffCos));

    program->setUniformValue(toCString(lightName + ".exponent"),
      float(light->getSpotExponent()));
  }
  else if (light->getType() == CGeomLight3DType::FLASHLIGHT) {
    // eye direction
    auto cutOffCos      = std::cos(CMathGen::DegToRad(light->getSpotCutOffAngle()));
    auto outerCutOffCos = std::cos(CMathGen::DegToRad(light->getSpotOuterCutOffAngle()));

    program->setUniformValue(toCString(lightName + ".cutoff"), float(cutOffCos));
    program->setUniformValue(toCString(lightName + ".outerCutoff"), float(outerCutOffCos));

    program->setUniformValue(toCString(lightName + ".exponent"),
      float(light->getSpotExponent()));
  }
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
  auto *camera = getEditCamera();

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

    camera->rotateY(-dx*da);
    camera->rotateX(-dy*da);
  }
  else if (mouseData_.button == Qt::RightButton) {
    camera->moveRight(-dx/10.0);
    camera->moveUp   ( dy/10.0);
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
  auto *camera = getEditCamera();

  auto d  = bbox_.getMaxSize()/100.0;
  auto dw = e->angleDelta().y()/250.0;

  auto *camera1 = dynamic_cast<Camera *>(camera);

  if (camera1)
    camera1->setDistance(camera1->distance() - dw*d);
}

void
Canvas::
keyPressEvent(QKeyEvent *e)
{
  auto *camera = getEditCamera();

  mouseData_.isShift   = (e->modifiers() & Qt::ShiftModifier);
  mouseData_.isControl = (e->modifiers() & Qt::ControlModifier);

  auto k = e->key();

  auto d  = bbox_.getMaxSize()/100.0;
  auto da = M_PI/60.0;

  if      (k == Qt::Key_Left) {
    camera->moveRight(-d);
  }
  else if (k == Qt::Key_Right) {
    camera->moveRight(d);
  }
  else if (k == Qt::Key_Up) {
    camera->moveUp(d);
  }
  else if (k == Qt::Key_Down) {
    camera->moveUp(-d);
  }
  else if (k == Qt::Key_Plus) {
    camera->moveFront(d);
  }
  else if (k == Qt::Key_Minus) {
    camera->moveFront(-d);
  }
  else if (k == Qt::Key_W) {
    camera->rotateX(da);
  }
  else if (k == Qt::Key_S) {
    camera->rotateX(-da);
  }
  else if (k == Qt::Key_A) {
    camera->rotateY(da);
  }
  else if (k == Qt::Key_D) {
    camera->rotateY(-da);
  }
  else if (k == Qt::Key_Q) {
    camera->rotateZ(-da);
  }
  else if (k == Qt::Key_E) {
    camera->rotateZ(da);
  }
  else if (k == Qt::Key_Space) {
    auto *camera1 = dynamic_cast<Camera *>(camera);

    if (camera1)
      camera1->printMatrices();
  }
  else if (k == Qt::Key_Question) {
    writeTexture(textureBuffer_, "buffer.png", /*isDepth*/false);
    writeTexture(shadowTextureBuffer_, "shadow.png", /*isDepth*/true);
    writeTexture(shadowCubeMapTextureBuffer_, "shadow_cube_map.png", /*isDepth*/true);
  }
}

CGLCameraIFace *
Canvas::
getEditCamera() const
{
  auto editType = this->editType();

  if      (editType == EditType::CAMERA)
    return camera_;
  else if (editType == EditType::LIGHT)
    return light_;
  else
    return nullptr;
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
getShader(const QString &name, const QString &vertexFile, const QString &fragmentFile)
{
  auto id = QString("V:%1,F:%2").arg(vertexFile).arg(fragmentFile);

  auto ps = shaders_.find(id);

  if (ps == shaders_.end()) {
    auto *shaderProgram = new ShaderProgram(app_, name);

    shaderProgram->setId(id);

    shaderProgram->addShaders(vertexFile, fragmentFile);

    ps = shaders_.insert(ps, Shaders::value_type(id, shaderProgram));
  }

  return (*ps).second;
}

ShaderProgram *
Canvas::
getShader(const QString &name, const QString &vertexFile, const QString &geometryFile,
          const QString &fragmentFile)
{
  auto id = QString("V:%1,G:%2,F:%3").arg(vertexFile).arg(geometryFile).arg(fragmentFile);

  auto ps = shaders_.find(id);

  if (ps == shaders_.end()) {
    auto *shaderProgram = new ShaderProgram(app_, name);

    shaderProgram->setId(id);

    shaderProgram->addShaders(vertexFile, fragmentFile);

    shaderProgram->addGeometryShader(geometryFile);

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
