#include <MixNode.h>
#include <App.h>
#include <NodeArea.h>
#include <NodeValue.h>
#include <UI.h>

#include <CQRealSpin.h>

#include <QVBoxLayout>

namespace CQTextureGen {

MixNode::
MixNode(App *app) :
 AppNode(app, "Mix")
{
  gui_ = new MixGUI(this);

  setChild(gui_);
  setWindowTitle(QString::fromStdString(name()));

  fitChild();

  valueInput1_ = addInputT (new ValueInput1(this));
  valueInput2_ = addInputT (new ValueInput2(this));
  valueOutput_ = addOutputT(new ValueOutput(this));
}

NodeValue *
MixNode::
calcValue()
{
  std::cerr << "MixNode::calcValue\n";

  auto *value1 = valueInput1_->getValue();
  auto *value2 = valueInput2_->getValue();

  if (! value1 || ! value2)
    return nullptr;

  if (value1 && value2) {
    if (! value_)
      value_ = new ColorArrayValue;

    int is = app_->textureSize();

    value_->resize(is*is);

    int ii = 0;

    for (int iy = 0; iy < is; ++iy) {
      for (int ix = 0; ix < is; ++ix) {
        auto c1 = value1->colorValue(ii);
        auto c2 = value2->colorValue(ii);

        value_->setColor(ii, c1.blended(c2, factor_));

        ++ii;
      }
    }

    return value_;
  }
  else
    return nullptr;
}

void
MixNode::
process()
{
}

//---

MixNode::ValueInput1::
ValueInput1(MixNode *node) :
 NodeInput("Input1"), node_(node)
{
}

CPoint2D
MixNode::ValueInput1::
pos() const
{
  auto *gui = node_->gui();

  return inputPos(gui, gui->input1Label());
}

//---

MixNode::ValueInput2::
ValueInput2(MixNode *node) :
 NodeInput("Input2"), node_(node)
{
}

CPoint2D
MixNode::ValueInput2::
pos() const
{
  auto *gui = node_->gui();

  return inputPos(gui, gui->input2Label());
}

//---

MixNode::ValueOutput::
ValueOutput(MixNode *node) :
 NodeOutput("Value"), node_(node)
{
}

NodeValue *
MixNode::ValueOutput::
getValue()
{
  return node_->calcValue();
}

CPoint2D
MixNode::ValueOutput::
pos() const
{
  auto *gui = node_->gui();

  return outputPos(gui, gui->valueLabel());
}

//---

MixGUI::
MixGUI(MixNode *node) :
 node_(node)
{
  auto *layout = new QVBoxLayout(this);

  UI ui(this, layout);

  factorEdit_ = ui.addLabelEdit("Factor", new CQRealSpin);

  input1Label_ = ui.addInputLabel("Input1");
  input2Label_ = ui.addInputLabel("Input2");

  valueLabel_  = ui.addOutputLabel("Value");

  ui.addStretch();

  connectSlots(true);

  updateWidgets();
}

void
MixGUI::
updateWidgets()
{
  connectSlots(false);

  factorEdit_->setValue(node_->factor());

  connectSlots(true);
}

void
MixGUI::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, factorEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(factorSlot(double)));
}

void
MixGUI::
factorSlot(double r)
{
  node_->setFactor(r);
}

}
