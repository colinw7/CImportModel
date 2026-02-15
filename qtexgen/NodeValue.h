#ifndef NodeValue_H
#define NodeValue_H

namespace CQTextureGen {

class NodeValue {
 public:
  NodeValue() { }

  virtual ~NodeValue() { }

  virtual uint numValues() const = 0;

  virtual double realValue(uint) const = 0;

  virtual CRGBA colorValue(uint) const = 0;
};

class ColorArrayValue : public NodeValue {
 public:
  ColorArrayValue() { }

  uint numValues() const override { return colors_.size(); }

  double realValue(uint i) const override { return colorValue(i).getRed(); }

  CRGBA colorValue(uint i) const override {
    return (i < colors_.size() ? colors_[i] : CRGBA(0, 0, 0, 1)); }

  void resize(uint n) { colors_.resize(n); }
  void setColor(uint i, const CRGBA &c) { colors_[i] = c; }

 private:
  std::vector<CRGBA> colors_;
};

class RealArrayValue : public NodeValue {
 public:
  RealArrayValue() { }

  uint numValues() const override { return reals_.size(); }

  double realValue(uint i) const override { return (i < reals_.size() ? reals_[i] : 0.0); }

  CRGBA colorValue(uint i) const override { auto r = realValue(i); return CRGBA(r, r, r, 1); }

  void resize(uint n) { reals_.resize(n); }
  void setReal(uint i, double r) { reals_[i] = r; }

 private:
  std::vector<double> reals_;
};

}

#endif
