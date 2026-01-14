#include <CQNewGLSkybox.h>
#include <CQNewGLShaderProgram.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLModel.h>
#include <CQNewGLShapes.h>

#include <CQGLCubemap.h>
#include <CQGLBuffer.h>
#include <CQGLTexture.h>
#include <CQGLUtil.h>
#include <CQImage.h>
#include <QFileInfo>

CQNewGLSkybox::
CQNewGLSkybox(CQNewGLCanvas *canvas) :
 CQNewGLObject(canvas), canvas_(canvas)
{
}

CQNewGLShaderProgram *
CQNewGLSkybox::
shaderProgram()
{
  return getShader("skybox.vs", "skybox.fs");
}

void
CQNewGLSkybox::
setDirName(const QString &s)
{
  dirName_ = s;

  loadCubeMap();
}

CQGLBuffer *
CQNewGLSkybox::
initBuffer()
{
  auto *buffer = CQNewGLObject::initBuffer();

  if (! cubemap_)
    loadCubeMap();

  return buffer;
}

void
CQNewGLSkybox::
loadCubeMap()
{
  std::vector<QString> strs = { "right", "left", "top", "bottom", "front", "back" };
  std::vector<QString> types = { "jpg", "png" };

  images_.resize(6);

  auto dirName1 = canvas_->app()->buildDir() + "/cube_map/" + dirName();

  for (uint i = 0; i < strs.size(); ++i) {
    auto filename = dirName1 + "/" + strs[i];

    QString filename1;

    for (uint j = 0; j < types.size(); ++j) {
      filename1 = filename + "." + types[j];

      QFileInfo fi(filename1);
      if (fi.exists())
        break;

      filename1 = "";
    }

    if (filename1 == "") {
      std::cerr << "No image for '" << strs[i].toStdString() << "'\n";
      continue;
    }

    CImageFileSrc src(filename1.toStdString());
    auto image = CImageMgrInst->createImage(src);

    if (! image) {
      std::cerr << "Invalid image '" << filename1.toStdString() << "'\n";
      continue;
    }

    images_[i] = dynamic_cast<CQImage *>(image.get())->getQImage();
  }

  delete cubemap_;

  cubemap_ = new CQGLCubemap;
  cubemap_->setImages(images_);

  canvas_->glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void
CQNewGLSkybox::
addGeometry()
{
  initBuffer();

  //---

#if 0
  auto addP = [&](double x, double y, double z) {
    buffer_->addPoint(width_*x, width_*y, width_*z);
  };

  // unit cube
  addP(-0.5, -0.5, -0.5); addP( 0.5, -0.5, -0.5); addP( 0.5,  0.5, -0.5); addP( 0.5,  0.5, -0.5);
  addP(-0.5,  0.5, -0.5); addP(-0.5, -0.5, -0.5); addP(-0.5, -0.5,  0.5); addP( 0.5, -0.5,  0.5);
  addP( 0.5,  0.5,  0.5); addP( 0.5,  0.5,  0.5); addP(-0.5,  0.5,  0.5); addP(-0.5, -0.5,  0.5);
  addP(-0.5,  0.5,  0.5); addP(-0.5,  0.5, -0.5); addP(-0.5, -0.5, -0.5); addP(-0.5, -0.5, -0.5);
  addP(-0.5, -0.5,  0.5); addP(-0.5,  0.5,  0.5); addP( 0.5,  0.5,  0.5); addP( 0.5,  0.5, -0.5);
  addP( 0.5, -0.5, -0.5); addP( 0.5, -0.5, -0.5); addP( 0.5, -0.5,  0.5); addP( 0.5,  0.5,  0.5);
  addP(-0.5, -0.5, -0.5); addP( 0.5, -0.5, -0.5); addP( 0.5, -0.5,  0.5); addP( 0.5, -0.5,  0.5);
  addP(-0.5, -0.5,  0.5); addP(-0.5, -0.5, -0.5); addP(-0.5,  0.5, -0.5); addP( 0.5,  0.5, -0.5);
  addP( 0.5,  0.5,  0.5); addP( 0.5,  0.5,  0.5); addP(-0.5,  0.5,  0.5); addP(-0.5,  0.5, -0.5);
#endif

#if 0
  static double cube_normal[6][3] = {
    {-1.0,  0.0,  0.0},
    { 0.0,  1.0,  0.0},
    { 1.0,  0.0,  0.0},
    { 0.0, -1.0,  0.0},
    { 0.0,  0.0,  1.0},
    { 0.0,  0.0, -1.0}
  };

  static int cube_faces[6][4] = {
    {0, 1, 2, 3},
    {3, 2, 6, 7},
    {7, 6, 5, 4},
    {4, 5, 1, 0},
    {5, 6, 2, 1},
    {7, 4, 0, 3}
  };

  double v[8][3];

  v[0][0] = v[1][0] = v[2][0] = v[3][0] = -0.5;
  v[4][0] = v[5][0] = v[6][0] = v[7][0] =  0.5;
  v[0][1] = v[1][1] = v[4][1] = v[5][1] = -0.5;
  v[2][1] = v[3][1] = v[6][1] = v[7][1] =  0.5;
  v[0][2] = v[3][2] = v[4][2] = v[7][2] = -0.5;
  v[1][2] = v[2][2] = v[5][2] = v[6][2] =  0.5;

  auto addPoint = [&](double *p, double *n, const CPoint2D &tp) {
    auto x = p[0];
    auto y = p[1];
    auto z = p[2];

    buffer_->addPoint(width_*x, width_*y, width_*z);

    buffer_->addNormal(n[0], n[1], n[2]);

    buffer_->addTexturePoint(tp.x, tp.y);
  };

  int pos = 0;

  for (GLint i = 5; i >= 0; i--) {
    CQNewGLFaceData faceData;

    faceData.pos = pos;
    faceData.len = 4;

    auto normal = &cube_normal[i][0];

    auto tp1 = CPoint2D(0.0, 0.0); addPoint(&v[cube_faces[i][0]][0], normal, tp1);
    auto tp2 = CPoint2D(1.0, 0.0); addPoint(&v[cube_faces[i][1]][0], normal, tp2);
    auto tp3 = CPoint2D(1.0, 1.0); addPoint(&v[cube_faces[i][2]][0], normal, tp3);
    auto tp4 = CPoint2D(0.0, 1.0); addPoint(&v[cube_faces[i][3]][0], normal, tp4);

    this->addFaceData(faceData);

    pos += faceData.len;
  }
#endif

  CQNewGLShapes::ShapeData shapeData;
  shapeData.color = CRGBA(1, 1, 1);

  CQNewGLFaceDataList faceDataList;

  auto bbox = CBBox3D(CPoint3D(-width_/2, 0, 0), CPoint3D(width_/2, 0, 0));

  CQNewGLShapes::addCube(buffer_, bbox, shapeData, faceDataList);

  setFaceDatas(faceDataList.faceDatas);

  //---

  buffer_->load();
}

void
CQNewGLSkybox::
drawGeometry()
{
  if (! buffer_)
    return;

//glDepthMask(GL_FALSE);
  glDepthFunc(GL_LEQUAL);

  auto *program = CQNewGLSkybox::shaderProgram();

  program->bind();

  buffer_->bind();

  //---

//auto modelMatrix = getModelMatrix();
  auto modelMatrix = CMatrix3D::identity();
  canvas_->addShaderMVP(program, modelMatrix);

  //---

  cubemap_->enable(/*enable*/true);

  glActiveTexture(GL_TEXTURE0);

  cubemap_->bind();

  //---

  program->setUniformValue("textureId", 0);

  program->setUniformValue("isWireframe", isWireframe());

  //---

  // draw cube
  for (const auto &faceData : this->faceDatas()) {
    if (isWireframe())
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
  }

  //---

  buffer_->unbind();

  program->release();

//glDepthMask(GL_TRUE);
  glDepthFunc(GL_LESS);
}
