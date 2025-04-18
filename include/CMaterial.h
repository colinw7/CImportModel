#ifndef CMATERIAL_H
#define CMATERIAL_H

#include <CRGBA.h>

class CMaterial {
 public:
  CMaterial() { }

  CMaterial(const CRGBA &ambient, const CRGBA &diffuse, const CRGBA &specular=CRGBA(0,0,0,1),
            const CRGBA &emission=CRGBA(0,0,0,1), double shininess=1.0, bool mirror=false) :
   ambient_(ambient), diffuse_(diffuse), specular_(specular), emission_(emission),
   shininess_(shininess), mirror_(mirror) {
  }

  explicit
  CMaterial(const CRGBA &color, const CRGBA &specular=CRGBA(0,0,0,1),
            const CRGBA &emission=CRGBA(0,0,0,1), double shininess=1.0, bool mirror=false) :
   ambient_(color), diffuse_(color), specular_(specular), emission_(emission),
   shininess_(shininess), mirror_(mirror) {
  }

  CMaterial(const CMaterial &m) :
   ambient_(m.ambient_), diffuse_(m.diffuse_), specular_(m.specular_), emission_(m.emission_),
   shininess_(m.shininess_), mirror_(m.mirror_) {
  }

  CMaterial &operator=(const CMaterial &material) {
    if (this != &material) {
      ambient_   = material.ambient_;
      diffuse_   = material.diffuse_;
      specular_  = material.specular_;
      emission_  = material.emission_;
      shininess_ = material.shininess_;
      mirror_    = material.mirror_;
    }

    return *this;
  }

  void setColor(const CRGBA &color, const CRGBA &specular=CRGBA(0,0,0,1),
                const CRGBA &emission=CRGBA(0,0,0,1)) {
    setAmbient(color);
    setDiffuse(color);

    setSpecular(specular);
    setEmission(emission);
  }

  const CRGBA &getAmbient() const { return ambient_; }
  void setAmbient(const CRGBA &ambient) { ambient_ = ambient; }

  const CRGBA &getDiffuse() const { return diffuse_; }
  void setDiffuse(const CRGBA &diffuse) { diffuse_ = diffuse; }

  const CRGBA &getSpecular() const { return specular_; }
  void setSpecular(const CRGBA &specular) { specular_ = specular; }

  const CRGBA &getEmission() const { return emission_; }
  void setEmission(const CRGBA &emission) { emission_ = emission; }

  double getShininess() const { return shininess_; }
  void setShininess(double shininess) { shininess_ = shininess; }

  const CRGBA &getColor() const { return ambient_; }

  void setMirror(bool mirror) { mirror_ = mirror; }
  bool isMirror() const { return mirror_; }

  bool equal(const CRGBA &ambient, const CRGBA &diffuse=CRGBA(1,1,1),
             const CRGBA &specular=CRGBA(0,0,0), const CRGBA &emission=CRGBA(0,0,0),
             double shininess=1.0, bool mirror=false) {
   return (ambient_   == ambient   && diffuse_  == diffuse &&
           specular_  == specular  && emission_ == emission &&
           shininess_ == shininess && mirror_   == mirror);
  }

  bool equal(const CMaterial &material) {
    return (ambient_   == material.ambient_   &&
            diffuse_   == material.diffuse_   &&
            specular_  == material.specular_  &&
            emission_  == material.emission_  &&
            shininess_ == material.shininess_ &&
            mirror_    == material.mirror_);
  }

 private:
  CRGBA  ambient_   { 1.0, 1.0, 1.0, 1.0 };
  CRGBA  diffuse_   { 1.0, 1.0, 1.0, 1.0 };
  CRGBA  specular_  { 0.0, 0.0, 0.0, 1.0 };
  CRGBA  emission_  { 0.0, 0.0, 0.0, 1.0 };
  double shininess_ { 1.0 };
  bool   mirror_    { false };
};

#endif
