#include <ColorNode.h>
#include <App.h>
#include <NodeArea.h>
#include <NodeValue.h>
#include <Util.h>
#include <UI.h>

#include <CQRealSpin.h>

#include <QVBoxLayout>

namespace CQTextureGen {

ColorNode::
ColorNode(App *app) :
 AppNode(app, "Color")
{
  gui_ = new ColorGUI(this);

  setChild(gui_);
  setWindowTitle(QString::fromStdString(name()));

  fitChild();

  redInput_    = addInputT (new RedInput   (this));
  greenInput_  = addInputT (new GreenInput (this));
  blueInput_   = addInputT (new BlueInput  (this));
  alphaInput_  = addInputT (new AlphaInput (this));
  colorOutput_ = addOutputT(new ColorOutput(this));
}

NodeValue *
ColorNode::
calcValue()
{
  std::cerr << "ColorNode::calcValue\n";

  app_->processNodeInput(redInput_);
  app_->processNodeInput(greenInput_);
  app_->processNodeInput(blueInput_);
  app_->processNodeInput(alphaInput_);

  auto *redValue   = redInput_  ->getValue();
  auto *greenValue = greenInput_->getValue();
  auto *blueValue  = blueInput_ ->getValue();
  auto *alphaValue = alphaInput_->getValue();

  if (redValue || greenValue || blueValue || alphaValue) {
    if (! value_)
      value_ = new ColorArrayValue;

    auto nr = (redValue   ? redValue  ->numValues() : 0);
    auto ng = (greenValue ? greenValue->numValues() : 0);
    auto nb = (blueValue  ? blueValue ->numValues() : 0);
    auto na = (alphaValue ? alphaValue->numValues() : 0);

    auto nc = std::max(nr, std::max(ng, nb));

    value_->resize(nc);

    for (uint i = 0; i < nc; ++i) {
      double r = (redValue   && i < nr ? redValue  ->realValue(i) : gui_->redEdit  ()->value());
      double g = (greenValue && i < ng ? greenValue->realValue(i) : gui_->greenEdit()->value());
      double b = (blueValue  && i < nb ? blueValue ->realValue(i) : gui_->blueEdit ()->value());
      double a = (alphaValue && i < na ? alphaValue->realValue(i) : gui_->alphaEdit()->value());

      value_->setColor(i, CRGBA(r, g, b, a).clamped());
    }

    return value_;
  }
  else
    return nullptr;
}

void
ColorNode::
process()
{
}

//---

ColorNode::RedInput::
RedInput(ColorNode *node) :
 NodeInput("R"), node_(node)
{
}

CPoint2D
ColorNode::RedInput::
pos() const
{
  auto *gui = node_->gui();

  return inputPos(gui, gui->redEdit());
}

//---

ColorNode::GreenInput::
GreenInput(ColorNode *node) :
 NodeInput("G"), node_(node)
{
}

CPoint2D
ColorNode::GreenInput::
pos() const
{
  auto *gui = node_->gui();

  return inputPos(gui, gui->greenEdit());
}

//---

ColorNode::BlueInput::
BlueInput(ColorNode *node) :
 NodeInput("B"), node_(node)
{
}

CPoint2D
ColorNode::BlueInput::
pos() const
{
  auto *gui = node_->gui();

  return inputPos(gui, gui->blueEdit());
}

//---

ColorNode::AlphaInput::
AlphaInput(ColorNode *node) :
 NodeInput("A"), node_(node)
{
}

CPoint2D
ColorNode::AlphaInput::
pos() const
{
  auto *gui = node_->gui();

  return inputPos(gui, gui->alphaEdit());
}

//---

ColorNode::ColorOutput::
ColorOutput(ColorNode *node) :
 NodeOutput("Color"), node_(node)
{
}

NodeValue *
ColorNode::ColorOutput::
getValue()
{
  return node_->calcValue();
}

CPoint2D
ColorNode::ColorOutput::
pos() const
{
  auto *gui = node_->gui();

  return outputPos(gui, gui->colorLabel());
}

//---

ColorGUI::
ColorGUI(ColorNode *node) :
 node_(node)
{
  auto *layout = new QVBoxLayout(this);

  UI ui(this, layout);

  redEdit_   = ui.addLabelEdit("R", new CQRealSpin);
  greenEdit_ = ui.addLabelEdit("G", new CQRealSpin);
  blueEdit_  = ui.addLabelEdit("B", new CQRealSpin);
  alphaEdit_ = ui.addLabelEdit("A", new CQRealSpin);

  alphaEdit_->setValue(1.0);

  colorLabel_ = ui.addOutputLabel("Color");

  ui.addStretch();

  connectSlots(true);

  updateWidgets();
}

void
ColorGUI::
updateWidgets()
{
  connectSlots(false);

  connectSlots(true);
}

void
ColorGUI::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, redEdit_, SIGNAL(realValueChanged(double)),
                           node_, SLOT(invalidate()));
  CQUtil::connectDisconnect(b, greenEdit_, SIGNAL(realValueChanged(double)),
                           node_, SLOT(invalidate()));
  CQUtil::connectDisconnect(b, blueEdit_, SIGNAL(realValueChanged(double)),
                           node_, SLOT(invalidate()));
  CQUtil::connectDisconnect(b, alphaEdit_, SIGNAL(realValueChanged(double)),
                           node_, SLOT(invalidate()));
}

}
