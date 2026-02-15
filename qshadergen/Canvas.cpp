#include <Canvas.h>
#include <Camera.h>

#include <util.h>
#include <shapes2d.h>
#include <shapes3d.h>

#include <QPainter>
#include <QTimer>
#include <QWheelEvent>

//---

namespace CQShaderGen {

Canvas::
Canvas(App *app) :
 app_(app)
{
  camera_ = new Camera;

  camera_->setPosition(toVector3D(cameraPos_));
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
  if (! is3D())
    draw2D();
  else
    draw3D(a_);
}

void
Canvas::
draw2D()
{
  MapFn2D *mapFn = nullptr;

  if      (type2D_ == Type2D::Circle) { mapFn = &sdCircleFn; }
  else if (type2D_ == Type2D::Heart ) { mapFn = &sdHeartFn ; }
  else if (type2D_ == Type2D::Cross ) { mapFn = &sdCrossFn ; }

  if (! mapFn)
    return;

  QPainter painter(this);

  painter.fillRect(rect(), Qt::black);

  auto w = double(std::min(width (), 512) - 1);
  auto h = double(std::min(height(), 512) - 1);

  for (int iy = 0; iy < h; ++iy) {
    auto y = 2.0*((h - iy)/h - 0.5);

    for (int ix = 0; ix < w; ++ix) {
      auto x = 2.0*(ix/w - 0.5);

      auto f = mapFn->exec(vec2(x, y));

      if (f <= 0)
        painter.setPen(Qt::white);
      else {
        auto f1 = std::min(f, 1.0);

        painter.setPen(QColor(f1*255, 0, 0));
      }

      painter.drawPoint(ix, iy);
    }
  }
}

void
Canvas::
draw3D(double a)
{
  MapFn3D *mapFn = nullptr;

  if      (type3D_ == Type3D::Sphere           ) { mapFn = &sdSphereFn           ; }
  else if (type3D_ == Type3D::Box              ) { mapFn = &sdBoxFn              ; }
  else if (type3D_ == Type3D::RoundBox         ) { mapFn = &sdRoundBoxFn         ; }
  else if (type3D_ == Type3D::BoxFrame         ) { mapFn = &sdBoxFrameFn         ; }
  else if (type3D_ == Type3D::Torus            ) { mapFn = &sdTorusFn            ; }
  else if (type3D_ == Type3D::CappedTorus      ) { mapFn = &sdCappedTorusFn      ; }
  else if (type3D_ == Type3D::Link             ) { mapFn = &sdLinkFn             ; }
  else if (type3D_ == Type3D::Cylinder         ) { mapFn = &sdCylinderFn         ; }
  else if (type3D_ == Type3D::Cone             ) { mapFn = &sdConeFn             ; }
  else if (type3D_ == Type3D::Plane            ) { mapFn = &sdPlaneFn            ; }
  else if (type3D_ == Type3D::HexPrism         ) { mapFn = &sdHexPrismFn         ; }
  else if (type3D_ == Type3D::Capsule          ) { mapFn = &sdCapsuleFn          ; }
  else if (type3D_ == Type3D::RotateCross      ) { mapFn = &sdRotateCrossFn      ; }
  else if (type3D_ == Type3D::ExtrudeCross     ) { mapFn = &sdExtrudeCrossFn     ; }
  else if (type3D_ == Type3D::ElongateEllipsoid) { mapFn = &sdElongateEllipsoidFn; }
  else if (type3D_ == Type3D::ElongateTorus    ) { mapFn = &sdElongateTorusFn    ; }
  else if (type3D_ == Type3D::OnionSphere      ) { mapFn = &sdOnionSphereFn      ; }

  if (! mapFn)
    return;

//---

#if 0
  auto rotAxis   = CVector3D(0.0, 1.0, 0.0).normalized();
  auto rotMatrix = CMatrix3DH::rotation(a, rotAxis);
  auto rotPoint  = toVector3D(cameraPos());

  auto cameraPos1 = rotMatrix*rotPoint;
#else
  auto ca = cameraPos_.z*std::cos(a);
  auto sa = cameraPos_.z*std::sin(a);

//auto cameraPos1 = CVector3D(ca, 0.0, sa);
  auto cameraPos1 = CVector3D(ca, cameraPos_.y, sa);
#endif

  camera_->setPosition(cameraPos1);

  //---

  auto tol = 1e-3;

  auto tmax = 10.0;

  auto iter = 64U;

  auto lightDir = normalize(this->lightPos());

  vec3 ambient = vec3(0.1, 0.1, 0.1);

  QPainter painter(this);

  painter.fillRect(rect(), Qt::black);

  auto w = double(std::min(width (), 512) - 1);
  auto h = double(std::min(height(), 512) - 1);

  for (int iy = 0; iy < h; ++iy) {
    auto y = 2.0*((h - iy)/h - 0.5);

    for (int ix = 0; ix < w; ++ix) {
      auto x = 2.0*(ix/w - 0.5);

      CPoint3D rp1, rp2;
      camera_->getPixelRay(x, y, rp1, rp2);

      auto ro = toVec3(rp1);
      auto rd = toVec3(rp2 - rp1);

      auto t = 0.0;

      for (uint ii = 0; ii < iter; ++ii) {
        auto pos = ro + t*rd;

        auto dist = mapFn->exec(pos);

        if (abs(dist) < tol || t >= tmax)
          break;

        t += dist;
      }

      if (t < 0 || t >= tmax)
        continue;

      //---

      // shading/lighting
      auto col = ambient;

      auto pos = ro + t*rd;
      vec3 nor = mapFn->calcNormal(pos);

      auto dif = clamp(dot(nor, lightDir), 0.0, 1.0);

      auto sha = mapFn->calcSoftshadow(pos, lightDir, 0.001, 1.0, 32.0);

#if 0
      vec3 hal = normalize(lightDir - rd);
      auto spe = pow(clamp(dot(nor, hal), 0.0, 1.0), 16.0)*
                 dif*(0.04 + 0.96*pow(clamp(1.0 - dot(hal, -rd), 0.0, 1.0), 5.0));
#endif

//    auto occ = calcOcclusion(pos, nor);
//    auto amb = 0.5 + 0.5*dot(nor, vec3(0.0, 1.0, 0.0));
//    col = vec3(0.5, 1.0, 1.2)*amb*occ;

    //col += vec3(2.8, 2.2, 1.8)*dif;
      col += vec3(1.0, 0.9, 0.8)*dif*sha;

//    col *= 0.2;

//    col += vec3(2.8, 2.2, 1.8)*spe*3.0;

      col = sqrt(col);

      //---

      painter.setPen(vec3ToQColor(col));
//    painter.setPen(Qt::white);

      painter.drawPoint(ix, iy);
    }
  }
}

void
Canvas::
wheelEvent(QWheelEvent *e)
{
  auto dw = e->angleDelta().y()/250.0;

  if (dw > 0)
    cameraPos_.z /= 1.1;
  else
    cameraPos_.z *= 1.1;

  update();
}

}
