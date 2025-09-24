#ifndef CWATER_SURFACE_H
#define CWATER_SURFACE_H

#include <CVector3D.h>
#include <vector>

class CWaterSurface {
 public:
  CWaterSurface(uint n = 50);

  virtual ~CWaterSurface();

  uint getSize() const { return n_; }

  double getX(uint i) const { return x_[i]; }
  double getX(uint i, uint j) const { return x_[arrayInd(i, j)]; }

  double getY(uint i) const { return y_[i]; }
  double getY(uint i, uint j) const { return y_[arrayInd(i, j)]; }

  double getZ(uint i, uint j) const { return (*z1_)[arrayInd(i, j)]; }
  double getZ(uint i) const { return (*z1_)[i]; }

  void setZ(uint i, double z) { (*z1_)[i] = z; (*z2_)[i] = z; }
  void setZ(uint i, uint j, double z) { uint ij = arrayInd(i, j); (*z1_)[ij] = z; (*z2_)[ij] = z; }

  double getDampening(uint i) const { return d_[i]; }
  double getDampening(uint i, uint j) const { return d_[arrayInd(i, j)]; }

  void setDampening(uint i, double d) { d_[i] = d; }
  void setDampening(uint i, uint j, double d) { d_[arrayInd(i, j)] = d; }

  const CVector3D &getNormal(uint i) const { return normal_[i]; }
  const CVector3D &getNormal(uint i, uint j) const { return normal_[arrayInd(i, j)]; }

  virtual void step(double dt = 0.05);

  void interpolate(double x, double y, double &z, CVector3D &normal) const;

 private:
  uint arrayInd(uint i, uint j) const { return i + j*(n_ + 1); }

 private:
  uint n_ { 0 };

  double c_ { 2.0 }; // wave speed m/s
  double h_ { 1.0 }; // grid cell width
  double l_ { 1.0 }; // grid width

  std::vector<double> x_;
  std::vector<double> y_;

  std::vector<double> z1_data_;
  std::vector<double> z2_data_;

  std::vector<double> *z1_;
  std::vector<double> *z2_;

  std::vector<double> d_;

  std::vector<CVector3D> normal_;
};

#endif
