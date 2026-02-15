#include <Canvas.h>
#include <Camera.h>

#include <CVector3D.h>
#include <vec2.h>
#include <vec3.h>

#include <QPainter>
#include <QTimer>

namespace {

float abs(float f) {
  return std::abs(f);
}

vec3 abs(vec3 p) {
  return p.abs();
}

#if 0
vec3 min(vec3 p, float f) {
  return p.min(f);
}
#endif

vec3 max(vec3 p, float f) {
  return p.max(f);
}

float min(float a, float b) {
  return std::min(a, b);
}

float max(float a, float b) {
  return std::max(a, b);
}

float length(vec3 p) {
  return p.length();
}

float length(vec2 p) {
  return p.length();
}

float dot(vec3 v1, vec3 v2) {
  return v1.dot(v2);
}

float dot(vec2 v1, vec2 v2) {
  return v1.dot(v2);
}

//---

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
  vec3 q = vec3(p.x, max(abs(p.y)-le, 0.0), p.z);
  return length(vec2(length(q.xy())-r1, q.z)) - r2;
}

#if 0
float sdCylinder(vec3 p, vec3 c) {
  return length(p.xz-c.xy())-c.z;
}

float sdCone(vec3 p, vec2 c, float h)
{
  // c is the sin/cos of the angle, h is height
  // Alternatively pass q instead of (c, h),
  // which is the point at the base in 2D
  vec2 q = h*vec2(c.x/c.y, -1.0);

  vec2 w = vec2(length(p.xz), p.y);
  vec2 a = w - q*clamp(dot(w, q)/dot(q, q), 0.0, 1.0);
  vec2 b = w - q*vec2(clamp(w.x/q.x, 0.0, 1.0), 1.0);
  float k = sign(q.y);
  float d = min(dot(a, a), dot(b, b));
  float s = max(k*(w.x*q.y-w.y*q.x), k*(w.y-q.y));
  return sqrt(d)*sign(s);
}

float sdCone(vec3 p, vec2 c)
{
  // c is the sin/cos of the angle
  vec2 q = vec2(length(p.xz), -p.y);
  float d = length(q-c*max(dot(q, c), 0.0));
  return d * ((q.x*c.y-q.y*c.x<0.0)?-1.0:1.0);
}

float sdPlane(vec3 p, vec3 n, float h)
{
  // n must be normalized
  return dot(p, n) + h;
}

float sdHexPrism(vec3 p, vec2 h)
{
  const vec3 k = vec3(-0.8660254, 0.5, 0.57735);
  p = abs(p);
  p.xy -= 2.0*min(dot(k.xy(), p.xy()), 0.0)*k.xy();
  vec2 d = vec2(
       length(p.xy() - vec2(clamp(p.x, -k.z*h.x, k.z*h.x), h.x))*sign(p.y-h.x),
       p.z-h.y);
  return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

float sdCapsule(vec3 p, vec3 a, vec3 b, float r)
{
  vec3 pa = p - a, ba = b - a;
  float h = clamp(dot(pa, ba)/dot(ba, ba), 0.0, 1.0);
  return length(pa - ba*h) - r;
}

float sdVerticalCapsule(vec3 p, float h, float r)
{
  p.y -= clamp(p.y, 0.0, h);
  return length(p) - r;
}

float sdCappedCylinder(vec3 p, float r, float h)
{
  vec2 d = abs(vec2(length(p.xz), p.y)) - vec2(r, h);
  return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

float sdCappedCylinder(vec3 p, vec3 a, vec3 b, float r)
{
  vec3  ba = b - a;
  vec3  pa = p - a;
  float baba = dot(ba, ba);
  float paba = dot(pa, ba);
  float x = length(pa*baba-ba*paba) - r*baba;
  float y = abs(paba-baba*0.5)-baba*0.5;
  float x2 = x*x;
  float y2 = y*y*baba;
  float d = (max(x, y)<0.0)?-min(x2, y2):(((x>0.0)?x2:0.0)+((y>0.0)?y2:0.0));
  return sign(d)*sqrt(abs(d))/baba;
}


Rounded Cylinder

float sdRoundedCylinder(vec3 p, float ra, float rb, float h)
{
  vec2 d = vec2(length(p.xz)-ra+rb, abs(p.y) - h + rb);
  return min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - rb;
}

Capped Cone

float sdCappedCone(vec3 p, float h, float r1, float r2)
{
  vec2 q = vec2(length(p.xz), p.y);
  vec2 k1 = vec2(r2, h);
  vec2 k2 = vec2(r2-r1, 2.0*h);
  vec2 ca = vec2(q.x-min(q.x, (q.y<0.0)?r1:r2), abs(q.y)-h);
  vec2 cb = q - k1 + k2*clamp(dot(k1-q, k2)/dot2(k2), 0.0, 1.0);
  float s = (cb.x<0.0 && ca.y<0.0) ? -1.0 : 1.0;
  return s*sqrt(min(dot2(ca), dot2(cb)));
}

float sdCappedCone(vec3 p, vec3 a, vec3 b, float ra, float rb) {
  float rba  = rb-ra;
  float baba = dot(b-a, b-a);
  float papa = dot(p-a, p-a);
  float paba = dot(p-a, b-a)/baba;
  float x = sqrt(papa - paba*paba*baba);
  float cax = max(0.0, x-((paba<0.5)?ra:rb));
  float cay = abs(paba-0.5)-0.5;
  float k = rba*rba + baba;
  float f = clamp((rba*(x-ra)+paba*baba)/k, 0.0, 1.0);
  float cbx = x-ra - f*rba;
  float cby = paba - f;
  float s = (cbx<0.0 && cay<0.0) ? -1.0 : 1.0;
  return s*sqrt(min(cax*cax + cay*cay*baba, cbx*cbx + cby*cby*baba));
}

float sdSolidAngle(vec3 p, vec2 c, float ra) {
  // c is the sin/cos of the angle
  vec2 q = vec2(length(p.xz), p.y);
  float l = length(q) - ra;
  float m = length(q - c*clamp(dot(q, c), 0.0, ra));
  return max(l, m*sign(c.y*q.x-c.x*q.y));
}

float sdCutSphere(vec3 p, float r, float h) {
  float w = sqrt(r*r-h*h);

  vec2 q = vec2(length(p.xz), p.y);
  float s = max((h-r)*q.x*q.x+w*w*(h+r-2.0*q.y), h*q.x-w*q.y);
  return (s<0.0) ? length(q)-r :
         (q.x<w) ? h - q.y     :
                   length(q-vec2(w, h));
}

float sdCutHollowSphere(vec3 p, float r, float h, float t) {
  float w = sqrt(r*r-h*h);
  vec2 q = vec2(length(p.xz), p.y);
  return ((h*q.x<w*q.y) ? length(q-vec2(w, h)) : abs(length(q)-r)) - t;
}

float sdDeathStar(vec3 p2, float ra, float rb, float d) {
  float a = (ra*ra - rb*rb + d*d)/(2.0*d);
  float b = sqrt(max(ra*ra-a*a, 0.0));

  vec2 p = vec2(p2.x, length(p2.yz));
  if(p.x*b-p.y*a > d*max(b-p.y, 0.0))
    return length(p-vec2(a, b));
  else
    return max( (length(p            )-ra),
               -(length(p-vec2(d, 0.0))-rb));
}

float sdRoundCone(vec3 p, float r1, float r2, float h)
{
  float b = (r1-r2)/h;
  float a = sqrt(1.0-b*b);

  vec2 q = vec2(length(p.xz), p.y);
  float k = dot(q, vec2(-b, a));
  if (k<0.0) return length(q) - r1;
  if (k>a*h) return length(q-vec2(0.0, h)) - r2;
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
  if (sign(z)*a2*z2>k) return  sqrt(x2 + z2)        *il2 - r2;
  if (sign(y)*a2*y2<k) return  sqrt(x2 + y2)        *il2 - r1;
                       return (sqrt(x2*a2*il2)+y*rr)*il2 - r1;
}

float sdVesicaSegment(in vec3 p, in vec3 a, in vec3 b, in float w) {
    vec3  c = (a+b)*0.5;
    float l = length(b-a);
    vec3  v = (b-a)/l;
    float y = dot(p-c, v);
    vec2  q = vec2(length(p-c-y*v), abs(y));

    float r = 0.5*l;
    float d = 0.5*(r*r-w*w)/w;
    vec3  h = (r*q.x<d*(q.y-r)) ? vec3(0.0, r, 0.0) : vec3(-d, 0.0, d+w);

    return length(q - h.xy()) - h.z;
}

float sdRhombus(vec3 p, float la, float lb, float h, float ra) {
  p = abs(p);
  float f = clamp((la*p.x-lb*p.z+lb*lb)/(la*la+lb*lb), 0.0, 1.0);
  vec2  w = p.xz - vec2(la, lb)*vec2(f, 1.0-f);
  vec2  q = vec2(length(w)*sign(w.x)-ra, p.y-h);
  return min(max(q.x, q.y), 0.0) + length(max(q, 0.0));
}

float sdOctahedron(vec3 p, float s) {
  p = abs(p);
  float m = p.x+p.y+p.z-s;
  vec3 q;
       if (3.0*p.x < m) q = p.xyz();
  else if (3.0*p.y < m) q = p.yzx();
  else if (3.0*p.z < m) q = p.zxy();
  else return m*0.57735027;

  float k = clamp(0.5*(q.z-q.y+s), 0.0, s);
  return length(vec3(q.x, q.y-s+k, q.z-k));
}

float sdOctahedron(vec3 p, float s) {
  p = abs(p);
  return (p.x+p.y+p.z-s)*0.57735027;
}

float sdPyramid(vec3 p, float h) {
  float m2 = h*h + 0.25;

  p.xz = abs(p.xz);
  p.xz = (p.z>p.x) ? p.zx : p.xz;
  p.xz -= 0.5;

  vec3 q = vec3(p.z, h*p.y - 0.5*p.x, h*p.x + 0.5*p.y);
  float s = max(-q.x, 0.0);
  float t = clamp((q.y-0.5*p.z)/(m2+0.25), 0.0, 1.0);
  float a = m2*(q.x+s)*(q.x+s) + q.y*q.y;
  float b = m2*(q.x+0.5*t)*(q.x+0.5*t) + (q.y-m2*t)*(q.y-m2*t);

  float d2 = min(q.y, -q.x*m2-q.y*0.5) > 0.0 ? 0.0 : min(a, b);
  return sqrt((d2+q.z*q.z)/m2) * sign(max(q.z, -p.y));
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
       dot2(ba*clamp(dot(ba, pa)/dot2(ba), 0.0, 1.0)-pa),
       dot2(cb*clamp(dot(cb, pb)/dot2(cb), 0.0, 1.0)-pb)),
       dot2(ac*clamp(dot(ac, pc)/dot2(ac), 0.0, 1.0)-pc)) :
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
       dot2(ba*clamp(dot(ba, pa)/dot2(ba), 0.0, 1.0)-pa),
       dot2(cb*clamp(dot(cb, pb)/dot2(cb), 0.0, 1.0)-pb)),
       dot2(dc*clamp(dot(dc, pc)/dot2(dc), 0.0, 1.0)-pc)),
       dot2(ad*clamp(dot(ad, pd)/dot2(ad), 0.0, 1.0)-pd)) :
       dot(nor, pa)*dot(nor, pa)/dot2(nor));
}

float sdEllipsoid(vec3 p, vec3 r) {
  float k0 = length(p/r);
  float k1 = length(p/(r*r));
  return k0*(k0-1.0)/k1;
}

float sdTriPrism(vec3 p, vec2 h) {
  vec3 q = abs(p);
  return max(q.z-h.y,max(q.x*0.866025+p.y*0.5,-p.y)-h.x*0.5);
}

float opRevolution( in vec3 p, in sdf2d primitive, in float o ) {
  vec2 q = vec2( length(p.xz) - o, p.y );
  return primitive(q)
}

float opExtrusion( in vec3 p, in sdf2d primitive, in float h ) {
  float d = primitive(p.xy)
  vec2 w = vec2( d, abs(p.z) - h );
  return min(max(w.x,w.y),0.0) + length(max(w,0.0));
}

float opElongate( in sdf3d primitive, in vec3 p, in vec3 h ) {
  vec3 q = p - clamp( p, -h, h );
  return primitive( q );
}

float opElongate( in sdf3d primitive, in vec3 p, in vec3 h ) {
  vec3 q = abs(p)-h;
  return primitive( max(q,0.0) ) + min(max(q.x,max(q.y,q.z)),0.0);
}

float opRound( in sdf3d primitive, in float rad ) {
  return primitive(p) - rad
}

float opOnion( in float sdf, in float thickness ) {
  return abs(sdf)-thickness;
}

float length2( vec3 p ) {
  p=p*p; return
  sqrt( p.x+p.y+p.z);
}

float length6( vec3 p ) {
  p=p*p*p; p=p*p; return
  pow(p.x+p.y+p.z,1.0/6.0);
}

float length8( vec3 p ) {
  p=p*p; p=p*p; p=p*p; return
  pow(p.x+p.y+p.z,1.0/8.0);
}

float opUnion( float a, float b ) {
  return min(a,b);
}

float opSubtraction( float a, float b ) {
  return max(-a,b);
}

float opIntersection( float a, float b ) {
  return max(a,b);
}

float opXor( float a, float b ) {
  return max(min(a,b),-max(a,b));
}

float opSmoothUnion( float a, float b, float k ) {
  k *= 4.0;
  float h = max(k-abs(a-b),0.0);
  return min(a, b) - h*h*0.25/k;
}

float opSmoothSubtraction( float a, float b, float k ) {
  return -opSmoothUnion(a,-b,k);

  // k *= 4.0;
  // float h = max(k-abs(-a-b),0.0);
  // return max(-a, b) + h*h*0.25/k;
}

float opSmoothIntersection( float a, float b, float k ) {
  return -opSmoothUnion(-a,-b,k);

  // k *= 4.0;
  // float h = max(k-abs(a-b),0.0);
  // return max(a, b) + h*h*0.25/k;
}

vec3 opTx( in vec3 p, in transform t, in sdf3d primitive ) {
  return primitive( invert(t)*p );
}

float opScale( in vec3 p, in float s, in sdf3d primitive ) {
  return primitive(p/s)*s;
}

float opSymX( in vec3 p, in sdf3d primitive ) {
  p.x = abs(p.x);
  return primitive(p);
}

float opSymXZ( in vec3 p, in sdf3d primitive ) {
  p.xz = abs(p.xz);
  return primitive(p);
}

float opRepetition( in vec3 p, in vec3 s, in sdf3d primitive ) {
  vec3 q = p - s*round(p/s);
  return primitive( q );
}

vec3 opLimitedRepetition( in vec3 p, in float s, in vec3 l, in sdf3d primitive ) {
  vec3 q = p - s*clamp(round(p/s),-l,l);
  return primitive( q );
}

float opDisplace( in sdf3d primitive, in vec3 p ) {
  float d1 = primitive(p);
  float d2 = displacement(p);
  return d1+d2;
}

float opTwist( in sdf3d primitive, in vec3 p ) {
  const float k = 10.0; // or some other amount
  float c = cos(k*p.y);
  float s = sin(k*p.y);
  mat2  m = mat2(c,-s,s,c);
  vec3  q = vec3(m*p.xz,p.y);
  return primitive(q);
}

float opCheapBend( in sdf3d primitive, in vec3 p ) {
  const float k = 10.0; // or some other amount
  float c = cos(k*p.x);
  float s = sin(k*p.x);
  mat2  m = mat2(c,-s,s,c);
  vec3  q = vec3(m*p.xy,p.z);
  return primitive(q);
}
#endif

}

namespace CQShaderGen {

Canvas::
Canvas()
{
  camera_ = new Camera;

  camera_->setPosition(CVector3D(0.0, 0.0, 1.0));
  camera_->setOrigin  (CVector3D(0.0, 0.0, 0.0));

  da_ = 10*M_PI/180;

  auto *timer = new QTimer(this);

  connect(timer, SIGNAL(timeout()), this, SLOT(updateSlot()));

  timer->start(100);
}

void
Canvas::
updateSlot()
{
  a_ += da_;

  update();
}

void
Canvas::
paintEvent(QPaintEvent *)
{
  drawAtAngle(a_);
}

void
Canvas::
drawAtAngle(double a)
{
#if 1
  auto rotAxis   = CVector3D(-1.0, 0.0, 1.0).normalized();
  auto rotMatrix = CMatrix3DH::rotation(a, rotAxis);
  auto rotPoint  = CVector3D(1.0, 0.0, 1.0).normalized();

  auto cameraPos = rotMatrix*rotPoint;
#else
  auto cx = std::cos(a);
  auto cy = std::sin(a);

  auto cameraPos = CVector3D(cx, 0.0, cy));
#endif

  camera_->setPosition(cameraPos);

  //---

  auto projectionMatrix = camera_->perspectiveMatrix();
  auto viewMatrix       = camera_->viewMatrix();

  auto iprojectionMatrix = projectionMatrix.inverse();
  auto iviewMatrix       = viewMatrix      .inverse();

//auto p1 = projectionMatrix*viewMatrix*CPoint3D(-1, -1, -1);
//auto p2 = projectionMatrix*viewMatrix*CPoint3D( 0,  0,  0);
//auto p3 = projectionMatrix*viewMatrix*CPoint3D( 1,  1,  1);
//std::cerr << p1 << " " << p2 << " " << p3 << "\n";
//auto p4 = iviewMatrix*iprojectionMatrix*p1;
//auto p5 = iviewMatrix*iprojectionMatrix*p2;
//auto p6 = iviewMatrix*iprojectionMatrix*p3;
//std::cerr << p4 << " " << p5 << " " << p6 << "\n";

  QPainter painter(this);

  painter.fillRect(rect(), Qt::black);

  auto w = double(width () - 1);
  auto h = double(height() - 1);

  for (int iy = 0; iy < h; ++iy) {
    auto y = 2.0*(iy/h - 0.5);

    for (int ix = 0; ix < w; ++ix) {
       auto x = 2.0*(ix/w - 0.5);

       auto p1 = CVector3D(x, y, 0.0);
       auto p2 = iprojectionMatrix*p1;
       auto p3 = iviewMatrix*p2;
//if (((ix % 200) == 0) && ((iy % 200) == 0)) {
//  std::cerr << ix << " " << iy << " : " << x << " " << y << "\n";
//  std::cerr << p1 << " " << p2 << " " << p3 << "\n";
//  auto p4 = projectionMatrix*viewMatrix*p3;
//  std::cerr << p4 << "\n";
//}

       auto p = vec3(p3.x(), p3.y(), p3.z());
     //auto p = vec3(x, y, 0.0);

       auto drawPoint = [&](float res) {
          if      (std::abs(res) < 0.01)
           painter.setPen(Qt::white);
         else if (res < 0.0)
           painter.setPen(QColor(-res*255, 0, 0));
         else
           painter.setPen(QColor(0, res*255, 0));

         painter.drawPoint(ix, iy);
       };

       switch (ind_) {
         case 0: {
           auto r = 0.3;
           drawPoint(sdSphere(p, r));
           break;
         }
         case 1: {
           auto b  = vec3(0.1, 0.2, 0.3);
           drawPoint(sdBox(p, b));
           break;
         }
         case 2: {
           auto br = 0.1;
           auto b  = vec3(0.1, 0.2, 0.3);
           drawPoint(sdRoundBox(p, b, br));
           break;
         }
         case 3: {
           auto br = 0.1;
           auto b  = vec3(0.1, 0.2, 0.3);
           drawPoint(sdBoxFrame(p, b, br));
           break;
         }
         case 4: {
           auto tt = vec2(0.2, 0.1);
           drawPoint(sdTorus(p, tt));
           break;
         }
         case 5: {
           auto sc = vec2(0.2, 0.1);
           auto ra = 0.2, rb = 0.1;
           drawPoint(sdCappedTorus(p, sc, ra, rb));
           break;
         }
         case 6: {
           auto le = 0.5, r1 = 0.2, r2 = 0.1;
           drawPoint(sdLink(p, le, r1, r2));
           break;
         }
       }
    }
  }
}

}
