#ifndef CSOLID_NOISE_H
#define CSOLID_NOISE_H

#include <cstdlib>
#include <CVector3D.h>
#include <CVector2D.h>

class CSolidNoise2D {
 public:
  CSolidNoise2D();

  double noise(const CVector2D &p) const;

  double turbulence(const CVector2D &p, int depth) const;

  double dturbulence(const CVector2D &p, int depth, double d) const;

  double omega(double t) const {
    t = fabs(t);

    return ((-6*t + 15)*t - 10)*t*t*t + 1;
  }

  CVector2D gamma(int i, int j) const {
    return grad2_[intGamma(i, j)];
  }

  int intGamma(int i, int j) const {
    int idx = phi2_[abs(j      ) % NUM_PERMUTATIONS_2];
        idx = phi2_[abs(i + idx) % NUM_PERMUTATIONS_2];

    return idx;
  }

  double knot(int i, int j, CVector2D &v) const {
    double x, y;

    v.getXY(&x, &y);

    return (omega(x)*omega(y))*gamma(i, j).dotProduct(v);
  }

 private:
  enum { NUM_PERMUTATIONS_2 = 8 };

  CVector2D grad2_[NUM_PERMUTATIONS_2];
  int       phi2_ [NUM_PERMUTATIONS_2];
};

//----

class CSolidNoise3D {
 public:
  CSolidNoise3D();

  double noise(const CVector3D &p) const;
  double noise(const CPoint3D &p) const;
  double noise(double x, double y, double z) const;

  double turbulence(const CVector3D &p, int depth) const;
  double turbulence(const CPoint3D  &p, int depth) const;

  double dturbulence(const CVector3D &p, int depth, double d) const;

  double omega(double t) const {
    t = fabs(t);

    return ((-6*t + 15)*t - 10)*t*t*t + 1;
  }

  CVector3D gamma(int i, int j, int k) const {
    return grad3_[intGamma(i, j, k)];
  }

  int intGamma(int i, int j) const {
    int idx = phi3_[abs(j      ) % NUM_PERMUTATIONS_3];
        idx = phi3_[abs(i + idx) % NUM_PERMUTATIONS_3];

    return idx;
  }

  int intGamma(int i, int j, int k) const {
    int idx = phi3_[abs(k      ) % NUM_PERMUTATIONS_3];
        idx = phi3_[abs(j + idx) % NUM_PERMUTATIONS_3];
        idx = phi3_[abs(i + idx) % NUM_PERMUTATIONS_3];

    return idx;
  }

  double knot(int i, int j, int k, CVector3D &v) const {
    double x, y, z;

    v.getXYZ(&x, &y, &z);

    return (omega(x)*omega(y)*omega(z))*gamma(i, j, k).dotProduct(v);
  }

 private:
  enum { NUM_PERMUTATIONS_3 = 16};

  CVector3D grad3_[NUM_PERMUTATIONS_3];
  int       phi3_ [NUM_PERMUTATIONS_3];
};

#endif
