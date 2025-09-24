#include <CRGBA.h>
#include <CParticle2D.h>

class CConfig;

class CFireworks {
 public:
  using ParticleList = CParticleSystem2D::ParticleList;

  CFireworks();

  virtual ~CFireworks() { }

  SACCESSOR(InjectXVelMin, double, injectData_, velocityXMin)
  SACCESSOR(InjectYVelMin, double, injectData_, velocityYMin)
  SACCESSOR(InjectXVelMax, double, injectData_, velocityXMax)
  SACCESSOR(InjectYVelMax, double, injectData_, velocityYMax)

//ACCESSOR(InjectTime    , int, inject_time)
  ACCESSOR(ExplodeTicks  , int, explode_ticks)
  ACCESSOR(ExplosionTicks, int, explosion_ticks)

  const CPoint2D &getOrigin() const { return origin_; }
  void setOrigin(const CPoint2D &v) { origin_ = v; }

  int getInjectTicks() const { return injectData_.ticks; }
  void setInjectTicks(int i) { injectData_.ticks = i; }

  //---

//int drawCount() const { return draw_count_; }

  // draw (needs derived class with draw interface)
  void draw(int w, int h);

  void drawParticles();

  void drawParticle(CParticle2D *particle);

  //---

  // update
  void updateParticles(bool draw=false);

  void explodeParticle(CParticle2D *particle);

  void step();

  void injectParticle();

  void stepParticles();

  //---

  // data
  size_t numParticles() const;

  CParticle2D *particle(size_t i) const;

  const ParticleList &currentParticles() { return currentParticles_; }

  void updateCurrentParticles();

  //---

  // optional draw interface
  virtual void clear(const CRGBA &) { assert(false); }

  virtual void setForeground(const CRGBA &) { assert(false); }

  virtual void drawPoint(int, int) { assert(false); }

  //---

 private:
  CParticleSystem2D particleSystem_;

  CConfig *config_ { nullptr };

  // simulation time step
  double time_step_ { 0.01 };

  // injection data
  struct InjectData {
    int    ticks        { 50 };    // time to next particle
    double velocityXMin { -25.0 }; // x velocity min/max
    double velocityXMax { 25.0 };
    double velocityYMin { 20.0 };  // y velocity min/max
    double velocityYMax { 100.0 };
  };

  InjectData injectData_;

  // explode data
  int explode_ticks_   { 400 }; // time to explode
  int explosion_ticks_ { 200 }; // explostion time

  // current state
//int inject_time_  { 0 };
  int tick_count_   { 0 };
  int inject_count_ { 0 };

//int draw_count_   { 0 };

  CPoint2D origin_;

  int w_ { 100 };
  int h_ { 100 };

  ParticleList currentParticles_;
};
