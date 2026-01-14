#ifndef CQNewGLBones_H
#define CQNewGLBones_H

#include <CQNewGLShaderProgram.h>
#include <CQNewGLFaceData.h>
#include <CQNewGLWidget.h>

#include <CGLVector3D.h>
#include <CGLCamera.h>
#include <CQGLBuffer.h>

#include <CPoint3D.h>
#include <CRGBA.h>
#include <CImagePtr.h>

#include <QMatrix4x4>

class CQNewGLModelObject;
class CQNewGLModel;
class CQNewGLCamera;
class CQNewGLLight;
class CQNewGLFont;
class CQNewGLText;
class CQNewGLAxes;
class CQNewGLObject;

class CQGLBuffer;
class CQGLTexture;
class CGeomScene3D;
class CGeomObject3D;
class CGeomTexture;
class CBBox3D;

//---

class CQNewGLBones : public CQNewGLWidget {
  Q_OBJECT

 public:
  enum class BonesTransform {
    INVERSE_BIND,
    LOCAL,
    GLOBAL
  };

 public:
  CQNewGLBones(CQNewGLModel *app);

  //---

  // bones
  const BonesTransform &bonesTransform() const { return bonesData_.transform; }
  void setBonesTransform(BonesTransform t) { bonesData_.transform = t; }

  const CRGBA &bonesColor() const { return bonesData_.color; }
  void setBonesColor(const CRGBA &c) { bonesData_.color = c; }

  const CRGBA &bonesSelectedColor() const { return bonesData_.selectedColor; }
  void setBonesSelectedColor(const CRGBA &c) { bonesData_.selectedColor = c; }

  //---

  CQNewGLCamera *getCurrentCamera() const override { return camera_; }

  //---

  // axes
  CQNewGLAxes *getAxes() const { return axes_; }

  //---

  struct BoneData {
    std::string           name;
    int                   ind { 0 };
    std::vector<CPoint3D> vertices;
  };

  //---

  const CGLMatrix3D &projectionMatrix() const override { return paintData_.projection; }
  const CGLMatrix3D &viewMatrix() const override { return paintData_.view; }

  const CGLVector3D &viewPos() const { return paintData_.viewPos; }

  CMatrix3D getModelMatrix() const;

  CPoint3D  sceneCenter() const override { return sceneCenter_; }
  CVector3D sceneSize  () const override { return sceneSize_; }
  double    sceneScale () const override { return sceneScale_; }

  //---

  void initializeGL() override;

  void paintGL() override;

  void resizeGL(int, int) override;

  //---

  void addCamera();
  void resetCamera();

  //---

  void mousePressEvent  (QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent   (QMouseEvent *event) override;

  void wheelEvent(QWheelEvent *) override;

  void keyPressEvent(QKeyEvent *event) override;

  void setMousePos(double x, double y, bool add, bool show);

  //---

  void updateObjectsData();

  // bones
  void updateObjectBones(CBBox3D &bbox);

  void drawObjectsBones();
  void drawObjectBones(CQNewGLModelObject *objectData);

  //---

  void updateCameraBuffer();

  //---

  // axes
  void updateAxes();
  void drawAxes();

  //---

  // bones
  void addBonesCube(CQNewGLModelObject *objectData, const CMatrix3D &m, double cubeSize, int nodeId,
                    const CRGBA &color, bool selected, int &pos, CBBox3D &bbox) const;
  void addBonesLine(CQNewGLModelObject *objectData, const CPoint3D &c1, const CPoint3D &c2,
                    double size, int boneId, int parentBoneId, const CRGBA &color, int &pos) const;

  //---

  std::vector<CGeomObject3D *> getRootObjects() const;

 private:
  void updateNodeMatrices(CQNewGLModelObject *objectData);

 private:
  struct PaintData {
    CGLVector3D viewPos;
    CGLMatrix3D projection;
    CGLMatrix3D view;

    std::vector<CMatrix3D>  nodeMatrices;
    std::vector<QMatrix4x4> nodeQMatrices;
    int                     numNodeMatrices { 128 };
  };

  bool initialized_ { false };

  // scene
  CVector3D sceneSize_     { 1.0, 1.0, 1.0 };
  double    sceneScale_    { 1.0 };
  CPoint3D  sceneCenter_   { 0, 0, 0 };

  PaintData paintData_;

  int pixelWidth_  { 0 };
  int pixelHeight_ { 0 };

  double aspect_ { 1.0 };

  // camera
  CQNewGLCamera* camera_ { nullptr };

  // bones
  struct BonesData {
    bool           enabled       { false };
    bool           showPoints    { false };
    bool           showVertices  { false };
    BonesTransform transform     { BonesTransform::INVERSE_BIND };
    CRGBA          color         { 1.0, 0.0, 0.0 };
    CRGBA          selectedColor { 1.0, 1.0, 1.0 };
  };

  BonesData bonesData_;

  // axes
  CQNewGLAxes* axes_ { nullptr };
};

#endif
