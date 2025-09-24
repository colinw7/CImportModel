#ifndef CQNewGLFaceData_H
#define CQNewGLFaceData_H

#include <CRGBA.h>
#include <CPoint3D.h>

class CQGLTexture;

struct CQNewGLFaceData {
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

#endif
