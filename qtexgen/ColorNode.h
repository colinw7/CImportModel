#ifndef ColorNode_H
#define ColorNode_H

#include <AppNode.h>
#include <NodeInput.h>
#include <NodeOutput.h>

class CQRealSpin;

class QLabel;

namespace CQTextureGen {

class ColorGUI;
class ColorArrayValue;

class ColorNode : public AppNode {
 public:
  class RedInput : public NodeInput {
   public:
    RedInput(ColorNode *node);

    CPoint2D pos() const override;

   private:
    ColorNode* node_ { nullptr };
  };

  class GreenInput : public NodeInput {
   public:
    GreenInput(ColorNode *node);

    CPoint2D pos() const override;

   private:
    ColorNode* node_ { nullptr };
  };

  class BlueInput : public NodeInput {
   public:
    BlueInput(ColorNode *node);

    CPoint2D pos() const override;

   private:
    ColorNode* node_ { nullptr };
  };

  class AlphaInput : public NodeInput {
   public:
    AlphaInput(ColorNode *node);

    CPoint2D pos() const override;

   private:
    ColorNode* node_ { nullptr };
  };

  class ColorOutput : public NodeOutput {
   public:
    ColorOutput(ColorNode *node);

    NodeValue* getValue() override;

    CPoint2D pos() const override;

   private:
    ColorNode* node_ { nullptr };
  };

 public:
  ColorNode(App *app);

  ColorGUI *gui() const { return gui_; }

  NodeValue *calcValue();

  RedInput    *redInput   () const { return redInput_   ; }
  GreenInput  *greenInput () const { return greenInput_ ; }
  BlueInput   *blueInput  () const { return blueInput_  ; }
  AlphaInput  *alphaInput () const { return alphaInput_ ; }
  ColorOutput *colorOutput() const { return colorOutput_; }

  void process() override;

 private:
  ColorGUI* gui_ { nullptr };

  RedInput    *redInput_    { nullptr };
  GreenInput  *greenInput_  { nullptr };
  BlueInput   *blueInput_   { nullptr };
  AlphaInput  *alphaInput_  { nullptr };
  ColorOutput *colorOutput_ { nullptr };

  ColorArrayValue* value_ { nullptr };
};

//---

class ColorGUI : public QFrame {
  Q_OBJECT

 public:
  ColorGUI(ColorNode *node_);

  CQRealSpin *redEdit   () const { return redEdit_   ; }
  CQRealSpin *greenEdit () const { return greenEdit_ ; }
  CQRealSpin *blueEdit  () const { return blueEdit_  ; }
  CQRealSpin *alphaEdit () const { return alphaEdit_ ; }
  QLabel     *colorLabel() const { return colorLabel_; }

  void updateWidgets();

  void connectSlots(bool b);

 private:
  ColorNode* node_ { nullptr };

  CQRealSpin *redEdit_    { nullptr };
  CQRealSpin *greenEdit_  { nullptr };
  CQRealSpin *blueEdit_   { nullptr };
  CQRealSpin *alphaEdit_  { nullptr };
  QLabel     *colorLabel_ { nullptr };
};

}

#endif
