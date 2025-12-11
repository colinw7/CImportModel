#ifndef CQCamera3DObject_H
#define CQCamera3DObject_H

#include <CQCamera3DFaceData.h>

#include <QString>
#include <map>

class CQCamera3DCanvas;
class CQCamera3DShaderProgram;
class CQCamera3DText;

class CQGLBuffer;

class CQCamera3DObject {
 public:
  using FaceDatas = std::vector<CQCamera3DFaceData>;
  using Texts     = std::vector<CQCamera3DText *>;

 public:
  CQCamera3DObject(CQCamera3DCanvas *canvas);

  virtual ~CQCamera3DObject() { }

  //---

  CQCamera3DCanvas *canvas() const { return canvas_; }

  //---

  bool isWireframe() const { return wireframe_; }
  void setWireframe(bool b) { wireframe_ = b; }

  //---

  CQCamera3DShaderProgram *getShader(const QString &vertex, const QString &fragment);

  virtual CQCamera3DShaderProgram *shaderProgram() = 0;

  void bindShader();
  void unbindShader();

  //---

  virtual CQGLBuffer *initBuffer();

  CQGLBuffer *buffer() const { return buffer_; }

  //---

  void addFaceData(const CQCamera3DFaceData &faceData);

  const FaceDatas &faceDatas() const { return faceDataList_.faceDatas; }

  void setFaceDatas(const FaceDatas &faceDatas) { faceDataList_.faceDatas = faceDatas; }

  //---

  void clearTexts();

  void addText(CQCamera3DText *text);

  void updateTexts();

  void drawTexts();

 protected:
  using Shaders = std::map<QString, CQCamera3DShaderProgram *>;

  CQCamera3DCanvas* canvas_ { nullptr };

  Shaders shaders_;

  CQCamera3DFaceDataList faceDataList_;

  CQGLBuffer* buffer_ { nullptr };

  bool wireframe_ { false };

  Texts texts_;
};

#endif
