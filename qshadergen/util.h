#ifndef util_H
#define util_H

#include <CVector3D.h>

#include <vec2.h>
#include <vec3.h>

namespace {

float abs(float f) {
  return std::abs(f);
}

vec2 abs(vec2 p) {
  return p.abs();
}

vec3 abs(vec3 p) {
  return p.abs();
}

double sqrt(double f) {
  return std::sqrt(f);
}

vec3 sqrt(vec3 p) {
  return p.sqrt();
}

double sign(double f) {
  return (f >= 0 ? 1.0 : -1.0);
}

#if 0
vec3 min(vec3 p, double f) {
  return p.min(f);
}
#endif

double min(double a, double b) {
  return std::min(a, b);
}

vec3 max(vec3 p, double f) {
  return p.max(f);
}

vec2 max(vec2 p, double f) {
  return p.max(f);
}

double max(double a, double b) {
  return std::max(a, b);
}

double length(vec3 p) {
  return p.length();
}

double length(vec2 p) {
  return p.length();
}

double dot(vec3 v1, vec3 v2) {
  return v1.dot(v2);
}

double dot(vec2 v1, vec2 v2) {
  return v1.dot(v2);
}

double dot2(vec2 v) {
  return dot(v, v);
}

double clamp(double val, double low, double high) {
  if (val < low ) return low;
  if (val > high) return high;
  return val;
}

vec3 clamp(const vec3 &val, const vec3 &low, const vec3 &high) {
  return vec3(clamp(val.x, low.x, high.x),
              clamp(val.y, low.y, high.y),
              clamp(val.z, low.z, high.z));
}

vec3 normalize(const vec3 &v) {
  return v.normalize();
}

vec3 toVec3(const CPoint3D &p) {
  return vec3(p.x, p.y, p.z);
}

CVector3D toVector3D(const vec3 &v) {
  return CVector3D(v.x, v.y, v.z);
}

QColor vec3ToQColor(const vec3 &v) {
  return QColor(255*std::min(v.x, 1.0), 255*std::min(v.y, 1.0), 255*std::min(v.z, 1.0));
}

}

#endif
