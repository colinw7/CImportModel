#ifndef CQNewGLObject_H
#define CQNewGLObject_H

#include <CQNewGLFaceData.h>

#include <QString>
#include <vector>

class CQNewGLCanvas;
class CQNewGLShaderProgram;
class CQGLBuffer;

class CQNewGLObject {
 public:
  using FaceDatas = std::vector<CQNewGLFaceData>;

 public:
  CQNewGLObject(CQNewGLCanvas *canvas);

  virtual ~CQNewGLObject() { }

  //---

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  bool isSelected() const { return selected_; }
  void setSelected(bool b) { selected_ = b; }

  bool isWireframe() const { return wireframe_; }
  void setWireframe(bool b) { wireframe_ = b; }

  bool isSolid() const { return solid_; }
  void setSolid(bool b) { solid_ = b; }

  //---

  void addFaceData(const CQNewGLFaceData &faceData);

  const FaceDatas &faceDatas() const { return faceDatas_; }

  void setFaceDatas(const FaceDatas &faceDatas) { faceDatas_ = faceDatas; }

  //---

  virtual CQNewGLShaderProgram *shaderProgram() = 0;

  virtual void initBuffer();

  CQGLBuffer *buffer() const { return buffer_; }

 protected:
  CQNewGLCanvas* canvas_ { nullptr };

  QString name_;
  bool    visible_   { true };
  bool    selected_  { false };
  bool    wireframe_ { false };
  bool    solid_     { true };

  FaceDatas faceDatas_;

  CQGLBuffer* buffer_ { nullptr };
};

#endif
