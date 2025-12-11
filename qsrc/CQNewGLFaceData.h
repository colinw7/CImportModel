#ifndef CQNewGLFaceData_H
#define CQNewGLFaceData_H

#include <CRGBA.h>
#include <CPoint3D.h>
#include <vector>

class CQGLTexture;

struct CQNewGLFaceData {
  enum class Type {
    NONE,
    CUBE,
    LINE
  };

  Type         type            { Type::NONE };
  bool         selected        { false };
  int          boneId          { -1 };
  int          parentBoneId    { -1 };
  int          pos             { 0 };
  int          len             { 0 };
  CPoint3D     position        { 0, 0, 0 };
  CQGLTexture *diffuseTexture  { nullptr };
  CQGLTexture *specularTexture { nullptr };
  CQGLTexture *normalTexture   { nullptr };
  CQGLTexture *emissiveTexture { nullptr };
  CRGBA        ambient         { 0, 0, 0 };
  CRGBA        diffuse         { 1, 1, 1 };
  CRGBA        specular        { 0, 0, 0 };
  CRGBA        emission        { 0, 0, 0, 0 };
  double       shininess       { 1.0 };
};

struct CQNewGLFaceDataList {
  void clear() { faceDatas.clear(); pos = 0; }

  void add(const CQNewGLFaceDataList &faceDataList) {
    for (const auto &faceData : faceDataList.faceDatas)
      faceDatas.push_back(faceData);
  }

  int                          pos { 0 };
  std::vector<CQNewGLFaceData> faceDatas;
};

#endif
