#ifndef CQNewGLMaze_H
#define CQNewGLMaze_H

#include <CQNewGLObject.h>

class CQNewGLCanvas;
class CQNewGLModel;

class CDungeon;

class CQNewGLMaze : public CQNewGLObject {
 public:
  CQNewGLMaze(CQNewGLCanvas *canvas);

  //---

  CDungeon *dungeon() const { return dungeon_; }

  double width() const { return width_; }
  void setWidth(double r) { width_ = r; }

  double height() const { return height_; }
  void setHeight(double r) { height_ = r; }

  //---

  CQGLBuffer *initBuffer() override;

  //---

  void updateGeometry() override;

  void drawGeometry() override;

  //---

  CQNewGLShaderProgram *shaderProgram() override;

 private:
  CQNewGLCanvas* canvas_ { nullptr };

  CDungeon* dungeon_ { nullptr };

  double width_  { 100.0 };
  double height_ { 5.0 };

  CQGLTexture* nwallTexture_ { nullptr };
  CQGLTexture* swallTexture_ { nullptr };
  CQGLTexture* wwallTexture_ { nullptr };
  CQGLTexture* ewallTexture_ { nullptr };

  CQGLTexture* nwallNormalTexture_ { nullptr };
  CQGLTexture* swallNormalTexture_ { nullptr };
  CQGLTexture* wwallNormalTexture_ { nullptr };
  CQGLTexture* ewallNormalTexture_ { nullptr };

  CQGLTexture* floorTexture_       { nullptr };
  CQGLTexture* floorNormalTexture_ { nullptr };

  CQGLTexture* doorTexture_ { nullptr };

  CQGLTexture* outsideTexture_ { nullptr };
};

#endif
