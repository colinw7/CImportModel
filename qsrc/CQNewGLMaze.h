#ifndef CQNewGLMaze_H
#define CQNewGLMaze_H

#include <CQNewGLObject.h>

class CDungeon;

class CQNewGLMaze : public CQNewGLObject {
 public:
  static void initShader(CQNewGLCanvas *canvas);

  //---

  CQNewGLMaze(CQNewGLCanvas *canvas);

  //---

  CDungeon *dungeon() const { return dungeon_; }

  double width() const { return width_; }
  void setWidth(double r) { width_ = r; }

  double height() const { return height_; }
  void setHeight(double r) { height_ = r; }

  //---

  void initBuffer() override;

  void addGeometry();

  void drawGeometry();

  CQNewGLShaderProgram *shaderProgram() override { return shaderProgram_; }

 private:
  static CQNewGLShaderProgram* shaderProgram_;

  CDungeon* dungeon_ { nullptr };

  double width_  { 100.0 };
  double height_ { 5.0 };

  CQGLTexture* nwallTexture_   { nullptr };
  CQGLTexture* swallTexture_   { nullptr };
  CQGLTexture* wwallTexture_   { nullptr };
  CQGLTexture* ewallTexture_   { nullptr };
  CQGLTexture* floorTexture_   { nullptr };
  CQGLTexture* doorTexture_    { nullptr };
  CQGLTexture* outsideTexture_ { nullptr };
};

#endif
