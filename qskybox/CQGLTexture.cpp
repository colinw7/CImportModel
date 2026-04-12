#include <CQGLTexture.h>
#include <CQImage.h>
#include <CImageLib.h>
#include <CMathGen.h>

#include <QFileInfo>
#include <QImageReader>

#include <GL/glut.h>

namespace {

bool checkError(const char *msg) {
  // check texture generated
  GLenum err = glGetError();

  if (err != GL_NO_ERROR) {
    std::cerr << "OpenGL Error: " << gluErrorString(err) << "(" << msg << ")\n";
    return false;
  }

  return true;
}

}

//---

CQGLTexture::
CQGLTexture()
{
}

CQGLTexture::
CQGLTexture(const QImage &image)
{
  setImage(image);
}

CQGLTexture::
CQGLTexture(const CImagePtr &image)
{
  setImage(image);
}

CQGLTexture::
~CQGLTexture()
{
  if (valid_ && glIsTexture(textureId_))
    glDeleteTextures(1, &textureId_);
}

bool
CQGLTexture::
load(const QString &fileName, bool flip)
{
  QFileInfo fi(fileName);

  if (! fi.exists(fileName)) {
    std::cerr << "Error: Invalid texture file '" << fileName.toStdString() << "'\n";
    return false;
  }

  QImageReader imageReader(fileName);

  QImage image;

  if (! imageReader.read(&image))
    return false;

  if (image.isNull()) {
    std::cerr << "Error: Failed to read image from '" << fileName.toStdString() << "'\n";
    return false;
  }

  return load(image, flip);
}

bool
CQGLTexture::
load(const QImage &image, bool flip)
{
  return initImage(image, flip);
}

void
CQGLTexture::
setImage(const QImage &image)
{
  assert(type_ == Type::NONE || type_ == Type::IMAGE);

  type_ = Type::IMAGE;

  initImage(image, /*flip*/false);
}

void
CQGLTexture::
setImage(const CImagePtr &image)
{
  assert(type_ == Type::NONE || type_ == Type::IMAGE);

  type_ = Type::IMAGE;

  QImage &qimage = dynamic_cast<CQImage *>(image.get())->getQImage();

  initImage(qimage, /*flip*/false);
}

bool
CQGLTexture::
setTarget(int w, int h)
{
  assert(type_ == Type::NONE || type_ == Type::TARGET);

  type_ = Type::TARGET;

  if (! valid_ || w != targetWidth_ || h != targetHeight_) {
    targetWidth_  = w;
    targetHeight_ = h;

    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    if (frameBufferId_ == 0) {
      functions_->glGenFramebuffers(1, &frameBufferId_);
      if (! checkError("glGenFramebuffers")) return false;
    }

    glBindFrameBuffer(frameBufferId_);

    // The texture we're going to render to
    if (textureId_ == 0) {
      glGenTextures(1, &textureId_);
      if (! checkError("glGenTextures")) return false;
    }

    // generate texture
    glBindTexture2D(textureId_);

    // Give an empty image to OpenGL ( the last "0" )
    // no difference for GL_RGBA and GL_RGB (TODO: support multisample)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, targetWidth_, targetHeight_,
                 /*border*/0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    if (! checkError("glTexImage2D")) return false;

    // Poor filtering (need min filter to avoid mip map use - not set)
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    if (! checkError("glTexParameteri")) return false;

  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  //if (! checkError("glTexParameteri")) return false;

    // allocate depth buffer
    if (depthRenderBuffer_ == 0)
      functions_->glGenRenderbuffers(1, &depthRenderBuffer_);

    functions_->glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer_);
    functions_->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
                                      targetWidth_, targetHeight_);
//  functions_->glBindRenderbuffer(GL_RENDERBUFFER, 0);

    functions_->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                          GL_RENDERBUFFER, depthRenderBuffer_);

    // attach it to currently bound framebuffer object
    //functions_->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureId_, 0);
    functions_->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                       GL_TEXTURE_2D, textureId_, 0);
    if (! checkError("glFramebufferTexture2D")) return false;

    // generate render buffer
    // Set the list of draw buffers.
    GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    functions_->glDrawBuffers(1, drawBuffers); // "1" is the size of DrawBuffers

    if (functions_->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      std::cerr << "Framebuffer error\n";
      return false;
    }

    // reset current frame buffer
    glBindFrameBuffer(0);

    // unbind texture
    glBindTexture2D(0);

    valid_ = true;
  }

  return true;
}

bool
CQGLTexture::
setCubemap()
{
  assert(type_ == Type::NONE || type_ == Type::CUBE_MAP);

  type_ = Type::CUBE_MAP;

  if (! valid_) {
    // create cubemap texture
    if (textureId_ == 0) {
      glGenTextures(1, &textureId_);
      if (! checkError("glGenTextures")) return false;
    }

    // make texture current
    glBindTextureCubeMap(textureId_);

    // set parameters
#if 1
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
#else
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
#endif
    if (! checkError("glTexParameteri")) return false;

#if 0
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#else
  //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#endif
    if (! checkError("glTexParameteri")) return false;

    // unbind texture
    glBindTextureCubeMap(0);

    valid_ = true;
  }

  return true;
}

bool
CQGLTexture::
setCubemapImages(const std::vector<QImage> &images, bool flip)
{
  enum { CUBE_MAP_IMAGE_COUNT = 6 };

  assert(type_ == Type::CUBE_MAP);

  if (images.size() != CUBE_MAP_IMAGE_COUNT) {
    std::cerr << "Invalid number of images\n";
    return false;
  }

  //---

  // create cube image data memory
  targetWidth_  = 0;
  targetHeight_ = 0;

  images_    .resize(CUBE_MAP_IMAGE_COUNT);
  imageDatas_.resize(CUBE_MAP_IMAGE_COUNT);

  for (int i = 0; i < CUBE_MAP_IMAGE_COUNT; ++i) {
    if (images[i].isNull()) {
      std::cerr << "Invalid image data\n";
      return false;
    }

    QImage image;

    if (useAlpha())
      image = images[i].convertToFormat(QImage::Format_RGBA8888);
    else
      image = images[i].convertToFormat(QImage::Format_RGB888);

    images_[i] = image;

    auto w1 = image.width ();
    auto h1 = image.height();

    if (i == 0) {
      targetWidth_  = w1;
      targetHeight_ = h1;
    }
    else {
      if (w1 != targetWidth_ || h1 != targetHeight_) {
        std::cerr << "Invalid image size\n";
        return false;
      }
    }

    auto *imageData = new unsigned char [4*targetWidth_*targetHeight_];

    imageDatas_[i] = imageData;

    int ii = 0;

    for (int y = 0; y < targetHeight_; ++y) {
      int y1 = (flip ? targetHeight_ - 1 - y : y);

      for (int x = 0; x < targetWidth_; ++x) {
        auto rgba = image.pixel(x, y1);

        imageData[ii++] = qBlue (rgba);
        imageData[ii++] = qGreen(rgba);
        imageData[ii++] = qRed  (rgba);
        imageData[ii++] = qAlpha(rgba);
      }
    }
  }

  //---

  // make texture current
  glBindTextureCubeMap(textureId_);

  // build our texture mipmaps
  //GLint internalFormat = (useAlpha() ? GL_RGBA : GL_RGB);
  GLint internalFormat = GL_RGB;

  for (int i = 0; i < CUBE_MAP_IMAGE_COUNT; ++i)
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat,
                 targetWidth_, targetHeight_, 0, GL_BGRA,
                 GL_UNSIGNED_BYTE, imageDatas_[i]);

  functions_->glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

  // unbind texture
  glBindTextureCubeMap(0);

  return true;
}

bool
CQGLTexture::
setShadow(int w, int h)
{
  assert(type_ == Type::NONE || type_ == Type::SHADOW);

  type_ = Type::SHADOW;

  if (! valid_ || w != targetWidth_ || h != targetHeight_) {
    targetWidth_  = w;
    targetHeight_ = h;

    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    if (frameBufferId_ == 0) {
      functions_->glGenFramebuffers(1, &frameBufferId_);
      if (! checkError("glGenFramebuffers")) return false;
    }

    // The texture we're going to render to
    if (textureId_ == 0) {
      glGenTextures(1, &textureId_);
      if (! checkError("glGenTextures")) return false;
    }

    // make texture current
    glBindTexture2D(textureId_);

    // Give an empty image to OpenGL ( the last "0" )
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, targetWidth_, targetHeight_,
                 /*border*/0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    if (! checkError("glTexImage2D")) return false;

    // Poor filtering (need min filter to avoid mip map use - not set)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    if (! checkError("glTexParameteri")) return false;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    if (! checkError("glTexParameteri")) return false;

    // const depth on texture border for clipping of light view
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

#if 0
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if (! checkError("glTexParameteri")) return false;
#endif

#if 0
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_ALPHA);
    if (! checkError("glTexParameteri")) return false;
#endif

#if 0
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    if (! checkError("glTexParameteri")) return false;
#endif

    int width, height, depth;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_DEPTH_SIZE, &depth);
    std::cerr << "Texture : " << width << " " << height << " " << depth << "\n";

    // make framebuffer current
    glBindFrameBuffer(frameBufferId_);

    // use texture as frame buffer depth
    functions_->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                       GL_TEXTURE_2D, textureId_, 0);
    if (! checkError("glFramebufferTexture2D")) return false;

    glDrawBuffer(GL_NONE); // disable color buffers
    glReadBuffer(GL_NONE);

    if (functions_->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      std::cerr << "Framebuffer error\n";
      return false;
    }

    GLint depthBits;
    functions_->glGetFramebufferAttachmentParameteriv(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                                      GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE,
                                                      &depthBits);
    std::cerr << "Framebuffer depth: " << depthBits << "\n";

    // reset current frame buffer
    glBindFrameBuffer(0);

    // unbind texture
    glBindTexture2D(0);

    valid_ = true;
  }

  return true;
}

bool
CQGLTexture::
setShadowCubeMap(int w, int h)
{
  enum { CUBE_MAP_IMAGE_COUNT = 6 };

  assert(type_ == Type::NONE || type_ == Type::SHADOW_CUBE_MAP);

  type_ = Type::SHADOW_CUBE_MAP;

  if (! valid_ || w != targetWidth_ || h != targetHeight_) {
    targetWidth_  = w;
    targetHeight_ = h;

    // create framebuffer
    if (frameBufferId_ == 0) {
      functions_->glGenFramebuffers(1, &frameBufferId_);
      if (! checkError("glGenFramebuffers")) return false;
    }

    // create depth cubemap texture
    if (textureId_ == 0) {
      glGenTextures(1, &textureId_);
      if (! checkError("glGenTextures")) return false;
    }

    // make texture current
    glBindTextureCubeMap(textureId_);

    // Give an empty image to OpenGL ( the last "0" )
    for (unsigned int i = 0; i < CUBE_MAP_IMAGE_COUNT; ++i)
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                   targetWidth_, targetHeight_, /*border*/0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    if (! checkError("glTexImage2D")) return false;

    // Poor filtering (need min filter to avoid mip map use - not set)
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    if (! checkError("glTexParameteri")) return false;

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    if (! checkError("glTexParameteri")) return false;

#if 0
    int width, height, depth;
    glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP, 0, GL_TEXTURE_HEIGHT, &height);
    glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP, 0, GL_TEXTURE_DEPTH_SIZE, &depth);
    std::cerr << "Texture : " << width << " " << height << " " << depth << "\n";
#endif

    // make framebuffer current
    glBindFrameBuffer(frameBufferId_);

    // use texture as frame buffer depth
    functions_->glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureId_, 0);
    if (! checkError("glFramebufferTexture")) return false;

    glDrawBuffer(GL_NONE); // disable color buffers
    glReadBuffer(GL_NONE);

    if (functions_->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      std::cerr << "Framebuffer error\n";
      return false;
    }

#if 0
    GLint depthBits;
    functions_->glGetFramebufferAttachmentParameteriv(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                                      GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE,
                                                      &depthBits);
    std::cerr << "Framebuffer depth: " << depthBits << "\n";
#endif

    // reset current frame buffer
    glBindFrameBuffer(0);

    // unbind texture
    glBindTextureCubeMap(0);

    valid_ = true;
  }

  return true;
}

bool
CQGLTexture::
initImage(const QImage &image, bool flip)
{
  if (image.isNull()) {
    std::cerr << "Invalid image data\n";
    return false;
  }

  if (useAlpha())
    image_ = image.convertToFormat(QImage::Format_RGBA8888);
  else
    image_ = image.convertToFormat(QImage::Format_RGB888);

//if (flip)
//  image_ = image_.flippedH();

  //------

  // convert to GL compatible data
  width_  = image_.width ();
  height_ = image_.height();

  imageData_ = new unsigned char [4*width_*height_];

  int i = 0;

  for (int y = 0; y < height_; ++y) {
    int y1 = (flip ? height_ - 1 - y : y);

    for (int x = 0; x < width_; ++x) {
      auto rgba = image_.pixel(x, y1);

      imageData_[i++] = qBlue (rgba);
      imageData_[i++] = qGreen(rgba);
      imageData_[i++] = qRed  (rgba);
      imageData_[i++] = qAlpha(rgba);
    }
  }

  //------

  //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // allocate texture id
  glGenTextures(1, &textureId_);
  if (! checkError("glGenTextures")) return false;

  valid_ = true;

  // set current texture
  glBindTexture2D(textureId_);

  if (wrapType() == WrapType::CLAMP) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }
  else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }
  if (! checkError("glTexParameteri")) return false;

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  if (! checkError("glTexParameteri")) return false;

  // select modulate to mix texture with color for shading
  //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  //if (! checkError("glTexEnvf")) return false;

  // build our texture mipmaps
  GLint internalFormat = (useAlpha() ? GL_RGBA : GL_RGB);

  if (CMathGen::isPowerOf(2, width_) && CMathGen::isPowerOf(2, height_)) {
#if 1
    // Hardware mipmap generation
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
    if (! checkError("glTexParameteri")) return false;

    glHint(GL_GENERATE_MIPMAP_HINT_SGIS, GL_NICEST);
    if (! checkError("glHint")) return false;
#endif

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width_, height_, 0,
                 GL_BGRA, GL_UNSIGNED_BYTE, &imageData_[0]);
    if (! checkError("glTexImage2D")) return false;

#if 0
    glGenerateMipmap(GL_TEXTURE_2D);
    if (! checkError("glGenerateMipmap")) return false;
#endif
  }
  else {
    // No hardware mipmap generation support, fall back to the
    // good old gluBuild2DMipmaps function
    gluBuild2DMipmaps(GL_TEXTURE_2D, internalFormat, width_, height_,
                      GL_BGRA, GL_UNSIGNED_BYTE, &imageData_[0]);
    if (! checkError("gluBuild2DMipmaps")) return false;
  }

  // unbind texture
  glBindTexture2D(0);

  return true;
}

//---

void
CQGLTexture::
bindTexture()
{
  assert(textureId_ > 0);

  if (type_ == Type::IMAGE || type_ == Type::TARGET)
    bind();
  else
    bindBuffer();
}

void
CQGLTexture::
unbindTexture()
{
  assert(textureId_ > 0);

  if (type_ == Type::IMAGE || type_ == Type::TARGET)
    unbind();
  else
    unbindBuffer();
}

//---

void
CQGLTexture::
bindFrameBuffer()
{
  assert(textureId_ > 0);

  if (type_ == Type::IMAGE || type_ == Type::TARGET)
    assert(false);
  else
    bind();
}

void
CQGLTexture::
unbindFrameBuffer()
{
  assert(textureId_ > 0);

  if (type_ == Type::IMAGE || type_ == Type::TARGET)
    assert(false);
  else
    unbind();
}

//---

void
CQGLTexture::
bind() const
{
  glEnable(GL_TEXTURE_2D);

  // bind image texture
  if      (type_ == Type::IMAGE) {
    glBindTexture2D(textureId_);
  }
  // bind target framebuffer
  else if (type_ == Type::TARGET) {
    assert(frameBufferId_ > 0);

    glBindFrameBuffer(frameBufferId_);

    functions_->glViewport(0, 0, targetWidth_, targetHeight_);

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
  // bind cube map texture
  else if (type_ == Type::CUBE_MAP) {
    glBindTextureCubeMap(textureId_);
  }
  // bind shadow framebuffer
  else if (type_ == Type::SHADOW) {
    assert(frameBufferId_ > 0);

    glBindFrameBuffer(frameBufferId_);

    functions_->glViewport(0, 0, targetWidth_, targetHeight_);

    // Clear the screen
    glClear(GL_DEPTH_BUFFER_BIT);
  }
  // bind shadow cube map framebuffer
  else if (type_ == Type::SHADOW_CUBE_MAP) {
    assert(frameBufferId_ > 0);

    glBindFrameBuffer(frameBufferId_);

    functions_->glViewport(0, 0, targetWidth_, targetHeight_);

    // Clear the screen
    glClear(GL_DEPTH_BUFFER_BIT);
  }
  else {
    assert(false);
  }
}

void
CQGLTexture::
unbind() const
{
  if      (type_ == Type::IMAGE) {
    glBindTexture2D(0);
  }
  else if (type_ == Type::TARGET) {
    glBindFrameBuffer(0);
  }
  else if (type_ == Type::CUBE_MAP) {
    glBindTextureCubeMap(0);
  }
  else if (type_ == Type::SHADOW) {
    glBindFrameBuffer(0);
  }
  else if (type_ == Type::SHADOW_CUBE_MAP) {
    glBindFrameBuffer(0);
  }
  else {
    assert(false);
  }
}

//---

void
CQGLTexture::
bindBuffer() const
{
  // bind texture for frame buffer types
  // Note: only for frame buffer types !!!

  assert(frameBufferId_);

  glEnable(GL_TEXTURE_2D);

  if      (type_ == Type::TARGET) {
    glBindTexture2D(textureId_);
  }
  else if (type_ == Type::SHADOW) {
    glBindTexture2D(textureId_);
  }
  else if (type_ == Type::SHADOW_CUBE_MAP) {
    glBindTextureCubeMap(textureId_);
  }
  else {
    assert(false);
  }
}

void
CQGLTexture::
unbindBuffer() const
{
  // unbind texture for frame buffer types
  // Note: only for frame buffer types !!!

  assert(frameBufferId_);

  if      (type_ == Type::TARGET) {
    glBindTexture2D(0);
  }
  else if (type_ == Type::SHADOW) {
    glBindTexture2D(0);
  }
  else if (type_ == Type::SHADOW_CUBE_MAP) {
    glBindTextureCubeMap(0);
  }
  else {
    assert(false);
  }
}

//---

void
CQGLTexture::
enable(bool b)
{
  if (b)
    glEnable(GL_TEXTURE_2D);
  else
    glDisable(GL_TEXTURE_2D);
}

//---

void
CQGLTexture::
draw()
{
  draw(0.0, 0.0, 1.0, 1.0);
}

void
CQGLTexture::
draw(double x1, double y1, double x2, double y2)
{
  draw(x1, y1, 0, x2, y2, 0, 0, 0, 1, 1);
}

void
CQGLTexture::
draw(double x1, double y1, double z1, double x2, double y2, double z2,
     double tx1, double ty1, double tx2, double ty2)
{
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  enable(true);

  bind();

  // select modulate to mix texture with color for shading
  //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  glBegin(GL_QUADS);

  if      (fabs(z2 - z1) < 1E-3) {
    glTexCoord2d(tx1, ty1); glVertex3d(x1, y1, z1);
    glTexCoord2d(tx1, ty2); glVertex3d(x1, y2, z1);
    glTexCoord2d(tx2, ty2); glVertex3d(x2, y2, z1);
    glTexCoord2d(tx2, ty1); glVertex3d(x2, y1, z1);
  }
  else if (fabs(y2 - y1) < 1E-3) {
    glTexCoord2d(tx1, ty1); glVertex3d(x1, y1, z1);
    glTexCoord2d(tx1, ty2); glVertex3d(x1, y1, z2);
    glTexCoord2d(tx2, ty2); glVertex3d(x2, y1, z2);
    glTexCoord2d(tx2, ty1); glVertex3d(x2, y1, z1);
  }
  else {
    glTexCoord2d(tx1, ty1); glVertex3d(x1, y1, z1);
    glTexCoord2d(tx1, ty2); glVertex3d(x1, y2, z1);
    glTexCoord2d(tx2, ty2); glVertex3d(x1, y2, z2);
    glTexCoord2d(tx2, ty1); glVertex3d(x1, y1, z2);
  }

  glEnd();

  enable(false);
}

#if 0
void
CQGLTexture::
displayFramebufferTexture(ShaderProgram *program, int vertexId)
{
  if (! notInitialized) {
    // initialize shader and vao w/ NDC vertex coordinates at top-right of the screen
    [...]
  }

  glActiveTexture(GL_TEXTURE0);

  program->bind();

  bind();

  glBindVertexArray(vertexId);

  glDrawArrays(GL_TRIANGLES, 0, 6);

  glBindVertexArray(0);

  unbind();

  program->unbind();
}
#endif

//---

void
CQGLTexture::
writeImage(const std::string &filename, const ImageData &data) const
{
  if (data.debug)
    std::cerr << "write: " << filename << "\n";

  glBindTexture2D(getId());

  int width, height;
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

  std::vector<float> pixels;
  std::vector<CRGBA> colors;

  colors.resize(width*height);

  if      (type_ == Type::SHADOW) {
    MinMax minMax;

    pixels.resize(width*height);

    glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, &pixels[0]);

    int ii = 0;

    for (int iy = 0; iy < height; ++iy) {
      int iy1 = height - 1 - iy;

      for (int ix = 0; ix < width; ++ix, ++ii) {
        auto r = pixels[ii];

        if (data.scale)
          minMax.update(r);
        else {
          int ii1 = iy1*width + ix;

          colors[ii1] = CRGBA(r, r, r);
        }
      }
    }

    if (data.scale) {
      int ii = 0;

      for (int iy = 0; iy < height; ++iy) {
        int iy1 = height - 1 - iy;

        for (int ix = 0; ix < width; ++ix, ++ii) {
          auto r = pixels[ii];

          auto r1 = minMax.map(r);

          int ii1 = iy1*width + ix;

          colors[ii1] = CRGBA(r1, r1, r1);
        }
      }
    }

    if (data.debug)
      minMax.print("r");
  }
  else if (type_ == Type::SHADOW_CUBE_MAP) {
    std::cerr << "Unimplemented\n";
  }
  else {
    MinMax minMax;
    MinMax rMinMax;
    MinMax gMinMax;
    MinMax bMinMax;
    MinMax aMinMax;

    pixels.resize(4*width*height);

    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, &pixels[0]);

    int ii = 0, ii1 = 0;

    for (int iy = 0; iy < height; ++iy) {
      int iy1 = height - 1 - iy;

      for (int ix = 0; ix < width; ++ix, ii += 4, ++ii1) {
        auto r = pixels[ii + 0];
        auto g = pixels[ii + 1];
        auto b = pixels[ii + 2];
        auto a = pixels[ii + 3];

        if (data.scale) {
          minMax.update(r);
          minMax.update(g);
          minMax.update(b);
        //minMax.update(a);

          if (data.debug) {
            rMinMax.update(r);
            gMinMax.update(g);
            bMinMax.update(b);
            aMinMax.update(a);
          }
        }
        else {
          int ii1 = iy1*width + ix;

          colors[ii1] = CRGBA(r, g, b, a);
        }
      }
    }

    if (data.scale) {
      int ii = 0;

      for (int iy = 0; iy < height; ++iy) {
        int iy1 = height - 1 - iy;

        for (int ix = 0; ix < width; ++ix, ii += 4) {
          auto r = pixels[ii + 0];
          auto g = pixels[ii + 1];
          auto b = pixels[ii + 2];

          auto r1 = minMax.map(r);
          auto g1 = minMax.map(g);
          auto b1 = minMax.map(b);

          int ii1 = iy1*width + ix;

          colors[ii1] = CRGBA(r1, g1, b1);
        }
      }

      if (data.debug) {
        rMinMax.print("r");
        gMinMax.print("g");
        bMinMax.print("b");
        aMinMax.print("a");
      }
    }
  }

  glBindTexture2D(0);

  //---

  auto src = CImageNoSrc();

  auto image = CImageMgrInst->createImage(src);

  image->setDataSize(width, height);

  int ii = 0;

  for (int iy = 0; iy < height; ++iy) {
    for (int ix = 0; ix < width; ++ix, ++ii) {
      image->setRGBAPixel(ix, iy, colors[ii]);
    }
  }

  CFile file(filename);

  image->writePNG(&file);
}

void
CQGLTexture::
getTextureRange(MinMax &minMax) const
{
  glBindTexture2D(getId());

  int width, height;
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

  std::vector<float> pixels;

  if      (type_ == Type::SHADOW) {
    pixels.resize(width*height);

    glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, &pixels[0]);

    int ii = 0;

    for (int iy = 0; iy < height; ++iy) {
      for (int ix = 0; ix < width; ++ix, ++ii) {
        auto r = pixels[ii];

        minMax.update(r);
      }
    }
  }
  else {
    pixels.resize(4*width*height);

    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, &pixels[0]);

    int ii = 0, ii1 = 0;

    for (int iy = 0; iy < height; ++iy) {
      for (int ix = 0; ix < width; ++ix, ii += 4, ++ii1) {
        auto r = pixels[ii + 0];
        auto g = pixels[ii + 1];
        auto b = pixels[ii + 2];
      //auto a = pixels[ii + 3];

        minMax.update(r);
        minMax.update(g);
        minMax.update(b);
      //minMax.update(a);
      }
    }
  }

  glBindTexture2D(0);
}

void
CQGLTexture::
getTextureCubeMapRange(MinMax &minMax) const
{
  enum { CUBE_MAP_IMAGE_COUNT = 6 };

  glBindTextureCubeMap(getId());

  for (int i = 0; i < CUBE_MAP_IMAGE_COUNT; ++i) {
    int width, height;
    glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_TEXTURE_HEIGHT, &height);

    std::vector<float> pixels;

    if      (type_ == Type::SHADOW_CUBE_MAP) {
      pixels.resize(width*height);

      glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                    GL_FLOAT, &pixels[0]);

      int ii = 0;

      for (int iy = 0; iy < height; ++iy) {
        for (int ix = 0; ix < width; ++ix, ++ii) {
          auto r = pixels[ii];

          minMax.update(r);
        }
      }
    }
    else {
      pixels.resize(4*width*height);

      glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, GL_FLOAT, &pixels[0]);

      int ii = 0, ii1 = 0;

      for (int iy = 0; iy < height; ++iy) {
        for (int ix = 0; ix < width; ++ix, ii += 4, ++ii1) {
          auto r = pixels[ii + 0];
          auto g = pixels[ii + 1];
          auto b = pixels[ii + 2];
        //auto a = pixels[ii + 3];

          minMax.update(r);
          minMax.update(g);
          minMax.update(b);
        //minMax.update(a);
        }
      }
    }
  }

  glBindTextureCubeMap(0);
}

//---

void
CQGLTexture::
glBindTexture2D(uint id) const
{
  if (id > 0) {
    assert(bindTextureId_ == 0);

    glBindTexture(GL_TEXTURE_2D, id);
  }
  else {
    assert(bindTextureId_ > 0);

    glBindTexture(GL_TEXTURE_2D, 0);
  }

  (void) checkError("glBindTexture");

  auto *th = const_cast<CQGLTexture *>(this);
  th->bindTextureId_ = id;
}

void
CQGLTexture::
glBindTextureCubeMap(uint id) const
{
  if (id > 0) {
    assert(bindTextureCubeMapId_ == 0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
  }
  else {
    assert(bindTextureCubeMapId_ > 0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  }

  (void) checkError("glBindTexture");

  auto *th = const_cast<CQGLTexture *>(this);
  th->bindTextureCubeMapId_ = id;
}

void
CQGLTexture::
glBindFrameBuffer(uint id) const
{
  if (id > 0) {
    assert(bindFrameBufferId_ == 0);

    functions_->glBindFramebuffer(GL_FRAMEBUFFER, id);
  }
  else {
    assert(bindFrameBufferId_ > 0);

    functions_->glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  (void) checkError("glBindFramebuffer");

  auto *th = const_cast<CQGLTexture *>(this);
  th->bindFrameBufferId_ = id;
}
