#ifndef CQGLCubemap_H
#define CQGLCubemap_H

#include <QImage>
#include <GL/gl.h>

class CQGLCubemap {
 public:
  enum class WrapType {
    CLAMP,
    REPEAT
  };

 public:
  CQGLCubemap();

 ~CQGLCubemap();

  const WrapType &wrapType() const { return wrapType_; }
  void setWrapType(const WrapType &v) { wrapType_ = v; }

  bool useAlpha() const { return useAlpha_; }
  void setUseAlpha(bool b) { useAlpha_ = b; }

  bool setImages(const std::vector<QImage> &images, bool flip=false);

  bool setParameters();

  void bind() const;
  void unbind() const;

  uint getId() const { return id_; }

  void enable(bool b);

 private:
  CQGLCubemap(const CQGLCubemap &);

  CQGLCubemap &operator=(const CQGLCubemap &);

 private:
  using ImageData = unsigned char *;

  std::vector<QImage>     images_;
  std::vector<ImageData> imageDatas_;

  int w_ { 0 };
  int h_ { 0 };

  uint     id_       { 0 };
  bool     valid_    { false };
  WrapType wrapType_ { WrapType::CLAMP };
  bool     useAlpha_ { true };
};

#endif
