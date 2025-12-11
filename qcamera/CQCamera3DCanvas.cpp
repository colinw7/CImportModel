#include <CQCamera3DCanvas.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DShaderProgram.h>
#include <CQCamera3DFaceData.h>
#include <CQCamera3DCamera.h>
#include <CQCamera3DShape.h>
#include <CQCamera3DAnnotation.h>
#include <CQCamera3DOverlay.h>
#include <CQCamera3DOverlay2D.h>
#include <CQCamera3DControl.h>
#include <CQCamera3DStatus.h>
#include <CQCamera3DAxes.h>
#include <CQCamera3DFont.h>
#include <CQCamera3DGeomObject.h>

#include <CQGLBuffer.h>
#include <CQGLTexture.h>
#include <CQGLUtil.h>
#include <CGeomScene3D.h>
#include <CGeomObject3D.h>
#include <CGeomTexture.h>

#include <QMouseEvent>
#include <QWheelEvent>

namespace {

double sign(double r) {
  if (r == 0.0) return 0.0;
  return (r > 0.0 ? 1.0 : -1.0);
}

}

CQCamera3DCanvas::
CQCamera3DCanvas(CQCamera3DApp *app) :
 app_(app)
{
  setFocusPolicy(Qt::StrongFocus);

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  for (int i = 0; i < 2; ++i) {
    auto *camera = new CQCamera3DCamera;

    connect(camera, SIGNAL(stateChanged()), this, SLOT(cameraChanged()));

    cameras_.push_back(camera);
  }

  setCameraInd(0);

  shape_ = new CQCamera3DShape(this);

#if 0
  annotation_ = new CQCamera3DAnnotation(this);
#endif

  overlay_   = new CQCamera3DOverlay  (this);
  overlay2D_ = new CQCamera3DOverlay2D(this);
}

void
CQCamera3DCanvas::
initializeGL()
{
  initializeOpenGLFunctions();

  glClearColor(bgColor_.redF(), bgColor_.greenF(), bgColor_.blueF(), 1.0f);

  //---

  font_ = new CQCamera3DFont(this);

  font_->init();

  font_->setSize(48);
  font_->setFontName("OpenSans-Regular.ttf");

  //---

  axes_ = new CQCamera3DAxes(this);

  //---

  initShader();

  addObjectsData();

  //---

  updateShapes();

  updateAnnotation();

  //---

  updateOverlay2D();

  //---

  updateOverlay();

  //---

  initCamera();
}

void
CQCamera3DCanvas::
paintGL()
{
  // clear canvas
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0, 0, pixelWidth(), pixelHeight());

  //---

  if (isDepthTest())
    glEnable(GL_DEPTH_TEST);
  else
    glDisable(GL_DEPTH_TEST);

  if (isCullFace())
    glEnable(GL_CULL_FACE);
  else
    glDisable(GL_CULL_FACE);

  glFrontFace(isFrontFace() ? GL_CW : GL_CCW);

  if (isPolygonLine())
    glEnable(GL_POLYGON_OFFSET_LINE);
  else
    glDisable(GL_POLYGON_OFFSET_LINE);

  glPolygonOffset(-1.0f, -1.0f);

  //---

  if (isShowAxes()) {
    axes_->updateGeometry();

    axes_->drawGeometry();
  }

  drawObjectsData();

  shape_->drawGeometry();

#if 0
  annotation_->drawGeometry();
#endif

  //---

  glDisable(GL_DEPTH_TEST);

  overlay2D_->drawGeometry();
  overlay_  ->drawGeometry();
}

void
CQCamera3DCanvas::
resizeGL(int width, int height)
{
  pixelWidth_  = width;
  pixelHeight_ = height;

  glViewport(0, 0, width, height);
}

void
CQCamera3DCanvas::
setCameraInd(int i)
{
  cameraInd_ = i;

  for (int i = 0; i < 2; ++i)
    cameras_[i]->setVisible(i == cameraInd_);
}

bool
CQCamera3DCanvas::
isShowAxes() const
{
  return (axes_ && axes_->isVisible());
}

void
CQCamera3DCanvas::
setShowAxes(bool b)
{
  if (axes_)
    axes_->setVisible(b);
}

void
CQCamera3DCanvas::
initShader()
{
  shaderProgram_ = new CQCamera3DShaderProgram(this);
  shaderProgram_->addShaders("model.vs", "model.fs");
}

void
CQCamera3DCanvas::
addObjectsData()
{
  bbox_ = CBBox3D();

  int io = 0;

  auto *scene = app_->getScene();

  for (auto *object : scene->getObjects()) {
    auto *objectData = getObjectData(io, object);

    //---

    auto *buffer = objectData->initBuffer();

    //---

    auto *diffuseTexture  = object->getDiffuseTexture();
//  auto *specularTexture = object->getSpecularTexture();
    auto *normalTexture   = object->getNormalTexture();
//  auto *emissiveTexture = object->getEmissiveTexture();

    //---

    CBBox3D bbox1;

    const auto &faces = object->getFaces();

    int pos = 0;

    for (const auto *face : faces) {
      CQCamera3DFaceData faceData;

      //---

      auto color = face->color().value_or(CRGBA(1, 1, 1));

      //---

      // set face textures
      auto *diffuseTexture1 = face->getDiffuseTexture();
      if (! diffuseTexture1) diffuseTexture1 = diffuseTexture;

//    auto *specularTexture1 = face->getSpecularTexture();
//    if (! specularTexture1) specularTexture1 = specularTexture;

      auto *normalTexture1 = face->getNormalTexture();
      if (! normalTexture1) normalTexture1 = normalTexture;

//    auto *emissiveTexture1 = face->getEmissiveTexture();
//    if (! emissiveTexture1) emissiveTexture1 = emissiveTexture;

      if (diffuseTexture1)
        faceData.diffuseTexture = getTexture(diffuseTexture1, /*add*/true);

//    if (specularTexture1)
//      faceData.specularTexture = getTexture(specularTexture1, /*add*/true);

      if (normalTexture1)
        faceData.normalTexture = getTexture(normalTexture1, /*add*/true);

//    if (emissiveTexture1)
//      faceData.emissiveTexture = getTexture(emissiveTexture1, /*add*/true);

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

      faceData.selected = face->getSelected();
      faceData.visible  = face->getVisible();

      int iv = 0;

      for (const auto &v : vertices) {
        const auto &vertex = object->getVertex(v);
        const auto &model  = vertex.getModel();

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

        buffer->addPoint(float(model.x), float(model.y), float(model.z));

        buffer->addNormal(float(normal1.getX()), float(normal1.getY()), float(normal1.getZ()));

        buffer->addColor(color1);

        //---

        if (faceData.diffuseTexture) {
          const auto &tpoint = face->getTexturePoint(vertex, iv);

          buffer->addTexturePoint(float(tpoint.x), float(tpoint.y));
        }
        else
          buffer->addTexturePoint(0.0f, 0.0f);

        //---

        ++iv;

        bbox1 += model;
      }

      pos += faceData.len;

      objectData->addFaceData(faceData);
    }

    //---

    objectData->setBBox(bbox1);

    bbox_ += bbox1;

    //---

    buffer->load();

    ++io;
  }

  if (! bbox_.isSet()) {
    bbox_.add(CPoint3D(-1, -1, -1));
    bbox_.add(CPoint3D( 1,  1,  1));
  }

  updateStatus();
}

void
CQCamera3DCanvas::
initCamera()
{
  auto center  = bbox_.getCenter();
  auto maxSize = bbox_.getMaxSize();

  auto s2 = std::sqrt(2.0);

  auto origin = CVector3D(center.x, center.y, center.z);
  auto pos    = CVector3D(center.x, center.y, center.z + s2*maxSize/2.0);

  auto v = CVector3D(origin, pos);
  auto a = std::atan2(v.z(), v.x());

  for (auto *camera : cameras_) {
    //camera->setOrigin(origin);
    camera->setPosition(pos);

    camera->setYaw(a);
  }

  updateStatus();
}

void
CQCamera3DCanvas::
updateStatus()
{
  auto center  = bbox_.getCenter();
  auto size    = bbox_.getSize();

  auto pointStr = [](const CPoint3D &p) {
    return QString("(%1 %2 %3)").arg(p.x).arg(p.y).arg(p.z);
  };

  auto vectorStr = [](const CVector3D &p) {
    return QString("(%1 %2 %3)").arg(p.x()).arg(p.y()).arg(p.z());
  };

  auto bboxStr = QString("Center: %1, Size: %2").arg(pointStr(center)).arg(vectorStr(size));

  app_->status()->setModelLabel(bboxStr);
}

void
CQCamera3DCanvas::
drawObjectsData()
{
  auto *program = shaderProgram_;

  program->bind();

  // camera projection
  auto *camera = currentCamera();

  auto projectionMatrix = camera->perspectiveMatrix();
  program->setUniformValue("projection", CQGLUtil::toQMatrix(projectionMatrix));

  // camera/view transformation
  auto viewMatrix = camera->viewMatrix();
  program->setUniformValue("view", CQGLUtil::toQMatrix(viewMatrix));

  // view pos
  program->setUniformValue("viewPos", CQGLUtil::toVector(camera->position()));

  //---

  program->setUniformValue("ambient", CQGLUtil::toVector(ambientColor()));

  //---

  int io = 0;

  auto *scene = app_->getScene();

  for (auto *object : scene->getObjects()) {
    if (! object->getVisible()) {
      ++io;
      continue;
    }

    auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);
    assert(object1);

    // model matrix
    //auto modelMatrix = CMatrix3DH::identity();
    auto modelMatrix = object1->calcTransform();
    program->setUniformValue("model", CQGLUtil::toQMatrix(modelMatrix));

    //---

    bool objectSelected = object->getHierSelected();

    auto *objectData = getObjectData(io, object);

    objectData->buffer()->bind();

    for (const auto &faceData : objectData->faceDatas()) {
      if (! faceData.visible)
        continue;

      bool faceSelected = faceData.selected;

      bool selected = objectSelected || faceSelected;

      program->setUniformValue("isSelected", selected);

      //---

      bool useDiffuseTexture = faceData.diffuseTexture;

      program->setUniformValue("diffuseTexture.enabled", useDiffuseTexture);

      if (useDiffuseTexture) {
        glActiveTexture(GL_TEXTURE0);
        faceData.diffuseTexture->bind();

        program->setUniformValue("diffuseTexture.texture", 0);
      }

      //---

#if 0
      bool useSpecularTexture = faceData.specularTexture;

      program->setUniformValue("specularTexture.enabled", useSpecularTexture);

      if (useSpecularTexture) {
        glActiveTexture(GL_TEXTURE1);
        faceData.specularTexture->bind();

        program->setUniformValue("specularTexture.texture", 1);
      }
#endif

      //---

      bool useNormalTexture = faceData.normalTexture;

      program->setUniformValue("normalTexture.enabled", useNormalTexture);

      if (useNormalTexture) {
        glActiveTexture(GL_TEXTURE2);
        faceData.normalTexture->bind();

        program->setUniformValue("normalTexture.texture", 2);
      }

      //---

#if 0
      bool useEmissiveTexture = faceData.emissiveTexture;

      program->setUniformValue("emissiveTexture.enabled", useEmissiveTexture);

      if (useEmissiveTexture) {
        glActiveTexture(GL_TEXTURE3);
        faceData.emissiveTexture->bind();

        program->setUniformValue("emissiveTexture.texture", 3);
      }
#endif

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
    }

    objectData->buffer()->unbind();

    ++io;
  }

  //---

  program->release();
}

CQCamera3DObjectData *
CQCamera3DCanvas::
getObjectData(int io, CGeomObject3D *object)
{
  while (int(objectDatas_.size()) <= io)
    objectDatas_.push_back(new CQCamera3DObjectData(this));

  auto *objectData = objectDatas_[io];

  objectData->setObject(object);

  return objectData;
}

bool
CQCamera3DCanvas::
selectObject(CGeomObject3D *object, bool update)
{
  auto changed = deselectAll(update);

  if (! object->getSelected()) {
    object->setSelected(true);
    changed = true;
  }

  setCurrentObject(dynamic_cast<CQCamera3DGeomObject *>(object));
  setCurrentFace  (nullptr);

  if (changed && update) {
    this->update();

    Q_EMIT stateChanged();
  }

  return changed;
}

bool
CQCamera3DCanvas::
selectFace(CGeomFace3D *face, bool update)
{
  auto changed = deselectAll(update);

  if (! face->getSelected()) {
    face->setSelected(true);
    changed = true;
  }

  setCurrentObject(nullptr);
  setCurrentFace  (face);

  if (changed && update) {
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
    }
  }

  if (changed && update) {
    this->update();

    Q_EMIT stateChanged();
  }

  return changed;
}

//---

void
CQCamera3DCanvas::
mousePressEvent(QMouseEvent *e)
{
  auto *camera = currentCamera();

  bool isShift   = (e->modifiers() & Qt::ShiftModifier);
  bool isControl = (e->modifiers() & Qt::ControlModifier);

  mouseData_.press.x = e->x();
  mouseData_.press.y = e->y();
  mouseData_.button  = e->button();

  if      (mouseData_.button == Qt::LeftButton) {
    // calc eye line
    EyeLine eyeLine;
    calcEyeLine(mouseData_.press, eyeLine);

    if (isShift) {
      setIntersectPoints(eyeLine.pv1, eyeLine.pv2);

      updateAnnotation();
      update();

      return;
    }

    auto posStr = QString("%1 %2 %3").arg(eyeLine.pv1.x).arg(eyeLine.pv1.y).arg(eyeLine.pv1.z);
    app_->status()->setMouseLabel(posStr);

    //---

    auto viewMatrix       = camera->viewMatrix();
    auto projectionMatrix = camera->perspectiveMatrix();

    struct InsideFace {
      int                   ind { 0 };
      CGeomObject3D*        object { nullptr };
      CGeomFace3D*          face   { nullptr };
      int                   vertex { 0 };
      std::vector<CPoint3D> points;
      CPoint3D              ipoint;
      double                t { 0.0 };
    };

    std::vector<InsideFace> insideFaces;
    InsideFace              insideFace;

    CLine3D line(eyeLine.px1, eyeLine.px2);

    CBBox3D mbbox, vbbox, pbbox;
    CBBox3D pxbbox;

    // intersect eye line with object faces
    auto *scene = app_->getScene();

    for (auto *object : scene->getObjects()) {
      auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);
      assert(object1);

      //auto *objectData = getObjectData(io, object);

      auto modelMatrix = CMatrix3DH(object1->calcTransform());

      const auto &faces = object->getFaces();

      for (auto *face : faces) {
        const auto &vertices = face->getVertices();

        std::vector<CPoint3D> points;

        for (const auto &v : vertices) {
          const auto &vertex = object->getVertex(v);
          const auto &model  = vertex.getModel();

          auto view    = viewMatrix*modelMatrix*model;
          auto project = projectionMatrix*view;

          //auto pixel = CPoint3D(project.x/project.z, project.y/project.z, project.z);
          //points.push_back(pixel);
          points.push_back(project);

          mbbox .add(model);
          vbbox .add(view);
          pbbox .add(project);
        //pxbbox.add(pixel);
        }

        int np = int(points.size());

        if      (selectType() == SelectType::OBJECT || selectType() == SelectType::FACE) {
          if (np < 3) continue;

          int i2 = 1;
          int i3 = 2;

          for (int i = 0; i < np - 2; ++i) {
            CTriangle3D triangle(points[0], points[i2], points[i3]);

            double tmin, tmax;
            if (triangle.intersect(line, &tmin, &tmax)) {
              InsideFace insideFace1;

              insideFace1.object = object;
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
        else if (selectType() == SelectType::EDGE) {
        }
        else if (selectType() == SelectType::POINT) {
          for (int i = 0; i < np; ++i) {
            auto d = points[i].distanceTo(eyeLine.px1);

            if (! insideFace.object || d < insideFace.t) {
              insideFace.object = object;
              insideFace.face   = face;
              insideFace.vertex = i;
              insideFace.t      = d;
            }
          }
        }
      }
    }

#if 0
    std::cerr << "Model BBox: " << mbbox << "\n";
    std::cerr << "View BBox: " << vbbox << "\n";
    std::cerr << "Project BBox: " << pbbox << "\n";
    std::cerr << "Pixel BBox: " << pxbbox << "\n";
#endif

    // sort inside faces by z
    std::map<double, int> tfaces;

    if (! insideFaces.empty()) {
      // sort by t
      for (const auto &insideFace : insideFaces)
        tfaces[-insideFace.t] = insideFace.ind;
    }

    bool changed = false;

    if      (selectType() == SelectType::OBJECT) {
      if (! tfaces.empty()) {
        const auto &insideFace = insideFaces[tfaces.begin()->second];

        changed = selectObject(insideFace.object, /*update*/false);
      }
    }
    else if (selectType() == SelectType::FACE) {
      if (! tfaces.empty()) {
        const auto &insideFace = insideFaces[tfaces.begin()->second];

        changed = selectFace(insideFace.face, /*update*/false);

        setIntersectPoints(insideFace.ipoint, insideFace.ipoint);
      }
    }
    else if (selectType() == SelectType::EDGE) {
    }
    else if (selectType() == SelectType::POINT) {
      if (insideFace.object) {
        for (auto *vertex : insideFace.object->getVertices()) {
          bool selected = (vertex->getInd() == uint(insideFace.vertex));

          if (vertex->getTag() != selected) {
            vertex->setTag(selected);
            changed = true;
          }
        }
      }
    }

    if (changed) {
      addObjectsData();
      updateAnnotation();

      update();

      Q_EMIT stateChanged();
    }
  }
  else if (mouseData_.button == Qt::RightButton) {
    // calc eye line
    EyeLine eyeLine;
    calcEyeLine(mouseData_.press, eyeLine);

    // set camera position/origin
    if      (isShift)
      camera->setPosition(CVector3D(eyeLine.pv1));
    else if (isControl)
      camera->setOrigin(CVector3D(eyeLine.pv1));

    // draw eye line
    eyeLine_ = eyeLine;

    updateAnnotation();
    update();

    Q_EMIT stateChanged();
  }
}

void
CQCamera3DCanvas::
mouseMoveEvent(QMouseEvent *e)
{
  auto *camera = currentCamera();

  mouseData_.move.x = e->x();
  mouseData_.move.y = e->y();

  bool isShift   = (e->modifiers() & Qt::ShiftModifier);
  bool isControl = (e->modifiers() & Qt::ControlModifier);

  auto dx = sign(mouseData_.move.x - mouseData_.press.x);
  auto dy = sign(mouseData_.move.y - mouseData_.press.y);

  auto bbox = app_->canvas()->bbox();
  auto size = bbox.getSize();

  if      (mouseData_.button == Qt::MiddleButton) {
    if      (isShift) {
      camera->movePosition(0, dx*size.getX()/100.0);
      camera->movePosition(1, dy*size.getY()/100.0);
    }
    else if (isControl) {
      camera->rotatePosition(2, dx*0.05);
    }
    else {
      //camera->rotatePosition(1, dx*0.05);
      camera->moveAroundY(-dx*size.getX()/100.0);

      //camera->rotatePosition(0, dy*0.05);
      camera->moveAroundX(dy*size.getY()/100.0);
    }
  }
  else if (mouseData_.button == Qt::RightButton) {
    // calc eye line
    EyeLine eyeLine;
    calcEyeLine(mouseData_.press, eyeLine);

    auto posStr = QString("%1 %2 %3").arg(eyeLine.pv1.x).arg(eyeLine.pv1.y).arg(eyeLine.pv1.z);
    app_->status()->setMouseLabel(posStr);

    if (isShift) {
      camera->moveOrigin(0, dx*size.getX()/100.0);
      camera->moveOrigin(1, dy*size.getY()/100.0);
    }
  }

  mouseData_.press.x = e->x();
  mouseData_.press.y = e->y();

  update();
}

void
CQCamera3DCanvas::
mouseReleaseEvent(QMouseEvent *)
{
}

void
CQCamera3DCanvas::
wheelEvent(QWheelEvent *e)
{
  auto d = bbox_.getMaxSize()/100.0;

  auto dw = e->angleDelta().y()/250.0;

  auto *camera = currentCamera();

  camera->zoom(dw*d);

  update();
}

void
CQCamera3DCanvas::
keyPressEvent(QKeyEvent *e)
{
  if      (mouseType_ == MouseType::CAMERA)
    cameraKeyPress(e);
  else if (mouseType_ == MouseType::OBJECT)
    objectKeyPress(e);
}

void
CQCamera3DCanvas::
cameraKeyPress(QKeyEvent *e)
{
  auto *camera = currentCamera();

  auto k = e->key();

  bool isShift   = (e->modifiers() & Qt::ShiftModifier);
  bool isControl = (e->modifiers() & Qt::ControlModifier);

  auto d = bbox_.getMaxSize()/100.0;

  if      (k == Qt::Key_A) {
    if (isShift)
      camera->setRoll(camera->roll() - 0.1);
    else
      camera->moveRight(-d);
  }
  else if (k == Qt::Key_D) {
    if (isShift)
      camera->setRoll(camera->roll() + 0.1);
    else
      camera->moveRight(d);
  }
  else if (k == Qt::Key_S) {
    if (isShift)
      camera->setPitch(camera->pitch() - 0.1);
    else
      camera->moveAroundX(-3*d);
  }
  else if (k == Qt::Key_W) {
    if (isShift)
      camera->setPitch(camera->pitch() + 0.1);
    else
      camera->moveAroundX(3*d);
  }
  else if (k == Qt::Key_Q) {
    if      (isShift)
      camera->setYaw(camera->yaw() + 0.1);
    else if (isControl)
      camera->moveAroundZ(3*d);
    else
      camera->moveAroundY(10*d);
  }
  else if (k == Qt::Key_E) {
    if      (isShift)
      camera->setYaw(camera->yaw() - 0.1);
    else if (isControl)
      camera->moveAroundZ(-3*d);
    else
      camera->moveAroundY(-10*d);
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
    std::cerr << "Key                   Shift       Control        \n";
    std::cerr << "---  ---------------  ----------  ---------------\n";
    std::cerr << "A    Move Left        Roll -                     \n";
    std::cerr << "D    Move Right       Roll +                     \n";
    std::cerr << "W    Move Front       Pitch +     Move Around X +\n";
    std::cerr << "S    Move Back        Pitch -     Move Around X -\n";
    std::cerr << "Q    Move Around Y +  Yaw +       Move Around Z +\n";
    std::cerr << "E    Move Around Y -  Yaw -       Move Around Z -\n";
    std::cerr << "Up   Move Up                                     \n";
    std::cerr << "Down Move Down                                   \n";
  }

  update();

  Q_EMIT stateChanged();
}

void
CQCamera3DCanvas::
objectKeyPress(QKeyEvent *e)
{
  auto *object = currentObject();
  if (! object) return;

  auto *camera = currentCamera();

  auto k = e->key();

  bool isShift   = (e->modifiers() & Qt::ShiftModifier);
  bool isControl = (e->modifiers() & Qt::ControlModifier);

  auto d = bbox_.getMaxSize()/100.0;

//CBBox3D obbox;
//object->getModelBBox(obbox, /*hier*/false);

  auto scale  = 1.1;
  auto iscale = 1.0/scale;

  if      (k == Qt::Key_A) {
    auto right = camera->right();

    if      (isShift) {
      auto m = CMatrix3D::scale(iscale, 1.0, 1.0);
      object->setTransform(m*object->getTransform());
    }
    else if (isControl) {
      auto m = CMatrix3D::rotation(-0.1, right);
      object->setTransform(m*object->getTransform());
    }
    else {
      auto t = -d*right;

      auto m = CMatrix3D::translation(t.getX(), t.getY(), t.getZ());
      object->setTransform(m*object->getTransform());
    }
  }
  else if (k == Qt::Key_D) {
    auto right = camera->right();

    if      (isShift) {
      auto m = CMatrix3D::scale(scale, 1.0, 1.0);
      object->setTransform(m*object->getTransform());
    }
    else if (isControl) {
      auto m = CMatrix3D::rotation(0.1, right);
      object->setTransform(m*object->getTransform());
    }
    else {
      auto t = d*right;

      auto m = CMatrix3D::translation(t.getX(), t.getY(), t.getZ());
      object->setTransform(m*object->getTransform());
    }
  }
  else if (k == Qt::Key_W) {
    auto up = camera->up();

    if      (isShift) {
      auto m = CMatrix3D::scale(1.0, scale, 1.0);
      object->setTransform(m*object->getTransform());
    }
    else if (isControl) {
      auto m = CMatrix3D::rotation(0.1, up);
      object->setTransform(m*object->getTransform());
    }
    else {
      auto t = d*up;

      auto m = CMatrix3D::translation(t.getX(), t.getY(), t.getZ());
      object->setTransform(m*object->getTransform());
    }
  }
  else if (k == Qt::Key_S) {
    auto up = camera->up();

    if      (isShift) {
      auto m = CMatrix3D::scale(1.0, iscale, 1.0);
      object->setTransform(m*object->getTransform());
    }
    else if (isControl) {
      auto m = CMatrix3D::rotation(-0.1, up);
      object->setTransform(m*object->getTransform());
    }
    else {
      auto t = -d*up;

      auto m = CMatrix3D::translation(t.getX(), t.getY(), t.getZ());
      object->setTransform(m*object->getTransform());
    }
  }
  else if (k == Qt::Key_Up) {
    auto front = camera->front();

    if      (isShift) {
      auto m = CMatrix3D::scale(1.0, 1.0, scale);
      object->setTransform(m*object->getTransform());
    }
    else if (isControl) {
      auto m = CMatrix3D::rotation(0.1, front);
      object->setTransform(m*object->getTransform());
    }
    else {
      auto t = d*front;

      auto m = CMatrix3D::translation(t.getX(), t.getY(), t.getZ());
      object->setTransform(m*object->getTransform());
    }
  }
  else if (k == Qt::Key_Down) {
    auto front = camera->front();

    if      (isShift) {
      auto m = CMatrix3D::scale(1.0, 1.0, iscale);
      object->setTransform(m*object->getTransform());
    }
    else if (isControl) {
      auto m = CMatrix3D::rotation(-0.1, front);
      object->setTransform(m*object->getTransform());
    }
    else {
      auto t = -d*front;

      auto m = CMatrix3D::translation(t.getX(), t.getY(), t.getZ());
      object->setTransform(m*object->getTransform());
    }
  }
  else if (k == Qt::Key_Question) {
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
calcEyeLine(const CPoint3D &pos, EyeLine &eyeLine, bool verbose) const
{
  auto *camera = currentCamera();

  auto projectionMatrix  = camera->perspectiveMatrix();
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
  auto aspect = camera->aspect();

  double x1, y1;

  if (aspect > 1.0) {
    x1 = CMathUtil::map(pos.x, 0, pixelWidth_  - 1.0, -aspect,  aspect);
    y1 = CMathUtil::map(pos.y, 0, pixelHeight_ - 1.0,     1.0,    -1.0);
  }
  else {
    x1 = CMathUtil::map(pos.x, 0, pixelWidth_  - 1.0,   -1.0,      1.0);
    y1 = CMathUtil::map(pos.y, 0, pixelHeight_ - 1.0,  aspect, -aspect);
  }

  //std::cerr << "PX: " << x1 << " " << y1 << "\n";

  //---

  auto pointToView = [&](const CPoint3D &p, CPoint3D &pp, CPoint3D &pv) {
    double xp, yp, zp;
    iProjectionMatrix.multiplyPoint(p.x, p.y, p.z, &xp, &yp, &zp);

    pp = CPoint3D(xp, yp, zp);

    zp = p.z;

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
  app_->control()->updateWidgets();

  if (isShowPlanes())
    updateAnnotation();

  update();
}

CQCamera3DCamera *
CQCamera3DCanvas::
currentCamera() const
{
  if (cameraInd_ >= 0 && cameraInd_ < int(cameras_.size()))
    return cameras_[cameraInd_];
  else
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
#if 0
  annotation_->updateGeometry();
#endif
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
  overlay2D_->updateGeometry();
}

void
CQCamera3DCanvas::
setCurrentObject(CQCamera3DGeomObject *object)
{
  currentObject_ = object;

  app_->control()->updateWidgets();
}

void
CQCamera3DCanvas::
setCurrentFace(CGeomFace3D *face)
{
  currentFace_ = face;

  app_->control()->updateWidgets();
}

bool
CQCamera3DCanvas::
createFontTexture(uint *texture, int w, int h, uchar *data)
{
  //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // allocate texture id
  glGenTextures(1, texture);
  //if (! checkError("glGenTextures")) return false;

  // set texture type
  glBindTexture(GL_TEXTURE_2D, *texture);
  //if (! checkError("glGenTexture")) return false;

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

//---

CQGLTexture *
CQCamera3DCanvas::
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
CQCamera3DCanvas::
getGeomTextureByName(const std::string &name) const
{
  for (const auto &pt : glTextures_) {
    const auto &textureData = pt.second;

    if (textureData.glTexture->getName() == name)
      return textureData.geomTexture;
  }

  return nullptr;
}

CQGLTexture *
CQCamera3DCanvas::
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
CQCamera3DCanvas::
makeTexture(const CImagePtr &image) const
{
  auto *texture = new CQGLTexture(image);

  texture->setFunctions(const_cast<CQCamera3DCanvas *>(this));

  return texture;
}

//---

CQCamera3DObjectData::
CQCamera3DObjectData(CQCamera3DCanvas *canvas) :
 canvas_(canvas)
{
}

CQGLBuffer *
CQCamera3DObjectData::
initBuffer()
{
  assert(canvas_->shaderProgram());

  if (! buffer_)
    buffer_ = canvas_->shaderProgram()->createBuffer();

  buffer_->clearBuffers();

  faceDatas_.clear();

  return buffer_;
}

void
CQCamera3DObjectData::
addFaceData(const CQCamera3DFaceData &faceData)
{
  faceDatas_.push_back(faceData);
}
