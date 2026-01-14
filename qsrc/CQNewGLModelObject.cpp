#include <CQNewGLModelObject.h>
#include <CQNewGLCanvas.h>

#include <CGeomObject3D.h>

CQNewGLModelObject::
CQNewGLModelObject(CQNewGLWidget *widget) :
 CQNewGLObject(widget)
{
}

CQNewGLShaderProgram *
CQNewGLModelObject::
shaderProgram()
{
  return getShader("model.vs", "model.fs");
}

CQNewGLShaderProgram *
CQNewGLModelObject::
normalShaderProgram()
{
  return getShader("normal.vs", "normal.fs");
}

CQNewGLShaderProgram *
CQNewGLModelObject::
bonesShaderProgram()
{
  return getShader("bones.vs", "bones.fs");
}

CQNewGLShaderProgram *
CQNewGLModelObject::
boneShaderProgram()
{
  return getShader("bone.vs", "bone.fs");
}

CQNewGLShaderProgram *
CQNewGLModelObject::
annotationShaderProgram()
{
  return getShader("annotation.vs", "annotation.fs");
}

const CQNewGLModelObject::Children &
CQNewGLModelObject::
getChildren() const
{
  assert(object_);

  auto *th = const_cast<CQNewGLModelObject *>(this);

  th->children_.clear();

  for (auto *child : object_->children()) {
    auto *objectData = widget_->getObjectData(child);

    th->children_.push_back(objectData);
  }

  return children_;
}

CMatrix3D
CQNewGLModelObject::
getTransform() const
{
  assert(object_);

  return object_->getHierTransform();
}
