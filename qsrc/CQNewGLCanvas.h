#ifndef CQNewGLCanvas_H
#define CQNewGLCanvas_H

#include <CGeom3DType.h>
#include <CGLVector3D.h>
#include <CPoint3D.h>

#include <QGLWidget>

class CQNewGLModel;
class CImportBase;
class CQGLBuffer;
class CGLTexture;
class CGLCamera;
class CGeomObject3D;

class QOpenGLShaderProgram;

class CQNewGLCanvas : public QGLWidget {
  Q_OBJECT

  Q_PROPERTY(QColor bgColor  READ bgColor    WRITE setBgColor)

  Q_PROPERTY(bool depthTest   READ isDepthTest   WRITE setDepthTest  )
  Q_PROPERTY(bool cullFace    READ isCullFace    WRITE setCullFace   )
  Q_PROPERTY(bool frontFace   READ isFrontFace   WRITE setFrontFace  )
  Q_PROPERTY(bool polygonLine READ isPolygonLine WRITE setPolygonLine)

 public:
  CQNewGLCanvas(CQNewGLModel *app);

  bool isFlipYZ() const { return flipYZ_; }
  void setFlipYZ(bool b) { flipYZ_ = b; }

  const QColor &bgColor() const { return bgColor_; }
  void setBgColor(const QColor &c);

  bool isDepthTest() const { return depthTest_; }
  void setDepthTest(bool b);

  bool isCullFace() const { return cullFace_; }
  void setCullFace(bool b);

  bool isFrontFace() const { return frontFace_; }
  void setFrontFace(bool b);

  bool isPolygonLine() const { return polygonLine_; }
  void setPolygonLine(bool b);

  bool loadModel(const QString &modelName, CGeom3DType format);

  void initializeGL() override;

  void paintGL() override;

  void resizeGL(int, int) override;

  void resetRange();

  void mousePressEvent  (QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent   (QMouseEvent *event) override;

  void wheelEvent(QWheelEvent *) override;

  void keyPressEvent(QKeyEvent *event) override;

 private:
  GLuint loadTexture(const std::string &name);

  void updateObjectData();

 private:
  struct FaceData {
    int         pos             { 0 };
    int         len             { 0 };
    CGLTexture *diffuseTexture  { nullptr };
    CGLTexture *specularTexture { nullptr };
    CGLTexture *normalTexture   { nullptr };
  };

  using FaceDatas = std::vector<FaceData>;

  FaceDatas faceDatas;

  struct ObjectData {
    CQGLBuffer* buffer { nullptr };
    FaceDatas   faceDatas;
  };

  using ObjectDatas = std::map<CGeomObject3D*, ObjectData *>;
  using GLTextures  = std::map<int, CGLTexture *>;

  CQNewGLModel*         app_                { nullptr };
  CImportBase*          import_             { nullptr };
  QColor                bgColor_            { 51, 76, 76 };
  bool                  depthTest_          { true };
  bool                  cullFace_           { false };
  bool                  frontFace_          { true };
  bool                  polygonLine_        { false };
  bool                  flipYZ_             { false };
  float                 sceneScale_         { 1.0 };
  CPoint3D              sceneCenter_        { 0 , 0, 0 };
  CGLCamera*            camera_             { nullptr };
  float                 modelXAngle_        { 0.0f };
  float                 modelYAngle_        { 0.0f };
  float                 modelZAngle_        { 0.0f };
  CGLVector3D           lightPos_           { 0.4f, 0.4f, 0.4f };
  QOpenGLShaderProgram* modelShaderProgram_ { nullptr };
  QOpenGLShaderProgram* lightShaderProgram_ { nullptr };
  CQGLBuffer*           lightBuffer_        { nullptr };
  ObjectDatas           objectDatas_;
  GLTextures            glTextures_;
};

#endif
