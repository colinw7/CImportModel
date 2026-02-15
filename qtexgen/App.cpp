#include <App.h>
#include <AppPipe.h>
#include <NodeArea.h>

#include <MaterialView.h>
#include <TextureArea.h>
#include <Overview.h>
#include <ToolBar.h>
#include <Status.h>

#include <CameraControl.h>
#include <LightControl.h>
#include <MaterialNode.h>

#include <NoiseTextureNode.h>
#include <MarbleTextureNode.h>
#include <ImageTextureNode.h>
#include <MixNode.h>

#include <XML.h>
#include <UI.h>

#include <CQTabSplit.h>

#include <QApplication>
#include <QVBoxLayout>

#define Q(x) #x
#define QUOTE(x) Q(x)

namespace CQTextureGen {

App::
App()
{
  setObjectName("app");

  buildDir_ = QUOTE(BUILD_DIR);

  //---

  auto *layout = new QVBoxLayout(this);

  toolbar_ = new ToolBar(this);

  layout->addWidget(toolbar_);

  //---

  auto *cframe  = new QFrame;
  auto *clayout = new QHBoxLayout(cframe);

  layout->addWidget(cframe);

  //---

  auto *tab = new CQTabSplit;
  tab->setState(CQTabSplit::State::TAB);
  tab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  clayout->addWidget(tab);

  //---

  nodeArea_     = new NodeArea(this);
  materialView_ = new MaterialView(this);
  overview_     = new Overview(this);

  tab->addWidget(nodeArea_    , "Nodes"   );
  tab->addWidget(materialView_, "View"    );
  tab->addWidget(overview_    , "Overview");

  //---

  auto *controlTab = new CQTabSplit;
  controlTab->setState(CQTabSplit::State::TAB);
  controlTab->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  clayout->addWidget(controlTab);

  //---

  textureArea_   = new TextureArea(this);
  lightControl_  = new LightControl(this);
  cameraControl_ = new CameraControl(this);

  controlTab->addWidget(textureArea_  , "Textures");
  controlTab->addWidget(lightControl_ , "Lights"  );
  controlTab->addWidget(cameraControl_, "Camera"  );

  //---

  auto *materialNode = addNodeT<MaterialNode>();
  materialNode->setId("material");

  //---

  updateImages();

  //---

  status_ = new Status(this);

  layout->addWidget(status_);

  //---

  processNodes();
}

void
App::
setTextureSize(int i)
{
  if (i != textureSize_) {
    textureSize_ = i;

    updateImages();
  }
}

void
App::
updateImages()
{
  auto makeImage = [&]() { return QImage(textureSize(), textureSize(), QImage::Format_ARGB32); };

  diffuseImage_ .image = makeImage();
  normalImage_  .image = makeImage();
  specularImage_.image = makeImage();
  emissionImage_.image = makeImage();
  metallicImage_.image = makeImage();
  ORMImage_     .image = makeImage();
}

bool
App::
loadXML(const std::string &filename)
{
  XML xml(this);

  if (! xml.read(filename))
    return false;

  processNodes();

  return true;
}

void
App::
addNode(AppNode *node)
{
  nodes_.push_back(node);

  connect(node, SIGNAL(geometryChanged()), nodeArea_, SLOT(update()));

  node->setVisible(true);

  placeNodes();
}

void
App::
removeNode(AppNode *node)
{
  nodes_.erase(std::remove(nodes_.begin(), nodes_.end(), node), nodes_.end());

  placeNodes();
}

AppNode *
App::
getNodeById(const std::string &id) const
{
  for (auto *node : nodes_) {
    if (node->id() == id)
      return node;
  }

  return nullptr;
}

void
App::
placeNodes()
{
  int left = 40;
  int top  = 60;

  int dx = 200;
  int dy = 200;

  int x = left;
  int y = top;

  //---

  bool hasInput = false;

  for (auto *node : nodes_) {
    if (node->isMoved())
      continue;

    if (node->inputs().empty()) {
      node->setPos(x, y);

      y += dy;

      hasInput = true;
    }
  }

  if (hasInput) {
    x += dx;
    y  = top;
  }

  //---

  bool hasIO = false;

  for (auto *node : nodes_) {
    if (node->isMoved())
      continue;

    if (! node->outputs().empty() && ! node->inputs().empty()) {
      node->setPos(x, y);

      y += dy;

      hasIO = true;
    }
  }

  if (hasIO) {
    x += dx;
    y  = top;
  }

  //---

  for (auto *node : nodes_) {
    if (node->isMoved())
      continue;

    if (node->outputs().empty() && ! node->inputs().empty()) {
      node->setPos(x, y);

      y += dy;
    }
  }
}

AppPipe *
App::
addPipe(const std::string &name, NodeOutput *output, NodeInput *input)
{
  auto *pipe = new AppPipe(name, output, input);

  pipes_.push_back(pipe);

  output->addPipe(pipe);
  input ->addPipe(pipe);

  output->node()->addPipe(pipe);
  input ->node()->addPipe(pipe);

  return pipe;
}

void
App::
removePipe(AppPipe *pipe)
{
  pipes_.erase(std::remove(pipes_.begin(), pipes_.end(), pipe), pipes_.end());

  pipe->output()->removePipe(pipe);
  pipe->input ()->removePipe(pipe);

  pipe->output()->node()->removePipe(pipe);
  pipe->input ()->node()->removePipe(pipe);

  delete pipe;
}

void
App::
processNodes()
{
  for (auto *node : nodes_) {
    for (auto *output : node->outputs())
      output->setProcessed(false);

    for (auto *input : node->inputs())
      input->setProcessed(false);

    node->setProcessed(false);

    node->reset();
  }

  for (auto *pipe : pipes_) {
    pipe->setInputProcessed (false);
    pipe->setOutputProcessed(false);
  }

#if 0
  for (auto *pipe : pipes_)
    processPipeInput(pipe);

  for (auto *pipe : pipes_)
    processPipeOutput(pipe);
#else
  for (auto *node : nodes_) {
    processAppNode(node);
  }
#endif

  textureArea_->update();

  Q_EMIT materialChanged();
}

void
App::
processPipeInput(AppPipe *pipe)
{
  if (pipe->isInputProcessed())
    return;

  pipe->setInputProcessed(true);

  processNodeIO(pipe->input());
}

void
App::
processPipeOutput(AppPipe *pipe)
{
  if (pipe->isOutputProcessed())
    return;

  pipe->setOutputProcessed(true);

  processNodeIO(pipe->output());

  pipe->process();
}

void
App::
processNodeIO(NodeIO *node)
{
  processAppNode(node->node());
}

void
App::
processNodeInput(NodeInput *input)
{
  for (auto *pipe : input->pipes())
    processPipeOutput(pipe);
}

void
App::
processAppNode(AppNode *node)
{
  if (node->isProcessed())
    return;

  std::cerr << "Process App Node : " << node->name() << "\n";

  node->setProcessed(true);

  for (auto *pipe : node->pipes())
    processPipeInput(pipe);

  for (auto *pipe : node->pipes())
    processPipeOutput(pipe);

  node->process();
}

}
