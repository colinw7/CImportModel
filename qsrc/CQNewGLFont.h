#ifndef CQNewGLFont_H
#define CQNewGLFont_H

#include <CGLVector2D.h>
#include <CGLVector3D.h>

#include <QString>

#include <vector>
#include <cstdint>

struct CQNewGLCanvas;
struct CQNewGLShaderProgram;
struct CQNewGLFontData;

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
  CQNewGLFont(CQNewGLCanvas *canvas);

  void init();

  bool setFont(const QString &name);

  int size() const;

  GlyphInfo makeGlyphInfo(uint32_t character, float offsetX, float offsetY) const;

  CQNewGLCanvas *canvas() const { return canvas_; }

  CQNewGLShaderProgram *shaderProgram() const { return shaderProgram_; }

  bool bindTexture();

  int textureId() const;

 private:
  bool readFile(const char *path, std::vector<uint8_t> &bytes) const;

 private:
  CQNewGLCanvas*        canvas_        { nullptr };
  CQNewGLShaderProgram* shaderProgram_ { nullptr };
  CQNewGLFontData*      fontData_      { nullptr };
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

  void updateText();

  void render();

 private:
  using RotatingLabel = CQNewGLFont::RotatingLabel;

  QString      text_;
  CQNewGLFont* font_  { nullptr };
  Color        color_ { 1, 1, 1 };
  CGLVector3D  position_;
  CGLVector3D  angle_;

  std::vector<CGLVector3D> vertices_;
  std::vector<CGLVector2D> uvs_;
  std::vector<Color>       colors_;
  std::vector<uint16_t>    indexes_;
  RotatingLabel            rotatingLabel_;
  int                      indexElementCount_ { 0 };
};

#endif
