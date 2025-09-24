#ifndef CBOID_H
#define CBOID_H

#include <CFlockingUtil.h>
#include <CVector3D.h>
#include <CMatrix3D.h>

class CFlock;
class CGeomObject3D;

class CBoid {
 public:
  CBoid(short id_v);
  CBoid(short id_v, CVector3D *pos_v, CVector3D *vel_v, CVector3D *ang_v);

 ~CBoid();

  CFlock *getFlock() const { return flock_; }
  void setFlock(CFlock *flock) { flock_ = flock; }

  short getId() const { return id_; }

  void FlockIt(CFlock *flock, double deltaTime);

  void AddToVisibleList(CBoid *ptr);

  void ClearVisibleList();

  const CVector3D &getPos() const;
  const CVector3D &getVelocity() const;
  const CVector3D &getAngle() const;

  void PrintData();

  CGeomObject3D *getObject();

  void updateObject();

 private:
  CVector3D Cruising();

  CVector3D FleeEnemies();

  CVector3D KeepDistance();

  CVector3D MatchHeading();

  int SeeEnemies(CFlock *flock);

  int SeeFriends(CFlock *flock);

  CVector3D SteerToCenter();

  void WorldBound();

  double AccumulateChanges(CVector3D &accumulator, CVector3D changes);

  double CanISee(CBoid *ptr);

  void ComputeRPY();

 private:
  static CBoid *VisibleFriendsList[CFlockingUtil::Max_Friends_Visible];

  CFlock *  flock_ { nullptr };
  short     id_ { 0 };
  double    perception_range_ { CFlockingUtil::Default_Perception_Range };
  CVector3D pos_;
  CVector3D vel_;
  CVector3D ang_;
  double    speed_ { 0.0 };

  short  num_flockmates_seen_       { 0 };
  CBoid* nearest_flockmate_         { nullptr };
  double dist_to_nearest_flockmate_ { CFlockingUtil::MY_INFINITY };

  short  num_enemies_seen_      { 0 };
  CBoid* nearest_enemy_         { nullptr };
  double dist_to_nearest_enemy_ { CFlockingUtil::MY_INFINITY };

  CVector3D oldpos_;
  CVector3D oldvel_;
  CMatrix3D imatrix_;

  CGeomObject3D *object_ { nullptr };
};

#endif
