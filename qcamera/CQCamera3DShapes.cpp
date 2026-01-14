#include <CQCamera3DShapes.h>

#include <CQGLBuffer.h>
#include <CGeomObject3D.h>
#include <CGeomCone3D.h>
#include <CGeomCylinder3D.h>
#include <CGeomPyramid3D.h>
#include <CGeomSphere3D.h>
#include <CGeomTorus3D.h>
#include <CGeometry3D.h>

namespace CQCamera3DShapes {

void
addCube(CQGLBuffer *buffer, const CBBox3D &bbox, const ShapeData &data,
        CQCamera3DFaceDataList &faceDataList)
{
  auto center = bbox.getCenter();

  auto data1 = data;

  data1.size = CPoint3D(bbox.getXSize(), bbox.getYSize(), bbox.getZSize());

  addCube(buffer, center - CPoint3D(0.0, 0.5, 0.0), center + CPoint3D(0.0, 0.5, 0.0),
          data1, faceDataList);
}

void
addCube(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, const ShapeData &data,
        CQCamera3DFaceDataList &faceDataList)
{
  auto size   = p1.distanceTo(p2);
  auto center = (p1 + p2)/2.0;

  auto mt = CMatrix3D::translation(center.x, center.y, center.z);
  auto mr = getShapeRotationMatrix(p1, p2);
  auto ms = CMatrix3D::scale(size*data.size.x, size*data.size.y, size*data.size.z);

  //---

  addCube(buffer, mt, mr, ms, data, faceDataList);
}

void
addCube(CQGLBuffer *buffer, const CMatrix3D &mt, const CMatrix3D &mr, const CMatrix3D &ms,
        const ShapeData &data, CQCamera3DFaceDataList &faceDataList)
{
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

  //---

  // transform to specified size and center
  auto modelMatrix  = mt*mr*ms;
  auto normalMatrix = modelMatrix.inverse().transposed();

  //---

  auto cubePoint = [&](int i, int j) {
    const auto &v1 = v[cube_faces[i][j]];

    return CPoint3D(v1[0], v1[1], v1[2]);
  };

  auto cubeNormal = [&](int i) {
    const auto &n = cube_normal[i];

    return CVector3D(n[0], n[1], n[2]);
  };

  auto addPoint = [&](const CPoint3D &p, const CVector3D &n, const CRGBA &c, const CPoint2D &tp) {
    auto p1 = modelMatrix*p;
    auto n1 = (normalMatrix*n).normalize();

    buffer->addPoint(p1.x, p1.y, p1.z);
    buffer->addNormal(n1.getX(), n1.getY(), n1.getZ());
    buffer->addColor(c);
    buffer->addTexturePoint(tp.x, tp.y);
  };

  auto tp1 = CPoint2D(0.0, 0.0);
  auto tp2 = CPoint2D(1.0, 0.0);
  auto tp3 = CPoint2D(1.0, 1.0);
  auto tp4 = CPoint2D(0.0, 1.0);

  for (GLint i = 5; i >= 0; i--) {
    CQCamera3DFaceData faceData;

    faceData.pos = faceDataList.pos;
    faceData.len = 4;

    auto p1 = cubePoint(i, 0);
    auto p2 = cubePoint(i, 1);
    auto p3 = cubePoint(i, 2);
    auto p4 = cubePoint(i, 3);

    auto normal = cubeNormal(i);

    addPoint(p1, normal, data.color, tp1);
    addPoint(p2, normal, data.color, tp2);
    addPoint(p3, normal, data.color, tp3);
    addPoint(p4, normal, data.color, tp4);

    faceDataList.faceDatas.push_back(faceData);

    faceDataList.pos += faceData.len;
  }
}

void
addOrientBox(CQGLBuffer *buffer, const CPoint3D &center, const CVector3D &xv,
             const CVector3D &yv, const CVector3D &zv,
             const ShapeData &data, CQCamera3DFaceDataList &faceDataList)
{
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

  v[0][0] = v[1][0] = v[2][0] = v[3][0] = -1.0;
  v[4][0] = v[5][0] = v[6][0] = v[7][0] =  1.0;
  v[0][1] = v[1][1] = v[4][1] = v[5][1] = -1.0;
  v[2][1] = v[3][1] = v[6][1] = v[7][1] =  1.0;
  v[0][2] = v[3][2] = v[4][2] = v[7][2] = -1.0;
  v[1][2] = v[2][2] = v[5][2] = v[6][2] =  1.0;

  //---

  auto cubePoint = [&](int i, int j) {
    const auto &v1 = v[cube_faces[i][j]];

    return CPoint3D(v1[0], v1[1], v1[2]);
  };

  auto cubeNormal = [&](int i) {
    const auto &n = cube_normal[i];

    return CPoint3D(n[0], n[1], n[2]);
  };

  auto addPoint = [&](const CPoint3D &p, const CPoint3D &n, const CRGBA &c, const CPoint2D &tp) {
    auto p1 = center + (p.x*xv + p.y*yv + p.z*zv).point();
    auto n1 = (n.x*xv + n.y*yv + n.z*zv).normalized();

    buffer->addPoint(p1.x, p1.y, p1.z);
    buffer->addNormal(n1.getX(), n1.getY(), n1.getZ());
    buffer->addColor(c);
    buffer->addTexturePoint(tp.x, tp.y);
  };

  auto tp1 = CPoint2D(0.0, 0.0);
  auto tp2 = CPoint2D(1.0, 0.0);
  auto tp3 = CPoint2D(1.0, 1.0);
  auto tp4 = CPoint2D(0.0, 1.0);

  for (GLint i = 5; i >= 0; i--) {
    CQCamera3DFaceData faceData;

    faceData.pos = faceDataList.pos;
    faceData.len = 4;

    auto p1 = cubePoint(i, 0);
    auto p2 = cubePoint(i, 1);
    auto p3 = cubePoint(i, 2);
    auto p4 = cubePoint(i, 3);

    auto normal = cubeNormal(i);

    addPoint(p1, normal, data.color, tp1);
    addPoint(p2, normal, data.color, tp2);
    addPoint(p3, normal, data.color, tp3);
    addPoint(p4, normal, data.color, tp4);

    faceDataList.faceDatas.push_back(faceData);

    faceDataList.pos += faceData.len;
  }
}

void
addCone(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, double w,
        const ShapeData &data, CQCamera3DFaceDataList &faceDataList)
{
  auto *object = CGeometry3DInst->createObject3D(nullptr, "");

  auto h = p1.distanceTo(p2);

  CGeomCone3D::addGeometry(object, CPoint3D(0.0, 0.0, 0.0), w, h, 20);
  CGeomCone3D::addNormals(object, w, h);

  //---

  auto mt = CMatrix3D::translation(p1.x, p1.y, p1.z);
  auto mr = getShapeRotationMatrix(p1, p2);
  auto ms = CMatrix3D::scale(data.size.x, data.size.y, data.size.z);

  auto mt1 = CMatrix3D::translation(0, h/2.0, 0.0);

  auto modelMatrix  = mt*mr*ms*mt1;
  auto normalMatrix = modelMatrix.inverse().transposed();

  //---

  auto addPoint = [&](const CPoint3D &p, const CVector3D &n, const CRGBA &c, const CPoint2D &tp) {
    auto p1 = modelMatrix *p;
    auto n1 = (normalMatrix*n).normalize();

    buffer->addPoint(p1.x, p1.y, p1.z);
    buffer->addNormal(n1.getX(), n1.getY(), n1.getZ());
    buffer->addColor(c);
    buffer->addTexturePoint(tp.x, tp.y);
  };

  for (auto *face : object->getFaces()) {
    const auto &vertices = face->getVertices();

    CQCamera3DFaceData faceData;

    faceData.pos = faceDataList.pos;
    faceData.len = vertices.size();

    for (auto vertexInd : vertices) {
      auto *vertex = object->getVertexP(vertexInd);

      addPoint(vertex->getModel(), vertex->getNormal(), data.color, vertex->getTextureMap());
    }

    faceDataList.faceDatas.push_back(faceData);

    faceDataList.pos += faceData.len;
  }

  delete object;
}

void
addCylinder(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, double r,
            const ShapeData &data, CQCamera3DFaceDataList &faceDataList)
{
  auto *object = CGeometry3DInst->createObject3D(nullptr, "");

  auto h = p1.distanceTo(p2);

  CGeomCylinder3D::addGeometry(object, CPoint3D(0.0, 0.0, 0.0), 2*r, h, 20);
  CGeomCylinder3D::addNormals(object, 2*r, h);

  //---

  auto mt = CMatrix3D::translation(p1.x, p1.y, p1.z);
  auto mr = getShapeRotationMatrix(p1, p2);
  auto ms = CMatrix3D::scale(data.size.x, data.size.y, data.size.z);

  auto mt1 = CMatrix3D::translation(0.0, h/2.0, 0.0);

  auto modelMatrix  = mt*mr*ms*mt1;
  auto normalMatrix = modelMatrix.inverse().transposed();

  //---

  auto addPoint = [&](const CPoint3D &p, const CVector3D &n, const CRGBA &c, const CPoint2D &tp) {
    auto p1 = modelMatrix *p;
    auto n1 = (normalMatrix*n).normalize();

    buffer->addPoint(p1.x, p1.y, p1.z);
    buffer->addNormal(n1.getX(), n1.getY(), n1.getZ());
    buffer->addColor(c);
    buffer->addTexturePoint(tp.x, tp.y);
  };

  for (auto *face : object->getFaces()) {
    const auto &vertices = face->getVertices();

    CQCamera3DFaceData faceData;

    faceData.pos = faceDataList.pos;
    faceData.len = vertices.size();

    for (auto vertexInd : vertices) {
      auto *vertex = object->getVertexP(vertexInd);

      addPoint(vertex->getModel(), vertex->getNormal(), data.color, vertex->getTextureMap());
    }

    faceDataList.faceDatas.push_back(faceData);

    faceDataList.pos += faceData.len;
  }

  delete object;
}

void
addSphere(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, const ShapeData &data,
          double angleStart, double angleDelta, CQCamera3DFaceDataList &faceDataList)
{
  auto *object = CGeometry3DInst->createObject3D(nullptr, "");

  auto r = p1.distanceTo(p2)/2.0;

  // sphere of radius r, centerered at (0, 0, 0)
  CGeomSphere3D::ConfigData configData;
  configData.angleStart = angleStart;
  configData.angleDelta = angleDelta;

  CGeomSphere3D::addGeometry(object, CPoint3D(0, 0, 0), r, configData);
  CGeomSphere3D::addNormals(object, r);
  CGeomSphere3D::addTexturePoints(object);

  //---

  // transform to specified size and center
  auto mt = CMatrix3D::translation(p1.x, p1.y, p1.z);
  auto mr = getShapeRotationMatrix(p1, p2);
  auto ms = CMatrix3D::scale(data.size.x, data.size.y, data.size.z);

  auto modelMatrix  = mt*mr*ms;
  auto normalMatrix = modelMatrix.inverse().transposed();

  //---

  auto addPoint = [&](const CPoint3D &p, const CVector3D &n, const CRGBA &c, const CPoint2D &tp) {
    auto p1 = modelMatrix *p;
    auto n1 = (normalMatrix*n).normalize();

    buffer->addPoint(p1.x, p1.y, p1.z);
    buffer->addNormal(n1.getX(), n1.getY(), n1.getZ());
    buffer->addColor(c);
    buffer->addTexturePoint(tp.x, tp.y);
  };

  for (auto *face : object->getFaces()) {
    const auto &vertices = face->getVertices();

    CQCamera3DFaceData faceData;

    faceData.pos = faceDataList.pos;
    faceData.len = vertices.size();

    const auto &texturePoints = face->getTexturePoints();

    size_t iv = 0;

    for (auto vertexInd : vertices) {
      CPoint2D tp;

      if (iv < texturePoints.size())
        tp = texturePoints[iv];

      auto *vertex = object->getVertexP(vertexInd);

      addPoint(vertex->getModel(), vertex->getNormal(), data.color, tp);

      ++iv;
    }

    faceDataList.faceDatas.push_back(faceData);

    faceDataList.pos += faceData.len;
  }

  delete object;
}

void
addPyramid(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, double w,
           const ShapeData &data, CQCamera3DFaceDataList &faceDataList)
{
  auto *object = CGeometry3DInst->createObject3D(nullptr, "");

  auto h = p1.distanceTo(p2);

  CGeomPyramid3D::addGeometry(object, CPoint3D(0.0, 0.0, 0.0), w, h);

  //---

  auto mt = CMatrix3D::translation(p1.x, p1.y, p1.z);
  auto mr = getShapeRotationMatrix(p1, p2);
  auto ms = CMatrix3D::scale(data.size.x, data.size.y, data.size.z);

  auto mt1 = CMatrix3D::translation(0.0, h/2.0, 0.0);

  auto modelMatrix  = mt*mr*ms*mt1;
  auto normalMatrix = modelMatrix.inverse().transposed();

  //---

  auto addPoint = [&](const CPoint3D &p, const CVector3D &n, const CRGBA &c, const CPoint2D &tp) {
    auto p1 = modelMatrix*p;
    auto n1 = (normalMatrix*n).normalize();

    buffer->addPoint(p1.x, p1.y, p1.z);
    buffer->addNormal(n1.getX(), n1.getY(), n1.getZ());
    buffer->addColor(c);
    buffer->addTexturePoint(tp.x, tp.y);
  };

  for (auto *face : object->getFaces()) {
    const auto &vertices = face->getVertices();

    CQCamera3DFaceData faceData;

    faceData.pos = faceDataList.pos;
    faceData.len = vertices.size();

    for (auto vertexInd : vertices) {
      auto *vertex = object->getVertexP(vertexInd);

      addPoint(vertex->getModel(), vertex->getNormal(), data.color, vertex->getTextureMap());
    }

    faceDataList.faceDatas.push_back(faceData);

    faceDataList.pos += faceData.len;
  }

  delete object;
}

void
addTorus(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, double rfactor,
         double power1, double power2, const ShapeData &data, CQCamera3DFaceDataList &faceDataList)
{
  auto *object = CGeometry3DInst->createObject3D(nullptr, "");

  auto r0 = p1.distanceTo(p2);
  auto r1 = rfactor*r0;

  int nu = 30;
  int nv = 30;
  CGeomTorus3D::addGeometry(object, CPoint3D(0, 0, 0), r0, r1, power1, power2, nu, nv);

  auto mt = CMatrix3D::translation(p1.x, p1.y, p1.z);
  auto mr = getShapeRotationMatrix(p1, p2);
  auto ms = CMatrix3D::scale(data.size.x, data.size.y, data.size.z);

  auto modelMatrix  = mt*mr*ms;
  auto normalMatrix = modelMatrix.inverse().transposed();

  auto addPoint = [&](const CPoint3D &p, const CVector3D &n, const CRGBA &c, const CPoint2D &tp) {
    auto p1 = modelMatrix*p;
    auto n1 = (normalMatrix*n).normalize();

    buffer->addPoint(p1.x, p1.y, p1.z);
    buffer->addNormal(n1.getX(), n1.getY(), n1.getZ());
    buffer->addColor(c);
    buffer->addTexturePoint(tp.x, tp.y);
  };

  for (auto *face : object->getFaces()) {
    const auto &vertices = face->getVertices();

    CQCamera3DFaceData faceData;

    faceData.pos = faceDataList.pos;
    faceData.len = vertices.size();

    const auto &texturePoints = face->getTexturePoints();

    size_t iv = 0;

    for (auto vertexInd : vertices) {
      CPoint2D tp;

      if (iv < texturePoints.size())
        tp = texturePoints[iv];

      auto *vertex = object->getVertexP(vertexInd);

      addPoint(vertex->getModel(), vertex->getNormal(), data.color, tp);

      ++iv;
    }

    faceDataList.faceDatas.push_back(faceData);

    faceDataList.pos += faceData.len;
  }

  delete object;
}

CMatrix3D
getShapeRotationMatrix(const CPoint3D &p1, const CPoint3D &p2)
{
  auto v1 = CVector3D(0, 1, 0); v1.normalize();
  auto v2 = CVector3D(p1, p2) ; v2.normalize();

  auto q = CQuaternion::rotationArc(v1, v2);

  CMatrix3D m;
  q.toRotationMatrix(m);

  return m;
}

}
