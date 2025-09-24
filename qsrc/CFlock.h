#ifndef _CFLOCK_H
#define _CFLOCK_H

#include <CBBox3D.h>
#include <CRGBA.h>
#include <vector>
#include <list>

class CFlock {
 public:
  using FlockList = std::vector<CFlock *>;
  using BoidList  = std::list<CBoid *>;

 public:
  static const CBBox3D &getWorld() { return world; }

  static uint getNumFlocks() { return uint(flocks_.size()); }

  static CFlock *getFlock(int i) { return flocks_[uint(i)]; }

  CFlock();
 ~CFlock();

  uint getId() const { return id_; }

  const CRGBA &getColor() const { return color_; }
  void setColor(const CRGBA &color) { color_ = color; }

  const BoidList &getBoids() const { return boids_; }

  void Update(double deltaTime);

  void AddTo(CBoid *boid);

  int GetCount();

  void PrintData();

  void RemoveFrom(CBoid *boid);

 private:
  static CBBox3D   world;
  static FlockList flocks_;

  uint     id_ { 0 };
  BoidList boids_;
  CRGBA    color_;
};

#endif
