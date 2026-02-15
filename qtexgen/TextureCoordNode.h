#ifndef TextureCoordNode_H
#define TextureCoordNode_H

#include <AppNode.h>
#include <NodeOutput.h>
#include <CRGBA.h>

class CQColorEdit;
class QLabel;

namespace CQTextureGen {

class TextureCoordGUI;
class RealArrayValue;

class TextureCoordNode : public AppNode {
 public:
  class CoordXOutput : public NodeOutput {
   public:
    CoordXOutput(TextureCoordNode *node);

    NodeValue* getValue() override;

    CPoint2D pos() const override;

   private:
    TextureCoordNode* node_  { nullptr };
    RealArrayValue*   value_ { nullptr };
  };

  class CoordYOutput : public NodeOutput {
   public:
    CoordYOutput(TextureCoordNode *node);

    NodeValue* getValue() override;

    CPoint2D pos() const override;

   private:
    TextureCoordNode* node_  { nullptr };
    RealArrayValue*   value_ { nullptr };
  };

 public:
  TextureCoordNode(App *app);

  TextureCoordGUI *gui() const { return gui_; }

  CoordXOutput *coordXOutput() const { return coordXOutput_; }
  CoordYOutput *coordYOutput() const { return coordYOutput_; }

  void process() override;

 private:
  TextureCoordGUI* gui_ { nullptr };

  CoordXOutput* coordXOutput_ { nullptr };
  CoordYOutput* coordYOutput_ { nullptr };
};

//---

class TextureCoordGUI : public QFrame {
  Q_OBJECT

 public:
  TextureCoordGUI(TextureCoordNode *material);

  QLabel *coordXLabel() const { return coordXLabel_; }
  QLabel *coordYLabel() const { return coordYLabel_; }

  void updateWidgets();

  void connectSlots(bool b);

 private:
  TextureCoordNode* node_ { nullptr };

  QLabel* coordXLabel_ { nullptr };
  QLabel* coordYLabel_ { nullptr };
};

}

#endif
