#include <CSolidNoise.h>
#include <CPerlinNoise.h>
#include <CRand.h>

CSolidNoise3D::
CSolidNoise3D()
{
  CRand rnd;

  grad3_[ 0] = CVector3D( 1,  1,  0);
  grad3_[ 1] = CVector3D(-1,  1,  0);
  grad3_[ 2] = CVector3D( 1, -1,  0);
  grad3_[ 3] = CVector3D(-1, -1,  0);
  grad3_[ 4] = CVector3D( 1,  0,  1);
  grad3_[ 5] = CVector3D(-1,  0,  1);
  grad3_[ 6] = CVector3D( 1,  0, -1);
  grad3_[ 7] = CVector3D(-1,  0, -1);
  grad3_[ 8] = CVector3D( 0,  1,  1);
  grad3_[ 9] = CVector3D( 0, -1,  1);
  grad3_[10] = CVector3D( 0,  1, -1);
  grad3_[11] = CVector3D( 0, -1, -1);
  grad3_[12] = CVector3D( 1,  1,  0);
  grad3_[13] = CVector3D(-1,  1,  0);
  grad3_[14] = CVector3D( 0, -1,  1);
  grad3_[15] = CVector3D( 0, -1, -1);

  for (int i = 0; i < NUM_PERMUTATIONS_3; ++i)
    phi3_[i] = i;

  for (int i = NUM_PERMUTATIONS_3 - 2; i >= 0; --i) {
    int j = i*int(rnd());

    std::swap(phi3_[i + 1], phi3_[j]);
  }
}

double
CSolidNoise3D::
noise(const CVector3D &p) const
{
  double x, y, z;

  p.getXYZ(&x, &y, &z);

  return noise(x, y, z);
}

double
CSolidNoise3D::
noise(const CPoint3D &p) const
{
  double x, y, z;

  p.getXYZ(&x, &y, &z);

  return noise(x, y, z);
}

double
CSolidNoise3D::
noise(double x, double y, double z) const
{
#if 1
  int fi = int(floor(x));
  int fj = int(floor(y));
  int fk = int(floor(z));

  double fu = x - fi;
  double fv = y - fj;
  double fw = z - fk;

  double sum = 0.0;

  CVector3D v;

  v.setXYZ(fu    , fv    , fw    ); sum += knot(fi    , fj    , fk    , v);
  v.setXYZ(fu    , fv    , fw - 1); sum += knot(fi    , fj    , fk + 1, v);
  v.setXYZ(fu    , fv - 1, fw    ); sum += knot(fi    , fj + 1, fk    , v);
  v.setXYZ(fu    , fv - 1, fw - 1); sum += knot(fi    , fj + 1, fk + 1, v);
  v.setXYZ(fu - 1, fv    , fw    ); sum += knot(fi + 1, fj    , fk    , v);
  v.setXYZ(fu - 1, fv    , fw - 1); sum += knot(fi + 1, fj    , fk + 1, v);
  v.setXYZ(fu - 1, fv - 1, fw    ); sum += knot(fi + 1, fj + 1, fk    , v);
  v.setXYZ(fu - 1, fv - 1, fw - 1); sum += knot(fi + 1, fj + 1, fk + 1, v);

  return sum;
#else
  return CPerlinNoiseInst->noise(x, y, z);
#endif
}

double
CSolidNoise3D::
turbulence(const CVector3D &p, int depth) const
{
  double sum = fabs(noise(p));

  double weight = 1.0;

  for (int i = 1; i < depth; ++i) {
    weight *= 2.0;

    sum += fabs(noise(p*weight))/weight;
  }

  return sum;
}

double
CSolidNoise3D::
turbulence(const CPoint3D &p, int depth) const
{
  double sum = fabs(noise(p));

  double weight = 1.0;

  for (int i = 1; i < depth; ++i) {
    weight *= 2.0;

    sum += fabs(noise(p*weight))/weight;
  }

  return sum;
}

double
CSolidNoise3D::
dturbulence(const CVector3D &p, int depth, double d) const
{
  double sum = fabs(noise(p))/d;

  double weight = 1.0;

  for (int i = 1; i < depth; ++i) {
    weight *= d;

    sum += fabs(noise(p*weight))/weight;
  }

  return sum;
}

//--------------

CSolidNoise2D::
CSolidNoise2D()
{
  CRand rnd;

  grad2_[0] = CVector2D( 1,  1);
  grad2_[1] = CVector2D(-1,  1);
  grad2_[2] = CVector2D( 1, -1);
  grad2_[3] = CVector2D(-1, -1);
  grad2_[4] = CVector2D( 1,  0);
  grad2_[5] = CVector2D(-1,  0);
  grad2_[6] = CVector2D( 0,  1);
  grad2_[7] = CVector2D( 0, -1);

  for (int i = 0; i < NUM_PERMUTATIONS_2; ++i)
    phi2_[i] = i;

  for (int i = NUM_PERMUTATIONS_2 - 2; i >= 0; --i)
    std::swap(phi2_[i + 1], phi2_[i*int(rnd())]);
}

double
CSolidNoise2D::
noise(const CVector2D &p) const
{
  double x, y;

  p.getXY(&x, &y);

  int fi = int(floor(x));
  int fj = int(floor(y));

  double fu = x - fi;
  double fv = y - fj;

  double sum = 0.0;

  CVector2D v;

  v.setXY(fu    , fv    ); sum += knot(fi    , fj    , v);
  v.setXY(fu    , fv - 1); sum += knot(fi    , fj + 1, v);
  v.setXY(fu - 1, fv    ); sum += knot(fi + 1, fj    , v);
  v.setXY(fu - 1, fv - 1); sum += knot(fi + 1, fj + 1, v);

  return sum;
}

double
CSolidNoise2D::
turbulence(const CVector2D &p, int depth) const
{
  double sum = fabs(noise(p));

  double weight = 1.0;

  for (int i = 1; i < depth; ++i) {
    weight *= 2.0;

    sum += fabs(noise(p*weight))/weight;
  }

  return sum;
}

double
CSolidNoise2D::
dturbulence(const CVector2D &p, int depth, double d) const
{
  double sum = fabs(noise(p))/d;

  double weight = 1.0;

  for (int i = 1; i < depth; ++i) {
    weight *= d;

    sum += fabs(noise(p*weight))/weight;
  }

  return sum;
}
