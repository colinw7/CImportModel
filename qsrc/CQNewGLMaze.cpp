#include <CQNewGLMaze.h>
#include <CQNewGLShaderProgram.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLModel.h>

#include <CQGLBuffer.h>
#include <CQGLUtil.h>
#include <CQGLTexture.h>
#include <CGeometry3D.h>
#include <CGeomTexture.h>

#include <CDungeon.h>
#include <CImageLib.h>

CQNewGLShaderProgram *CQNewGLMaze::shaderProgram_;

void
CQNewGLMaze::
initShader(CQNewGLCanvas *canvas)
{
  shaderProgram_ = new CQNewGLShaderProgram(canvas);
  shaderProgram_->addShaders("maze.vs", "maze.fs");
}

CQNewGLMaze::
CQNewGLMaze(CQNewGLCanvas *canvas) :
 CQNewGLObject(canvas)
{
  auto filename = canvas_->app()->buildDir() + "/data/maze.xml";

  dungeon_ = new CDungeon;

  dungeon_->init();
  dungeon_->load(filename.toStdString());
}

void
CQNewGLMaze::
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

  if (! nwallTexture_) {
    nwallTexture_ = addTexture("textures/wall2.jpg");
    swallTexture_ = addTexture("textures/wall2.jpg");
    wwallTexture_ = addTexture("textures/wall2.jpg");
    ewallTexture_ = addTexture("textures/wall2.jpg");

    floorTexture_   = addTexture("textures/floor1.gif");
    doorTexture_    = addTexture("textures/door1.jpg");
    outsideTexture_ = addTexture("textures/wall1.jpg");
  }
}

void
CQNewGLMaze::
addGeometry()
{
  initBuffer();

  //---

  auto size = this->width();

  auto ymin = 0.0;
  auto ymax = this->height();

  int pos = 0;

  auto color = QColor(100, 100, 100);

  auto addPoint = [&](const CPoint3D &p, const QColor &c, const CVector3D &normal,
                      const CPoint2D &pt) {
    buffer_->addPoint(p.x, p.y, p.z);
    buffer_->addNormal(normal.getX(), normal.getY(), normal.getZ());
    buffer_->addColor(c.redF(), c.greenF(), c.blueF());
    buffer_->addTexturePoint(pt.x, pt.y);
  };

  auto addRect = [&](const CPoint3D &p1, const CPoint3D &p2,
                     const CPoint3D &p3, const CPoint3D &p4,
                     CQGLTexture *texture) {
    CQNewGLFaceData faceData;

    faceData.pos = pos;
    faceData.len = 4;

    faceData.diffuseTexture = texture;

    CVector3D diff1(p1, p2);
    CVector3D diff2(p2, p3);

    auto normal = diff1.crossProduct(diff2).normalized();

    addPoint(p1, color, normal, CPoint2D(0, 0));
    addPoint(p2, color, normal, CPoint2D(1, 0));

    addPoint(p3, color, normal, CPoint2D(1, 1));
    addPoint(p4, color, normal, CPoint2D(0, 1));

    this->addFaceData(faceData);

    pos += faceData.len;
  };

  auto *dungeon = this->dungeon();

  const auto &dbbox = dungeon->getBBox();

  auto xs = size/dbbox.getWidth ();
  auto ys = size/dbbox.getHeight();

  auto dx = -size/2.0;
//auto dy = 0.0;
  auto dz = -size/2.0;

  const auto &rooms = dungeon->getRooms();

  for (auto *room : rooms) {
    const auto &bbox = room->getBBox();

    double x1 = bbox.getXMin()*xs + dx, y1 = ymin, z1 = bbox.getYMin()*ys + dz;
    double x2 = bbox.getXMax()*xs + dx, y2 = ymax, z2 = bbox.getYMax()*ys + dz;

    // floor
    addRect(CPoint3D(x1, ymin, z2), CPoint3D(x2, ymin, z2),
            CPoint3D(x2, ymin, z1), CPoint3D(x1, ymin, z1),
            floorTexture_);

    auto *nwall = room->getWall(CCompassType::NORTH);
    auto *swall = room->getWall(CCompassType::SOUTH);
    auto *wwall = room->getWall(CCompassType::WEST );
    auto *ewall = room->getWall(CCompassType::EAST );

    bool nvis = nwall->getVisible();
    bool svis = swall->getVisible();
    bool wvis = wwall->getVisible();
    bool evis = ewall->getVisible();

    auto *nroom = room->getNRoom();
    auto *sroom = room->getSRoom();
    auto *wroom = room->getWRoom();
    auto *eroom = room->getERoom();

    if (nvis)
      addRect(CPoint3D(x1, y1, z2), CPoint3D(x2, y1, z2),
              CPoint3D(x2, y2, z2), CPoint3D(x1, y2, z2),
              nwallTexture_);

    if (! nroom) {
      if (! nvis)
        addRect(CPoint3D(x1, y1, z2), CPoint3D(x2, y1, z2),
                CPoint3D(x2, y2, z2), CPoint3D(x1, y2, z2),
                doorTexture_);
      else
        addRect(CPoint3D(x1, y2, z2), CPoint3D(x2, y2, z2),
                CPoint3D(x2, y1, z2), CPoint3D(x1, y1, z2),
                outsideTexture_);
    }

    if (svis)
      addRect(CPoint3D(x2, y1, z1), CPoint3D(x1, y1, z1),
              CPoint3D(x1, y2, z1), CPoint3D(x2, y2, z1),
              swallTexture_);

    if (! sroom) {
      if (! svis)
        addRect(CPoint3D(x2, y1, z1), CPoint3D(x1, y1, z1),
                CPoint3D(x1, y2, z1), CPoint3D(x2, y2, z1),
                doorTexture_);
      else
        addRect(CPoint3D(x2, y1, z1), CPoint3D(x2, y2, z1),
                CPoint3D(x1, y2, z1), CPoint3D(x1, y1, z1),
                outsideTexture_);
    }

    if (wvis)
      addRect(CPoint3D(x1, y1, z1), CPoint3D(x1, y1, z2),
              CPoint3D(x1, y2, z2), CPoint3D(x1, y2, z1),
              wwallTexture_);

    if (! wroom) {
      if (! wvis)
        addRect(CPoint3D(x1, y1, z1), CPoint3D(x1, y1, z2),
                CPoint3D(x1, y2, z2), CPoint3D(x1, y2, z1),
                doorTexture_);
      else
        addRect(CPoint3D(x1, y1, z1), CPoint3D(x1, y2, z1),
                CPoint3D(x1, y2, z2), CPoint3D(x1, y1, z2),
                outsideTexture_);
    }

    if (evis)
      addRect(CPoint3D(x2, y1, z1), CPoint3D(x2, y2, z1),
              CPoint3D(x2, y2, z2), CPoint3D(x2, y1, z2),
              ewallTexture_);

    if (! eroom) {
      if (! evis)
        addRect(CPoint3D(x2, y1, z1), CPoint3D(x2, y2, z1),
                CPoint3D(x2, y2, z2), CPoint3D(x2, y1, z2),
                doorTexture_);
      else
        addRect(CPoint3D(x2, y1, z1), CPoint3D(x2, y1, z2),
                CPoint3D(x2, y2, z2), CPoint3D(x2, y2, z1),
                outsideTexture_);
    }
  }

  buffer_->load();
}

void
CQNewGLMaze::
drawGeometry()
{
  if (! buffer_)
    return;

  auto *program = CQNewGLMaze::shaderProgram();

  program->bind();

  buffer_->bind();

  //---

  //auto modelMatrix = getModelMatrix();
  auto modelMatrix = CMatrix3D::identity();
  canvas_->addShaderMVP(program, modelMatrix);

  //---

  for (const auto &faceData : this->faceDatas()) {
    glActiveTexture(GL_TEXTURE0);
    faceData.diffuseTexture->bind();

    program->setUniformValue("textureId", 0);

    // draw quad
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
  }

  buffer_->unbind();

  program->release();
}
