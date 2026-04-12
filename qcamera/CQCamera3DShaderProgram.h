#ifndef CQCamera3DShaderProgram_H
#define CQCamera3DShaderProgram_H

#include <QOpenGLShaderProgram>
#include <QString>

class CQCamera3DApp;
class CQGLBuffer;

class CQCamera3DShaderVar {
 public:
  CQCamera3DShaderVar(QOpenGLShaderProgram *program, const QString &name) :
   program_(program), name_(name) {
    location_ = program_->uniformLocation(name_);
  }

 protected:
  QOpenGLShaderProgram* program_  { nullptr };
  QString               name_;
  int                   location_ { -1 };
};

//---

template<typename T>
class CQCamera3DShaderVarT : public CQCamera3DShaderVar {
 public:
  CQCamera3DShaderVarT(QOpenGLShaderProgram *program, const QString &name, const T &value=T()) :
   CQCamera3DShaderVar(program, name), value_(value) {
  }

  void setValue(const T &value) {
    if (valid_ && value == value_)
      return;

    program_->setUniformValue(location_, value_);

    valid_ = true;
  }

 private:
  T    value_;
  bool valid_ { false };
};

//---

class CQCamera3DShaderProgram : public QOpenGLShaderProgram {
 public:
  CQCamera3DShaderProgram(CQCamera3DApp *app);

  void addShaders(const QString &vertex, const QString &fragment);

  void addVertexShader  (const QString &name);
  void addFragmentShader(const QString &name);
  void addGeometryShader(const QString &name);

  CQGLBuffer *createBuffer();

  template<typename T>
  CQCamera3DShaderVarT<T> addVar(const QString &name, const T &value=T()) {
    auto *var = new CQCamera3DShaderVarT(this, name, value);

    valueMap_[name] = var;

    return var;
  }

  template<typename T>
  void setVar(const QString &name, const T &value=T()) {
    auto pv = valueMap_.find(name);
    assert(pv != valueMap_.end());

    auto *var = dynamic_cast<CQCamera3DShaderVarT<T> *>((*pv).second);

    var->setValue(value);
  }

 private:
  using ValueMap = std::map<QString, CQCamera3DShaderVar *>;

  CQCamera3DApp* app_ { nullptr };
  ValueMap       valueMap_;
};

#endif
