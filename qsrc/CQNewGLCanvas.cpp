#include <CQNewGLCanvas.h>
#include <CQNewGLModel.h>
#include <CQNewGLFont.h>
#include <CQNewGLCamera.h>
#include <CQNewGLLight.h>
#include <CQNewGLAxes.h>
#include <CQNewGLBBox.h>
#include <CQNewGLNormals.h>
#include <CQNewGLHull.h>
#include <CQNewGLTerrain.h>
#include <CQNewGLMaze.h>
#include <CQNewGLSkybox.h>
#include <CQNewGLEmitter.h>
#include <CQNewGLFractal.h>
#include <CQNewGLDrawTree.h>
#include <CQNewGLShape.h>
#include <CQNewGLPath.h>
#include <CQNewGLToolbar.h>
#include <CQNewGLUtil.h>

#include <CImportBase.h>
#include <CQGLBuffer.h>
#include <CQGLTexture.h>
#include <CGeometry3D.h>
#include <CGeomScene3D.h>
#include <CGeomTexture.h>
#include <CGeomCone3D.h>
#include <CGeomCylinder3D.h>
#include <CGeomSphere3D.h>
#include <CQGLUtil.h>
#include <CFile.h>
#include <CSphere3D.h>
#include <CStrUtil.h>

#include <QOpenGLShaderProgram>
#include <QMouseEvent>
#include <QTimer>

#include <GL/glu.h>

//---

CQNewGLCanvas::
CQNewGLCanvas(CQNewGLModel *app) :
 QGLWidget(), app_(app)
{
  setFocusPolicy(Qt::StrongFocus);

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  //---

  scene_ = CGeometryInst->createScene3D();

  //---

  timer_ = new QTimer;

  connect(timer_, &QTimer::timeout, this, &CQNewGLCanvas::timerSlot);
}

void
CQNewGLCanvas::
setFlipYZ(bool b)
{
  flipYZ_ = b;

  updateObjectsData();
  update();
}

void
CQNewGLCanvas::
setShowBasis(bool b)
{
  basisData_.show = b;

  update();
}

void
CQNewGLCanvas::
setInvertDepth(bool b)
{
  invertDepth_ = b;

  for (auto *light : lights()) {
    if (isInvertDepth())
      light->flipYZ();
  }

  updateObjectsData();
  update();
}

void
CQNewGLCanvas::
setOrtho(bool b)
{
  ortho_ = b;

  update();
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
setAmbientColor(const QColor &c)
{
  ambientColor_ = c;

  update();
}

void
CQNewGLCanvas::
setDiffuseColor(const QColor &c)
{
  diffuseColor_ = c;

  updateObjectsData();
  update();
}

void
CQNewGLCanvas::
setEmissionColor(const QColor &c)
{
  emissionColor_ = c;

  updateObjectsData();
  update();
}

void
CQNewGLCanvas::
setDepthTest(bool b)
{
  depthTest_ = b;

  enableDepthTest();
}

void
CQNewGLCanvas::
enableDepthTest()
{
  if (depthTest_)
    glEnable(GL_DEPTH_TEST);
  else
    glDisable(GL_DEPTH_TEST);
}

void
CQNewGLCanvas::
setCullFace(bool b)
{
  cullFace_ = b;

  enableCullFace();
}

void
CQNewGLCanvas::
enableCullFace()
{
  if (cullFace_)
    glEnable(GL_CULL_FACE);
  else
    glDisable(GL_CULL_FACE);
}

void
CQNewGLCanvas::
setFrontFace(bool b)
{
  frontFace_ = b;

  enableFrontFace();
}

void
CQNewGLCanvas::
enableFrontFace()
{
  if (frontFace_)
    glFrontFace(GL_CW);
  else
    glFrontFace(GL_CCW);
}

void
CQNewGLCanvas::
setPolygonSolid(bool b)
{
  polygonSolid_ = b;
}

void
CQNewGLCanvas::
setPolygonLine(bool b)
{
  polygonLine_ = b;

  enablePolygonLine();
}

void
CQNewGLCanvas::
enablePolygonLine()
{
  if (polygonLine_)
    glEnable(GL_POLYGON_OFFSET_LINE);
  else
    glDisable(GL_POLYGON_OFFSET_LINE);

  glPolygonOffset(-1.0f, -1.0f);
}

void
CQNewGLCanvas::
setShowBonePoints(bool b)
{
  showBonePoints_ = b;
}

//---

void
CQNewGLCanvas::
setShowNormals(bool b)
{
  showNormals_ = b;
}

double
CQNewGLCanvas::
calcNormalsSize() const
{
  if (normalsSize_ <= 0.0)
    return sceneScale_/100.0;

  return normalsSize_;
}

void
CQNewGLCanvas::
setNormalsSize(double r)
{
  normalsSize_ = r;
}

void
CQNewGLCanvas::
setNormalsColor(const QColor &c)
{
  normalsColor_ = c;
}

void
CQNewGLCanvas::
setTangentSpaceNormal(bool b)
{
  tangentSpaceNormal_ = b;
}

//---

void
CQNewGLCanvas::
setCurrentObject(CGeomObject3D *object)
{
  int i = 0;

  for (auto &po : objectDatas_) {
    auto *objectData = po.second;

    if (objectData->object == object) {
      setCurrentObjectNum(i);
      return;
    }

    ++i;
  }
}

void
CQNewGLCanvas::
setCurrentObjectNum(int i)
{
  currentObjectNum_ = i;

  updateObjectsData();
  update();

  Q_EMIT currentObjectChanged();
}

CGeomObject3D *
CQNewGLCanvas::
getCurrentObject() const
{
  return getObject(currentObjectNum_);
}

CQNewGLCanvas::ObjectData *
CQNewGLCanvas::
getCurrentObjectData() const
{
  return getObjectData(currentObjectNum_);
}

CGeomObject3D *
CQNewGLCanvas::
getObject(int ind) const
{
  auto *objectData = getObjectData(ind);

  if (objectData)
    return objectData->object;

  return nullptr;
}

CQNewGLCanvas::ObjectData *
CQNewGLCanvas::
getObjectData(int ind) const
{
  int i = 0;

  for (auto &po : objectDatas_) {
    auto *objectData = po.second;

    if (i == ind)
      return objectData;

    ++i;
  }

  return nullptr;
}

CGeomObject3D *
CQNewGLCanvas::
getSelectedObject() const
{
  for (auto &po : objectDatas_) {
    auto *objectData = po.second;

    if (objectData->object->getSelected())
      return objectData->object;
  }

  return nullptr;
}

CGeomObject3D *
CQNewGLCanvas::
getBonesObject() const
{
  // TODO: bone for current object
  const auto &objects = scene_->getObjects();

  auto n = int(objects.size());

  for (int i = 0; i < n; ++i) {
    if (! objects[i]->getNodes().empty())
      return objects[i];
  }

  return nullptr;
}

//---

void
CQNewGLCanvas::
setShowCameras(bool b)
{
  showCameras_ = b;

  update();
}

void
CQNewGLCanvas::
setCurrentCamera(int i)
{
  currentCamera_ = i;

  Q_EMIT currentCameraChanged();
}

CQNewGLCamera *
CQNewGLCanvas::
getCurrentCamera() const
{
  return getCamera(currentCamera_);
}

CQNewGLCamera *
CQNewGLCanvas::
getCamera(int ind) const
{
  int ind1 = ind;

  if (ind1 < 0 || ind1 > int(cameras_.size()))
    ind1 = 0;

  return cameras_[ind1];
}

void
CQNewGLCanvas::
setCameraRotate(bool b)
{
  cameraRotate_ = b;

  getCurrentCamera()->setRotate(b);
}

void
CQNewGLCanvas::
setCameraRotateAt(const CGLCamera::RotateAt &at)
{
  cameraRotateAt_ = at;

  getCurrentCamera()->setRotateAt(at);
}

//---

void
CQNewGLCanvas::
setShowLights(bool b)
{
  showLights_ = b;

  update();
}

#if 0
CGLVector3D
CQNewGLCanvas::
lightPos(int i) const
{
  return lights_[i]->position();
}

QColor
CQNewGLCanvas::
lightColor(int i) const
{
  return lights_[i]->color();
}
#endif

void
CQNewGLCanvas::
setCurrentLight(int i)
{
  currentLight_ = i;

  Q_EMIT currentLightChanged();
}

CQNewGLLight *
CQNewGLCanvas::
getCurrentLight() const
{
  int currentLight = currentLight_;

  if (currentLight < 0 || currentLight >= int(lights_.size()))
    currentLight = 0;

  return lights_[currentLight];
}

//---

void
CQNewGLCanvas::
setTimerRunning(bool b)
{
  if (b != timerRunning_) {
    timerRunning_ = b;

    if (timerRunning_) {
      startTimer();

      timerSlot();
    }
    else
      stopTimer();
  }
}

void
CQNewGLCanvas::
startTimer()
{
  timer_->start(100);
}

void
CQNewGLCanvas::
stopTimer()
{
  timer_->stop();
}

void
CQNewGLCanvas::
timerSlot()
{
  Q_EMIT timerStep();
}

//---

bool
CQNewGLCanvas::
loadModel(const QString &fileName, CGeom3DType format, const LoadData &loadData)
{
  static uint modeInd;

  auto modelName = QString("Model.%1").arg(++modeInd);

  // import model
  auto *im = CImportBase::createModel(format, modelName.toStdString());

  if (! im) {
    std::cerr << "Invalid format\n";
    return false;
  }

  im->setInvertX(loadData.invertX);
  im->setInvertY(loadData.invertY);
  im->setInvertZ(loadData.invertZ);

  CFile file(fileName.toStdString());

  if (! im->read(file)) {
    delete im;
    std::cerr << "Failed to read model '" << fileName.toStdString() << "'\n";
    return false;
  }

  auto *scene = im->releaseScene();

  delete im;

  for (auto *object : scene->getObjects())
    scene_->addObject(object);

  Q_EMIT modelAdded();

  return true;
}

void
CQNewGLCanvas::
setInitTextures(const InitTextureDatas &textures)
{
  initTextures_ = textures;
}

void
CQNewGLCanvas::
setTextureMap(const std::string &map)
{
  textureMap_.clear();

  if (map == "")
    return;

  CFile file(map);

  while (! file.eof()) {
    std::string line;

    file.readLine(line);

    std::vector<std::string> words;

    CStrUtil::toWords(line, words);
    if (words.size() < 2) continue;

    TextureMapData textureData;

    textureData.diffuse = words[1];

    if (words.size() > 2)
      textureData.normal = words[2];

    if (words.size() > 3)
      textureData.specular = words[3];

    textureMap_[words[0]] = textureData;
  }
}

void
CQNewGLCanvas::
initializeGL()
{
  initialized_ = true;

  //---

  initializeOpenGLFunctions();

  glClearColor(bgColor_.redF(), bgColor_.greenF(), bgColor_.blueF(), 1.0f);

  //---

  enableDepthTest  ();
  enableCullFace   ();
  enableFrontFace  ();
  enablePolygonLine();

  //---

  modelShaderProgram_ = new CQNewGLShaderProgram(this);
  modelShaderProgram_->addShaders("model.vs", "model.fs");

  //---

  bonesShaderProgram_ = new CQNewGLShaderProgram(this);
  bonesShaderProgram_->addShaders("bones.vs", "bones.fs");

  //---

  boneShaderProgram_ = new CQNewGLShaderProgram(this);
  boneShaderProgram_->addShaders("bone.vs", "bone.fs");

  //---

  // lights
  addLight(/*update*/false);

  //---

  basisData_.shaderProgram = new CQNewGLShaderProgram(this);
  basisData_.shaderProgram->addShaders("basis.vs", "basis.fs");

  //---

  normalShaderProgram_ = new CQNewGLShaderProgram(this);
  normalShaderProgram_->addShaders("normal.vs", "normal.fs");

  //---

  loadInitTextures();

  //---

  font_ = new CQNewGLFont(this);

  font_->init();

  font_->setSize(48);
  font_->setFontName("OpenSans-Regular.ttf");

  //---

#if 0
  text_ = new CQNewGLText("Hello World");

  text_->setFont(font_);
  text_->setColor(CQNewGLFont::Color(1, 0, 0));
  text_->setPosition(CGLVector3D(1, 1, 1));
#endif

  //---

  axes_ = new CQNewGLAxes(this);
  axes_->setVisible(false);

  CQNewGLAxes::initShader(this);

  //---

  bbox_ = new CQNewGLBBox(this);
  bbox_->setVisible(false);

  CQNewGLBBox::initShader(this);

  //---

  normals_ = new CQNewGLNormals(this);
  normals_->setVisible(false);

  CQNewGLNormals::initShader(this);

  //---

  hull_ = new CQNewGLHull(this);
  hull_->setVisible(false);

  CQNewGLHull::initShader(this);

  //---

  terrain_ = new CQNewGLTerrain(this);

  CQNewGLTerrain::initShader(this);

  //---

  maze_ = new CQNewGLMaze(this);

  CQNewGLMaze::initShader(this);

  //---

  skybox_ = new CQNewGLSkybox(this);

  CQNewGLSkybox::initShader(this);

  //---

  fractal_ = new CQNewGLFractal(this);

  CQNewGLFractal::initShader(this);

  //---

  drawTree_ = new CQNewGLDrawTree(this);

  CQNewGLDrawTree::initShader(this);

  //---

  CQNewGLShape::initShader(this);

  //---

  CQNewGLPath::initShader(this);

  //---

  updateObjectsData();

  //---

  // cameras
  addCamera(/*update*/false);

  //---

  updateCameraBuffer();

  updateLightBuffer();
}

void
CQNewGLCanvas::
addCamera(bool update)
{
  CQNewGLCamera::initShader(this);

  //---

  auto f  = std::sqrt(2.0);
  auto f1 = f*sceneScale_;

  //---

  CGLVector3D position, up;
  QColor      color;

  auto ic = cameras_.size();

#if 0
  double x = 0;

  if (ic > 0) {
    auto ic2 = (ic + 1)/2;

    if (ic & 1)
      x -= ic2*sceneScale_;
    else
      x += ic2*sceneScale_;
  }

  position = CGLVector3D(x, f1, f1);
#else
  auto ic1 = ic % 4;

  if      (ic1 == 0) {
    position = CGLVector3D(f1, f1, f1);
    up       = CGLVector3D(0, 1, 0);
    color    = QColor(255, 255, 255);
  }
  else if (ic1 == 1) {
    position = CGLVector3D(f1, sceneCenter_.y, sceneCenter_.z);
    up       = CGLVector3D(0, 1, 0);
    color    = QColor(255, 0, 0);
  }
  else if (ic1 == 2) {
    position = CGLVector3D(sceneCenter_.x, f1, sceneCenter_.z);
    up       = CGLVector3D(0, 0, 1);
    color    = QColor(0, 255, 0);
  }
  else if (ic1 == 3) {
    position = CGLVector3D(sceneCenter_.x, sceneCenter_.y, f1);
    up       = CGLVector3D(0, 1, 0);
    color    = QColor(0, 0, 255);
  }
#endif

  //---

  auto origin = CGLVector3D(sceneCenter_.x, sceneCenter_.y, sceneCenter_.z);

  auto *camera = new CQNewGLCamera(this, origin, position, up);

  camera->setName(QString("Camera.%1").arg(cameras_.size() + 1));

  camera->setColor(color);

  camera->setLastPos(float(app_->windowWidth())/2.0f, float(app_->windowHeight())/2.0f);

  cameras_.push_back(camera);

  //---

  Q_EMIT cameraAdded();

  //---

  if (update)
    updateCameraBuffer();
}

void
CQNewGLCanvas::
resetCamera()
{
  auto *camera = cameras_[0];

  auto *object = getCurrentObject();

  auto f = std::sqrt(2.0);

  CGLVector3D position, origin;

  if (object) {
    auto bbox = getObjectBBox(object);

    auto c = bbox.getCenter();
    auto s = bbox.getMaxSize();

    auto f1 = f*s;

    position = CGLVector3D(f1, f1, f1);
    origin   = CGLVector3D(c.x, c.y, c.z);
  }
  else {
    auto f1 = f*sceneScale_;

    position = CGLVector3D(f1, f1, f1);
    origin   = CGLVector3D(sceneCenter_.x, sceneCenter_.y, sceneCenter_.z);
  }

  camera->setOrigin(origin);
  camera->setPosition(position);
}

void
CQNewGLCanvas::
addLight(bool update)
{
  CQNewGLLight::initShader(this);

  //---

  auto lightRadius = 4*sceneScale_;

  //--

  auto il = lights_.size();

  double x = 0;

  if (il > 0) {
    auto il2 = (il + 1)/2;

    if (il & 1)
      x -= il2;
    else
      x += il2;
  }

  //---

  auto *light = new CQNewGLLight(this);

  light->setName    (QString("Light%1").arg(il + 1));
  light->setPosition(CGLVector3D(x*lightRadius, lightRadius, lightRadius));

  if (il == 0)
    light->setType(CQNewGLLight::Type::DIRECTIONAL);
  else
    light->setType(CQNewGLLight::Type::POINT);

  lights_.push_back(light);

  //---

  if (update)
    updateLightBuffer();

  Q_EMIT lightAdded();
}

bool
CQNewGLCanvas::
createFontTexture(uint *texture, int w, int h, uchar *data)
{
  //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // allocate texture id
  glGenTextures(1, texture);
  if (! checkError("glGenTextures")) return false;

  // set texture type
  glBindTexture(GL_TEXTURE_2D, *texture);
  if (! checkError("glGenTexture")) return false;

#if 0
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  if (! checkError("glTexParameteri")) return false;

#if 0
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
#else
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#endif
  if (! checkError("glTexParameteri")) return false;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);

  glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
  if (! checkError("glTexParameteri")) return false;
#else
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  if (! checkError("glPixelStorei")) return false;
#endif

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, data);
  if (! checkError("glTexImage2D")) return false;

  glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
  if (! checkError("glHint")) return false;

  glGenerateMipmap(GL_TEXTURE_2D);
  if (! checkError("glGenerateMipmap")) return false;

  //---

#if 0
  // dump font to png
  auto src = CImageNoSrc();

  auto image = CImageMgrInst->createImage(src);

  image->setDataSize(w, h);

  int ii = 0;

  for (int iy = 0; iy < h; ++iy) {
    for (int ix = 0; ix < w; ++ix, ++ii) {
      image->setRGBAPixel(ix, iy, 1.0, 1.0, 1.0, data[ii]/255.0);
    }
  }

  CFile file("texture.png");

  image->writePNG(&file);
#endif

  return true;
}

void
CQNewGLCanvas::
loadInitTextures()
{
  auto loadTexture = [&](const QString &name, bool flipV=false) {
    CImageFileSrc src(name.toStdString());
    auto image = CImageMgrInst->createImage(src);

    if (flipV)
      image->flipH();

    auto *texture1 = CGeometryInst->createTexture(image);

    texture1->setName(name.toStdString());

    (void) getTexture(texture1, /*add*/true);
  };

  //---

  auto defNames = QStringList() << "Marble_Texture.jpg";

  auto texturesDir = app()->buildDir() + "/textures";

  for (const auto &name : defNames)
    loadTexture(texturesDir + "/" + name);

  //---

  for (const auto &textureData : initTextures_)
    loadTexture(QString::fromStdString(textureData.name), textureData.flipV);

  //---

  for (auto *object : scene_->getObjects()) {
    auto name = object->getName();

    auto pt = textureMap_.find(name);
    if (pt == textureMap_.end())
      continue;

    const auto &textureData = (*pt).second;

    auto *texture = getGeomTextureByName(textureData.diffuse);

    if (texture)
      object->setDiffuseTexture(texture);

    if (textureData.normal != "") {
      auto *texture = getGeomTextureByName(textureData.normal);

      if (texture)
        object->setNormalTexture(texture);
    }

    if (textureData.specular != "") {
      auto *texture = getGeomTextureByName(textureData.specular);

      if (texture)
        object->setSpecularTexture(texture);
    }

    if (textureData.emissive != "") {
      auto *texture = getGeomTextureByName(textureData.emissive);

      if (texture)
        object->setEmissiveTexture(texture);
    }
  }
}

//---

void
CQNewGLCanvas::
updateCameraBuffer()
{
  for (auto *camera : cameras_)
    camera->addGeometry();
}

void
CQNewGLCanvas::
drawCameras()
{
//auto *currentCamera = getCurrentCamera();

  CQNewGLShaderProgram *program = nullptr;

  for (auto *camera : cameras_) {
//  if (camera == currentCamera)
//    continue;

    if (! program) {
      program = camera->shaderProgram();

      program->bind();
    }

    camera->drawGeometry();
  }

  if (program)
    program->release();
}

//---

void
CQNewGLCanvas::
updateLightBuffer()
{
  for (auto *light : lights())
    light->addGeometry();
}

void
CQNewGLCanvas::
drawLights()
{
  CQNewGLShaderProgram *program = nullptr;

  for (auto *light : lights()) {
    if (! program) {
      auto *program = light->shaderProgram();

      program->bind();
    }

    light->drawGeometry();
  }

  if (program)
    program->release();
}

//---

void
CQNewGLCanvas::
updateTerrain()
{
  if (terrain_)
    terrain_->addGeometry();
}

void
CQNewGLCanvas::
drawTerrain()
{
  if (terrain_)
    terrain_->drawGeometry();
}

//---

void
CQNewGLCanvas::
updateMaze()
{
  if (maze_)
    maze_->addGeometry();
}

void
CQNewGLCanvas::
drawMaze()
{
  if (maze_)
    maze_->drawGeometry();
}

//---

void
CQNewGLCanvas::
updateSkybox()
{
  if (skybox_)
    skybox_->addGeometry();
}

void
CQNewGLCanvas::
drawSkybox()
{
  if (skybox_)
    skybox_->drawGeometry();
}

//---

void
CQNewGLCanvas::
addEmitter()
{
  if (emitters_.empty())
    CQNewGLEmitter::initShader(this);

  auto *emitter = new CQNewGLEmitter(this);

  emitters_.push_back(emitter);

  emitterNum_ = emitters_.size() - 1;

  emitter->setName(QString("Emitter.%1").arg(emitters_.size()));
}

CQNewGLEmitter *
CQNewGLCanvas::
getCurrentEmitter() const
{
  if (emitterNum_ < 0 || emitterNum_ >= int(emitters_.size()))
    return nullptr;

  return emitters_[emitterNum_];
}

void
CQNewGLCanvas::
updateEmitters()
{
  for (auto *emitter : emitters_) {
    if (! emitter->isEnabled())
      continue;

    emitter->addGeometry();
  }
}

void
CQNewGLCanvas::
drawEmitters()
{
  for (auto *emitter : emitters_) {
    if (! emitter->isEnabled())
      continue;

    emitter->drawGeometry();
  }
}

//---

void
CQNewGLCanvas::
updateFractal()
{
  if (fractal_)
    fractal_->addGeometry();
}

void
CQNewGLCanvas::
drawFractal()
{
  if (fractal_)
    fractal_->drawGeometry();
}

//---

void
CQNewGLCanvas::
updateDrawTree()
{
  if (drawTree_)
    drawTree_->addGeometry();
}

void
CQNewGLCanvas::
drawDrawTree()
{
  if (drawTree_)
    drawTree_->drawGeometry();
}

//---

void
CQNewGLCanvas::
setCurrentShape(int i)
{
  currentShape_ = i;

  Q_EMIT currentShapeChanged();
}

CQNewGLShape *
CQNewGLCanvas::
getCurrentShape() const
{
  if (shapes_.empty())
    return nullptr;

  int currentShape = currentShape_;

  if (currentShape < 0 || currentShape >= int(shapes_.size()))
    currentShape = 0;

  return shapes_[currentShape];
}

CQNewGLShape *
CQNewGLCanvas::
addShape()
{
  auto *shape = new CQNewGLShape(this);

  shapes_.push_back(shape);

  shape->setName(QString("Shape.%1").arg(shapes_.size()));
  shape->setActive(true);

  Q_EMIT shapeAdded();

  return shape;
}

void
CQNewGLCanvas::
drawShapes()
{
  for (auto *shape : shapes_) {
    if (shape->isVisible())
      shape->drawGeometry();
  }
}

//---

void
CQNewGLCanvas::
clearEyeLines()
{
  delete eyeLine1_;
  delete eyeLine2_;

  eyeLine1_ = nullptr;
  eyeLine2_ = nullptr;
}

void
CQNewGLCanvas::
drawPaths()
{
  if (! isShowEyeline())
    return;

  if (eyeLine1_)
    eyeLine1_->drawGeometry();

  if (eyeLine2_)
    eyeLine2_->drawGeometry();

  for (auto *path : paths_)
    path->drawGeometry();
}

//---

void
CQNewGLCanvas::
updateObjectsData()
{
  if (! initialized_)
    return;

  if      (app_->isShowBone())
    updateBoneData();
  else if (isBonesEnabled())
    updateBonesData();
  else
    updateModelData();

//updateAxes();
  axesNeedsUpdate_ = true;

  if (bbox_->isVisible())
    updateBBox();

  if (normals_->isVisible())
    updateNormals();

  if (hull_->isVisible())
    updateHull();

//updateBasis();
  basisData_.needsUpdate = true;

//text_->updateText();
}

void
CQNewGLCanvas::
updateObjectData(CGeomObject3D *)
{
  updateObjectsData();
}

void
CQNewGLCanvas::
updateModelData()
{
  // set up vertex data (and buffer(s)) and configure vertex attributes
  if (scene_->getObjects().empty()) {
    sceneScale_    = 1.0;
    invSceneScale_ = 1.0;
    return;
  }

  //---

  CBBox3D bbox;

//CBBox3D bbox1;
//scene_->getBBox(bbox1);

  //---

  bool useBones       = (isAnimEnabled() && animName() != "");
  bool showNormals    = isShowNormals();
  bool showBonePoints = (useBones && isShowBonePoints());

  int    boneNodeIds[4];
  double boneWeights[4];

  auto normalsSize  = calcNormalsSize();
  auto normalsColor = this->normalsColor();

  //---

  const auto &objects = scene_->getObjects();

  for (auto *object : objects) {
    ObjectData *objectData { nullptr };

    auto pd = objectDatas_.find(object);

    if (pd == objectDatas_.end())
      pd = objectDatas_.insert(pd, ObjectDatas::value_type(object, new ObjectData));

    objectData = (*pd).second;

    if (! objectData->object)
      objectData->object = object;

    //---

    if (! objectData->modelData.buffer)
      objectData->modelData.buffer = modelShaderProgram_->createBuffer();

    if (showNormals) {
      if (! objectData->normalData.buffer)
        objectData->normalData.buffer = normalShaderProgram_->createBuffer();
    }

    //---

#if 1
    auto meshMatrix = getMeshGlobalTransform(objectData, /*invert*/false);
#else
    auto meshMatrix = CMatrix3D::identity();

    if (useBones)
      meshMatrix = getMeshGlobalTransform(objectData, /*invert*/true);
#endif

    //---

    objectData->modelData.faceDatas.clear();

    auto *modelBuffer = objectData->modelData.buffer;

    modelBuffer->clearBuffers();

    //---

    CQGLBuffer *normalBuffer = nullptr;

    if (showNormals) {
      normalBuffer = objectData->normalData.buffer;

      normalBuffer->clearBuffers();
    }

    //---

    auto *rootObject = getRootObject(object);

    const auto &nodes = rootObject->getNodes();

    int nodeId = -1;

    for (const auto &pn : nodes) {
      const auto &node = pn.second;
      if (node.valid) {
        nodeId = pn.first;
        break;
      }
    }

    //---

    if (showBonePoints)
      (void) rootObject->updateNodesAnimationData(animName().toStdString(), time());

    //---

    auto *diffuseTexture  = object->getDiffuseTexture();
    auto *specularTexture = object->getSpecularTexture();
    auto *normalTexture   = object->getNormalTexture();
    auto *emissiveTexture = object->getEmissiveTexture();

    //---

    const auto &faces = object->getFaces();

    int pos = 0;

    for (const auto *face : faces) {
      CQNewGLFaceData faceData;

      faceData.boneId = nodeId;

      //---

      const auto &color = face->color().value_or(QColorToRGBA(diffuseColor()));

      //---

      // set face textures
      auto *diffuseTexture1 = face->getDiffuseTexture();
      if (! diffuseTexture1) diffuseTexture1 = diffuseTexture;

      auto *specularTexture1 = face->getSpecularTexture();
      if (! specularTexture1) specularTexture1 = specularTexture;

      auto *normalTexture1 = face->getNormalTexture();
      if (! normalTexture1) normalTexture1 = normalTexture;

      auto *emissiveTexture1 = face->getEmissiveTexture();
      if (! emissiveTexture1) emissiveTexture1 = emissiveTexture;

      if (diffuseTexture1)
        faceData.diffuseTexture = getTexture(diffuseTexture1, /*add*/true);

      if (specularTexture1)
        faceData.specularTexture = getTexture(specularTexture1, /*add*/true);

      if (normalTexture1)
        faceData.normalTexture = getTexture(normalTexture1, /*add*/true);

      if (emissiveTexture1)
        faceData.emissiveTexture = getTexture(emissiveTexture1, /*add*/true);

      //---

      // set face material

//    const auto &material = face->getMaterial();

//    faceData.ambient   = material.getAmbient ();
//    faceData.diffuse   = material.getDiffuse ();
//    faceData.specular  = material.getSpecular();
      faceData.emission  = face->emission().value_or(QColorToRGBA(emissionColor()));
      faceData.shininess = face->shininess().value_or(app_->shininess());

      //---

      // set face normal
      CVector3D normal;

      if (face->getNormalSet())
        normal = face->getNormal();
      else
        face->calcNormal(normal);

      //---

      const auto &vertices = face->getVertices();

      faceData.pos = pos;
      faceData.len = int(vertices.size());

      int iv = 0;

      for (const auto &v : vertices) {
        auto &vertex = object->getVertex(v);

        const auto &model = vertex.getModel();

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
          const auto &tpoint = face->getTexturePoint(vertex, iv);

          int tw = faceData.normalTexture->getWidth ();
          int th = faceData.normalTexture->getHeight();

          auto tx = CMathUtil::clamp(tpoint.x, 0.0, 1.0);
          auto ty = CMathUtil::clamp(tpoint.y, 0.0, 1.0);

          // get normal value from texture
          auto rgba = faceData.normalTexture->getImage().pixel(tx*(tw - 1), ty*(th - 1));
          auto tnormal = CVector3D(qRed(rgba)/255.0, qGreen(rgba)/255.0, qBlue(rgba)/255.0);

          // this normal is in tangent space
          if (isTangentSpaceNormal())
            normal1 = (tnormal*2.0 - CVector3D(1.0, 1.0, 1.0)).normalized();
          else
            normal1 = tnormal;
        }

        //---

        auto calcPoint = [&](const CPoint3D &p, CPoint3D &p1, CPoint3D &p2) {
          auto zSign = (isInvertDepth() ? -1 : 1);

          if (! isFlipYZ())
            p1 = CPoint3D(p.x, p.y, zSign*p.z);
          else
            p1 = CPoint3D(p.x, p.z, zSign*p.y);

          p2 = meshMatrix*p1;
        };

        auto calcNormal = [&](const CVector3D &n) {
          auto zSign = (isInvertDepth() ? -1 : 1);

          CVector3D n1;

          if (! isFlipYZ())
            n1 = CVector3D(n.getX(), n.getY(), zSign*n.getZ());
          else
            n1 = CVector3D(n.getX(), n.getZ(), zSign*n.getY());

          return n1;
        };

        auto addPointNormal = [&](const CPoint3D &p, const CVector3D &n) {
          auto n1 = calcNormal(n);

//        auto p3 = p + normalsSize*n2.normalize();
          auto p3 = p + normalsSize*n1.normalize();

          normalBuffer->addPoint(float(p.x), float(p.y), float(p.z));
          normalBuffer->addColor(normalsColor);

          normalBuffer->addPoint(float(p3.x), float(p3.y), float(p3.z));
          normalBuffer->addColor(normalsColor);
        };

        auto addPoint = [&](const CPoint3D &p, const CVector3D &n) {
          CPoint3D p1, p2;
          calcPoint(p, p1, p2);

          auto p11 = (useBones ? p1 : p2);

          if (showBonePoints) {
            p11 = applyBonePointTransform(p1, boneNodeIds, boneWeights);
          }

          modelBuffer->addPoint(float(p11.x), float(p11.y), float(p11.z));

          //---

          if (showNormals)
            addPointNormal(p11, n);
        };

        auto addNormal = [&](const CVector3D &n) {
          auto n1 = calcNormal(n);

          modelBuffer->addNormal(float(n1.getX()), float(n1.getY()), float(n1.getZ()));
        };

        //---

        const auto &jointData = vertex.getJointData();

        if (jointData.nodeDatas[0].node >= 0) {
          for (int i = 0; i < 4; ++i) {
            boneNodeIds[i] = jointData.nodeDatas[i].node;
            boneWeights[i] = jointData.nodeDatas[i].weight;
          }

          modelBuffer->addBoneIds    (boneNodeIds[0], boneNodeIds[1],
                                      boneNodeIds[2], boneNodeIds[3]);
          modelBuffer->addBoneWeights(boneWeights[0], boneWeights[1],
                                      boneWeights[2], boneWeights[3]);
        }

        //---

        addPoint(model, normal1);

        //---

        addNormal(normal1);

        //---

        modelBuffer->addColor(color1);

        //---

        if (faceData.diffuseTexture) {
          const auto &tpoint = face->getTexturePoint(vertex, iv);

          modelBuffer->addTexturePoint(float(tpoint.x), float(tpoint.y));
        }
        else
          modelBuffer->addTexturePoint(0.0f, 0.0f);

        //---

        ++iv;
      }

      pos += faceData.len;

      objectData->modelData.faceDatas.push_back(faceData);

      ++iv;
    }

    //---

    modelBuffer->load();

    if (showNormals)
      normalBuffer->load();

    //---

    if (! object->parent()) {
      auto bbox1 = getObjectBBox(object);

      bbox += bbox1;
    }
  }

  //---

  setSceneBBox(bbox);

  //std::cerr << "Scene Scale : " << sceneScale_ << "\n";

#if 0
  // move to center, add scale to unit cube
  modelTranslate_.setX(-sceneCenter_.getX());
  modelTranslate_.setY(-sceneCenter_.getY());
  modelTranslate_.setZ(-sceneCenter_.getZ());

  modelScale_.setX(invSceneScale_);
  modelScale_.setY(invSceneScale_);
  modelScale_.setZ(invSceneScale_);
#endif
}

void
CQNewGLCanvas::
setSceneBBox(const CBBox3D &bbox)
{
  sceneSize_   = bbox.getSize();
  sceneCenter_ = bbox.getCenter();

  std::cerr << "Scene Size : " << sceneSize_.getX() << " " <<
               sceneSize_.getY() << " " << sceneSize_.getZ() << "\n";
  std::cerr << "Scene Center : " << sceneCenter_.getX() << " " <<
               sceneCenter_.getY() << " " << sceneCenter_.getZ() << "\n";

  sceneScale_    = max3(sceneSize_.getX(), sceneSize_.getY(), sceneSize_.getZ());
  invSceneScale_ = (sceneScale_ > 0.0 ? 1.0/sceneScale_ : 1.0);

  //std::cerr << "Scene Scale : " << sceneScale_ << "\n";
}

CPoint3D
CQNewGLCanvas::
applyBonePointTransform(const CPoint3D &p, int *boneIds, double *boneWeights) const
{
  auto result = CPoint3D(0, 0, 0);

  for (int i = 0; i < 4; ++i) {
    auto boneTransform = paintData_.nodeMatrices[boneIds[i]];

    result += boneWeights[i]*(boneTransform*p);
  }

  return result;
}

CPoint3D
CQNewGLCanvas::
applyBoneTransform(const CPoint3D &p, int boneId) const
{
  return paintData_.nodeMatrices[boneId]*p;
}

void
CQNewGLCanvas::
updateBonesData()
{
  // set up vertex data (and buffer(s)) and configure vertex attributes
  if (scene_->getObjects().empty()) {
    sceneScale_    = 1.0;
    invSceneScale_ = 1.0;
    return;
  }

  //---

  CBBox3D bbox;

//CBBox3D bbox1;
//scene_->getBBox(bbox1);

  //---

  auto color = CRGBA(1.0, 0.0, 0.0);

  //---

  // transform node to model coords
  auto transformNode = [&](const CGeomObject3D::NodeData &nodeData) {
    CMatrix3D m;

    if      (bonesTransform() == BonesTransform::INVERSE_BIND)
      m = nodeData.inverseBindMatrix.inverse();
    else if (bonesTransform() == BonesTransform::LOCAL)
      m = nodeData.localTransform;
    else if (bonesTransform() == BonesTransform::GLOBAL)
      m = nodeData.globalTransform;

    return m*CPoint3D(0.0, 0.0, 0.0);
  };

  //---

  const auto &objects = scene_->getObjects();

  for (auto *object : objects) {
    ObjectData *objectData { nullptr };

    auto pd = objectDatas_.find(object);

    if (pd == objectDatas_.end())
      pd = objectDatas_.insert(pd, ObjectDatas::value_type(object, new ObjectData));

    objectData = (*pd).second;

    if (! objectData->object)
      objectData->object = object;

    if (! objectData->bonesData.buffer)
      objectData->bonesData.buffer = bonesShaderProgram_->createBuffer();

    //---

    auto *buffer = objectData->bonesData.buffer;

    buffer->clearBuffers();

    objectData->bonesData.faceDatas.clear();

    //---

    auto *rootObject = getRootObject(object);

    const auto &nodeIds = rootObject->getNodeIds();

    int pos = 0;

    for (const auto &nodeId : nodeIds) {
      const auto &node = rootObject->getNode(nodeId);
      if (! node.valid) continue;

      //---

      auto c = transformNode(node);

      bbox += c;

      //---

      addBonesCube(objectData, c, nodeId, color, pos);

      //---

      if (node.parent >= 0) {
        //int parentInd = rootObject->mapNodeId(node.parent);

        const auto &pnode = rootObject->getNode(node.parent);

        if (pnode.valid) {
          auto c1 = transformNode(pnode);

          addBonesLine(objectData, c, c1, nodeId, node.parent, color, pos);
        }
      }
    }

    buffer->load();
  }

  //---

  setSceneBBox(bbox);
}

void
CQNewGLCanvas::
updateBoneData()
{
  // set up vertex data (and buffer(s)) and configure vertex attributes
  if (scene_->getObjects().empty()) {
    sceneScale_    = 1.0;
    invSceneScale_ = 1.0;
    return;
  }

  //---

  CBBox3D bbox;
//scene.getBBox(bbox);

  //---

  int boneInd = app_->boneInd();

  CGeomObject3D *rootObject = nullptr;

  if (! objectDatas_.empty())
    rootObject = getRootObject(objectDatas_.begin()->first);

  if (rootObject) {
    ObjectData *objectData { nullptr };

    auto pd = objectDatas_.find(rootObject);

    if (pd == objectDatas_.end())
      pd = objectDatas_.insert(pd, ObjectDatas::value_type(rootObject, new ObjectData));

    objectData = (*pd).second;

    if (! objectData->object)
      objectData->object = rootObject;

    if (! objectData->boneData.buffer)
      objectData->boneData.buffer = boneShaderProgram_->createBuffer();

    //---

    auto *buffer = objectData->boneData.buffer;

    buffer->clearBuffers();

    objectData->boneData.faceDatas.clear();

    //---

    int pos = 0;

    addBoneCube(objectData, app_->boneMatrix(), boneInd, 1.0, CRGBA(1.0, 0.0, 0.0), pos, bbox);

#if 0
    //auto cubeSize = sceneScale_/2.0;
    auto cubeSize = 1.0;

    addBoneCube(objectData,
      CMatrix3D::translation(sceneCenter_.getX(), sceneCenter_.getY(), sceneCenter_.getZ()),
      boneInd, cubeSize, CRGBA(0.0, 1.0, 0.0), pos, bbox);
#else
    addBoneCube(objectData, CMatrix3D::identity(), boneInd, 1.0, CRGBA(0.0, 1.0, 0.0), pos, bbox);
#endif

    buffer->load();
  }

  //---

  setSceneBBox(bbox);
}

//---

void
CQNewGLCanvas::
addBonesCube(ObjectData *objectData, const CPoint3D &c, int nodeId,
             const CRGBA &color, int &pos) const
{
  static GLfloat cube_normal[6][3] = {
    {-1.0,  0.0,  0.0},
    { 0.0,  1.0,  0.0},
    { 1.0,  0.0,  0.0},
    { 0.0, -1.0,  0.0},
    { 0.0,  0.0,  1.0},
    { 0.0,  0.0, -1.0}
  };

  static GLint cube_faces[6][4] = {
    {0, 1, 2, 3},
    {3, 2, 6, 7},
    {7, 6, 5, 4},
    {4, 5, 1, 0},
    {5, 6, 2, 1},
    {7, 4, 0, 3}
  };

  GLfloat v[8][3];

  v[0][0] = v[1][0] = v[2][0] = v[3][0] = -0.5;
  v[4][0] = v[5][0] = v[6][0] = v[7][0] =  0.5;
  v[0][1] = v[1][1] = v[4][1] = v[5][1] = -0.5;
  v[2][1] = v[3][1] = v[6][1] = v[7][1] =  0.5;
  v[0][2] = v[3][2] = v[4][2] = v[7][2] = -0.5;
  v[1][2] = v[2][2] = v[5][2] = v[6][2] =  0.5;

  //---

  auto pointSize = 0.05f;

  auto *buffer = objectData->bonesData.buffer;

  auto cubePoint = [&](int i, int j) {
    const auto &v1 = v[cube_faces[i][j]];

    return CPoint3D(c.x + pointSize*v1[0], c.y + pointSize*v1[1], c.z + pointSize*v1[2]);
  };

  auto cubeNormal = [&](int i) {
    const auto &n = cube_normal[i];

    return CPoint3D(n[0], n[1], n[2]);
  };

  auto addPoint = [&](const CPoint3D &p, const CPoint3D &n, const CRGBA &c) {
    buffer->addPoint(p.x, p.y, p.z);
    buffer->addNormal(n.x, n.y, n.z);
    buffer->addColor(c);
    buffer->addTexturePoint(0.0f, 0.0f);
  };

  for (GLint i = 5; i >= 0; i--) {
    CQNewGLFaceData faceData;

    faceData.boneId       = nodeId;
    faceData.parentBoneId = -1;
    faceData.pos          = pos;
    faceData.len          = 4;

    auto p1 = cubePoint(i, 0);
    auto p2 = cubePoint(i, 1);
    auto p3 = cubePoint(i, 2);
    auto p4 = cubePoint(i, 3);
    auto n  = cubeNormal(i);

    addPoint(p1, n, color);
    addPoint(p2, n, color);
    addPoint(p3, n, color);
    addPoint(p4, n, color);

    objectData->bonesData.faceDatas.push_back(faceData);

    pos += faceData.len;
  }
}

void
CQNewGLCanvas::
addBoneCube(ObjectData *objectData, const CMatrix3D &m, int nodeId, double cubeSize,
            const CRGBA &color, int &pos, CBBox3D &bbox) const
{
  auto c = CPoint3D(0.0, 0.0, 0.0);

  static GLfloat cube_normal[6][3] = {
    {-1.0,  0.0,  0.0},
    { 0.0,  1.0,  0.0},
    { 1.0,  0.0,  0.0},
    { 0.0, -1.0,  0.0},
    { 0.0,  0.0,  1.0},
    { 0.0,  0.0, -1.0}
  };

  static GLint cube_faces[6][4] = {
    {0, 1, 2, 3},
    {3, 2, 6, 7},
    {7, 6, 5, 4},
    {4, 5, 1, 0},
    {5, 6, 2, 1},
    {7, 4, 0, 3}
  };

  GLfloat v[8][3];

  v[0][0] = v[1][0] = v[2][0] = v[3][0] = -0.5;
  v[4][0] = v[5][0] = v[6][0] = v[7][0] =  0.5;
  v[0][1] = v[1][1] = v[4][1] = v[5][1] = -0.5;
  v[2][1] = v[3][1] = v[6][1] = v[7][1] =  0.5;
  v[0][2] = v[3][2] = v[4][2] = v[7][2] = -0.5;
  v[1][2] = v[2][2] = v[5][2] = v[6][2] =  0.5;

  //---

  auto *buffer = objectData->boneData.buffer;

  auto cubePoint = [&](int i, int j) {
    const auto &v1 = v[cube_faces[i][j]];

    return CPoint3D(c.x + cubeSize*v1[0], c.y + cubeSize*v1[1], c.z + cubeSize*v1[2]);
  };

  auto cubeNormal = [&](int i) {
    const auto &n = cube_normal[i];

    return CPoint3D(n[0], n[1], n[2]);
  };

  auto addPoint = [&](const CPoint3D &p, const CPoint3D &n, const CRGBA &c) {
    auto p1 = m*p;
    auto n1 = m*n;

    buffer->addPoint(p1.x, p1.y, p1.z);
    buffer->addNormal(n1.x, n1.y, n1.z);
    buffer->addColor(c);

    bbox += CPoint3D(p1.x, p1.y, p1.z);
  };

  for (GLint i = 5; i >= 0; i--) {
    CQNewGLFaceData faceData;

    faceData.boneId       = nodeId;
    faceData.parentBoneId = -1;
    faceData.pos          = pos;
    faceData.len          = 4;

    auto p1 = cubePoint(i, 0);
    auto p2 = cubePoint(i, 1);
    auto p3 = cubePoint(i, 2);
    auto p4 = cubePoint(i, 3);
    auto n  = cubeNormal(i);

    addPoint(p1, n, color);
    addPoint(p2, n, color);
    addPoint(p3, n, color);
    addPoint(p4, n, color);

    objectData->boneData.faceDatas.push_back(faceData);

    pos += faceData.len;
  }
}

void
CQNewGLCanvas::
addBonesLine(ObjectData *objectData, const CPoint3D &c1, const CPoint3D &c2,
             int boneId, int parentBoneId, const CRGBA &color, int &pos) const
{
  auto pointSize = 0.05f;

  auto *buffer = objectData->bonesData.buffer;

  auto addPoint = [&](const CPoint3D &p, const CPoint3D &n, const CRGBA &c, int id) {
    buffer->addPoint(p.x, p.y, p.z);
    buffer->addNormal(n.x, n.y, n.z);
    buffer->addColor(c);
    buffer->addTexturePoint(float(id), 0.0f);
  };

  CQNewGLFaceData faceData;

  faceData.boneId       = boneId;
  faceData.parentBoneId = parentBoneId;
  faceData.pos          = pos;
  faceData.len          = 4;

  auto dp = CPoint3D(pointSize, 0, 0);

  auto p1 = c1 - dp;
  auto p2 = c1 + dp;
  auto p3 = c2 + dp;
  auto p4 = c2 - dp;
  auto n  = CPoint3D(0, 0, 1);

  addPoint(p1, n, color, 0);
  addPoint(p2, n, color, 0);
  addPoint(p3, n, color, 1);
  addPoint(p4, n, color, 1);

  objectData->bonesData.faceDatas.push_back(faceData);

  pos += faceData.len;
}

//---

CQGLTexture *
CQNewGLCanvas::
makeTexture(const CImagePtr &image) const
{
  auto *texture = new CQGLTexture(image);

  texture->setFunctions(const_cast<CQNewGLCanvas *>(this));

  return texture;
}

CQGLTexture *
CQNewGLCanvas::
getTexture(CGeomTexture *texture, bool add)
{
  auto pt = glTextures_.find(texture->id());

  if (pt == glTextures_.end()) {
    if (! add)
      return nullptr;

    const auto &image = texture->image()->image();

    auto *glTexture = makeTexture(image);

    glTexture->setName(texture->name());

    TextureData textureData;

    textureData.glTexture   = glTexture;
    textureData.geomTexture = texture;

    pt = glTextures_.insert(pt, GLTextures::value_type(texture->id(), textureData));

    Q_EMIT textureAdded();
  }

  const auto &textureData = (*pt).second;

  return textureData.glTexture;
}

CQGLTexture *
CQNewGLCanvas::
getTextureByName(const std::string &name) const
{
  for (const auto &pt : glTextures_) {
    const auto &textureData = pt.second;

    if (textureData.glTexture->getName() == name)
      return textureData.glTexture;
  }

  return nullptr;
}

CGeomTexture *
CQNewGLCanvas::
getGeomTextureByName(const std::string &name) const
{
  for (const auto &pt : glTextures_) {
    const auto &textureData = pt.second;

    if (textureData.glTexture->getName() == name)
      return textureData.geomTexture;
  }

  return nullptr;
}

void
CQNewGLCanvas::
paintGL()
{
  // per-frame time logic
  auto *camera = getCurrentCamera();

  camera->updateFrameTime();

  //---

  // clear canvas
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //---

  pixelWidth_  = width ();
  pixelHeight_ = height();

  // set view
  aspect_ = float(pixelWidth_)/float(pixelHeight_);

  if (isOrtho())
    paintData_.projection = camera->getOrthoMatrix();
  else
    paintData_.projection = camera->getPerspectiveMatrix(aspect_);

  paintData_.view    = camera->getViewMatrix();
  paintData_.viewPos = camera->position();

  //---

  // draw model objects
  for (auto &po : objectDatas_) {
    auto *objectData = po.second;

    auto *object = objectData->object;
    if (! object->getVisible())
      continue;

    if      (app_->isShowBone())
      drawObjectBone(objectData);
    else if (isBonesEnabled()) {
      drawObjectBones(objectData);

      if (! isPolygonSolid() && isPolygonLine())
        drawObjectModel(objectData);
    }
    else
      drawObjectModel(objectData);
  }

  //---

  if (isShowNormals()) {
    for (auto &po : objectDatas_) {
      auto *objectData = po.second;

      auto *object = objectData->object;
      if (! object->getVisible())
        continue;

      drawObjectNormals(objectData);
    }
  }

  //---

  drawTerrain();

  //---

  drawMaze();

  //---

  drawSkybox();

  //---

  drawEmitters();

  //---

  drawFractal();

  //---

  drawDrawTree();

  //---

  drawShapes();

  //---

  drawPaths();

  //---

  // draw cameras
  if (isShowCameras())
    drawCameras();

  // draw lights
  if (isShowLights())
    drawLights();

  //---

  // draw test text
//text_->render();

  //---

  // draw axes
  if (axes_->isVisible())
    drawAxes();

  //---

  // draw current object bbox
  if (bbox_->isVisible())
    drawBBox();

  //---

  // draw current object normal
  if (normals_->isVisible())
    drawNormals();

  //---

  // draw current object hull
  if (hull_->isVisible())
    drawHull();

  //---

  // draw current object basis
  if (isShowBasis())
    drawBasis();
}

void
CQNewGLCanvas::
drawObjectModel(ObjectData *objectData)
{
  auto *object     = objectData->object;
  auto *rootObject = getRootObject(object);

  // update bone nodes
  bool useBones = (isAnimEnabled() && animName() != "");

  if (useBones) {
    if (! rootObject->updateNodesAnimationData(animName().toStdString(), time()))
      useBones = false;
  }

  bool useBonePoints = (useBones && objectData->modelData.buffer->hasBonesPart());
  bool showBonePoints = (useBones && isShowBonePoints());

  //---

  auto isSelected = object->getSelected();

  objectData->modelData.buffer->bind();

  modelShaderProgram_->bind();

  //---

  modelShaderProgram_->setUniformValue("viewPos", CQGLUtil::toVector(viewPos()));

  //---

  // model rotation
#if 0
  auto modelMatrix = CMatrix3D::identity();

  modelMatrix.scaled(modelScale_.getX(), modelScale_.getY(), modelScale_.getZ());
  modelMatrix.rotated(modelRotate_.x(), CVector3D(1.0, 0.0, 0.0));
  modelMatrix.rotated(modelRotate_.y(), CVector3D(0.0, 1.0, 0.0));
  modelMatrix.rotated(modelRotate_.z(), CVector3D(0.0, 0.0, 1.0));
  modelMatrix.translated(modelTranslate_.getX(), modelTranslate_.getY(), modelTranslate_.getZ());
#else
  //auto modelMatrix = getModelMatrix();
  auto modelMatrix = object->getHierTransform();
#endif

  if (useBones) {
    auto meshMatrix = getMeshGlobalTransform(objectData, /*invert*/false);
    modelMatrix = meshMatrix*modelMatrix;
  }

  addShaderMVP(modelShaderProgram_, modelMatrix);

  //---

  addShaderLights(modelShaderProgram_);

  //---

  // bones transform
  if (useBones) {
    updateNodeMatrices(objectData);

    modelShaderProgram_->setUniformValueArray("globalBoneTransform",
      &paintData_.nodeQMatrices[0], paintData_.numNodeMatrices);
  }

  //---

  if (! showBonePoints) {
    modelShaderProgram_->setUniformValue("useBones", useBones);
    modelShaderProgram_->setUniformValue("useBonePoints", useBonePoints);
  }
  else {
    modelShaderProgram_->setUniformValue("useBones", false);
    modelShaderProgram_->setUniformValue("useBonePoints", false);
  }

  //---

  addShaderLightGlobals(modelShaderProgram_);

  //---

  modelShaderProgram_->setUniformValue("isSelected", isSelected);

  // render model
  for (const auto &faceData : objectData->modelData.faceDatas) {
    int boneInd = rootObject->mapNodeId(faceData.boneId);

    modelShaderProgram_->setUniformValue("boneId", boneInd);

    //---

    // material
    modelShaderProgram_->setUniformValue("shininess", float(faceData.shininess));

    //---

    bool useDiffuseTexture = faceData.diffuseTexture;

    modelShaderProgram_->setUniformValue("diffuseTexture.enabled", useDiffuseTexture);

    if (useDiffuseTexture) {
      glActiveTexture(GL_TEXTURE0);
      faceData.diffuseTexture->bind();

      modelShaderProgram_->setUniformValue("diffuseTexture.texture", 0);
    }

    //---

    bool useSpecularTexture = faceData.specularTexture;

    modelShaderProgram_->setUniformValue("specularTexture.enabled", useSpecularTexture);

    if (useSpecularTexture) {
      glActiveTexture(GL_TEXTURE1);
      faceData.specularTexture->bind();

      modelShaderProgram_->setUniformValue("specularTexture.texture", 1);
    }

    //---

    bool useNormalTexture = faceData.normalTexture;

    modelShaderProgram_->setUniformValue("normalTexture.enabled", useNormalTexture);

    if (useNormalTexture) {
      glActiveTexture(GL_TEXTURE2);
      faceData.normalTexture->bind();

      modelShaderProgram_->setUniformValue("normalTexture.texture", 2);
    }

    //---

    bool useEmissiveTexture = faceData.emissiveTexture;

    modelShaderProgram_->setUniformValue("emissiveTexture.enabled", useEmissiveTexture);

    if (useEmissiveTexture) {
      glActiveTexture(GL_TEXTURE3);
      faceData.emissiveTexture->bind();

      modelShaderProgram_->setUniformValue("emissiveTexture.texture", 3);
    }

    modelShaderProgram_->setUniformValue("emissionColor", CQGLUtil::toVector(faceData.emission));

    //---

    modelShaderProgram_->setUniformValue("tangentSpaceNormal", isTangentSpaceNormal());

    //---

    if (isPolygonSolid()) {
      modelShaderProgram_->setUniformValue("isWireframe", 0);

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

      glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
    }

    if (isPolygonLine()) {
      modelShaderProgram_->setUniformValue("isWireframe", 1);

      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

      glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
    }
  }

  //---

  objectData->modelData.buffer->unbind();

  modelShaderProgram_->release();
}

void
CQNewGLCanvas::
addShaderMVP(CQNewGLShaderProgram *program, const CMatrix3D &modelMatrix)
{
  // pass projection matrix to shader (note that in this case it could change every frame)
  program->setUniformValue("projection", CQGLUtil::toQMatrix(projectionMatrix()));

  // camera/view transformation
  program->setUniformValue("view", CQGLUtil::toQMatrix(viewMatrix()));

  // model matrix
  program->setUniformValue("model", CQGLUtil::toQMatrix(modelMatrix));
}

void
CQNewGLCanvas::
addShaderLightGlobals(CQNewGLShaderProgram *program)
{
  program->setUniformValue("ambientColor", CQGLUtil::toVector(ambientColor()));

  //---

  program->setUniformValue("ambientStrength" , float(app_->ambientStrength()));
  program->setUniformValue("diffuseStrength" , float(app_->diffuseStrength()));
  program->setUniformValue("specularStrength", float(app_->specularStrength()));
  program->setUniformValue("emissiveStrength", float(app_->emissiveStrength()));
}

void
CQNewGLCanvas::
addShaderLights(CQNewGLShaderProgram *program)
{
  // max four active lights
  int il = 0;

  for (auto *light : lights()) {
    auto lightName = QString("lights[%1]").arg(il);

    light->setShaderData(program, lightName);

    ++il;

    if (il >= 4)
      break;
  }

  for ( ; il < 4; ++il) {
    auto lightName = QString("lights[%1]").arg(il);

    program->setUniformValue(toCString(lightName + ".enabled"), false);
  }
}

void
CQNewGLCanvas::
addShaderCurrentLight(CQNewGLShaderProgram *program)
{
  auto *light = getCurrentLight();

  program->setUniformValue("lightColor", CQGLUtil::toVector(QColorToVector(light->color())));
  program->setUniformValue("lightPos", CQGLUtil::toVector(light->position()));
}

void
CQNewGLCanvas::
drawObjectBones(ObjectData *objectData)
{
  auto *object     = objectData->object;
  auto *rootObject = getRootObject(object);

  // update bone nodes
  bool useBones = (isAnimEnabled() && animName() != "");

  if (useBones) {
    if (! rootObject->updateNodesAnimationData(animName().toStdString(), time()))
      useBones = false;
  }

  //---

  auto isSelected = object->getSelected();

  objectData->bonesData.buffer->bind();

  bonesShaderProgram_->bind();

  //---

  bonesShaderProgram_->setUniformValue("viewPos", CQGLUtil::toVector(viewPos()));

  auto modelMatrix = getModelMatrix();
  addShaderMVP(bonesShaderProgram_, modelMatrix);

  //---

  addShaderCurrentLight(bonesShaderProgram_);

  addShaderLightGlobals(bonesShaderProgram_);

  //---

  // bones transform
  if (useBones) {
    updateNodeMatrices(objectData);

    bonesShaderProgram_->setUniformValueArray("globalBoneTransform",
      &paintData_.nodeQMatrices[0], paintData_.numNodeMatrices);
  }

  //---

  bonesShaderProgram_->setUniformValue("useBones", useBones);

  bonesShaderProgram_->setUniformValue("isSelected", isSelected);

  //---

  // render model
  for (const auto &faceData : objectData->bonesData.faceDatas) {
    int boneInd       = rootObject->mapNodeId(faceData.boneId);
    int parentBoneInd = rootObject->mapNodeId(faceData.parentBoneId);

    bonesShaderProgram_->setUniformValue("boneId"      , boneInd);
    bonesShaderProgram_->setUniformValue("parentBoneId", parentBoneInd);

    // material
    bonesShaderProgram_->setUniformValue("shininess", float(faceData.shininess));

    //---

    if (isPolygonSolid()) {
      bonesShaderProgram_->setUniformValue("isWireframe", 0);

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

      glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
    }

    if (isPolygonLine()) {
      bonesShaderProgram_->setUniformValue("isWireframe", 1);

      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

      glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
    }
  }

  objectData->bonesData.buffer->unbind();

  bonesShaderProgram_->release();
}

void
CQNewGLCanvas::
drawObjectBone(ObjectData *objectData)
{
  auto *object     = objectData->object;
  auto *rootObject = getRootObject(object);

  const auto &nodes = rootObject->getNodes();
  if (nodes.empty()) return;

  //---

  if (! objectData->boneData.buffer)
    return;

  objectData->boneData.buffer->bind();

  boneShaderProgram_->bind();

  //---

  boneShaderProgram_->setUniformValue("viewPos", CQGLUtil::toVector(viewPos()));

  auto modelMatrix = getModelMatrix();
  addShaderMVP(boneShaderProgram_, modelMatrix);

  //---

  addShaderCurrentLight(boneShaderProgram_);

  addShaderLightGlobals(boneShaderProgram_);

  //---

  // render model
  for (const auto &faceData : objectData->boneData.faceDatas) {
    // material
    boneShaderProgram_->setUniformValue("shininess", float(faceData.shininess));

    //---

#if 0
    if (isPolygonSolid()) {
      boneShaderProgram_->setUniformValue("isWireframe", 0);

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

      glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
    }

    if (isPolygonLine()) {
      boneShaderProgram_->setUniformValue("isWireframe", 1);

      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

      glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
    }
#else
    boneShaderProgram_->setUniformValue("isWireframe", 1);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
#endif
  }

  objectData->boneData.buffer->unbind();
}

void
CQNewGLCanvas::
drawObjectNormals(ObjectData *objectData)
{
  auto *object = objectData->object;

  //---

  if (! objectData->normalData.buffer)
    return;

  objectData->normalData.buffer->bind();

  normalShaderProgram_->bind();

  //---

//auto modelMatrix = getModelMatrix();
  auto modelMatrix = object->getHierTransform();
  addShaderMVP(normalShaderProgram_, modelMatrix);

  //---

  // draw lines
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glDrawArrays(GL_LINES, 0, objectData->normalData.buffer->numPoints());

  //---

  objectData->normalData.buffer->unbind();

  //---

  boneShaderProgram_->release();
}

CMatrix3D
CQNewGLCanvas::
getModelMatrix() const
{
  auto modelMatrix = CMatrix3D::identity();

#if 0
  modelMatrix.scaled(invSceneScale_, invSceneScale_, invSceneScale_);
  modelMatrix.rotated(modelRotate_.x(), CVector3D(1.0, 0.0, 0.0));
  modelMatrix.rotated(modelRotate_.y(), CVector3D(0.0, 1.0, 0.0));
  modelMatrix.rotated(modelRotate_.z(), CVector3D(0.0, 0.0, 1.0));
  modelMatrix.translated(-sceneCenter_.getX(), -sceneCenter_.getY(), -sceneCenter_.getZ());
#endif

  return modelMatrix;
}

CMatrix3D
CQNewGLCanvas::
getMeshGlobalTransform(ObjectData *objectData, bool invert) const
{
  auto *object     = objectData->object;
  auto *rootObject = getRootObject(object);

  int meshNodeId = object->getMeshNode();

  if (meshNodeId < 0)
    meshNodeId = rootObject->getMeshNode();

  const auto &meshNodeData = rootObject->getNode(meshNodeId);

  auto meshMatrix        = meshNodeData.globalTransform;
  auto inverseMeshMatrix = meshMatrix.inverse();

  return (invert ? inverseMeshMatrix : meshMatrix);
}

CMatrix3D
CQNewGLCanvas::
getMeshLocalTransform(ObjectData *objectData, bool invert) const
{
  auto *object     = objectData->object;
  auto *rootObject = getRootObject(object);

  int meshNodeId = object->getMeshNode();

  if (meshNodeId < 0)
    meshNodeId = rootObject->getMeshNode();

  const auto &meshNodeData = rootObject->getNode(meshNodeId);

  auto meshMatrix        = meshNodeData.localTransform;
  auto inverseMeshMatrix = meshMatrix.inverse();

  return (invert ? inverseMeshMatrix : meshMatrix);
}

void
CQNewGLCanvas::
updateNodeMatrices(ObjectData *objectData)
{
  auto *object     = objectData->object;
  auto *rootObject = getRootObject(object);

  std::vector<CMatrix3D> nodeMatrices;

  for (int i = 0; i < paintData_.numNodeMatrices; i++)
    nodeMatrices.push_back(CMatrix3D::identity());

  struct WorkData {
    const CGeomObject3D::NodeData *parent     { nullptr };
    const CGeomObject3D::NodeData *node       { nullptr };
//  CMatrix3D                      animMatrix { CMatrix3D::identity() };
    CMatrix3D                      transform  { CMatrix3D::identity() };
  };

  int meshNodeId = rootObject->getMeshNode();

  const auto &meshNodeData = rootObject->getNode(meshNodeId);

  auto meshMatrix        = meshNodeData.globalTransform;
  auto inverseMeshMatrix = meshMatrix.inverse();

#if 0
  int nodeMax = -1;

  std::vector<WorkData> workList;

  int rootNodeId = rootObject->getRootNode();

  const auto &rootNodeData = rootObject->getNode(rootNodeId);

  if (rootNodeData.valid) {
    auto rootInverseBindMatrix = rootNodeData.inverseBindMatrix;

//  auto rootInverseMatrix = rootInverseBindMatrix.inverse();
//  auto rootInverseMatrix = rootNodeData.globalTransform.inverse();
//  auto rootInverseMatrix = rootNodeData.localTransform.inverse();
//  auto rootInverseMatrix = CMatrix3D::identity();
//  auto rootInverseMatrix = rootNodeData.animMatrix.inverse();

    WorkData rootWorkData;

    rootWorkData.parent     = nullptr;
    rootWorkData.node       = &rootNodeData;
//  rootWorkData.animMatrix = rootNodeData.animMatrix;
//  rootWorkData.transform  = rootWorkData.animMatrix*rootInverseBindMatrix;
    rootWorkData.transform  = inverseMeshMatrix*rootNodeData.hierAnimMatrix*rootInverseBindMatrix;

    nodeMatrices[rootNodeId] = rootWorkData.transform;

    nodeMax = std::max(nodeMax, rootNodeId);

    workList.push_back(rootWorkData);
  }

  while (! workList.empty()) {
    const auto &parentWorkData = workList.back(); workList.pop_back();

    for (const auto &id : parentWorkData.node->children) {
      const auto &childNodeData = rootObject->getNode(id);
      assert(childNodeData.valid);

      auto childInverseBindMatrix = childNodeData.inverseBindMatrix;

//    auto childInverseMatrix = childInverseBindMatrix.inverse();
//    auto childInverseMatrix = childNodeData.globalTransform.inverse();
//    auto childInverseMatrix = childNodeData.localTransform.inverse();
//    auto childInverseMatrix = CMatrix3D::identity();

      WorkData childWorkData;

      childWorkData.parent     = parentWorkData.node;
      childWorkData.node       = &childNodeData;
//    childWorkData.animMatrix = parentWorkData.animMatrix*childNodeData.animMatrix;
//    childWorkData.transform  = childWorkData.animMatrix*childInverseBindMatrix;
      childWorkData.transform  =
        inverseMeshMatrix*childNodeData.hierAnimMatrix*childInverseBindMatrix;

      nodeMatrices[id] = childWorkData.transform;

      nodeMax = std::max(nodeMax, id);

      workList.push_back(childWorkData);
    }
  }

//std::cerr << nodeMax << "\n";
  assert(nodeMax < paintData_.numNodeMatrices);
#else
  const auto &nodeIds = rootObject->getNodeIds();

  int ij = 0;

  for (const auto &nodeId : nodeIds) {
    if (ij >= paintData_.numNodeMatrices)
      break;

    const auto &nodeData = rootObject->getNode(nodeId);

    if (nodeData.valid) {
      auto transform = inverseMeshMatrix*nodeData.hierAnimMatrix*nodeData.inverseBindMatrix;

      nodeMatrices[ij] = transform;
    }

    ++ij;
  }

  if (ij >= paintData_.numNodeMatrices)
    std::cerr << "Too many bones\n";
#endif

  paintData_.nodeMatrices .resize(paintData_.numNodeMatrices);
  paintData_.nodeQMatrices.resize(paintData_.numNodeMatrices);

  int im = 0;
  for (const auto &m : nodeMatrices) {
    paintData_.nodeMatrices [im] = m;
    paintData_.nodeQMatrices[im] = CQGLUtil::toQMatrix(m);

    ++im;
  }
}

//---

void
CQNewGLCanvas::
updateAxes()
{
  axes_->updateGeometry();
}

void
CQNewGLCanvas::
drawAxes()
{
  if (axesNeedsUpdate_) {
    updateAxes();

    axesNeedsUpdate_ = false;
  }

  axes_->drawGeometry();
}

//---

void
CQNewGLCanvas::
updateBBox()
{
  bbox_->updateGeometry();
}

void
CQNewGLCanvas::
drawBBox()
{
  bbox_->drawGeometry();
}

//---

void
CQNewGLCanvas::
updateNormals()
{
  normals_->updateGeometry();
}

void
CQNewGLCanvas::
drawNormals()
{
  normals_->drawGeometry();
}

//---

void
CQNewGLCanvas::
updateHull()
{
  hull_->updateGeometry();
}

void
CQNewGLCanvas::
drawHull()
{
  hull_->drawGeometry();
}

//---

void
CQNewGLCanvas::
updateBasis()
{
  auto *object = getCurrentObject();
  if (! object) return;

  basisData_.object = object;

  //---

  if (! basisData_.buffer)
    basisData_.buffer = basisData_.shaderProgram->createBuffer();

  basisData_.buffer->clearBuffers();

  basisData_.faceDatas.clear();

  //---

  auto lineWidth = basisData_.lineWidth;
  auto lineSize  = basisData_.lineSize;

  if (lineWidth < 0) {
    auto bbox = getModelBBox(object);

    lineWidth = bbox.getMaxSize()/250.0;
  }

  if (lineSize < 0)
    lineSize = 1.0;

  auto color = basisData_.color;

  //---

  int pos = 0;

#if 0
  auto addPoint = [&](const CPoint3D &p, const CPoint3D &n, const CRGBA &c) {
    basisData_.buffer->addPoint(p.x, p.y, p.z);
    basisData_.buffer->addNormal(n.x, n.y, n.z);
    basisData_.buffer->addColor(c);
  };
#endif

  auto addLine = [&](const CPoint3D &c1, const CPoint3D &c2, const CPoint3D & /*dp*/) {
#if 0
    CQNewGLFaceData faceData;

    faceData.pos = pos;
    faceData.len = 4;

    auto dp1 = lineWidth*dp;

    auto p1 = c1 - dp1;
    auto p2 = c1 + dp1;
    auto p3 = c2 + dp1;
    auto p4 = c2 - dp1;
    auto n  = CPoint3D(0, 0, 1);

    addPoint(p1, n, color);
    addPoint(p2, n, color);
    addPoint(p3, n, color);
    addPoint(p4, n, color);

    basisData_.faceDatas.push_back(faceData);

    pos += faceData.len;
#else
    addCylinder(basisData_.buffer, c1, c2, lineWidth, color, basisData_.faceDatas, pos);
#endif
  };

  //---

  CPoint3D pu, pv, pw;

  if (object) {
    CVector3D u, v, w;
    getBasis(object, u, v, w);

    auto bbox = getModelBBox(object);

    auto c = bbox.getCenter();
    auto s = lineSize*bbox.getRadius();

    pu = c + s*u;
    pv = c + s*v;
    pw = c + s*w;

    addLine(c, pu, CPoint3D(0, 1, 0));
    addLine(c, pv, CPoint3D(1, 0, 0));
    addLine(c, pw, CPoint3D(0, 1, 0));
  }

  //---

  basisData_.buffer->load();

  //---

  auto transform = object->getHierTransform();

  for (auto *text : basisData_.texts)
    delete text;

  basisData_.texts.clear();

  auto createText = [&](const QString &str, const CPoint3D &pos, double size) {
    auto pos1 = transform*pos;

    auto *text = new CQNewGLText(str);

    text->setFont(font_);
    text->setColor(CQNewGLFont::Color(1, 1, 1));
    text->setPosition(CGLVector3D(pos1.x, pos1.y, pos1.z));
    text->setSize(size);

    basisData_.texts.push_back(text);

    return text;
  };

  if (object) {
    auto ts = sceneScale_/10.0;

    (void) createText("U", pu, ts);
    (void) createText("V", pv, ts);
    (void) createText("W", pw, ts);

    for (auto *text : basisData_.texts)
      text->updateText();
  }
}

void
CQNewGLCanvas::
drawBasis()
{
  if (basisData_.needsUpdate) {
    updateBasis();

    basisData_.needsUpdate = false;
  }

  //---

  auto *object = basisData_.object;

  //---

  basisData_.buffer->bind();

  basisData_.shaderProgram->bind();

  //---

//auto modelMatrix = getModelMatrix();
  auto modelMatrix = object->getHierTransform();
  addShaderMVP(basisData_.shaderProgram, modelMatrix);

  //---

  // render basis
  for (const auto &faceData : basisData_.faceDatas) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
  }

  basisData_.buffer->unbind();

  //---

  basisData_.shaderProgram->release();

  //---

  for (auto *text : basisData_.texts)
    text->render();
}

//---

CGeomObject3D *
CQNewGLCanvas::
getRootObject(CGeomObject3D *object) const
{
  auto *rootObject = object;

  while (rootObject && rootObject->parent())
    rootObject = rootObject->parent();

  return rootObject;
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
  mouseData_.pressX = e->x();
  mouseData_.pressY = e->y();
  mouseData_.button = e->button();

  bool isLeftButton = (mouseData_.button == Qt::LeftButton);
  bool showEyeLine  = ! isLeftButton && isShowEyeline();

  if (showEyeLine) {
    clearEyeLines();

    eyeLine1_ = new CQNewGLPath(this);
    eyeLine2_ = new CQNewGLPath(this);
  }

  setMousePos(mouseData_.pressX, mouseData_.pressY, /*add*/false, showEyeLine);

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
CQNewGLCanvas::
mouseReleaseEvent(QMouseEvent *)
{
#if 0
  clearEyeLines();
#endif

  mouseData_.button = Qt::NoButton;

  update();
}

void
CQNewGLCanvas::
mouseMoveEvent(QMouseEvent *e)
{
  mouseData_.moveX = e->x();
  mouseData_.moveY = e->y();

  bool isLeftButton = (mouseData_.button == Qt::LeftButton);
  bool showEyeLine  = ! isLeftButton && isShowEyeline();

  setMousePos(mouseData_.moveX, mouseData_.moveY, /*add*/true, showEyeLine);

  //---

  if (isLeftButton) {
    auto type = app_->type();

    if      (type == CQNewGLModel::Type::CAMERA) {
      auto *camera = getCurrentCamera();

      float xoffset, yoffset;
      camera->deltaPos(mouseData_.moveX, mouseData_.moveY, xoffset, yoffset);

      camera->setLastPos(mouseData_.moveX, mouseData_.moveY);

      camera->processMouseMovement(xoffset, yoffset);
      updateCameraBuffer();
    }
    else if (type == CQNewGLModel::Type::LIGHT) {
      auto *light = getCurrentLight();

      auto dx = sceneSize_.getX()*(mouseData_.moveX - mouseData_.pressX)/width ();
      auto dy = sceneSize_.getY()*(mouseData_.moveY - mouseData_.pressY)/height();

      light->setPosition(light->position() + CGLVector3D(dx, dy, 0.0f));

      app()->updateLights();
    }
    else if (type == CQNewGLModel::Type::MODEL) {
    }
  }

  //---

  mouseData_.pressX = mouseData_.moveX;
  mouseData_.pressY = mouseData_.moveY;

  update();
}

void
CQNewGLCanvas::
setMousePos(double xpos, double ypos, bool add, bool show)
{
  CPoint3D ep1, ep2;
  CVector3D ev;
  calcEyeLine(CPoint2D(xpos, ypos), ep1, ep2, ev);
  //std::cerr << "Eye Line: (" << xpos << "," << ypos << ") (" << ep1 << "," << ep2 << ")\n";

  // show on toolbar
  app_->toolbar()->setPosLabel(QString("%1 %2 %3").arg(ep1.x).arg(ep1.y).arg(ep1.z));

//auto *camera = getCurrentCamera();
//ep2 = camera->origin();

  auto ep3 = ep2;
  auto ep4 = ep2;

  //---

  // intersect eye line with current object
  auto *object = getCurrentObject();

  if (object) {
    CLine3D line(ep1, ep2);

    auto bbox = getObjectBBox(object);

    auto c = bbox.getCenter();
    auto s = bbox.getMaxSize();

    CSphere3D sphere(s/2.0);

    sphere.translate(c);

    double tmin, tmax;
    if (sphere.intersect(line, &tmin, &tmax)) {
      ep3 = line.point(tmin);
      ep4 = line.point(tmax);
    }
  }

  //---

  // show eye line
  if (show) {
    CGLPath3D path;

    path.lineTo(ep1);
    path.lineTo(ep2);
    path.lineTo(ep3);
    path.lineTo(ep4);

    if (add) {
      eyeVector2_ = ev;

      eyeLine2_->setPath(path);
      eyeLine2_->updateGeometry();

      //---

#if 0
      auto *object = getCurrentObject();

      if (object) {
        auto *camera = getCurrentCamera();

        auto bbox = getObjectBBox(object);

        auto cop = camera->origin().vector();
        auto cor = CVector3D(bbox.getCenter());

        auto q = camera->trackBall(cop, cor, eyeVector1_, eyeVector2_);

        CMatrix3D m2;
        q.toRotationMatrix(m2);

        auto m1 = CMatrix3D::translation(-cor.getX(), -cor.getY(), -cor.getZ());
        auto m3 = CMatrix3D::translation( cor.getX(),  cor.getY(),  cor.getZ());

        object->setTransform(object->getTransform()*m1*m2*m3);
      }
#endif

      eyeVector1_ = eyeVector2_;
    }
    else {
      eyeVector1_ = ev;

      eyeLine1_->setPath(path);
      eyeLine1_->updateGeometry();
    }
  }
}

void
CQNewGLCanvas::
calcEyeLine(const CPoint2D &pos, CPoint3D &ep1, CPoint3D &ep2, CVector3D &ev)
{
  auto imatrix1 = projectionMatrix().inverse();

  const auto &viewMatrix = this->viewMatrix();
  auto imatrix2 = viewMatrix.inverse();

  //---

  // set pixel (mouse) position in GL coords
  double x1, y1;

  if (aspect_ > 1.0) {
    x1 = CMathUtil::map(pos.x, 0, pixelWidth_  - 1.0, -aspect_,  aspect_);
    y1 = CMathUtil::map(pos.y, 0, pixelHeight_ - 1.0,      1.0,     -1.0);
  }
  else {
    x1 = CMathUtil::map(pos.x, 0, pixelWidth_  - 1.0,    -1.0,       1.0);
    y1 = CMathUtil::map(pos.y, 0, pixelHeight_ - 1.0,  aspect_, -aspect_);
  }

  auto z1 = 1.0;

  // unobserve point
  float xp1, yp1, zp1;
  imatrix1.multiplyPoint(x1, y1, z1, &xp1, &yp1, &zp1);
//imatrix1.multiplyVector(x1, y1, z1, &xp1, &yp1, &zp1);

  zp1 = -1.0;

  float xv1, yv1, zv1;
//imatrix2.multiplyPoint(xp1, yp1, zp1, &xv1, &yv1, &zv1);
  imatrix2.multiplyVector(xp1, yp1, zp1, &xv1, &yv1, &zv1);

  ev = CVector3D(xv1, yv1, zv1).normalized();

  ep1 = CPoint3D(xv1, yv1, zv1);

  //---

#if 0
  // screen center in GL coords
//auto x2 = x1;
//auto y2 = y1;
  auto x2 = 0.0;
  auto y2 = 0.0;
  auto z2 = 1.0;

  // unobserve points
  float xp2, yp2, zp2;
  imatrix1.multiplyPoint(x2, y2, z2, &xp2, &yp2, &zp2);

  float xv2, yv2, zv2;
  imatrix2.multiplyPoint(xp2, yp2, zp2, &xv2, &yv2, &zv2);

  ep2 = CPoint3D(xv2, yv2, zv2);
#else
  auto *camera = getCurrentCamera();
  ep2 = camera->origin().point();
#endif
}

void
CQNewGLCanvas::
wheelEvent(QWheelEvent *e)
{
  auto dw = e->angleDelta().y()/250.0;

  auto type = app_->type();

  if      (type == CQNewGLModel::Type::CAMERA) {
    auto *camera = getCurrentCamera();

    camera->processMouseScroll(dw);
    updateCameraBuffer();
  }
  else if (type == CQNewGLModel::Type::LIGHT) {
    auto *light = getCurrentLight();

    auto dz = dw*sceneSize_.getZ()/4.0;

    light->setPosition(light->position() + CGLVector3D(0.0f, 0.0f, dz));

    app()->updateLights();
  }
  else if (type == CQNewGLModel::Type::MODEL) {
  }

  update();
}

void
CQNewGLCanvas::
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
  processKeyData.rotate    = cameraRotate_;
  processKeyData.strafe    = isShift;

  auto type = app_->type();

  if      (e->key() == Qt::Key_Q) {
    if      (type == CQNewGLModel::Type::CAMERA) {
      processKeyData.rotateAt =
        (isControl ? CGLCamera::RotateAt::POSITION : CGLCamera::RotateAt::ORIGIN);
      camera->processKeyboard(CGLCamera::Movement::ROTATE_LEFT, processKeyData);
      updateCameraBuffer();
    }
    else if (type == CQNewGLModel::Type::LIGHT) {
    }
    else if (type == CQNewGLModel::Type::MODEL) {
      auto *object = getCurrentObject();

      if (object) {
        CVector3D u, v, w;
        getBasis(object, u, v, w);

        auto bbox = getObjectBBox(object);
        auto o = bbox.getCenter();

        auto da = 1.0;

        auto m1 = CMatrix3D::translation(o.getX(), o.getY(), o.getZ());
        CMatrix3D m2;
        if      (isControl)
          m2 = CMatrix3D::rotation(CMathGen::DegToRad(da), u);
        else if (isShift)
          m2 = CMatrix3D::rotation(CMathGen::DegToRad(da), w);
        else
          m2 = CMatrix3D::rotation(CMathGen::DegToRad(da), v);
        auto m3 = CMatrix3D::translation(-o.getX(), -o.getY(), -o.getZ());
        object->setTransform(m1*m2*m3*object->getTransform());

        if (getCurrentCamera()->isFollowObject())
          placeObjectCamera(object);

        updateObjectData(object);
      }
    }
  }
  else if (e->key() == Qt::Key_E) {
    if      (type == CQNewGLModel::Type::CAMERA) {
      processKeyData.rotateAt =
        (isControl ? CGLCamera::RotateAt::POSITION : CGLCamera::RotateAt::ORIGIN);
      camera->processKeyboard(CGLCamera::Movement::ROTATE_RIGHT, processKeyData);
      updateCameraBuffer();
    }
    else if (type == CQNewGLModel::Type::LIGHT) {
    }
    else if (type == CQNewGLModel::Type::MODEL) {
      auto *object = getCurrentObject();

      if (object) {
        CVector3D u, v, w;
        getBasis(object, u, v, w);

        auto bbox = getObjectBBox(object);
        auto o = bbox.getCenter();

        auto da = 1.0;

        auto m1 = CMatrix3D::translation(o.getX(), o.getY(), o.getZ());
        CMatrix3D m2;
        if      (isControl)
          m2 = CMatrix3D::rotation(-CMathGen::DegToRad(da), u);
        else if (isShift)
          m2 = CMatrix3D::rotation(-CMathGen::DegToRad(da), w);
        else
          m2 = CMatrix3D::rotation(-CMathGen::DegToRad(da), v);
        auto m3 = CMatrix3D::translation(-o.getX(), -o.getY(), -o.getZ());
        object->setTransform(m1*m2*m3*object->getTransform());

        if (getCurrentCamera()->isFollowObject())
          placeObjectCamera(object);

        updateObjectData(object);
      }
    }
  }
  else if (e->key() == Qt::Key_W) {
    if      (type == CQNewGLModel::Type::CAMERA) {
      camera->processKeyboard(CGLCamera::Movement::FORWARD, processKeyData);
      updateCameraBuffer();
    }
    else if (type == CQNewGLModel::Type::LIGHT) {
      auto *light = getCurrentLight();

      light->setPosition(light->position() + CGLVector3D(0.0f, 0.1f, 0.0f));

      app()->updateLights();
    }
    else if (type == CQNewGLModel::Type::MODEL) {
#if 0
      if (isControl) {
        modelTranslate_.setZ(modelTranslate_.getZ() + sceneSize_.z()/100.0);

        Q_EMIT modelMatrixChanged();
      }
#else
      auto *object = getCurrentObject();

      if (object) {
        CVector3D u, v, w;
        getBasis(object, u, v, w);
        auto t = w*sceneScale_/100.0;

        auto m = CMatrix3D::translation(t.getX(), t.getY(), t.getZ());
        object->setTransform(m*object->getTransform());

        if (getCurrentCamera()->isFollowObject())
          placeObjectCamera(object);

        updateObjectData(object);
      }
    }
#endif
  }
  else if (e->key() == Qt::Key_S) {
    if      (type == CQNewGLModel::Type::CAMERA) {
      camera->processKeyboard(CGLCamera::Movement::BACKWARD, processKeyData);
      updateCameraBuffer();
    }
    else if (type == CQNewGLModel::Type::LIGHT) {
      auto *light = getCurrentLight();

      light->setPosition(light->position() - CGLVector3D(0.0f, 0.1f, 0.0f));

      app()->updateLights();
    }
    else if (type == CQNewGLModel::Type::MODEL) {
#if 0
      if (isControl) {
        modelTranslate_.setZ(modelTranslate_.getZ() - sceneSize_.z()/100.0);

        Q_EMIT modelMatrixChanged();
      }
#else
      auto *object = getCurrentObject();

      if (object) {
        CVector3D u, v, w;
        getBasis(object, u, v, w);
        auto t = w*sceneScale_/100.0;

        auto m = CMatrix3D::translation(-t.getX(), -t.getY(), -t.getZ());
        object->setTransform(m*object->getTransform());

        if (getCurrentCamera()->isFollowObject())
          placeObjectCamera(object);

        updateObjectData(object);
      }
    }
#endif
  }
  else if (e->key() == Qt::Key_A) {
    if      (type == CQNewGLModel::Type::CAMERA) {
      camera->processKeyboard(CGLCamera::Movement::STRAFE_LEFT, processKeyData);
      updateCameraBuffer();
    }
    else if (type == CQNewGLModel::Type::LIGHT) {
      auto *light = getCurrentLight();

      light->setPosition(light->position() - CGLVector3D(0.1f, 0.0f, 0.0f));

      app()->updateLights();
    }
    else if (type == CQNewGLModel::Type::MODEL) {
      auto *object = getCurrentObject();

      if (object) {
        CVector3D u, v, w;
        getBasis(object, u, v, w);
        auto t = u*sceneScale_/100.0;

        auto m = CMatrix3D::translation(-t.getX(), -t.getY(), -t.getZ());
        object->setTransform(m*object->getTransform());

        if (getCurrentCamera()->isFollowObject())
          placeObjectCamera(object);

        updateObjectData(object);
      }
    }
  }
  else if (e->key() == Qt::Key_D) {
    if      (type == CQNewGLModel::Type::CAMERA) {
      camera->processKeyboard(CGLCamera::Movement::STRAFE_RIGHT, processKeyData);
      updateCameraBuffer();
    }
    else if (type == CQNewGLModel::Type::LIGHT) {
      auto *light = getCurrentLight();

      light->setPosition(light->position() + CGLVector3D(0.1f, 0.0f, 0.0f));

      app()->updateLights();
    }
    else if (type == CQNewGLModel::Type::MODEL) {
      auto *object = getCurrentObject();

      if (object) {
        CVector3D u, v, w;
        getBasis(object, u, v, w);
        auto t = u*sceneScale_/100.0;

        auto m = CMatrix3D::translation(t.getX(), t.getY(), t.getZ());
        object->setTransform(m*object->getTransform());

        if (getCurrentCamera()->isFollowObject())
          placeObjectCamera(object);

        updateObjectData(object);
      }
    }
  }
  else if (e->key() == Qt::Key_X) {
    if (type == CQNewGLModel::Type::MODEL) {
#if 0
      if (isControl) {
        modelTranslate_.setX(modelTranslate_.getX() + sceneSize_.x()/100.0);
      }
      else {
        modelRotate_.setX(modelRotate_.x() + da);
      }

      Q_EMIT modelMatrixChanged();
#endif
    }
  }
  else if (e->key() == Qt::Key_Y) {
    if (type == CQNewGLModel::Type::MODEL) {
#if 0
      if (isControl) {
        modelTranslate_.setY(modelTranslate_.getY() + sceneSize_.y()/100.0);
      }
      else {
        modelRotate_.setY(modelRotate_.y() + da);
      }

      Q_EMIT modelMatrixChanged();
#endif
    }
  }
  else if (e->key() == Qt::Key_Z) {
    if (type == CQNewGLModel::Type::MODEL) {
#if 0
      if (isControl) {
        modelTranslate_.setZ(modelTranslate_.getZ() + sceneSize_.z()/100.0);
      }
      else {
        modelRotate_.setZ(modelRotate_.z() + da);
      }

      Q_EMIT modelMatrixChanged();
#endif
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
    else if (type == CQNewGLModel::Type::LIGHT) {
      auto *light = getCurrentLight();

      light->setPosition(light->position() + CGLVector3D(0.0f, 0.0f, 0.1f));

      app()->updateLights();
    }
    else if (type == CQNewGLModel::Type::MODEL) {
#if 0
      if (isControl) {
        modelTranslate_.setY(modelTranslate_.getY() + sceneSize_.y()/100.0);
      }
      else {
        double f = 1.1;
        modelScale_.setX(modelScale_.getX()*f);
        modelScale_.setY(modelScale_.getY()*f);
        modelScale_.setZ(modelScale_.getZ()*f);
      }

      Q_EMIT modelMatrixChanged();
#else
      auto *object = getCurrentObject();

      if (object) {
        CVector3D u, v, w;
        getBasis(object, u, v, w);
        auto t = v*sceneScale_/100.0;

        auto m = CMatrix3D::translation(t.getX(), t.getY(), t.getZ());
        object->setTransform(m*object->getTransform());

        if (getCurrentCamera()->isFollowObject())
          placeObjectCamera(object);

        updateObjectData(object);
      }
#endif
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
    else if (type == CQNewGLModel::Type::LIGHT) {
      auto *light = getCurrentLight();

      light->setPosition(light->position() - CGLVector3D(0.0f, 0.0f, 0.1f));

      app()->updateLights();
    }
    else if (type == CQNewGLModel::Type::MODEL) {
#if 0
      if (isControl) {
        modelTranslate_.setY(modelTranslate_.getY() - sceneSize_.y()/100.0);
      }
      else {
        double f = 1.1;
        modelScale_.setX(modelScale_.getX()/f);
        modelScale_.setY(modelScale_.getY()/f);
        modelScale_.setZ(modelScale_.getZ()/f);
      }

      Q_EMIT modelMatrixChanged();
#else
      auto *object = getCurrentObject();

      if (object) {
        CVector3D u, v, w;
        getBasis(object, u, v, w);
        auto t = v*sceneScale_/100.0;

        auto m = CMatrix3D::translation(-t.getX(), -t.getY(), -t.getZ());
        object->setTransform(m*object->getTransform());

        if (getCurrentCamera()->isFollowObject())
          placeObjectCamera(object);

        updateObjectData(object);
      }
#endif
    }
  }
  else if (e->key() == Qt::Key_Left) {
    if      (type == CQNewGLModel::Type::CAMERA) {
    }
    else if (type == CQNewGLModel::Type::LIGHT) {
      auto *light = getCurrentLight();

      light->setPosition(light->position() + CGLVector3D(0.1f, 0.0f, 0.0f));

      app()->updateLights();
    }
    else if (type == CQNewGLModel::Type::MODEL) {
#if 0
      if (isControl) {
        modelTranslate_.setX(modelTranslate_.getX() - sceneSize_.x()/100.0);

        Q_EMIT modelMatrixChanged();
      }
#else
      auto *object = getCurrentObject();

      if (object) {
        CVector3D u, v, w;
        getBasis(object, u, v, w);
        auto t = u*sceneScale_/100.0;

        auto m = CMatrix3D::translation(-t.getX(), -t.getY(), -t.getZ());
        object->setTransform(m*object->getTransform());

        if (getCurrentCamera()->isFollowObject())
          placeObjectCamera(object);

        updateObjectData(object);
      }
#endif
    }
  }
  else if (e->key() == Qt::Key_Right) {
    if      (type == CQNewGLModel::Type::CAMERA) {
    }
    else if (type == CQNewGLModel::Type::LIGHT) {
      auto *light = getCurrentLight();

      light->setPosition(light->position() - CGLVector3D(0.1f, 0.0f, 0.0f));

      app()->updateLights();
    }
    else if (type == CQNewGLModel::Type::MODEL) {
#if 0
      if (isControl) {
        modelTranslate_.setX(modelTranslate_.getX() + sceneSize_.x()/100.0);

        Q_EMIT modelMatrixChanged();
      }
#else
      auto *object = getCurrentObject();

      if (object) {
        CVector3D u, v, w;
        getBasis(object, u, v, w);
        auto t = u*sceneScale_/100.0;

        auto m = CMatrix3D::translation(t.getX(), t.getY(), t.getZ());
        object->setTransform(m*object->getTransform());

        if (getCurrentCamera()->isFollowObject())
          placeObjectCamera(object);

        updateObjectData(object);
      }
#endif
    }
  }

  update();
}

void
CQNewGLCanvas::
placeObjectCamera(CGeomObject3D *object)
{
  CVector3D u, v, w;
  getBasis(object, u, v, w);

  auto bbox = getObjectBBox(object);
  auto o = bbox.getCenter();

  auto *currentCamera = getCurrentCamera();

  auto p = o - w*bbox.getSize();

  currentCamera->setOrigin(CGLVector3D(o.x, o.y, o.z));
  currentCamera->setPosition(CGLVector3D(p.x, p.y, p.z));

  updateCameraBuffer();
}

CBBox3D
CQNewGLCanvas::
getObjectBBox(CGeomObject3D *object) const
{
  auto bbox = getModelBBox(object);

  auto modelMatrix = object->getHierTransform();

  const auto &pmin = bbox.getMin();
  const auto &pmax = bbox.getMax();

  std::vector<CPoint3D> points;

  auto addPoint = [&](double x, double y, double z) {
    points.push_back(CPoint3D(x, y, z));
  };

  addPoint(pmin.getX(), pmin.getY(), pmin.getZ());
  addPoint(pmax.getX(), pmin.getY(), pmin.getZ());
  addPoint(pmax.getX(), pmax.getY(), pmin.getZ());
  addPoint(pmin.getX(), pmax.getY(), pmin.getZ());
  addPoint(pmin.getX(), pmin.getY(), pmax.getZ());
  addPoint(pmax.getX(), pmin.getY(), pmax.getZ());
  addPoint(pmax.getX(), pmax.getY(), pmax.getZ());
  addPoint(pmin.getX(), pmax.getY(), pmax.getZ());

  CBBox3D bbox1;

  for (const auto &p : points)
    bbox1.add(modelMatrix*p);

  return bbox1;
}

CBBox3D
CQNewGLCanvas::
getModelBBox(CGeomObject3D *object) const
{
  CBBox3D bbox;
  object->getModelBBox(bbox);

#if 0
  const auto &modelMatrix = object->getHierTransform();
  auto p1 = modelMatrix*bbox.getMin();
  auto p2 = modelMatrix*bbox.getMax();

  return CBBox3D(p1, p2);
#endif

  return bbox;
}

void
CQNewGLCanvas::
getBasis(CGeomObject3D *object, CVector3D &u, CVector3D &v, CVector3D &w) const
{
  CVector3D u1, v1, w1;
  object->getBasis(u1, v1, w1);

  const auto &modelMatrix = object->getHierTransform();

  u = (modelMatrix*u1).normalized();
  v = (modelMatrix*v1).normalized();
  w = (modelMatrix*w1).normalized();
}

//---

void
CQNewGLCanvas::
addCube(CQGLBuffer *buffer, const CPoint3D &center, double size, const CRGBA &color,
        std::vector<CQNewGLFaceData> &faceDatas) const
{
  CBBox3D bbox(center.getX() - size/2.0, center.getY() - size/2.0, center.getZ() - size/2.0,
               center.getX() + size/2.0, center.getY() + size/2.0, center.getZ() + size/2.0);

  addCube(buffer, bbox, color, faceDatas);
}

void
CQNewGLCanvas::
addCube(CQGLBuffer *buffer, const CBBox3D &bbox, const CRGBA &color,
        std::vector<CQNewGLFaceData> &faceDatas) const
{
  static double cube_normal[6][3] = {
    {-1.0,  0.0,  0.0},
    { 0.0,  1.0,  0.0},
    { 1.0,  0.0,  0.0},
    { 0.0, -1.0,  0.0},
    { 0.0,  0.0,  1.0},
    { 0.0,  0.0, -1.0}
  };

  static int cube_faces[6][4] = {
    {0, 1, 2, 3},
    {3, 2, 6, 7},
    {7, 6, 5, 4},
    {4, 5, 1, 0},
    {5, 6, 2, 1},
    {7, 4, 0, 3}
  };

  double v[8][3];

  v[0][0] = v[1][0] = v[2][0] = v[3][0] = -0.5;
  v[4][0] = v[5][0] = v[6][0] = v[7][0] =  0.5;
  v[0][1] = v[1][1] = v[4][1] = v[5][1] = -0.5;
  v[2][1] = v[3][1] = v[6][1] = v[7][1] =  0.5;
  v[0][2] = v[3][2] = v[4][2] = v[7][2] = -0.5;
  v[1][2] = v[2][2] = v[5][2] = v[6][2] =  0.5;

  //---

  auto x_size = bbox.getXSize();
  auto y_size = bbox.getYSize();
  auto z_size = bbox.getZSize();
  auto center = bbox.getCenter();

  //---

  auto cubePoint = [&](int i, int j) {
    const auto &v1 = v[cube_faces[i][j]];

    return CPoint3D(center.x + x_size*v1[0], center.y + y_size*v1[1], center.z + z_size*v1[2]);
  };

  auto cubeNormal = [&](int i) {
    const auto &n = cube_normal[i];

    return CPoint3D(n[0], n[1], n[2]);
  };

  auto addPoint = [&](const CPoint3D &p, const CPoint3D &n, const CRGBA &c, const CPoint2D &tp) {
    buffer->addPoint(p.x, p.y, p.z);
    buffer->addNormal(n.x, n.y, n.z);
    buffer->addColor(c);
    buffer->addTexturePoint(tp.x, tp.y);
  };

  auto tp1 = CPoint2D(0.0, 0.0);
  auto tp2 = CPoint2D(1.0, 0.0);
  auto tp3 = CPoint2D(1.0, 1.0);
  auto tp4 = CPoint2D(0.0, 1.0);

  int pos = 0;

  for (GLint i = 5; i >= 0; i--) {
    CQNewGLFaceData faceData;

    faceData.pos = pos;
    faceData.len = 4;

    auto p1 = cubePoint(i, 0);
    auto p2 = cubePoint(i, 1);
    auto p3 = cubePoint(i, 2);
    auto p4 = cubePoint(i, 3);

    auto normal = cubeNormal(i);

    addPoint(p1, normal, color, tp1);
    addPoint(p2, normal, color, tp2);
    addPoint(p3, normal, color, tp3);
    addPoint(p4, normal, color, tp4);

    faceDatas.push_back(faceData);

    pos += faceData.len;
  }
}

void
CQNewGLCanvas::
addCone(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, double w,
        const CRGBA &color, std::vector<CQNewGLFaceData> &faceDatas, int &pos) const
{
  auto *object = new CGeomObject3D(nullptr, "");

  auto h = p1.distanceTo(p2);

  CGeomCone3D::addGeometry(object, CPoint3D(0.0, 0.0, 0.0), w, h, 20);
  CGeomCone3D::addNormals(object, w, h);

  auto m1 = CMatrix3D::translation(0, h/2.0, 0.0);
  auto m2 = getShapeRotationMatrix(p1, p2);;
  auto m3 = CMatrix3D::translation(p1.x, p1.y, p1.z);

  auto modelMatrix = m3*m2*m1;

  auto addPoint = [&](const CPoint3D &p, const CVector3D &n, const CRGBA &c, const CPoint2D &tp) {
    auto p1 = modelMatrix*p;

    buffer->addPoint(p1.x, p1.y, p1.z);
    buffer->addNormal(n.getX(), n.getY(), n.getZ());
    buffer->addColor(c);
    buffer->addTexturePoint(tp.x, tp.y);
  };

  for (auto *face : object->getFaces()) {
    const auto &vertices = face->getVertices();

    CQNewGLFaceData faceData;

    faceData.pos = pos;
    faceData.len = vertices.size();

    for (auto vertexInd : vertices) {
      auto *vertex = object->getVertexP(vertexInd);

      addPoint(vertex->getModel(), vertex->getNormal(), color, vertex->getTextureMap());
    }

    faceDatas.push_back(faceData);

    pos += faceData.len;
  }

  delete object;
}

void
CQNewGLCanvas::
addCylinder(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, double r,
            const CRGBA &color, std::vector<CQNewGLFaceData> &faceDatas, int &pos) const
{
  auto *object = new CGeomObject3D(nullptr, "");

  auto h = p1.distanceTo(p2);

  CGeomCylinder3D::addGeometry(object, CPoint3D(0.0, 0.0, 0.0), 2*r, h, 20);
  CGeomCylinder3D::addNormals(object, 2*r, h);

  auto m1 = CMatrix3D::translation(0.0, h/2.0, 0.0);
  auto m2 = getShapeRotationMatrix(p1, p2);;
  auto m3 = CMatrix3D::translation(p1.x, p1.y, p1.z);

  auto modelMatrix = m3*m2*m1;

  auto addPoint = [&](const CPoint3D &p, const CVector3D &n, const CRGBA &c, const CPoint2D &tp) {
    auto p1 = modelMatrix*p;

    buffer->addPoint(p1.x, p1.y, p1.z);
    buffer->addNormal(n.getX(), n.getY(), n.getZ());
    buffer->addColor(c);
    buffer->addTexturePoint(tp.x, tp.y);
  };

  for (auto *face : object->getFaces()) {
    const auto &vertices = face->getVertices();

    CQNewGLFaceData faceData;

    faceData.pos = pos;
    faceData.len = vertices.size();

    for (auto vertexInd : vertices) {
      auto *vertex = object->getVertexP(vertexInd);

      addPoint(vertex->getModel(), vertex->getNormal(), color, vertex->getTextureMap());
    }

    faceDatas.push_back(faceData);

    pos += faceData.len;
  }

  delete object;
}

void
CQNewGLCanvas::
addSphere(CQGLBuffer *buffer, const CPoint3D &c, double r,
          const CRGBA &color, std::vector<CQNewGLFaceData> &faceDatas, int &pos) const
{
  auto *object = new CGeomObject3D(nullptr, "");

  CGeomSphere3D::addGeometry(object, CPoint3D(0, 0, 0), r);
  CGeomSphere3D::addNormals(object, r);
  CGeomSphere3D::addTexturePoints(object);

  auto modelMatrix = CMatrix3D::translation(c.x, c.y, c.z);

  auto addPoint = [&](const CPoint3D &p, const CVector3D &n, const CRGBA &c, const CPoint2D &tp) {
    auto p1 = modelMatrix*p;

    buffer->addPoint(p1.x, p1.y, p1.z);
    buffer->addNormal(n.getX(), n.getY(), n.getZ());
    buffer->addColor(c);
    buffer->addTexturePoint(tp.x, tp.y);
  };

  for (auto *face : object->getFaces()) {
    const auto &vertices = face->getVertices();

    CQNewGLFaceData faceData;

    faceData.pos = pos;
    faceData.len = vertices.size();

    const auto &texturePoints = face->getTexturePoints();

    size_t iv = 0;

    for (auto vertexInd : vertices) {
      CPoint2D tp;

      if (iv < texturePoints.size())
        tp = texturePoints[iv];

      auto *vertex = object->getVertexP(vertexInd);

      addPoint(vertex->getModel(), vertex->getNormal(), color, tp);

      ++iv;
    }

    faceDatas.push_back(faceData);

    pos += faceData.len;
  }

  delete object;
}

CMatrix3D
CQNewGLCanvas::
getShapeRotationMatrix(const CPoint3D &p1, const CPoint3D &p2) const
{
  auto v1 = CVector3D(0, 1, 0); v1.normalize();
  auto v2 = CVector3D(p1, p2) ; v2.normalize();

  auto q = CQuaternion::rotationArc(v1, v2);

  CMatrix3D m;
  q.toRotationMatrix(m);

  return m;
}
