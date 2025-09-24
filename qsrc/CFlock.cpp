#include <CFlocking.h>

CFlock::FlockList CFlock::flocks_;

CBBox3D CFlock::world(0, 0, 0, 50, 50, 50);

CFlock::
CFlock()
{
#ifdef FLOCK_DEBUG
  printf("\nCFlock constructor called for %d\n", getNumFlocks());
#endif

  id_ = getNumFlocks();

  flocks_.push_back(this);
}

CFlock::
~CFlock()
{
#ifdef FLOCK_DEBUG
  printf("\nCFlock destructor called for %d\n", id_);
#endif

  id_ = 0;

  int i = 0;

  for (auto *flock : flocks_) {
    if (flock == this) {
      flocks_[i] = nullptr;
      break;
    }

    ++i;
  }
}

void
CFlock::
Update(double deltaTime)
{
  for (auto *boid : getBoids())
    boid->FlockIt(this, deltaTime);
}

void
CFlock::
AddTo(CBoid *boid)
{
#ifdef FLOCK_DEBUG
  printf("adding %x to flock %d\n", boid, id_);
#endif

  boid->setFlock(this);

  boids_.push_back(boid);
}

int
CFlock::
GetCount()
{
  return int(boids_.size());
}

void
CFlock::
PrintData()
{
#ifdef FLOCK_DEBUG
  printf("\n");
  printf("=== Flock # %d ===\n",id_);

  printf("Number of members = %d\n", GetCount());
#endif

  for (auto *boid : getBoids())
    boid->PrintData();
}

void
CFlock::
RemoveFrom(CBoid *boid)
{
#ifdef FLOCK_DEBUG
  printf("\nremove %x\n", boid);
#endif

  boids_.remove(boid);
}
