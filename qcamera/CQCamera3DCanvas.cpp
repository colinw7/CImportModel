#include <CQCamera3DCanvas.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DShaderProgram.h>
#include <CQCamera3DFaceData.h>
#include <CQCamera3DCamera.h>
#include <CQCamera3DLight.h>
#include <CQCamera3DShape.h>
#include <CQCamera3DAnnotation.h>
#include <CQCamera3DBillboard.h>
#include <CQCamera3DNormals.h>
#include <CQCamera3DBasis.h>
#include <CQCamera3DBBox.h>
#include <CQCamera3DOverlay.h>
#include <CQCamera3DOverlay2D.h>
#include <CQCamera3DControl.h>
#include <CQCamera3DStatus.h>
#include <CQCamera3DAxes.h>
#include <CQCamera3DFont.h>
#include <CQCamera3DTexture.h>
#include <CQCamera3DGeomObject.h>
#include <CQCamera3DGeomFace.h>
#include <CQCamera3DCanvasMouseModeIFace.h>
#include <CQCamera3DMouseMode.h>
#include <CQCamera3DOpWidget.h>
#include <CQCamera3DUtil.h>

#include <CQGLBuffer.h>
#include <CQGLTexture.h>
#include <CQGLUtil.h>
#ifdef CQ_PERF_GRAPH
#include <CQPerfMonitor.h>
#else
struct CQPerfTrace {
  CQPerfTrace(const char *) { }
};
#endif

#include <CGeomScene3D.h>
#include <CGeomObject3D.h>
#include <CGeomTexture.h>
#include <CGeomNodeData.h>
#include <CGeometry3D.h>

#include <CStrUtil.h>

#include <QMouseEvent>
#include <QWheelEvent>

CQCamera3DCanvas::
CQCamera3DCanvas(CQCamera3DApp *app) :
 CQCamera3DWidget(app)
{
  setObjectName("canvas");

  //---

  auto addCamera = [&](const QString &name, bool perspective=true) {
    auto *camera = new CQCamera3DCamera;

    camera->setId(cameras_.size() + 1);

    camera->setName(name.toStdString());

    camera->setPerspective(perspective);

    connect(camera, SIGNAL(stateChanged()), this, SLOT(cameraChanged()));

    cameras_.push_back(camera);

    return camera;
  };

  perspectiveCamera_ = addCamera("perspective");

  debugCamera_ = addCamera("debug");

  setCurrentCamera(cameras_[0]);

  topCamera_    = addCamera("top"   , /*perspective*/ false);
  bottomCamera_ = addCamera("bottom", /*perspective*/ false);
  frontCamera_  = addCamera("front" , /*perspective*/ false);
  backCamera_   = addCamera("back"  , /*perspective*/ false);
  leftCamera_   = addCamera("left"  , /*perspective*/ false);
  rightCamera_  = addCamera("right" , /*perspective*/ false);

  //---

  addLight();

  //---

  shape_ = new CQCamera3DShape(this);

  annotation_ = new CQCamera3DAnnotation(this);

  billboard_ = new CQCamera3DBillboard(this);

  normals_ = new CQCamera3DNormals(this);

  basis_ = new CQCamera3DBasis(this);

  bboxOverlay_ = new CQCamera3DBBox(this);

  overlay_   = new CQCamera3DOverlay  (this);
//overlay2D_ = new CQCamera3DOverlay2D(this);

  //---

  mouseModeIFace_ = new CQCamera3DCanvasMouseModeIFace(this);

  mouseModeMgr_ = new CQCamera3DMouseModeMgr(mouseModeIFace_);

  opWidget_ = new CQCamera3DOpWidget(this, mouseModeMgr_);

  hideOptions();

  //---

  connect(this, SIGNAL(stateChanged()), this, SLOT(updateStatus()));

  connect(app_, SIGNAL(animNameChanged()), this, SLOT(updateObjectsData()));
  connect(app_, SIGNAL(animTimeChanged()), this, SLOT(update()));
}

void
CQCamera3DCanvas::
updateQuadCameras()
{
  CQPerfTrace trace("CQCamera3DCanvas::updateQuadCameras");

  auto s = std::sqrt(2.0);

  auto center  = CVector3D(bbox_.getCenter());
  auto maxSize = s*bbox_.getMaxSize();

  topCamera_->setPitch(-M_PI/2.0);
  topCamera_->setYaw(M_PI/2.0);
  topCamera_->setRoll(0.0);

  topCamera_->setPosition(CVector3D(center.x(), center.y() + maxSize, center.z()));
  topCamera_->setOrigin  (center);

  bottomCamera_->setPitch(M_PI/2.0);
  bottomCamera_->setYaw(M_PI/2.0);
  bottomCamera_->setRoll(0.0);

  bottomCamera_->setPosition(CVector3D(center.x(), center.y() - maxSize, center.z()));
  bottomCamera_->setOrigin  (center);

  //---

  frontCamera_->setPitch(0.0);
  frontCamera_->setYaw(0.0);
  frontCamera_->setRoll(0.0);

  frontCamera_->setPosition(CVector3D(center.x() - maxSize, center.y(), center.z()));
  frontCamera_->setOrigin(center);

  backCamera_->setPitch(M_PI);
  backCamera_->setYaw(0.0);
  backCamera_->setRoll(0.0);

  backCamera_->setPosition(CVector3D(center.x() + maxSize, center.y(), center.z()));
  backCamera_->setOrigin(center);

  //---

  leftCamera_->setPitch(0.0);
  leftCamera_->setYaw(M_PI/2.0);
  leftCamera_->setRoll(0.0);

  leftCamera_->setPosition(CVector3D(center.x(), center.y(), center.z() + maxSize));
  leftCamera_->setOrigin  (center);

  rightCamera_->setPitch(0.0);
  rightCamera_->setYaw(-M_PI/2.0);
  rightCamera_->setRoll(0.0);

  rightCamera_->setPosition(CVector3D(center.x(), center.y(), center.z() - maxSize));
  rightCamera_->setOrigin  (center);
}

void
CQCamera3DCanvas::
addLight()
{
  CQPerfTrace trace("CQCamera3DCanvas::addLight");

  auto id = uint(lights_.size() + 1);

  auto name = QString("light.%1").arg(id);

  auto *scene = app_->getScene();

  auto *light = CGeometry3DInst->createLight3D(scene, name.toStdString());

  auto *light1 = dynamic_cast<CQCamera3DLight *>(light);

  light1->setId(id);

  resetLight(light1);

  lights_.push_back(light1);

  Q_EMIT lightAdded();
}

void
CQCamera3DCanvas::
resetLight(CQCamera3DLight *light)
{
  CQPerfTrace trace("CQCamera3DCanvas::resetLight");

  light->setDirection(CVector3D(0, 1, 0));

  if (bbox_.isSet()) {
    auto center  = bbox_.getCenter();
    auto maxSize = bbox_.getMaxSize();

    light->setPosition(CPoint3D(center.x + maxSize/2, center.y + maxSize/2.0, center.z + maxSize));

    light->setPointRadius(2*maxSize);

    light->setSpotDirection(CVector3D(center.x, center.y, center.z + maxSize));
  }
  else {
    light->setPosition(CPoint3D(0, 1, 0));

    light->setPointRadius(10);

    light->setSpotDirection(CVector3D(0, 0, 1));
  }

  light->setSpotCutOffAngle(90);
}

void
CQCamera3DCanvas::
initializeGL()
{
  CQPerfTrace trace("CQCamera3DCanvas::initializeGL");

  initializeOpenGLFunctions();

  //---

  font_ = new CQCamera3DFont(this);

  font_->init();

  font_->setSize(48);
  font_->setFontName("OpenSans-Regular.ttf");

  //---

  if (initTextureMap_ != "")
    loadTextureMap(initTextureMap_);

  if (initMaterialMap_ != "")
    loadMaterialMap(initMaterialMap_);

  //---

  axes_ = new CQCamera3DAxes(this);

  //---

  addObjectsData();

  //---

  updateShapes();

  updateAnnotation();

  updateBillboard();

  //---

  updateOverlay2D();

  //---

  updateOverlay();

  //---

  initCamera();

  //---

  for (auto *light : lights_)
    resetLight(light);

  //---

  Q_EMIT stateChanged();
}

bool
CQCamera3DCanvas::
loadTextureMap(const std::string &fileName)
{
  struct TextureMapData {
    std::string diffuse;
    std::string normal;
    std::string specular;
    std::string emissive;
  };

  using TextureMap = std::map<std::string, TextureMapData>;

  TextureMap textureMap;

  CFile file(fileName);
  if (! file.exists()) return false;

  while (! file.eof()) {
    std::string line;

    file.readLine(line);

    //---

    // skip comment
    int pos = 0;

    while (line[pos] != '\0' && isspace(line[pos]))
      ++pos;

    if (line[pos] == '#')
      continue;

    //---

    std::vector<std::string> words;

    CStrUtil::toWords(line, words);
    if (words.size() < 2) continue;

    TextureMapData textureData;

    textureData.diffuse = words[1];

    if (words.size() > 2)
      textureData.normal = words[2];

    if (words.size() > 3)
      textureData.specular = words[3];

    if (words.size() > 4)
      textureData.emissive = words[4];

    textureMap[words[0]] = textureData;
  }

  for (auto &pt : textureMap) {
    const auto &data = pt.second;

    if (data.diffuse != "" && data.diffuse != "none")
      addTextureFile(data.diffuse);
    if (data.normal != "" && data.normal != "none")
      addTextureFile(data.normal);
    if (data.specular != "" && data.specular != "none")
      addTextureFile(data.specular);
    if (data.emissive != "" && data.emissive != "none")
      addTextureFile(data.emissive);
  }

  auto *scene = app_->getScene();

  for (auto *object : scene->getObjects()) {
    auto name = object->getName();

    auto pt = textureMap.find(name);
    if (pt == textureMap.end())
      continue;

    const auto &data = (*pt).second;

    auto *texture = app_->getTextureByName(data.diffuse);

    if (texture)
      object->setDiffuseTexture(texture);

    if (data.normal != "") {
      auto *texture = app_->getTextureByName(data.normal);

      if (texture)
        object->setNormalTexture(texture);
    }

    if (data.specular != "") {
      auto *texture = app_->getTextureByName(data.specular);

      if (texture)
        object->setSpecularTexture(texture);
    }

    if (data.emissive != "") {
      auto *texture = app_->getTextureByName(data.emissive);

      if (texture)
        object->setEmissiveTexture(texture);
    }
  }

  objectsChanged();

  return true;
}

bool
CQCamera3DCanvas::
loadMaterialMap(const std::string &fileName)
{
  struct MaterialMapData {
    std::string diffuse;
    std::string normal;
    std::string specular;
    std::string emissive;
  };

  using MaterialMap = std::map<std::string, MaterialMapData>;

  MaterialMap materialMap;

  CFile file(fileName);
  if (! file.exists()) return false;

  while (! file.eof()) {
    std::string line;

    file.readLine(line);

    //---

    // skip comment
    int pos = 0;

    while (line[pos] != '\0' && isspace(line[pos]))
      ++pos;

    if (line[pos] == '#')
      continue;

    //---

    std::vector<std::string> words;

    CStrUtil::toWords(line, words);
    if (words.size() < 2) continue;

    MaterialMapData maerialData;

    maerialData.diffuse = words[1];

    if (words.size() > 2)
      maerialData.normal = words[2];

    if (words.size() > 3)
      maerialData.specular = words[3];

    if (words.size() > 4)
      maerialData.emissive = words[4];

    materialMap[words[0]] = maerialData;
  }

  for (auto &pt : materialMap) {
    const auto &data = pt.second;

    if (data.diffuse != "" && data.diffuse != "none")
      addTextureFile(data.diffuse);
    if (data.normal != "" && data.normal != "none")
      addTextureFile(data.normal);
    if (data.specular != "" && data.specular != "none")
      addTextureFile(data.specular);
    if (data.emissive != "" && data.emissive != "none")
      addTextureFile(data.emissive);
  }

  auto *scene = app_->getScene();

  for (auto *material : scene->getMaterials()) {
    auto name = material->name();

    auto pm = materialMap.find(name);
    if (pm == materialMap.end())
      continue;

    const auto &data = (*pm).second;

    auto *texture = app_->getTextureByName(data.diffuse);

    if (texture)
      material->setDiffuseTexture(texture);

    if (data.normal != "") {
      auto *texture = app_->getTextureByName(data.normal);

      if (texture)
        material->setNormalTexture(texture);
    }

    if (data.specular != "") {
      auto *texture = app_->getTextureByName(data.specular);

      if (texture)
        material->setSpecularTexture(texture);
    }

    if (data.emissive != "") {
      auto *texture = app_->getTextureByName(data.emissive);

      if (texture)
        material->setEmissiveTexture(texture);
    }
  }

  objectsChanged();

  return true;
}

void
CQCamera3DCanvas::
addTextureFile(const std::string &fileName)
{
  if (fileName == "")
    return;

  auto fileName1 = fileName;
  bool flipped   = false;

  auto len = fileName1.size();

  if (len > 5 && fileName1.substr(len - 5) == ".flip") {
    fileName1 = fileName1.substr(0, len - 5);
    flipped   = true;
  }

  auto name1 = fileName1;
//auto name2 = fileName1 + ".flip";

  auto *texture1 = app_->getTextureByName(name1);
//auto *texture2 = app_->getTextureByName(name2);

  if (! texture1) {
    auto *scene = app_->getScene();

    CImageFileSrc src(fileName1);

    auto image = CImageMgrInst->createImage(src);
    if (! image->isValid()) return;

    auto *gtexture1 = CGeometry3DInst->createTexture(image);

    auto *gtexture2 = dynamic_cast<CQCamera3DTexture *>(gtexture1);
    assert(gtexture2);

    gtexture1->setName(name1);

    (void) initGLTexture(gtexture2);

    scene->addTexture(gtexture1);

    gtexture2->setFlipped(flipped);

    Q_EMIT textureAdded();
  }
}

void
CQCamera3DCanvas::
resizeGL(int width, int height)
{
  CQPerfTrace trace("CQCamera3DCanvas::resizeGL");

  setPixelWidth (width);
  setPixelHeight(height);

  glViewport(0, 0, width, height);

  setAspect(double(width)/double(height));

  //---

  // update camera
  for (auto *camera : cameras_)
    camera->setAspect(aspect());
}

void
CQCamera3DCanvas::
paintGL()
{
  CQPerfTrace trace("CQCamera3DCanvas::paintGL");

  // clear canvas
  glClearColor(bgColor_.redF(), bgColor_.greenF(), bgColor_.blueF(), 1.0f);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //---

  int w = pixelWidth();
  int h = pixelHeight();

  if (! isQuadView()) {
    auto *camera = getCurrentCamera();

    glViewport(0, 0, w, h);

    drawContents(camera);
  }
  else {
    int xm = w/2;
    int ym = h/2;

    // top
    glViewport(0, 0, xm, ym);
    drawContents(topCamera_);

    // 3d
    glViewport(xm, 0, w - xm, ym);
    drawContents(perspectiveCamera_);

    // front
    glViewport(0, ym, xm, h - ym);
    drawContents(frontCamera_);

    // right
    glViewport(xm, ym, w - xm, h - ym);
    drawContents(rightCamera_);
  }
}

void
CQCamera3DCanvas::
drawContents(CGLCameraIFace *camera)
{
  CQPerfTrace trace("CQCamera3DCanvas::drawContents");

  cameraData_.camera = camera;

  // camera projection
  cameraData_.worldMatrix = cameraData_.camera->worldMatrix();

  // camera/view transformation
  cameraData_.viewMatrix = cameraData_.camera->viewMatrix();

  // view pos
  cameraData_.viewPos = cameraData_.camera->position();

  //---

  // set GL state
  enableDepthTest  ();
  enableCullFace   ();
  enableFrontFace  ();
  enablePolygonLine();

  //---

  // draw axes
  if (isShowAxes()) {
    axes_->updateGeometry();

    axes_->drawGeometry();
  }

  //---

  // draw model objects
  drawObjectsData();

  //---

  // draw shapes
  shape_->drawGeometry();

  //---

  // draw normals
  if (normals_->isPointNormals() || normals_->isFaceNormals()) {
    normals_->updateGeometry();

    normals_->drawGeometry();
  }

  // draw basis
  if (isShowBasis()) {
    basis_->updateGeometry();

    basis_->drawGeometry();
  }

  // draw bbox
  if (isShowBBox()) {
    bboxOverlay_->updateGeometry();

    bboxOverlay_->drawGeometry();
  }

  annotation_->drawGeometry();

  billboard_->drawGeometry();

  //---

//overlay2D_->drawGeometry();

  if (! isQuadView() && viewType_ == ViewType::PERSPECTIVE) {
    overlay_->drawGeometry();
  }
}

void
CQCamera3DCanvas::
setCurrentCamera(CGLCameraIFace *camera)
{
  setCurrentCameraInd(camera->id());
}

void
CQCamera3DCanvas::
setCurrentCameraInd(uint ind)
{
  cameraInd_ = ind;

  bool found = false;

  for (auto *camera : cameras_) {
    bool found1 = (camera->id() == ind);

    camera->setVisible(found1);

    if (found1)
      found = found1;
  }

  if (! found)
    cameraInd_ = 1;
}

bool
CQCamera3DCanvas::
isShowAxes() const
{
  return (axes_ && axes_->isVisible());
}

CQCamera3DShaderProgram *
CQCamera3DCanvas::
shaderProgram()
{
  if (! shaderProgram_) {
    shaderProgram_ = new CQCamera3DShaderProgram(app_);

    shaderProgram_->addShaders("model.vs", "model.fs");
  }

  return shaderProgram_;
}

void
CQCamera3DCanvas::
updateObjectsData()
{
  addObjectsData();

  update();
}

void
CQCamera3DCanvas::
addObjectsData()
{
  CQPerfTrace trace("CQCamera3DCanvas::addObjectsData");

  bbox_ = CBBox3D();

  //---

  int    boneNodeIds[4];
  double boneWeights[4];

  int io = 0;

  auto *scene = app_->getScene();

  for (auto *object : scene->getObjects()) {
    auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);
    assert(object1);

    //(void) initObjectData(io, object);

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
      auto *face1 = dynamic_cast<CQCamera3DGeomFace *>(face);

      face1->clearPoints();

      //---

      CQCamera3DFaceData faceData;

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

        face1->addPoint(model2);

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
          normal1 = (tnormal*2.0 - CVector3D(1.0, 1.0, 1.0)).normalized();
        }

        //---

        buffer->addInd(vertex.getInd());

        buffer->addPoint(float(model.x), float(model.y), float(model.z));

        buffer->addNormal(float(normal1.getX()), float(normal1.getY()), float(normal1.getZ()));

        buffer->addColor(color1);

        //---

        const auto &jointData = vertex.getJointData();

        if (jointData.nodeDatas[0].node >= 0) {
          for (int i = 0; i < 4; ++i) {
            boneNodeIds[i] = jointData.nodeDatas[i].node;
            boneWeights[i] = jointData.nodeDatas[i].weight;
          }

          buffer->addBoneIds    (boneNodeIds[0], boneNodeIds[1], boneNodeIds[2], boneNodeIds[3]);
          buffer->addBoneWeights(boneWeights[0], boneWeights[1], boneWeights[2], boneWeights[3]);
        }

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

    const auto &lines = object->getLines();

    for (const auto *line : lines) {
      CQCamera3DFaceData faceData;

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

    //---

    object1->setBBox(bbox1);

    bbox_ += bbox1;

    //---

    buffer->load();

    ++io;
  }

  if (! bbox_.isSet()) {
    bbox_.add(CPoint3D(-1, -1, -1));
    bbox_.add(CPoint3D( 1,  1,  1));
  }
}

//---

void
CQCamera3DCanvas::
initCamera()
{
  CQPerfTrace trace("CQCamera3DCanvas::initCamera");

  auto center  = bbox_.getCenter();
  auto maxSize = bbox_.getMaxSize();

  auto s2 = std::sqrt(2.0);

  for (auto *camera : cameras_) {
    auto maxSize1 = s2*maxSize + camera->near();

    auto origin = CVector3D(center.x, center.y, center.z);
    auto pos    = CVector3D(center.x, center.y, center.z + maxSize1);

    auto v = CVector3D(origin, pos);
    auto a = std::atan2(v.z(), v.x());

    //camera->setOrigin(origin);
    camera->setPosition(pos);

    camera->setYaw(a);
  }
}

void
CQCamera3DCanvas::
resetCamera(CGLCameraIFace *camera)
{
  auto bbox = this->bbox();

  auto center  = bbox.getCenter();
  auto maxSize = bbox.getMaxSize();

  auto s2 = std::sqrt(2.0);

  auto maxSize1 = s2*maxSize + camera->near();

  auto origin = CVector3D(center.x, center.y, center.z);
  auto pos    = CVector3D(center.x, center.y + maxSize1/2.0, center.z + maxSize1);

  if (camera == debugCamera_) {
    origin = CVector3D(center.x, center.y, center.z);
    pos    = CVector3D(center.x, center.y, center.z + maxSize1/4.0);
  }

  camera->setPosition(pos);

  camera->setPitch(-M_PI/6.0);
  camera->setYaw(0.0);
  camera->setRoll(0.0);

  camera->setOrigin(origin);

  camera->moveAroundX(0.1);
  camera->moveAroundY(0.1);
}

//---

void
CQCamera3DCanvas::
updateStatus()
{
  CQPerfTrace trace("CQCamera3DCanvas::updateStatus");

  auto center = bbox_.getCenter();
  auto size   = bbox_.getSize();

  auto pointStr = [](const CPoint3D &p) {
    return QString("(%1 %2 %3)").arg(p.x).arg(p.y).arg(p.z);
  };

  auto vectorStr = [](const CVector3D &p) {
    return QString("(%1 %2 %3)").arg(p.x()).arg(p.y()).arg(p.z());
  };

  auto bboxStr = QString("Center: %1, Size: %2").arg(pointStr(center)).arg(vectorStr(size));

  auto *object = currentObject();
  auto *face   = currentFace();

  if      (object) {
    auto name     = QString::fromStdString(object->getName());
    auto meshName = QString::fromStdString(object->getMeshName());

    bboxStr += QString(", Object: %1 (Mesh: %2, #%3)").
      arg(name).arg(meshName).arg(object->getInd());
  }
  else if (face) {
    bboxStr += QString(", Face: %1").arg(face->getInd());
  }

  app_->status()->setModelLabel(bboxStr);
}

void
CQCamera3DCanvas::
drawObjectsData()
{
  CQPerfTrace trace("CQCamera3DCanvas::drawObjectsData");

  auto *program = this->shaderProgram();

  program->bind();

  //---

  // camera projection
  program->setUniformValue("projection", CQGLUtil::toQMatrix(cameraData_.worldMatrix));

  // camera/view transformation
  program->setUniformValue("view", CQGLUtil::toQMatrix(cameraData_.viewMatrix));

  // view pos
  program->setUniformValue("viewPos", CQGLUtil::toVector(cameraData_.viewPos));

  //---

  // add light data to shader program
  addShaderLights(program);

  //---

  glPointSize(pointSize());
  glLineWidth(lineWidth());

  paintData_.reset();

  //---

  bool isAnim = (app_->animName() != "");

  //---

  using VertexIndices           = std::vector<int>;
  using FaceEdgeIndices         = std::map<int, VertexIndices>;
  using SelectedObjectVertices  = std::map<int, VertexIndices>;
  using SelectedObjectFaceEdges = std::map<int, FaceEdgeIndices>;

  SelectedObjectVertices  selectedObjectVertices;
  SelectedObjectFaceEdges selectedObjectFaceEdges;

  //---

  auto selectColor    = (editMode() == EditMode::OBJECT ? this->selectColor() : this->focusColor());
  auto wireframeColor = this->wireframeColor();

  auto wireframeTransparency = 0.5f;

  program->setUniformValue("selectColor", CQGLUtil::toVector(ColorToVector(selectColor)));
  program->setUniformValue("wireframeColor", CQGLUtil::toVector(ColorToVector(wireframeColor)));

  program->setUniformValue("wireframeTransparency", wireframeTransparency);

  //---

  Objects objects;

  if (isLocalMode()) {
    objects = getSelectedObjects();
  }
  else {
    auto *scene = app_->getScene();

    objects = scene->getObjects();
  }

  for (auto *object : objects) {
    if (! object->getVisible())
      continue;

    auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);
    assert(object1);

    auto &selectedVertices  = selectedObjectVertices [object->getInd()];
    auto &selectedFaceEdges = selectedObjectFaceEdges[object->getInd()];

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

    // anim
    program->setUniformValue("useBonePoints", isAnim); // per object ?

    if (isAnim) {
      updateNodeMatrices(object);

      program->setUniformValueArray("globalBoneTransform",
        &paintData_.nodeQMatrices[0], PaintData::NUM_NODE_MATRICES);
    }

    //---

    bool objectSelected = object->getHierSelected();

    object1->buffer()->bind();

    //---

    auto *objectMaterial = object->getMaterialP();

    //---

    auto drawFace = [&](const CQCamera3DFaceData &faceData, double transparency) {
      bool faceSelected = faceData.face->getSelected();

      bool selected = objectSelected || faceSelected;

      program->setUniformValue("isSelected", selected);

      //---

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

      if (isWireframe() || selected) {
        program->setUniformValue("isWireframe", true);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
      }

      if (isSolid() || selected) {
        program->setUniformValue("isWireframe", false);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
      }

      if (isPoints()) {
        program->setUniformValue("isWireframe", true);

        glDrawArrays(GL_POINTS, faceData.pos, faceData.len);
      }
    };

    //---

    bool anyTransparent = false;

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

        if (faceMaterial && faceMaterial->transparency() > 0.0) {
          anyTransparent = true;
          continue;
        }

        drawFace(faceData, 0.0);

        int iv = faceData.pos;

        for (const auto &v : faceData.vertices) {
          const auto &vertex = object->getVertex(v);

          if (vertex.isSelected())
            selectedVertices.push_back(iv);

          ++iv;
        }

        auto *face1 = dynamic_cast<const CQCamera3DGeomFace *>(face);

        auto &selectedEdges1 = selectedFaceEdges[face1->getInd()];

        const auto &selectedEdges = face1->selectedEdges();

        for (const auto &selectedEdge : selectedEdges)
          selectedEdges1.push_back(faceData.pos + selectedEdge);
      }
      else if (faceData.line) {
        auto *line = faceData.line;

        if (! line->getVisible())
          continue;

        program->setUniformValue("isWireframe", true);

        glDrawArrays(GL_LINES, faceData.pos, faceData.len);
      }
      else
        assert(false);
    }

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

    //---

    if (! selectedVertices.empty()) {
      program->setUniformValue("isWireframe", true);

      for (const auto &iv : selectedVertices) {
        glDrawArrays(GL_POINTS, iv, 1);
      }
    }

    //---

    if (! selectedFaceEdges.empty()) {
      program->setUniformValue("isWireframe", true);

      for (const auto &pf : selectedFaceEdges) {
        const auto &vertices = pf.second;

        auto nv = vertices.size();

        for (uint iv = 0; iv < nv; ++iv) {
          glDrawArrays(GL_LINES, vertices[iv], 2);
        }
      }
    }

    //---

    object1->buffer()->unbind();
  }

  //---

  program->release();
}

void
CQCamera3DCanvas::
addShaderLights(CQCamera3DShaderProgram *program)
{
  CQPerfTrace trace("CQCamera3DCanvas::addShaderLights");

  program->setUniformValue("ambientColor", CQGLUtil::toVector(ambientColor()));
  program->setUniformValue("ambientStrength", float(ambientStrength()));

  program->setUniformValue("diffuseStrength", float(diffuseStrength()));

  program->setUniformValue("specularColor"   , CQGLUtil::toVector(specularColor()));
  program->setUniformValue("specularStrength", float(specularStrength()));

  program->setUniformValue("emissionColor"   , CQGLUtil::toVector(emissiveColor()));
  program->setUniformValue("emissiveStrength", float(emissiveStrength()));

  program->setUniformValue("fixedDiffuse", isFixedDiffuse());

  //---

  // max four active lights
  enum { MAX_LIGHTS = 4 };

  int il = 0;

  for (auto *light : lights()) {
    auto lightName = QString("lights[%1]").arg(il);

    program->setUniformValue(toCString(lightName + ".type"), int(light->getType()));
    program->setUniformValue(toCString(lightName + ".enabled"), light->getEnabled());

    program->setUniformValue(toCString(lightName + ".position"),
      CQGLUtil::toVector(light->getPosition()));

    program->setUniformValue(toCString(lightName + ".color"),
      CQGLUtil::toVector(ColorToVector(light->getDiffuse())));

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
}

void
CQCamera3DCanvas::
updateNodeMatrices(CGeomObject3D *object)
{
  CQPerfTrace trace("CQCamera3DCanvas::updateNodeMatrices");

  // anim data always on root object
  auto *rootObject = object->getRootObject();

  if (rootObject == paintData_.rootObject)
    return;

  paintData_.rootObject = rootObject;

  //---

  const auto &nodeMatrices = app_->getObjectNodeMatrices(object);

  //---

  std::vector<CMatrix3D> nodeMatrixArray;

  for (int i = 0; i < PaintData::NUM_NODE_MATRICES; i++)
    nodeMatrixArray.push_back(CMatrix3D::identity());

  for (const auto &pn : nodeMatrices) {
    auto nodeId = pn.first;

    if (nodeId >= PaintData::NUM_NODE_MATRICES) {
      std::cerr << "Too few node matrices for node " << nodeId << "\n";
      continue;
    }

    nodeMatrixArray[nodeId] = pn.second;
  }

  paintData_.nodeMatrices .resize(PaintData::NUM_NODE_MATRICES);
  paintData_.nodeQMatrices.resize(PaintData::NUM_NODE_MATRICES);

  int im = 0;
  for (const auto &m : nodeMatrixArray) {
    paintData_.nodeMatrices [im] = m;
    paintData_.nodeQMatrices[im] = CQGLUtil::toQMatrix(m);

    ++im;
  }
}

CQCamera3DGeomObject *
CQCamera3DCanvas::
defaultRootObject() const
{
  CQCamera3DGeomObject *rootGeomObject { nullptr };

  // get root object of first visible object
  auto *scene = app_->getScene();

  for (auto *object : scene->getObjects()) {
    if (! object->getVisible())
      continue;

    auto *rootObject = object->getRootObject();

    rootGeomObject = dynamic_cast<CQCamera3DGeomObject *>(rootObject);

    break;
  }

  return rootGeomObject;
}

CGeomObject3D *
CQCamera3DCanvas::
getGeomObject(CGeomObject3D *object) const
{
  if (! object->getFaces().empty())
    return object;

  for (auto *child : object->children()) {
    auto *object1 = getGeomObject(child);
    if (object1) return object1;
  }

  return nullptr;
}

//---

void
CQCamera3DCanvas::
setEditType(const EditType &v)
{
  editType_ = v;

  auto *object = currentObject();

  if      (editType_ == EditType::CURSOR) {
    auto *mode = new CQCamera3DCursorMouseMode(object);

    mouseModeMgr_->startMode(mode);
  }
  else if (editType_ == EditType::MOVE) {
    auto *mode = new CQCamera3DMoveMouseMode;

    mouseModeMgr_->startMode(mode);
  }
  else if (editType_ == EditType::SCALE) {
    auto *mode = new CQCamera3DScaleMouseMode(object);

    mouseModeMgr_->startMode(mode);
  }
  else if (editType_ == EditType::ROTATE) {
    auto *mode = new CQCamera3DRotateMouseMode(object);

    mouseModeMgr_->startMode(mode);
  }

  Q_EMIT editTypeChanged();
}

//---

void
CQCamera3DCanvas::
setLocalMode(bool b)
{
  localMode_ = b;

  updateObjectsData();
}

//---

CQCamera3DCanvas::SelectType
CQCamera3DCanvas::
calcSelectType() const
{
  if (editMode() == EditMode::OBJECT)
    return SelectType::OBJECT;
  else
    return selectType_;
}

void
CQCamera3DCanvas::
setSelectType(const SelectType &v)
{
  selectType_ = v;

  Q_EMIT selectTypeChanged();
}

void
CQCamera3DCanvas::
selectAllObjects()
{
  auto *scene = app_->getScene();

  CGeomObject3D *object = nullptr;

  for (auto *object1 : scene->getObjects()) {
    if (! object)
      object = object1;

    object1->setSelected(true);
  }

  updateCurrentObject();

  this->update();
}

bool
CQCamera3DCanvas::
selectObjects(const Objects &objects, bool clear, bool update)
{
  bool changed = false;

  if (clear)
    changed = deselectAll(update);

  for (auto *object : objects) {
    if (selectObject(object, /*clear*/false, /*update*/false))
      changed = true;
  }

  if (changed && update) {
    this->update();

    Q_EMIT stateChanged();
  }

  return changed;
}

bool
CQCamera3DCanvas::
selectObject(CGeomObject3D *object, bool clear, bool update)
{
  bool changed = false;

  if (clear)
    changed = deselectAll(update);

  if (! object->getSelected()) {
    object->setSelected(true);
    changed = true;
  }

  if (changed && update) {
    updateCurrentObject();

    this->update();

    Q_EMIT stateChanged();
  }

  return changed;
}

bool
CQCamera3DCanvas::
selectFace(CGeomFace3D *face, bool clear, bool update)
{
  bool changed = false;

  if (clear)
    changed = deselectAll(update);

  if (! face->getSelected()) {
    face->setSelected(true);
    changed = true;
  }

  if (changed && update) {
    updateCurrentObject();

    this->update();

    Q_EMIT stateChanged();
  }

  return changed;
}

bool
CQCamera3DCanvas::
selectFaces(const std::vector<CGeomFace3D *> &faces, bool clear, bool update)
{
  auto changed = false;

  if (clear)
    changed = deselectAll(update);

  for (auto *face : faces) {
    if (! face->getSelected()) {
      face->setSelected(true);
      changed = true;
    }
  }

  if (changed && update) {
    updateCurrentObject();

    this->update();

    Q_EMIT stateChanged();
  }

  return changed;
}

void
CQCamera3DCanvas::
selectFaceEdge(CGeomFace3D *face, int ind, bool clear, bool update)
{
  if (clear)
    deselectAll(/*update*/ false);

  auto *face1 = dynamic_cast<CQCamera3DGeomFace *>(face);

  face1->selectEdge(ind);

  if (update) {
    this->update();

    Q_EMIT stateChanged();
  }
}

bool
CQCamera3DCanvas::
selectVertex(CGeomVertex3D *vertex, bool clear, bool update)
{
  auto changed = false;

  if (clear)
    changed = deselectAll(update);

  if (! vertex->isSelected()) {
    vertex->setSelected(true);
    changed = true;
  }

  if (changed && update) {
    updateCurrentObject();

    this->update();

    Q_EMIT stateChanged();
  }

  return changed;
}

bool
CQCamera3DCanvas::
selectVertices(const ObjectSelectInds &selectInds, bool update)
{
  auto changed = deselectAll(update);

  auto *scene = app_->getScene();

  CGeomVertex3D *pvertex = nullptr;

  for (auto *object : scene->getObjects()) {
    auto po = selectInds.find(object);
    if (po == selectInds.end()) continue;

    const auto &vinds = (*po).second;

    const auto &faces = object->getFaces();

    for (const auto *face : faces) {
      auto vertices = face->getVertices();

      for (const auto &v : vertices) {
        auto pv = vinds.find(v);
        if (pv == vinds.end()) continue;

        auto &vertex = object->getVertex(v);

        if (! pvertex)
          pvertex = const_cast<CGeomVertex3D *>(&vertex);

        if (! vertex.isSelected()) {
          vertex.setSelected(true);
          changed = true;
        }
      }
    }
  }

  if (changed && update) {
    updateCurrentObject();

    this->update();

    Q_EMIT stateChanged();
  }

  return changed;
}

bool
CQCamera3DCanvas::
deselectAll(bool update)
{
  bool changed = false;

  auto *scene = app_->getScene();

  for (auto *object : scene->getObjects()) {
    if (object->getSelected()) {
      object->setSelected(false);
      changed = true;
    }

    const auto &faces = object->getFaces();

    for (auto *face : faces) {
      if (face->getSelected()) {
        face->setSelected(false);
        changed = true;
      }

      auto vertices = face->getVertices();

      for (const auto &v : vertices) {
        auto &vertex = object->getVertex(v);

        if (vertex.isSelected()) {
          vertex.setSelected(false);
          changed = true;
        }
      }

      auto *face1 = dynamic_cast<CQCamera3DGeomFace *>(face);

      if (face1->hasSelectedEdge()) {
        face1->deselecttEdges();
        changed = true;
      }
    }
  }

  currentObject_ = nullptr;
  currentFace_   = nullptr;
  currentVertex_ = nullptr;

  if (changed && update) {
    updateCurrentObject();

    this->update();

    Q_EMIT stateChanged();
  }

  return changed;
}

CQCamera3DCanvas::SelectData
CQCamera3DCanvas::
getSelection() const
{
  SelectData selectData;

  selectData.type = calcSelectType();

  if      (selectData.type == SelectType::OBJECT)
    selectData.objects = getSelectedObjects();
  else if (selectData.type == SelectType::FACE)
    selectData.faces = getSelectedFaces();
  else if (selectData.type == SelectType::EDGE)
    selectData.faceEdges = getSelectedFaceEdges();
  else if (selectData.type == SelectType::POINT)
    selectData.vertices = getSelectedVertices();

  return selectData;
}

void
CQCamera3DCanvas::
updateCurrentObject()
{
  auto selectData = getSelection();

  setCurrentObject(nullptr, /*update*/ false);
  setCurrentFace  (nullptr, /*update*/ false);
  setCurrentVertex(nullptr, /*update*/ false);

  if      (selectData.type == SelectType::OBJECT) {
    if (! selectData.objects.empty())
      setCurrentObject(selectData.objects[0], /*update*/ false);
  }
  else if (selectData.type == SelectType::FACE) {
    if (! selectData.faces.empty())
      setCurrentFace(selectData.faces[0], /*update*/ false);
  }
  else if (selectData.type == SelectType::POINT) {
    if (! selectData.vertices.empty()) {
      auto pv = selectData.vertices.begin();

      auto *object = (*pv).first;
      int   ind    = *(*pv).second.begin();

      auto &vertex = object->getVertex(ind);

      setCurrentVertex(&vertex, /*update*/ false);
    }
  }
}

std::vector<CGeomObject3D *>
CQCamera3DCanvas::
getSelectedObjects() const
{
  std::vector<CGeomObject3D *> selectedObjects;

  auto *scene = app_->getScene();

  for (auto *object : scene->getObjects()) {
    if (object->getSelected())
      selectedObjects.push_back(object);
  }

  return selectedObjects;
}

std::vector<CGeomFace3D *>
CQCamera3DCanvas::
getSelectedFaces() const
{
  std::vector<CGeomFace3D *> selectedFaces;

  auto *scene = app_->getScene();

  for (auto *object : scene->getObjects()) {
    const auto &faces = object->getFaces();

    for (auto *face : faces) {
      if (face->getSelected())
        selectedFaces.push_back(face);
    }
  }

  return selectedFaces;
}

CQCamera3DCanvas::ObjectSelectInds
CQCamera3DCanvas::
getSelectedVertices() const
{
  ObjectSelectInds objectSelectInds;

  auto *scene = app_->getScene();

  for (auto *object : scene->getObjects()) {
    const auto &faces = object->getFaces();

    for (auto *face : faces) {
      const auto &vertices = face->getVertices();

      for (const auto &v : vertices) {
        auto &vertex = object->getVertex(v);

        if (vertex.isSelected())
          objectSelectInds[object].insert(v);
      }
    }
  }

  return objectSelectInds;
}

//---

void
CQCamera3DCanvas::
mousePressEvent(QMouseEvent *e)
{
  auto *mode = mouseModeMgr_->currentMode();

  if (mode)
    return mode->mousePress(e);

  //---

  mouseData_.pressed = true;
  mouseData_.button  = e->button();
  mouseData_.press   = CPoint2D(e->x(), e->y());

  mouseData_.isShift   = (e->modifiers() & Qt::ShiftModifier);
  mouseData_.isControl = (e->modifiers() & Qt::ControlModifier);

  //---

  if      (mouseData_.button == Qt::LeftButton) {
    if      (editType() == EditType::SELECT) {
      selectObjectAtMouse();
    }
    else if (editType() == EditType::CURSOR) {
      moveCursorToMouse(CPoint3D(mouseData_.press));
    }
  }
  else if (mouseData_.button == Qt::RightButton) {
    showEyelineAtMouse();
  }
}

void
CQCamera3DCanvas::
mouseMoveEvent(QMouseEvent *e)
{
  auto *mode = mouseModeMgr_->currentMode();

  if (mode)
    return mode->mouseMove(e);

  //---

  mouseData_.move.x = e->x();
  mouseData_.move.y = e->y();

  mouseData_.isShift   = (e->modifiers() & Qt::ShiftModifier);
  mouseData_.isControl = (e->modifiers() & Qt::ControlModifier);

  //---

  auto dx = CMathUtil::sign(mouseData_.move.x - mouseData_.press.x);
  auto dy = CMathUtil::sign(mouseData_.move.y - mouseData_.press.y);

  auto bbox = app_->canvas()->bbox();
  auto size = bbox.getSize();

  auto dx1 = mouseScale()*dx*size.getX();
  auto dy1 = mouseScale()*dy*size.getX();

  if      (mouseData_.button == Qt::LeftButton) {
  }
  else if (mouseData_.button == Qt::MiddleButton) {
    // TODO: move current light
    if (viewType_ == ViewType::PERSPECTIVE) {
      auto *camera = getInteractiveCamera();

      if      (mouseData_.isShift) {
        camera->movePosition(0, dx1);
        camera->movePosition(1, dy1);
      }
      else if (mouseData_.isControl) {
        camera->rotatePosition(2, dx*0.05);
      }
      else {
        //camera->rotatePosition(1, dx*0.05);
        camera->moveAroundY(-dx1);

        //camera->rotatePosition(0, dy*0.05);
        camera->moveAroundX(dy1);
      }
    }
  }
  else if (mouseData_.button == Qt::RightButton) {
    if (viewType_ == ViewType::PERSPECTIVE) {
      auto *camera = getInteractiveCamera();

      // calc eye line
      calcEyeLine(CPoint3D(mouseData_.press), eyeLine_);
      setEyeLineLabel();

      if (mouseData_.isShift) {
        camera->moveOrigin(0, dx1);
        camera->moveOrigin(1, dy1);
      }
    }
  }

  //---

  mouseData_.press = mouseData_.move;

  update();
}

void
CQCamera3DCanvas::
mouseReleaseEvent(QMouseEvent *e)
{
  auto *mode = mouseModeMgr_->currentMode();

  if (mode)
    return mode->mouseRelease(e);

  //---

  mouseData_.pressed = false;
  mouseData_.button  = Qt::NoButton;
}

void
CQCamera3DCanvas::
wheelEvent(QWheelEvent *e)
{
  auto *mode = mouseModeMgr_->currentMode();

  if (mode)
    return mode->wheelEvent(e);

  //---

  auto d = bbox_.getMaxSize()/100.0;

  auto dw = e->angleDelta().y()/250.0;

  auto *camera = getInteractiveCamera();

  camera->moveFront(dw*d);

  update();
}

//---

void
CQCamera3DCanvas::
selectObjectAtMouse()
{
  auto *camera = getCurrentCamera();

  auto useAnim = (app_->animName() != "");

  auto selectType = calcSelectType();

  //---

  // calc eye line
  calcEyeLine(CPoint3D(mouseData_.press), eyeLine_);
  setEyeLineLabel();

  if (mouseData_.isShift) {
    setIntersectPoints(eyeLine_.pv1, eyeLine_.pv2);

    updateAnnotation();
    update();

    return;
  }

  //---

  auto objectNodeMatrices = app_->calcNodeMatrices();

  auto viewMatrix       = camera->viewMatrix();
  auto projectionMatrix = camera->worldMatrix();

  struct InsideFace {
    uint                  ind    { 0 };
    CGeomFace3D*          face   { nullptr };
    int                   edge   { -1 };
    int                   vertex { -1 };
    std::vector<CPoint3D> points;
    CPoint3D              ipoint;
    double                t      { 0.0 };
  };

  std::vector<InsideFace> insideFaces;
  InsideFace              insideFace;

  double minLineDist = 0.0;

  CLine3D line(eyeLine_.px1, eyeLine_.px2);

  // intersect eye line with object faces
  Objects objects;

  if      (editMode() == EditMode::EDIT) {
    objects = selectedObjects_;
  }
  else if (isLocalMode()) {
    objects = getSelectedObjects();
  }
  else {
    auto *scene = app_->getScene();

    objects = scene->getObjects();
  }

  for (auto *object : objects) {
    if (! object->getVisible())
      continue;

    auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);
    assert(object1);

    //---

    auto *rootObject = object->getRootObject();

    auto &nodeMatrices = objectNodeMatrices[rootObject->getInd()];

    //---

    auto modelMatrix = CMatrix3DH(object1->getHierTransform());
    auto meshMatrix  = CMatrix3DH(object->getMeshGlobalTransform());

    //---

    const auto &faces = object->getFaces();

    for (auto *face : faces) {
      if (! face->getVisible())
        continue;

      // get face points
      const auto &vertices = face->getVertices();

      std::vector<CPoint3D> points;

      for (const auto &v : vertices) {
        const auto &vertex = object->getVertex(v);
        const auto &model  = vertex.getModel();

        auto p = model;

        //---

        if (useAnim)
          p = app_->adjustAnimPoint(vertex, p, nodeMatrices);

        p = meshMatrix*p;

        //---

        auto view    = viewMatrix*modelMatrix*p;
        auto project = projectionMatrix*view;

        points.push_back(project);
      }

      auto np = points.size();

      if      (selectType == SelectType::OBJECT || selectType == SelectType::FACE) {
        if (np < 3) continue;

        // add face if inside
        uint i2 = 1;
        uint i3 = 2;

        for (uint i = 0; i < np - 2; ++i) {
          CTriangle3D triangle(points[0], points[i2], points[i3]);

          double tmin, tmax;
          if (triangle.intersect(line, &tmin, &tmax)) {
            InsideFace insideFace1;

            insideFace1.ind    = insideFaces.size();
            insideFace1.face   = face;
            insideFace1.points = points;
            insideFace1.ipoint = line.interp(tmin);
            insideFace1.t      = tmin;

            insideFaces.push_back(insideFace1);
          }

          i2 = i3;
          i3 = i2 + 1;

          if (i3 >= np)
            i3 = 0;
        }
      }
      else if (selectType == SelectType::EDGE) {
        if (np < 3) continue;

        // intersect line with face and find nearest edge to intersect point
        std::vector<CPoint3D> ipoints;

        uint i2 = 1;
        uint i3 = 2;

        for (uint i = 0; i < np - 2; ++i) {
          CTriangle3D triangle(points[0], points[i2], points[i3]);

          double tmin, tmax;
          if (triangle.intersect(line, &tmin, &tmax)) {
            auto ipoint = line.interp(tmin);

            ipoints.push_back(ipoint);
          }

          i2 = i3;
          i3 = i2 + 1;

          if (i3 >= np)
            i3 = 0;
        }

        if (! ipoints.empty()) {
          auto pi = ipoints[0];

          int minInd = -1;

          size_t i1 = np - 1;

          for (size_t i2 = 0; i2 < np; i1 = i2++) {
            double dist = 0.0;
            (void) CMathGeom3D::PointLineDistance(pi, CLine3D(points[i1], points[i2]), &dist);

            if (minInd < 0 || dist < minLineDist) {
              minInd      = i1;
              minLineDist = dist;
            }
          }

          if (minInd >= 0) {
            insideFace.face = face;
            insideFace.edge = minInd;
          }
        }
      }
      else if (selectType == SelectType::POINT) {
        // add point if closed
        for (uint i = 0; i < np; ++i) {
          auto d = points[i].distanceTo(eyeLine_.px1);

          if (! insideFace.face || d < insideFace.t) {
            insideFace.face   = face;
            insideFace.vertex = i;
            insideFace.t      = d;
          }
        }
      }
    }
  }

  // sort inside faces by z
  std::map<double, int> tfaces;

  if (! insideFaces.empty()) {
    // sort by t
    for (const auto &insideFace : insideFaces)
      tfaces[-insideFace.t] = insideFace.ind;
  }

  //---

  bool clear = ! mouseData_.isControl;

  bool changed = false;

  if      (selectType == SelectType::OBJECT) {
    if (! tfaces.empty()) {
      const auto &insideFace = insideFaces[tfaces.begin()->second];

      auto *object = insideFace.face->getObject();

      // toggle selection
      if (mouseData_.isControl) {
        if (! object->getSelected())
          object->setSelected(true);
        else
          object->setSelected(false);

        changed = true;
      }
      else
        changed = selectObject(object, clear, /*update*/false);
    }
  }
  else if (selectType == SelectType::FACE) {
    if (! tfaces.empty()) {
      const auto &insideFace = insideFaces[tfaces.begin()->second];

      changed = selectFace(insideFace.face, clear, /*update*/false);

      setIntersectPoints(insideFace.ipoint, insideFace.ipoint);
    }
  }
  else if (selectType == SelectType::EDGE) {
    if (insideFace.face) {
      selectFaceEdge(insideFace.face, insideFace.edge, /*clear*/true, /*update*/true);
      changed = true;
    }
  }
  else if (selectType == SelectType::POINT) {
    if (insideFace.face) {
      auto *object = insideFace.face->getObject();

      for (auto *vertex : object->getVertices()) {
        if (vertex->getInd() == uint(insideFace.vertex)) {
          changed = selectVertex(vertex, /*clear*/true, /*update*/false);
        }
      }
    }
  }

  if (changed) {
    addObjectsData();

    updateCurrentObject();

    updateAnnotation();

    update();

    Q_EMIT stateChanged();
  }
}

void
CQCamera3DCanvas::
moveCursorToMouse(const CPoint3D &p)
{
  // get eye line in view coords
  calcEyeLine(p, eyeLine_);
  setEyeLineLabel();

  CLine3D line(eyeLine_.pp1, eyeLine_.pp2);

  //---

  // get camera right/up plane in view coordinates
  auto *camera = getCurrentCamera();

  auto c  = camera->origin();
  auto p1 = c + camera->right();
  auto p2 = c + camera->up();

  auto viewMatrix = camera->viewMatrix();

  auto cv1 = viewMatrix*c;
  auto cv2 = viewMatrix*p1;
  auto cv3 = viewMatrix*p2;

  auto cameraPlane = CPlane3D(cv1.point(), cv2.point(), cv3.point());

  // intersect eye line and camera plane
  double t = 0.0;
  (void) cameraPlane.intersectLine(line, &t);
  auto pi = line.point(t);

  // decompose intersect point into vector coordinates in camera plane
  auto pvi = CVector3D(cv1.point(), pi); // intersect vector

  auto pv1 = CVector3D(cv1, cv2).normalized(); // right vector
  auto pv2 = cameraPlane.getNormal().crossProduct(pv1).normalized(); // up vector

  auto pd1 = pv1.dotProduct(pvi); // intersect vector as multiple of right vector
  auto pd2 = pv2.dotProduct(pvi); // intersect vector as multiple of up vector

  // use vector sizes to interpolate back to world position using camera vectors
  auto pi3 = camera->origin() + pd1*camera->right() + pd2*camera->up();

  setCursor(pi3.point());
}

void
CQCamera3DCanvas::
showEyelineAtMouse()
{
  if (viewType_ != ViewType::PERSPECTIVE)
    return;

  auto *camera = getCurrentCamera();

  // calc eye line
  calcEyeLine(CPoint3D(mouseData_.press), eyeLine_);
  setEyeLineLabel();

  // set camera position/origin
  if      (mouseData_.isShift)
    camera->setPosition(CVector3D(eyeLine_.pv1));
  else if (mouseData_.isControl)
    camera->setOrigin(CVector3D(eyeLine_.pv1));

  // draw eye line
  updateAnnotation();
  update();

  Q_EMIT stateChanged();
}

void
CQCamera3DCanvas::
setEyeLineLabel()
{
  auto posStr = QString("%1 %2 %3").arg(eyeLine_.pv1.x).arg(eyeLine_.pv1.y).arg(eyeLine_.pv1.z);
  app_->status()->setMouseLabel(posStr);

  updateShapes();

  Q_EMIT eyeLineChanged();
}

//---

bool
CQCamera3DCanvas::
event(QEvent *e)
{
  if (e->type() == QEvent::KeyPress) {
    auto *ke = static_cast<QKeyEvent *>(e);

    if (ke->key() == Qt::Key_Tab) {
      keyPressEvent(ke);
      return true;
    }
  }

  return CQCamera3DWidget::event(e);
}

void
CQCamera3DCanvas::
keyPressEvent(QKeyEvent *e)
{
  if (e->key() == Qt::Key_Escape) {
    mouseModeMgr_->endAllModes();
    return;
  }

  //---

  auto *mode = mouseModeMgr_->currentMode();

  if (mode)
    return mode->keyPress(e);

  //---

  if (e->key() == Qt::Key_Tab) {
    setEditMode(editMode() == EditMode::OBJECT ? EditMode::EDIT : EditMode::OBJECT);
    return;
  }

  //---

  mouseData_.isShift   = (e->modifiers() & Qt::ShiftModifier);
  mouseData_.isControl = (e->modifiers() & Qt::ControlModifier);

  if      (editType_ == EditType::CAMERA) {
    cameraKeyPress(e);
    return;
  }
  else if (editType_ == EditType::LIGHT) {
    lightKeyPress(e);
    return;
  }
  else if (editType_ == EditType::SELECT) {
    if (! mouseData_.isShift && ! mouseData_.isControl) {
      if      (e->key() == Qt::Key_G) { // Grab
        setEditType(EditType::MOVE);
        return;
      }
      else if (e->key() == Qt::Key_S) { // Scale
        setEditType(EditType::SCALE);
        return;
      }
      else if (e->key() == Qt::Key_R) { // Rotate
        setEditType(EditType::ROTATE);
        return;
      }
      else if (e->key() == Qt::Key_E) { // Extrude
        extrudeMode();
        return;
      }
    }
  }

#if 0
  if      (mouseType_ == MouseType::OBJECT)
    objectKeyPress(e);
  else if (mouseType_ == MouseType::FACE)
    faceKeyPress(e);
  else if (mouseType_ == MouseType::EDGE)
    edgeKeyPress(e);
  else if (mouseType_ == MouseType::POINT)
    pointKeyPress(e);
#endif
}

void
CQCamera3DCanvas::
cameraKeyPress(QKeyEvent *e)
{
  // TODO: use camera vectors

  auto *camera = getInteractiveCamera();

  auto k = e->key();

  auto d = bbox_.getMaxSize()/100.0;

  if (viewType_ == ViewType::PERSPECTIVE) {
    if      (k == Qt::Key_A) {
      if (mouseData_.isShift)
        camera->setRoll(camera->roll() - 0.1);
      else
        camera->moveRight(-d);
    }
    else if (k == Qt::Key_D) {
      if (mouseData_.isShift)
        camera->setRoll(camera->roll() + 0.1);
      else
        camera->moveRight(d);
    }
    else if (k == Qt::Key_S) {
      if (mouseData_.isShift)
        camera->setPitch(camera->pitch() - 0.1);
      else
        camera->moveAroundX(-3*d);
    }
    else if (k == Qt::Key_W) {
      if (mouseData_.isShift)
        camera->setPitch(camera->pitch() + 0.1);
      else
        camera->moveAroundX(3*d);
    }
    else if (k == Qt::Key_Q) {
      if      (mouseData_.isShift)
        camera->setYaw(camera->yaw() + 0.1);
      else if (mouseData_.isControl)
        camera->moveAroundZ(3*d);
      else
        camera->moveAroundY(10*d);
    }
    else if (k == Qt::Key_E) {
      if      (mouseData_.isShift)
        camera->setYaw(camera->yaw() - 0.1);
      else if (mouseData_.isControl)
        camera->moveAroundZ(-3*d);
      else
        camera->moveAroundY(-10*d);
    }
    else if (k == Qt::Key_Left) {
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
    else if (k == Qt::Key_Question) {
      std::cerr << "Key                    Shift       Control        \n";
      std::cerr << "---   ---------------  ----------  ---------------\n";
      std::cerr << "A     Move Left        Roll -                     \n";
      std::cerr << "D     Move Right       Roll +                     \n";
      std::cerr << "W     Move Front       Pitch +     Move Around X +\n";
      std::cerr << "S     Move Back        Pitch -     Move Around X -\n";
      std::cerr << "Q     Move Around Y +  Yaw +       Move Around Z +\n";
      std::cerr << "E     Move Around Y -  Yaw -       Move Around Z -\n";
      std::cerr << "Left  Move Left                                   \n";
      std::cerr << "Right Move Right                                  \n";
      std::cerr << "Up    Move Up                                     \n";
      std::cerr << "Down  Move Down                                   \n";
    }
  }
  else if (viewType_ == ViewType::TOP || viewType_ == ViewType::BOTTOM) { // XZ
    if      (k == Qt::Key_A) {
      camera->moveRight(-d);
    }
    else if (k == Qt::Key_D) {
      camera->moveRight(d);
    }
  }

  update();

  Q_EMIT stateChanged();
}

void
CQCamera3DCanvas::
lightKeyPress(QKeyEvent *)
{
  // TODO
}

void
CQCamera3DCanvas::
objectKeyPress(QKeyEvent *e)
{
  auto *object = currentObject(/*includeRoot*/true);
  if (! object) return;

  auto d = bbox_.getMaxSize()/100.0;

  CVector3D u, v, w;
  if (isMouseBasis())
    getBasis(object, u, v, w);
  else {
    auto *camera = getCurrentCamera();

    u = camera->right();
    v = camera->up();
    w = camera->front();
  }

//CBBox3D obbox;
//object->getModelBBox(obbox, /*hier*/false);

  auto scale  = 1.1;
  auto iscale = 1.0/scale;

  if      (e->key() == Qt::Key_A) {
    if      (mouseData_.isShift)
      scaleObject(object, CVector3D(iscale, 1.0, 1.0));
    else if (mouseData_.isControl)
      rotateObject(object, -0.1, u);
    else
      moveObject(object, -d*u);
  }
  else if (e->key() == Qt::Key_D) {
    if      (mouseData_.isShift)
      scaleObject(object, CVector3D(scale, 1.0, 1.0));
    else if (mouseData_.isControl)
      rotateObject(object, 0.1, u);
    else
      moveObject(object, d*u);
  }
  else if (e->key() == Qt::Key_W) {
    if      (mouseData_.isShift)
      scaleObject(object, CVector3D(1.0, scale, 1.0));
    else if (mouseData_.isControl)
      rotateObject(object, 0.1, v);
    else
      moveObject(object, d*v);
  }
  else if (e->key() == Qt::Key_S) {
    if      (mouseData_.isShift)
      scaleObject(object, CVector3D(1.0, iscale, 1.0));
    else if (mouseData_.isControl)
      rotateObject(object, -0.1, v);
    else
      moveObject(object, -d*v);
  }
  else if (e->key() == Qt::Key_Up) {
    if      (mouseData_.isShift)
      scaleObject(object, CVector3D(1.0, 1.0, scale));
    else if (mouseData_.isControl)
      rotateObject(object, 0.1, w);
    else
      moveObject(object, d*w);
  }
  else if (e->key() == Qt::Key_Down) {
    if      (mouseData_.isShift)
      scaleObject(object, CVector3D(1.0, 1.0, iscale));
    else if (mouseData_.isControl)
      rotateObject(object, -0.1, w);
    else
      moveObject(object, -d*w);
  }
  else if (e->key() == Qt::Key_Question) {
    std::cerr << "Key               Shift      Control   \n";
    std::cerr << "---  -----------  ---------  ----------\n";
    std::cerr << "A    Move Left    Scale X-   Rotate X- \n";
    std::cerr << "D    Move Right   Scale X+   Rotate X+ \n";
    std::cerr << "W    Move Front   Scale Y-   Rotate Y- \n";
    std::cerr << "S    Move Back    Scale Y+   Rotate Y+ \n";
    std::cerr << "Up   Move Back    Scale Z+   Rotate Z- \n";
    std::cerr << "Down Move Front   Scale Z-   Rotate Z+ \n";
  }

  update();

  Q_EMIT stateChanged();
}

void
CQCamera3DCanvas::
faceKeyPress(QKeyEvent *e)
{
  auto faces = getSelectedFaces();
  if (faces.empty()) return;

  auto d = bbox_.getMaxSize()/100.0;

  auto faceNormal = [](const CGeomFace3D *face) {
    CVector3D n;

    if (face->getNormalSet())
      n = face->getNormal();
    else
      face->calcNormal(n);

    return n;
  };

  if      (e->key() == Qt::Key_Up || e->key() == Qt::Key_Down) {
    auto dir = (e->key() == Qt::Key_Up ? 1 : -1);

    if (editType() == EditType::MOVE) {
      for (auto *face : faces) {
        auto n = faceNormal(face);

        moveFace(face, dir*d*n);
      }

      updateObjectsData();
    }
  }
  else if (e->key() == Qt::Key_S) {
    for (auto *face : faces)
      face->divideCenter();

    updateObjectsData();
  }
  else if (e->key() == Qt::Key_E) {
    extrude();
  }
}

void
CQCamera3DCanvas::
edgeKeyPress(QKeyEvent *e)
{
  auto faceEdges = getSelectedFaceEdges();
  if (faceEdges.empty()) return;

  const auto &pf = faceEdges.begin();

  auto *face    = (*pf).first;
  auto  edgeNum = (*pf).second[0];

  auto d = bbox_.getMaxSize()/100.0;

  CVector3D n;

  if (face->getNormalSet())
    n = face->getNormal();
  else
    face->calcNormal(n);

  auto *face1 = dynamic_cast<CQCamera3DGeomFace *>(face);

  const auto &edge = face1->edge(edgeNum);

  auto *object = face->getObject();

  auto &v1 = object->getVertex(edge.v1);
  auto &v2 = object->getVertex(edge.v2);

  if      (e->key() == Qt::Key_Up) {
    if (editType() == EditType::MOVE) {
      v1.setModel(v1.getModel() + d*n);
      v2.setModel(v2.getModel() + d*n);
      updateObjectsData();
    }
  }
  else if (e->key() == Qt::Key_Down) {
    if (editType() == EditType::MOVE) {
      v1.setModel(v1.getModel() - d*n);
      v2.setModel(v2.getModel() - d*n);
      updateObjectsData();
    }
  }
}

void
CQCamera3DCanvas::
pointKeyPress(QKeyEvent *e)
{
  auto *vertex = currentVertex();
  if (! vertex) return;

  auto n = vertex->getNormal();

  auto d = bbox_.getMaxSize()/100.0;

  if      (e->key() == Qt::Key_Up) {
    if (editType() == EditType::MOVE) {
      vertex->setModel(vertex->getModel() + d*n);
      updateObjectsData();
    }
  }
  else if (e->key() == Qt::Key_Down) {
    if (editType() == EditType::MOVE) {
      vertex->setModel(vertex->getModel() - d*n);
      updateObjectsData();
    }
  }
}

//---

void
CQCamera3DCanvas::
moveObject(CGeomObject3D *object, const CVector3D &d)
{
  auto m = CMatrix3D::translation(d.getX(), d.getY(), d.getZ());

  object->setTranslate(m*object->getTranslate());
}

void
CQCamera3DCanvas::
scaleObject(CGeomObject3D *object, const CVector3D &d)
{
  auto m = CMatrix3D::scale(d.getX(), d.getY(), d.getZ());

  object->setScale(m*object->getScale());
}

void
CQCamera3DCanvas::
rotateObject(CGeomObject3D *object, double da, const CVector3D &axis)
{
  auto m = CMatrix3D::rotation(da, axis);

  object->setRotate(m*object->getRotate());
}

//---

void
CQCamera3DCanvas::
moveFace(CGeomFace3D *face, const CVector3D &d)
{
  face->moveBy(d);
}

//---

void
CQCamera3DCanvas::
setCursor(const CPoint3D &v)
{
  cursor_ = v;

  update();

  Q_EMIT stateChanged();
}

//---

void
CQCamera3DCanvas::
extrudeMode()
{
  auto *mode = new CQCamera3DExtrudeMouseMode();

  mouseModeMgr_->startMode(mode);
}

void
CQCamera3DCanvas::
extrude()
{
  auto faces = getSelectedFaces();
  if (faces.empty()) return;

  auto d = bbox_.getMaxSize()/20.0;

  mouseModeIFace_->setExtrudeMoveDelta(d);

  std::vector<CGeomFace3D *> newFaces;

  for (auto *face : faces) {
    auto *newFace = face->extrude(d);

    newFaces.push_back(newFace);
  }

  updateObjectsData();

  selectFaces(newFaces, /*clear*/true, /*update*/true);
}

void
CQCamera3DCanvas::
extrudeMove(double d)
{
  auto faces = getSelectedFaces();
  if (faces.empty()) return;

  for (auto *face : faces)
    face->extrudeMove(d);

  updateObjectsData();
}

void
CQCamera3DCanvas::
loopCut()
{
  auto faces = getSelectedFaces();
  if (faces.empty()) return;

  std::vector<CGeomFace3D *> newFaces;

  for (auto *face : faces) {
    auto *newFace = face->loopCut();

    if (newFace)
      newFaces.push_back(newFace);
  }

  updateObjectsData();

  selectFaces(newFaces, /*clear*/true, /*update*/true);
}

//---

void
CQCamera3DCanvas::
calcEyeLine(const CPoint3D &pos, EyeLine &eyeLine, bool verbose) const
{
  auto *camera = getCurrentCamera();

  auto projectionMatrix  = camera->worldMatrix();
  auto iProjectionMatrix = projectionMatrix.inverse();

  auto viewMatrix  = camera->viewMatrix();
  auto iViewMatrix = viewMatrix.inverse();

  //---

#if 0
  CBBox3D vbbox(CPoint3D(-1, -1, -1), CPoint3D(1, 1, 1));

  CBBox3D pbbox;
  for (const auto &p : vbbox.points()) {
    double px1, py1, pz1;
    projectionMatrix.multiplyPoint(p.x, p.y, p.z, &px1, &py1, &pz1);
    pbbox.add(CPoint3D(px1, py1, pz1));
  }

  std::cerr << pbbox << "\n";
#endif

  //---

  // set pixel (mouse) position in GL coords
  auto aspect = this->aspect();

  double x1, y1;

  if (aspect > 1.0) {
    x1 = CMathUtil::map(pos.x, 0, pixelWidth () - 1.0, -aspect,  aspect);
    y1 = CMathUtil::map(pos.y, 0, pixelHeight() - 1.0,     1.0,    -1.0);
  }
  else {
    x1 = CMathUtil::map(pos.x, 0, pixelWidth () - 1.0,   -1.0,      1.0);
    y1 = CMathUtil::map(pos.y, 0, pixelHeight() - 1.0,  aspect, -aspect);
  }

  //std::cerr << "PX: " << x1 << " " << y1 << "\n";

  //---

  auto pointToView = [&](const CPoint3D &p, CPoint3D &pp, CPoint3D &pv) {
    // convert projected point to camera
    double xp, yp, zp;
    iProjectionMatrix.multiplyPoint(p.x, p.y, p.z, &xp, &yp, &zp);

    pp = CPoint3D(xp, yp, zp);

    zp = p.z;

    // convert camera point to view
    double xv, yv, zv;
    iViewMatrix.multiplyPoint(xp, yp, zp, &xv, &yv, &zv);

    pv = CPoint3D(xv, yv, zv);

    //std::cerr << p << " " << pp << " " << pv << "\n";
  };

#if 0
  std::map<double, CPoint3D> viewPoints;

  for (int i = 0; i < 100; ++i) {
    auto z1 = CMathUtil::map(i, 0, 100, camera->near(), camera->far());

    CPoint3D pp, pv;
    pointToView(CPoint3D(x1, y1, z1), pp, pv);

    viewPoints[pv.z] = pv;
  }

  auto pv1 = viewPoints.begin ()->second;
  auto pv2 = viewPoints.rbegin()->second;
#else
  //auto px1 = CPoint3D(x1, y1, eyeLineZ()      );
  //auto px2 = CPoint3D(x1, y1, eyeLineZ() + 0.1);
  auto px1 = CPoint3D(x1, y1, eyeLineZ1());
  auto px2 = CPoint3D(x1, y1, eyeLineZ2());

  CPoint3D pp1, pv1;
  pointToView(px1, pp1, pv1);
  //std::cerr << "P1: " << px1 << " " << pp1 << " " << pv1 << "\n";

  CPoint3D pp2, pv2;
  pointToView(px2, pp2, pv2);
  //std::cerr << "P2: " << px2 << " " << pp2 << " " << pv2 << "\n";
#endif

  eyeLine.px1 = px1; eyeLine.px2 = px2;
  eyeLine.pp1 = pp1; eyeLine.pp2 = pp2;
  eyeLine.pv1 = pv1; eyeLine.pv2 = pv2;
  //std::cerr << eyeLine.pv1 << " " << eyeLine.pv2 << "\n";

  //---

  // calc vector
  eyeLine.v = CVector3D(eyeLine.pv2, eyeLine.pv1).normalized();

  if (verbose)
    std::cerr << CPoint2D(x1, y1) << " " << pv1 << "\n";

  //---

#if 0
  // check maps back
//viewMatrix.multiplyVector(p.x, p.y, p.z, &xv1, &yv1, &zv1);
  viewMatrix.multiplyPoint(eyeLine.pv1.x, eyeLine.pv1.y, eyeLine.pv1.z, &xv1, &yv1, &zv1);
  projectionMatrix.multiplyPoint(xv1, yv1, zv1, &xp1, &yp1, &zp1);

  if (verbose)
    std::cerr << CPoint3D(xv1, yv1, zv1) << " " << CPoint3D(xp1, yp1, zp1) << "\n";
#endif

  eyeLine.isSet = true;
}

void
CQCamera3DCanvas::
cameraChanged()
{
  if (isShowPlanes())
    updateAnnotation();

  shape_->updateGeometry();

  Q_EMIT stateChanged();
  Q_EMIT cameraStateChanged();

  update();
}

void
CQCamera3DCanvas::
setDebugCamera(bool b)
{
  enableDebugCamera_ = b;

  Q_EMIT cameraStateChanged();

  update();
}

CGLCameraIFace *
CQCamera3DCanvas::
getInteractiveCamera() const
{
  if (isDebugCamera())
    return debugCamera_;
  else
    return getCurrentCamera();
}

CGLCameraIFace *
CQCamera3DCanvas::
getCurrentCamera(bool view) const
{
  if (! view)
    return getCameraById(cameraInd_);
  else {
    if (viewType_ == ViewType::PERSPECTIVE)
      return getCameraById(cameraInd_);
    else
      return getViewCamera();
  }
}

CGLCameraIFace *
CQCamera3DCanvas::
getCameraById(uint id) const
{
  for (auto *camera : cameras_) {
    if (camera->id() == id)
      return camera;
  }

  return nullptr;
}

CQCamera3DLight *
CQCamera3DCanvas::
currentLight() const
{
  auto *light = getLightById(lightInd_);

  if (! light && ! lights_.empty())
    light = lights_[0];

  return light;
}

void
CQCamera3DCanvas::
setCurrentLight(CQCamera3DLight *light)
{
  lightInd_ = (light ? light->id() : 0);
}

CQCamera3DLight*
CQCamera3DCanvas::
getLightById(uint id) const
{
  for (auto *light : lights_) {
    if (light->id() == id)
      return light;
  }

  return nullptr;
}

void
CQCamera3DCanvas::
updateShapes()
{
  shape_->updateGeometry();
}

void
CQCamera3DCanvas::
updateAnnotation()
{
  annotation_->updateGeometry();
}

void
CQCamera3DCanvas::
updateBillboard()
{
  billboard_->updateGeometry();
}

void
CQCamera3DCanvas::
updateOverlay()
{
  overlay_->updateGeometry();
}

void
CQCamera3DCanvas::
updateOverlay2D()
{
//overlay2D_->updateGeometry();
}

//---

CGeomObject3D *
CQCamera3DCanvas::
currentObject(bool includeRoot) const
{
  if (currentObject_)
    return currentObject_;

  return (includeRoot ? defaultRootObject() : nullptr);
}

CQCamera3DGeomObject *
CQCamera3DCanvas::
currentGeomObject(bool includeRoot) const
{
  auto *object = currentObject(includeRoot);

  if (object)
    object = dynamic_cast<CQCamera3DGeomObject *>(getGeomObject(object));

  return dynamic_cast<CQCamera3DGeomObject *>(object);
}

void
CQCamera3DCanvas::
setCurrentObject(CGeomObject3D *object, bool update)
{
  currentObject_ = object;

  if (update)
    Q_EMIT stateChanged();
}

void
CQCamera3DCanvas::
setCurrentFace(CGeomFace3D *face, bool update)
{
  currentFace_ = face;

  if (update)
    Q_EMIT stateChanged();
}

void
CQCamera3DCanvas::
setCurrentVertex(CGeomVertex3D *vertex, bool update)
{
  currentVertex_ = vertex;

  if (update)
    Q_EMIT stateChanged();
}

CQCamera3DCanvas::FaceEdges
CQCamera3DCanvas::
getSelectedFaceEdges() const
{
  FaceEdges faceEdges;

  auto *scene = app_->getScene();

  for (auto *object : scene->getObjects()) {
    for (auto *face : object->getFaces()) {
      auto *face1 = dynamic_cast<CQCamera3DGeomFace *>(face);

      const auto &edges = face1->selectedEdges();

      for (const auto &edge : edges)
        faceEdges[face].push_back(edge);
    }
  }

  return faceEdges;
}

//---

void
CQCamera3DCanvas::
setEditMode(const EditMode &m)
{
  auto m1 = m;

  if      (m1 == EditMode::OBJECT) {
    selectObjects(selectedObjects_, /*clear*/true);

    selectedObjects_.clear();
  }
  else if (m1 == EditMode::EDIT) {
    selectedObjects_ = getSelectedObjects();

    if (selectedObjects_.empty())
      m1 = EditMode::OBJECT;
  }

  editMode_ = m1;

  updateStateLabel();

  Q_EMIT editModeChanged();
}

void
CQCamera3DCanvas::
updateStateLabel()
{
  QString s;

  s += (editMode() == EditMode::OBJECT ? "Object" : "Edit");

  if (editMode() == EditMode::EDIT) {
    s += " : ";

    if (selectedObjects_.size() == 1)
      s += QString::fromStdString(selectedObjects_[0]->getName());
    else
      s += QString("#%1").arg(selectedObjects_.size());
  }

  app_->status()->setStateLabel(s);
}

//---

void
CQCamera3DCanvas::
setViewType(const ViewType &v)
{
  viewType_ = v;

  if (updateQuad_) {
    updateQuad_ = false;

    updateQuadCameras();
  }

  update();
}

CGLCameraIFace *
CQCamera3DCanvas::
getViewCamera() const
{
  switch (viewType()) {
    case ViewType::PERSPECTIVE: return perspectiveCamera_;
    case ViewType::TOP        : return topCamera_;
    case ViewType::BOTTOM     : return bottomCamera_;
    case ViewType::LEFT       : return leftCamera_;
    case ViewType::RIGHT      : return rightCamera_;
    case ViewType::FRONT      : return frontCamera_;
    case ViewType::BACK       : return backCamera_;
  }

  return perspectiveCamera_;
}

//---

void
CQCamera3DCanvas::
setMoveDirection(const MoveDirection &v)
{
  moveDirection_ = v;

  updateAnnotation();

  update();
}

//---

CGeomObject3D *
CQCamera3DCanvas::
addCircle()
{
  return addObject("circle", AddObjectType::CIRCLE);
}

CGeomObject3D *
CQCamera3DCanvas::
addCone()
{
  return addObject("cone", AddObjectType::CONE);
}

CGeomObject3D *
CQCamera3DCanvas::
addCube()
{
  return addObject("cube", AddObjectType::CUBE);
}

CGeomObject3D *
CQCamera3DCanvas::
addCylinder()
{
  return addObject("cylinder", AddObjectType::CYLINDER);
}

CGeomObject3D *
CQCamera3DCanvas::
addPlane()
{
  return addObject("plane", AddObjectType::PLANE);
}

CGeomObject3D *
CQCamera3DCanvas::
addPyramid()
{
  return addObject("pyramid", AddObjectType::PYRAMID);
}

CGeomObject3D *
CQCamera3DCanvas::
addSphere()
{
  return addObject("sphere", AddObjectType::SPHERE);
}

CGeomObject3D *
CQCamera3DCanvas::
addTorus()
{
  return addObject("torus", AddObjectType::TORUS);
}

CGeomObject3D *
CQCamera3DCanvas::
addObject(const std::string &typeName, const AddObjectType &type)
{
  auto *scene = app_->getScene();

  auto n = scene->getObjects().size();
  std::string name = typeName + "." + std::to_string(n + 1);

  auto *object = CGeometry3DInst->createObject3D(scene, name);

  scene->addObject(object);

  auto *mode = new CQCamera3DAddMouseMode(type, object);

  mouseModeMgr_->startMode(mode);

  mode->updateObject();

  Q_EMIT objectAdded();

  selectObject(object, /*clear*/true, /*update*/true);

  return object;
}

void
CQCamera3DCanvas::
setOptions(CQCamera3DOptions *options)
{
  opWidget_->setOptions(options);
}

void
CQCamera3DCanvas::
showOptions()
{
  opWidget_->show();
}

void
CQCamera3DCanvas::
hideOptions()
{
  opWidget_->hide();
}

//---

bool
CQCamera3DCanvas::
createFontTexture(uint *texture, int w, int h, uchar *data)
{
  //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // allocate texture id
  glGenTextures(1, texture);
  if (! checkError("glGenTextures")) return false;

  // set texture type
  glBindTexture(GL_TEXTURE_2D, *texture);
  if (! checkError("glBindTexture")) return false;

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
  //if (! checkError("glPixelStorei")) return false;
#endif

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, data);
  //if (! checkError("glTexImage2D")) return false;

  glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
  //if (! checkError("glHint")) return false;

  glGenerateMipmap(GL_TEXTURE_2D);
  //if (! checkError("glGenerateMipmap")) return false;

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
CQCamera3DCanvas::
getBasis(CGeomObject3D *object, CVector3D &u, CVector3D &v, CVector3D &w) const
{
  CVector3D u1, v1, w1;
  object->getBasis(u1, v1, w1);

  const auto &modelMatrix = object->getHierTransform();

  u = (modelMatrix*u1).normalized();
  v = (modelMatrix*v1).normalized();
  w = (modelMatrix*w1).normalized();
}
