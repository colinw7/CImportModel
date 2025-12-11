#ifndef CQNewGLShapes_H
#define CQNewGLShapes_H

#include <CQNewGLFaceData.h>
#include <CMatrix3D.h>
#include <CBBox3D.h>

class CQGLBuffer;

namespace CQNewGLShapes {

struct ShapeData {
  CRGBA    color { 1, 1, 1};
  CPoint3D size  { 1, 1, 1};
};

void addCube(CQGLBuffer *buffer, const CBBox3D &bbox, const ShapeData &data,
             CQNewGLFaceDataList &faceDataList);
void addCube(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, const ShapeData &data,
             CQNewGLFaceDataList &faceDataList);

void addCone(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, double w,
             const ShapeData &data, CQNewGLFaceDataList &faceDataList);

void addCylinder(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, double r,
                 const ShapeData &data, CQNewGLFaceDataList &faceDataList);

void addSphere(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, const ShapeData &data,
               double angleStart, double angleDelta, CQNewGLFaceDataList &faceDataList);

void addPyramid(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, double w,
                const ShapeData &data, CQNewGLFaceDataList &faceDataList);

void addTorus(CQGLBuffer *buffer, const CPoint3D &p1, const CPoint3D &p2, double r1,
              double power1, double power2, const ShapeData &data,
              CQNewGLFaceDataList &faceDataList);

CMatrix3D getShapeRotationMatrix(const CPoint3D &p1, const CPoint3D &p2);

}

#endif
