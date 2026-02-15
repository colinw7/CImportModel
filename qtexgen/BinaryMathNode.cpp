#include <BinaryMathNode.h>
#include <App.h>
#include <NodeArea.h>
#include <NodeValue.h>
#include <Util.h>
#include <UI.h>

#include <CQRealSpin.h>

#include <QVBoxLayout>
#include <QComboBox>

namespace {

class MathTypeInd : public ValueMap<CQTextureGen::BinaryMathNode::MathType, int> {
 public:
  MathTypeInd() {
    add("None"    , CQTextureGen::BinaryMathNode::MathType::NONE    , 0);
    add("Add"     , CQTextureGen::BinaryMathNode::MathType::ADD     , 1);
    add("Subtracr", CQTextureGen::BinaryMathNode::MathType::SUBTRACT, 2);
    add("Multiply", CQTextureGen::BinaryMathNode::MathType::MULTIPLY, 3);
    add("Divide"  , CQTextureGen::BinaryMathNode::MathType::DIVIDE  , 4);
  };

  CQTextureGen::BinaryMathNode::MathType indToType(int ind) {
    return lookup(ind);
  }

  int typeToInd(const CQTextureGen::BinaryMathNode::MathType &type) {
    return lookup(type);
  }
};

MathTypeInd mathTypeInd;

}

//---

namespace CQTextureGen {

BinaryMathNode::
BinaryMathNode(App *app) :
 AppNode(app, "Binary Math")
{
  gui_ = new BinaryMathGUI(this);

  setChild(gui_);
  setWindowTitle(QString::fromStdString(name()));

  fitChild();

  value1Input_ = addInputT (new Value1Input(this));
  value2Input_ = addInputT (new Value2Input(this));
  valueOutput_ = addOutputT(new ValueOutput(this));
}

void
BinaryMathNode::
setMathTypeFromString(const std::string &str)
{
  setMathType(mathTypeInd.fromString(QString::fromStdString(str)));

  gui_->updateWidgets();
}

NodeValue *
BinaryMathNode::
calcValue()
{
  std::cerr << "BinaryMathNode::calcValue\n";

  app_->processNodeInput(value1Input_);
  app_->processNodeInput(value2Input_);

  auto *value1 = value1Input_->getValue();
  auto *value2 = value2Input_->getValue();

  if (value1 || value2) {
    if (! value_)
      value_ = new RealArrayValue;

    auto n1 = (value1 ? value1->numValues() : 0);
    auto n2 = (value2 ? value2->numValues() : 0);

    auto n = std::max(n1, n2);

    value_->resize(n);

    for (uint ii = 0; ii < n; ++ii) {
      auto r1 = (value1 ? value1->realValue(ii) : gui_->input1Edit()->value());
      auto r2 = (value2 ? value2->realValue(ii) : gui_->input2Edit()->value());

      if      (mathType() == MathType::ADD)
        value_->setReal(ii, r1 + r2);
      else if (mathType() == MathType::SUBTRACT)
        value_->setReal(ii, r1 - r2);
      else if (mathType() == MathType::MULTIPLY)
        value_->setReal(ii, r1*r2);
      else if (mathType() == MathType::DIVIDE)
        value_->setReal(ii, r1/r2);
      else
        value_->setReal(ii, r1);
    }

    return value_;
  }
  else
    return nullptr;
}

void
BinaryMathNode::
process()
{
}

//---

BinaryMathNode::Value1Input::
Value1Input(BinaryMathNode *node) :
 NodeInput("LHS"), node_(node)
{
}

CPoint2D
BinaryMathNode::Value1Input::
pos() const
{
  auto *gui = node_->gui();

  return inputPos(gui, gui->input1Edit());
}

//---

BinaryMathNode::Value2Input::
Value2Input(BinaryMathNode *node) :
 NodeInput("RHS"), node_(node)
{
}

CPoint2D
BinaryMathNode::Value2Input::
pos() const
{
  auto *gui = node_->gui();

  return inputPos(gui, gui->input2Edit());
}

//---

BinaryMathNode::ValueOutput::
ValueOutput(BinaryMathNode *node) :
 NodeOutput("Output"), node_(node)
{
}

NodeValue *
BinaryMathNode::ValueOutput::
getValue()
{
  return node_->calcValue();
}

CPoint2D
BinaryMathNode::ValueOutput::
pos() const
{
  auto *gui = node_->gui();

  return outputPos(gui, gui->valueLabel());
}

//---

BinaryMathGUI::
BinaryMathGUI(BinaryMathNode *node) :
 node_(node)
{
  auto *layout = new QVBoxLayout(this);

  UI ui(this, layout);

  typeCombo_ = ui.addLabelEdit("Type", new QComboBox);
  typeCombo_->addItems(mathTypeInd.names());

  input1Edit_ = ui.addLabelEdit  ("LHS", new CQRealSpin);
  input2Edit_ = ui.addLabelEdit  ("RHS", new CQRealSpin);
  valueLabel_ = ui.addOutputLabel("Output");

  ui.addStretch();

  connectSlots(true);

  updateWidgets();
}

void
BinaryMathGUI::
updateWidgets()
{
  connectSlots(false);

  typeCombo_->setCurrentIndex(mathTypeInd.typeToInd(node_->mathType()));

  connectSlots(true);
}

void
BinaryMathGUI::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, typeCombo_, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(typeSlot(int)));
}

void
BinaryMathGUI::
typeSlot(int ind)
{
  node_->setMathType(mathTypeInd.indToType(ind));
}

}
