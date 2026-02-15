#ifndef Shapes_H
#define Shapes_H

#include <FaceData.h>
#include <CMatrix3D.h>
#include <CBBox3D.h>
#include <CRGBA.h>

class CQGLBuffer;

namespace Shapes {

struct ShapeData {
  CRGBA    color { 1, 1, 1};
  CPoint3D size  { 1, 1, 1};
};

void addCube(CQGLBuffer *buffer, const CBBox3D &bbox, const ShapeData &data,
             FaceDataList &faceDataList);
void addCube(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, const ShapeData &data,
             FaceDataList &faceDataList);
void addCube(CQGLBuffer *buffer, const CMatrix3D &m1, const CMatrix3D &m2, const CMatrix3D &m3,
             const ShapeData &data, FaceDataList &faceDataList);

void addOrientBox(CQGLBuffer *buffer, const CPoint3D &c, const CVector3D &xv,
                  const CVector3D &yv, const CVector3D &zv,
                  const ShapeData &data, FaceDataList &faceDataList);

void addCone(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, double w,
             const ShapeData &data, FaceDataList &faceDataList);

void addCylinder(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, double r,
                 const ShapeData &data, FaceDataList &faceDataList);

void addSphere(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, const ShapeData &data,
               double angleStart, double angleDelta, FaceDataList &faceDataList);

void addPyramid(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, double w,
                const ShapeData &data, FaceDataList &faceDataList);

void addTorus(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, double r1,
              double power1, double power2, const ShapeData &data,
              FaceDataList &faceDataList);

void addPlane(CQGLBuffer *buffer, const CBBox3D &bbox, const ShapeData &data,
              FaceDataList &faceDataList);

CMatrix3D getShapeRotationMatrix(const CPoint3D &p1, const CPoint3D &p2);

}

#endif
