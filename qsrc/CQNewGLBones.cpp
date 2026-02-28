#include <CQNewGLBones.h>
#include <CQNewGLModel.h>
#include <CQNewGLModelObject.h>
#include <CQNewGLFont.h>
#include <CQNewGLCamera.h>
#include <CQNewGLAxes.h>
#include <CQNewGLUtil.h>

#include <CQGLBuffer.h>
#include <CQGLTexture.h>
#include <CGeometry3D.h>
#include <CGeomScene3D.h>
#include <CGeomNodeData.h>
#include <CQGLUtil.h>
#include <CStrUtil.h>

#include <QMouseEvent>

#include <set>

//---

CQNewGLBones::
CQNewGLBones(CQNewGLModel *app) :
 CQNewGLWidget(app)
{
}

void
CQNewGLBones::
initializeGL()
{
  initialized_ = true;

  //---

  initializeOpenGLFunctions();

  glClearColor(bgColor_.redF(), bgColor_.greenF(), bgColor_.blueF(), 1.0f);

  //---

  initFont();

  //---

#if 0
  axes_ = new CQNewGLAxes(this);
  axes_->setVisible(false);
#endif

  //---

  updateObjectsData();

  //---

  // cameras
  addCamera();
}

void
CQNewGLBones::
resizeGL(int width, int height)
{
  setPixelWidth (width);
  setPixelHeight(height);

  glViewport(0, 0, width, height);

  setAspect(double(width)/double(height));
}

void
CQNewGLBones::
paintGL()
{
  // clear canvas
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0, 0, pixelWidth(), pixelHeight());

  //---

  // set GL state
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glFrontFace(GL_CW);

  //---

  // set camera
  auto *camera = getCurrentCamera();

  camera->setAspect(aspect());

  //---

  // set view state
  paintData_.projection = camera->getPerspectiveMatrix();
  paintData_.view       = camera->getViewMatrix();
  paintData_.viewPos    = camera->position();

  //---

  // draw bone skeleton
  drawObjectsBones();

  //---

#if 0
  // draw axes
  if (axes_->isVisible())
    drawAxes();
#endif
}

//---

void
CQNewGLBones::
addCamera()
{
  CGLVector3D position, up;
  QColor      color;

  CameraData cameraData;
  calcCameraData(0, cameraData);

  //---

  camera_ = new CQNewGLCamera(this, 0, cameraData.origin, cameraData.position, cameraData.up);

  camera_->setColor(cameraData.color);

  camera_->setLastPos(float(pixelWidth())/2.0f, float(pixelHeight())/2.0f);
}

void
CQNewGLBones::
resetCamera()
{
  auto *camera = getCurrentCamera();
  if (! camera) return;

  sceneScale_ = std::max(sceneSize_.maxComponent(), 1.0);

  //---

  auto f = std::sqrt(2.0);

  CGLVector3D position, origin;

  auto f1 = f*sceneScale_;

  position = CGLVector3D(f1, f1, f1);
  origin   = CGLVector3D(sceneCenter_.x, sceneCenter_.y, sceneCenter_.z);

  camera->setOrigin(origin);
  camera->setPosition(position);
}

//---

void
CQNewGLBones::
updateObjectsData()
{
  if (! initialized_)
    return;

  CBBox3D bbox;
  updateObjectBones(bbox);

  if (bbox.isSet()) {
    sceneSize_   = bbox.getSize();
    sceneCenter_ = bbox.getCenter();
  }
  else {
    sceneSize_   = CVector3D(1, 1, 1);
    sceneCenter_ = CPoint3D(0, 0, 0);
  }

  sceneScale_    = max3(sceneSize_.getX(), sceneSize_.getY(), sceneSize_.getZ());
//invSceneScale_ = (sceneScale_ > 0.0 ? 1.0/sceneScale_ : 1.0);

  resetCamera();

#if 0
  axes_->setValid(false);
#endif
}

//---

void
CQNewGLBones::
updateObjectBones(CBBox3D &bbox)
{
  auto *scene = app_->getScene();

  // set up vertex data (and buffer(s)) and configure vertex attributes
  if (scene->getObjects().empty())
    return;

  //---

  // transform node to model coords
  auto getNodeTransform = [&](const CGeomNodeData &nodeData) {
    if      (bonesTransform() == BonesTransform::INVERSE_BIND)
      return nodeData.inverseBindMatrix().inverse();
    else if (bonesTransform() == BonesTransform::LOCAL)
      return nodeData.calcLocalTransform();
    else if (bonesTransform() == BonesTransform::GLOBAL)
      return nodeData.globalTransform();
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

  auto *font = getFont();

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
      if (! node.isValid()) continue;

      if (! node.isJoint())
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

      auto color = (node.isSelected() ? bonesSelectedColor() : bonesColor());

      addBonesCube(objectData, m, cubeSize, nodeId, color, node.isSelected(), pos, bbox);

      //---

      if (node.parent() >= 0) {
        const auto &pnode = rootObject->getNode(node.parent());

        if (pnode.isValid() && pnode.isJoint()) {
#if 1
          auto m1 = getNodeTransform(pnode);
        //auto c1 = transformNode(pnode);
          auto c1 = m1*CPoint3D(0.0, 0.0, 0.0);
#else
          auto c1 = hierTransformNode(pnode);
#endif

          addBonesLine(objectData, c, c1, lineSize, nodeId, node.parent(), lineColor, pos);
        }
      }

      //---

      auto c1 = c + CPoint3D(0.0, cubeSize/2.0, cubeSize/2.0);

      (void) createText(node.name(), c1, ts1);
    }

    for (auto *text : objectData->bonesData().texts)
      text->updateText();

    buffer->load();
  }
}

void
CQNewGLBones::
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
CQNewGLBones::
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
CQNewGLBones::
drawObjectsBones()
{
  auto rootObjects = getRootObjects();

  for (auto *rootObject : rootObjects) {
    auto *objectData = getObjectData(rootObject);

    drawObjectBones(objectData);
  }
}

void
CQNewGLBones::
drawObjectBones(CQNewGLModelObject *objectData)
{
  if (! objectData->bonesData().buffer)
    return;

  //---

  auto *object     = objectData->object();
  auto *rootObject = object->getRootObject();

  // update bone nodes
  bool useBones = (app_->isAnimEnabled() && app_->animName() != "");

  if (useBones) {
    auto t = app_->time();

    if (! rootObject->updateNodesAnimationData(app_->animName().toStdString(), t))
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
      text->render(this);
  }
}

//---

CMatrix3D
CQNewGLBones::
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

void
CQNewGLBones::
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

  auto meshMatrix        = meshNodeData.globalTransform();
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

    if (nodeData.isValid()) {
      auto transform = inverseMeshMatrix*nodeData.hierAnimMatrix()*nodeData.inverseBindMatrix();

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

#if 0
void
CQNewGLBones::
updateAxes()
{
  axes_->updateGeometry();

  axes_->setValid(true);
}

void
CQNewGLBones::
drawAxes()
{
  if (! axes_->isValid())
    updateAxes();

  axes_->drawGeometry();
}
#endif

//---

std::vector<CGeomObject3D *>
CQNewGLBones::
getRootObjects() const
{
  std::set<CGeomObject3D *>    rootObjectSet;
  std::vector<CGeomObject3D *> rootObjects;

  auto *scene = app_->getScene();

  const auto &objects = scene->getObjects();

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
CQNewGLBones::
mousePressEvent(QMouseEvent *e)
{
  mouseData_.pressed = true;
  mouseData_.button  = e->button();

  mouseData_.press.x = e->x();
  mouseData_.press.y = e->y();

  mouseData_.isShift   = (e->modifiers() & Qt::ShiftModifier);
  mouseData_.isControl = (e->modifiers() & Qt::ControlModifier);

  //---

  bool isLeftButton = (mouseData_.button == Qt::LeftButton);

  if (isLeftButton) {
    auto type = app_->type();

    if (type == CQNewGLModel::Type::CAMERA) {
      auto *camera = getCurrentCamera();

      camera->setLastPos(mouseData_.press.x, mouseData_.press.y);
    }
  }

  //---

  update();
}

void
CQNewGLBones::
mouseMoveEvent(QMouseEvent *e)
{
  mouseData_.move.x = e->x();
  mouseData_.move.y = e->y();

  mouseData_.isShift   = (e->modifiers() & Qt::ShiftModifier);
  mouseData_.isControl = (e->modifiers() & Qt::ControlModifier);

  //---

  bool isLeftButton = (mouseData_.button == Qt::LeftButton);

  if (isLeftButton) {
    auto type = app_->type();

    if (type == CQNewGLModel::Type::CAMERA) {
      auto *camera = getCurrentCamera();

      float xoffset, yoffset;
      camera->deltaPos(mouseData_.move.x, mouseData_.move.y, xoffset, yoffset);

      camera->setLastPos(mouseData_.move.x, mouseData_.move.y);

      camera->processMouseMovement(xoffset, yoffset, /*constrainPitch*/true);
      updateCameraBuffer();
    }
  }

  //---

  mouseData_.press = mouseData_.move;

  update();
}

void
CQNewGLBones::
mouseReleaseEvent(QMouseEvent *)
{
  mouseData_.pressed = false;
  mouseData_.button  = Qt::NoButton;

  update();
}

void
CQNewGLBones::
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
CQNewGLBones::
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
CQNewGLBones::
updateCameraBuffer()
{
  auto *camera = getCurrentCamera();

  camera->addGeometry();
}
