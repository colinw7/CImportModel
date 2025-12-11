#ifndef CQNewGLCamera_H
#define CQNewGLCamera_H

#include <CQNewGLObject.h>
#include <CGLCamera.h>
#include <QColor>

class CQNewGLModel;
class CQNewGLShaderProgram;

class CQNewGLCamera : public CQNewGLObject, public CGLCamera {
 public:
  CQNewGLCamera(CQNewGLWidget *widget, int ind, const CGLVector3D &origin,
                const CGLVector3D &position, const CGLVector3D &up);

  int ind() const { return ind_; }

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  const QColor &color() const { return color_; }
  void setColor(const QColor &c) { color_ = c; }

  bool isFollowObject() const { return followObject_; }
  void setFollowObject(bool b) { followObject_ = b; }

  void viewChanged() override;

  void reset();

  //---

  void updateGeometry() override { }

  // TODO: rename
  void addGeometry();

  void drawGeometry() override;

  //---

  CQNewGLShaderProgram *shaderProgram() override;

 private:
  int ind_ { -1 };

  QString name_;

  QColor color_ { 0, 255, 0 };

  bool followObject_ { false };
};

#endif
