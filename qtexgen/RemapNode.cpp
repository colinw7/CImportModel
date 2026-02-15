#include <RemapNode.h>
#include <App.h>
#include <NodeArea.h>
#include <NodeValue.h>
#include <Util.h>
#include <UI.h>

#include <CQRealSpin.h>

#include <QVBoxLayout>

//---

namespace CQTextureGen {

RemapNode::
RemapNode(App *app) :
 AppNode(app, "Remap")
{
  gui_ = new RemapGUI(this);

  setChild(gui_);
  setWindowTitle(QString::fromStdString(name()));

  fitChild();

  valueInput_  = addInputT (new ValueInput (this));
  valueOutput_ = addOutputT(new ValueOutput(this));
}

NodeValue *
RemapNode::
calcValue()
{
  std::cerr << "RemapNode::calcValue\n";

  app_->processNodeInput(valueInput_);

  auto *value = valueInput_->getValue();

  if (! value)
    return nullptr;

  if (value) {
    if (! value_)
      value_ = new RealArrayValue;

    value_->resize(value->numValues());

    for (uint ii = 0; ii < value->numValues(); ++ii) {
      auto r = value->realValue(ii);

      auto r1 = CMathUtil::map(r, fromMin_, fromMax_, toMin_, toMax_);

      value_->setReal(ii, r1);
    }

    return value_;
  }
  else
    return nullptr;
}

void
RemapNode::
process()
{
}

//---

RemapNode::ValueInput::
ValueInput(RemapNode *node) :
 NodeInput("Input"), node_(node)
{
}

CPoint2D
RemapNode::ValueInput::
pos() const
{
  auto *gui = node_->gui();

  return inputPos(gui, gui->inputLabel());
}

//---

RemapNode::ValueOutput::
ValueOutput(RemapNode *node) :
 NodeOutput("Output"), node_(node)
{
}

NodeValue *
RemapNode::ValueOutput::
getValue()
{
  return node_->calcValue();
}

CPoint2D
RemapNode::ValueOutput::
pos() const
{
  auto *gui = node_->gui();

  return outputPos(gui, gui->outputLabel());
}

//---

RemapGUI::
RemapGUI(RemapNode *node) :
 node_(node)
{
  auto *layout = new QVBoxLayout(this);

  UI ui(this, layout);

  inputLabel_ = ui.addInputLabel ("Input");

  fromMinEdit_ = ui.addLabelEdit("From Min", new CQRealSpin);
  fromMaxEdit_ = ui.addLabelEdit("From Max", new CQRealSpin);

  toMinEdit_ = ui.addLabelEdit("To Min", new CQRealSpin);
  toMaxEdit_ = ui.addLabelEdit("To Max", new CQRealSpin);

  outputLabel_ = ui.addOutputLabel("Output");

  ui.addStretch();

  connectSlots(true);

  updateWidgets();
}

void
RemapGUI::
updateWidgets()
{
  connectSlots(false);

  fromMinEdit_->setValue(node_->fromMin());
  fromMaxEdit_->setValue(node_->fromMax());

  toMinEdit_->setValue(node_->toMin());
  toMaxEdit_->setValue(node_->toMax());

  connectSlots(true);
}

void
RemapGUI::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, fromMinEdit_, SIGNAL(realValueChanged(double)),
                           this, SLOT(fromMinSlot(double)));
  CQUtil::connectDisconnect(b, fromMaxEdit_, SIGNAL(realValueChanged(double)),
                           this, SLOT(fromMaxSlot(double)));

  CQUtil::connectDisconnect(b, toMinEdit_, SIGNAL(realValueChanged(double)),
                           this, SLOT(toMinSlot(double)));
  CQUtil::connectDisconnect(b, toMaxEdit_, SIGNAL(realValueChanged(double)),
                           this, SLOT(toMaxSlot(double)));
}

void
RemapGUI::
fromMinSlot(double r)
{
  node_->setFromMin(r);
}

void
RemapGUI::
fromMaxSlot(double r)
{
  node_->setFromMax(r);
}

void
RemapGUI::
toMinSlot(double r)
{
  node_->setToMin(r);
}

void
RemapGUI::
toMaxSlot(double r)
{
  node_->setToMax(r);
}

}
