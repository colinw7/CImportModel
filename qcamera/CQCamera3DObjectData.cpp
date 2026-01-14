#include <CQCamera3DObjectData.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DShaderProgram.h>

#include <CQGLBuffer.h>

CQCamera3DObjectData::
CQCamera3DObjectData(CQCamera3DCanvas *canvas) :
 canvas_(canvas)
{
}

CQGLBuffer *
CQCamera3DObjectData::
initBuffer()
{
  assert(canvas_->shaderProgram());

  if (! buffer_)
    buffer_ = canvas_->shaderProgram()->createBuffer();

  buffer_->clearBuffers();

  faceDatas_.clear();

  return buffer_;
}

void
CQCamera3DObjectData::
addFaceData(const CQCamera3DFaceData &faceData)
{
  faceDatas_.push_back(faceData);
}
