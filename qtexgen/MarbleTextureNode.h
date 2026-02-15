#ifndef MarbleTextureNode_H
#define MarbleTextureNode_H

#include <AppNode.h>
#include <NodeOutput.h>
#include <CSolidNoise.h>
#include <CRGBA.h>

class CQRealSpin;
class CQIntegerSpin;
class CQColorEdit;

namespace CQTextureGen {

class MarbleTextureGUI;
class ColorArrayValue;

class MarbleTextureNode : public AppNode {
 public:
  class ValueOutput : public NodeOutput {
   public:
    ValueOutput(MarbleTextureNode *node);

    NodeValue* getValue() override;

    CPoint2D pos() const override;

   private:
    MarbleTextureNode* node_ { nullptr };
  };

 public:
  MarbleTextureNode(App *app);

  MarbleTextureGUI *gui() const { return gui_; }

  NodeValue *calcValue();

  ValueOutput *valueOutput() const { return valueOutput_; }

  double freq() const { return freq_; }
  void setFreq(double r) { freq_ = r; invalidate(); }

  double scale() const { return scale_; }
  void setScale(double r) { scale_ = r; invalidate(); }

  int octaves() const { return octaves_; }
  void setOctaves(int i) { octaves_ = i; invalidate(); }

  const CRGBA &color1() const { return color1_; }
  void setColor1(const CRGBA &v) { color1_ = v; invalidate(); }

  const CRGBA &color2() const { return color2_; }
  void setColor2(const CRGBA &v) { color2_ = v; invalidate(); }

  const CRGBA &color3() const { return color3_; }
  void setColor3(const CRGBA &v) { color3_ = v; invalidate(); }

  void process() override;

 private:
  MarbleTextureGUI* gui_ { nullptr };

  ValueOutput *valueOutput_ { nullptr };

  double freq_    { 0.2 };
  double scale_   { 5.0 };
  int    octaves_ { 8 };

  CRGBA color1_ { 0.8 , 0.8 , 0.8  };
  CRGBA color2_ { 0.4 , 0.2 , 0.1  };
  CRGBA color3_ { 0.06, 0.04, 0.02 };

  CSolidNoise3D noise_;

  ColorArrayValue* value_ { nullptr };
};

//---

class MarbleTextureGUI : public QFrame {
  Q_OBJECT

 public:
  MarbleTextureGUI(MarbleTextureNode *node_);

  QLabel *valueLabel() const { return valueLabel_; }

  void updateWidgets();

  void connectSlots(bool b);

 private Q_SLOTS:
  void freqSlot(double);
  void scaleSlot(double);
  void octavesSlot(int);

  void color1Slot(const QColor &);
  void color2Slot(const QColor &);
  void color3Slot(const QColor &);

 private:
  MarbleTextureNode* node_ { nullptr };

  CQRealSpin*    freqEdit_    { nullptr };
  CQRealSpin*    scaleEdit_   { nullptr };
  CQIntegerSpin* octavesEdit_ { nullptr };

  CQColorEdit* color1Edit_ { nullptr };
  CQColorEdit* color2Edit_ { nullptr };
  CQColorEdit* color3Edit_ { nullptr };

  QLabel* valueLabel_ { nullptr };
};

}

#endif
