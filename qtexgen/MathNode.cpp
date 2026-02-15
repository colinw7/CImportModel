#include <MathNode.h>
#include <App.h>
#include <NodeArea.h>
#include <NodeValue.h>
#include <Util.h>
#include <UI.h>

#include <QVBoxLayout>
#include <QComboBox>

namespace {

class MathTypeInd : public ValueMap<CQTextureGen::MathNode::MathType, int> {
 public:
  MathTypeInd() {
    add("None", CQTextureGen::MathNode::MathType::NONE, 0);
    add("Cos" , CQTextureGen::MathNode::MathType::COS , 1);
    add("Sin" , CQTextureGen::MathNode::MathType::SIN , 2);
    add("Tan" , CQTextureGen::MathNode::MathType::TAN , 3);
  };

  CQTextureGen::MathNode::MathType indToType(int ind) {
    return lookup(ind);
  }

  int typeToInd(const CQTextureGen::MathNode::MathType &type) {
    return lookup(type);
  }
};

MathTypeInd mathTypeInd;

}

//---

namespace CQTextureGen {

MathNode::
MathNode(App *app) :
 AppNode(app, "Math")
{
  gui_ = new MathGUI(this);

  setChild(gui_);
  setWindowTitle(QString::fromStdString(name()));

  fitChild();

  valueInput_  = addInputT (new ValueInput (this));
  valueOutput_ = addOutputT(new ValueOutput(this));
}

void
MathNode::
setMathTypeFromString(const std::string &str)
{
  setMathType(mathTypeInd.fromString(QString::fromStdString(str)));

  gui_->updateWidgets();
}

NodeValue *
MathNode::
calcValue()
{
  std::cerr << "MathNode::calcValue\n";

  app_->processNodeInput(valueInput_);

  auto *value = valueInput_->getValue();

  if (value) {
    if (! value_)
      value_ = new RealArrayValue;

    value_->resize(value->numValues());

    for (uint ii = 0; ii < value->numValues(); ++ii) {
      auto r = value->realValue(ii);

      if      (mathType() == MathType::COS)
        value_->setReal(ii, std::cos(r));
      else if (mathType() == MathType::SIN)
        value_->setReal(ii, std::cos(r));
      else if (mathType() == MathType::TAN)
        value_->setReal(ii, std::tan(r));
      else
        value_->setReal(ii, r);
    }

    return value_;
  }
  else
    return nullptr;
}

void
MathNode::
process()
{
}

//---

MathNode::ValueInput::
ValueInput(MathNode *node) :
 NodeInput("Input"), node_(node)
{
}

CPoint2D
MathNode::ValueInput::
pos() const
{
  auto *gui = node_->gui();

  return inputPos(gui, gui->inputLabel());
}

//---

MathNode::ValueOutput::
ValueOutput(MathNode *node) :
 NodeOutput("Output"), node_(node)
{
}

NodeValue *
MathNode::ValueOutput::
getValue()
{
  return node_->calcValue();
}

CPoint2D
MathNode::ValueOutput::
pos() const
{
  auto *gui = node_->gui();

  return outputPos(gui, gui->valueLabel());
}

//---

MathGUI::
MathGUI(MathNode *node) :
 node_(node)
{
  auto *layout = new QVBoxLayout(this);

  UI ui(this, layout);

  typeCombo_ = ui.addLabelEdit("Type", new QComboBox);
  typeCombo_->addItems(mathTypeInd.names());

  inputLabel_ = ui.addInputLabel ("Input");
  valueLabel_ = ui.addOutputLabel("Output");

  ui.addStretch();

  connectSlots(true);

  updateWidgets();
}

void
MathGUI::
updateWidgets()
{
  connectSlots(false);

  typeCombo_->setCurrentIndex(mathTypeInd.typeToInd(node_->mathType()));

  connectSlots(true);
}

void
MathGUI::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, typeCombo_, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(typeSlot(int)));
}

void
MathGUI::
typeSlot(int ind)
{
  node_->setMathType(mathTypeInd.indToType(ind));
}

}
