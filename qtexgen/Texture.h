#ifndef Texture_H
#define Texture_H

#include <CGeomTexture.h>
#include <CQGLTexture.h>

class CQGLTexture;

namespace CQTextureGen {

class Texture : public CGeomTexture {
 public:
  Texture() { }

  CQGLTexture *glTexture() { return glTexture_; }

  bool isWrapped() const { return wrapped_; }

  void setWrapped(bool b) {
    wrapped_ = b;

    if (glTexture_)
      glTexture_->setWrapType(wrapped_ ?
       CQGLTexture::WrapType::REPEAT : CQGLTexture::WrapType::CLAMP);
  }

  void setGlTexture(CQGLTexture *t) {
    glTexture_ = t;

    setWrapped(wrapped_);
  }

 private:
  CQGLTexture* glTexture_ { nullptr };
  bool         wrapped_   { true };
};

}

#endif
