#ifndef BinaryMathNode_H
#define BinaryMathNode_H

#include <AppNode.h>
#include <NodeInput.h>
#include <NodeOutput.h>

class CQRealSpin;

class QComboBox;
class QLabel;

namespace CQTextureGen {

class BinaryMathGUI;
class RealArrayValue;

class BinaryMathNode : public AppNode {
 public:
  enum MathType {
    NONE,
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE
  };

 public:
  class Value1Input : public NodeInput {
   public:
    Value1Input(BinaryMathNode *node);

    CPoint2D pos() const override;

   private:
    BinaryMathNode* node_ { nullptr };
  };

  class Value2Input : public NodeInput {
   public:
    Value2Input(BinaryMathNode *node);

    CPoint2D pos() const override;

   private:
    BinaryMathNode* node_ { nullptr };
  };

  class ValueOutput : public NodeOutput {
   public:
    ValueOutput(BinaryMathNode *node);

    NodeValue* getValue() override;

    CPoint2D pos() const override;

   private:
    BinaryMathNode* node_ { nullptr };
  };

 public:
  BinaryMathNode(App *app);

  BinaryMathGUI *gui() const { return gui_; }

  const MathType &mathType() const { return mathType_; }
  void setMathType(const MathType &v) { mathType_ = v; invalidate(); }

  void setMathTypeFromString(const std::string &str);

  NodeValue *calcValue();

  Value1Input *value1Input() const { return value1Input_; }
  Value2Input *value2Input() const { return value2Input_; }
  ValueOutput *valueOutput() const { return valueOutput_; }

  void process() override;

 private:
  BinaryMathGUI* gui_ { nullptr };

  MathType mathType_ { MathType::NONE };

  Value1Input *value1Input_ { nullptr };
  Value2Input *value2Input_ { nullptr };
  ValueOutput *valueOutput_ { nullptr };

  RealArrayValue* value_ { nullptr };
};

//---

class BinaryMathGUI : public QFrame {
  Q_OBJECT

 public:
  BinaryMathGUI(BinaryMathNode *node_);

  CQRealSpin *input1Edit() const { return input1Edit_; }
  CQRealSpin *input2Edit() const { return input2Edit_; }
  QLabel     *valueLabel() const { return valueLabel_; }

  void updateWidgets();

  void connectSlots(bool b);

 private Q_SLOTS:
  void typeSlot(int);

 private:
  BinaryMathNode* node_ { nullptr };

  QComboBox* typeCombo_ { nullptr };

  CQRealSpin* input1Edit_ { nullptr };
  CQRealSpin* input2Edit_ { nullptr };
  QLabel*     valueLabel_ { nullptr };
};

}

#endif
