#ifndef RemapNode_H
#define RemapNode_H

#include <AppNode.h>
#include <NodeInput.h>
#include <NodeOutput.h>

class CQRealSpin;

class QLabel;

namespace CQTextureGen {

class RemapGUI;
class RealArrayValue;

class RemapNode : public AppNode {
 public:
  class ValueInput : public NodeInput {
   public:
    ValueInput(RemapNode *node);

    CPoint2D pos() const override;

   private:
    RemapNode* node_ { nullptr };
  };

  class ValueOutput : public NodeOutput {
   public:
    ValueOutput(RemapNode *node);

    NodeValue* getValue() override;

    CPoint2D pos() const override;

   private:
    RemapNode* node_ { nullptr };
  };

 public:
  RemapNode(App *app);

  RemapGUI *gui() const { return gui_; }

  NodeValue *calcValue();

  ValueInput  *valueInput () const { return valueInput_; }
  ValueOutput *valueOutput() const { return valueOutput_; }

  double fromMin() const { return fromMin_; }
  void setFromMin(double r) { fromMin_ = r; }

  double fromMax() const { return fromMax_; }
  void setFromMax(double r) { fromMax_ = r; }

  double toMin() const { return toMin_; }
  void setToMin(double r) { toMin_ = r; }

  double toMax() const { return toMax_; }
  void setToMax(double r) { toMax_ = r; }

  void process() override;

 private:
  RemapGUI* gui_ { nullptr };

  ValueInput  *valueInput_  { nullptr };
  ValueOutput *valueOutput_ { nullptr };

  double fromMin_ { 0.0 };
  double fromMax_ { 1.0 };

  double toMin_ { 0.0 };
  double toMax_ { 1.0 };

  RealArrayValue* value_ { nullptr };
};

//---

class RemapGUI : public QFrame {
  Q_OBJECT

 public:
  RemapGUI(RemapNode *node_);

  QLabel *inputLabel () const { return inputLabel_ ; }
  QLabel *outputLabel() const { return outputLabel_; }

  void updateWidgets();

  void connectSlots(bool b);

 private Q_SLOTS:
  void fromMinSlot(double);
  void fromMaxSlot(double);

  void toMinSlot(double);
  void toMaxSlot(double);

 private:
  RemapNode* node_ { nullptr };

  QLabel* inputLabel_  { nullptr };
  QLabel* outputLabel_ { nullptr };

  CQRealSpin* fromMinEdit_ { nullptr };
  CQRealSpin* fromMaxEdit_ { nullptr };

  CQRealSpin* toMinEdit_ { nullptr };
  CQRealSpin* toMaxEdit_ { nullptr };
};

}

#endif
