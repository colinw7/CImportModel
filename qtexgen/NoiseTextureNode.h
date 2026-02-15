#ifndef NoiseTextureNode_H
#define NoiseTextureNode_H

#include <AppNode.h>
#include <NodeOutput.h>
#include <CSolidNoise.h>
#include <CRGBA.h>

class CQRealSpin;
class CQColorEdit;
class QLabel;

namespace CQTextureGen {

class NoiseTextureGUI;
class ColorArrayValue;

class NoiseTextureNode : public AppNode {
 public:
  class ValueOutput : public NodeOutput {
   public:
    ValueOutput(NoiseTextureNode *node);

    NodeValue* getValue() override;

    CPoint2D pos() const override;

   private:
    NoiseTextureNode* node_ { nullptr };
  };

 public:
  NoiseTextureNode(App *app);

  NoiseTextureGUI *gui() const { return gui_; }

  NodeValue *calcValue();

  ValueOutput *valueOutput() const { return valueOutput_; }

  double scale() const { return scale_; }
  void setScale(double r) { scale_ = r; invalidate(); }

  const CRGBA &color1() const { return color1_; }
  void setColor1(const CRGBA &v) { color1_ = v; invalidate(); }

  const CRGBA &color2() const { return color2_; }
  void setColor2(const CRGBA &v) { color2_ = v; invalidate(); }

  void process() override;

 private:
  NoiseTextureGUI* gui_ { nullptr };

  ValueOutput *valueOutput_ { nullptr };

  double scale_  { 1.0 };

  CRGBA color1_ { 0.0, 0.0, 1.0 };
  CRGBA color2_ { 1.0, 1.0, 1.0 };

  CSolidNoise3D noise_;

  ColorArrayValue* value_ { nullptr };
};

//---

class NoiseTextureGUI : public QFrame {
  Q_OBJECT

 public:
  NoiseTextureGUI(NoiseTextureNode *node);

  QLabel *valueLabel() const { return valueLabel_; }

  void updateWidgets();

  void connectSlots(bool b);

 private Q_SLOTS:
  void scaleSlot(double);
  void color1Slot(const QColor &);
  void color2Slot(const QColor &);

 private:
  NoiseTextureNode* node_ { nullptr };

  CQRealSpin* scaleEdit_ { nullptr };

  CQColorEdit* color1Edit_ { nullptr };
  CQColorEdit* color2Edit_ { nullptr };

  QLabel* valueLabel_ { nullptr };
};

}

#endif
