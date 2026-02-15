#ifndef ImageTextureNode_H
#define ImageTextureNode_H

#include <AppNode.h>
#include <NodeOutput.h>

class CQFilename;
class QLabel;

namespace CQTextureGen {

class ImageTextureGUI;
class ColorArrayValue;

class ImageTextureNode : public AppNode {
 public:
  class ValueOutput : public NodeOutput {
   public:
    ValueOutput(ImageTextureNode *node);

    NodeValue* getValue() override;

    CPoint2D pos() const override;

   private:
    ImageTextureNode* node_ { nullptr };
  };

 public:
  ImageTextureNode(App *app);

  ImageTextureGUI *gui() const { return gui_; }

  const std::string &filename() const { return filename_; }
  void setFilename(const std::string &s);

  NodeValue *calcValue();

  ValueOutput *valueOutput() const { return valueOutput_; }

  void process() override;

 private:
  ImageTextureGUI* gui_ { nullptr };

  std::string filename_;
  QImage      image_;
  QImage      image1_;

  ValueOutput *valueOutput_ { nullptr };

  ColorArrayValue* value_ { nullptr };
};

//---

class ImageTextureGUI : public QFrame {
  Q_OBJECT

 public:
  ImageTextureGUI(ImageTextureNode *node_);

  QLabel *valueLabel() const { return valueLabel_; }

  void updateWidgets();

  void connectSlots(bool b);

 private Q_SLOTS:
  void nameSlot(const QString &);

 private:
  ImageTextureNode* node_ { nullptr };

  CQFilename* nameEdit_ { nullptr };

  QLabel* valueLabel_ { nullptr };
};

}

#endif
