#ifndef Util_H
#define Util_H

#if 0
#include <GL/glut.h>
#endif

namespace CQShadow3D {

inline const char *toCString(const QString &str) {
  static char cString[256];
  assert(str.length() < 255);
  strcpy(cString, str.toStdString().c_str());
  return cString;
}

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

 private:
  QStringList             names_;    // ordered names
  std::map<QString, ENUM> nameEnum_; // name to enum
  std::map<ENUM, QString> enumName_; // enum to name
  std::map<ENUM, VALUE>   map1_;     // enum to value
  std::map<VALUE, ENUM>   map2_;     // value to enum
};

}

//---

namespace {

#if 0
inline bool checkError(const char *msg) {
  // check texture generated
  GLenum err = glGetError();

  if (err != GL_NO_ERROR) {
    std::cerr << "OpenGL Error: " << gluErrorString(err) << "(" << msg << ")\n";
    return false;
  }

  return true;
}
#endif

}

#endif
