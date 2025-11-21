#include <CQNewGLCanvas.h>
#include <CQNewGLModel.h>
#include <CQNewGLModelObject.h>
#include <CQNewGLFont.h>
#include <CQNewGLCamera.h>
#include <CQNewGLLight.h>
#include <CQNewGLAxes.h>
#include <CQNewGLBBox.h>
#include <CQNewGLNormals.h>
#include <CQNewGLWireframe.h>
#include <CQNewGLHull.h>
#include <CQNewGLBasis.h>
#include <CQNewGLTerrain.h>
#include <CQNewGLMaze.h>
#include <CQNewGLSkybox.h>
#include <CQNewGLEmitter.h>
#include <CQNewGLFractal.h>
#include <CQNewGLDrawTree.h>
#include <CQNewGLShape.h>
#include <CQNewGLPath.h>
#include <CQNewGLToolbar.h>
#include <CQNewGLStatusBar.h>
#include <CQNewGLUtil.h>

#include <CImportBase.h>
#include <CQGLBuffer.h>
#include <CQGLTexture.h>
#include <CGeometry3D.h>
#include <CGeomScene3D.h>
#include <CGeomTexture.h>
#include <CGeomCone3D.h>
#include <CGeomCylinder3D.h>
#include <CGeomPyramid3D.h>
#include <CGeomSphere3D.h>
#include <CGeomTorus3D.h>
#include <CQGLUtil.h>
#include <CFile.h>
#include <CSphere3D.h>
#include <CStrUtil.h>

#include <QOpenGLShaderProgram>
#include <QMouseEvent>
#include <QTimer>

#include <GL/glu.h>

#include <set>

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
  bonesData_.showPoints = b;
}

void
CQNewGLCanvas::
setShowBoneVertices(bool b)
{
  bonesData_.showVertices = b;
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
  for (auto &po : objectDatas_) {
    auto *objectData = po.second;

    if (objectData->object() == object) {
      setCurrentObjectNum(objectData->ind());
      return;
    }
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

CQNewGLModelObject *
CQNewGLCanvas::
getCurrentObjectData() const
{
  if (objectDatas_.empty())
    return nullptr;

  auto *objectData = getObjectData(currentObjectNum_);

  if (! objectData)
    objectData = objectDatas_.begin()->second;

  return objectData;
}

CGeomObject3D *
CQNewGLCanvas::
getObject(int ind) const
{
  auto *objectData = getObjectData(ind);

  if (objectData)
    return objectData->object();

  return nullptr;
}

CQNewGLModelObject *
CQNewGLCanvas::
getObjectData(int ind) const
{
  for (auto &po : objectDatas_) {
    auto *objectData = po.second;

    if (objectData->ind() == ind)
      return objectData;
  }

  return nullptr;
}

CQNewGLModelObject *
CQNewGLCanvas::
getObjectData(CGeomObject3D *object)
{
  if (! object)
    return nullptr;

  auto pd = objectDatas_.find(object);

  if (pd == objectDatas_.end()) {
    auto *objectData1 = new CQNewGLModelObject(this);

    objectData1->setInd(objectDatas_.size() + 1);

    pd = objectDatas_.insert(pd, ObjectDatas::value_type(object, objectData1));
  }

  auto *objectData = (*pd).second;

  if (! objectData->object())
    objectData->setObject(object);

  return objectData;
}

CGeomObject3D *
CQNewGLCanvas::
getSelectedObject() const
{
  for (auto &po : objectDatas_) {
    auto *objectData = po.second;

    if (objectData->object()->getSelected())
      return objectData->object();
  }

  return nullptr;
}

#if 0
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
#endif

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
loadModel(const QString &fileName, CGeom3DType format, LoadData &loadData)
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

  for (auto *object : scene->getObjects()) {
    scene_->addObject(object);

    loadData.objects.push_back(object);
  }

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

  CQNewGLModelObject::initShader(this);

  //---

  // lights
  addLight(/*update*/false);

  //---

  loadInitTextures();

  //---

  font_ = new CQNewGLFont(this);

  font_->init();

  font_->setSize(48);
  font_->setFontName("OpenSans-Regular.ttf");

  //---

  axes_ = new CQNewGLAxes(this);
  axes_->setVisible(false);

  CQNewGLAxes::initShader(this);

  //---

  bbox_ = new CQNewGLBBox(this);
  bbox_->setVisible(false);

  CQNewGLBBox::initShader(this);

  //------

  // annotation objects

  normals_ = new CQNewGLNormals(this);
  normals_->setVisible(false);

  addAnnotationObject(normals_);

  //--

  wireframe_ = new CQNewGLWireframe(this);
  wireframe_->setVisible(false);

  addAnnotationObject(wireframe_);

  //--

  hull_ = new CQNewGLHull(this);
  hull_->setVisible(false);

  CQNewGLHull::initShader(this);

  addAnnotationObject(hull_);

  //------

  basis_ = new CQNewGLBasis(this);
  basis_->setVisible(false);

  CQNewGLBasis::initShader(this);

  //---

  terrain_ = new CQNewGLTerrain(this);
  terrain_->setVisible(false);

  CQNewGLTerrain::initShader(this);

  //---

  maze_ = new CQNewGLMaze(this);
  maze_->setVisible(false);

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
  CQNewGLCamera *camera = nullptr;

  for (auto *camera1 : cameras_) {
    if (! camera) {
      camera = camera1;

      camera->bindShader();
    }

    camera1->drawGeometry();
  }

  if (camera)
    camera->unbindShader();
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
  CQNewGLLight *light = nullptr;

  for (auto *light1 : lights()) {
    if (! light) {
      light = light1;

      light->bindShader();
    }

    light1->drawGeometry();
  }

  if (light)
    light->unbindShader();
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
updateMaze(CBBox3D &bbox)
{
  if (! maze_->isValid())
    maze_->updateGeometry();

  bbox += maze_->getBBox();
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

  emitter->setEnabled(true);

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

    emitter->updateGeometry();
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

  updateAnnotationObjects();

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

  currentShape_ = shapes_.size() - 1;

  Q_EMIT shapeAdded();

  return shape;
}

void
CQNewGLCanvas::
deleteShape(CQNewGLShape *shape)
{
  Shapes shapes;

  for (auto *shape1 : shapes_)
    if (shape1 != shape)
      shapes.push_back(shape1);

  std::swap(shapes, shapes_);

  delete shape;

  Q_EMIT shapeDeleted();
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

  CBBox3D bbox;

  if      (app_->isShowBone())
    updateObjectBone(bbox);
  else if (isBonesEnabled())
    updateObjectBones(bbox);
  else
    updateModelData(bbox);

  //---

  for (auto *shape : shapes_) {
    bbox += shape->getBBox();
  }

  //---

  // annotations
//updateAxes();
  axes_->setValid(false);

  if (maze_ && maze_->isVisible())
    updateMaze(bbox);

  if (bbox_->isVisible())
    updateBBox();

  if (normals_->isVisible())
    updateNormals();

  if (wireframe_->isVisible())
    updateWireframe();

  if (hull_->isVisible())
    updateHull();

  //---

  basis_->setValid(false);

  setSceneBBox(bbox);
}

void
CQNewGLCanvas::
updateObjectData(CGeomObject3D *)
{
  updateObjectsData();
}

void
CQNewGLCanvas::
updateModelData(CBBox3D &bbox)
{
  // set up vertex data (and buffer(s)) and configure vertex attributes
  if (scene_->getObjects().empty()) {
    sceneScale_    = 1.0;
    invSceneScale_ = 1.0;
    return;
  }

  //---

//CBBox3D bbox1;
//scene_->getBBox(bbox1);

  //---

  bool useBones         = (isAnimEnabled() && animName() != "");
  bool showNormals      = isShowNormals();
  bool showBonePoints   = (useBones && isShowBonePoints());
  bool showBoneVertices = isShowBoneVertices();

  int    boneNodeIds[4];
  double boneWeights[4];

  auto normalsSize  = calcNormalsSize();
  auto normalsColor = this->normalsColor();

  //---

  const auto &objects = scene_->getObjects();

  for (auto *object : objects) {
    auto *objectData = getObjectData(object);

    //---

    objectData->initBuffer();

    if (showNormals)
      objectData->initNormalBuffer();

    if (showBoneVertices)
      objectData->initAnnotationBuffer();

    //---

#if 1
    auto meshMatrix = getMeshGlobalTransform(objectData->object(), /*invert*/false);
#else
    auto meshMatrix = CMatrix3D::identity();

    if (useBones)
      meshMatrix = getMeshGlobalTransform(objectData->object(), /*invert*/true);
#endif

    //---

    auto *modelBuffer = objectData->buffer();

    //---

    CQGLBuffer *normalBuffer = nullptr;

    if (showNormals) {
      objectData->clearNormalData();

      normalBuffer = objectData->normalData().buffer;
    }

    //---

    CQGLBuffer *annotationBuffer = nullptr;

    if (showBoneVertices) {
      objectData->clearAnnotationData();

      annotationBuffer = objectData->annotationData().buffer;
    }

    //---

    auto *rootObject = object->getRootObject();

    //---

    int nodeId = -1;

#if 0
    const auto &nodes = rootObject->getNodes();

    for (const auto &pn : nodes) {
      const auto &node = pn.second;
      if (node.valid) {
        nodeId = pn.first;
        break;
      }
    }
    //std::cerr << "Node Id: " << nodeId << "(" << rootObject->mapNodeId(nodeId) << ")\n";
#else
    const auto &nodeIds = rootObject->getNodeIds();

    if (! nodeIds.empty())
      nodeId = nodeIds[0];
#endif

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
          auto zSign = (objectData->isInvertDepth() ? -1 : 1);

          if (! objectData->isFlipYZ())
            p1 = CPoint3D(p.x, p.y, zSign*p.z);
          else
            p1 = CPoint3D(p.x, p.z, zSign*p.y);

          p2 = meshMatrix*p1;
        };

        auto calcNormal = [&](const CVector3D &n) {
          auto zSign = (objectData->isInvertDepth() ? -1 : 1);

          CVector3D n1;

          if (! objectData->isFlipYZ())
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

      objectData->addFaceData(faceData);

      ++iv;
    }

    //---

    if (showBoneVertices) {
      BoneData boneData;
      getBoneData(object, currentBone(), boneData);

      int pos1 = 0;

      for (const auto &v : boneData.vertices) {
        auto v1 = meshMatrix*v;

        auto cubeSize = sceneScale_/100.0;

        CBBox3D bbox;
        bbox.add(v1 - cubeSize*CPoint3D(0.5, 0.5, 0.5));
        bbox.add(v1 + cubeSize*CPoint3D(0.5, 0.5, 0.5));

        ShapeData shapeData;
        shapeData.color = CRGBA(1, 0, 0);
        std::vector<CQNewGLFaceData> faceDatas;
        addCube(annotationBuffer, bbox, shapeData, faceDatas, pos1);

        for (const auto &faceData : faceDatas)
          objectData->addAnnotationFaceData(faceData);
      }
    }

    //---

    modelBuffer->load();

    if (showNormals)
      normalBuffer->load();

    if (showBoneVertices)
      annotationBuffer->load();

    //---

#if 0
    if (! object->parent()) {
      auto bbox1 = getObjectBBox(object);

      bbox += bbox1;
    }
#else
    auto bbox1 = getObjectBBox(object);

    bbox += bbox1;
#endif
  }

  //---

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

//---

void
CQNewGLCanvas::
updateObjectBones(CBBox3D &bbox)
{
  // set up vertex data (and buffer(s)) and configure vertex attributes
  if (scene_->getObjects().empty()) {
    sceneScale_    = 1.0;
    invSceneScale_ = 1.0;
    return;
  }

  //---

//CBBox3D bbox1;
//scene_->getBBox(bbox1);

  //---

  // transform node to model coords
  auto getNodeTransform = [&](const CGeomNodeData &nodeData) {
    if      (bonesTransform() == BonesTransform::INVERSE_BIND)
      return nodeData.inverseBindMatrix.inverse();
    else if (bonesTransform() == BonesTransform::LOCAL)
      return nodeData.localTransform;
    else if (bonesTransform() == BonesTransform::GLOBAL)
      return nodeData.globalTransform;
    else
      return CMatrix3D::identity();
  };

#if 0
  auto transformNode = [&](const CGeomNodeData &nodeData) {
    auto m = getNodeTransform(nodeData);

    return m*CPoint3D(0.0, 0.0, 0.0);
  };
#endif

  //---

  auto *font = this->getFont();

  auto cubeSize = 0.10f;
  auto lineSize = 0.05f;

  auto ts1 = cubeSize;

  CRGBA lineColor { 1.0, 1.0, 0.0 };

  //---

  // process root objects
  auto rootObjects = getRootObjects();

  for (auto *rootObject : rootObjects) {
#if 0
    auto hierTransformNode = [&](const CGeomNodeData &nodeData) {
      auto m = getNodeTransform(nodeData);

      auto parentNodeId = nodeData.parent;

      while (parentNodeId >= 0) {
        const auto &parentNodeData = rootObject->getNode(parentNodeId);

        auto m1 = getNodeTransform(parentNodeData);

        m = m1*m;

        parentNodeId = parentNodeData.parent;
      }

      return m*CPoint3D(0.0, 0.0, 0.0);
    };
#endif

    //---

    auto *objectData = getObjectData(rootObject);

    //---

    objectData->initBonesBuffer();

    //---

    objectData->clearBonesData();

    auto *buffer = objectData->bonesData().buffer;

    //---

    auto createText = [&](const std::string &str, const CPoint3D &pos, double size) {
      auto *text = new CQNewGLText(QString::fromStdString(str));

      text->setFont(font);
      text->setColor(CQNewGLFont::Color(1, 1, 1));
      text->setPosition(CGLVector3D(pos.x, pos.y, pos.z));
      text->setSize(size);

      objectData->addBonesText(text);

      return text;
    };

    //---

    const auto &nodeIds = rootObject->getNodeIds();

    int pos = 0;

    for (const auto &nodeId : nodeIds) {
      const auto &node = rootObject->getNode(nodeId);
      if (! node.valid) continue;

      if (! node.isJoint)
        continue;

      //---

#if 1
      auto m = getNodeTransform(node);
//    auto c = transformNode(node);
      auto c = m*CPoint3D(0.0, 0.0, 0.0);

      m = CMatrix3D::translation(c.x, c.y, c.z);
#else
      auto c = hierTransformNode(node);
#endif

      //---

      auto color = (node.selected ? bonesSelectedColor() : bonesColor());

      addBonesCube(objectData, m, cubeSize, nodeId, color, node.selected, pos, bbox);

      //---

      if (node.parent >= 0) {
        const auto &pnode = rootObject->getNode(node.parent);

        if (pnode.valid && pnode.isJoint) {
#if 1
          auto m1 = getNodeTransform(pnode);
        //auto c1 = transformNode(pnode);
          auto c1 = m1*CPoint3D(0.0, 0.0, 0.0);
#else
          auto c1 = hierTransformNode(pnode);
#endif

          addBonesLine(objectData, c, c1, lineSize, nodeId, node.parent, lineColor, pos);
        }
      }

      //---

      auto c1 = c + CPoint3D(0.0, cubeSize/2.0, cubeSize/2.0);

      (void) createText(node.name, c1, ts1);
    }

    for (auto *text : objectData->bonesData().texts)
      text->updateText();

    buffer->load();
  }
}

void
CQNewGLCanvas::
addBonesCube(CQNewGLModelObject *objectData, const CMatrix3D &m, double cubeSize, int nodeId,
             const CRGBA &color, bool selected, int &pos, CBBox3D &bbox) const
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

  // TODO: Normal  = mat3(transpose(inverse(model)))*norm;

  //---

  auto *buffer = objectData->bonesData().buffer;

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
    buffer->addTexturePoint(0.0f, 0.0f);

    bbox += CPoint3D(p1.x, p1.y, p1.z);
  };

  for (GLint i = 5; i >= 0; i--) {
    CQNewGLFaceData faceData;

    faceData.type         = CQNewGLFaceData::Type::CUBE;
    faceData.selected     = selected;
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

    objectData->addBonesFaceData(faceData);

    pos += faceData.len;
  }
}

void
CQNewGLCanvas::
addBonesLine(CQNewGLModelObject *objectData, const CPoint3D &c1, const CPoint3D &c2, double size,
             int boneId, int parentBoneId, const CRGBA &color, int &pos) const
{
  auto *buffer = objectData->bonesData().buffer;

  auto addPoint = [&](const CPoint3D &p, const CPoint3D &n, const CRGBA &c, int id) {
    buffer->addPoint(p.x, p.y, p.z);
    buffer->addNormal(n.x, n.y, n.z);
    buffer->addColor(c);
    buffer->addTexturePoint(float(id), 0.0f);
  };

  CQNewGLFaceData faceData;

  faceData.type         = CQNewGLFaceData::Type::LINE;
  faceData.boneId       = boneId;
  faceData.parentBoneId = parentBoneId;
  faceData.pos          = pos;
  faceData.len          = 4;

  auto dp = CPoint3D(size, 0, 0);

  auto p1 = c1 - dp;
  auto p2 = c1 + dp;
  auto p3 = c2 + dp;
  auto p4 = c2 - dp;
  auto n  = CPoint3D(0, 0, 1);

  addPoint(p1, n, color, 0);
  addPoint(p2, n, color, 0);
  addPoint(p3, n, color, 1);
  addPoint(p4, n, color, 1);

  objectData->addBonesFaceData(faceData);

  pos += faceData.len;
}

void
CQNewGLCanvas::
drawObjectsBones()
{
  auto rootObjects = getRootObjects();

  for (auto *rootObject : rootObjects) {
    auto *objectData = getObjectData(rootObject);

    drawObjectBones(objectData);
  }
}

void
CQNewGLCanvas::
drawObjectBones(CQNewGLModelObject *objectData)
{
  if (! objectData->bonesData().buffer)
    return;

  //---

  auto *object     = objectData->object();
  auto *rootObject = object->getRootObject();

  // update bone nodes
  bool useBones = (isAnimEnabled() && animName() != "");

  if (useBones) {
    if (! rootObject->updateNodesAnimationData(animName().toStdString(), time()))
      useBones = false;
  }

  //---

//auto isSelected = object->getSelected();

  objectData->bindBonesData();

  //---

  auto *program = objectData->bonesShaderProgram();

  program->setUniformValue("viewPos", CQGLUtil::toVector(viewPos()));

  auto modelMatrix = getModelMatrix();
  addShaderMVP(program, modelMatrix);

  //---

  addShaderCurrentLight(program);

  addShaderLightGlobals(program);

  //---

  // bones transform
  if (useBones) {
    updateNodeMatrices(objectData);

    program->setUniformValueArray("globalBoneTransform",
      &paintData_.nodeQMatrices[0], paintData_.numNodeMatrices);
  }

  //---

  program->setUniformValue("useBones", useBones);

  //---

  // render model
  for (const auto &faceData : objectData->bonesData().faceDatas) {
    int boneInd       = rootObject->mapNodeId(faceData.boneId);
    int parentBoneInd = rootObject->mapNodeId(faceData.parentBoneId);

    program->setUniformValue("boneId"      , boneInd);
    program->setUniformValue("parentBoneId", parentBoneInd);

    // material
    program->setUniformValue("shininess", float(faceData.shininess));

    //---

    bool isLine = (faceData.type == CQNewGLFaceData::Type::LINE);

    program->setUniformValue("isLine"    , isLine);
//  program->setUniformValue("isSelected", faceData.selected);
    program->setUniformValue("isSelected", false);

    if (objectData->isSolid()) {
      program->setUniformValue("isWireframe", 0);

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

      glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
    }

    if (objectData->isWireframe()) {
      program->setUniformValue("isWireframe", 1);

      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

      glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
    }
  }

  objectData->unbindBonesData();

  //---

  using Texts       = std::vector<CQNewGLText *>;
  using SortedTexts = std::map<double, Texts>;

  SortedTexts sortedTexts;

  for (auto *text : objectData->bonesData().texts) {
    const auto &p = text->position();

    sortedTexts[p.getZ()].push_back(text);
  }

  for (const auto &pt : sortedTexts) {
    for (auto *text : pt.second)
      text->render();
  }
}

//---

void
CQNewGLCanvas::
updateObjectBone(CBBox3D &bbox)
{
  // set up vertex data (and buffer(s)) and configure vertex attributes
  if (scene_->getObjects().empty()) {
    sceneScale_    = 1.0;
    invSceneScale_ = 1.0;
    return;
  }

  //---

//CBBox3D bbox1;
//scene.getBBox(bbox1);

  //---

  int boneInd = app_->boneInd();

  CGeomObject3D *rootObject = nullptr;

  if (! objectDatas_.empty())
    rootObject = objectDatas_.begin()->first->getRootObject();

  assert(rootObject);

  //---

  auto *objectData = getObjectData(rootObject);

  //---

  objectData->initBoneBuffer();

  //---

  objectData->clearBoneData();

  auto *buffer = objectData->boneData().buffer;

  //---

  //auto cubeSize = sceneScale_/2.0;
  auto cubeSize = 1.0;

  auto color1 = CRGBA(1.0, 0.0, 0.0);
  auto color2 = CRGBA(0.0, 1.0, 0.0);

  int pos = 0;

  addBoneCube(objectData, app_->boneMatrix(), cubeSize, boneInd, color1, pos, bbox);

#if 0
  auto m = CMatrix3D::translation(sceneCenter_.getX(), sceneCenter_.getY(), sceneCenter_.getZ());

  addBoneCube(objectData, m, cubeSize, boneInd, color2, pos, bbox);
#else
  addBoneCube(objectData, CMatrix3D::identity(), cubeSize, boneInd, color2, pos, bbox);
#endif

  buffer->load();
}

void
CQNewGLCanvas::
addBoneCube(CQNewGLModelObject *objectData, const CMatrix3D &m, double cubeSize, int nodeId,
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

  // TODO: Normal  = mat3(transpose(inverse(model)))*norm;

  //---

  auto *buffer = objectData->boneData().buffer;

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
    buffer->addTexturePoint(0.0f, 0.0f);

    bbox += CPoint3D(p1.x, p1.y, p1.z);
  };

  for (GLint i = 5; i >= 0; i--) {
    CQNewGLFaceData faceData;

    faceData.type         = CQNewGLFaceData::Type::CUBE;
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

    objectData->addBoneFaceData(faceData);

    pos += faceData.len;
  }
}

void
CQNewGLCanvas::
drawObjectBone(CQNewGLModelObject *objectData)
{
  auto *object     = objectData->object();
  auto *rootObject = object->getRootObject();

  const auto &nodes = rootObject->getNodes();
  if (nodes.empty()) return;

  //---

  if (! objectData->boneData().buffer)
    return;

  //---

  objectData->bindBoneData();

  //---

  auto *program = objectData->boneShaderProgram();

  program->setUniformValue("viewPos", CQGLUtil::toVector(viewPos()));

  auto modelMatrix = getModelMatrix();

  if (isShowBoneVertices())
    modelMatrix = object->getHierTransform();

  addShaderMVP(program, modelMatrix);

  //---

  addShaderCurrentLight(program);

  addShaderLightGlobals(program);

  //---

  // render model
  for (const auto &faceData : objectData->boneData().faceDatas) {
    // material
    program->setUniformValue("shininess", float(faceData.shininess));

    //---

#if 0
    if (isBoneSolid()) {
      program->setUniformValue("isWireframe", 0);

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

      glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
    }

    if (isBoneWireframe()) {
      program->setUniformValue("isWireframe", 1);

      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

      glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
    }
#else
    program->setUniformValue("isWireframe", 1);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
#endif
  }

  //---

  objectData->unbindBoneData();
}

void
CQNewGLCanvas::
drawObjectAnnotation(CQNewGLModelObject *objectData)
{
  if (! objectData->annotationData().buffer)
    return;

  objectData->bindAnnotationData();

  auto *object = objectData->object();

  //---

  auto *annotationShaderProgram = objectData->annotationShaderProgram();

  auto modelMatrix = object->getHierTransform();
  addShaderMVP(annotationShaderProgram, modelMatrix);

  //---

  // draw annotations
  for (const auto &faceData : objectData->annotationData().faceDatas) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
  }

  objectData->unbindAnnotationData();
}

//---

void
CQNewGLCanvas::
setSceneBBox(const CBBox3D &bbox)
{
  if (! bbox.isSet())
    return;

  sceneSize_   = bbox.getSize();
  sceneCenter_ = bbox.getCenter();

  sceneScale_    = max3(sceneSize_.getX(), sceneSize_.getY(), sceneSize_.getZ());
  invSceneScale_ = (sceneScale_ > 0.0 ? 1.0/sceneScale_ : 1.0);

  auto sizeStr   = QString("Scene Size: %1 %2 %3").arg(sceneSize_.getX()).
                     arg(sceneSize_.getY()).arg(sceneSize_.getZ());
  auto centerStr = QString("Scene Center: %1 %2 %3").arg(sceneCenter_.getX()).
                     arg(sceneCenter_.getY()).arg(sceneCenter_.getZ());
#if 0
  std::cerr << sizeStr.toStdString() << "\n";
  std::cerr << centerStr.toStdString() << "\n";

  //std::cerr << "Scene Scale : " << sceneScale_ << "\n";
#endif

  app_->statusBar()->setScaleLabel(sizeStr + ", " + centerStr);
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

  if      (app_->isShowBone()) {
    // draw bone nodes
    for (auto &po : objectDatas_) {
      auto *objectData = po.second;

      auto *object = objectData->object();
      if (! object->getVisible())
        continue;

      drawObjectBone(objectData);
    }
  }
  else if (isBonesEnabled()) {
    // draw bone skeleton
    drawObjectsBones();

    // overlay model objects if wireframe
    for (auto &po : objectDatas_) {
      auto *objectData = po.second;

      auto *object = objectData->object();
      if (! object->getVisible())
      continue;

      if (! objectData->isSolid() && objectData->isWireframe())
        drawObjectModel(objectData);
    }

  }
  else {
    // draw model objects
    for (auto &po : objectDatas_) {
      auto *objectData = po.second;

      auto *object = objectData->object();
      if (! object->getVisible())
        continue;

      drawObjectModel(objectData);

      if (isShowBoneVertices())
        drawObjectAnnotation(objectData);
    }
  }

  //---

  if (isShowNormals()) {
    for (auto &po : objectDatas_) {
      auto *objectData = po.second;

      auto *object = objectData->object();
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

  // draw current object wireframe
  if (wireframe_->isVisible())
    drawWireframe();

  //---

  // draw current object hull
  if (hull_->isVisible())
    drawHull();

  //---

  // draw current object basis
  if (basis_->isShow())
    drawBasis();
}

void
CQNewGLCanvas::
drawObjectModel(CQNewGLModelObject *objectData)
{
  auto *object     = objectData->object();
  auto *rootObject = object->getRootObject();

  // update bone nodes
  bool useBones = (isAnimEnabled() && animName() != "");

  if (useBones) {
    if (! rootObject->updateNodesAnimationData(animName().toStdString(), time()))
      useBones = false;
  }

  bool useBonePoints  = (useBones && objectData->buffer()->hasBonesPart());
  bool showBonePoints = (useBones && isShowBonePoints());

  //---

  auto *program = objectData->shaderProgram();

  auto isSelected = object->getSelected();

  objectData->buffer()->bind();

  program->bind();

  //---

#if 0
  auto bbox = objectData->buffer()->getBBox();
  std::cerr << "Buffer BBox: " << bbox << "\n";
#endif

  //---

  program->setUniformValue("viewPos", CQGLUtil::toVector(viewPos()));

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
    auto meshMatrix = getMeshGlobalTransform(objectData->object(), /*invert*/false);

    modelMatrix = meshMatrix*modelMatrix;
  }

  addShaderMVP(program, modelMatrix);

  //---

  addShaderLights(program);

  addShaderLightGlobals(program);

  //---

  // bones transform
  if (useBones) {
    updateNodeMatrices(objectData);

    program->setUniformValueArray("globalBoneTransform",
      &paintData_.nodeQMatrices[0], paintData_.numNodeMatrices);
  }

  //---

  if (! showBonePoints) {
    program->setUniformValue("useBones", useBones);
    program->setUniformValue("useBonePoints", useBonePoints);
  }
  else {
    program->setUniformValue("useBones", false);
    program->setUniformValue("useBonePoints", false);
  }

  //---

  program->setUniformValue("isSelected", isSelected);

  //---

  // render model
  for (const auto &faceData : objectData->faceDatas()) {
    int boneInd = rootObject->mapNodeId(faceData.boneId);

    program->setUniformValue("boneId", boneInd);

    //---

    // material
    program->setUniformValue("shininess", float(faceData.shininess));

    //---

    bool useDiffuseTexture = faceData.diffuseTexture;

    program->setUniformValue("diffuseTexture.enabled", useDiffuseTexture);

    if (useDiffuseTexture) {
      glActiveTexture(GL_TEXTURE0);
      faceData.diffuseTexture->bind();

      program->setUniformValue("diffuseTexture.texture", 0);
    }

    //---

    bool useSpecularTexture = faceData.specularTexture;

    program->setUniformValue("specularTexture.enabled", useSpecularTexture);

    if (useSpecularTexture) {
      glActiveTexture(GL_TEXTURE1);
      faceData.specularTexture->bind();

      program->setUniformValue("specularTexture.texture", 1);
    }

    //---

    bool useNormalTexture = faceData.normalTexture;

    program->setUniformValue("normalTexture.enabled", useNormalTexture);

    if (useNormalTexture) {
      glActiveTexture(GL_TEXTURE2);
      faceData.normalTexture->bind();

      program->setUniformValue("normalTexture.texture", 2);
    }

    //---

    bool useEmissiveTexture = faceData.emissiveTexture;

    program->setUniformValue("emissiveTexture.enabled", useEmissiveTexture);

    if (useEmissiveTexture) {
      glActiveTexture(GL_TEXTURE3);
      faceData.emissiveTexture->bind();

      program->setUniformValue("emissiveTexture.texture", 3);
    }

    program->setUniformValue("emissionColor", CQGLUtil::toVector(faceData.emission));

    //---

    program->setUniformValue("tangentSpaceNormal", isTangentSpaceNormal());

    //---

    if (objectData->isSolid()) {
      program->setUniformValue("isWireframe", 0);

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

      glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
    }

    if (objectData->isWireframe()) {
      program->setUniformValue("isWireframe", 1);

      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

      glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
    }
  }

  //---

  objectData->buffer()->unbind();

  program->release();
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
drawObjectNormals(CQNewGLModelObject *objectData)
{
  auto *object = objectData->object();

  //---

  if (! objectData->normalData().buffer)
    return;

  objectData->normalData().buffer->bind();

  //---

  objectData->bindNormalData();

  //---

  auto *normalShaderProgram = objectData->normalShaderProgram();

//auto modelMatrix = getModelMatrix();
  auto modelMatrix = object->getHierTransform();
  addShaderMVP(normalShaderProgram, modelMatrix);

  //---

  // draw lines
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glDrawArrays(GL_LINES, 0, objectData->normalData().buffer->numPoints());

  //---

  objectData->unbindNormalData();
}

void
CQNewGLCanvas::
getBoneData(CGeomObject3D *object, int nodeId, BoneData &boneData) const
{
//auto *rootObject = object->getRootObject();
//int boneInd = rootObject->mapNodeId(nodeId);
  int boneInd = nodeId;

  const auto &nodeData = object->getNode(boneInd);

  boneData.name = nodeData.name;
  boneData.ind  = nodeData.ind;

  getNodeVertices(object, nodeId, boneData);
}

void
CQNewGLCanvas::
getNodeVertices(CGeomObject3D *object, int nodeId, BoneData &boneData) const
{
  std::set<uint> vertexInds;

  const auto &faces = object->getFaces();

  for (const auto *face : faces) {
    const auto &vertices = face->getVertices();

    for (const auto &v : vertices) {
      const auto &vertex = object->getVertex(v);

      if (vertexInds.find(vertex.getInd()) != vertexInds.end())
        continue;

      vertexInds.insert(vertex.getInd());

      //---

      const auto &jointData = vertex.getJointData();

      bool match = false;

      for (int i = 0; i < 4; ++i) {
        if (jointData.nodeDatas[i].node == nodeId) {
          match = true;
          break;
        }
      }

      if (! match)
        continue;

      const auto &model = vertex.getModel();

      boneData.vertices.push_back(model);
    }
  }

  for (auto *child : object->children())
    getNodeVertices(child, nodeId, boneData);
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
getMeshGlobalTransform(CGeomObject3D *object, bool invert) const
{
  auto meshMatrix = object->getMeshGlobalTransform();

  return (invert ? meshMatrix.inverse() : meshMatrix);
}

#if 0
CMatrix3D
CQNewGLCanvas::
getMeshLocalTransform(CGeomObject3D *object, bool invert) const
{
  auto meshMatrix = object->getMeshLocalTransform();

  return (invert ? meshMatrix.inverse() : meshMatrix);
}
#endif

void
CQNewGLCanvas::
updateNodeMatrices(CQNewGLModelObject *objectData)
{
  auto *object     = objectData->object();
  auto *rootObject = object->getRootObject();

  std::vector<CMatrix3D> nodeMatrices;

  for (int i = 0; i < paintData_.numNodeMatrices; i++)
    nodeMatrices.push_back(CMatrix3D::identity());

  struct WorkData {
    const CGeomNodeData *parent     { nullptr };
    const CGeomNodeData *node       { nullptr };
//  CMatrix3D            animMatrix { CMatrix3D::identity() };
    CMatrix3D            transform  { CMatrix3D::identity() };
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

  axes_->setValid(true);
}

void
CQNewGLCanvas::
drawAxes()
{
  if (! axes_->isValid())
    updateAxes();

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
  if (! normals_->shaderProgram())
    updateNormals();

  normals_->drawGeometry();
}

//---

void
CQNewGLCanvas::
updateWireframe()
{
  if (wireframe_)
    wireframe_->updateGeometry();
}

void
CQNewGLCanvas::
drawWireframe()
{
  if (! wireframe_->shaderProgram())
    updateWireframe();

  if (wireframe_)
    wireframe_->drawGeometry();
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
drawBasis()
{
  basis_->drawGeometry();
}

//---

void
CQNewGLCanvas::
addAnnotationObject(CQNewGLObject *object)
{
  annotationObjects_.push_back(object);
}

void
CQNewGLCanvas::
updateAnnotationObjects()
{
  for (auto *object : annotationObjects_)
    object->updateGeometry();

  update();
}

std::vector<CQNewGLObject *>
CQNewGLCanvas::
getAnnotationObjects() const
{
  std::vector<CQNewGLObject *> objects;

  auto *objectData = getCurrentObjectData();

  if (objectData)
    objects.push_back(objectData);

  auto *shape = getCurrentShape();

  if (shape)
    objects.push_back(shape);

  auto *terrain = getTerrain();

  if (terrain)
    objects.push_back(terrain);

  auto *maze = getMaze();

  if (maze)
    objects.push_back(maze);

  return objects;
}

//---

std::vector<CGeomObject3D *>
CQNewGLCanvas::
getRootObjects() const
{
  std::set<CGeomObject3D *>    rootObjectSet;
  std::vector<CGeomObject3D *> rootObjects;

  const auto &objects = scene_->getObjects();

  for (auto *object : objects) {
    auto *rootObject = object->getRootObject();

    if (rootObjectSet.find(rootObject) == rootObjectSet.end()) {
      rootObjectSet.insert(rootObject);

      rootObjects.push_back(rootObject);
    }
  }

  return rootObjects;
}

//---

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
  object->getModelBBox(bbox, /*hier*/false);

#if 1
  auto modelMatrix = getMeshGlobalTransform(object, /*invert*/false);
//const auto &modelMatrix = object->getHierTransform();

  auto p1 = modelMatrix*bbox.getMin();
  auto p2 = modelMatrix*bbox.getMax();

  bbox = CBBox3D(p1, p2);
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
addCube(CQGLBuffer *buffer, const CBBox3D &bbox, const ShapeData &data,
        std::vector<CQNewGLFaceData> &faceDatas, int &pos) const
{
  auto center = bbox.getCenter();

  auto data1 = data;

  data1.size = CPoint3D(bbox.getXSize(), bbox.getYSize(), bbox.getZSize());

  addCube(buffer, center - CPoint3D(0.0, 0.5, 0.0), center + CPoint3D(0.0, 0.5, 0.0),
          data1, faceDatas, pos);
}

void
CQNewGLCanvas::
addCube(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, const ShapeData &data,
        std::vector<CQNewGLFaceData> &faceDatas, int &pos) const
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

  auto size   = p1.distanceTo(p2);
  auto center = (p1 + p2)/2.0;

  //---

  // transform to specified size and center
  auto m1 = CMatrix3D::translation(center.x, center.y, center.z);
  auto m2 = getShapeRotationMatrix(p1, p2);
  auto m3 = CMatrix3D::scale(size*data.size.x, size*data.size.y, size*data.size.z);

  auto modelMatrix  = m1*m2*m3;
  auto normalMatrix = modelMatrix.inverse().transposed();

  //---

  auto cubePoint = [&](int i, int j) {
    const auto &v1 = v[cube_faces[i][j]];

    return CPoint3D(v1[0], v1[1], v1[2]);
  };

  auto cubeNormal = [&](int i) {
    const auto &n = cube_normal[i];

    return CVector3D(n[0], n[1], n[2]);
  };

  auto addPoint = [&](const CPoint3D &p, const CVector3D &n, const CRGBA &c, const CPoint2D &tp) {
    auto p1 = modelMatrix*p;
    auto n1 = (normalMatrix*n).normalize();

    buffer->addPoint(p1.x, p1.y, p1.z);
    buffer->addNormal(n1.getX(), n1.getY(), n1.getZ());
    buffer->addColor(c);
    buffer->addTexturePoint(tp.x, tp.y);
  };

  auto tp1 = CPoint2D(0.0, 0.0);
  auto tp2 = CPoint2D(1.0, 0.0);
  auto tp3 = CPoint2D(1.0, 1.0);
  auto tp4 = CPoint2D(0.0, 1.0);

  for (GLint i = 5; i >= 0; i--) {
    CQNewGLFaceData faceData;

    faceData.pos = pos;
    faceData.len = 4;

    auto p1 = cubePoint(i, 0);
    auto p2 = cubePoint(i, 1);
    auto p3 = cubePoint(i, 2);
    auto p4 = cubePoint(i, 3);

    auto normal = cubeNormal(i);

    addPoint(p1, normal, data.color, tp1);
    addPoint(p2, normal, data.color, tp2);
    addPoint(p3, normal, data.color, tp3);
    addPoint(p4, normal, data.color, tp4);

    faceDatas.push_back(faceData);

    pos += faceData.len;
  }
}

void
CQNewGLCanvas::
addCone(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, double w,
        const ShapeData &data, std::vector<CQNewGLFaceData> &faceDatas, int &pos) const
{
  auto *object = new CGeomObject3D(nullptr, "");

  auto h = p1.distanceTo(p2);

  CGeomCone3D::addGeometry(object, CPoint3D(0.0, 0.0, 0.0), w, h, 20);
  CGeomCone3D::addNormals(object, w, h);

  //---

  auto m1 = CMatrix3D::translation(p1.x, p1.y, p1.z);
  auto m2 = getShapeRotationMatrix(p1, p2);
  auto m3 = CMatrix3D::scale(data.size.x, data.size.y, data.size.z);
  auto m4 = CMatrix3D::translation(0, h/2.0, 0.0);

  auto modelMatrix  = m1*m2*m3*m4;
  auto normalMatrix = modelMatrix.inverse().transposed();

  //---

  auto addPoint = [&](const CPoint3D &p, const CVector3D &n, const CRGBA &c, const CPoint2D &tp) {
    auto p1 = modelMatrix *p;
    auto n1 = (normalMatrix*n).normalize();

    buffer->addPoint(p1.x, p1.y, p1.z);
    buffer->addNormal(n1.getX(), n1.getY(), n1.getZ());
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

      addPoint(vertex->getModel(), vertex->getNormal(), data.color, vertex->getTextureMap());
    }

    faceDatas.push_back(faceData);

    pos += faceData.len;
  }

  delete object;
}

void
CQNewGLCanvas::
addCylinder(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, double r,
            const ShapeData &data, std::vector<CQNewGLFaceData> &faceDatas, int &pos) const
{
  auto *object = new CGeomObject3D(nullptr, "");

  auto h = p1.distanceTo(p2);

  CGeomCylinder3D::addGeometry(object, CPoint3D(0.0, 0.0, 0.0), 2*r, h, 20);
  CGeomCylinder3D::addNormals(object, 2*r, h);

  //---

  auto m1 = CMatrix3D::translation(p1.x, p1.y, p1.z);
  auto m2 = getShapeRotationMatrix(p1, p2);
  auto m3 = CMatrix3D::scale(data.size.x, data.size.y, data.size.z);
  auto m4 = CMatrix3D::translation(0.0, h/2.0, 0.0);

  auto modelMatrix  = m1*m2*m3*m4;
  auto normalMatrix = modelMatrix.inverse().transposed();

  //---

  auto addPoint = [&](const CPoint3D &p, const CVector3D &n, const CRGBA &c, const CPoint2D &tp) {
    auto p1 = modelMatrix *p;
    auto n1 = (normalMatrix*n).normalize();

    buffer->addPoint(p1.x, p1.y, p1.z);
    buffer->addNormal(n1.getX(), n1.getY(), n1.getZ());
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

      addPoint(vertex->getModel(), vertex->getNormal(), data.color, vertex->getTextureMap());
    }

    faceDatas.push_back(faceData);

    pos += faceData.len;
  }

  delete object;
}

void
CQNewGLCanvas::
addSphere(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, const ShapeData &data,
          double angleStart, double angleDelta,
          std::vector<CQNewGLFaceData> &faceDatas, int &pos) const
{
  auto *object = new CGeomObject3D(nullptr, "");

  auto r = p1.distanceTo(p2)/2.0;

  // sphere of radius r, centerered at (0, 0, 0)
  CGeomSphere3D::ConfigData configData;
  configData.angleStart = angleStart;
  configData.angleDelta = angleDelta;

  CGeomSphere3D::addGeometry(object, CPoint3D(0, 0, 0), r, configData);
  CGeomSphere3D::addNormals(object, r);
  CGeomSphere3D::addTexturePoints(object);

  //---

  // transfrom to specified size and center
  auto m1 = CMatrix3D::translation(p1.x, p1.y, p1.z);
  auto m2 = getShapeRotationMatrix(p1, p2);
  auto m3 = CMatrix3D::scale(data.size.x, data.size.y, data.size.z);

  auto modelMatrix  = m1*m2*m3;
  auto normalMatrix = modelMatrix.inverse().transposed();

  //---

  auto addPoint = [&](const CPoint3D &p, const CVector3D &n, const CRGBA &c, const CPoint2D &tp) {
    auto p1 = modelMatrix *p;
    auto n1 = (normalMatrix*n).normalize();

    buffer->addPoint(p1.x, p1.y, p1.z);
    buffer->addNormal(n1.getX(), n1.getY(), n1.getZ());
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

      addPoint(vertex->getModel(), vertex->getNormal(), data.color, tp);

      ++iv;
    }

    faceDatas.push_back(faceData);

    pos += faceData.len;
  }

  delete object;
}

void
CQNewGLCanvas::
addPyramid(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, double w,
           const ShapeData &data, std::vector<CQNewGLFaceData> &faceDatas, int &pos) const
{
  auto *object = new CGeomObject3D(nullptr, "");

  auto h = p1.distanceTo(p2);

  CGeomPyramid3D::addGeometry(object, CPoint3D(0.0, 0.0, 0.0), w, h);

  //---

  auto m1 = CMatrix3D::translation(p1.x, p1.y, p1.z);
  auto m2 = getShapeRotationMatrix(p1, p2);
  auto m3 = CMatrix3D::scale(data.size.x, data.size.y, data.size.z);
  auto m4 = CMatrix3D::translation(0.0, h/2.0, 0.0);

  auto modelMatrix  = m1*m2*m3*m4;
  auto normalMatrix = modelMatrix.inverse().transposed();

  //---

  auto addPoint = [&](const CPoint3D &p, const CVector3D &n, const CRGBA &c, const CPoint2D &tp) {
    auto p1 = modelMatrix*p;
    auto n1 = (normalMatrix*n).normalize();

    buffer->addPoint(p1.x, p1.y, p1.z);
    buffer->addNormal(n1.getX(), n1.getY(), n1.getZ());
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

      addPoint(vertex->getModel(), vertex->getNormal(), data.color, vertex->getTextureMap());
    }

    faceDatas.push_back(faceData);

    pos += faceData.len;
  }

  delete object;
}

void
CQNewGLCanvas::
addTorus(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, double rfactor,
         double power1, double power2, const ShapeData &data,
         std::vector<CQNewGLFaceData> &faceDatas, int &pos) const
{
  auto *object = new CGeomObject3D(nullptr, "");

  auto r0 = p1.distanceTo(p2);
  auto r1 = rfactor*r0;

  int nu = 30;
  int nv = 30;
  CGeomTorus3D::addGeometry(object, CPoint3D(0, 0, 0), r0, r1, power1, power2, nu, nv);

  auto m1 = CMatrix3D::translation(p1.x, p1.y, p1.z);
  auto m2 = getShapeRotationMatrix(p1, p2);
  auto m3 = CMatrix3D::scale(data.size.x, data.size.y, data.size.z);

  auto modelMatrix  = m1*m2*m3;
  auto normalMatrix = modelMatrix.inverse().transposed();

  auto addPoint = [&](const CPoint3D &p, const CVector3D &n, const CRGBA &c, const CPoint2D &tp) {
    auto p1 = modelMatrix *p;
    auto n1 = (normalMatrix*n).normalize();

    buffer->addPoint(p1.x, p1.y, p1.z);
    buffer->addNormal(n1.getX(), n1.getY(), n1.getZ());
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

      addPoint(vertex->getModel(), vertex->getNormal(), data.color, tp);

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
