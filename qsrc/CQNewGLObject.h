#ifndef CQNewGLObject_H
#define CQNewGLObject_H

#include <CQNewGLFaceData.h>
#include <CBBox3D.h>
#include <CMatrix3D.h>

#include <QString>
#include <vector>

class CQNewGLWidget;
class CQNewGLModel;
class CQNewGLShaderProgram;
class CQNewGLText;

class CQGLBuffer;

class CQNewGLObject {
 public:
  using Children  = std::vector<CQNewGLObject *>;
  using FaceDatas = std::vector<CQNewGLFaceData>;
  using Texts     = std::vector<CQNewGLText *>;

 public:
  CQNewGLObject(CQNewGLWidget *widget);

  virtual ~CQNewGLObject() { }

  //---

  CQNewGLModel* app() const;

  CQNewGLWidget* widget() const { return widget_; }

  //---

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  bool isValid() const { return valid_; }
  void setValid(bool b) { valid_ = b; }

  bool isSelected() const { return selected_; }
  void setSelected(bool b) { selected_ = b; }

  bool isWireframe() const { return wireframe_; }
  void setWireframe(bool b) { wireframe_ = b; }

  bool isSolid() const { return solid_; }
  void setSolid(bool b) { solid_ = b; }

  //---

  void setHierSolid(bool b);
  void setHierWireframe(bool b);

  //---

  void addFaceData(const CQNewGLFaceData &faceData);

  const FaceDatas &faceDatas() const { return faceDataList_.faceDatas; }

  void setFaceDatas(const FaceDatas &faceDatas) { faceDataList_.faceDatas = faceDatas; }

  //---

  virtual CQNewGLShaderProgram *shaderProgram() = 0;

  void bindShader();
  void unbindShader();

  //---

  virtual CQGLBuffer *initBuffer();

  CQGLBuffer *buffer() const { return buffer_; }

  //---

  virtual void updateGeometry() = 0;
  virtual void drawGeometry() = 0;

  //---

  void clearTexts();

  void addText(CQNewGLText *text);

  void updateTexts();

  void drawTexts();

  //---

  virtual const Children &getChildren() const { return children_; }

  //---

  virtual CBBox3D getBBox() const;

  virtual CMatrix3D getTransform() const;

 protected:
  CQNewGLWidget* widget_ { nullptr };

  QString name_;
  bool    visible_   { true };
  bool    valid_     { false };
  bool    selected_  { false };
  bool    wireframe_ { false };
  bool    solid_     { true };

  CQNewGLFaceDataList faceDataList_;

  CQGLBuffer* buffer_ { nullptr };

  Texts texts_;

  Children children_;
};

#endif
