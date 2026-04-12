#include <CQCamera3DWidget.h>
#include <CQCamera3DTexture.h>
#include <CQCamera3DApp.h>

#include <CQGLTexture.h>
#include <CQGLState.h>

CQCamera3DWidget::
CQCamera3DWidget(CQCamera3DApp *app) :
 app_(app)
{
  static uint s_lastInd;

  ind_ = ++s_lastInd;

  //---

  setFocusPolicy(Qt::StrongFocus);

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  //---

  if (app->isMultisample()) {
    QGLFormat sformat;
    sformat.setSamples(16);
    sformat.setDepthBufferSize(24);
    setFormat(sformat);
  }
}

CQCamera3DWidget::
~CQCamera3DWidget()
{
}

//---

void
CQCamera3DWidget::
enableDepthTest()
{
  if (isDepthTest())
    CQGLStateInst->setDepthTest(true);
  else
    CQGLStateInst->setDepthTest(false);
}

void
CQCamera3DWidget::
enableCullFace()
{
  if (isCullFace())
    CQGLStateInst->setCullFace(true);
  else
    CQGLStateInst->setCullFace(false);
}

void
CQCamera3DWidget::
enableFrontFace()
{
  CQGLStateInst->setFrontFace(isFrontFace() ? GL_CW : GL_CCW);
}

void
CQCamera3DWidget::
enablePolygonLine()
{
  if (isPolygonLine())
    CQGLStateInst->setPolygonOffsetLine(true);
  else
    CQGLStateInst->setPolygonOffsetLine(false);

  glPolygonOffset(-1.0f, -1.0f);
}

//---

CQGLTexture *
CQCamera3DWidget::
getGLTexture(CGeomTexture *texture, bool /*add*/)
{
  auto *texture1 = dynamic_cast<CQCamera3DTexture *>(texture);
  assert(texture1);

  if (! texture1->glTexture(this)) {
    //if (! add) return nullptr;

    initGLTexture(texture1);

    Q_EMIT textureAdded();
  }

  return texture1->glTexture(this);
}

void
CQCamera3DWidget::
initGLTexture(CQCamera3DTexture *texture)
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
CQCamera3DWidget::
makeTexture(const CImagePtr &image) const
{
  auto *texture = new CQGLTexture(image);

  texture->setFunctions(const_cast<CQCamera3DWidget *>(this));

  return texture;
}
