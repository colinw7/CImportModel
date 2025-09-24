#ifndef CFLOCKING_UTIL_H
#define CFLOCKING_UTIL_H

#include <cmath>

namespace CFlockingUtil {
  const float MaxSpeed       = 1.0f;
  const float MinUrgency     = 0.05f;
  const float MaxUrgency     = 0.1f;
  const float MaxChange      = (MaxSpeed * MaxUrgency);
  const float DesiredSpeed   = (MaxSpeed/2);
  const float KeepAwayDist   = 6.0f;
  const float SeparationDist = 3.0f;

  const int Max_Friends_Visible      = 10;
  const int Default_Perception_Range = 8.0;

  const bool UseTruth       = false;
  const bool ReactToEnemies = true;

  const double HALF_PI = M_PI/2.0;

  const double GRAVITY = 9.806650;

  const double MY_INFINITY = 999999.0;

  inline double RAND() { return double(rand()/(RAND_MAX * 1.0)); }
  inline double SIGN(double x) { return (x < 0.0 ? -1.0 : 1.0); }
}

#endif
