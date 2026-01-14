#include <CQNewGLEmitter.h>
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

#include <CFireworks.h>
#include <CFlocking.h>
#include <CParticle3D.h>
#include <CImageLib.h>
#include <COSRand.h>

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

class CQNewGLEmitterParticleSystem : public CParticleSystem3D {
 public:
  CQNewGLEmitterParticleSystem() {
  }

  CParticle3D *createParticle() override {
    return new CQNewGLParticle(*this);
  }
};

//---

CQNewGLEmitter::
CQNewGLEmitter(CQNewGLCanvas *canvas) :
 CQNewGLObject(canvas), canvas_(canvas)
{
  auto *app = canvas_->app();

  particleSystem_ = new CQNewGLEmitterParticleSystem;

  connect(app, SIGNAL(timerStep()), this, SLOT(stepSlot()));
}

CQNewGLShaderProgram *
CQNewGLEmitter::
shaderProgram()
{
  return getShader("particle.vs", "particle.fs");
}

void
CQNewGLEmitter::
setType(const Type &t)
{
  type_ = t;

  if      (type_ == Type::FLOCKING) {
    if (! flocking_) {
      flocking_ = new CFlocking;
    }
  }
  else if (type_ == Type::FIREWORKS) {
    if (! fireworks_) {
      fireworks_ = new CFireworks;

      fireworks_->setOrigin(CPoint2D(position_.x, position_.y));

//    fireworks_->setInjectXVelMin(minVelocity_.getX());
//    fireworks_->setInjectYVelMin(minVelocity_.getX());
//    fireworks_->setInjectXVelMax(maxVelocity_.getX());
//    fireworks_->setInjectYVelMax(maxVelocity_.getX());

      fireworks_->setInjectTicks(emitInterval_);
    }
  }
}

void
CQNewGLEmitter::
setPosition(const CPoint3D &p)
{
  position_ = p;

  if (type_ == Type::FIREWORKS) {
    if (! fireworks_) return;

    fireworks_->setOrigin(CPoint2D(position_.x, position_.y));
  }
}

void
CQNewGLEmitter::
setMinVelocity(const CVector3D &v)
{
  minVelocity_ = v;

  if (type_ == Type::FIREWORKS) {
    if (! fireworks_) return;

//  fireworks_->setInjectXVelMin(minVelocity_.getX());
//  fireworks_->setInjectYVelMin(minVelocity_.getX());
  }
}

void
CQNewGLEmitter::
setMaxVelocity(const CVector3D &v)
{
  maxVelocity_ = v;

  if (type_ == Type::FIREWORKS) {
    if (! fireworks_) return;

//  fireworks_->setInjectXVelMax(maxVelocity_.getX());
//  fireworks_->setInjectYVelMax(maxVelocity_.getX());
  }
}

void
CQNewGLEmitter::
setEmitInterval(int i)
{
  emitInterval_ = i;

  if (type_ == Type::FIREWORKS) {
    if (! fireworks_) return;

    fireworks_->setInjectTicks(emitInterval_);
  }
}

void
CQNewGLEmitter::
setImageName(const QString &s)
{
  imageName_ = s;

  updateTexture();
}

void
CQNewGLEmitter::
updateTexture()
{
  auto *app = canvas_->app();

  auto addTexture = [&](const QString &name, bool flipV=false) {
    auto filename = app->buildDir() + "/textures/" + name;

    CImageFileSrc src(filename.toStdString());
    auto image = CImageMgrInst->createImage(src);
    if (! image) return static_cast<CQGLTexture *>(nullptr);

    if (flipV)
      image->flipH();

    auto *texture = CGeometry3DInst->createTexture(image);

    texture->setName(filename.toStdString());

    return canvas_->getTexture(texture, /*add*/true);
  };

  texture_ = addTexture(imageName_);
}

void
CQNewGLEmitter::
stepSlot()
{
  if (! isRunning())
    return;

  //---

  ++steps_;

  //--

  auto emitInterval = std::max(this->emitInterval(), 1);

  if      (type_ == Type::GENERATOR) {
    particleSystem_->setGravity(gravity());

    particleSystem_->step(step());
    particleSystem_->age();

    //---

    if ((steps_ % emitInterval) == 0) {
      particleSystem_->setMaxParticles(maxParticles());
      particleSystem_->setMaxAge(maxAge());

      auto *particle = particleSystem_->addParticle();

      particle->setMass(mass());

      particle->setPosition(position().x, position().y, position().z);

      auto vx = COSRand::randIn(minVelocity().getX(), maxVelocity().getX());
      auto vy = COSRand::randIn(minVelocity().getY(), maxVelocity().getY());
      auto vz = COSRand::randIn(minVelocity().getZ(), maxVelocity().getZ());

      particle->setVelocity(vx, vy, vz);

      particle->setColor(startColor_);

      updateGeometry_ = true;
    }
  }
  else if (type_ == Type::FLOCKING) {
    flocking_->update(0.1);

    updateGeometry_ = true;
  }
  else if (type_ == Type::FIREWORKS) {
    fireworks_->step();

    fireworks_->updateParticles();

    fireworks_->updateCurrentParticles();

    updateGeometry_ = true;
  }

  canvas_->update();
}

void
CQNewGLEmitter::
updateGeometry()
{
  if (! updateGeometry_)
    return;

  updateGeometry_ = false;

  //---

  initBuffer();

  if (! texture_)
    updateTexture();

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

  if      (type_ == Type::GENERATOR) {
    for (auto *particle : particleSystem_->getParticles()) {
      auto *particle1 = dynamic_cast<CQNewGLParticle *>(particle);

      auto pos = particle1->getPosition();

      auto color = startColor_.blended(endColor_,
        CMathUtil::map(particle->getAge(), 0.0, maxAge() - 1.0, 1.0, 0.0));

      FaceDatas faceDatas;
#if 0
      canvas_->addCube(buffer_, CPoint3D(0, 0, 0), 1.0, color(), faceDatas);
#else
      CQNewGLFaceData faceData;

      addRect(CPoint3D(-0.5, -0.5, 0.0), CPoint3D( 0.5, -0.5, 0.0),
              CPoint3D( 0.5,  0.5, 0.0), CPoint3D(-0.5,  0.5, 0.0),
              color, faceData);

      faceDatas.push_back(faceData);
#endif

      particle1->setFaceDatas(faceDatas);
    }
  }
  else if (type_ == Type::FLOCKING) {
    double w = CFlock::getWorld().getXSize()/2;
    double h = CFlock::getWorld().getYSize()/2;
    double l = CFlock::getWorld().getZSize()/2;

    auto sceneScale = canvas_->sceneScale();

    for (auto *boid : flocking_->getBoids()) {
      auto p = boid->getPos();

      auto x = CMathUtil::map(p.getX(), -w, w, -sceneScale, sceneScale);
      auto y = CMathUtil::map(p.getY(), -h, h, -sceneScale, sceneScale);
      auto z = CMathUtil::map(p.getZ(), -l, l, -sceneScale, sceneScale);

      auto c = boid->getFlock()->getColor();

      auto point = CPoint3D(x, y, z);
      auto color = CRGBA(c.getRed(), c.getGreen(), c.getBlue());

      CQNewGLFaceData faceData;

      faceData.position = point;

      addRect(CPoint3D(-0.5, -0.5, 0.0), CPoint3D( 0.5, -0.5, 0.0),
              CPoint3D( 0.5,  0.5, 0.0), CPoint3D(-0.5,  0.5, 0.0),
              color, faceData);

      faceDataList_.faceDatas.push_back(faceData);
    }
  }
  else if (type_ == Type::FIREWORKS) {
    const auto &particles = fireworks_->currentParticles();

    auto sceneScale = canvas_->sceneScale();

    for (uint i = 0; i < particles.size(); ++i) {
      auto *particle = particles[i];
      if (! particle) continue;

      auto p = particle->getPosition();

      auto x = CMathUtil::map(p.getX(), -100, 100, -sceneScale, sceneScale);
      auto y = position_.y;
      auto z = CMathUtil::map(p.getY(), 0, 200, -sceneScale, sceneScale);

      auto c1 = particle->getColor();

      auto point = CPoint3D(x, y, z);
      auto color = CRGBA(c1.getRed(), c1.getGreen(), c1.getBlue());

      CQNewGLFaceData faceData;

      faceData.position = point;
      faceData.boneId   = particle->getAge();

      addRect(CPoint3D(-0.5, -0.5, 0.0), CPoint3D( 0.5, -0.5, 0.0),
              CPoint3D( 0.5,  0.5, 0.0), CPoint3D(-0.5,  0.5, 0.0),
              color, faceData);

      faceDataList_.faceDatas.push_back(faceData);
    }
  }

  buffer_->load();
}

void
CQNewGLEmitter::
drawGeometry()
{
  if (! isRunning())
    return;

  updateGeometry();

  //---

  canvas_->enableBlend();

  //---

  auto *program = CQNewGLEmitter::shaderProgram();

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

  if      (type_ == Type::GENERATOR) {
    // sort particles by z
    using Particles       = std::vector<CQNewGLParticle *>;
    using SortedParticles = std::map<double, Particles>;

    SortedParticles sortedParticles;

    for (auto *particle : particleSystem_->getParticles()) {
      auto *particle1 = dynamic_cast<CQNewGLParticle *>(particle);

      if (particle1->isDead())
        continue;

      auto pos  = particle1->getPosition();
      auto pos1 = camera->getViewMatrix()*vectorToGLVector(pos);

      sortedParticles[pos1.getZ()].push_back(particle1);
    }

    //---

    // draw particles
    program->setUniformValue("maxAge", particleSystem_->maxAge());

    for (const auto &ps : sortedParticles) {
      for (auto *particle1 : ps.second) {
        auto pos = particle1->getPosition();

        program->setUniformValue("position", vectorToQVector(pos));
        program->setUniformValue("size"    , float(pointSize()));
        program->setUniformValue("age"     , particle1->getAge());

        for (const auto &faceData : particle1->faceDatas()) {
          if (isWireframe())
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
          else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

          glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
        }
      }
    }
  }
  else if (type_ == Type::FLOCKING) {
    program->setUniformValue("maxAge", 100);

    for (const auto &faceData : faceDataList_.faceDatas) {
      program->setUniformValue("position", pointToQVector(faceData.position));
      program->setUniformValue("size"    , float(pointSize()));
      program->setUniformValue("age"     , 0);

      if (isWireframe())
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

      glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
    }
  }
  else if (type_ == Type::FIREWORKS) {
    program->setUniformValue("maxAge", 100);

    for (const auto &faceData : faceDataList_.faceDatas) {
      program->setUniformValue("position", pointToQVector(faceData.position));
      program->setUniformValue("size"    , float(pointSize()));
      program->setUniformValue("age"     , faceData.boneId);

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

  canvas_->disableBlend();
}
