#ifndef SeparateColorNode_H
#define SeparateColorNode_H

#include <AppNode.h>
#include <NodeInput.h>
#include <NodeOutput.h>

class QComboBox;
class QLabel;

namespace CQTextureGen {

class SeparateColorGUI;
class RealArrayValue;

class SeparateColorNode : public AppNode {
 public:
  class ColorInput : public NodeInput {
   public:
    ColorInput(SeparateColorNode *node);

    CPoint2D pos() const override;

   private:
    SeparateColorNode* node_ { nullptr };
  };

  class RedOutput : public NodeOutput {
   public:
    RedOutput(SeparateColorNode *node);

    NodeValue* getValue() override;

    CPoint2D pos() const override;

   private:
    SeparateColorNode* node_ { nullptr };
  };

  class GreenOutput : public NodeOutput {
   public:
    GreenOutput(SeparateColorNode *node);

    NodeValue* getValue() override;

    CPoint2D pos() const override;

   private:
    SeparateColorNode* node_ { nullptr };
  };

  class BlueOutput : public NodeOutput {
   public:
    BlueOutput(SeparateColorNode *node);

    NodeValue* getValue() override;

    CPoint2D pos() const override;

   private:
    SeparateColorNode* node_ { nullptr };
  };

  class AlphaOutput : public NodeOutput {
   public:
    AlphaOutput(SeparateColorNode *node);

    NodeValue* getValue() override;

    CPoint2D pos() const override;

   private:
    SeparateColorNode* node_ { nullptr };
  };

 public:
  SeparateColorNode(App *app);

  SeparateColorGUI *gui() const { return gui_; }

  void calcValues();

  ColorInput  *colorInput () const { return colorInput_ ; }
  RedOutput   *redOutput  () const { return redOutput_  ; }
  GreenOutput *greenOutput() const { return greenOutput_; }
  BlueOutput  *blueOutput () const { return blueOutput_ ; }
  AlphaOutput *alphaOutput() const { return alphaOutput_; }

  RealArrayValue* rvalue() const { return rvalue_; }
  RealArrayValue* gvalue() const { return gvalue_; }
  RealArrayValue* bvalue() const { return bvalue_; }
  RealArrayValue* avalue() const { return avalue_; }

  void process() override;

 private:
  SeparateColorGUI* gui_ { nullptr };

  ColorInput  *colorInput_  { nullptr };
  RedOutput   *redOutput_   { nullptr };
  GreenOutput *greenOutput_ { nullptr };
  BlueOutput  *blueOutput_  { nullptr };
  AlphaOutput *alphaOutput_ { nullptr };

  RealArrayValue* rvalue_ { nullptr };
  RealArrayValue* gvalue_ { nullptr };
  RealArrayValue* bvalue_ { nullptr };
  RealArrayValue* avalue_ { nullptr };
};

//---

class SeparateColorGUI : public QFrame {
  Q_OBJECT

 public:
  SeparateColorGUI(SeparateColorNode *node_);

  QLabel *colorLabel() const { return colorLabel_; }
  QLabel *redLabel  () const { return redLabel_  ; }
  QLabel *greenLabel() const { return greenLabel_; }
  QLabel *blueLabel () const { return blueLabel_ ; }
  QLabel *alphaLabel() const { return alphaLabel_; }

  void updateWidgets();

  void connectSlots(bool b);

 private:
  SeparateColorNode* node_ { nullptr };

  QLabel *colorLabel_ { nullptr };
  QLabel *redLabel_   { nullptr };
  QLabel *greenLabel_ { nullptr };
  QLabel *blueLabel_  { nullptr };
  QLabel *alphaLabel_ { nullptr };
};

}

#endif
