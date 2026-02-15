#ifndef ValueMap_H
#define ValueMap_H

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

 private:
  QStringList             names_;    // ordered names
  std::map<QString, ENUM> nameEnum_; // name to enum
  std::map<ENUM, QString> enumName_; // enum to name
  std::map<ENUM, VALUE>   map1_;     // enum to value
  std::map<VALUE, ENUM>   map2_;     // value to enum
};

}

#endif
