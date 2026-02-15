#include <TextureCoordNode.h>
#include <App.h>
#include <NodeArea.h>
#include <NodeValue.h>
#include <UI.h>
#include <Util.h>

#include <CQColorEdit.h>

namespace CQTextureGen {

TextureCoordNode::
TextureCoordNode(App *app) :
 AppNode(app, "TextureCoord")
{
  gui_ = new TextureCoordGUI(this);

  setChild(gui_);
  setWindowTitle(QString::fromStdString(name()));

  fitChild();

  coordXOutput_ = addOutputT(new TextureCoordNode::CoordXOutput(this));
  coordYOutput_ = addOutputT(new TextureCoordNode::CoordYOutput(this));
}

void
TextureCoordNode::
process()
{
}

//---

TextureCoordNode::CoordXOutput::
CoordXOutput(TextureCoordNode *node) :
 NodeOutput("X"), node_(node)
{
}

CPoint2D
TextureCoordNode::CoordXOutput::
pos() const
{
  auto *gui = node_->gui();

  return outputPos(gui, gui->coordXLabel());
}

NodeValue *
TextureCoordNode::CoordXOutput::
getValue()
{
  std::cerr << "TextureCoordNode::CoordXOutput::getValue\n";

  if (! value_)
    value_ = new RealArrayValue;

  int is = node_->app()->textureSize();

  value_->resize(is*is);

  int ii = 0;

  for (int iy = 0; iy < is; ++iy) {
    for (int ix = 0; ix < is; ++ix) {
      auto x = double(ix)/double(is - 1);

      value_->setReal(ii, x);

      ++ii;
    }
  }

  return value_;
}

//---

TextureCoordNode::CoordYOutput::
CoordYOutput(TextureCoordNode *node) :
 NodeOutput("Y"), node_(node)
{
}

CPoint2D
TextureCoordNode::CoordYOutput::
pos() const
{
  auto *gui = node_->gui();

  return outputPos(gui, gui->coordYLabel());
}

NodeValue *
TextureCoordNode::CoordYOutput::
getValue()
{
  std::cerr << "TextureCoordNode::CoordXOutput::getValue\n";

  if (! value_)
    value_ = new RealArrayValue;

  int is = node_->app()->textureSize();

  value_->resize(is*is);

  int ii = 0;

  for (int iy = 0; iy < is; ++iy) {
    auto y = double(iy)/double(is - 1);

    for (int ix = 0; ix < is; ++ix) {
      value_->setReal(ii, y);

      ++ii;
    }
  }

  return value_;
}

//---

TextureCoordGUI::
TextureCoordGUI(TextureCoordNode *node) :
 node_(node)
{
  auto *layout = new QVBoxLayout(this);

  UI ui(this, layout);

  coordXLabel_ = ui.addInputLabel("X");
  coordYLabel_ = ui.addInputLabel("Y");

  ui.addStretch();

  connectSlots(true);

  updateWidgets();
}

void
TextureCoordGUI::
updateWidgets()
{
  connectSlots(false);

  connectSlots(true);
}

void
TextureCoordGUI::
connectSlots(bool)
{
}

}
