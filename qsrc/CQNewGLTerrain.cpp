#include <CQNewGLTerrain.h>
#include <CQNewGLShaderProgram.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLModel.h>

#include <CQGLBuffer.h>
#include <CQGLUtil.h>
#include <CQGLTexture.h>
#include <CGeometry3D.h>
#include <CGeomTexture.h>

#include <CSolidNoise.h>
#include <CWaterSurface.h>

CQNewGLTerrain::
CQNewGLTerrain(CQNewGLCanvas *canvas) :
 CQNewGLObject(canvas), canvas_(canvas)
{
  auto *app = canvas_->app();

  connect(app, SIGNAL(timerStep()), this, SLOT(stepSlot()));
}

CQNewGLShaderProgram *
CQNewGLTerrain::
shaderProgram()
{
  return getShader("terrain.vs", "terrain.fs");
}

void
CQNewGLTerrain::
stepSlot()
{
  if (type() == Type::WATER_SURFACE) {
    if (! waterSurface_)
      return;

    waterSurface_->step();

    updateWaterSurfaceGeometry();

    canvas_->update();
  }
}

CQGLBuffer *
CQNewGLTerrain::
initBuffer()
{
  auto *buffer = CQNewGLObject::initBuffer();

  //---

  auto *app = canvas_->app();

  auto addTexture = [&](const QString &name, bool flipV=false) {
    auto filename = app->buildDir() + "/" + name;

    CImageFileSrc src(filename.toStdString());
    auto image = CImageMgrInst->createImage(src);

    if (flipV)
      image->flipH();

    auto *texture = CGeometry3DInst->createTexture(image);

    texture->setName(filename.toStdString());

    return canvas_->getTexture(texture, /*add*/true);
  };

  if (! texture_)
    texture_ = addTexture("textures/terrain.jpg");

  //---

  return buffer;
}

void
CQNewGLTerrain::
addGeometry()
{
  if      (type() == Type::NOISE)
    addNoiseGeometry();
  else if (type() == Type::WATER_SURFACE)
    addWaterSurfaceGeometry();
}

void
CQNewGLTerrain::
addNoiseGeometry()
{
  // calc terrain
  CSolidNoise2D noise;

  double width  = this->width ();
  double height = this->height();

  xmin_ = -width/2.0;
  ymin_ = -width/2.0;
  xmax_ =  width/2.0;
  ymax_ =  width/2.0;

  int n       = this->gridSize();
  int octaves = this->octaves();

  std::vector<double> x, y, z;

  x.resize(n);
  y.resize(n);
  z.resize(n*n);

  for (int iy = 0, iz = 0; iy < n; ++iy) {
    y[iy] = CMathUtil::map(iy, 0, n - 1, ymin_, ymax_);

    for (int ix = 0; ix < n; ++ix, ++iz) {
      x[ix] = CMathUtil::map(ix, 0, n - 1, xmin_, xmax_);

      z[iz] = noise.turbulence(CVector2D(x[ix], y[iy]), octaves);
    }
  }

  //---

  initBuffer();

  //---

  int pos = 0;

  auto addPoint = [&](const CPoint3D &p, const QColor &c, const CVector3D &normal,
                      const CPoint2D &tp) {
    buffer_->addPoint(p.x, p.y, p.z);
    buffer_->addNormal(normal.getX(), normal.getY(), normal.getZ());
    buffer_->addColor(c.redF(), c.greenF(), c.blueF());
    buffer_->addTexturePoint(tp.x, tp.y);
  };

  struct PointData {
    CPoint3D  p;
    CVector3D n;
    CPoint2D  tp;
    QColor    c;
  };

  auto pointColor = [&](const CPoint3D &p) {
    auto y1 = p.y/height;

    if      (y1 < 0.2) {
      auto f = CMathUtil::map(y1, 0.0, 0.2, 0, 255);

      return QColor(10, 10, f);
    }
    else if (y1 < 0.8) {
      auto f = CMathUtil::map(y1, 0.2, 0.8, 0, 255);

      return QColor(10, f, 10);
    }
    else {
      auto f = CMathUtil::map(y1, 0.8, 1.0, 0, 255);

      return QColor(f, f, f);
    }
  };

  auto addRect = [&](const PointData &p1, const PointData &p2,
                     const PointData &p3, const PointData &p4) {
    CQNewGLFaceData faceData;

    faceData.pos = pos;
    faceData.len = 4;

    addPoint(p1.p, p1.c, p1.n, p1.tp);
    addPoint(p2.p, p2.c, p2.n, p2.tp);

    addPoint(p3.p, p3.c, p3.n, p3.tp);
    addPoint(p4.p, p4.c, p4.n, p4.tp);

    this->addFaceData(faceData);

    pos += faceData.len;
  };

  auto genIZ = [&](int ix, int iy) {
    return (iy*n + ix);
  };

  auto getTerrainPoint = [&](int ix, int iy) {
    auto x1 = width*x[ix];
    auto y1 = width*y[iy];

    auto z1 = height*z[genIZ(ix, iy)];

    return CPoint3D(x1, z1, y1);
  };

  auto dx = 1.0/(n - 1.0);
  auto dy = 1.0/(n - 1.0);

  auto calcTerrainPoint = [&](int ix, int iy, PointData &p) {
    p.p = getTerrainPoint(ix, iy);

    if (ix > 0 && ix < n - 1 && iy > 0 && iy < n - 1) {
#if 1
      auto p1 = getTerrainPoint(ix - 1, iy);
      auto p2 = getTerrainPoint(ix + 1, iy);
      auto p3 = getTerrainPoint(ix, iy - 1);
      auto p4 = getTerrainPoint(ix, iy + 1);

      CVector3D diff1(p.p, p1);
      CVector3D diff2(p.p, p2);
      CVector3D diff3(p.p, p3);
      CVector3D diff4(p.p, p4);

      auto n1 = diff3.crossProduct(diff1).normalized();
      auto n2 = diff1.crossProduct(diff4).normalized();
      auto n3 = diff2.crossProduct(diff3).normalized();
      auto n4 = diff4.crossProduct(diff2).normalized();

      p.n = (n1 + n2 + n3 + n4).normalized();
#else
      p.n = CVector3D(0, 1, 0);
#endif
    }
    else
      p.n = CVector3D(0, 1, 0);

    p.tp = CPoint2D(ix*dx, iy*dy);

    p.c = pointColor(p.p);
  };

  for (int iy = 1; iy < n; ++iy) {
    for (int ix = 1; ix < n; ++ix) {
      PointData p1, p2, p3, p4;

      calcTerrainPoint(ix - 1, iy - 1, p1);
      calcTerrainPoint(ix    , iy - 1, p2);
      calcTerrainPoint(ix    , iy    , p3);
      calcTerrainPoint(ix - 1, iy    , p4);

      addRect(p1, p2, p3, p4);
    }
  }

  //---

  buffer_->load();
}

void
CQNewGLTerrain::
addWaterSurfaceGeometry()
{
  int n = this->gridSize();

  //---

  delete waterSurface_;
  waterSurface_ = new CWaterSurface(n - 1);

  //---

  for (int iy = 1; iy < n - 1; ++iy) {
    for (int ix = 1; ix < n - 1; ++ix) {
      waterSurface_->setDampening(ix, iy, 1.0);

      waterSurface_->setZ(ix, iy, 0.0);
    }
  }

  waterSurface_->setZ(int(    n/4.0), int(    n/4.0), 1.0);
  waterSurface_->setZ(int(3.0*n/4.0), int(3.0*n/4.0), 1.0);

  //---

  updateWaterSurfaceGeometry();
}

void
CQNewGLTerrain::
updateWaterSurfaceGeometry()
{
  initBuffer();

  //---

  int n = this->gridSize();

  double width  = this->width ()/std::sqrt(1.0*n);
  double height = this->height();

  //---

  int pos = 0;

  auto addPoint = [&](const CPoint3D &p, const QColor &c, const CVector3D &normal,
                      const CPoint2D &tp) {
    buffer_->addPoint(width*p.x, height*p.y, width*p.z);
    buffer_->addNormal(normal.getX(), normal.getY(), normal.getZ());
    buffer_->addColor(c.redF(), c.greenF(), c.blueF());
    buffer_->addTexturePoint(tp.x, tp.y);
  };

  struct PointData {
    CPoint3D  p;
    CVector3D n;
    CPoint2D  tp;
  };

  auto addRect = [&](const PointData &p1, const PointData &p2,
                     const PointData &p3, const PointData &p4) {
    CQNewGLFaceData faceData;

    faceData.pos = pos;
    faceData.len = 4;

    addPoint(p1.p, color_, p1.n, p1.tp);
    addPoint(p2.p, color_, p2.n, p2.tp);

    addPoint(p3.p, color_, p3.n, p3.tp);
    addPoint(p4.p, color_, p4.n, p4.tp);

    this->addFaceData(faceData);

    pos += faceData.len;
  };

  auto getWaterPoint = [&](int ix, int iy) {
    return CPoint3D(waterSurface_->getX(ix, iy),
                    waterSurface_->getZ(ix, iy),
                    waterSurface_->getY(ix, iy));
  };

  auto getWaterNormal = [&](int ix, int iy) {
    auto n = waterSurface_->getNormal(ix, iy);

    return CVector3D(n.getX(), n.getZ(), n.getY());
  };

  auto dx = 1.0/(n - 1.0);
  auto dy = 1.0/(n - 1.0);

  auto calcWaterPoint = [&](int ix, int iy, PointData &p) {
    p.p = getWaterPoint(ix, iy);

    if (ix > 0 && ix < n - 1 && iy > 0 && iy < n - 1) {
#if 0
      auto p1 = getWaterPoint(ix - 1, iy);
      auto p2 = getWaterPoint(ix + 1, iy);
      auto p3 = getWaterPoint(ix, iy - 1);
      auto p4 = getWaterPoint(ix, iy + 1);

      CVector3D diff1(p.p, p1);
      CVector3D diff2(p.p, p2);
      CVector3D diff3(p.p, p3);
      CVector3D diff4(p.p, p4);

      auto n1 = diff3.crossProduct(diff1).normalized();
      auto n2 = diff1.crossProduct(diff4).normalized();
      auto n3 = diff2.crossProduct(diff3).normalized();
      auto n4 = diff4.crossProduct(diff2).normalized();

      p.n = (n1 + n2 + n3 + n4).normalized();
#else
      p.n = getWaterNormal(ix, iy);
#endif
    }
    else
      p.n = CVector3D(0, 1, 0);

    p.tp = CPoint2D(ix*dx, iy*dy);
  };

  for (int iy = 1; iy < n; ++iy) {
    for (int ix = 1; ix < n; ++ix) {
      PointData p1, p2, p3, p4;

      calcWaterPoint(ix - 1, iy - 1, p1);
      calcWaterPoint(ix    , iy - 1, p2);
      calcWaterPoint(ix    , iy    , p3);
      calcWaterPoint(ix - 1, iy    , p4);

      addRect(p1, p2, p3, p4);
    }
  }

  //---

  buffer_->load();
}

void
CQNewGLTerrain::
drawGeometry()
{
  if (! buffer_)
    return;

  //---

  auto *program = CQNewGLTerrain::shaderProgram();

  program->bind();

  buffer_->bind();

  //---

  program->setUniformValue("viewPos", CQGLUtil::toVector(canvas_->viewPos()));

  auto modelMatrix = CMatrix3D::identity();
  canvas_->addShaderMVP(program, modelMatrix);

  //---

  canvas_->addShaderLights(program);

  //---

  canvas_->addShaderLightGlobals(program);

  program->setUniformValue("shininess", 10.0f);
  program->setUniformValue("emissionColor", CQGLUtil::toVector(QColor(255, 255, 255)));

  program->setUniformValue("isSelected", isSelected());

  //---

  glActiveTexture(GL_TEXTURE0);
  texture_->bind();

  program->setUniformValue("useTexture", isTextured());
  program->setUniformValue("textureId", 0);

  //---

  // draw terrain
  for (const auto &faceData : faceDatas()) {
    if (isWireframe()) {
      program->setUniformValue("isWireframe", true);

      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

      glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
    }

    if (isSolid()) {
      program->setUniformValue("isWireframe", false);

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

      glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
    }
  }

  //---

  buffer_->unbind();

  program->release();
}
