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

#include <QOpenGLShaderProgram>
#include <QMouseEvent>

#define Q(x) #x
#define QUOTE(x) Q(x)

namespace {

CGLVector3D QColorToVector(const QColor &c) {
  return CGLVector3D(c.red()/255.0, c.green()/255.0, c.blue ()/255.0);
}

double max3(double x, double y, double z) {
  return std::max(std::max(x, y), z);
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

  if (isInvertDepth())
    lightPos_ = CGLVector3D(0.4, 0.4, -0.4);
  else
    lightPos_ = CGLVector3D(0.4, 0.4, 0.4);

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

  buildDir_ = QUOTE(BUILD_DIR);

  //---

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

  //---

  axisShaderProgram_ = new CQNewGLShaderProgram(this);

  axisShaderProgram_->addShaders("axis.vs", "axis.fs");

  //---

  loadInitTextures();

  //---

  // camera
  camera_ = new CQNewGLCamera(this, CGLVector3D(0.0f, 0.0f, 1.414f));

  camera_->setLastPos(float(app_->windowWidth())/2.0f, float(app_->windowHeight())/2.0f);

  //---

  font_ = new CQNewGLFont(this);

  font_->init();

  font_->setFont("OpenSans-Regular.ttf");

  text_ = new CQNewGLText("Hello World");

  text_->setFont(font_);
  text_->setPosition(CGLVector3D(1, 1, 1));
  text_->setColor(CQNewGLFont::Color(1, 0, 0));

  //---

  updateObjectData();

  //---

  updateLightBuffer();
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
  // set up vertex data (and buffer(s)) and configure vertex attributes
  auto *lightBuffer = lightShaderProgram_->getBuffer();

  auto addLightPoint = [&](float x, float y, float z) {
    lightBuffer->addPoint(x, y, z);
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

  lightBuffer->load();
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

  text_->updateText();
}

void
CQNewGLCanvas::
updateModelData()
{
  // set up vertex data (and buffer(s)) and configure vertex attributes
  auto *importBase = this->importBase();

  if (! importBase) {
    sceneScale_ = 1.0;
    return;
  }

  //---

  auto &scene = importBase->getScene();

  CBBox3D bbox;
//scene.getBBox(bbox);

  //---

  bool useBones = (isAnimEnabled() && animName() != "");

  const auto &objects = scene.getObjects();

  for (auto *object : objects) {
    ObjectData *objectData { nullptr };

    auto pd = objectDatas_.find(object);

    if (pd == objectDatas_.end())
      pd = objectDatas_.insert(pd, ObjectDatas::value_type(object, new ObjectData));

    objectData = (*pd).second;

    if (! objectData->object)
      objectData->object = object;

    objectData->modelData.buffer = modelShaderProgram_->getBuffer();

    //---

#if 1
    auto meshMatrix = getMeshGlobalTransform(objectData, /*invert*/false);
#else
    auto meshMatrix = CMatrix3D::identity();

    if (useBones)
      meshMatrix = getMeshGlobalTransform(objectData, /*invert*/true);
#endif

    //---

    auto *buffer = objectData->modelData.buffer;

    buffer->clearBuffers();

    objectData->modelData.faceDatas.clear();

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

      const auto &material = face->getMaterial();

//    faceData.ambient   = material.getAmbient ();
//    faceData.diffuse   = material.getDiffuse ();
//    faceData.specular  = material.getSpecular();
      faceData.shininess = material.getShininess();

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

        auto addPoint = [&](const CPoint3D &p) {
          auto zSign = (isInvertDepth() ? -1 : 1);

          CPoint3D p1;

          if (! isFlipYZ())
            p1 = CPoint3D(p.x, p.y, zSign*p.z);
          else
            p1 = CPoint3D(p.x, p.z, zSign*p.y);

          auto p2 = meshMatrix*p1;

          if (useBones)
            buffer->addPoint(float(p1.x), float(p1.y), float(p1.z));
          else
            buffer->addPoint(float(p2.x), float(p2.y), float(p2.z));

          bbox += p2;
        };

        addPoint(model);

        //---

#if 0
        if (! isFlipYZ())
          buffer->addNormal(float(normal.getX()), float(normal.getY()), float(normal.getZ()));
        else
          buffer->addNormal(float(normal.getX()), float(normal.getZ()), float(normal.getY()));
#else
        buffer->addNormal(float(normal.getX()), float(normal.getY()), float(normal.getZ()));
#endif

        //---

        buffer->addColor(color.getRedF(), color.getGreenF(), color.getBlueF());

        //---

        if (faceData.diffuseTexture) {
          const auto &tpoint = face->getTexturePoint(vertex, iv);

          buffer->addTexturePoint(float(tpoint.x), float(tpoint.y));
        }
        else
          buffer->addTexturePoint(0.0f, 0.0f);

        //---

        const auto &jointData = vertex.getJointData();

        if (jointData.nodeDatas[0].node >= 0) {
          buffer->addBoneIds(jointData.nodeDatas[0].node,
                             jointData.nodeDatas[1].node,
                             jointData.nodeDatas[2].node,
                             jointData.nodeDatas[3].node);

          buffer->addBoneWeights(jointData.nodeDatas[0].weight,
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

    buffer->load();
  }

  //---

  sceneSize_   = bbox.getSize();
  sceneCenter_ = bbox.getCenter();

  std::cerr << "Scene Size : " << sceneSize_.getX() << " " <<
               sceneSize_.getY() << " " << sceneSize_.getZ() << "\n";
  std::cerr << "Scene Center : " << sceneCenter_.getX() << " " <<
               sceneCenter_.getY() << " " << sceneCenter_.getZ() << "\n";

  sceneScale_ = float(1.0/max3(sceneSize_.getX(), sceneSize_.getY(), sceneSize_.getZ()));
  //std::cerr << "Scene Scale : " << sceneScale_ << "\n";

  modelTranslate_.setX(-sceneCenter_.getX());
  modelTranslate_.setY(-sceneCenter_.getY());
  modelTranslate_.setZ(-sceneCenter_.getZ());

  modelScale_.setX(sceneScale_);
  modelScale_.setY(sceneScale_);
  modelScale_.setZ(sceneScale_);
}

void
CQNewGLCanvas::
updateBonesData()
{
  // set up vertex data (and buffer(s)) and configure vertex attributes
  auto *importBase = this->importBase();

  if (! importBase) {
    sceneScale_ = 1.0;
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

  sceneScale_ = float(1.0/max3(sceneSize_.getX(), sceneSize_.getY(), sceneSize_.getZ()));
  //std::cerr << "Scene Scale : " << sceneScale_ << "\n";
}

void
CQNewGLCanvas::
updateBoneData()
{
  // set up vertex data (and buffer(s)) and configure vertex attributes
  auto *importBase = this->importBase();

  if (! importBase) {
    sceneScale_ = 1.0;
    return;
  }

  //---

  CBBox3D bbox;
//scene.getBBox(bbox);

  auto updateSceneSize = [&]() {
    sceneSize_   = bbox.getSize();
    sceneCenter_ = bbox.getCenter();
    sceneScale_  = float(1.0/max3(sceneSize_.getX(), sceneSize_.getY(), sceneSize_.getZ()));

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

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //---

  auto aspect = float(app_->windowWidth())/float(app_->windowHeight());

  if (isOrtho())
    paintData_.projection =
      CGLMatrix3D::ortho(-1, 1, -1, 1, camera_->near(), camera_->far());
  else
    paintData_.projection =
      CGLMatrix3D::perspective(camera_->zoom(), aspect, camera_->near(), camera_->far());

  paintData_.view = camera_->getViewMatrix();

  //---

  // lighting
  paintData_.viewPos = camera_->position();

  //---

  if (isShowAxis())
    drawAxis();

  //---

  // setup model shader
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
  }

  //---

  drawLight();

  text_->render();
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

  auto lightColor1   = QColorToVector(lightColor  ());
  auto ambientColor1 = QColorToVector(ambientColor());

  modelShaderProgram_->setUniformValue("lightColor", CQGLUtil::toVector(lightColor1));
  modelShaderProgram_->setUniformValue("lightPos"  , CQGLUtil::toVector(lightPos()));

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

    bool useEmissiveTexture = faceData.emissiveTexture;

    modelShaderProgram_->setUniformValue("useEmissiveTexture", useEmissiveTexture);

    if (useEmissiveTexture) {
      glActiveTexture(GL_TEXTURE3);
      faceData.emissiveTexture->bind();

      modelShaderProgram_->setUniformValue("emissiveTexture", 3);
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

  if (useBones)
    rootObject->updateNodesAnimationData(animName().toStdString(), time());

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

  auto lightColor1   = QColorToVector(lightColor  ());
  auto ambientColor1 = QColorToVector(ambientColor());

  bonesShaderProgram_->setUniformValue("lightColor", CQGLUtil::toVector(lightColor1));
  bonesShaderProgram_->setUniformValue("lightPos"  , CQGLUtil::toVector(lightPos()));

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

  auto lightColor1   = QColorToVector(lightColor  ());
  auto ambientColor1 = QColorToVector(ambientColor());

  boneShaderProgram_->setUniformValue("lightColor", CQGLUtil::toVector(lightColor1));
  boneShaderProgram_->setUniformValue("lightPos"  , CQGLUtil::toVector(lightPos()));

  boneShaderProgram_->setUniformValue("ambientColor", CQGLUtil::toVector(ambientColor1));

  // model rotation
  auto modelMatrix = getModelMatrix();
  boneShaderProgram_->setUniformValue("model", CQGLUtil::toQMatrix(modelMatrix));

  //---

  boneShaderProgram_->setUniformValue("ambientStrength" , float(app_->ambientStrength()));
  boneShaderProgram_->setUniformValue("diffuseStrength" , float(app_->diffuseStrength()));
  boneShaderProgram_->setUniformValue("specularStrength", float(app_->specularStrength()));

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
}

void
CQNewGLCanvas::
drawLight()
{
  auto *lightBuffer = lightShaderProgram_->getBuffer();

  // setup light shader
  lightBuffer->bind();

  lightShaderProgram_->bind();

  lightShaderProgram_->setUniformValue("projection", CQGLUtil::toQMatrix(projectionMatrix()));
  lightShaderProgram_->setUniformValue("view", CQGLUtil::toQMatrix(viewMatrix()));

  auto lightMatrix = CMatrix3D::translation(lightPos().x(), lightPos().y(), lightPos().z());
  lightMatrix.scaled(0.01, 0.01, 0.01);
  lightShaderProgram_->setUniformValue("model", CQGLUtil::toQMatrix(lightMatrix));

  // draw light
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  //lightBuffer->drawTriangles();

  lightBuffer->unbind();
}

CMatrix3D
CQNewGLCanvas::
getModelMatrix() const
{
  auto modelMatrix = CMatrix3D::identity();
  modelMatrix.scaled(sceneScale_, sceneScale_, sceneScale_);
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

#if 0
  addLine(CPoint3D(-sceneSize_.getX(), 0, 0), CPoint3D(sceneSize_.getX(), 0, 0));
  addLine(CPoint3D(0, -sceneSize_.getY(), 0), CPoint3D(0, sceneSize_.getY(), 0));
  addLine(CPoint3D(0, 0, -sceneSize_.getZ()), CPoint3D(0, 0, sceneSize_.getZ()));
#else
  auto d = 2.0*sceneScale/10.0;

  for (int i = -5; i <= 5; ++i) {
    addLine(CPoint3D(-sceneScale, i*d, 0), CPoint3D(sceneScale, i*d, 0), CPoint3D(0, pointSize, 0));
    addLine(CPoint3D(0, -sceneScale, i*d), CPoint3D(0, sceneScale, i*d), CPoint3D(pointSize, 0, 0));
    addLine(CPoint3D(i*d, 0, -sceneScale), CPoint3D(i*d, 0, sceneScale), CPoint3D(0, pointSize, 0));
  }
#endif

  axisBuffer->load();
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
      lightPos_ += CGLVector3D(0.0f, 0.1f, 0.0f);

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
      lightPos_ -= CGLVector3D(0.0f, 0.1f, 0.0f);

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
      lightPos_ -= CGLVector3D(0.1f, 0.0f, 0.0f);

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
      lightPos_ += CGLVector3D(0.1f, 0.0f, 0.0f);

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
      lightPos_ += CGLVector3D(0.0f, 0.0f, 0.1f);

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
      lightPos_ -= CGLVector3D(0.0f, 0.0f, 0.1f);

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
      lightPos_ += CGLVector3D(0.1f, 0.0f, 0.0f);

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
      lightPos_ -= CGLVector3D(0.1f, 0.0f, 0.0f);

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
