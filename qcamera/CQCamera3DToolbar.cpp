#include <CQCamera3DToolbar.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DOverview.h>
#include <CQCamera3DUI.h>

#include <CQUtil.h>
#include <CQPixmapCache.h>

#include <QComboBox>

CQCamera3DToolbar::
CQCamera3DToolbar(CQCamera3DApp *app) :
 app_(app)
{
  setObjectName("toolbar");

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  auto *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(4);

  //---

  CQCamera3DUI ui(this, layout);

  //---

  canvasSelectData_.frame = ui.startFrame(/*horizontal*/true);

  canvasSelectData_.editModeCombo = new QComboBox;
  canvasSelectData_.editModeCombo->addItems(QStringList() << "Select" << "Edit");

  ui.addWidget(canvasSelectData_.editModeCombo);

  canvasSelectData_.pointSelectButton =
    ui.addIconCheckButton("pointSelect", "POINT_SELECT", "Point Select");
  canvasSelectData_.edgeSelectButton =
    ui.addIconCheckButton("edgeSelect" , "EDGE_SELECT" , "Edge Select");
  canvasSelectData_.faceSelectButton =
    ui.addIconCheckButton("faceSelect" , "FACE_SELECT" , "Face Select");

  ui.endFrame();

  //---

  overviewSelectData_.frame = ui.startFrame(/*horizontal*/true);

  overviewSelectData_.pointSelectButton =
    ui.addIconCheckButton("pointSelect", "POINT_SELECT", "Point Select");
  overviewSelectData_.edgeSelectButton =
    ui.addIconCheckButton("edgeSelect" , "EDGE_SELECT" , "Edge Select");
  overviewSelectData_.faceSelectButton =
    ui.addIconCheckButton("faceSelect" , "FACE_SELECT" , "Face Select");
  overviewSelectData_.objectSelectButton =
    ui.addIconCheckButton("objectSelect", "OBJECT_SELECT", "Object Select");

  ui.endFrame();

  //---

  menuFrame_ = ui.startFrame(/*horizontal*/true);

  menuBar_ = ui.startMenuBar();

  ui.startMenu("View");

  ui.addAction("Perspective", SLOT(perspectiveSlot()));
  ui.addAction("Top"        , SLOT(topSlot()));
  ui.addAction("Bottom"     , SLOT(bottomSlot()));
  ui.addAction("Left"       , SLOT(leftSlot()));
  ui.addAction("Right"      , SLOT(rightSlot()));
  ui.addAction("Front"      , SLOT(frontSlot()));
  ui.addAction("Back"       , SLOT(backSlot()));

  ui.addMenuSeparator();

  ui.addCheckAction("Local", canvas()->isLocalMode(), SLOT(localSlot(bool)));

  ui.endMenu();

  ui.startMenu("Select");

  ui.addAction("All" , SLOT(selectAllSlot()));
  ui.addAction("None", SLOT(selectNoneSlot()));

  ui.endMenu();

  ui.startMenu("Add");

  ui.addAction("Plane"   , SLOT(addPlaneSlot()));
  ui.addAction("Cube"    , SLOT(addCubeSlot()));
  ui.addAction("Circle"  , SLOT(addCircleSlot()));
  ui.addAction("Sphere"  , SLOT(addSphereSlot()));
  ui.addAction("Cylinder", SLOT(addCylinderSlot()));
  ui.addAction("Cone"    , SLOT(addConeSlot()));
  ui.addAction("Torus"   , SLOT(addTorusSlot()));

  ui.endMenu();

  ui.startMenu("Object");

  ui.addAction("Transform");

  ui.endMenu();

  ui.endMenuBar();

  ui.endFrame();

  //---

  ui.addStretch();

  depthTestButton_ = ui.addIconCheckButton("depthTest", "DEPTH3D", "Depth Test");
  cullFaceButton_  = ui.addIconCheckButton("cullFace" , "CULL3D" , "Cull Face" );
  frontFaceButton_ = ui.addIconCheckButton("frontFace", "FRONT3D", "Front Face");

  wireframeButton_   = ui.addIconButton("wireframe"  , "WIREFRAME"   , "Wireframe"   );
  solidFillButton_   = ui.addIconButton("solidFill"  , "SOLID_FILL"  , "Solid Fill"  );
  textureFillButton_ = ui.addIconButton("textureFill", "TEXTURE_FILL", "Texture Fill");

  //---

  auto *debugButton = addDebugButton();

  ui.addWidget(debugButton);

  //---

  connectSlots(true);

  viewTypeSlot();

  updateWidgets();
}

QToolButton *
CQCamera3DToolbar::
addDebugButton()
{
  int is = QFontMetrics(font()).height() + 6;

  auto *button = new QToolButton;

  button->setIcon(CQPixmapCacheInst->getIcon("MENU"));
  button->setPopupMode(QToolButton::InstantPopup);

  button->setAutoRaise(true);
  button->setIconSize(QSize(is, is));

  auto *menu = new QMenu;

  auto *action1 = menu->addAction("Meta Edit");
  auto *action2 = menu->addAction("Performance");
  auto *action3 = menu->addAction("Options");

  connect(action1, SIGNAL(triggered()), this, SLOT(metaEditSlot()));
  connect(action2, SIGNAL(triggered()), this, SLOT(performanceSlot()));
  connect(action3, SIGNAL(triggered()), this, SLOT(optionsSlot()));

  button->setMenu(menu);

  return button;
}

void
CQCamera3DToolbar::
connectSlots(bool b)
{
  auto *canvas = this->canvas();

  CQUtil::connectDisconnect(b,
    app_, SIGNAL(viewTypeChanged()), this, SLOT(viewTypeSlot()));

  CQUtil::connectDisconnect(b,
    canvas, SIGNAL(selectTypeChanged()), this, SLOT(updateWidgets()));
  CQUtil::connectDisconnect(b,
    canvas, SIGNAL(editModeChanged()), this, SLOT(updateWidgets()));
  CQUtil::connectDisconnect(b,
    canvas, SIGNAL(glStateChanged()), this, SLOT(updateWidgets()));

  CQUtil::connectDisconnect(b,
    overview(), SIGNAL(selectTypeChanged()), this, SLOT(updateWidgets()));

  CQUtil::connectDisconnect(b, canvasSelectData_.editModeCombo,
    SIGNAL(currentIndexChanged(int)), this, SLOT(editModeSlot(int)));

  CQUtil::connectDisconnect(b,
    canvasSelectData_.pointSelectButton, SIGNAL(toggled(bool)), this, SLOT(pointSelectSlot(bool)));
  CQUtil::connectDisconnect(b,
    canvasSelectData_.edgeSelectButton, SIGNAL(toggled(bool)), this, SLOT(edgeSelectSlot(bool)));
  CQUtil::connectDisconnect(b,
    canvasSelectData_.faceSelectButton, SIGNAL(toggled(bool)), this, SLOT(faceSelectSlot(bool)));

  CQUtil::connectDisconnect(b,
    overviewSelectData_.pointSelectButton, SIGNAL(toggled(bool)),
    this, SLOT(pointSelectSlot(bool)));
  CQUtil::connectDisconnect(b,
    overviewSelectData_.edgeSelectButton, SIGNAL(toggled(bool)),
    this, SLOT(edgeSelectSlot(bool)));
  CQUtil::connectDisconnect(b,
    overviewSelectData_.faceSelectButton, SIGNAL(toggled(bool)),
    this, SLOT(faceSelectSlot(bool)));
  CQUtil::connectDisconnect(b,
    overviewSelectData_.objectSelectButton, SIGNAL(toggled(bool)),
    this, SLOT(objectSelectSlot(bool)));

  CQUtil::connectDisconnect(b,
    depthTestButton_, SIGNAL(clicked(bool)), this, SLOT(depthTestSlot(bool)));
  CQUtil::connectDisconnect(b,
    cullFaceButton_, SIGNAL(clicked(bool)), this, SLOT(cullFaceSlot(bool)));
  CQUtil::connectDisconnect(b,
    frontFaceButton_, SIGNAL(clicked(bool)), this, SLOT(frontFaceSlot(bool)));

  CQUtil::connectDisconnect(b,
    wireframeButton_, SIGNAL(clicked()), this, SLOT(wireframeSlot()));
  CQUtil::connectDisconnect(b,
    solidFillButton_, SIGNAL(clicked()), this, SLOT(solidFillSlot()));
  CQUtil::connectDisconnect(b,
    textureFillButton_, SIGNAL(clicked()), this, SLOT(textureFillSlot()));
}

void
CQCamera3DToolbar::
viewTypeSlot()
{
  viewType_ = app_->viewType();

  canvasSelectData_  .frame->setVisible(viewType_ == CQCamera3DViewType::MODEL);
  overviewSelectData_.frame->setVisible(viewType_ == CQCamera3DViewType::OVERVIEW);

  menuFrame_->setVisible(viewType_ == CQCamera3DViewType::MODEL ||
                         viewType_ == CQCamera3DViewType::OVERVIEW);

  updateWidgets();
}

void
CQCamera3DToolbar::
editModeSlot(int s)
{
  connectSlots(false);

  auto editMode = (s ? CQCamera3DCanvas::EditMode::EDIT : CQCamera3DCanvas::EditMode::OBJECT);

  canvas()->setEditMode(editMode);

  connectSlots(true);

  updateWidgets();
}

void
CQCamera3DToolbar::
objectSelectSlot(bool)
{
  if (viewType_ == CQCamera3DViewType::OVERVIEW)
    overviewSelectData_.selectType = CQCamera3DCanvas::SelectType::OBJECT;

  updateSelectType();

  updateWidgets();
}

void
CQCamera3DToolbar::
pointSelectSlot(bool)
{
  if      (viewType_ == CQCamera3DViewType::MODEL)
    canvasSelectData_.selectType = CQCamera3DCanvas::SelectType::POINT;
  else if (viewType_ == CQCamera3DViewType::OVERVIEW)
    overviewSelectData_.selectType = CQCamera3DCanvas::SelectType::POINT;

  updateSelectType();

  updateWidgets();
}

void
CQCamera3DToolbar::
edgeSelectSlot(bool)
{
  if      (viewType_ == CQCamera3DViewType::MODEL)
    canvasSelectData_.selectType = CQCamera3DCanvas::SelectType::EDGE;
  else if (viewType_ == CQCamera3DViewType::OVERVIEW)
    overviewSelectData_.selectType = CQCamera3DCanvas::SelectType::EDGE;

  updateSelectType();

  updateWidgets();
}

void
CQCamera3DToolbar::
faceSelectSlot(bool)
{
  if      (viewType_ == CQCamera3DViewType::MODEL)
    canvasSelectData_.selectType = CQCamera3DCanvas::SelectType::FACE;
  else if (viewType_ == CQCamera3DViewType::OVERVIEW)
    overviewSelectData_.selectType = CQCamera3DCanvas::SelectType::FACE;

  updateSelectType();

  updateWidgets();
}

void
CQCamera3DToolbar::
updateSelectType()
{
  connectSlots(false);

  if      (viewType_ == CQCamera3DViewType::MODEL)
    canvas()->setSelectType(canvasSelectData_.selectType);
  else if (viewType_ == CQCamera3DViewType::OVERVIEW)
    overview()->setSelectType(overviewSelectData_.selectType);

  connectSlots(true);
}

void
CQCamera3DToolbar::
updateWidgets()
{
  connectSlots(false);

  auto *canvas = this->canvas();

  depthTestButton_->setChecked(canvas->isDepthTest());
  cullFaceButton_ ->setChecked(canvas->isCullFace());
  frontFaceButton_->setChecked(canvas->isFrontFace());

  if      (viewType_ == CQCamera3DViewType::MODEL) {
    auto editMode = canvas->editMode();

    if (editMode == CQCamera3DCanvas::EditMode::EDIT)
      canvasSelectData_.selectType = canvas->selectType();

    canvasSelectData_.editModeCombo->
      setCurrentIndex(editMode == CQCamera3DCanvas::EditMode::EDIT ? 1 : 0);

    canvasSelectData_.pointSelectButton->setVisible(editMode == CQCamera3DCanvas::EditMode::EDIT);
    canvasSelectData_.edgeSelectButton ->setVisible(editMode == CQCamera3DCanvas::EditMode::EDIT);
    canvasSelectData_.faceSelectButton ->setVisible(editMode == CQCamera3DCanvas::EditMode::EDIT);

    canvasSelectData_.pointSelectButton->
      setChecked(canvasSelectData_.selectType == CQCamera3DCanvas::SelectType::POINT);
    canvasSelectData_.edgeSelectButton ->
      setChecked(canvasSelectData_.selectType == CQCamera3DCanvas::SelectType::EDGE);
    canvasSelectData_.faceSelectButton ->
      setChecked(canvasSelectData_.selectType == CQCamera3DCanvas::SelectType::FACE);
  }
  else if (viewType_ == CQCamera3DViewType::OVERVIEW) {
    overviewSelectData_.pointSelectButton->
      setChecked(overviewSelectData_.selectType == CQCamera3DCanvas::SelectType::POINT);
    overviewSelectData_.edgeSelectButton ->
      setChecked(overviewSelectData_.selectType == CQCamera3DCanvas::SelectType::EDGE);
    overviewSelectData_.faceSelectButton ->
      setChecked(overviewSelectData_.selectType == CQCamera3DCanvas::SelectType::FACE);
    overviewSelectData_.objectSelectButton->
      setChecked(overviewSelectData_.selectType == CQCamera3DCanvas::SelectType::OBJECT);
  }

  connectSlots(true);
}

void
CQCamera3DToolbar::
perspectiveSlot()
{
  canvas()->setViewType(CQCamera3DCanvas::ViewType::PERSPECTIVE);
}

void
CQCamera3DToolbar::
topSlot()
{
  canvas()->setViewType(CQCamera3DCanvas::ViewType::TOP);
}

void
CQCamera3DToolbar::
bottomSlot()
{
  canvas()->setViewType(CQCamera3DCanvas::ViewType::BOTTOM);
}

void
CQCamera3DToolbar::
leftSlot()
{
  canvas()->setViewType(CQCamera3DCanvas::ViewType::LEFT);
}

void
CQCamera3DToolbar::
rightSlot()
{
  canvas()->setViewType(CQCamera3DCanvas::ViewType::RIGHT);
}

void
CQCamera3DToolbar::
frontSlot()
{
  canvas()->setViewType(CQCamera3DCanvas::ViewType::FRONT);
}

void
CQCamera3DToolbar::
backSlot()
{
  canvas()->setViewType(CQCamera3DCanvas::ViewType::BACK);
}

void
CQCamera3DToolbar::
localSlot(bool)
{
  canvas()->setLocalMode(! canvas()->isLocalMode());
}

void
CQCamera3DToolbar::
selectAllSlot()
{
  canvas()->selectAllObjects();
}

void
CQCamera3DToolbar::
selectNoneSlot()
{
  canvas()->deselectAll();
}

void
CQCamera3DToolbar::
addPlaneSlot()
{
  canvas()->addPlane();
}

void
CQCamera3DToolbar::
addCubeSlot()
{
  if      (viewType_ == CQCamera3DViewType::MODEL)
    canvas()->addCube();
  else if (viewType_ == CQCamera3DViewType::OVERVIEW)
    overview()->addCube();
}

void
CQCamera3DToolbar::
addCircleSlot()
{
  if      (viewType_ == CQCamera3DViewType::MODEL)
    canvas()->addCircle();
  else if (viewType_ == CQCamera3DViewType::OVERVIEW)
    overview()->addCircle();
}

void
CQCamera3DToolbar::
addSphereSlot()
{
  if      (viewType_ == CQCamera3DViewType::MODEL)
    canvas()->addSphere();
  else if (viewType_ == CQCamera3DViewType::OVERVIEW)
    overview()->addSphere();
}

void
CQCamera3DToolbar::
addCylinderSlot()
{
  if      (viewType_ == CQCamera3DViewType::MODEL)
    canvas()->addCylinder();
  else if (viewType_ == CQCamera3DViewType::OVERVIEW)
    overview()->addCylinder();
}

void
CQCamera3DToolbar::
addConeSlot()
{
  if      (viewType_ == CQCamera3DViewType::MODEL)
    canvas()->addCone();
  else if (viewType_ == CQCamera3DViewType::OVERVIEW)
    overview()->addCone();
}

void
CQCamera3DToolbar::
addTorusSlot()
{
  if      (viewType_ == CQCamera3DViewType::MODEL)
    canvas()->addTorus();
  else if (viewType_ == CQCamera3DViewType::OVERVIEW)
    overview()->addTorus();
}

void
CQCamera3DToolbar::
depthTestSlot(bool b)
{
  auto *canvas = this->canvas();

  canvas->setDepthTest(b);

  canvas->update();
}

void
CQCamera3DToolbar::
cullFaceSlot(bool b)
{
  auto *canvas = this->canvas();

  canvas->setCullFace(b);

  canvas->update();
}

void
CQCamera3DToolbar::
frontFaceSlot(bool b)
{
  auto *canvas = this->canvas();

  canvas->setFrontFace(b);

  canvas->update();
}

void
CQCamera3DToolbar::
wireframeSlot()
{
  auto *canvas = this->canvas();

  canvas->setWireframe(true);
  canvas->setSolid    (false);
  canvas->setTextured (false);

  canvas->update();
}

void
CQCamera3DToolbar::
solidFillSlot()
{
  auto *canvas = this->canvas();

  canvas->setWireframe(false);
  canvas->setSolid    (true);
  canvas->setTextured (false);

  canvas->update();
}

void
CQCamera3DToolbar::
textureFillSlot()
{
  auto *canvas = this->canvas();

  canvas->setWireframe(false);
  canvas->setSolid    (true);
  canvas->setTextured (true);

  canvas->update();
}

void
CQCamera3DToolbar::
metaEditSlot()
{
  app_->showMetaEdit();
}

void
CQCamera3DToolbar::
performanceSlot()
{
  app_->showPerfDialog();
}

void
CQCamera3DToolbar::
optionsSlot()
{
  app_->showAppOptions();
}
