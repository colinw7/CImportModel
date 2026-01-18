#ifndef CQCamera3DFaceData_H
#define CQCamera3DFaceData_H

#include <CRGBA.h>
#include <CPoint3D.h>
#include <vector>

class CGeomFace3D;
class CGeomLine3D;

class CQGLTexture;

struct CQCamera3DFaceData {
  CGeomFace3D*     face            { nullptr };
  CGeomLine3D*     line            { nullptr };
  bool             selected        { false };
  bool             visible         { true };
  bool             wireframe       { false };
  bool             lines           { false };
  int              pos             { 0 };
  int              len             { 0 };
  CPoint3D         position        { 0, 0, 0 };
  CQGLTexture*     diffuseTexture  { nullptr };
  CQGLTexture*     normalTexture   { nullptr };
  CQGLTexture*     specularTexture { nullptr };
  CQGLTexture*     emissiveTexture { nullptr };
  CRGBA            ambient         { 0, 0, 0 };
  CRGBA            diffuse         { 1, 1, 1 };
  CRGBA            specular        { 0, 0, 0 };
  CRGBA            emission        { 0, 0, 0, 0 };
  double           shininess       { 1.0 };
  std::vector<int> vertices;
};

struct CQCamera3DFaceDataList {
  void clear() { faceDatas.clear(); pos = 0; }

  void add(const CQCamera3DFaceDataList &faceDataList) {
    for (const auto &faceData : faceDataList.faceDatas)
      faceDatas.push_back(faceData);
  }

  int                             pos { 0 };
  std::vector<CQCamera3DFaceData> faceDatas;
};

#endif
