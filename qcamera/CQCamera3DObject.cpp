#include <CQCamera3DObject.h>
#include <CQCamera3DShaderProgram.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DFont.h>

#include <CQGLBuffer.h>

CQCamera3DObject::
CQCamera3DObject(CQCamera3DCanvas *canvas) :
 canvas_(canvas)
{
}

//---

CQCamera3DShaderProgram *
CQCamera3DObject::
getShader(const QString &vertex, const QString &fragment)
{
  auto id = QString("V:%1,F:%2").arg(vertex).arg(fragment);

  auto ps = shaders_.find(id);

  if (ps == shaders_.end()) {
    auto *shaderProgram = new CQCamera3DShaderProgram(canvas_->app());

    shaderProgram->addShaders(vertex, fragment);

    ps = shaders_.insert(ps, Shaders::value_type(id, shaderProgram));
  }

  return (*ps).second;
}

//---

void
CQCamera3DObject::
bindShader()
{
  shaderProgram()->bind();
}

void
CQCamera3DObject::
unbindShader()
{
  shaderProgram()->release();
}

//---

CQGLBuffer *
CQCamera3DObject::
initBuffer()
{
  assert(shaderProgram());

  if (! buffer_)
    buffer_ = shaderProgram()->createBuffer();

  buffer_->clearBuffers();

  faceDataList_.clear();

  return buffer_;
}

//---

void
CQCamera3DObject::
addFaceData(const CQCamera3DFaceData &faceData)
{
  faceDataList_.faceDatas.push_back(faceData);
}

//---

void
CQCamera3DObject::
clearTexts()
{
  for (auto *text : texts_)
    delete text;

  texts_.clear();
}

void
CQCamera3DObject::
addText(CQCamera3DText *text)
{
  texts_.push_back(text);
}

void
CQCamera3DObject::
updateTexts()
{
  for (auto *text : texts_)
    text->updateText();
}

void
CQCamera3DObject::
drawTexts()
{
  for (auto *text : texts_)
    text->render(canvas_);
}
