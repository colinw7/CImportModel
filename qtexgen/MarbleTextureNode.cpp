#include <MarbleTextureNode.h>
#include <App.h>
#include <NodeArea.h>
#include <NodeValue.h>
#include <UI.h>
#include <Util.h>

#include <CQRealSpin.h>
#include <CQIntegerSpin.h>
#include <CQColorEdit.h>

namespace CQTextureGen {

MarbleTextureNode::
MarbleTextureNode(App *app) :
 AppNode(app, "Marble Texture")
{
  gui_ = new MarbleTextureGUI(this);

  setChild(gui_);
  setWindowTitle(QString::fromStdString(name()));

  fitChild();

  valueOutput_ = addOutputT(new ValueOutput(this));
}

NodeValue *
MarbleTextureNode::
calcValue()
{
  std::cerr << "MarbleTextureNode::calcValue\n";

  if (! value_)
    value_ = new ColorArrayValue;

  int is = app_->textureSize();

  value_->resize(is*is);

  int ii = 0;

  for (int iy = 0; iy < is; ++iy) {
    for (int ix = 0; ix < is; ++ix) {
      CPoint3D p(double(ix)/double(is - 1), double(iy)/double(is - 1), 0.0);

      auto t1 = scale_*noise_.turbulence(freq_*p, octaves_);

      auto t = 2*std::fabs(std::sin(freq_*p.getX() + t1));

      CRGBA c;

      if (t < 1)
        c = t*color2_ + (1 - t)*color3_;
      else {
        t -= 1;

        c = t*color1_ + (1 - t)*color2_;
      }

      value_->setColor(ii, c);

      ++ii;
    }
  }

  return value_;
}

void
MarbleTextureNode::
process()
{
}

//---

MarbleTextureNode::ValueOutput::
ValueOutput(MarbleTextureNode *node) :
 NodeOutput("Value"), node_(node)
{
}

NodeValue *
MarbleTextureNode::ValueOutput::
getValue()
{
  return node_->calcValue();
}

CPoint2D
MarbleTextureNode::ValueOutput::
pos() const
{
  auto *gui = node_->gui();

  return outputPos(gui, gui->valueLabel());
}

//---

MarbleTextureGUI::
MarbleTextureGUI(MarbleTextureNode *node) :
 node_(node)
{
  auto *layout = new QVBoxLayout(this);

  UI ui(this, layout);

  freqEdit_    = ui.addLabelEdit("Freq"   , new CQRealSpin);
  scaleEdit_   = ui.addLabelEdit("Scale"  , new CQRealSpin);
  octavesEdit_ = ui.addLabelEdit("Octaves", new CQIntegerSpin);

  color1Edit_ = ui.addLabelEdit("Color1", new CQColorEdit);
  color2Edit_ = ui.addLabelEdit("Color2", new CQColorEdit);
  color3Edit_ = ui.addLabelEdit("Color3", new CQColorEdit);

  valueLabel_ = ui.addOutputLabel("Value");

  ui.addStretch();

  connectSlots(true);

  updateWidgets();
}

void
MarbleTextureGUI::
updateWidgets()
{
  connectSlots(false);

  freqEdit_   ->setValue(node_->freq());
  scaleEdit_  ->setValue(node_->scale());
  octavesEdit_->setValue(node_->octaves());

  color1Edit_->setColor(RGBAToQColor(node_->color1()));
  color2Edit_->setColor(RGBAToQColor(node_->color2()));
  color3Edit_->setColor(RGBAToQColor(node_->color3()));

  connectSlots(true);
}

void
MarbleTextureGUI::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, freqEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(freqSlot(double)));
  CQUtil::connectDisconnect(b, scaleEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(scaleSlot(double)));
  CQUtil::connectDisconnect(b, octavesEdit_, SIGNAL(valueChanged(int)),
                            this, SLOT(octavesSlot(int)));

  CQUtil::connectDisconnect(b, color1Edit_, SIGNAL(colorChanged(const QColor &)),
                            this, SLOT(color1Slot(const QColor &)));
  CQUtil::connectDisconnect(b, color2Edit_, SIGNAL(colorChanged(const QColor &)),
                            this, SLOT(color2Slot(const QColor &)));
  CQUtil::connectDisconnect(b, color3Edit_, SIGNAL(colorChanged(const QColor &)),
                            this, SLOT(color3Slot(const QColor &)));
}

void
MarbleTextureGUI::
freqSlot(double r)
{
  node_->setFreq(r);
}

void
MarbleTextureGUI::
scaleSlot(double r)
{
  node_->setScale(r);
}

void
MarbleTextureGUI::
octavesSlot(int i)
{
  node_->setOctaves(i);
}

void
MarbleTextureGUI::
color1Slot(const QColor &c)
{
  node_->setColor1(QColorToRGBA(c));
}

void
MarbleTextureGUI::
color2Slot(const QColor &c)
{
  node_->setColor2(QColorToRGBA(c));
}

void
MarbleTextureGUI::
color3Slot(const QColor &c)
{
  node_->setColor3(QColorToRGBA(c));
}

}
