#include <SeparateColorNode.h>
#include <App.h>
#include <NodeArea.h>
#include <NodeValue.h>
#include <Util.h>
#include <UI.h>

#include <QVBoxLayout>

namespace CQTextureGen {

SeparateColorNode::
SeparateColorNode(App *app) :
 AppNode(app, "Color")
{
  gui_ = new SeparateColorGUI(this);

  setChild(gui_);
  setWindowTitle(QString::fromStdString(name()));

  fitChild();

  colorInput_  = addInputT (new ColorInput (this));
  redOutput_   = addOutputT(new RedOutput  (this));
  greenOutput_ = addOutputT(new GreenOutput(this));
  blueOutput_  = addOutputT(new BlueOutput (this));
  alphaOutput_ = addOutputT(new AlphaOutput(this));
}

void
SeparateColorNode::
calcValues()
{
  std::cerr << "SeparateColorNode::calcValues\n";

  app_->processNodeInput(colorInput_);

  auto *colorValue = colorInput_->getValue();

  if (colorValue) {
    if (! rvalue_) {
      rvalue_ = new RealArrayValue;
      gvalue_ = new RealArrayValue;
      bvalue_ = new RealArrayValue;
      avalue_ = new RealArrayValue;
    }

    auto nc = colorValue->numValues();

    rvalue_->resize(nc);
    gvalue_->resize(nc);
    bvalue_->resize(nc);
    avalue_->resize(nc);

    for (uint i = 0; i < nc; ++i) {
      auto c = colorValue->colorValue(i);

      double r = c.getRed  ();
      double g = c.getGreen();
      double b = c.getBlue ();
      double a = c.getAlpha();

      rvalue_->setReal(i, r);
      gvalue_->setReal(i, g);
      bvalue_->setReal(i, b);
      avalue_->setReal(i, a);
    }
  }
  else {
    delete rvalue_;
    delete gvalue_;
    delete bvalue_;
    delete avalue_;

    rvalue_ = nullptr;
    gvalue_ = nullptr;
    bvalue_ = nullptr;
    avalue_ = nullptr;
  }
}

void
SeparateColorNode::
process()
{
}

//---

SeparateColorNode::ColorInput::
ColorInput(SeparateColorNode *node) :
 NodeInput("Color"), node_(node)
{
}

CPoint2D
SeparateColorNode::ColorInput::
pos() const
{
  auto *gui = node_->gui();

  return inputPos(gui, gui->colorLabel());
}

//---

SeparateColorNode::RedOutput::
RedOutput(SeparateColorNode *node) :
 NodeOutput("R"), node_(node)
{
}

NodeValue *
SeparateColorNode::RedOutput::
getValue()
{
  node_->calcValues();

  return node_->rvalue();
}

CPoint2D
SeparateColorNode::RedOutput::
pos() const
{
  auto *gui = node_->gui();

  return outputPos(gui, gui->redLabel());
}

//---

SeparateColorNode::GreenOutput::
GreenOutput(SeparateColorNode *node) :
 NodeOutput("G"), node_(node)
{
}

NodeValue *
SeparateColorNode::GreenOutput::
getValue()
{
  node_->calcValues();

  return node_->gvalue();
}

CPoint2D
SeparateColorNode::GreenOutput::
pos() const
{
  auto *gui = node_->gui();

  return outputPos(gui, gui->greenLabel());
}

//---

SeparateColorNode::BlueOutput::
BlueOutput(SeparateColorNode *node) :
 NodeOutput("B"), node_(node)
{
}

NodeValue *
SeparateColorNode::BlueOutput::
getValue()
{
  node_->calcValues();

  return node_->bvalue();
}

CPoint2D
SeparateColorNode::BlueOutput::
pos() const
{
  auto *gui = node_->gui();

  return outputPos(gui, gui->blueLabel());
}

//---

SeparateColorNode::AlphaOutput::
AlphaOutput(SeparateColorNode *node) :
 NodeOutput("A"), node_(node)
{
}

NodeValue *
SeparateColorNode::AlphaOutput::
getValue()
{
  node_->calcValues();

  return node_->avalue();
}

CPoint2D
SeparateColorNode::AlphaOutput::
pos() const
{
  auto *gui = node_->gui();

  return outputPos(gui, gui->alphaLabel());
}

//---

SeparateColorGUI::
SeparateColorGUI(SeparateColorNode *node) :
 node_(node)
{
  auto *layout = new QVBoxLayout(this);

  UI ui(this, layout);

  colorLabel_ = ui.addInputLabel("Color");

  redLabel_   = ui.addOutputLabel("R");
  greenLabel_ = ui.addOutputLabel("G");
  blueLabel_  = ui.addOutputLabel("B");
  alphaLabel_ = ui.addOutputLabel("A");

  ui.addStretch();

  connectSlots(true);

  updateWidgets();
}

void
SeparateColorGUI::
updateWidgets()
{
  connectSlots(false);

  connectSlots(true);
}

void
SeparateColorGUI::
connectSlots(bool)
{
}

}
