#include <CQNewGLModelObject.h>
#include <CQNewGLCanvas.h>
#include <CGeomObject3D.h>

CQNewGLShaderProgram* CQNewGLModelObject::shaderProgram_;
CQNewGLShaderProgram* CQNewGLModelObject::normalShaderProgram_;
CQNewGLShaderProgram* CQNewGLModelObject::bonesShaderProgram_;
CQNewGLShaderProgram* CQNewGLModelObject::boneShaderProgram_;
CQNewGLShaderProgram* CQNewGLModelObject::annotationShaderProgram_;

void
CQNewGLModelObject::
initShader(CQNewGLCanvas *canvas)
{
  shaderProgram_ = new CQNewGLShaderProgram(canvas);
  shaderProgram_->addShaders("model.vs", "model.fs");

  //---

  normalShaderProgram_ = new CQNewGLShaderProgram(canvas);
  normalShaderProgram_->addShaders("normal.vs", "normal.fs");

  //---

  bonesShaderProgram_ = new CQNewGLShaderProgram(canvas);
  bonesShaderProgram_->addShaders("bones.vs", "bones.fs");

  boneShaderProgram_ = new CQNewGLShaderProgram(canvas);
  boneShaderProgram_->addShaders("bone.vs", "bone.fs");

  //---

  annotationShaderProgram_ = new CQNewGLShaderProgram(canvas);
  annotationShaderProgram_->addShaders("annotation.vs", "annotation.fs");
}

CQNewGLModelObject::
CQNewGLModelObject(CQNewGLCanvas *canvas) :
 CQNewGLObject(canvas)
{
}

const CQNewGLModelObject::Children &
CQNewGLModelObject::
getChildren() const
{
  assert(object_);

  auto *th = const_cast<CQNewGLModelObject *>(this);

  th->children_.clear();

  for (auto *child : object_->children()) {
    auto *objectData = canvas_->getObjectData(child);

    th->children_.push_back(objectData);
  }

  return children_;
}
