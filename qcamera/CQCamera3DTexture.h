#ifndef CQCamera3DTexture_H
#define CQCamera3DTexture_H

#include <CGeomTexture.h>
#include <CQGLTexture.h>

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

  bool isWrapped() const { return wrapped_; }

  void setWrapped(bool b) {
    wrapped_ = b;

    for (auto pw : widgetTextureData_) {
      auto &textureData = pw.second;

      if (textureData.glTexture)
        textureData.glTexture->setWrapType(wrapped_ ?
         CQGLTexture::WrapType::REPEAT : CQGLTexture::WrapType::CLAMP);

      if (textureData.glTextureFlipped)
        textureData.glTextureFlipped->setWrapType(wrapped_ ?
         CQGLTexture::WrapType::REPEAT : CQGLTexture::WrapType::CLAMP);
    }
  }

  void setGlTextures(CQCamera3DWidget *widget, CQGLTexture *t1, CQGLTexture *t2) {
    auto &textureData = widgetTextureData_[widget->ind()];

    textureData.glTexture        = t1;
    textureData.glTextureFlipped = t2;

    setWrapped(wrapped_);
  }

 private:
  struct TextureData {
    CQGLTexture* glTexture        { nullptr };
    CQGLTexture* glTextureFlipped { nullptr };
  };

  using WidgetTextureData = std::map<uint, TextureData>;

  bool flipped_ { false }; // TODO: per widget

  WidgetTextureData widgetTextureData_;
  bool              wrapped_ { true };
};

#endif
