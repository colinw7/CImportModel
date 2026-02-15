#ifndef shapes3d_H
#define shapes3d_H

namespace {

class MapFn3D {
 public:
  MapFn3D() { }

  virtual ~MapFn3D() { }

  virtual double exec(const vec3 &pos) const = 0;

  // https://iquilezles.org/articles/normalsSDF
  vec3 calcNormal(vec3 pos) {
    vec2 e = vec2(1.0, -1.0)*0.5773;

    const double eps = 0.0005;

    return normalize(xyy(e)*exec(pos + xyy(e)*eps) +
                     yyx(e)*exec(pos + yyx(e)*eps) +
                     yxy(e)*exec(pos + yxy(e)*eps) +
                     xxx(e)*exec(pos + xxx(e)*eps));
  }

  double calcSoftshadow(const vec3 &ro, const vec3 &rd, float tmin, float tmax, float k) {
    float res = 1.0;
    float t   = tmin;

    for (int i = 0; i < 50; ++i) {
      float h = exec(ro + rd*t);

      res = min(res, k*h/t);

      t += clamp(h, 0.02, 0.20);

      if (res < 0.005 || t > tmax)
        break;
    }

    return clamp( res, 0.0, 1.0 );
  }

 private:
  vec3 xyy(const vec2 &v) { return vec3(v.x, v.y, v.y); }
  vec3 yxy(const vec2 &v) { return vec3(v.y, v.x, v.y); }
  vec3 yyx(const vec2 &v) { return vec3(v.y, v.y, v.x); }
  vec3 xxx(const vec2 &v) { return vec3(v.x, v.x, v.x); }
};

}

//---

// 3d shapes
namespace {

float sdSphere(vec3 p, float r) {
  return length(p) - r;
}

float sdBox(vec3 p, vec3 b) {
  vec3 q = abs(p) - b;
  return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float sdRoundBox(vec3 p, vec3 b, float r) {
  vec3 q = abs(p) - b + r;
  return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0) - r;
}

float sdBoxFrame(vec3 p, vec3 b, float e) {
       p = abs(p    ) - b;
  vec3 q = abs(p + e) - e;

  return min(min(
      length(max(vec3(p.x, q.y, q.z), 0.0)) + min(max(p.x, max(q.y, q.z)), 0.0),
      length(max(vec3(q.x, p.y, q.z), 0.0)) + min(max(q.x, max(p.y, q.z)), 0.0)),
      length(max(vec3(q.x, q.y, p.z), 0.0)) + min(max(q.x, max(q.y, p.z)), 0.0));
}

float sdTorus(vec3 p, vec2 t) {
  vec2 q = vec2(length(p.xz()) - t.x, p.y);

  return length(q) - t.y;
}

float sdCappedTorus(vec3 p, vec2 sc, float ra, float rb) {
  p.x = abs(p.x);
  float k = (sc.y*p.x > sc.x*p.y) ? dot(p.xy(), sc) : length(p.xy());
  return sqrt(dot(p, p) + ra*ra - 2.0*ra*k) - rb;
}

float sdLink(vec3 p, float le, float r1, float r2) {
  //vec3 q = vec3(p.x, max(abs(p.y) - le, 0.0), p.z);
  vec3 q = vec3(p.x, p.y - clamp(p.y, -le, le), p.z);
  return length(vec2(length(q.xy()) - r1, q.z)) - r2;
}

float sdCylinder(vec3 p, vec3 c) {
  return length(p.xz() - c.xy()) - c.z;
}

float sdCone(vec3 p, vec2 c, float h) {
  // c is the sin/cos of the angle, h is height
  // Alternatively pass q instead of (c, h),
  // which is the point at the base in 2D
  vec2 q = h*vec2(c.x/c.y, -1.0);

  vec2 w = vec2(length(p.xz()), p.y);
  vec2 a = w - q*clamp(dot(w, q)/dot(q, q), 0.0, 1.0);
  vec2 b = w - q*vec2(clamp(w.x/q.x, 0.0, 1.0), 1.0);
  float k = sign(q.y);
  float d = min(dot(a, a), dot(b, b));
  float s = max(k*(w.x*q.y - w.y*q.x), k*(w.y - q.y));
  return sqrt(d)*sign(s);
}

#if 0
float sdInfCone(vec3 p, vec2 c) {
  // c is the sin/cos of the angle
  vec2 q = vec2(length(p.xz()), -p.y);
  float d = length(q - c*max(dot(q, c), 0.0));
  return d * ((q.x*c.y - q.y*c.x<0.0) ? -1.0 : 1.0);
}
#endif

float sdPlane(vec3 p, vec3 n, float h) {
  // n must be normalized
  return dot(p, n) + h;
}

float sdHexPrism(vec3 p, vec2 h) {
  const vec3 k = vec3(-0.8660254, 0.5, 0.57735);
  p = abs(p);
  p.setXY(p.xy() - 2.0*min(dot(k.xy(), p.xy()), 0.0)*k.xy());
  vec2 d = vec2(length(p.xy() - vec2(clamp(p.x, -k.z*h.x, k.z*h.x), h.x))*sign(p.y - h.x),
                p.z - h.y);
  return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

float sdCapsule(vec3 p, vec3 a, vec3 b, float r) {
  vec3 pa = p - a, ba = b - a;
  float h = clamp(dot(pa, ba)/dot(ba, ba), 0.0, 1.0);
  return length(pa - ba*h) - r;
}

#if 0
float sdVerticalCapsule(vec3 p, float h, float r) {
  p.y -= clamp(p.y, 0.0, h);
  return length(p) - r;
}

float sdCappedCylinder(vec3 p, float r, float h) {
  vec2 d = abs(vec2(length(p.xz()), p.y)) - vec2(r, h);
  return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

float sdArbitraryCappedCylinder(vec3 p, vec3 a, vec3 b, float r) {
  vec3  ba = b - a;
  vec3  pa = p - a;
  float baba = dot(ba, ba);
  float paba = dot(pa, ba);
  float x = length(pa*baba - ba*paba) - r*baba;
  float y = abs(paba - baba*0.5) - baba*0.5;
  float x2 = x*x;
  float y2 = y*y*baba;
  float d = (max(x, y) < 0.0) ? -min(x2, y2) : (((x>0.0) ? x2 : 0.0) + ((y>0.0) ? y2 : 0.0));
  return sign(d)*sqrt(abs(d))/baba;
}

float sdRoundedCylinder(vec3 p, float ra, float rb, float h) {
  vec2 d = vec2(length(p.xz()) - ra + rb, abs(p.y) - h + rb);
  return min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - rb;
}

float sdCappedCone(vec3 p, float h, float r1, float r2) {
  vec2 q = vec2(length(p.xz()), p.y);
  vec2 k1 = vec2(r2, h);
  vec2 k2 = vec2(r2 - r1, 2.0*h);
  vec2 ca = vec2(q.x - min(q.x, (q.y<0.0)?r1:r2), abs(q.y) - h);
  vec2 cb = q - k1 + k2*clamp(dot(k1 - q, k2)/dot2(k2), 0.0, 1.0);
  float s = (cb.x<0.0 && ca.y<0.0) ? -1.0 : 1.0;
  return s*sqrt(min(dot2(ca), dot2(cb)));
}

float sdCappedCone(vec3 p, vec3 a, vec3 b, float ra, float rb) {
  float rba  = rb - ra;
  float baba = dot(b - a, b - a);
  float papa = dot(p - a, p - a);
  float paba = dot(p - a, b - a)/baba;
  float x = sqrt(papa - paba*paba*baba);
  float cax = max(0.0, x - ((paba<0.5)?ra:rb));
  float cay = abs(paba - 0.5) - 0.5;
  float k = rba*rba + baba;
  float f = clamp((rba*(x - ra) + paba*baba)/k, 0.0, 1.0);
  float cbx = x - ra - f*rba;
  float cby = paba - f;
  float s = (cbx<0.0 && cay<0.0) ? -1.0 : 1.0;
  return s*sqrt(min(cax*cax + cay*cay*baba, cbx*cbx + cby*cby*baba));
}

float sdSolidAngle(vec3 p, vec2 c, float ra) {
  // c is the sin/cos of the angle
  vec2 q = vec2(length(p.xz()), p.y);
  float l = length(q) - ra;
  float m = length(q - c*clamp(dot(q, c), 0.0, ra));
  return max(l, m*sign(c.y*q.x - c.x*q.y));
}

float sdCutSphere(vec3 p, float r, float h) {
  float w = sqrt(r*r - h*h);

  vec2 q = vec2(length(p.xz()), p.y);
  float s = max((h - r)*q.x*q.x + w*w*(h + r - 2.0*q.y), h*q.x - w*q.y);
  return (s  < 0.0) ? length(q) - r :
         (q.x< w  ) ? h - q.y       :
                      length(q - vec2(w, h));
}

float sdCutHollowSphere(vec3 p, float r, float h, float t) {
  float w = sqrt(r*r - h*h);
  vec2 q = vec2(length(p.xz()), p.y);
  return ((h*q.x<w*q.y) ? length(q - vec2(w, h)) : abs(length(q) - r)) - t;
}

float sdDeathStar(vec3 p2, float ra, float rb, float d) {
  float a = (ra*ra - rb*rb + d*d)/(2.0*d);
  float b = sqrt(max(ra*ra - a*a, 0.0));

  vec2 p = vec2(p2.x, length(p2.yz));
  if (p.x*b - p.y*a > d*max(b - p.y, 0.0))
    return length(p - vec2(a, b));
  else
    return max( (length(p               ) - ra),
               -(length(p - vec2(d, 0.0)) - rb));
}

float sdRoundCone(vec3 p, float r1, float r2, float h)
{
  float b = (r1 - r2)/h;
  float a = sqrt(1.0 - b*b);

  vec2 q = vec2(length(p.xz()), p.y);
  float k = dot(q, vec2(-b, a));
  if (k<0.0) return length(q) - r1;
  if (k>a*h) return length(q - vec2(0.0, h)) - r2;
  return dot(q, vec2(a, b)) - r1;
}

float sdRoundCone(vec3 p, vec3 a, vec3 b, float r1, float r2) {
  vec3  ba = b - a;
  float l2 = dot(ba, ba);
  float rr = r1 - r2;
  float a2 = l2 - rr*rr;
  float il2 = 1.0/l2;

  vec3 pa = p - a;
  float y = dot(pa, ba);
  float z = y - l2;
  float x2 = dot2(pa*l2 - ba*y);
  float y2 = y*y*l2;
  float z2 = z*z*l2;

  // single square root!
  float k = sign(rr)*rr*rr*x2;
  if (sign(z)*a2*z2>k) return  sqrt(x2 + z2)          *il2 - r2;
  if (sign(y)*a2*y2<k) return  sqrt(x2 + y2)          *il2 - r1;
                       return (sqrt(x2*a2*il2) + y*rr)*il2 - r1;
}

float sdVesicaSegment(vec3 p, vec3 a, vec3 b, float w) {
  vec3  c = (a + b)*0.5;
  float l = length(b - a);
  vec3  v = (b - a)/l;
  float y = dot(p - c, v);
  vec2  q = vec2(length(p - c -y*v), abs(y));

  float r = 0.5*l;
  float d = 0.5*(r*r - w*w)/w;
  vec3  h = (r*q.x < d*(q.y - r)) ? vec3(0.0, r, 0.0) : vec3(-d, 0.0, d + w);

  return length(q - h.xy()) - h.z;
}

float sdRhombus(vec3 p, float la, float lb, float h, float ra) {
  p = abs(p);
  float f = clamp((la*p.x - lb*p.z + lb*lb)/(la*la + lb*lb), 0.0, 1.0);
  vec2  w = p.xz() - vec2(la, lb)*vec2(f, 1.0 - f);
  vec2  q = vec2(length(w)*sign(w.x) - ra, p.y - h);
  return min(max(q.x, q.y), 0.0) + length(max(q, 0.0));
}

float sdOctahedron(vec3 p, float s) {
  p = abs(p);
  float m = p.x + p.y + p.z - s;
  vec3 q;
       if (3.0*p.x < m) q = p.xyz();
  else if (3.0*p.y < m) q = p.yzx();
  else if (3.0*p.z < m) q = p.zxy();
  else return m*0.57735027;

  float k = clamp(0.5*(q.z - q.y + s), 0.0, s);
  return length(vec3(q.x, q.y - s + k, q.z - k));
}

float sdOctahedron(vec3 p, float s) {
  p = abs(p);
  return (p.x + p.y + p.z - s)*0.57735027;
}

float sdPyramid(vec3 p, float h) {
  float m2 = h*h + 0.25;

  p.xz = abs(p.xz());
  p.xz = (p.z>p.x) ? p.zx : p.xz();
  p.xz -= 0.5;

  vec3 q = vec3(p.z, h*p.y - 0.5*p.x, h*p.x + 0.5*p.y);
  float s = max(-q.x, 0.0);
  float t = clamp((q.y - 0.5*p.z)/(m2 + 0.25), 0.0, 1.0);
  float a = m2*(q.x + s)*(q.x + s) + q.y*q.y;
  float b = m2*(q.x + 0.5*t)*(q.x + 0.5*t) + (q.y - m2*t)*(q.y - m2*t);

  float d2 = min(q.y, -q.x*m2 - q.y*0.5) > 0.0 ? 0.0 : min(a, b);
  return sqrt((d2 + q.z*q.z)/m2) * sign(max(q.z, -p.y));
}

float udTriangle(vec3 p, vec3 a, vec3 b, vec3 c) {
  vec3 ba = b - a; vec3 pa = p - a;
  vec3 cb = c - b; vec3 pb = p - b;
  vec3 ac = a - c; vec3 pc = p - c;
  vec3 nor = cross(ba, ac);

  return sqrt(
    (sign(dot(cross(ba, nor), pa)) +
     sign(dot(cross(cb, nor), pb)) +
     sign(dot(cross(ac, nor), pc))<2.0)
     ?
     min(min(
       dot2(ba*clamp(dot(ba, pa)/dot2(ba), 0.0, 1.0) - pa),
       dot2(cb*clamp(dot(cb, pb)/dot2(cb), 0.0, 1.0) - pb)),
       dot2(ac*clamp(dot(ac, pc)/dot2(ac), 0.0, 1.0) - pc)) :
       dot(nor, pa)*dot(nor, pa)/dot2(nor));
}

float udQuad(vec3 p, vec3 a, vec3 b, vec3 c, vec3 d) {
  vec3 ba = b - a; vec3 pa = p - a;
  vec3 cb = c - b; vec3 pb = p - b;
  vec3 dc = d - c; vec3 pc = p - c;
  vec3 ad = a - d; vec3 pd = p - d;
  vec3 nor = cross(ba, ad);

  return sqrt(
    (sign(dot(cross(ba, nor), pa)) +
     sign(dot(cross(cb, nor), pb)) +
     sign(dot(cross(dc, nor), pc)) +
     sign(dot(cross(ad, nor), pd))<3.0) ?
     min(min(min(
       dot2(ba*clamp(dot(ba, pa)/dot2(ba), 0.0, 1.0) - pa),
       dot2(cb*clamp(dot(cb, pb)/dot2(cb), 0.0, 1.0) - pb)),
       dot2(dc*clamp(dot(dc, pc)/dot2(dc), 0.0, 1.0) - pc)),
       dot2(ad*clamp(dot(ad, pd)/dot2(ad), 0.0, 1.0) - pd)) :
       dot(nor, pa)*dot(nor, pa)/dot2(nor));
}
#endif

float sdEllipsoid(vec3 p, vec3 r) {
  float k0 = length(p/r);
  float k1 = length(p/(r*r));
  return k0*(k0 - 1.0)/k1;
}

#if 0
float sdTriPrism(vec3 p, vec2 h) {
  vec3 q = abs(p);
  return max(q.z - h.y, max(q.x*0.866025 + p.y*0.5, -p.y) - h.x*0.5);
}
#endif

float opRevolution(vec3 p, const MapFn2D *primitive, float o) {
  vec2 q = vec2(length(p.xz()) - o, p.y);
  return primitive->exec(q);
}

float opExtrusion(vec3 p, const MapFn2D *primitive, float h) {
  float d = primitive->exec(p.xy());
  vec2 w = vec2(d, abs(p.z) - h);
  return min(max(w.x, w.y), 0.0) + length(max(w, 0.0));
}

float opElongate(const MapFn3D *primitive, vec3 p, vec3 h) {
  vec3 q = p - clamp(p, -h, h);
  return primitive->exec(q);
}

#if 0
float opElongate(const MapFn2D *primitive, vec3 p, vec3 h) {
  vec3 q = abs(p) - h;
  return primitive->exec(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float opRound(const MapFn2D *primitive, float rad) {
  return primitive->exec(p) - rad
}
#endif

float opOnion(float sdf, float thickness) {
  return abs(sdf) - thickness;
}

#if 0
float length2(vec3 p) {
  p=p*p;
  return sqrt(p.x + p.y + p.z);
}

float length6(vec3 p) {
  p=p*p*p; p=p*p; return
  pow(p.x + p.y + p.z, 1.0/6.0);
}

float length8(vec3 p) {
  p=p*p; p=p*p; p=p*p; return
  pow(p.x + p.y + p.z, 1.0/8.0);
}

float opUnion(float a, float b) {
  return min(a, b);
}

float opSubtraction(float a, float b) {
  return max(-a, b);
}

float opIntersection(float a, float b) {
  return max(a, b);
}

float opXor(float a, float b) {
  return max(min(a, b), -max(a, b));
}

float opSmoothUnion(float a, float b, float k) {
  k *= 4.0;
  float h = max(k - abs(a - b), 0.0);
  return min(a, b) - h*h*0.25/k;
}

float opSmoothSubtraction(float a, float b, float k) {
  return -opSmoothUnion(a, -b, k);

  // k *= 4.0;
  // float h = max(k - abs(-a - b), 0.0);
  // return max(-a, b) + h*h*0.25/k;
}

float opSmoothIntersection(float a, float b, float k) {
  return -opSmoothUnion(-a, -b, k);

  // k *= 4.0;
  // float h = max(k - abs(a - b), 0.0);
  // return max(a, b) + h*h*0.25/k;
}

vec3 opTx(vec3 p, transform t, sdf3d primitive) {
  return primitive(invert(t)*p);
}

float opScale(vec3 p, float s, sdf3d primitive) {
  return primitive(p/s)*s;
}

float opSymX(vec3 p, sdf3d primitive) {
  p.x = abs(p.x);
  return primitive(p);
}

float opSymXZ(vec3 p, sdf3d primitive) {
  p.xz = abs(p.xz());
  return primitive(p);
}

float opRepetition(vec3 p, vec3 s, sdf3d primitive) {
  vec3 q = p - s*round(p/s);
  return primitive(q);
}

vec3 opLimitedRepetition(vec3 p, float s, vec3 l, sdf3d primitive) {
  vec3 q = p - s*clamp(round(p/s), -l, l);
  return primitive(q);
}

float opDisplace(sdf3d primitive, vec3 p) {
  float d1 = primitive(p);
  float d2 = displacement(p);
  return d1 + d2;
}

float opTwist(sdf3d primitive, vec3 p) {
  const float k = 10.0; // or some other amount
  float c = cos(k*p.y);
  float s = sin(k*p.y);
  mat2  m = mat2(c, -s, s, c);
  vec3  q = vec3(m*p.xz(), p.y);
  return primitive(q);
}

float opCheapBend(sdf3d primitive, vec3 p) {
  const float k = 10.0; // or some other amount
  float c = cos(k*p.x);
  float s = sin(k*p.x);
  mat2  m = mat2(c, -s, s, c);
  vec3  q = vec3(m*p.xy, p.z);
  return primitive(q);
}
#endif

}

//---

namespace {

class SdSphereFn : public MapFn3D {
 public:
  SdSphereFn(double r) :
   r_(r) {
  }

  double exec(const vec3 &pos) const override { return sdSphere(pos, r_); }

 private:
  double r_ { 1.0 };
};

class SdBoxFn : public MapFn3D {
 public:
  SdBoxFn(const vec3 &b) :
   b_(b) {
  }

  double exec(const vec3 &pos) const override { return sdBox(pos, b_); }

 private:
  vec3 b_ { 1.0, 1.0, 1.0 };
};

class SdRoundBoxFn : public MapFn3D {
 public:
  SdRoundBoxFn(const vec3 &b, double r) :
   b_(b), r_(r) {
  }

  double exec(const vec3 &pos) const override { return sdRoundBox(pos, b_, r_); }

 private:
  vec3   b_ { 1.0, 1.0, 1.0 };
  double r_ { 1.0 };
};

class SdBoxFrameFn : public MapFn3D {
 public:
  SdBoxFrameFn(const vec3 &b, double r) :
   b_(b), r_(r) {
  }

  double exec(const vec3 &pos) const override { return sdBoxFrame(pos, b_, r_); }

 private:
  vec3   b_ { 1.0, 1.0, 1.0 };
  double r_ { 1.0 };
};

class SdTorusFn : public MapFn3D {
 public:
  SdTorusFn(const vec2 &t) :
   t_(t) {
  }

  double exec(const vec3 &pos) const override { return sdTorus(pos, t_); }

 private:
  vec2 t_ { 1.0, 1.0 };
};

class SdCappedTorusFn : public MapFn3D {
 public:
  SdCappedTorusFn(const vec2 &sc, double ra, double rb) :
   sc_(sc), ra_(ra), rb_(rb) {
  }

  double exec(const vec3 &pos) const override { return sdCappedTorus(pos, sc_, ra_, rb_); }

 private:
  vec2   sc_ { 1.0, 1.0 };
  double ra_ { 1.0 };
  double rb_ { 1.0 };
};

class SdLinkFn : public MapFn3D {
 public:
  SdLinkFn(double le, double r1, double r2) :
   le_(le), r1_(r1), r2_(r2) {
  }

  double exec(const vec3 &pos) const override { return sdLink(pos, le_, r1_, r2_); }

 private:
  double le_ { 0.0 };
  double r1_ { 0.1 };
  double r2_ { 0.2 };
};

class SdCylinderFn : public MapFn3D {
 public:
  SdCylinderFn(const vec3 &c) :
   c_(c) {
  }

  double exec(const vec3 &pos) const override { return sdCylinder(pos, c_); }

 private:
  vec3 c_ { 0, 0, 0 };
};

class SdConeFn : public MapFn3D {
 public:
  SdConeFn(const vec2 &c, double h) :
   c_(c), h_(h) {
  }

  double exec(const vec3 &pos) const override { return sdCone(pos, c_, h_); }

 private:
  vec2   c_ { 0, 0 };
  double h_ { 1 };
};

class SdPlaneFn : public MapFn3D {
 public:
  SdPlaneFn(const vec3 &n, double h) :
   n_(n), h_(h) {
  }

  double exec(const vec3 &pos) const override { return sdPlane(pos, n_, h_); }

 private:
  vec3   n_ { 0, 1, 0 };
  double h_ { 1 };
};

class SdHexPrismFn : public MapFn3D {
 public:
  SdHexPrismFn(const vec2 &h) :
   h_(h) {
  }

  double exec(const vec3 &pos) const override { return sdHexPrism(pos, h_); }

 private:
  vec2 h_ { 1, 1};
};

class SdCapsuleFn : public MapFn3D {
 public:
  SdCapsuleFn(const vec3 &a, const vec3 &b, double r) :
   a_(a), b_(b), r_(r) {
  }

  double exec(const vec3 &pos) const override { return sdCapsule(pos, a_, b_, r_); }

 private:
  vec3   a_ { 0, 0, 0 };
  vec3   b_ { 0, 0, 0 };
  double r_ { 1 };
};

class SdRotateCrossFn : public MapFn3D {
 public:
  SdRotateCrossFn(const vec2 &b, double r, double o) :
   crossFn_(b, r), o_(o) {
  }

  double exec(const vec3 &pos) const override {
    return opRevolution(pos, &crossFn_, o_);
  }

 private:
  SdCrossFn crossFn_;
  double    o_ { 1 };
};

class SdExtrudeCrossFn : public MapFn3D {
 public:
  SdExtrudeCrossFn(const vec2 &b, double r, double h) :
   crossFn_(b, r), h_(h) {
  }

  double exec(const vec3 &pos) const override {
    return opExtrusion(pos, &crossFn_, h_);
  }

 private:
  SdCrossFn crossFn_;
  double    h_ { 1 };
};

class SdEllipsoidFn : public MapFn3D {
 public:
  SdEllipsoidFn(const vec3 &r) :
   r_(r) {
  }

  double exec(const vec3 &pos) const override {
    return sdEllipsoid(pos, r_);
  }

 private:
  vec3 r_ { 1, 1, 1 };
};

class SdElongateEllipsoidFn : public MapFn3D {
 public:
  SdElongateEllipsoidFn(const vec3 &r, const vec3 &h) :
   ellipsoidFn_(r), h_(h) {
  }

  double exec(const vec3 &pos) const override {
    auto f1 = ellipsoidFn_.exec(pos - vec3(0, 0, -0.75));

    auto f2 = opElongate(&ellipsoidFn_, pos - vec3(0, 0, 0.75), h_);

    return min(f1, f2);
  }

 private:
  SdEllipsoidFn ellipsoidFn_;
  vec3          h_ { 1, 1, 1 };
};

class SdElongateTorusFn : public MapFn3D {
 public:
  SdElongateTorusFn(const vec2 &t, const vec3 &h) :
   torusFn_(t), h_(h) {
  }

  double exec(const vec3 &pos) const override {
    auto f1 = torusFn_.exec(pos - vec3(0, 0, -0.75));

    auto f2 = opElongate(&torusFn_, pos - vec3(0, 0, 0.75), h_);

    return min(f1, f2);
  }

 private:
  SdTorusFn torusFn_;
  vec3      h_ { 1, 1, 1 };
};

class SdOnionSphereFn : public MapFn3D {
 public:
  SdOnionSphereFn(double r) :
   sphereFn_(r) {
  }

  double exec(const vec3 &pos) const override {
    auto f1 = sphereFn_.exec(pos - vec3(0, 0, -0.75));

    auto f2 = opOnion(opOnion(sphereFn_.exec(pos - vec3(0, 0, 0.75)), 0.05), 0.02);

    return min(f1, f2);
  }

 private:
  SdSphereFn sphereFn_;
};

static SdSphereFn      sdSphereFn     (0.3);
static SdBoxFn         sdBoxFn        (vec3(0.1, 0.2, 0.3));
static SdRoundBoxFn    sdRoundBoxFn   (vec3(0.1, 0.2, 0.3), 0.1);
static SdBoxFrameFn    sdBoxFrameFn   (vec3(0.1, 0.2, 0.3), 0.01);
static SdTorusFn       sdTorusFn      (vec2(0.2, 0.1));
static SdCappedTorusFn sdCappedTorusFn(vec2(0.2, 0.1), 0.2, 0.1);
static SdLinkFn        sdLinkFn       (0.13, 0.2, 0.09);
static SdCylinderFn    sdCylinderFn   (vec3(0.1, 0.2, 0.3));
static SdConeFn        sdConeFn       (vec2(0.1, 0.2), 0.5);
static SdPlaneFn       sdPlaneFn      (vec3(1, 2, 3).normalize(), 0.1);
static SdHexPrismFn    sdHexPrismFn   (vec2(0.4, 0.1));
static SdCapsuleFn     sdCapsuleFn    (vec3(0.1, 0.2, 0.3), vec3(0.2, 0.4, 0.1), 0.5);

static SdRotateCrossFn  sdRotateCrossFn (vec2(0.8, 0.35), 0.2, 0.4);
static SdExtrudeCrossFn sdExtrudeCrossFn(vec2(0.8, 0.35), 0.2, 0.4);

static SdElongateEllipsoidFn sdElongateEllipsoidFn(vec3(0.4, 0.2, 0.1), vec3(0.2, 0.0, 0.3));
static SdElongateTorusFn     sdElongateTorusFn    (vec2(0.4, 0.05), vec3(0.2, 0.0, 0.3));

static SdOnionSphereFn sdOnionSphereFn(0.4);

}

#endif
