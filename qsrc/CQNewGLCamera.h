#ifndef CQNewGLCamera_H
#define CQNewGLCamera_H

#include <CQNewGLObject.h>
#include <CGLCamera.h>
#include <QColor>

class CQNewGLCanvas;
class CQNewGLShaderProgram;

class CQNewGLCamera : public CQNewGLObject, public CGLCamera {
 public:
  static void initShader(CQNewGLCanvas *canvas);

  //---

  CQNewGLCamera(CQNewGLCanvas *canvas, const CGLVector3D &origin,
                const CGLVector3D &position, const CGLVector3D &up);

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

  CQNewGLShaderProgram *shaderProgram() override { return shaderProgram_; }

 private:
  static CQNewGLShaderProgram* shaderProgram_;

  CGLVector3D initOrigin_;
  CGLVector3D initPosition_;
  CGLVector3D initUp_;

  QString name_;

  QColor color_ { 0, 255, 0 };

  bool followObject_ { false };
};

#endif
