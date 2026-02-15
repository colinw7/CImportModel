#include <NoiseTextureNode.h>
#include <App.h>
#include <NodeArea.h>
#include <NodeValue.h>
#include <UI.h>
#include <Util.h>

#include <CQColorEdit.h>
#include <CQRealSpin.h>

namespace CQTextureGen {

NoiseTextureNode::
NoiseTextureNode(App *app) :
 AppNode(app, "Noise Texture")
{
  gui_ = new NoiseTextureGUI(this);

  setChild(gui_);
  setWindowTitle(QString::fromStdString(name()));

  fitChild();

  valueOutput_ = addOutputT(new ValueOutput(this));
}

void
NoiseTextureNode::
process()
{
}

NodeValue *
NoiseTextureNode::
calcValue()
{
  std::cerr << "NoiseTextureNode::calcValue\n";

  if (! value_)
    value_ = new ColorArrayValue;

  int is = app_->textureSize();

  value_->resize(is*is);

  int ii = 0;

  for (int iy = 0; iy < is; ++iy) {
    for (int ix = 0; ix < is; ++ix) {
      CPoint3D p(double(ix)/double(is - 1), double(iy)/double(is - 1), 0.0);

      auto t = (1 + noise_.noise(p*scale_))/2;

      auto c =  t*color1_ + (1 - t)*color2_;

      value_->setColor(ii, c);

      ++ii;
    }
  }

  return value_;
}

//---

NoiseTextureNode::ValueOutput::
ValueOutput(NoiseTextureNode *node) :
 NodeOutput("Value"), node_(node)
{
}

NodeValue *
NoiseTextureNode::ValueOutput::
getValue()
{
  return node_->calcValue();
}

CPoint2D
NoiseTextureNode::ValueOutput::
pos() const
{
  auto *gui = node_->gui();

  return outputPos(gui, gui->valueLabel());
}

//---

NoiseTextureGUI::
NoiseTextureGUI(NoiseTextureNode *node) :
 node_(node)
{
  auto *layout = new QVBoxLayout(this);

  UI ui(this, layout);

  scaleEdit_  = ui.addLabelEdit("Scale" , new CQRealSpin);
  color1Edit_ = ui.addLabelEdit("Color1", new CQColorEdit);
  color2Edit_ = ui.addLabelEdit("Color2", new CQColorEdit);

  valueLabel_ = ui.addOutputLabel("Value");

  ui.addStretch();

  connectSlots(true);

  updateWidgets();
}

void
NoiseTextureGUI::
updateWidgets()
{
  connectSlots(false);

  scaleEdit_->setValue(node_->scale());
  color1Edit_->setColor(RGBAToQColor(node_->color1()));
  color2Edit_->setColor(RGBAToQColor(node_->color2()));

  connectSlots(true);
}

void
NoiseTextureGUI::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, scaleEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(scaleSlot(double)));
  CQUtil::connectDisconnect(b, color1Edit_, SIGNAL(colorChanged(const QColor &)),
                            this, SLOT(color1Slot(const QColor &)));
  CQUtil::connectDisconnect(b, color2Edit_, SIGNAL(colorChanged(const QColor &)),
                            this, SLOT(color2Slot(const QColor &)));
}

void
NoiseTextureGUI::
scaleSlot(double r)
{
  node_->setScale(r);
}

void
NoiseTextureGUI::
color1Slot(const QColor &c)
{
  node_->setColor1(QColorToRGBA(c));
}

void
NoiseTextureGUI::
color2Slot(const QColor &c)
{
  node_->setColor2(QColorToRGBA(c));
}

}
