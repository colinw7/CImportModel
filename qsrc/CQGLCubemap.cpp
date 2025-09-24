#include <CQGLCubemap.h>
#include <CMathGen.h>

#include <QFileInfo>
#include <QImageReader>
#include <iostream>

#if 0
#include <glad/glad.h>
#endif
#include <GL/glut.h>

namespace {

bool checkError(const char *msg) {
  // check texture generated
  GLenum err = glGetError();

  if (err != GL_NO_ERROR) {
    std::cerr << "OpenGL Error: " << gluErrorString(err) << "(" <<  msg << ")\n";
    return false;
  }

  return true;
}

}

//---

CQGLCubemap::
CQGLCubemap()
{
}

CQGLCubemap::
~CQGLCubemap()
{
  if (valid_ && glIsTexture(id_))
    glDeleteTextures(1, &id_);
}

bool
CQGLCubemap::
setImages(const std::vector<QImage> &images, bool flip)
{
  if (images.size() != 6) {
    std::cerr << "Invalid number of images\n";
    return false;
  }

  w_ = 0;
  h_ = 0;

  images_    .resize(6);
  imageDatas_.resize(6);

  for (int i = 0; i < 6; ++i) {
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

//  if (flip)
//    image = image.flippedH();

    auto w1 = image.width ();
    auto h1 = image.height();

    if (i == 0) {
      w_ = w1;
      h_ = h1;
    }
    else {
      if (w1 != w_ || h1 != h_) {
        std::cerr << "Invalid image size\n";
        return false;
      }
    }

    auto *imageData = new unsigned char [4*w_*h_];

    imageDatas_[i] = imageData;

    int ii = 0;

    for (int y = 0; y < h_; ++y) {
      int y1 = (flip ? h_ - 1 - y : y);

      for (int x = 0; x < w_; ++x) {
        auto rgba = image.pixel(x, y1);

        imageData[ii++] = qBlue (rgba);
        imageData[ii++] = qGreen(rgba);
        imageData[ii++] = qRed  (rgba);
        imageData[ii++] = qAlpha(rgba);
      }
    }
  }

  //------

  //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // allocate texture id
  glGenTextures(1, &id_);
  if (! checkError("glGenTextures")) return false;

  valid_ = true;

  // set texture type
  glBindTexture(GL_TEXTURE_CUBE_MAP, id_);
  if (! checkError("glBindTexture")) return false;

  return setParameters();
}

bool
CQGLCubemap::
setParameters()
{
  if (wrapType() == WrapType::CLAMP) {
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  }
  else {
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
  }
  if (! checkError("glTexParameteri")) return false;

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  if (! checkError("glTexParameteri")) return false;

  // select modulate to mix texture with color for shading
  //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  //if (! checkError("glTexEnvf")) return false;

  // build our texture mipmaps
  GLint internalFormat = (useAlpha() ? GL_RGBA : GL_RGB);

  for (int i = 0; i < 6; ++i)
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, w_, h_, 0,
                 GL_BGRA, GL_UNSIGNED_BYTE, imageDatas_[i]);

  //glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

  return true;
}

void
CQGLCubemap::
bind() const
{
  glBindTexture(GL_TEXTURE_CUBE_MAP, id_);
}

void
CQGLCubemap::
unbind() const
{
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void
CQGLCubemap::
enable(bool b)
{
  if (b) {
    glEnable(GL_TEXTURE_CUBE_MAP);
    glEnable(GL_TEXTURE_CUBE_MAP_EXT);
  }
  else {
    glDisable(GL_TEXTURE_CUBE_MAP);
    glDisable(GL_TEXTURE_CUBE_MAP_EXT);
  }
}
