#ifndef MixNode_H
#define MixNode_H

#include <AppNode.h>
#include <NodeInput.h>
#include <NodeOutput.h>

class CQRealSpin;
class QLabel;

namespace CQTextureGen {

class MixGUI;
class ColorArrayValue;

class MixNode : public AppNode {
 public:
  class ValueInput1 : public NodeInput {
   public:
    ValueInput1(MixNode *node);

    CPoint2D pos() const override;

   private:
    MixNode* node_ { nullptr };
  };

  class ValueInput2 : public NodeInput {
   public:
    ValueInput2(MixNode *node);

    CPoint2D pos() const override;

   private:
    MixNode* node_ { nullptr };
  };

  class ValueOutput : public NodeOutput {
   public:
    ValueOutput(MixNode *node);

    NodeValue* getValue() override;

    CPoint2D pos() const override;

   private:
    MixNode* node_ { nullptr };
  };

 public:
  MixNode(App *app);

  MixGUI *gui() const { return gui_; }

  double factor() const { return factor_; }
  void setFactor(double r) { factor_ = r; invalidate(); }

  NodeValue *calcValue();

  ValueInput1 *valueInput1() const { return valueInput1_; }
  ValueInput2 *valueInput2() const { return valueInput2_; }
  ValueOutput *valueOutput() const { return valueOutput_; }

  void process() override;

 private:
  MixGUI* gui_ { nullptr };

  double factor_ { 0.5 };

  ValueInput1 *valueInput1_ { nullptr };
  ValueInput2 *valueInput2_ { nullptr };
  ValueOutput *valueOutput_ { nullptr };

  ColorArrayValue* value_ { nullptr };
};

//---

class MixGUI : public QFrame {
  Q_OBJECT

 public:
  MixGUI(MixNode *node_);

  QLabel *input1Label() const { return input1Label_; }
  QLabel *input2Label() const { return input2Label_; }
  QLabel *valueLabel () const { return valueLabel_ ; }

  void updateWidgets();

  void connectSlots(bool b);

 private Q_SLOTS:
  void factorSlot(double);

 private:
  MixNode* node_ { nullptr };

  CQRealSpin* factorEdit_ { nullptr };

  QLabel* input1Label_ { nullptr };
  QLabel* input2Label_ { nullptr };
  QLabel* valueLabel_  { nullptr };
};

}

#endif
