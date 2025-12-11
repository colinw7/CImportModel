#ifndef CQNewGLFont_H
#define CQNewGLFont_H

#include <CGLVector2D.h>
#include <CGLVector3D.h>
#include <CMatrix3D.h>

#include <QString>

#include <vector>
#include <cstdint>

class  CQNewGLModel;
struct CQNewGLFontData;
struct CQNewGLWidget;
struct CQNewGLShaderProgram;

class CQGLBuffer;

class CQNewGLFont {
 public:
  struct GlyphInfo {
    CGLVector3D positions[4];
    CGLVector2D uvs[4];
    float       offsetX { 0 };
    float       offsetY { 0 };
  };

  struct Color {
    Color() = default;

    Color(float r_, float g_, float b_) :
     r(r_), g(g_), b(b_) {
    }

    float r { 0.0f };
    float g { 0.0f };
    float b { 0.0f };
  };

  struct RotatingLabel {
    uint  vao = 0;
    uint  vertexBuffer = 0;
    uint  uvBuffer = 0;
    uint  colBuffer = 0;
    uint  indexBuffer = 0;
    uint  indexElementCount = 0;
    float angle { 0.0f };
  };

 public:
  CQNewGLFont(CQNewGLWidget *widget);

  const QString &fontName() { return name_; }
  bool setFontName(const QString &name);

  int size() const { return size_; }
  void setSize(int s);

  GlyphInfo makeGlyphInfo(uint32_t character, float offsetX, float offsetY) const;

  CQNewGLWidget *widget() const { return widget_; }

  CQNewGLShaderProgram *shaderProgram() const;

  bool bindTexture();

  int textureId() const;

 private:
  bool updateFontData();

  bool readFile(const char *path, std::vector<uint8_t> &bytes) const;

 private:
  CQNewGLWidget*   widget_   { nullptr };
  QString          name_;
  int              size_     { 40 };
  CQNewGLFontData* fontData_ { nullptr };
};

//---

class CQNewGLText {
 public:
  using Color = CQNewGLFont::Color;

 public:
  CQNewGLText(const QString &text="");

  const QString &text() const { return text_; }
  void setText(const QString &s) { text_ = s; }

  const CQNewGLFont *font() const { return font_; }
  void setFont(CQNewGLFont *p) { font_ = p; }

  const Color &color() const { return color_; }
  void setColor(const Color &v) { color_ = v; }

  const CGLVector3D &position() const { return position_; }
  void setPosition(const CGLVector3D &v) { position_ = v; }

  const CGLVector3D &angle() const { return angle_; }
  void setAngle(const CGLVector3D &v) { angle_ = v; }

  double size() const { return size_; }
  void setSize(double r) { size_ = r; }

  void updateText();

  void render(CQNewGLWidget *widget);

 private:
  void initBuffer();

  CMatrix3D getModelMatrix() const;

 private:
//using RotatingLabel = CQNewGLFont::RotatingLabel;

  QString      text_;
  CQNewGLFont* font_  { nullptr };
  Color        color_ { 1, 1, 1 };
  CGLVector3D  position_;
  CGLVector3D  angle_ { 0, 0, 0 };
  double       size_  { 0.1 };

  CQGLBuffer *buffer_ { nullptr };

#if 0
  std::vector<CGLVector3D> vertices_;
  std::vector<CGLVector2D> uvs_;
  std::vector<Color>       colors_;
  std::vector<uint16_t>    indexes_;
#endif

//RotatingLabel rotatingLabel_;

//int indexElementCount_ { 0 };
};

#endif
