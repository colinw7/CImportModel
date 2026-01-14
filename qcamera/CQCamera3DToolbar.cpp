#include <CQCamera3DToolbar.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DCanvas.h>

#include <CQIconButton.h>

#include <QComboBox>
#include <QMenuBar>
#include <QHBoxLayout>

CQCamera3DToolbar::
CQCamera3DToolbar(CQCamera3DApp *app) :
 app_(app)
{
  setObjectName("toolbar");

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  auto *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(4);

  auto addToolButton = [&](const QString &name, const QString &iconName,
                           const QString &tip, const char *slotName) {
    auto *button = new CQIconButton;

    button->setObjectName(name);
    button->setIcon(iconName);
    button->setIconSize(QSize(32, 32));
    button->setAutoRaise(true);
    button->setToolTip(tip);

    connect(button, SIGNAL(clicked()), this, slotName);

    layout->addWidget(button);

    return button;
  };

  editCombo_ = new QComboBox;
  editCombo_->addItems(QStringList() << "Select" << "Edit");

  layout->addWidget(editCombo_);

  pointSelectButton_ =
    addToolButton("pointSelect", "POINT_SELECT", "Point Select", SLOT(pointSelectSlot()));
  edgeSelectButton_ =
    addToolButton("edgeSelect", "EDGE_SELECT", "Edge Select", SLOT(edgeSelectSlot()));
  faceSelectButton_ =
    addToolButton("faceSelect", "FACE_SELECT", "Face Select", SLOT(faceSelectSlot()));

  pointSelectButton_->setCheckable(true);
  edgeSelectButton_ ->setCheckable(true);
  faceSelectButton_ ->setCheckable(true);

  //---

  menuBar_ = new QMenuBar;

  layout->addWidget(menuBar_);

  //---

  QMenu *currentMenu = nullptr;
  std::vector<QMenu *> menuStack;

  auto startMenu = [&](const QString &name) {
    menuStack.push_back(currentMenu);

    if (! currentMenu)
      currentMenu = menuBar_->addMenu(name);
    else
      currentMenu = currentMenu->addMenu(name);
  };

  auto endMenu = [&]() {
    currentMenu = menuStack.back();

    menuStack.pop_back();
  };

  auto addAction = [&](const QString &name, const char *slotName=nullptr) {
    auto *action = currentMenu->addAction(name);

    if (slotName)
      connect(action, SIGNAL(triggered()), this, slotName);
  };

  //---

  startMenu("View");

  addAction("Top");
  addAction("Bottom");

  endMenu();

  startMenu("Select");

  addAction("All" , SLOT(selectAllSlot()));
  addAction("None", SLOT(selectNoneSlot()));

  endMenu();

  startMenu("Add");

  addAction("Plane"   , SLOT(addPlaneSlot()));
  addAction("Cube"    , SLOT(addCubeSlot()));
  addAction("Circle"  , SLOT(addCircleSlot()));
  addAction("Cylinder", SLOT(addCylinderSlot()));
  addAction("Cone"    , SLOT(addConeSlot()));
  addAction("Torus"   , SLOT(addTorusSlot()));

  endMenu();

  startMenu("Object");

  addAction("Transform");

  endMenu();

  //---

  layout->addStretch(1);

  wireframeButton_ =
    addToolButton("solidFill", "WIREFRAME", "Wireframe", SLOT(wireframeSlot()));
  solidFillButton_ =
    addToolButton("solidFill", "SOLID_FILL", "Solid Fill", SLOT(solidFillSlot()));
  textureFillButton_ =
    addToolButton("textureFill", "TEXTURE_FILL", "Texture Fill", SLOT(textureFillSlot()));
}

void
CQCamera3DToolbar::
pointSelectSlot()
{
}

void
CQCamera3DToolbar::
edgeSelectSlot()
{
}

void
CQCamera3DToolbar::
faceSelectSlot()
{
}

void
CQCamera3DToolbar::
selectAllSlot()
{
  auto *canvas = app_->canvas();

  canvas->selectAllObjects();
}

void
CQCamera3DToolbar::
selectNoneSlot()
{
  auto *canvas = app_->canvas();

  canvas->deselectAll();
}

void
CQCamera3DToolbar::
addPlaneSlot()
{
}

void
CQCamera3DToolbar::
addCubeSlot()
{
  app_->canvas()->addCube();
}

void
CQCamera3DToolbar::
addCircleSlot()
{
  app_->canvas()->addCircle();
}

void
CQCamera3DToolbar::
addCylinderSlot()
{
  app_->canvas()->addCylinder();
}

void
CQCamera3DToolbar::
addConeSlot()
{
}

void
CQCamera3DToolbar::
addTorusSlot()
{
  app_->canvas()->addTorus();
}

void
CQCamera3DToolbar::
wireframeSlot()
{
  auto *canvas = app_->canvas();

  canvas->setWireframe(true);
  canvas->setSolid    (false);
  canvas->setTextured (false);

  canvas->update();
}

void
CQCamera3DToolbar::
solidFillSlot()
{
  auto *canvas = app_->canvas();

  canvas->setWireframe(false);
  canvas->setSolid    (true);
  canvas->setTextured (false);

  canvas->update();
}

void
CQCamera3DToolbar::
textureFillSlot()
{
  auto *canvas = app_->canvas();

  canvas->setWireframe(false);
  canvas->setSolid    (true);
  canvas->setTextured (true);

  canvas->update();
}
