#ifndef CQCamera3DShapes_H
#define CQCamera3DShapes_H

#include <CQCamera3DFaceData.h>
#include <CMatrix3D.h>
#include <CBBox3D.h>

class CQGLBuffer;

namespace CQCamera3DShapes {

struct ShapeData {
  CRGBA    color       { 1, 1, 1 };
  CPoint3D size        { 1, 1, 1 };
  double   alpha       { 1 };
  int      num_patches { -1 };
};

void addCube(CQGLBuffer *buffer, const CBBox3D &bbox, const ShapeData &data,
             CQCamera3DFaceDataList &faceDataList);
void addCube(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, const ShapeData &data,
             CQCamera3DFaceDataList &faceDataList);
void addCube(CQGLBuffer *buffer, const CMatrix3D &m1, const CMatrix3D &m2, const CMatrix3D &m3,
             const ShapeData &data, CQCamera3DFaceDataList &faceDataList);

void addOrientBox(CQGLBuffer *buffer, const CPoint3D &c, const CVector3D &xv,
                  const CVector3D &yv, const CVector3D &zv,
                  const ShapeData &data, CQCamera3DFaceDataList &faceDataList);

void addCone(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, double w,
             const ShapeData &data, CQCamera3DFaceDataList &faceDataList);

void addCylinder(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, double r,
                 const ShapeData &data, CQCamera3DFaceDataList &faceDataList);

void addSphere(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, const ShapeData &data,
               double angleStart, double angleDelta, CQCamera3DFaceDataList &faceDataList);

void addPyramid(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, double w,
                const ShapeData &data, CQCamera3DFaceDataList &faceDataList);

void addTorus(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, double r1,
              double power1, double power2, const ShapeData &data,
              CQCamera3DFaceDataList &faceDataList);

CMatrix3D getShapeRotationMatrix(const CPoint3D &p1, const CPoint3D &p2);

}

#endif
