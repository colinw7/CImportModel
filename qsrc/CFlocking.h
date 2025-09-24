#ifndef CFLOCKING_H
#define CFLOCKING_H

#include <CBoid.h>
#include <CFlock.h>
#include <CFlockingUtil.h>

#include <CGeometry3D.h>

#include <CVector3D.h>
#include <CBBox3D.h>
#include <CMatrix3D.h>
#include <CRGBA.h>

class CFlocking {
 public:
  using FlockList = std::vector<CFlock *>;
  using BoidList  = std::vector<CBoid *>;

 public:
  CFlocking();
 ~CFlocking();

  const uint &numBoids() const { return numBoids_; }
  void setNumBoids(const uint &n) { numBoids_ = n; createBoids(); }

  const uint &numFlocks() const { return numFlocks_; }
  void setNumFlocks(const uint &n) { numFlocks_ = n; createBoids(); }

  const CRGBA &getFlockColor(int i) const;

  CGeomObject3D *getObject();

  void update(double dt=0.1);

  const BoidList &getBoids() const { return boids_; }

  void getObjects(std::vector<CGeomObject3D *> &objects);

 private:
  void createBoids();

  void updateFlock(CFlock *flock, double dt);

  void addFlockObjects(CFlock *flock, std::vector<CGeomObject3D *> &objects);

 private:
  uint           numBoids_ { 200 };
  uint           numFlocks_ { 4 };
  FlockList      flocks_;
  BoidList       boids_;
  CGeomObject3D *object_ { nullptr };
};

#endif
