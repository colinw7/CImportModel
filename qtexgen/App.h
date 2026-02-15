#ifndef App_H
#define App_H

#include <CRGBA.h>

#include <QFrame>

namespace CQTextureGen {

//---

class AppNode;
class AppPipe;

class MaterialView;
class TextureArea;
class Overview;
class Status;
class ToolBar;

class NodeArea;
class LightControl;
class CameraControl;

class MaterialNode;
class NodeIO;
class NodeInput;
class NodeOutput;
class NodeValue;

class NoiseTextureNode;
class MarbleTextureNode;
class ImageTextureNode;
class MixNode;

class App : public QFrame {
  Q_OBJECT

 public:
  using Nodes = std::vector<AppNode *>;
  using Pipes = std::vector<AppPipe *>;

 public:
  App();

  //---

  const QString &buildDir() const { return buildDir_; }
  void setBuildDir(const QString &s) { buildDir_ = s; }

  int textureSize() const { return textureSize_; }
  void setTextureSize(int i);

  //---

  const Nodes &nodes() const { return nodes_; }
  const Pipes &pipes() const { return pipes_; }

  NodeArea*      nodeArea     () const { return nodeArea_     ; }
  MaterialView*  materialView () const { return materialView_ ; }
  Overview*      overview     () const { return overview_     ; }
  TextureArea*   textureArea  () const { return textureArea_  ; }
  LightControl*  lightControl () const { return lightControl_ ; }
  CameraControl* cameraControl() const { return cameraControl_; }

  Status* status() const { return status_; }

  //---

  QImage &diffuseImage() { return diffuseImage_.image; }

  bool isDiffuseImageSet() const { return diffuseImage_.set; }
  void setDiffuseImageSet(bool b) { diffuseImage_.set = b; }

  const CRGBA &diffuseColor() const { return diffuseColor_; }
  void setDiffuseColor(const CRGBA &v) { diffuseColor_ = v; }

  QImage &normalImage() { return normalImage_.image; }

  bool isNormalImageSet() const { return normalImage_.set; }
  void setNormalImageSet(bool b) { normalImage_.set = b; }

  QImage &specularImage() { return specularImage_.image; }

  bool isSpecularImageSet() const { return specularImage_.set; }
  void setSpecularImageSet(bool b) { specularImage_.set = b; }

  QImage &emissionImage() { return emissionImage_.image; }

  bool isEmissionImageSet() const { return emissionImage_.set; }
  void setEmissionImageSet(bool b) { emissionImage_.set = b; }

  QImage &metallicImage() { return metallicImage_.image; }

  bool isMetallicImageSet() const { return metallicImage_.set; }
  void setMetallicImageSet(bool b) { metallicImage_.set = b; }

  QImage &ORMImage() { return ORMImage_.image; }

  bool isORMImageSet() const { return ORMImage_.set; }
  void setORMImageSet(bool b) { ORMImage_.set = b; }

  //---

  bool loadXML(const std::string &filename);

  //---

  template<typename T>
  T *addNodeT() {
    auto *node = new T(this);
    addNode(node);
    return node;
  }

  void addNode(AppNode *node);
  void removeNode(AppNode *node);

  AppNode *getNodeById(const std::string &id) const;

  void placeNodes();

  //---

  AppPipe *addPipe(const std::string &name, NodeOutput *output, NodeInput *input);
  void removePipe(AppPipe *pipe);

  //---

  void processNodes();
  void processPipeInput(AppPipe *pipe);
  void processPipeOutput(AppPipe *pipe);
  void processNodeIO(NodeIO *node);
  void processNodeInput(NodeInput *node);
  void processAppNode(AppNode *node);

  QSize sizeHint() const override { return QSize(2048, 1760); }

 private:
  void updateImages();

 Q_SIGNALS:
  void materialChanged();

 public:
  QString buildDir_;

  Nodes nodes_;
  Pipes pipes_;

  NodeArea* nodeArea_ { nullptr };

  MaterialView* materialView_ { nullptr };
  TextureArea*  textureArea_  { nullptr };
  Overview*     overview_      { nullptr };

  Status*  status_  { nullptr };
  ToolBar* toolbar_ { nullptr };

  LightControl*  lightControl_  { nullptr };
  CameraControl* cameraControl_ { nullptr };

  struct ImageData {
    QImage image;
    bool   set { false };
  };

  int textureSize_ { 256 };

  ImageData diffuseImage_;
  CRGBA     diffuseColor_    { CRGBA::white() };
  ImageData normalImage_;
  ImageData specularImage_;
  ImageData emissionImage_;
  ImageData metallicImage_;
  ImageData ORMImage_;
};

}

#endif
