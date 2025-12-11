#include <CQNewGLWidget.h>
#include <CQNewGLModel.h>
#include <CQNewGLModelObject.h>
#include <CQNewGLFont.h>
#include <CQNewGLShaderProgram.h>
#include <CQNewGLUtil.h>

#include <CGeomObject3D.h>
#include <CQGLTexture.h>
#include <CQGLUtil.h>
#include <CGeomTexture.h>

CQNewGLWidget::
CQNewGLWidget(CQNewGLModel *app) :
 QGLWidget(), app_(app)
{
  setFocusPolicy(Qt::StrongFocus);

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

CQNewGLWidget::
~CQNewGLWidget()
{
}

//---

void
CQNewGLWidget::
setBgColor(const QColor &c)
{
  bgColor_ = c;

  glClearColor(bgColor_.redF(), bgColor_.greenF(), bgColor_.blueF(), 1.0f);
}

//---

CQNewGLShaderProgram *
CQNewGLWidget::
getShader(const QString &vertex, const QString &fragment)
{
  auto id = QString("V:%1,F:%2").arg(vertex).arg(fragment);

  auto ps = shaders_.find(id);

  if (ps == shaders_.end()) {
    auto *shaderProgram = new CQNewGLShaderProgram(app_);

    shaderProgram->addShaders(vertex, fragment);

    ps = shaders_.insert(ps, Shaders::value_type(id, shaderProgram));
  }

  return (*ps).second;
}

//---

CQNewGLModelObject *
CQNewGLWidget::
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

CQNewGLModelObject *
CQNewGLWidget::
getObjectData(int ind) const
{
  for (auto &po : objectDatas_) {
    auto *objectData = po.second;

    if (objectData->ind() == ind)
      return objectData;
  }

  return nullptr;
}

CGeomObject3D *
CQNewGLWidget::
getSelectedObject() const
{
  for (auto &po : objectDatas_) {
    auto *objectData = po.second;

    if (objectData->object()->getSelected())
      return objectData->object();
  }

  return nullptr;
}

//---

void
CQNewGLWidget::
initFont()
{
  if (font_)
    return;

  font_ = new CQNewGLFont(this);

  font_->setSize(48);
  font_->setFontName("OpenSans-Regular.ttf");
}

bool
CQNewGLWidget::
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

//---

void
CQNewGLWidget::
calcCameraData(int ic, CameraData &cameraData) const
{
  auto sceneCenter = this->sceneCenter();
  auto sceneScale  = this->sceneScale();

  auto *object = this->getCurrentObject();

  if (object) {
    auto bbox = app_->getObjectBBox(object);

    sceneCenter = bbox.getCenter();
    sceneScale  = bbox.getMaxSize();
  }

  //---

  auto f  = std::sqrt(2.0);
  auto f1 = f*sceneScale;

  cameraData.origin = CGLVector3D(sceneCenter.x, sceneCenter.y, sceneCenter.z);

  auto ic1 = ic % 4;

  if      (ic1 == 0) {
    cameraData.position = CGLVector3D(f1, f1, f1);
    cameraData.up       = CGLVector3D(0, 1, 0);
    cameraData.color    = QColor(255, 255, 255);
  }
  else if (ic1 == 1) {
    cameraData.position = CGLVector3D(f1, sceneCenter.y, sceneCenter.z);
    cameraData.up       = CGLVector3D(0, 1, 0);
    cameraData.color    = QColor(255, 0, 0);
  }
  else if (ic1 == 2) {
    cameraData.position = CGLVector3D(sceneCenter.x, f1, sceneCenter.z);
    cameraData.up       = CGLVector3D(0, 0, 1);
    cameraData.color    = QColor(0, 255, 0);
  }
  else if (ic1 == 3) {
    cameraData.position = CGLVector3D(sceneCenter.x, sceneCenter.y, f1);
    cameraData.up       = CGLVector3D(0, 1, 0);
    cameraData.color    = QColor(0, 0, 255);
  }
}

void
CQNewGLWidget::
addShaderMVP(CQNewGLShaderProgram *program, const CMatrix3D &modelMatrix)
{
  (void) checkError("addShaderMVP");

  // pass projection matrix to shader (note that in this case it could change every frame)
  program->setUniformValue("projection", CQGLUtil::toQMatrix(this->projectionMatrix()));

  // camera/view transformation
  program->setUniformValue("view", CQGLUtil::toQMatrix(this->viewMatrix()));

  // model matrix
  program->setUniformValue("model", CQGLUtil::toQMatrix(modelMatrix));

  (void) checkError("addShaderMVP1");
}

//---

CQGLTexture *
CQNewGLWidget::
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
CQNewGLWidget::
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
CQNewGLWidget::
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
CQNewGLWidget::
makeTexture(const CImagePtr &image) const
{
  auto *texture = new CQGLTexture(image);

  texture->setFunctions(const_cast<CQNewGLWidget *>(this));

  return texture;
}

//---

void
CQNewGLWidget::
enableBlend()
{
  assert(! blend_);

  glDepthFunc(GL_LEQUAL);
  (void) checkError("glDepthFunc");

  glEnable(GL_BLEND);
  (void) checkError("glEnable GL_BLEND");
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  (void) checkError("glBlendFunc GL_SRC_ALPHA, GL_ONE");
//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//glBlendFunc(GL_ONE, GL_ONE);

  blend_ = true;
}

void
CQNewGLWidget::
disableBlend()
{
  assert(blend_);

  glDisable(GL_BLEND);
  (void) checkError("glDisable GL_BLEND");

  blend_ = false;
}
