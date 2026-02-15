#include <Control.h>
#include <Canvas.h>
#include <App.h>
#include <UI.h>
#include <ValueMap.h>

#include <CQRealSpin.h>

#include <QComboBox>

namespace {

using Canvas = CQShaderGen::Canvas;

class Type2DInd : public ValueMap<Canvas::Type2D, int> {
 public:
  Type2DInd() {
    add("None"  , Canvas::Type2D::None  , 0);
    add("Circle", Canvas::Type2D::Circle, 1);
    add("Heart" , Canvas::Type2D::Heart , 2);
    add("Cross" , Canvas::Type2D::Cross , 3);
  }

  Canvas::Type2D indToType(int ind) {
    return lookup(ind);
  }

  int typeToInd(const Canvas::Type2D &type) {
    return lookup(type);
  }
};

class Type3DInd : public ValueMap<Canvas::Type3D, int> {
 public:
  Type3DInd() {
    add("None"             , Canvas::Type3D::None             ,  0);
    add("Sphere"           , Canvas::Type3D::Sphere           ,  1);
    add("Box"              , Canvas::Type3D::Box              ,  2);
    add("RoundBox"         , Canvas::Type3D::RoundBox         ,  3);
    add("BoxFrame"         , Canvas::Type3D::BoxFrame         ,  4);
    add("Torus"            , Canvas::Type3D::Torus            ,  5);
    add("CappedTorus"      , Canvas::Type3D::CappedTorus      ,  6);
    add("Link"             , Canvas::Type3D::Link             ,  7);
    add("Cylinder"         , Canvas::Type3D::Cylinder         ,  8);
    add("Cone"             , Canvas::Type3D::Cone             ,  9);
    add("Plane"            , Canvas::Type3D::Plane            , 10);
    add("HexPrism"         , Canvas::Type3D::HexPrism         , 11);
    add("Capsule"          , Canvas::Type3D::Capsule          , 12);
    add("RotateCross"      , Canvas::Type3D::RotateCross      , 13);
    add("ExtrudeCross"     , Canvas::Type3D::ExtrudeCross     , 14);
    add("ElongateEllipsoid", Canvas::Type3D::ElongateEllipsoid, 15);
    add("ElongateTorus"    , Canvas::Type3D::ElongateTorus    , 16);
    add("OnionSphere"      , Canvas::Type3D::OnionSphere      , 17);
  }

  Canvas::Type3D indToType(int ind) {
    return lookup(ind);
  }

  int typeToInd(const Canvas::Type3D &type) {
    return lookup(type);
  }
};

Type2DInd type2DInd;
Type3DInd type3DInd;

}

//---

namespace CQShaderGen {

Control::
Control(App *app) :
 app_(app)
{
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  auto *layout = new QVBoxLayout(this);

  UI ui(this, layout);

  dimCombo_ = ui.addLabelEdit("Dim", new QComboBox);
  dimCombo_->addItems(QStringList() << "2D" << "3D");

  type2DCombo_ = ui.addLabelEdit("2D Type", new QComboBox);
  type2DCombo_->addItems(type2DInd.names());

  type3DCombo_ = ui.addLabelEdit("3D Type", new QComboBox);
  type3DCombo_->addItems(type3DInd.names());

  ui.startGroup("Camera");

  cameraXEdit_ = ui.addLabelEdit("X", new CQRealSpin);
  cameraYEdit_ = ui.addLabelEdit("Y", new CQRealSpin);
  cameraZEdit_ = ui.addLabelEdit("Z", new CQRealSpin);

  ui.endGroup();

  ui.startGroup("Light");

  lightXEdit_ = ui.addLabelEdit("X", new CQRealSpin);
  lightYEdit_ = ui.addLabelEdit("Y", new CQRealSpin);
  lightZEdit_ = ui.addLabelEdit("Z", new CQRealSpin);

  ui.endGroup();

  ui.addStretch();

  //---

  connectSlots(true);

  updateWidgets();
}

void
Control::
updateWidgets()
{
  connectSlots(false);

  dimCombo_->setCurrentIndex(app_->canvas()->is3D() ? 1 : 0);

  type2DCombo_->setCurrentIndex(type2DInd.typeToInd(app_->canvas()->type2D()));
  type3DCombo_->setCurrentIndex(type3DInd.typeToInd(app_->canvas()->type3D()));

  auto cameraPos = app_->canvas()->cameraPos();
  auto lightPos  = app_->canvas()->lightPos();

  cameraXEdit_->setValue(cameraPos.x);
  cameraYEdit_->setValue(cameraPos.y);
  cameraZEdit_->setValue(cameraPos.z);

  lightXEdit_->setValue(lightPos.x);
  lightYEdit_->setValue(lightPos.y);
  lightZEdit_->setValue(lightPos.z);

  connectSlots(true);
}

void
Control::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, dimCombo_, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(dimSlot(int)));
  CQUtil::connectDisconnect(b, type2DCombo_, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(type2DSlot(int)));
  CQUtil::connectDisconnect(b, type3DCombo_, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(type3DSlot(int)));

  CQUtil::connectDisconnect(b, cameraXEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(cameraXSlot(double)));
  CQUtil::connectDisconnect(b, cameraYEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(cameraYSlot(double)));
  CQUtil::connectDisconnect(b, cameraZEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(cameraZSlot(double)));

  CQUtil::connectDisconnect(b, lightXEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(lightXSlot(double)));
  CQUtil::connectDisconnect(b, lightYEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(lightYSlot(double)));
  CQUtil::connectDisconnect(b, lightZEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(lightZSlot(double)));
}

void
Control::
dimSlot(int ind)
{
  app_->canvas()->set3D(ind == 1);
}

void
Control::
type2DSlot(int ind)
{
  auto type = type2DInd.indToType(ind);

  app_->canvas()->setType2D(type);
}

void
Control::
type3DSlot(int ind)
{
  auto type = type3DInd.indToType(ind);

  app_->canvas()->setType3D(type);
}

void
Control::
cameraXSlot(double r)
{
  connectSlots(false);

  auto cameraPos = app_->canvas()->cameraPos();

  cameraPos.x = r;

  app_->canvas()->setCameraPos(cameraPos);

  connectSlots(true);
}

void
Control::
cameraYSlot(double r)
{
  connectSlots(false);

  auto cameraPos = app_->canvas()->cameraPos();

  cameraPos.y = r;

  app_->canvas()->setCameraPos(cameraPos);

  connectSlots(true);
}

void
Control::
cameraZSlot(double r)
{
  connectSlots(false);

  auto cameraPos = app_->canvas()->cameraPos();

  cameraPos.z = r;

  app_->canvas()->setCameraPos(cameraPos);

  connectSlots(true);
}

void
Control::
lightXSlot(double r)
{
  connectSlots(false);

  auto lightPos = app_->canvas()->lightPos();

  lightPos.x = r;

  app_->canvas()->setLightPos(lightPos);

  connectSlots(true);
}

void
Control::
lightYSlot(double r)
{
  connectSlots(false);

  auto lightPos = app_->canvas()->lightPos();

  lightPos.y = r;

  app_->canvas()->setLightPos(lightPos);

  connectSlots(true);
}

void
Control::
lightZSlot(double r)
{
  connectSlots(false);

  auto lightPos = app_->canvas()->lightPos();

  lightPos.z = r;

  app_->canvas()->setLightPos(lightPos);

  connectSlots(true);
}

}
