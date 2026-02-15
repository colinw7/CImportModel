#ifndef DebugNode_H
#define DebugNode_H

#include <AppNode.h>
#include <NodeInput.h>
#include <CRGBA.h>

class CQColorEdit;
class QLabel;

namespace CQTextureGen {

class DebugGUI;

class DebugNode : public AppNode {
 public:
  class Input : public NodeInput {
   public:
    Input(DebugNode *node);

    CPoint2D pos() const override;

   private:
    DebugNode* node_ { nullptr };
  };

 public:
  DebugNode(App *app);

  DebugGUI *gui() const { return gui_; }

  Input *input() const { return input_; }

  void process() override;

 private:
  DebugGUI* gui_ { nullptr };

  Input* input_ { nullptr };
};

//---

class DebugGUI : public QFrame {
  Q_OBJECT

 public:
  DebugGUI(DebugNode *node);

  QLabel *inputLabel() const { return inputLabel_; }

  void updateWidgets();

  void connectSlots(bool b);

 private:
  DebugNode* node_ { nullptr };

  QLabel* inputLabel_ { nullptr };
};

}

#endif
