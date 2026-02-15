#include <MaterialNode.h>
#include <App.h>
#include <NodeArea.h>
#include <NodeValue.h>
#include <MaterialView.h>
#include <UI.h>
#include <Util.h>

#include <CQColorEdit.h>
#include <CQRealSpin.h>

namespace CQTextureGen {

MaterialNode::
MaterialNode(App *app) :
 AppNode(app, "Material")
{
  gui_ = new MaterialGUI(this);

  setChild(gui_);
  setWindowTitle(QString::fromStdString(name()));

  fitChild();

  diffuseInput_  = addInputT(new MaterialNode::DiffuseInput (this));
  normalInput_   = addInputT(new MaterialNode::NormalInput  (this));
  specularInput_ = addInputT(new MaterialNode::SpecularInput(this));
  emissionInput_ = addInputT(new MaterialNode::EmissionInput(this));

  metallicInput_  = addInputT(new MaterialNode::MetallicInput (this));
  roughnessInput_ = addInputT(new MaterialNode::RoughnessInput(this));
  occlusionInput_ = addInputT(new MaterialNode::OcclusionInput(this));
  ormInput_       = addInputT(new MaterialNode::ORMInput      (this));
}

void
MaterialNode::
process()
{
  std::cerr << "MaterialNode::process\n";

  int is = app_->textureSize();

  auto updateImage = [&](QImage &image, NodeValue *value,
                         const CRGBA &color=CRGBA::transparent()) {
    if (value) {
      int ii = 0;

      for (int iy = 0; iy < is; ++iy) {
        for (int ix = 0; ix < is; ++ix) {
          const auto &c = value->colorValue(ii);

          image.setPixelColor(ix, iy, RGBAToQColor(c));

          ++ii;
        }
      }

      return true;
    }
    else {
      image.fill(RGBAToQColor(color));

      return false;
    }
  };

  app_->setDiffuseImageSet(
    updateImage(app_->diffuseImage(), diffuseInput_->getValue(), diffuse()));
  app_->setDiffuseColor(diffuse());

  //---

  app_->setNormalImageSet  (updateImage(app_->normalImage  (), normalInput_  ->getValue()));
  app_->setSpecularImageSet(updateImage(app_->specularImage(), specularInput_->getValue()));
  app_->setEmissionImageSet(updateImage(app_->emissionImage(), emissionInput_->getValue()));

  app_->setMetallicImageSet (updateImage(app_->metallicImage (), metallicInput_ ->getValue()));
//app_->setRoughnessImageSet(updateImage(app_->roughnessImage(), roughnessInput_->getValue()));
//app_->setOcclusionImageSet(updateImage(app_->occlusionImage(), occlusionInput_->getValue()));
  app_->setORMImageSet      (updateImage(app_->ORMImage      (), ormInput_      ->getValue()));
}

//---

MaterialNode::DiffuseInput::
DiffuseInput(MaterialNode *node) :
 NodeInput("Diffuse"), node_(node)
{
}

CPoint2D
MaterialNode::DiffuseInput::
pos() const
{
  auto *gui = node_->gui();

  return inputPos(gui, gui->diffuseEdit());
}

//---

MaterialNode::NormalInput::
NormalInput(MaterialNode *node) :
 NodeInput("Normal"), node_(node)
{
}

CPoint2D
MaterialNode::NormalInput::
pos() const
{
  auto *gui = node_->gui();

  return inputPos(gui, gui->normalLabel());
}

//---

MaterialNode::SpecularInput::
SpecularInput(MaterialNode *node) :
 NodeInput("Specular"), node_(node)
{
}

CPoint2D
MaterialNode::SpecularInput::
pos() const
{
  auto *gui = node_->gui();

  return inputPos(gui, gui->specularLabel());
}

//---

MaterialNode::EmissionInput::
EmissionInput(MaterialNode *node) :
 NodeInput("Emission"), node_(node)
{
}

CPoint2D
MaterialNode::EmissionInput::
pos() const
{
  auto *gui = node_->gui();

  return inputPos(gui, gui->emissionLabel());
}

//---

MaterialNode::MetallicInput::
MetallicInput(MaterialNode *node) :
 NodeInput("Metallic"), node_(node)
{
}

CPoint2D
MaterialNode::MetallicInput::
pos() const
{
  auto *gui = node_->gui();

  return inputPos(gui, gui->metallicEdit());
}

//---

MaterialNode::RoughnessInput::
RoughnessInput(MaterialNode *node) :
 NodeInput("Roughness"), node_(node)
{
}

CPoint2D
MaterialNode::RoughnessInput::
pos() const
{
  auto *gui = node_->gui();

  return inputPos(gui, gui->roughnessEdit());
}

//---

MaterialNode::OcclusionInput::
OcclusionInput(MaterialNode *node) :
 NodeInput("Occlusion"), node_(node)
{
}

CPoint2D
MaterialNode::OcclusionInput::
pos() const
{
  auto *gui = node_->gui();

  return inputPos(gui, gui->occlusionEdit());
}

//---

MaterialNode::ORMInput::
ORMInput(MaterialNode *node) :
 NodeInput("ORM"), node_(node)
{
}

CPoint2D
MaterialNode::ORMInput::
pos() const
{
  auto *gui = node_->gui();

  return inputPos(gui, gui->ORMLabel());
}

//---

MaterialGUI::
MaterialGUI(MaterialNode *node) :
 node_(node)
{
  auto *layout = new QVBoxLayout(this);

  UI ui(this, layout);

  diffuseEdit_   = ui.addLabelEdit ("Diffuse", new CQColorEdit);
  normalLabel_   = ui.addInputLabel("Normal");
  specularLabel_ = ui.addInputLabel("Specular");
  emissionLabel_ = ui.addInputLabel("Emission");

  metallicEdit_  = ui.addLabelEdit ("Metallic" , new CQRealSpin);
  roughnessEdit_ = ui.addLabelEdit ("Roughness", new CQRealSpin);
  occlusionEdit_ = ui.addLabelEdit ("Occlusion", new CQRealSpin);
  ORMLabel_      = ui.addInputLabel("ORM");

  ui.addStretch();

  connectSlots(true);

  updateWidgets();
}

void
MaterialGUI::
updateWidgets()
{
  connectSlots(false);

  auto *material = node_->app()->materialView()->material();

  diffuseEdit_->setColor(RGBAToQColor(node_->diffuse()));

  metallicEdit_ ->setValue(material->getMetallic        ());
  roughnessEdit_->setValue(material->getRoughness       ());
  occlusionEdit_->setValue(material->getAmbientOcclusion());

  connectSlots(true);
}

void
MaterialGUI::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, diffuseEdit_, SIGNAL(colorChanged(const QColor &)),
                            this, SLOT(diffuseSlot(const QColor &)));

  CQUtil::connectDisconnect(b, metallicEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(metallicSlot(double)));
  CQUtil::connectDisconnect(b, roughnessEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(roughnessSlot(double)));
  CQUtil::connectDisconnect(b, occlusionEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(occlusionSlot(double)));
}

void
MaterialGUI::
diffuseSlot(const QColor &c)
{
  node_->setDiffuse(QColorToRGBA(c));
}

void
MaterialGUI::
metallicSlot(double r)
{
  auto *material = node_->app()->materialView()->material();

  material->setMetallic(r);

  node_->invalidate();
}

void
MaterialGUI::
roughnessSlot(double r)
{
  auto *material = node_->app()->materialView()->material();

  material->setRoughness(r);

  node_->invalidate();
}

void
MaterialGUI::
occlusionSlot(double r)
{
  auto *material = node_->app()->materialView()->material();

  material->setAmbientOcclusion(r);

  node_->invalidate();
}

}
