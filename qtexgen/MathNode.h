#ifndef MathNode_H
#define MathNode_H

#include <AppNode.h>
#include <NodeInput.h>
#include <NodeOutput.h>

class QComboBox;
class QLabel;

namespace CQTextureGen {

class MathGUI;
class RealArrayValue;

class MathNode : public AppNode {
 public:
  enum MathType {
    NONE,
    COS,
    SIN,
    TAN
  };

 public:
  class ValueInput : public NodeInput {
   public:
    ValueInput(MathNode *node);

    CPoint2D pos() const override;

   private:
    MathNode* node_ { nullptr };
  };

  class ValueOutput : public NodeOutput {
   public:
    ValueOutput(MathNode *node);

    NodeValue* getValue() override;

    CPoint2D pos() const override;

   private:
    MathNode* node_ { nullptr };
  };

 public:
  MathNode(App *app);

  MathGUI *gui() const { return gui_; }

  const MathType &mathType() const { return mathType_; }
  void setMathType(const MathType &v) { mathType_ = v; invalidate(); }

  void setMathTypeFromString(const std::string &str);

  NodeValue *calcValue();

  ValueInput  *valueInput () const { return valueInput_; }
  ValueOutput *valueOutput() const { return valueOutput_; }

  void process() override;

 private:
  MathGUI* gui_ { nullptr };

  MathType mathType_ { MathType::NONE };

  ValueInput  *valueInput_  { nullptr };
  ValueOutput *valueOutput_ { nullptr };

  RealArrayValue* value_ { nullptr };
};

//---

class MathGUI : public QFrame {
  Q_OBJECT

 public:
  MathGUI(MathNode *node_);

  QLabel *inputLabel() const { return inputLabel_; }
  QLabel *valueLabel() const { return valueLabel_; }

  void updateWidgets();

  void connectSlots(bool b);

 private Q_SLOTS:
  void typeSlot(int);

 private:
  MathNode* node_ { nullptr };

  QComboBox* typeCombo_ { nullptr };

  QLabel* inputLabel_ { nullptr };
  QLabel* valueLabel_ { nullptr };
};

}

#endif
