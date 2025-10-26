#ifndef CQNewGLModelObject_H
#define CQNewGLModelObject_H

#include <CQNewGLObject.h>
#include <CQNewGLFaceData.h>
#include <CQNewGLShaderProgram.h>
#include <CQGLBuffer.h>

class CQNewGLText;
class CGeomObject3D;

struct CQNewGLModelObjectDrawData {
  using FaceDatas = std::vector<CQNewGLFaceData>;
  using Texts     = std::vector<CQNewGLText *>;

  CQGLBuffer *buffer { nullptr };
  FaceDatas   faceDatas;
  Texts       texts;
};

//---

class CQNewGLModelObject : public CQNewGLObject {
 public:
  using ObjectDrawData = CQNewGLModelObjectDrawData;

 public:
  static void initShader(CQNewGLCanvas *canvas);

  //---

  CQNewGLModelObject(CQNewGLCanvas *canvas);

  int ind() const { return ind_; }
  void setInd(int i) { ind_ = i; }

  CQNewGLShaderProgram *shaderProgram() override { return shaderProgram_; }

  //---

  CGeomObject3D *object() const { return object_; }
  void setObject(CGeomObject3D *object) { object_ = object; }

  //---

  const Children &getChildren() const override;

  //---

  bool isFlipYZ() const { return flipYZ_; }
  void setFlipYZ(bool b) { flipYZ_ = b; }

  bool isInvertDepth() const { return invertDepth_; }
  void setInvertDepth(bool b) { invertDepth_ = b; }

  //---

  // normals (TODO: remove ?)
  CQNewGLShaderProgram *normalShaderProgram() { return normalShaderProgram_; }

  const ObjectDrawData &normalData() const { return normalData_; }
  void setNormalData(const ObjectDrawData &v) { normalData_ = v; }

  void initNormalBuffer() {
    if (! normalData_.buffer)
      normalData_.buffer = normalShaderProgram_->createBuffer();
  }

  void clearNormalData() {
    normalData_.faceDatas.clear();

    normalData_.buffer->clearBuffers();
  }

  void bindNormalData() {
    normalData_.buffer->bind();

    normalShaderProgram_->bind();
  }

  void unbindNormalData() {
    normalData_.buffer->unbind();

    normalShaderProgram_->release();
  }

  //---

  // bones
  CQNewGLShaderProgram *bonesShaderProgram() { return bonesShaderProgram_; }

  const ObjectDrawData &bonesData() const { return bonesData_; }
  void setBonesData(const ObjectDrawData &v) { bonesData_ = v; }

  void initBonesBuffer() {
    if (! bonesData_.buffer)
      bonesData_.buffer = bonesShaderProgram_->createBuffer();
  }

  void clearBonesData() {
    bonesData_.faceDatas.clear();

    bonesData_.buffer->clearBuffers();

    bonesData_.texts.clear();
  }

  void addBonesFaceData(const CQNewGLFaceData &faceData) {
    bonesData_.faceDatas.push_back(faceData);
  }

  void addBonesText(CQNewGLText *text) {
    bonesData_.texts.push_back(text);
  }

  void bindBonesData() {
    bonesData_.buffer->bind();

    bonesShaderProgram_->bind();
  }

  void unbindBonesData() {
    bonesData_.buffer->unbind();

    bonesShaderProgram_->release();
  }

  //---

  // bone
  CQNewGLShaderProgram *boneShaderProgram() { return boneShaderProgram_; }

  const ObjectDrawData &boneData() const { return boneData_; }
  void setBoneData(const ObjectDrawData &v) { boneData_ = v; }

  void initBoneBuffer() {
    if (! boneData_.buffer)
      boneData_.buffer = boneShaderProgram_->createBuffer();
  }

  void clearBoneData() {
    boneData_.faceDatas.clear();

    boneData_.buffer->clearBuffers();

    boneData_.texts.clear();
  }

  void addBoneFaceData(const CQNewGLFaceData &faceData) {
    boneData_.faceDatas.push_back(faceData);
  }

  void bindBoneData() {
    boneData_.buffer->bind();

    boneShaderProgram_->bind();
  }

  void unbindBoneData() {
    boneData_.buffer->unbind();

    boneShaderProgram_->release();
  }

  //---

  // annotation
  CQNewGLShaderProgram *annotationShaderProgram() { return annotationShaderProgram_; }

  const ObjectDrawData &annotationData() const { return annotationData_; }
  void setAnnotationData(const ObjectDrawData &v) { annotationData_ = v; }

  void initAnnotationBuffer() {
    if (! annotationData_.buffer)
      annotationData_.buffer = annotationShaderProgram_->createBuffer();
  }

  void clearAnnotationData() {
    annotationData_.faceDatas.clear();

    annotationData_.buffer->clearBuffers();

    annotationData_.texts.clear();
  }

  void addAnnotationFaceData(const CQNewGLFaceData &faceData) {
    annotationData_.faceDatas.push_back(faceData);
  }

  void bindAnnotationData() {
    annotationData_.buffer->bind();

    annotationShaderProgram_->bind();
  }

  void unbindAnnotationData() {
    annotationData_.buffer->unbind();

    annotationShaderProgram_->release();
  }

  //---

  void updateGeometry() override { }

  void drawGeometry() override { }

 private:
  static CQNewGLShaderProgram* shaderProgram_;
  static CQNewGLShaderProgram* normalShaderProgram_;
  static CQNewGLShaderProgram* bonesShaderProgram_;
  static CQNewGLShaderProgram* boneShaderProgram_;
  static CQNewGLShaderProgram* annotationShaderProgram_;

  int ind_ { 0 };

  CGeomObject3D* object_ { nullptr };

  bool flipYZ_      { false };
  bool invertDepth_ { false };

  ObjectDrawData normalData_;
  ObjectDrawData bonesData_;
  ObjectDrawData boneData_;
  ObjectDrawData annotationData_;
};

#endif
