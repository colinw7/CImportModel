#ifndef CQNewGLTerrain_H
#define CQNewGLTerrain_H

#include <CQNewGLObject.h>

#include <QObject>
#include <QColor>
#include <vector>

class CQNewGLShaderProgram;
class CQNewGLModel;
class CQNewGLCanvas;

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

  int octaves() const { return octaves_; }
  void setOctaves(int i) { octaves_ = i; }

  double xmin() const { return xmin_; }
  void setXMin(double r) { xmin_ = r; }

  double ymin() const { return ymin_; }
  void setYMin(double r) { ymin_ = r; }

  double xmax() const { return xmax_; }
  void setXMax(double r) { xmax_ = r; }

  double ymax() const { return ymax_; }
  void setYMax(double r) { ymax_ = r; }

  //---

  bool isTextured() const { return textured_; }
  void setTextured(bool b) { textured_ = b; }

  //---

  CQGLBuffer *initBuffer() override;

  //---

  void updateGeometry() override { }

  void addGeometry();

  void drawGeometry() override;

  //---

  CQNewGLShaderProgram *shaderProgram() override;

 private Q_SLOTS:
  void stepSlot();

 private:
  void addNoiseGeometry();
  void addWaterSurfaceGeometry();
  void updateWaterSurfaceGeometry();

 private:
  CQNewGLCanvas* canvas_ { nullptr };

  double width_    { 10.0 };
  double height_   { 1.0 };
  int    gridSize_ { 100 };
  int    octaves_  { 8 };

  double xmin_ { -1 };
  double ymin_ { -1 };
  double xmax_ {  1 };
  double ymax_ {  1 };

  QColor color_ { 50, 50, 255 };

  Type type_ { Type::NOISE };

  CWaterSurface* waterSurface_ { nullptr };

  bool         textured_ { false };
  CQGLTexture* texture_  { nullptr };
};

#endif
