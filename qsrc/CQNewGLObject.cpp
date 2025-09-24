#include <CQNewGLObject.h>
#include <CQNewGLShaderProgram.h>
#include <CQGLBuffer.h>

CQNewGLObject::
CQNewGLObject(CQNewGLCanvas *canvas) :
 canvas_(canvas)
{
}

void
CQNewGLObject::
initBuffer()
{
  assert(shaderProgram());

  if (! buffer_)
    buffer_ = shaderProgram()->createBuffer();

  buffer_->clearBuffers();

  faceDatas_.clear();
}

void
CQNewGLObject::
addFaceData(const CQNewGLFaceData &faceData)
{
  faceDatas_.push_back(faceData);
}
