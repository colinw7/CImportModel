#include <CQNewGLCanvas.h>
#include <CQNewGLModel.h>
#include <CQNewGLControl.h>
#include <CQNewGLFont.h>

#include <CImportBase.h>
#include <CQGLBuffer.h>
#include <CGLTexture.h>
#include <CGeometry3D.h>
#include <CGeomScene3D.h>
#include <CGeomTexture.h>
#include <CQGLUtil.h>
#include <CFile.h>
#include <CStrUtil.h>
#include <CInterval.h>

#include <QOpenGLShaderProgram>
#include <QMouseEvent>

#include <GL/glu.h>

namespace {

CGLVector3D QColorToVector(const QColor &c) {
  return CGLVector3D(c.red()/255.0, c.green()/255.0, c.blue()/255.0);
}

CGLVector3D ColorToVector(const CRGBA &c) {
  return CGLVector3D(c.getRed(), c.getGreen(), c.getBlue());
}

CRGBA QColorToRGBA(const QColor &c) {
  return CRGBA(c.red()/255.0, c.green()/255.0, c.blue()/255.0);
}

double max3(double x, double y, double z) {
  return std::max(std::max(x, y), z);
}

bool checkError() {
  // check GL error generated
  GLenum err = glGetError();

  if (err != GL_NO_ERROR) {
    std::cerr << "OpenGL Error: " << gluErrorString(err) << "\n";
    return false;
  }

  return true;
}

static char cString[256];

const char *toCString(const QString &str) {
  assert(str.length() < 255);
  strcpy(cString, str.toStdString().c_str());
  return cString;
}

}

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
setFlipYZ(bool b)
{
  flipYZ_ = b;

  updateObjectData();
  update();
}

void
CQNewGLCanvas::
setShowAxis(bool b)
{
  showAxis_ = b;

  update();
}

void
CQNewGLCanvas::
setInvertDepth(bool b)
{
  invertDepth_ = b;

  for (auto *light : lights_) {
    if (isInvertDepth())
      light->pos.flipYZ();
  }

  updateObjectData();
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

  updateObjectData();
  update();
}

void
CQNewGLCanvas::
setEmissionColor(const QColor &c)
{
  emissionColor_ = c;

  updateObjectData();
  update();
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

void
CQNewGLCanvas::
setShowNormals(bool b)
{
  showNormals_ = b;
}

//---

CGLVector3D
CQNewGLCanvas::
lightPos(int i) const
{
  return lights_[i]->pos;
}

QColor
CQNewGLCanvas::
lightColor(int i) const
{
  return lights_[i]->color;
}

void
CQNewGLCanvas::
setCurrentLight(int i)
{
  currentLight_ = i;
}

void
CQNewGLCanvas::
setCurrentLightPos(const CGLVector3D &v)
{
  lights_[currentLight_]->pos = v;
}

void
CQNewGLCanvas::
setCurrentLightColor(const QColor &c)
{
  lights_[currentLight_]->color = c;
}

//---

bool
CQNewGLCanvas::
loadModel(const QString &modelName, CGeom3DType format, const LoadData &loadData)
{
  // import model
  import_ = CImportBase::createModel(format);

  if (! import_) {
    std::cerr << "Invalid format\n";
    return false;
  }

  import_->setInvertX(loadData.invertX);
  import_->setInvertY(loadData.invertY);
  import_->setInvertZ(loadData.invertZ);

  CFile file(modelName.toStdString());

  if (! import_->read(file)) {
    std::cerr << "Failed to read model '" << modelName.toStdString() << "'\n";
    return false;
  }

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

  setDepthTest  (depthTest_);
  setCullFace   (cullFace_);
  setFrontFace  (frontFace_);
  setPolygonLine(polygonLine_);

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

  lightShaderProgram_ = new CQNewGLShaderProgram(this);
  lightShaderProgram_->addShaders("light.vs", "light.fs");

  addLight(/*update*/false);

  //---

  axisShaderProgram_ = new CQNewGLShaderProgram(this);
  axisShaderProgram_->addShaders("axis.vs", "axis.fs");

  //---

  normalShaderProgram_ = new CQNewGLShaderProgram(this);
  normalShaderProgram_->addShaders("normal.vs", "normal.fs");

  //---

  loadInitTextures();

  //---

  // camera
  camera_ = new CQNewGLCamera(this, CGLVector3D(0.0f, 0.0f, 1.414f));

  camera_->setLastPos(float(app_->windowWidth())/2.0f, float(app_->windowHeight())/2.0f);

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

  updateObjectData();

  //---

  updateLightBuffer();
}

void
CQNewGLCanvas::
addLight(bool update)
{
  auto il = lights_.size();

  double x = 0;

  if (il > 0) {
    auto il2 = (il + 1)/2;

    if (il & 1)
      x -= il2*0.1;
    else
      x += il2*0.1;
  }

  auto *light = new Light;

  light->name = QString("Light%1").arg(il + 1);

  light->pos = CGLVector3D(x, 0.4f, 0.4f);

  lights_.push_back(light);

  if (update)
    updateLightBuffer();
}

bool
CQNewGLCanvas::
createFontTexture(uint *texture, int w, int h, uchar *data)
{
  //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // allocate texture id
  glGenTextures(1, texture);
  if (! checkError()) return false;

  // set texture type
  glBindTexture(GL_TEXTURE_2D, *texture);
  if (! checkError()) return false;

#if 0
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  if (! checkError()) return false;

#if 0
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
#else
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#endif
  if (! checkError()) return false;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);

  glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
  if (! checkError()) return false;
#else
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  if (! checkError()) return false;
#endif

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, data);
  if (! checkError()) return false;

  glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
  if (! checkError()) return false;

  glGenerateMipmap(GL_TEXTURE_2D);
  if (! checkError()) return false;

  //---

#if 0
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
  for (const auto &textureData : initTextures_) {
    CImageFileSrc src(textureData.name);

    auto image = CImageMgrInst->createImage(src);

    if (textureData.flipV)
      image->flipH();

    auto *texture1 = CGeometryInst->createTexture(image);

    texture1->setName(textureData.name);

    (void) getTexture(texture1, /*add*/true);
  }

  //---

  auto *importBase = this->importBase();

  auto &scene = importBase->getScene();

  for (auto *object : scene.getObjects()) {
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

void
CQNewGLCanvas::
updateLightBuffer()
{
  for (auto *light : lights_) {
    // set up vertex data (and buffer(s)) and configure vertex attributes
    if (! light->buffer)
      light->buffer = new CQGLBuffer(lightShaderProgram_);

    auto addLightPoint = [&](float x, float y, float z) {
      light->buffer->addPoint(x, y, z);
    };

    addLightPoint(-0.5f, -0.5f, -0.5f);
    addLightPoint( 0.5f, -0.5f, -0.5f);
    addLightPoint( 0.5f,  0.5f, -0.5f);

    addLightPoint( 0.5f,  0.5f, -0.5f);
    addLightPoint(-0.5f,  0.5f, -0.5f);
    addLightPoint(-0.5f, -0.5f, -0.5f);

    addLightPoint(-0.5f, -0.5f,  0.5f);
    addLightPoint( 0.5f, -0.5f,  0.5f);
    addLightPoint( 0.5f,  0.5f,  0.5f);

    addLightPoint( 0.5f,  0.5f,  0.5f);
    addLightPoint(-0.5f,  0.5f,  0.5f);
    addLightPoint(-0.5f, -0.5f,  0.5f);

    addLightPoint(-0.5f,  0.5f,  0.5f);
    addLightPoint(-0.5f,  0.5f, -0.5f);
    addLightPoint(-0.5f, -0.5f, -0.5f);

    addLightPoint(-0.5f, -0.5f, -0.5f);
    addLightPoint(-0.5f, -0.5f,  0.5f);
    addLightPoint(-0.5f,  0.5f,  0.5f);

    addLightPoint( 0.5f,  0.5f,  0.5f);
    addLightPoint( 0.5f,  0.5f, -0.5f);
    addLightPoint( 0.5f, -0.5f, -0.5f);

    addLightPoint( 0.5f, -0.5f, -0.5f);
    addLightPoint( 0.5f, -0.5f,  0.5f);
    addLightPoint( 0.5f,  0.5f,  0.5f);

    addLightPoint(-0.5f, -0.5f, -0.5f);
    addLightPoint( 0.5f, -0.5f, -0.5f);
    addLightPoint( 0.5f, -0.5f,  0.5f);

    addLightPoint( 0.5f, -0.5f,  0.5f);
    addLightPoint(-0.5f, -0.5f,  0.5f);
    addLightPoint(-0.5f, -0.5f, -0.5f);

    addLightPoint(-0.5f,  0.5f, -0.5f);
    addLightPoint( 0.5f,  0.5f, -0.5f);
    addLightPoint( 0.5f,  0.5f,  0.5f);

    addLightPoint( 0.5f,  0.5f,  0.5f);
    addLightPoint(-0.5f,  0.5f,  0.5f);
    addLightPoint(-0.5f,  0.5f, -0.5f);

    light->buffer->load();
  }
}

void
CQNewGLCanvas::
updateObjectData()
{
  if (! initialized_)
    return;

  if      (app_->isShowBone())
    updateBoneData();
  else if (isBonesEnabled())
    updateBonesData();
  else
    updateModelData();

  updateAxis();

//text_->updateText();
}

void
CQNewGLCanvas::
updateModelData()
{
  // set up vertex data (and buffer(s)) and configure vertex attributes
  auto *importBase = this->importBase();

  if (! importBase) {
    sceneScale_    = 1.0;
    invSceneScale_ = 1.0;
    return;
  }

  //---

  auto &scene = importBase->getScene();

  CBBox3D bbox;
//scene.getBBox(bbox);

  //---

  bool useBones    = (isAnimEnabled() && animName() != "");
  bool showNormals = isShowNormals();

  //---

  const auto &objects = scene.getObjects();

  for (auto *object : objects) {
    ObjectData *objectData { nullptr };

    auto pd = objectDatas_.find(object);

    if (pd == objectDatas_.end())
      pd = objectDatas_.insert(pd, ObjectDatas::value_type(object, new ObjectData));

    objectData = (*pd).second;

    if (! objectData->object)
      objectData->object = object;

    if (! objectData->modelData.buffer)
      objectData->modelData.buffer = new CQGLBuffer(modelShaderProgram_);

    if (showNormals && ! objectData->normalData.buffer)
      objectData->normalData.buffer = new CQGLBuffer(normalShaderProgram_);

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

    auto *diffuseTexture  = object->getDiffuseTexture();
    auto *specularTexture = object->getSpecularTexture();
    auto *normalTexture   = object->getNormalTexture();
    auto *emissiveTexture = object->getEmissiveTexture();

    //---

    const auto &faces = object->getFaces();

    int pos = 0;

    for (const auto *face : faces) {
      FaceData faceData;

      faceData.boneId = nodeId;

      //---

      const auto &color = face->color().value_or(QColorToRGBA(diffuseColor()));

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

      auto *emissiveTexture1 = face->getEmissiveTexture();

      if (! emissiveTexture1)
        emissiveTexture1 = emissiveTexture;

      //---

      if (diffuseTexture1)
        faceData.diffuseTexture = getTexture(diffuseTexture1, /*add*/true);

      if (specularTexture1)
        faceData.specularTexture = getTexture(specularTexture1, /*add*/true);

      if (normalTexture1)
        faceData.normalTexture = getTexture(normalTexture1, /*add*/true);

      if (emissiveTexture1)
        faceData.emissiveTexture = getTexture(emissiveTexture1, /*add*/true);

//    const auto &material = face->getMaterial();

//    faceData.ambient   = material.getAmbient ();
//    faceData.diffuse   = material.getDiffuse ();
//    faceData.specular  = material.getSpecular();
      faceData.emission  = face->emission().value_or(QColorToRGBA(emissionColor()));
      faceData.shininess = face->shininess().value_or(app_->shininess());

      CVector3D normal;

      if (face->getNormalSet())
        normal = face->getNormal();
      else
        face->calcNormal(normal);

      const auto &vertices = face->getVertices();

      faceData.pos = pos;
      faceData.len = int(vertices.size());

      int iv = 0;

      for (const auto &v : vertices) {
        auto &vertex = object->getVertex(v);

        const auto &model = vertex.getModel();

        //---

        auto normal1 = normal;
        auto color1  = color;

        if (vertex.hasNormal())
          normal1 = vertex.getNormal();

        if (vertex.hasColor())
          color1 = vertex.getColor();

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

        auto addPoint = [&](const CPoint3D &p, const CVector3D &n) {
          CPoint3D p1, p2;
          calcPoint(p, p1, p2);

          auto p11 = (useBones ? p1 : p2);

          modelBuffer->addPoint(float(p11.x), float(p11.y), float(p11.z));

          bbox += p2;

          //---

          if (showNormals) {
            auto p3 = p11 + sceneScale_*calcNormal(n).normalize()/100.0;

            normalBuffer->addPoint(float(p11.x), float(p11.y), float(p11.z));
            normalBuffer->addColor(1.0f, 1.0f, 1.0f);

            normalBuffer->addPoint(float(p3.x), float(p3.y), float(p3.z));
            normalBuffer->addColor(1.0f, 1.0f, 1.0f);
          }
        };

        auto addNormal = [&](const CVector3D &n) {
          auto n1 = calcNormal(n);

          modelBuffer->addNormal(float(n1.getX()), float(n1.getY()), float(n1.getZ()));
        };

        //---

        addPoint(model, normal1);

        //---

        addNormal(normal1);

        //---

        modelBuffer->addColor(color1.getRedF(), color1.getGreenF(), color1.getBlueF());

        //---

        if (faceData.diffuseTexture) {
          const auto &tpoint = face->getTexturePoint(vertex, iv);

          modelBuffer->addTexturePoint(float(tpoint.x), float(tpoint.y));
        }
        else
          modelBuffer->addTexturePoint(0.0f, 0.0f);

        //---

        const auto &jointData = vertex.getJointData();

        if (jointData.nodeDatas[0].node >= 0) {
          modelBuffer->addBoneIds(jointData.nodeDatas[0].node,
                                  jointData.nodeDatas[1].node,
                                  jointData.nodeDatas[2].node,
                                  jointData.nodeDatas[3].node);

          modelBuffer->addBoneWeights(jointData.nodeDatas[0].weight,
                                      jointData.nodeDatas[1].weight,
                                      jointData.nodeDatas[2].weight,
                                      jointData.nodeDatas[3].weight);
        }

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
  }

  //---

  sceneSize_   = bbox.getSize();
  sceneCenter_ = bbox.getCenter();

  std::cerr << "Scene Size : " << sceneSize_.getX() << " " <<
               sceneSize_.getY() << " " << sceneSize_.getZ() << "\n";
  std::cerr << "Scene Center : " << sceneCenter_.getX() << " " <<
               sceneCenter_.getY() << " " << sceneCenter_.getZ() << "\n";

  sceneScale_    = max3(sceneSize_.getX(), sceneSize_.getY(), sceneSize_.getZ());
  invSceneScale_ = (sceneScale_ > 0.0 ? 1.0/sceneScale_ : 1.0);

  //std::cerr << "Scene Scale : " << sceneScale_ << "\n";

  modelTranslate_.setX(-sceneCenter_.getX());
  modelTranslate_.setY(-sceneCenter_.getY());
  modelTranslate_.setZ(-sceneCenter_.getZ());

  modelScale_.setX(invSceneScale_);
  modelScale_.setY(invSceneScale_);
  modelScale_.setZ(invSceneScale_);
}

void
CQNewGLCanvas::
updateBonesData()
{
  // set up vertex data (and buffer(s)) and configure vertex attributes
  auto *importBase = this->importBase();

  if (! importBase) {
    sceneScale_    = 1.0;
    invSceneScale_ = 1.0;
    return;
  }

  //---

  auto &scene = importBase->getScene();

  CBBox3D bbox;
//scene.getBBox(bbox);

  //---

  auto pointSize = 0.05f;

  auto color = CRGBA(1.0, 0.0, 0.0);

  auto addCube = [&](ObjectData *objectData, const CPoint3D &c, int nodeId, int &pos) {
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
      buffer->addColor(c.getRedF(), c.getGreenF(), c.getBlueF());
      buffer->addTexturePoint(0.0f, 0.0f);
    };

    for (GLint i = 5; i >= 0; i--) {
      FaceData faceData;

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
  };

  //---

  auto addLine = [&](ObjectData *objectData, const CPoint3D &c1, const CPoint3D &c2,
                     int boneId, int parentBoneId, int &pos) {
    auto *buffer = objectData->bonesData.buffer;

    auto addPoint = [&](const CPoint3D &p, const CPoint3D &n, const CRGBA &c, int id) {
      buffer->addPoint(p.x, p.y, p.z);
      buffer->addNormal(n.x, n.y, n.z);
      buffer->addColor(c.getRedF(), c.getGreenF(), c.getBlueF());
      buffer->addTexturePoint(float(id), 0.0f);
    };

    FaceData faceData;

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
  };

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

  const auto &objects = scene.getObjects();

  for (auto *object : objects) {
    ObjectData *objectData { nullptr };

    auto pd = objectDatas_.find(object);

    if (pd == objectDatas_.end())
      pd = objectDatas_.insert(pd, ObjectDatas::value_type(object, new ObjectData));

    objectData = (*pd).second;

    if (! objectData->object)
      objectData->object = object;

    objectData->bonesData.buffer = bonesShaderProgram_->getBuffer();

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

      addCube(objectData, c, nodeId, pos);

      //---

      if (node.parent >= 0) {
        //int parentInd = rootObject->mapNodeId(node.parent);

        const auto &pnode = rootObject->getNode(node.parent);

        if (pnode.valid) {
          auto c1 = transformNode(pnode);

          addLine(objectData, c, c1, nodeId, node.parent, pos);
        }
      }
    }

    buffer->load();
  }

  //---

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

void
CQNewGLCanvas::
updateBoneData()
{
  // set up vertex data (and buffer(s)) and configure vertex attributes
  auto *importBase = this->importBase();

  if (! importBase) {
    sceneScale_    = 1.0;
    invSceneScale_ = 1.0;
    return;
  }

  //---

  CBBox3D bbox;
//scene.getBBox(bbox);

  auto updateSceneSize = [&]() {
    sceneSize_   = bbox.getSize();
    sceneCenter_ = bbox.getCenter();

    sceneScale_    = max3(sceneSize_.getX(), sceneSize_.getY(), sceneSize_.getZ());
    invSceneScale_ = (sceneScale_ > 0.0 ? 1.0/sceneScale_ : 1.0);

    std::cerr << "Scene Size : " << sceneSize_.getX() << " " <<
                 sceneSize_.getY() << " " << sceneSize_.getZ() << "\n";
    std::cerr << "Scene Center : " << sceneCenter_.getX() << " " <<
                 sceneCenter_.getY() << " " << sceneCenter_.getZ() << "\n";
  //std::cerr << "Scene Scale : " << sceneScale_ << "\n";
  };

  //---

  auto addCube = [&](ObjectData *objectData, const CMatrix3D &m, int nodeId, int &pos,
                     double cubeSize, const CRGBA &color) {
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
      buffer->addColor(c.getRedF(), c.getGreenF(), c.getBlueF());

      bbox += CPoint3D(p1.x, p1.y, p1.z);
    };

    for (GLint i = 5; i >= 0; i--) {
      FaceData faceData;

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
  };

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

    objectData->boneData.buffer = boneShaderProgram_->getBuffer();

    //---

    auto *buffer = objectData->boneData.buffer;

    buffer->clearBuffers();

    objectData->boneData.faceDatas.clear();

    //---

    int pos = 0;

    addCube(objectData, app_->boneMatrix(), boneInd, pos, 1.0, CRGBA(1.0, 0.0, 0.0));

    updateSceneSize();

#if 0
    //auto cubeSize = max3(sceneSize_.getX(), sceneSize_.getY(), sceneSize_.getZ())/2.0;
    auto cubeSize = 1.0;

    addCube(objectData,
      CMatrix3D::translation(sceneCenter_.getX(), sceneCenter_.getY(), sceneCenter_.getZ()),
      boneInd, pos, cubeSize, CRGBA(0.0, 1.0, 0.0));
#else
    addCube(objectData, CMatrix3D::identity(), boneInd, pos, 1.0, CRGBA(0.0, 1.0, 0.0));
#endif

    buffer->load();
  }

  //---

  updateSceneSize();
}

CGLTexture *
CQNewGLCanvas::
getTexture(CGeomTexture *texture, bool add)
{
  auto pt = glTextures_.find(texture->id());

  if (pt == glTextures_.end()) {
    if (! add)
      return nullptr;

    const auto &image = texture->image()->image();

    auto *glTexture = new CGLTexture(image);

    glTexture->setName(texture->name());

    TextureData textureData;

    textureData.glTexture   = glTexture;
    textureData.geomTexture = texture;

    pt = glTextures_.insert(pt, GLTextures::value_type(texture->id(), textureData));

    app_->control()->updateTextures();
  }

  const auto &textureData = (*pt).second;

  return textureData.glTexture;
}

CGLTexture *
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
  camera_->updateFrameTime();

  //---

  // clear canvas
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //---

  // set view
  auto aspect = float(app_->windowWidth())/float(app_->windowHeight());

  if (isOrtho())
    paintData_.projection =
      CGLMatrix3D::ortho(-1, 1, -1, 1, camera_->near(), camera_->far());
  else
    paintData_.projection =
      CGLMatrix3D::perspective(camera_->zoom(), aspect, camera_->near(), camera_->far());

  paintData_.view = camera_->getViewMatrix();

  paintData_.viewPos = camera_->position();

  //---

  // draw model objects
  for (auto &po : objectDatas_) {
    auto *objectData = po.second;

    auto *object = objectData->object;
    if (! object->getVisible())
      continue;

    if      (app_->isShowBone())
      drawObjectBone(objectData);
    else if (isBonesEnabled())
      drawObjectBones(objectData);
    else
      drawObjectModel(objectData);

    if (isShowNormals())
      drawNormals(objectData);
  }

  //---

  // draw light
  drawLight();

  //---

  // draw test text
//text_->render();

  //---

  // draw axis
  if (isShowAxis())
    drawAxis();
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

  //---

  auto isSelected = object->getSelected();

  objectData->modelData.buffer->bind();

  modelShaderProgram_->bind();

  //---

  modelShaderProgram_->setUniformValue("viewPos", CQGLUtil::toVector(paintData_.viewPos));

  // pass projection matrix to shader (note that in this case it could change every frame)
  modelShaderProgram_->setUniformValue("projection", CQGLUtil::toQMatrix(projectionMatrix()));

  // camera/view transformation
  modelShaderProgram_->setUniformValue("view", CQGLUtil::toQMatrix(viewMatrix()));

  int il = 0;

  for (auto *light : lights_) {
    auto lightName = QString("lights[%1]").arg(il);

    modelShaderProgram_->setUniformValue(toCString(lightName + ".enabled"), true);

    modelShaderProgram_->setUniformValue(toCString(lightName + ".position"),
      CQGLUtil::toVector(light->pos));
    modelShaderProgram_->setUniformValue(toCString(lightName + ".color"),
      CQGLUtil::toVector(QColorToVector(light->color)));

    ++il;

    if (il >= 3)
      break;
  }

  for ( ; il < 3; ++il) {
    auto lightName = QString("lights[%1]").arg(il);

    modelShaderProgram_->setUniformValue(toCString(lightName + ".enabled"), false);
  }

  auto ambientColor1 = QColorToVector(ambientColor());

  modelShaderProgram_->setUniformValue("ambientColor", CQGLUtil::toVector(ambientColor1));

  // model rotation
  auto modelMatrix = CMatrix3D::identity();
  modelMatrix.scaled(modelScale_.getX(), modelScale_.getY(), modelScale_.getZ());
  modelMatrix.rotated(modelRotate_.x(), CVector3D(1.0, 0.0, 0.0));
  modelMatrix.rotated(modelRotate_.y(), CVector3D(0.0, 1.0, 0.0));
  modelMatrix.rotated(modelRotate_.z(), CVector3D(0.0, 0.0, 1.0));
  modelMatrix.translated(modelTranslate_.getX(), modelTranslate_.getY(), modelTranslate_.getZ());

  if (useBones) {
    auto meshMatrix = getMeshGlobalTransform(objectData, /*invert*/false);
    modelMatrix = meshMatrix*modelMatrix;
  }

  modelShaderProgram_->setUniformValue("model", CQGLUtil::toQMatrix(modelMatrix));

  //---

  // bones transform
  if (useBones) {
    updateNodeMatrices(objectData);

    modelShaderProgram_->setUniformValueArray("globalBoneTransform",
      &paintData_.nodeMatrices[0], paintData_.numNodeMatrices);
  }

  //---

  modelShaderProgram_->setUniformValue("useBones", useBones);
  modelShaderProgram_->setUniformValue("useBonePoints", useBonePoints);

  modelShaderProgram_->setUniformValue("isSelected", isSelected);

  modelShaderProgram_->setUniformValue("ambientStrength" , float(app_->ambientStrength()));
  modelShaderProgram_->setUniformValue("diffuseStrength" , float(app_->diffuseStrength()));
  modelShaderProgram_->setUniformValue("specularStrength", float(app_->specularStrength()));
  modelShaderProgram_->setUniformValue("emissiveStrength", float(app_->emissiveStrength()));

  //---

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

    auto emissionColor1 = ColorToVector(faceData.emission);

    modelShaderProgram_->setUniformValue("emissionColor", CQGLUtil::toVector(emissionColor1));

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

  objectData->modelData.buffer->unbind();
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

  bonesShaderProgram_->setUniformValue("viewPos", CQGLUtil::toVector(paintData_.viewPos));

  // pass projection matrix to shader (note that in this case it could change every frame)
  bonesShaderProgram_->setUniformValue("projection", CQGLUtil::toQMatrix(projectionMatrix()));

  // camera/view transformation
  bonesShaderProgram_->setUniformValue("view", CQGLUtil::toQMatrix(viewMatrix()));

  bonesShaderProgram_->setUniformValue("lightColor",
    CQGLUtil::toVector(QColorToVector(lightColor(currentLight_))));
  bonesShaderProgram_->setUniformValue("lightPos",
    CQGLUtil::toVector(lightPos(currentLight_)));

  auto ambientColor1 = QColorToVector(ambientColor());

  bonesShaderProgram_->setUniformValue("ambientColor", CQGLUtil::toVector(ambientColor1));

  // model rotation
  auto modelMatrix = getModelMatrix();
  bonesShaderProgram_->setUniformValue("model", CQGLUtil::toQMatrix(modelMatrix));

  //---

  // bones transform
  if (useBones) {
    updateNodeMatrices(objectData);

    bonesShaderProgram_->setUniformValueArray("globalBoneTransform",
      &paintData_.nodeMatrices[0], paintData_.numNodeMatrices);
  }

  //---

  bonesShaderProgram_->setUniformValue("useBones", useBones);
  bonesShaderProgram_->setUniformValue("isSelected", isSelected);

  bonesShaderProgram_->setUniformValue("ambientStrength" , float(app_->ambientStrength()));
  bonesShaderProgram_->setUniformValue("diffuseStrength" , float(app_->diffuseStrength()));
  bonesShaderProgram_->setUniformValue("specularStrength", float(app_->specularStrength()));
  bonesShaderProgram_->setUniformValue("emissiveStrength", float(app_->emissiveStrength()));

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

    bonesShaderProgram_->setUniformValue("isWireframe", 0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);

    if (isPolygonLine()) {
      bonesShaderProgram_->setUniformValue("isWireframe", 1);

      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

      glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
    }
  }

  objectData->bonesData.buffer->unbind();
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

  boneShaderProgram_->setUniformValue("viewPos", CQGLUtil::toVector(paintData_.viewPos));

  // pass projection matrix to shader (note that in this case it could change every frame)
  boneShaderProgram_->setUniformValue("projection", CQGLUtil::toQMatrix(projectionMatrix()));

  // camera/view transformation
  boneShaderProgram_->setUniformValue("view", CQGLUtil::toQMatrix(viewMatrix()));

  boneShaderProgram_->setUniformValue("lightColor",
    CQGLUtil::toVector(QColorToVector(lightColor(currentLight_))));
  boneShaderProgram_->setUniformValue("lightPos",
    CQGLUtil::toVector(lightPos(currentLight_)));

  auto ambientColor1 = QColorToVector(ambientColor());

  boneShaderProgram_->setUniformValue("ambientColor", CQGLUtil::toVector(ambientColor1));

  // model rotation
  auto modelMatrix = getModelMatrix();
  boneShaderProgram_->setUniformValue("model", CQGLUtil::toQMatrix(modelMatrix));

  //---

  boneShaderProgram_->setUniformValue("ambientStrength" , float(app_->ambientStrength()));
  boneShaderProgram_->setUniformValue("diffuseStrength" , float(app_->diffuseStrength()));
  boneShaderProgram_->setUniformValue("specularStrength", float(app_->specularStrength()));
  boneShaderProgram_->setUniformValue("emissiveStrength", float(app_->emissiveStrength()));

  //---

  // render model
  for (const auto &faceData : objectData->boneData.faceDatas) {
    // material
    boneShaderProgram_->setUniformValue("shininess", float(faceData.shininess));

    //---

#if 0
    boneShaderProgram_->setUniformValue("isWireframe", 0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);

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
drawAxis()
{
  auto *axisBuffer = axisShaderProgram_->getBuffer();

  axisBuffer->bind();

  axisShaderProgram_->bind();

  //---

  // pass projection matrix to shader (note that in this case it could change every frame)
  axisShaderProgram_->setUniformValue("projection", CQGLUtil::toQMatrix(projectionMatrix()));

  // camera/view transformation
  axisShaderProgram_->setUniformValue("view", CQGLUtil::toQMatrix(viewMatrix()));

  // model rotation
  auto modelMatrix = getModelMatrix();
  axisShaderProgram_->setUniformValue("model", CQGLUtil::toQMatrix(modelMatrix));

  //---

  // render axis
  for (const auto &faceData : axisFaceDatas_) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
  }

  axisBuffer->unbind();

  //---

  for (auto *text : axisTexts_)
    text->render();
}

void
CQNewGLCanvas::
drawNormals(ObjectData *objectData)
{
  if (! objectData->normalData.buffer)
    return;

  objectData->normalData.buffer->bind();

  normalShaderProgram_->bind();

  //---

  // pass projection matrix to shader (note that in this case it could change every frame)
  normalShaderProgram_->setUniformValue("projection", CQGLUtil::toQMatrix(projectionMatrix()));

  // camera/view transformation
  normalShaderProgram_->setUniformValue("view", CQGLUtil::toQMatrix(viewMatrix()));

  // model rotation
  auto modelMatrix = getModelMatrix();
  normalShaderProgram_->setUniformValue("model", CQGLUtil::toQMatrix(modelMatrix));

  //---

  // draw lines
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glDrawArrays(GL_LINES, 0, objectData->normalData.buffer->numPoints());

  //---

  objectData->normalData.buffer->unbind();
}

void
CQNewGLCanvas::
drawLight()
{
  for (auto *light : lights_) {
    // setup light shader
    light->buffer->bind();

    lightShaderProgram_->bind();

    lightShaderProgram_->setUniformValue("projection", CQGLUtil::toQMatrix(projectionMatrix()));
    lightShaderProgram_->setUniformValue("view", CQGLUtil::toQMatrix(viewMatrix()));

    auto lightPos = light->pos;

    auto lightMatrix = CMatrix3D::translation(lightPos.x(), lightPos.y(), lightPos.z());
    lightMatrix.scaled(0.01, 0.01, 0.01);
    lightShaderProgram_->setUniformValue("model", CQGLUtil::toQMatrix(lightMatrix));

    lightShaderProgram_->setUniformValue("lightColor",
      CQGLUtil::toVector(QColorToVector(light->color)));

    // draw light
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //light->buffer->drawTriangles();

    light->buffer->unbind();
  }
}

CMatrix3D
CQNewGLCanvas::
getModelMatrix() const
{
  auto modelMatrix = CMatrix3D::identity();
  modelMatrix.scaled(invSceneScale_, invSceneScale_, invSceneScale_);
  modelMatrix.rotated(modelRotate_.x(), CVector3D(1.0, 0.0, 0.0));
  modelMatrix.rotated(modelRotate_.y(), CVector3D(0.0, 1.0, 0.0));
  modelMatrix.rotated(modelRotate_.z(), CVector3D(0.0, 0.0, 1.0));
  modelMatrix.translated(-sceneCenter_.getX(), -sceneCenter_.getY(), -sceneCenter_.getZ());
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
      childWorkData.transform  = inverseMeshMatrix*childNodeData.hierAnimMatrix*childInverseBindMatrix;

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

  paintData_.nodeMatrices.resize(paintData_.numNodeMatrices);

  int im = 0;
  for (const auto &m : nodeMatrices)
    paintData_.nodeMatrices[im++] = CQGLUtil::toQMatrix(m);
}

void
CQNewGLCanvas::
updateAxis()
{
  auto *axisBuffer = axisShaderProgram_->getBuffer();

  axisBuffer->clearBuffers();

  axisFaceDatas_.clear();

  auto sceneScale = max3(sceneSize_.getX(), sceneSize_.getY(), sceneSize_.getZ());

  auto pointSize = sceneScale/1000.0;

  auto color = CRGBA(0.5, 0.5, 0.5);

  int pos = 0;

  auto addPoint = [&](const CPoint3D &p, const CPoint3D &n, const CRGBA &c) {
    axisBuffer->addPoint(p.x, p.y, p.z);
    axisBuffer->addNormal(n.x, n.y, n.z);
    axisBuffer->addColor(c.getRedF(), c.getGreenF(), c.getBlueF());
  };

  auto addLine = [&](const CPoint3D &c1, const CPoint3D &c2, const CPoint3D &dp) {
    FaceData faceData;

    faceData.pos = pos;
    faceData.len = 4;

    auto p1 = c1 - dp;
    auto p2 = c1 + dp;
    auto p3 = c2 + dp;
    auto p4 = c2 - dp;
    auto n  = CPoint3D(0, 0, 1);

    addPoint(p1, n, color);
    addPoint(p2, n, color);
    addPoint(p3, n, color);
    addPoint(p4, n, color);

    axisFaceDatas_.push_back(faceData);

    pos += faceData.len;
  };

  //---

  CInterval interval(-sceneScale, sceneScale);

  //---

#if 0
  addLine(CPoint3D(-sceneSize_.getX(), 0, 0), CPoint3D(sceneSize_.getX(), 0, 0));
  addLine(CPoint3D(0, -sceneSize_.getY(), 0), CPoint3D(0, sceneSize_.getY(), 0));
  addLine(CPoint3D(0, 0, -sceneSize_.getZ()), CPoint3D(0, 0, sceneSize_.getZ()));
#else
  for (uint i = 0; i <= interval.calcNumMajor(); ++i) {
    double x = interval.interval(i);

    addLine(CPoint3D(x, 0, -sceneScale), CPoint3D(x, 0, sceneScale), CPoint3D(0, pointSize, 0));
    addLine(CPoint3D(x, -sceneScale, 0), CPoint3D(x, sceneScale, 0), CPoint3D(0, 0, pointSize));

    addLine(CPoint3D(-sceneScale, x, 0), CPoint3D(sceneScale, x, 0), CPoint3D(0, 0, pointSize));
    addLine(CPoint3D(0, x, -sceneScale), CPoint3D(0, x, sceneScale), CPoint3D(pointSize, 0, 0));

    addLine(CPoint3D(0, -sceneScale, x), CPoint3D(0, sceneScale, x), CPoint3D(pointSize, 0, 0));
    addLine(CPoint3D(-sceneScale, 0, x), CPoint3D(sceneScale, 0, x), CPoint3D(0, pointSize, 0));
  }
#endif

  axisBuffer->load();

  //---

  axisTexts_.clear();

  auto createText = [&](const QString &str, const CPoint3D &pos, double size) {
    auto *text = new CQNewGLText(str);

    text->setFont(font_);
    text->setColor(CQNewGLFont::Color(1, 1, 1));
    text->setPosition(CGLVector3D(pos.x, pos.y, pos.z));
    text->setSize(size);

    axisTexts_.push_back(text);

    return text;
  };

  auto ts1 = sceneScale/10.0;
  auto ts2 = sceneScale/20.0;

  (void) createText("X", CPoint3D(sceneScale, -ts1, 0), ts1);
  (void) createText("Y", CPoint3D(0, sceneScale, ts1), ts1);
  (void) createText("Z", CPoint3D(0, ts1, sceneScale), ts1);

  for (uint i = 0; i <= interval.calcNumMajor(); ++i) {
    double x = interval.interval(i);

    auto label = QString("%1").arg(x);

    auto d = label.length()*ts2/2.0;

    (void) createText(label, CPoint3D(x - d, 0, 0), ts2);
    (void) createText(label, CPoint3D(0, x - d, 0), ts2);
    (void) createText(label, CPoint3D(0, 0, x - d), ts2);
  }

  for (auto *text : axisTexts_)
    text->updateText();
}

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
  mousePressX_ = e->x();
  mousePressY_ = e->y();

  auto type = app_->type();

  if (type == CQNewGLModel::Type::CAMERA)
    camera_->setLastPos(mousePressX_, mousePressY_);

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
  mouseMoveX_ = e->x();
  mouseMoveY_ = e->y();

  //---

  auto type = app_->type();

  if      (type == CQNewGLModel::Type::CAMERA) {
    float xoffset, yoffset;
    camera_->deltaPos(mouseMoveX_, mouseMoveY_, xoffset, yoffset);

    camera_->setLastPos(mouseMoveX_, mouseMoveY_);

    camera_->processMouseMovement(xoffset, yoffset);
  }
  else if (type == CQNewGLModel::Type::LIGHT) {
    auto dx = sceneSize_.getX()*(mouseMoveX_ - mousePressX_)/width ();
    auto dy = sceneSize_.getY()*(mouseMoveY_ - mousePressY_)/height();

    lights_[currentLight_]->pos += CGLVector3D(dx, dy, 0.0f);

    app()->updateLights();
  }
  else if (type == CQNewGLModel::Type::MODEL) {
  }

  mousePressX_ = mouseMoveX_;
  mousePressY_ = mouseMoveY_;

  update();
}

void
CQNewGLCanvas::
wheelEvent(QWheelEvent *e)
{
  auto dw = e->angleDelta().y()/250.0;

  auto type = app_->type();

  if      (type == CQNewGLModel::Type::CAMERA) {
    camera_->processMouseScroll(dw);
  }
  else if (type == CQNewGLModel::Type::LIGHT) {
    auto dz = dw*sceneSize_.getZ()/4.0;

    lights_[currentLight_]->pos += CGLVector3D(0.0f, 0.0f, dz);

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
  bool isShift   = (e->modifiers() & Qt::ShiftModifier);
  bool isControl = (e->modifiers() & Qt::ControlModifier);

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
    else if (type == CQNewGLModel::Type::LIGHT) {
      lights_[currentLight_]->pos += CGLVector3D(0.0f, 0.1f, 0.0f);

      app()->updateLights();
    }
    else if (type == CQNewGLModel::Type::MODEL) {
      if (isControl) {
        modelTranslate_.setZ(modelTranslate_.getZ() + sceneSize_.z()/100.0);

        Q_EMIT modelMatrixChanged();
      }
    }
  }
  else if (e->key() == Qt::Key_S) {
    if      (type == CQNewGLModel::Type::CAMERA)
      camera_->processKeyboard(CGLCamera::Movement::BACKWARD, dt);
    else if (type == CQNewGLModel::Type::LIGHT) {
      lights_[currentLight_]->pos -= CGLVector3D(0.0f, 0.1f, 0.0f);

      app()->updateLights();
    }
    else if (type == CQNewGLModel::Type::MODEL) {
      if (isControl) {
        modelTranslate_.setZ(modelTranslate_.getZ() - sceneSize_.z()/100.0);

        Q_EMIT modelMatrixChanged();
      }
    }
  }
  else if (e->key() == Qt::Key_A) {
    if      (type == CQNewGLModel::Type::CAMERA) {
      if (! isControl)
        camera_->processKeyboard(CGLCamera::Movement::LEFT, dt);
      else
        camera_->processKeyboard(CGLCamera::Movement::UP, dt);
    }
    else if (type == CQNewGLModel::Type::LIGHT) {
      lights_[currentLight_]->pos -= CGLVector3D(0.1f, 0.0f, 0.0f);

      app()->updateLights();
    }
  }
  else if (e->key() == Qt::Key_D) {
    if      (type == CQNewGLModel::Type::CAMERA) {
      if (! isControl)
        camera_->processKeyboard(CGLCamera::Movement::RIGHT, dt);
      else
        camera_->processKeyboard(CGLCamera::Movement::DOWN, dt);
    }
    else if (type == CQNewGLModel::Type::LIGHT) {
      lights_[currentLight_]->pos += CGLVector3D(0.1f, 0.0f, 0.0f);

      app()->updateLights();
    }
  }
  else if (e->key() == Qt::Key_X) {
    if (type == CQNewGLModel::Type::MODEL) {
      if (isControl) {
        modelTranslate_.setX(modelTranslate_.getX() + sceneSize_.x()/100.0);

        Q_EMIT modelMatrixChanged();
      }
      else {
        modelRotate_.setX(modelRotate_.x() + da);

        Q_EMIT modelMatrixChanged();
      }
    }
  }
  else if (e->key() == Qt::Key_Y) {
    if (type == CQNewGLModel::Type::MODEL) {
      if (isControl)
        modelTranslate_.setY(modelTranslate_.getY() + sceneSize_.y()/100.0);
      else {
        modelRotate_.setY(modelRotate_.y() + da);

        Q_EMIT modelMatrixChanged();
      }
    }
  }
  else if (e->key() == Qt::Key_Z) {
    if (type == CQNewGLModel::Type::MODEL) {
      if (isControl)
        modelTranslate_.setZ(modelTranslate_.getZ() + sceneSize_.z()/100.0);
      else {
        modelRotate_.setZ(modelRotate_.z() + da);

        Q_EMIT modelMatrixChanged();
      }
    }
  }
  else if (e->key() == Qt::Key_Up) {
    if      (type == CQNewGLModel::Type::LIGHT) {
      lights_[currentLight_]->pos += CGLVector3D(0.0f, 0.0f, 0.1f);

      app()->updateLights();
    }
    else if (type == CQNewGLModel::Type::MODEL) {
      if (isControl) {
        modelTranslate_.setY(modelTranslate_.getY() + sceneSize_.y()/100.0);

        Q_EMIT modelMatrixChanged();
      }
      else {
        double f = 1.1;

        modelScale_.setX(modelScale_.getX()*f);
        modelScale_.setY(modelScale_.getY()*f);
        modelScale_.setZ(modelScale_.getZ()*f);

        Q_EMIT modelMatrixChanged();
      }
    }
  }
  else if (e->key() == Qt::Key_Down) {
    if      (type == CQNewGLModel::Type::LIGHT) {
      lights_[currentLight_]->pos -= CGLVector3D(0.0f, 0.0f, 0.1f);

      app()->updateLights();
    }
    else if (type == CQNewGLModel::Type::MODEL) {
      if (isControl) {
        modelTranslate_.setY(modelTranslate_.getY() - sceneSize_.y()/100.0);

        Q_EMIT modelMatrixChanged();
      }
      else {
        double f = 1.1;

        modelScale_.setX(modelScale_.getX()/f);
        modelScale_.setY(modelScale_.getY()/f);
        modelScale_.setZ(modelScale_.getZ()/f);

        Q_EMIT modelMatrixChanged();
      }
    }
  }
  else if (e->key() == Qt::Key_Left) {
    if      (type == CQNewGLModel::Type::LIGHT) {
      lights_[currentLight_]->pos += CGLVector3D(0.1f, 0.0f, 0.0f);

      app()->updateLights();
    }
    else if (type == CQNewGLModel::Type::MODEL) {
      if (isControl) {
        modelTranslate_.setX(modelTranslate_.getX() - sceneSize_.x()/100.0);

        Q_EMIT modelMatrixChanged();
      }
    }
  }
  else if (e->key() == Qt::Key_Right) {
    if      (type == CQNewGLModel::Type::LIGHT) {
      lights_[currentLight_]->pos -= CGLVector3D(0.1f, 0.0f, 0.0f);

      app()->updateLights();
    }
    else if (type == CQNewGLModel::Type::MODEL) {
      if (isControl) {
        modelTranslate_.setX(modelTranslate_.getX() + sceneSize_.x()/100.0);

        Q_EMIT modelMatrixChanged();
      }
    }
  }

  update();
}

//---

CQNewGLCamera::
CQNewGLCamera(CQNewGLCanvas *canvas, const CGLVector3D &v) :
 CGLCamera(v), canvas_(canvas), v_(v)
{
}

void
CQNewGLCamera::
viewChanged()
{
  canvas_->app()->updateCamera();
}

void
CQNewGLCamera::
reset()
{
  init(v_, CGLVector3D(0, 1, 0), -90, 0);
}
