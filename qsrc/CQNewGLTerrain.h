#ifndef CQNewGLTerrain_H
#define CQNewGLTerrain_H

#include <CQNewGLObject.h>

#include <QObject>
#include <QColor>
#include <vector>

class CQNewGLShaderProgram;
class CQGLTexture;
class CWaterSurface;

class CQNewGLTerrain : public QObject, public CQNewGLObject {
  Q_OBJECT

 public:
  enum class Type {
    NOISE,
    WATER_SURFACE
  };

 public:
  static void initShader(CQNewGLCanvas *canvas);

  //---

  CQNewGLTerrain(CQNewGLCanvas *canvas);

  //---

  const Type &type() const { return type_; }
  void setType(const Type &v) { type_ = v; }

  double width() const { return width_; }
  void setWidth(double r) { width_ = r; }

  double height() const { return height_; }
  void setHeight(double r) { height_ = r; }

  int gridSize() const { return gridSize_; }
  void setGridSize(int i) { gridSize_ = i; }

  bool isTextured() const { return textured_; }
  void setTextured(bool b) { textured_ = b; }

  //---

  void initBuffer() override;

  void addGeometry();

  void drawGeometry();

  CQNewGLShaderProgram *shaderProgram() override { return shaderProgram_; }

 private Q_SLOTS:
  void stepSlot();

 private:
  void addNoiseGeometry();
  void addWaterSurfaceGeometry();
  void updateWaterSurfaceGeometry();

 private:
  static CQNewGLShaderProgram* shaderProgram_;

  double width_    { 100.0 };
  double height_   { 5.0 };
  int    gridSize_ { 100 };
  bool   textured_ { false };

  QColor color_ { 50, 50, 255 };

  Type type_ { Type::NOISE };

  CWaterSurface* waterSurface_ { nullptr };

  CQGLTexture* texture_ { nullptr };
};

#endif
