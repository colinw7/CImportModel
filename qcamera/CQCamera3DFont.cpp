#include <CQCamera3DFont.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DShaderProgram.h>
#include <CQCamera3DCamera.h>
#include <CQCamera3DApp.h>

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

struct CQCamera3DFontData {
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

//----

CQCamera3DFont::
CQCamera3DFont(CQCamera3DCanvas *canvas) :
 canvas_(canvas)
{
}

void
CQCamera3DFont::
init()
{
  shaderProgram_ = new CQCamera3DShaderProgram(canvas_);

  shaderProgram_->addShaders("font.vs", "font.fs");
}

bool
CQCamera3DFont::
setFontName(const QString &name)
{
  name_ = name;

  return updateFontData();
}

bool
CQCamera3DFont::
updateFontData()
{
  if (name_ == "")
    return true;

  //---

  delete fontData_;

  fontData_ = new CQCamera3DFontData;

  fontData_->size = size_;

  //---

  auto *app = canvas_->app();

  // get true type font data
  auto path = app->buildDir() + "/fonts/" + name_;

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

  if (! stbtt_PackFontRange(&context, fontData.data(), 0, size(), fontData_->firstChar,
                            fontData_->charCount, fontData_->charInfo.get()))
    assert(false);

  stbtt_PackEnd(&context);

  //---

#if 0
//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // allocate texture id
  glGenTextures(1, &fontData_->texture);
  if (! checkError("glGenTextures")) return false;

  // set texture type
  glBindTexture(GL_TEXTURE_2D, fontData_->texture);
  if (! checkError("glBindTexture")) return false;

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  if (! checkError("glTexParameteri")) return false;

#if 0
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
#else
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#endif
  if (! checkError("glTexParameteri")) return false;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);

  glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
  if (! checkError("glTexParameteri")) return false;

  glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
  if (! checkError("glHint")) return false;

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fontData_->atlasWidth, fontData_->atlasHeight, 0,
               GL_RED, GL_UNSIGNED_BYTE, atlasData.get());
  if (! checkError("glTexImage2D")) return false;

  canvas_->glGenerateMipmap(GL_TEXTURE_2D);
  if (! checkError("glGenerateMipmap")) return false;
#else
  if (! canvas_->createFontTexture(&fontData_->texture,
                                   fontData_->atlasWidth, fontData_->atlasHeight,
                                   atlasData.get()))
    return false;
#endif

  return true;
}

void
CQCamera3DFont::
setSize(int s)
{
  size_ = s;

  (void) updateFontData();
}

CQCamera3DFont::GlyphInfo
CQCamera3DFont::
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

#if 1
  info.uvs[0] = CGLVector2D(quad.s0, quad.t1);
  info.uvs[1] = CGLVector2D(quad.s0, quad.t0);
  info.uvs[2] = CGLVector2D(quad.s1, quad.t0);
  info.uvs[3] = CGLVector2D(quad.s1, quad.t1);
#else
  info.uvs[0] = CGLVector2D(0.0, 1.0);
  info.uvs[1] = CGLVector2D(0.0, 0.0);
  info.uvs[2] = CGLVector2D(1.0, 0.0);
  info.uvs[3] = CGLVector2D(1.0, 1.0);
#endif

  return info;
}

bool
CQCamera3DFont::
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
CQCamera3DFont::
bindTexture()
{
  glEnable(GL_TEXTURE_2D);
  //if (! checkError("glEnable")) return false;

  glBindTexture(GL_TEXTURE_2D, textureId());
  //if (! checkError("glBindTexture")) return false;

#if 0
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
#else
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#endif
  //if (! checkError("glTexParameteri")) return false;

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  //if (! checkError("glTexParameteri")) return false;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);
  //if (! checkError("glTexParameterf")) return false;

  glActiveTexture(GL_TEXTURE0);
  //if (! checkError("glActiveTexture")) return false;

  return true;
}

int
CQCamera3DFont::
textureId() const
{
  return fontData_->texture;
}

//---

CQCamera3DText::
CQCamera3DText(const QString &text) :
 text_(text)
{
}

void
CQCamera3DText::
updateText()
{
  initBuffer();

  //---

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

      //pos += position();

      buffer_->addPoint(pos.x(), pos.y(), pos.z());

      // u, v
      buffer_->addTexturePoint(glyphInfo.uvs[i].x(), glyphInfo.uvs[i].y());

      // color
      buffer_->addColor(color().r, color().g, color().b);
    };

#if 0
    addPos(0);
    addPos(1);
    addPos(2);
    addPos(3);

    //---

    buffer_->addIndex(lastIndex);
    buffer_->addIndex(lastIndex + 1);
    buffer_->addIndex(lastIndex + 2);

    buffer_->addIndex(lastIndex);
    buffer_->addIndex(lastIndex + 2);
    buffer_->addIndex(lastIndex + 3);

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

  buffer_->load();

//indexElementCount_ = 6*text_.length();
}

void
CQCamera3DText::
initBuffer()
{
  if (! buffer_) {
    auto *program = font_->shaderProgram();

    buffer_ = program->createBuffer();
  }

  buffer_->clearAll();
}

void
CQCamera3DText::
render(CQCamera3DCanvas *canvas)
{
  glDepthFunc(GL_LEQUAL);
  //(void) checkError("glDepthFunc");

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  //if (! checkError("glBlendFunc GL_SRC_ALPHA, GL_ONE")) return;

  //---

  auto *program = font_->shaderProgram();

  program->bind();

  buffer_->bind();

  //---

//auto mm1 = canvas->getModelMatrix();
  auto mm1 = CMatrix3DH::identity();
  auto mm2 = getModelMatrix();

  //---

  auto *camera = canvas->getCurrentCamera();

  // camera projection
  auto projectionMatrix = camera->perspectiveMatrix();
  program->setUniformValue("projection", CQGLUtil::toQMatrix(projectionMatrix));

  // camera/view transformation
  auto viewMatrix = camera->viewMatrix();
  program->setUniformValue("view", CQGLUtil::toQMatrix(viewMatrix));

  // model matrix
  program->setUniformValue("model", CQGLUtil::toQMatrix(mm1*mm2));

  program->setUniformValue("viewPos", CQGLUtil::toVector(camera->position()));

  //---

  font_->bindTexture();

//program->setUniformValue("mainTex", font_->textureId());
  program->setUniformValue("mainTex", 0);

//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

//glDrawElements(GL_TRIANGLES, indexElementCount_, GL_UNSIGNED_SHORT, nullptr);
  glDrawArrays(GL_TRIANGLES, 0, int(buffer_->numPoints()));

  //---

  buffer_->unbind();

  program->release();

  glDisable(GL_BLEND);
}

CMatrix3DH
CQCamera3DText::
getModelMatrix() const
{
  auto modelMatrix = CMatrix3DH::identity();

  modelMatrix.translated(position().x(), position().y(), position().z());

  modelMatrix.rotated(angle().x(), CVector3D(1.0, 0.0, 0.0));
  modelMatrix.rotated(angle().y(), CVector3D(0.0, 1.0, 0.0));
  modelMatrix.rotated(angle().z(), CVector3D(0.0, 0.0, 1.0));

  modelMatrix.scaled(size(), size(), 1.0);

  return modelMatrix;
}
