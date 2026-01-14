#ifndef CQCamera3DTexture_H
#define CQCamera3DTexture_H

#include <CGeomTexture.h>

class CQGLTexture;

class CQCamera3DTexture : public CGeomTexture {
 public:
  CQCamera3DTexture() { }

  bool isFlipped() const { return flipped_; }
  void setFlipped(bool b) { flipped_ = b; }

  CQGLTexture *glTexture(CQCamera3DWidget *widget) {
    auto &widgetTexture = widgetTextureData_[widget->ind()];

    if (flipped_)
      return widgetTexture.glTextureFlipped;
    else
      return widgetTexture.glTexture;
  }

  void setGlTextures(CQCamera3DWidget *widget, CQGLTexture *t1, CQGLTexture *t2) {
    auto &widgetTexture = widgetTextureData_[widget->ind()];

    widgetTexture.glTexture        = t1;
    widgetTexture.glTextureFlipped = t2;
  }

 private:
  struct TextureData {
    CQGLTexture* glTexture        { nullptr };
    CQGLTexture* glTextureFlipped { nullptr };
  };

  using WidgetTextureData = std::map<uint, TextureData>;

  bool flipped_ { false }; // TODO: per widget

  WidgetTextureData widgetTextureData_;
};

#endif
