#include <CFlocking.h>
#include <COSRand.h>
#include <CFuncs.h>

CFlocking::
CFlocking()
{
  createBoids();
}

CFlocking::
~CFlocking()
{
  for (auto *flock : flocks_)
    delete flock;

  for (auto *boid : boids_)
    delete boid;
}

void
CFlocking::
createBoids()
{
  COSRand::srand();

  boids_.resize(numBoids());

  for (uint i = 0; i < numBoids(); ++i)
    boids_[i] = new CBoid(short(i));

  flocks_.resize(numFlocks());

  for (uint i = 0; i < numFlocks(); i++)
    flocks_[i] = new CFlock();

  uint count1 = 0;

  for (uint count = 0; count < 50; ++count, ++count1)
    flocks_[0]->AddTo(boids_[count1]);

  flocks_[0]->setColor(CRGBA(1, 0, 0));

  for (uint count = 0; count < 120; ++count, ++count1)
    flocks_[1]->AddTo(boids_[count1]);

  flocks_[1]->setColor(CRGBA(0, 1, 0));

  for (uint count = 0; count < 20; ++count, ++count1)
    flocks_[2]->AddTo(boids_[count1]);

  flocks_[2]->setColor(CRGBA(0,0,1));

  for (uint count = 0; count < 10; ++count, ++count1)
    flocks_[3]->AddTo(boids_[count1]);

  flocks_[3]->setColor(CRGBA(1,0,1));

#ifdef FLOCK_DEBUG
  for (uint i = 0; i < numFlocks(); ++i)
    printf("Flock %d count = %d\n", flocks_[i]->GetCount());

  printf("Total # of flocks = %d\n", CFlock::FlockCount);
#endif
}

const CRGBA &
CFlocking::
getFlockColor(int i) const
{
  return flocks_[uint(i)]->getColor();
}

CGeomObject3D *
CFlocking::
getObject()
{
  if (! object_) {
    // Bounding Box Object
    double w = CFlock::getWorld().getXSize()/2;
    double h = CFlock::getWorld().getYSize()/2;
    double l = CFlock::getWorld().getZSize()/2;

    object_ = CGeometry3DInst->createObject3D(nullptr, "object");

    uint vertex1 = object_->addVertex(CPoint3D(-w, -h, -l));
    uint vertex2 = object_->addVertex(CPoint3D( w, -h, -l));
    uint vertex3 = object_->addVertex(CPoint3D( w,  h, -l));
    uint vertex4 = object_->addVertex(CPoint3D(-w,  h, -l));
    uint vertex5 = object_->addVertex(CPoint3D(-w, -h,  l));
    uint vertex6 = object_->addVertex(CPoint3D( w, -h,  l));
    uint vertex7 = object_->addVertex(CPoint3D( w,  h,  l));
    uint vertex8 = object_->addVertex(CPoint3D(-w,  h,  l));

    object_->addLine(vertex1, vertex2);
    object_->addLine(vertex2, vertex3);
    object_->addLine(vertex3, vertex4);
    object_->addLine(vertex4, vertex1);
    object_->addLine(vertex5, vertex6);
    object_->addLine(vertex6, vertex7);
    object_->addLine(vertex7, vertex8);
    object_->addLine(vertex8, vertex5);
    object_->addLine(vertex1, vertex5);
    object_->addLine(vertex2, vertex6);
    object_->addLine(vertex3, vertex7);
    object_->addLine(vertex4, vertex8);

    //line1->setColor(CRGBA(0, 0, 0, 0));
    //line2->setColor(CRGBA(0, 0, 0, 0));
  }

  return object_;
}

void
CFlocking::
update(double dt)
{
  auto num_flocks = CFlock::getNumFlocks();

  for (uint i = 0; i < num_flocks; ++i) {
    auto *flock = CFlock::getFlock(int(i));

    updateFlock(flock, dt);
  }
}

void
CFlocking::
updateFlock(CFlock *flock, double dt)
{
  flock->Update(dt);

  for (auto *boid : flock->getBoids())
    boid->updateObject();
}

void
CFlocking::
getObjects(std::vector<CGeomObject3D *> &objects)
{
  uint num_flocks = CFlock::getNumFlocks();

  for (uint i = 0; i < num_flocks; ++i) {
    auto *flock = CFlock::getFlock(int(i));

    addFlockObjects(flock, objects);
  }

  auto *object = getObject();

  objects.push_back(object);
}

void
CFlocking::
addFlockObjects(CFlock *flock, std::vector<CGeomObject3D *> &objects)
{
  for (auto *boid : flock->getBoids()) {
    auto *object = boid->getObject();

    objects.push_back(object);
  }
}
