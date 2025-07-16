#include <CQNewGLFont.h>
#include <CQNewGLCanvas.h>

#include <CQGLBuffer.h>
#include <CQGLUtil.h>
#include <CGLVector2D.h>
#include <CGLVector3D.h>


#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <memory>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#include <fstream>

struct CQNewGLFontData {
  uint32_t                            size = 40;
  uint32_t                            atlasWidth = 1024;
  uint32_t                            atlasHeight = 1024;
  uint32_t                            oversampleX = 2;
  uint32_t                            oversampleY = 2;
  uint32_t                            firstChar = ' ';
  uint32_t                            charCount = '~' - ' ';
  std::unique_ptr<stbtt_packedchar[]> charInfo;
  GLuint                              texture = 0;
};

class TextShaderProgram : public CQNewGLShaderProgram {
 public:
  TextShaderProgram(CQNewGLCanvas *canvas) :
   CQNewGLShaderProgram(canvas) {
  }

  GLint posAttr        { 0 };
  GLint colAttr        { 0 };
  GLint texPosAttr     { 0 };
  GLint textureUniform { 0 };
};

//----

namespace {

bool checkError() {
  // check texture generated
  GLenum err = glGetError();

  if (err != GL_NO_ERROR) {
    std::cerr << "OpenGL Error: " << gluErrorString(err) << "\n";
    return false;
  }

  return true;
}

}

//----

CQNewGLFont::
CQNewGLFont(CQNewGLCanvas *canvas) :
 canvas_(canvas)
{
}

void
CQNewGLFont::
init()
{
  shaderProgram_ = new CQNewGLShaderProgram(canvas_);

  shaderProgram_->addShaders("font.vs", "font.fs");
}

bool
CQNewGLFont::
setFont(const QString &name)
{
  delete fontData_;

  fontData_ = new CQNewGLFontData;

  //---

  auto path = canvas_->buildDir() + "/fonts/" + name;

  std::vector<uint8_t> fontData;

  if (! readFile(path.toLatin1().constData(), fontData))
    return false;

  auto atlasData = std::make_unique<uint8_t[]>(fontData_->atlasWidth*fontData_->atlasHeight);

  fontData_->charInfo = std::make_unique<stbtt_packedchar[]>(fontData_->charCount);

  stbtt_pack_context context;
  if (! stbtt_PackBegin(&context, atlasData.get(), fontData_->atlasWidth,
                        fontData_->atlasHeight, 0, 1, nullptr))
    assert(false);

  stbtt_PackSetOversampling(&context, fontData_->oversampleX, fontData_->oversampleY);

  if (! stbtt_PackFontRange(&context, fontData.data(), 0, fontData_->size, fontData_->firstChar,
                            fontData_->charCount, fontData_->charInfo.get()))
    assert(false);

  stbtt_PackEnd(&context);

  //---

//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // allocate texture id
  glGenTextures(1, &fontData_->texture);
  if (! checkError()) return false;

  // set texture type
  glBindTexture(GL_TEXTURE_2D, fontData_->texture);
  if (! checkError()) return false;

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  if (! checkError()) return false;

#if 0
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
#else
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#endif
  if (! checkError()) return false;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);

  glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
  if (! checkError()) return false;

  glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
  if (! checkError()) return false;

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fontData_->atlasWidth, fontData_->atlasHeight, 0,
               GL_RED, GL_UNSIGNED_BYTE, atlasData.get());
  if (! checkError()) return false;

#if 0
  glGenerateMipmap(GL_TEXTURE_2D);
  if (! checkError()) return false;
#endif

  return true;
}

int
CQNewGLFont::
size() const
{
  return fontData_->size;
}

CQNewGLFont::GlyphInfo
CQNewGLFont::
makeGlyphInfo(uint32_t character, float offsetX, float offsetY) const
{
  stbtt_aligned_quad quad;

  stbtt_GetPackedQuad(fontData_->charInfo.get(), fontData_->atlasWidth, fontData_->atlasHeight,
                      character - fontData_->firstChar, &offsetX, &offsetY, &quad, 1);

  const auto xmin =  quad.x0;
  const auto ymin = -quad.y1;
  const auto xmax =  quad.x1;
  const auto ymax = -quad.y0;

  GlyphInfo info{};

  info.offsetX = offsetX;
  info.offsetY = offsetY;

  info.positions[0] = CGLVector3D(xmin, ymin, 0.0f);
  info.positions[1] = CGLVector3D(xmin, ymax, 0.0f);
  info.positions[2] = CGLVector3D(xmax, ymax, 0.0f);
  info.positions[3] = CGLVector3D(xmax, ymin, 0.0f);

  info.uvs[0] = CGLVector2D(quad.s0, quad.t1);
  info.uvs[1] = CGLVector2D(quad.s0, quad.t0);
  info.uvs[2] = CGLVector2D(quad.s1, quad.t0);
  info.uvs[3] = CGLVector2D(quad.s1, quad.t1);

  return info;
}

bool
CQNewGLFont::
readFile(const char *path, std::vector<uint8_t> &bytes) const
{
  std::ifstream file(path, std::ios::binary | std::ios::ate);

  if (! file.is_open())
    return false;

  const auto size = file.tellg();

  file.seekg(0, std::ios::beg);
  bytes = std::vector<uint8_t>(size);
  file.read(reinterpret_cast<char *>(&bytes[0]), size);
  file.close();

  return true;
}

bool
CQNewGLFont::
bindTexture()
{
  glBindTexture(GL_TEXTURE_2D, textureId());
  if (! checkError()) return false;

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  if (! checkError()) return false;

#if 0
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
#else
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#endif
  if (! checkError()) return false;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);
  if (! checkError()) return false;

  glActiveTexture(GL_TEXTURE0);
  if (! checkError()) return false;

  return true;
}

int
CQNewGLFont::
textureId() const
{
  return fontData_->texture;
}

//---

CQNewGLText::
CQNewGLText(const QString &text) :
 text_(text)
{
}

void
CQNewGLText::
updateText()
{
  auto *program = font_->shaderProgram();

  auto *buffer = program->getBuffer();

  buffer->clearAll();

//uint16_t lastIndex = 0;

  float offsetX = 0, offsetY = 0;

  double f = 1.0/font_->size();

  for (const auto &c : text_) {
    const auto glyphInfo = font_->makeGlyphInfo(c.toLatin1(), offsetX, offsetY);

    offsetX = glyphInfo.offsetX;
    offsetY = glyphInfo.offsetY;

    auto addPos = [&](int i) {
      // x, y, z
      auto pos = glyphInfo.positions[i];

      pos.scaleX(f);
      pos.scaleY(f);

      buffer->addPoint(pos.x(), pos.y(), pos.z());

      // u, v
      buffer->addTexturePoint(glyphInfo.uvs[i].x(), glyphInfo.uvs[i].y());

      // color
      buffer->addColor(color().r, color().g, color().b);
    };

#if 0
    addPos(0);
    addPos(1);
    addPos(2);
    addPos(3);

    //---

    buffer->addIndex(lastIndex);
    buffer->addIndex(lastIndex + 1);
    buffer->addIndex(lastIndex + 2);
    buffer->addIndex(lastIndex);
    buffer->addIndex(lastIndex + 2);
    buffer->addIndex(lastIndex + 3);

    lastIndex += 4;
#else
    addPos(0);
    addPos(1);
    addPos(2);

    addPos(0);
    addPos(2);
    addPos(3);
#endif
  }

  buffer->load();

  indexElementCount_ = 6*text_.length();
}

void
CQNewGLText::
render()
{
  auto *program = font_->shaderProgram();
  auto *buffer  = program->getBuffer();
  auto *canvas  = font_->canvas();

  buffer->bind();

  program->bind();

  program->setUniformValue("projection", CQGLUtil::toQMatrix(canvas->projectionMatrix()));

  program->setUniformValue("view", CQGLUtil::toQMatrix(canvas->viewMatrix()));

  program->setUniformValue("model", CQGLUtil::toQMatrix(canvas->getModelMatrix()));

  //---

  font_->bindTexture();

  program->setUniformValue("mainTex", font_->textureId());

//glDrawElements(GL_TRIANGLES, indexElementCount_, GL_UNSIGNED_SHORT, nullptr);
  glDrawArrays(GL_TRIANGLES, 0, int(buffer->numPoints()));

  //---

  buffer->unbind();
}
