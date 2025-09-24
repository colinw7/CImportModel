#include <CQNewGLFractal.h>
#include <CQNewGLShaderProgram.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLModel.h>
#include <CQNewGLCamera.h>
#include <CQNewGLUtil.h>

#include <CQGLBuffer.h>
#include <CQGLUtil.h>
#include <CQGLTexture.h>
#include <CGeometry3D.h>
#include <CGeomTexture.h>

#include <CLorenzCalc.h>
#include <CParticle3D.h>
#include <CImageLib.h>

class CQNewGLLorenzCalc : public CLorenzCalc {
 public:
  CQNewGLLorenzCalc(CQNewGLFractal *fractal) :
   fractal_(fractal) {
  }

  void init() {
    int n = getIterationEnd() - getIterationStart();

    points_.resize(n);

    pos_ = 0;
  }

  void drawPoint(double x, double y, double z) override {
    points_[pos_++] = CPoint3D(x, y, z);
  }

  const std::vector<CPoint3D> &points() const { return points_; }

 private:
  CQNewGLFractal*       fractal_ { nullptr };
  std::vector<CPoint3D> points_;
  int                   pos_ { 0 };
};

//---

class CQNewGLParticle : public CParticle3D {
 public:
  using FaceDatas = std::vector<CQNewGLFaceData>;

 public:
  CQNewGLParticle(const CParticleSystem3D &system) :
   CParticle3D(system) {
  }

 ~CQNewGLParticle() override { }

  const FaceDatas &faceDatas() const { return faceDatas_; }

  void setFaceDatas(const FaceDatas &faceDatas) { faceDatas_ = faceDatas; }

 private:
  FaceDatas faceDatas_;
};

class CQNewGLFractalParticleSystem : public CParticleSystem3D {
 public:
  CQNewGLFractalParticleSystem() {
  }

  CParticle3D *createParticle() override {
    return new CQNewGLParticle(*this);
  }
};

//---

CQNewGLShaderProgram *CQNewGLFractal::shaderProgram_;

void
CQNewGLFractal::
initShader(CQNewGLCanvas *canvas)
{
  shaderProgram_ = new CQNewGLShaderProgram(canvas);
  shaderProgram_->addShaders("particle.vs", "particle.fs");
}

CQNewGLFractal::
CQNewGLFractal(CQNewGLCanvas *canvas) :
 CQNewGLObject(canvas)
{
  particleSystem_ = new CQNewGLFractalParticleSystem;

  setType(Type::LORENZ);
}

void
CQNewGLFractal::
setActive(bool b)
{
  active_ = b;

  updateGeometry_ = true;

  canvas_->update();
}

void
CQNewGLFractal::
setType(const Type &t)
{
  type_ = t;

  if (type_ == Type::LORENZ) {
    if (! lorenz_) {
      lorenz_ = new CQNewGLLorenzCalc(this);
    }
  }
}

void
CQNewGLFractal::
initBuffer()
{
  CQNewGLObject::initBuffer();

  //---

  auto addTexture = [&](const QString &name, bool flipV=false) {
    auto filename = canvas_->app()->buildDir() + "/" + name;

    CImageFileSrc src(filename.toStdString());
    auto image = CImageMgrInst->createImage(src);

    if (flipV)
      image->flipH();

    auto *texture = CGeometryInst->createTexture(image);

    texture->setName(filename.toStdString());

    return canvas_->getTexture(texture, /*add*/true);
  };

  if (! texture_)
    texture_ = addTexture("textures/particle.png");
}

void
CQNewGLFractal::
addGeometry()
{
  if (! updateGeometry_)
    return;

  initBuffer();

  //---

  int pos = 0;

  auto addPoint = [&](const CPoint3D &p, const QColor &c, const CVector3D &normal) {
    buffer_->addPoint(p.x, p.y, p.z);
    buffer_->addNormal(normal.getX(), normal.getY(), normal.getZ());
    buffer_->addColor(c.redF(), c.greenF(), c.blueF());
  };

  auto addRect = [&](const CPoint3D &p1, const CPoint3D &p2,
                     const CPoint3D &p3, const CPoint3D &p4,
                     const CRGBA &c, CQNewGLFaceData &faceData) {
    auto color = RGBAToQColor(c);

    faceData.pos = pos;
    faceData.len = 4;

    CVector3D diff1(p1, p2);
    CVector3D diff2(p2, p3);
    CVector3D diff3(p3, p4);
    CVector3D diff4(p4, p1);

    auto normal1 = diff4.crossProduct(diff1).normalized();
    auto normal2 = diff1.crossProduct(diff2).normalized();
    auto normal3 = diff2.crossProduct(diff3).normalized();
    auto normal4 = diff3.crossProduct(diff4).normalized();

    addPoint(p1, color, normal1);
    addPoint(p2, color, normal2);

    addPoint(p3, color, normal3);
    addPoint(p4, color, normal4);

    pos += 4;
  };

  //---

  if      (type_ == Type::LORENZ) {
    CRGBA color(1, 1, 1);

    lorenz_->init();
    lorenz_->draw();

    for (const auto &p : lorenz_->points()) {
      CQNewGLFaceData faceData;

      faceData.position = p;

      addRect(CPoint3D(-0.5, -0.5, 0.0), CPoint3D( 0.5, -0.5, 0.0),
              CPoint3D( 0.5,  0.5, 0.0), CPoint3D(-0.5,  0.5, 0.0),
              color, faceData);

      faceDatas_.push_back(faceData);
    }
  }

  buffer_->load();
}

void
CQNewGLFractal::
drawGeometry()
{
  if (! active_)
    return;

  if (updateGeometry_) {
    addGeometry();

    updateGeometry_ = false;
  }

  //---

  glDepthFunc(GL_LEQUAL);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//glBlendFunc(GL_ONE, GL_ONE);

  //---

  auto *program = CQNewGLFractal::shaderProgram();

  program->bind();

  buffer_->bind();

  //---

  auto modelMatrix = CMatrix3D::identity();
  canvas_->addShaderMVP(program, modelMatrix);

  //---

  // set texture
  glActiveTexture(GL_TEXTURE0);
  texture_->bind();

  program->setUniformValue("useTexture", true);
  program->setUniformValue("textureId", 0);

  //---

  // set camera (for billboard)
  auto *camera = canvas_->getCurrentCamera();

  program->setUniformValue("cameraUp"   , CQGLUtil::toVector(camera->up()));
  program->setUniformValue("cameraRight", CQGLUtil::toVector(camera->right()));

  //---

  if      (type_ == Type::LORENZ) {
    for (const auto &faceData : faceDatas()) {
      auto point = faceData.position;

      program->setUniformValue("position", pointToQVector(point));
      program->setUniformValue("size"    , float(pointSize()));
      program->setUniformValue("age"     , 1);

      if (isWireframe())
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

      glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
    }
  }

  //---

  buffer_->unbind();

  program->release();

  //---

  glDisable(GL_BLEND);
}
