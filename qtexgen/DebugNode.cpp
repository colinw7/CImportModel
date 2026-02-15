#include <DebugNode.h>
#include <App.h>
#include <NodeArea.h>
#include <NodeValue.h>
#include <UI.h>
#include <Util.h>

#include <CQColorEdit.h>

namespace CQTextureGen {

DebugNode::
DebugNode(App *app) :
 AppNode(app, "Debug")
{
  gui_ = new DebugGUI(this);

  setChild(gui_);
  setWindowTitle(QString::fromStdString(name()));

  fitChild();

  input_ = addInputT(new DebugNode::Input (this));
}

void
DebugNode::
process()
{
  std::cerr << "Debug::process\n";

  auto *value  = input_->getValue();
  auto *colors = dynamic_cast<ColorArrayValue *>(value);
  auto *reals  = dynamic_cast<RealArrayValue *>(value);

  if      (colors) {
    std::cerr << "colors: " << colors->numValues() << "\n";
  }
  else if (reals) {
    std::cerr << "reals: " << reals->numValues() << "\n";

    for (uint ii = 0; ii < reals->numValues(); ++ii)
      std::cerr << " " << reals->realValue(ii);
    std::cerr << "\n";
  }
}

//---

DebugNode::Input::
Input(DebugNode *node) :
 NodeInput("Input"), node_(node)
{
}

CPoint2D
DebugNode::Input::
pos() const
{
  auto *gui = node_->gui();

  return inputPos(gui, gui->inputLabel());
}

//---

DebugGUI::
DebugGUI(DebugNode *node) :
 node_(node)
{
  auto *layout = new QVBoxLayout(this);

  UI ui(this, layout);

  inputLabel_ = ui.addInputLabel("Input");

  ui.addStretch();

  connectSlots(true);

  updateWidgets();
}

void
DebugGUI::
updateWidgets()
{
  connectSlots(false);

  connectSlots(true);
}

void
DebugGUI::
connectSlots(bool)
{
}

}
