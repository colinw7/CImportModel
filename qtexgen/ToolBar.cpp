#include <ToolBar.h>
#include <App.h>
#include <MaterialView.h>
#include <Overview.h>

#include <NoiseTextureNode.h>
#include <MarbleTextureNode.h>
#include <ImageTextureNode.h>
#include <MixNode.h>
#include <MathNode.h>
#include <BinaryMathNode.h>
#include <RemapNode.h>
#include <TextureCoordNode.h>
#include <ColorNode.h>
#include <SeparateColorNode.h>
#include <DebugNode.h>

#include <UI.h>

namespace CQTextureGen {

ToolBar::
ToolBar(App *app) :
 app_(app)
{
  setObjectName("toolbar");

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  auto *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(4);

  //---

  UI ui(this, layout);

  //---

  ui.startHFrame();

  ui.startMenuBar();

  ui.startMenu("Add");

  ui.addAction("Noise Texture" , SLOT(noiseTextureSlot()));
  ui.addAction("Marble Texture", SLOT(marbleTextureSlot()));
  ui.addAction("Image Texture" , SLOT(imageTextureSlot()));
  ui.addAction("Mix"           , SLOT(mixSlot()));
  ui.addAction("Math"          , SLOT(mathSlot()));
  ui.addAction("Binary Math"   , SLOT(binaryMathSlot()));
  ui.addAction("Remap"         , SLOT(remapSlot()));
  ui.addAction("Texture Coord" , SLOT(textureCoordSlot()));
  ui.addAction("Color"         , SLOT(colorSlot()));
  ui.addAction("Debug"         , SLOT(debugSlot()));

  ui.endMenu();

  ui.startMenu("Shape");

  ui.addAction("Cube"  , SLOT(cubeSlot()));
  ui.addAction("Sphere", SLOT(sphereSlot()));
  ui.addAction("Plane" , SLOT(planeSlot()));

  ui.endMenu();

  ui.startMenu("Edit");

  ui.addAction("Light" , SLOT(lightSlot()));
  ui.addAction("Camera", SLOT(cameraSlot()));

  ui.endMenu();

  ui.endMenuBar();

  ui.endFrame();

  //---

  ui.addStretch();

  //---

  auto *invalidateButton = new QPushButton("Invalidate");

  connect(invalidateButton, SIGNAL(clicked()), this, SLOT(invalidateSlot()));

  ui.addWidget(invalidateButton);
}

void
ToolBar::
noiseTextureSlot()
{
  app_->addNodeT<NoiseTextureNode>();
}

void
ToolBar::
marbleTextureSlot()
{
  app_->addNodeT<MarbleTextureNode>();
}

void
ToolBar::
imageTextureSlot()
{
  app_->addNodeT<ImageTextureNode>();
}

void
ToolBar::
mixSlot()
{
  app_->addNodeT<MixNode>();
}

void
ToolBar::
mathSlot()
{
  app_->addNodeT<MathNode>();
}

void
ToolBar::
binaryMathSlot()
{
  app_->addNodeT<BinaryMathNode>();
}

void
ToolBar::
remapSlot()
{
  app_->addNodeT<RemapNode>();
}

void
ToolBar::
textureCoordSlot()
{
  app_->addNodeT<TextureCoordNode>();
}

void
ToolBar::
colorSlot()
{
  app_->addNodeT<ColorNode>();
}

void
ToolBar::
separateColorSlot()
{
  app_->addNodeT<SeparateColorNode>();
}

void
ToolBar::
debugSlot()
{
  app_->addNodeT<DebugNode>();
}

void
ToolBar::
cubeSlot()
{
  app_->materialView()->setShapeType(MaterialView::ShapeType::CUBE);
}

void
ToolBar::
sphereSlot()
{
  app_->materialView()->setShapeType(MaterialView::ShapeType::SPHERE);
}

void
ToolBar::
planeSlot()
{
  app_->materialView()->setShapeType(MaterialView::ShapeType::PLANE);
}

void
ToolBar::
lightSlot()
{
  app_->overview()->setEditType(Overview::EditType::LIGHT);
}

void
ToolBar::
cameraSlot()
{
  app_->overview()->setEditType(Overview::EditType::CAMERA);
}

void
ToolBar::
invalidateSlot()
{
  app_->processNodes();
}

}
