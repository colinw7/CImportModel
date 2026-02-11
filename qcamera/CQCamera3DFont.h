#ifndef CQCamera3DFont_H
#define CQCamera3DFont_H

#include <CGLVector2D.h>
#include <CGLVector3D.h>
#include <CMatrix3DH.h>

#include <QString>

#include <vector>
#include <cstdint>

class  CQCamera3DCanvas;
struct CQCamera3DFontData;
class  CQCamera3DShaderProgram;
class  CQGLBuffer;

class CQCamera3DFont {
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
  CQCamera3DFont(CQCamera3DCanvas *canvas);

  void init();

  const QString &fontName() { return name_; }
  bool setFontName(const QString &name);

  int size() const { return size_; }
  void setSize(int s);

  bool isBillboard() const { return billboard_; }
  void setBillboard(bool b) { billboard_ = b; }

  GlyphInfo makeGlyphInfo(uint32_t character, float offsetX, float offsetY) const;

  CQCamera3DCanvas *canvas() const { return canvas_; }

  CQCamera3DShaderProgram *shaderProgram() const { return shaderProgram_; }

  bool bindTexture();

  int textureId() const;

 private:
  bool updateFontData();

  bool readFile(const char *path, std::vector<uint8_t> &bytes) const;

 private:
  CQCamera3DCanvas*        canvas_        { nullptr };
  QString                  name_;
  int                      size_          { 40 };
  CQCamera3DShaderProgram* shaderProgram_ { nullptr };
  CQCamera3DFontData*      fontData_      { nullptr };
  bool                     billboard_     { true };
};

//---

class CQCamera3DText {
 public:
  using Color = CQCamera3DFont::Color;

 public:
  CQCamera3DText(const QString &text="");

  const QString &text() const { return text_; }
  void setText(const QString &s) { text_ = s; }

  const CQCamera3DFont *font() const { return font_; }
  void setFont(CQCamera3DFont *p) { font_ = p; }

  const Color &color() const { return color_; }
  void setColor(const Color &v) { color_ = v; }

  const CGLVector3D &position() const { return position_; }
  void setPosition(const CGLVector3D &v) { position_ = v; }

  const CGLVector3D &angle() const { return angle_; }
  void setAngle(const CGLVector3D &v) { angle_ = v; }

  double size() const { return size_; }
  void setSize(double r) { size_ = r; }

  void updateText();

  void render(CQCamera3DCanvas *canvas);

 private:
  void initBuffer();

  CMatrix3DH getModelMatrix() const;

 private:
//using RotatingLabel = CQCamera3DFont::RotatingLabel;

  QString         text_;
  CQCamera3DFont* font_  { nullptr };
  Color           color_ { 1, 1, 1 };
  CGLVector3D     position_;
  CGLVector3D     angle_ { 0, 0, 0 };
  double          size_  { 0.1 };

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
