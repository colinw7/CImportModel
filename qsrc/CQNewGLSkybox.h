#ifndef CQNewGLSkybox_H
#define CQNewGLSkybox_H

#include <CQNewGLObject.h>

#include <CImageMgr.h>

#include <QImage>
#include <vector>

class CQNewGLShaderProgram;
class CQGLCubemap;

class CQNewGLSkybox : public CQNewGLObject {
 public:
  static void initShader(CQNewGLCanvas *canvas);

  //---

  CQNewGLSkybox(CQNewGLCanvas *canvas);

  //---

  const QString &dirName() const { return dirName_; }
  void setDirName(const QString &s);

  double width() const { return width_; }
  void setWidth(double r) { width_ = r; }

  //---

  void initBuffer() override;

  //---

  void updateGeometry() override { }

  void addGeometry();

  void drawGeometry() override;

  //---

  CQNewGLShaderProgram *shaderProgram() override { return shaderProgram_; }

 private:
  void loadCubeMap();

 private:
  static CQNewGLShaderProgram* shaderProgram_;

  QString dirName_ { "beach" };
  double  width_   { 1000.0 };

  std::vector<QImage> images_;

  CQGLCubemap* cubemap_ { nullptr };
};

#endif
