#ifndef Util_H
#define Util_H

#include <CGLVector3D.h>

namespace {

// map name->enum->value
template<typename ENUM, typename VALUE>
class ValueMap {
 public:
  ValueMap() { }

  void add(const QString &name, const ENUM &e, const VALUE &v) {
    names_ << name;

    nameEnum_[name] = e;
    enumName_[e]    = name;

    map1_[e] = v;
    map2_[v] = e;
  }

  const QStringList &names() const { return names_; }

  const VALUE &lookup(const ENUM &e, const VALUE &def=VALUE()) {
    auto p = map1_.find(e);
    return (p != map1_.end() ? (*p).second : def);
  }

  const ENUM &lookup(const VALUE &v, const ENUM &def=ENUM()) {
    auto p = map2_.find(v);
    return (p != map2_.end() ? (*p).second : def);
  }

  const ENUM &fromString(const QString &name, const ENUM &def=ENUM()) {
    auto p = nameEnum_.find(name);
    return (p != nameEnum_.end() ? (*p).second : def);
  }

 private:
  QStringList             names_;    // ordered names
  std::map<QString, ENUM> nameEnum_; // name to enum
  std::map<ENUM, QString> enumName_; // enum to name
  std::map<ENUM, VALUE>   map1_;     // enum to value
  std::map<VALUE, ENUM>   map2_;     // value to enum
};

//---

inline QColor RGBAToQColor(const CRGBA &c) {
  auto rtoi = [](double r) { return int(255*r); };

  return QColor(rtoi(c.getRed()), rtoi(c.getGreen()), rtoi(c.getBlue()), rtoi(c.getAlpha()));
}

inline CRGBA QColorToRGBA(const QColor &c) {
  auto itor = [](int i) { return i/255.0; };

  return CRGBA(itor(c.red()), itor(c.green()), itor(c.blue()), itor(c.alpha()));
}

inline CGLVector3D QColorToVector(const QColor &c) {
  auto itor = [](int i) { return i/255.0; };

  return CGLVector3D(itor(c.red()), itor(c.green()), itor(c.blue()));
}

inline CGLVector3D ColorToVector(const CRGBA &c) {
  return CGLVector3D(c.getRed(), c.getGreen(), c.getBlue());
}

//---

inline const char *toCString(const QString &str) {
  static char cString[256];
  assert(str.length() < 255);
  strcpy(cString, str.toStdString().c_str());
  return cString;
}

}

#endif
