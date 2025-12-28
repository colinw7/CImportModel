#include <CQCamera3DControl.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DOverview.h>
#include <CQCamera3DCamera.h>
#include <CQCamera3DLight.h>
#include <CQCamera3DGeomObject.h>
#include <CQCamera3DStatus.h>
#include <CQCamera3DAxes.h>
#include <CQCamera3DUVMap.h>
#include <CQCamera3DNormals.h>
#include <CQCamera3DBones.h>
#include <CQCamera3DObjectsList.h>
#include <CQCamera3DMaterialList.h>
#include <CQCamera3DLightList.h>
#include <CQCamera3DTextureChooser.h>
#include <CQCamera3DAnimChooser.h>
#include <CQCamera3DBonesList.h>
#include <CQCamera3DUtil.h>

#include <CImportBase.h>
#include <CGeometry3D.h>
#include <CGeomScene3D.h>
#include <CGeomObject3D.h>
#include <CGeomFace3D.h>
#include <CGeomNodeData.h>

#include <CGeomCube3D.h>
#include <CGeomCylinder3D.h>
#include <CGeomPyramid3D.h>
#include <CGeomSphere3D.h>
#include <CGeomTorus3D.h>
#include <CQGLTexture.h>

#include <CQMatrix3D.h>
#include <CQPoint3DEdit.h>
#include <CQPoint4DEdit.h>
#include <CQRealSpin.h>
#include <CQColorEdit.h>
#include <CQIconButton.h>
#include <CQTextLabel.h>
#include <CQUtil.h>

#include <QFileDialog>
#include <QTableWidget>
#include <QHeaderView>
#include <QTextEdit>
#include <QGroupBox>
#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QMouseEvent>

namespace {

class SelectTypeInd : public ValueMap<CQCamera3DSelectType, int> {
 public:
  SelectTypeInd() {
    add("Object", CQCamera3DSelectType::OBJECT, 0);
    add("Face"  , CQCamera3DSelectType::FACE  , 1);
    add("Edge"  , CQCamera3DSelectType::EDGE  , 2);
    add("Point" , CQCamera3DSelectType::POINT , 3);
  };

  CQCamera3DSelectType indToType(int ind) {
    return lookup(ind);
  }

  int typeToInd(const CQCamera3DSelectType &type) {
    return lookup(type);
  }
};

SelectTypeInd selectTypeInd;

class MouseTypeInd : public ValueMap<CQCamera3DMouseType, int> {
 public:
  MouseTypeInd() {
    add("Camera", CQCamera3DMouseType::CAMERA, 0);
    add("Object", CQCamera3DMouseType::OBJECT, 1);
    add("Light" , CQCamera3DMouseType::LIGHT , 2);
  };

  CQCamera3DMouseType indToType(int ind) {
    return lookup(ind);
  }

  int typeToInd(const CQCamera3DMouseType &type) {
    return lookup(type);
  }
};

MouseTypeInd mouseTypeInd;

class UVTextureTypeInd : public ValueMap<CQCamera3DUVMap::TextureType, int> {
 public:
  UVTextureTypeInd() {
    add("Diffuse" , CQCamera3DUVMap::TextureType::DIFFUSE , 0);
    add("Normal"  , CQCamera3DUVMap::TextureType::NORMAL  , 1);
    add("Specular", CQCamera3DUVMap::TextureType::SPECULAR, 2);
    add("Emissive", CQCamera3DUVMap::TextureType::EMISSIVE, 3);
  };

  CQCamera3DUVMap::TextureType indToType(int ind) {
    return lookup(ind);
  }

  int typeToInd(const CQCamera3DUVMap::TextureType &type) {
    return lookup(type);
  }
};

UVTextureTypeInd uvTextureTypeInd;

class LightTypeInd : public ValueMap<CGeomLight3DType, int> {
 public:
  LightTypeInd() {
    add("Directional", CGeomLight3DType::DIRECTIONAL, 0);
    add("Point"      , CGeomLight3DType::POINT      , 1);
    add("Spot"       , CGeomLight3DType::SPOT       , 2);
  };

  CGeomLight3DType indToType(int ind) {
    return lookup(ind);
  }

  int typeToInd(const CGeomLight3DType &type) {
    return lookup(type);
  }
};

LightTypeInd lightTypeInd;

class OverviewModelTypeInd : public ValueMap<CQCamera3DOverview::ModelType, int> {
 public:
  OverviewModelTypeInd() {
    add("None"     , CQCamera3DOverview::ModelType::NONE     , 0);
    add("Wireframe", CQCamera3DOverview::ModelType::WIREFRAME, 1);
    add("Solid"    , CQCamera3DOverview::ModelType::SOLID    , 2);
  };

  CQCamera3DOverview::ModelType indToType(int ind) {
    return lookup(ind);
  }

  int typeToInd(const CQCamera3DOverview::ModelType &type) {
    return lookup(type);
  }
};

OverviewModelTypeInd overviewModelTypeInd;

class MaterialShadingInd : public ValueMap<CGeomMaterial::Shading, int> {
 public:
  MaterialShadingInd() {
    add("Wire"   , CGeomMaterial::Shading::WIRE   , 0);
    add("Flat"   , CGeomMaterial::Shading::FLAT   , 1);
    add("Gouraud", CGeomMaterial::Shading::GOURAUD, 2);
    add("Phong"  , CGeomMaterial::Shading::PHONG  , 3);
    add("Metal"  , CGeomMaterial::Shading::METAL  , 4);
  };

  CGeomMaterial::Shading indToType(int ind) {
    return lookup(ind);
  }

  int typeToInd(const CGeomMaterial::Shading &type) {
    return lookup(type);
  }
};

MaterialShadingInd materialShadingInd;

class AnimInterpolationInd : public ValueMap<CGeomAnimationData::Interpolation, int> {
 public:
  AnimInterpolationInd() {
    add("None"  , CGeomAnimationData::Interpolation::NONE       , 0);
    add("Linear", CGeomAnimationData::Interpolation::LINEAR     , 1);
    add("Step"  , CGeomAnimationData::Interpolation::STEP       , 2);
    add("Cubic" , CGeomAnimationData::Interpolation::CUBICSPLINE, 3);
  };

  CGeomAnimationData::Interpolation indToType(int ind) {
    return lookup(ind);
  }

  int typeToInd(const CGeomAnimationData::Interpolation &type) {
    return lookup(type);
  }
};

AnimInterpolationInd animInterpolationInd;

}

CQCamera3DControl::
CQCamera3DControl(CQCamera3DApp *app) :
 app_(app)
{
  setObjectName("control");

  auto *canvas = app_->canvas();

  //---

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  auto *layout = new QVBoxLayout(this);

  //---

  QBoxLayout *currentLayout = nullptr;

  auto addLabelEdit = [&](const QString &label, auto *w) {
    auto *frame = new QFrame;

    auto *layout1 = new QHBoxLayout(frame);

    layout1->addWidget(new QLabel(label));
    layout1->addWidget(w);

    currentLayout->addWidget(frame);

    return w;
  };

  auto addCheck = [&](const QString &label) {
    auto *check = new QCheckBox(label);

    currentLayout->addWidget(check);

    return check;
  };

  auto addStretch = [&]() {
    currentLayout->addStretch(1);
  };

  //---

  std::vector<QBoxLayout *> layoutStack;

  auto startGroup = [&](const QString &name, bool horizontal=false) {
    auto *group = new QGroupBox(name);

    QBoxLayout *layout = nullptr;

    if (horizontal)
      layout = new QHBoxLayout(group);
    else
      layout = new QVBoxLayout(group);

    currentLayout->addWidget(group);

    layoutStack.push_back(currentLayout);

    currentLayout = layout;
  };

  auto endGroup = [&]() {
    currentLayout = layoutStack.back();

    layoutStack.pop_back();
  };

  auto startFrame = [&](bool horizontal=false) {
    auto *frame = new QFrame;

    QBoxLayout *layout = nullptr;

    if (horizontal)
      layout = new QHBoxLayout(frame);
    else
      layout = new QVBoxLayout(frame);

    currentLayout->addWidget(frame);

    layoutStack.push_back(currentLayout);

    currentLayout = layout;
  };

  auto endFrame = [&]() {
    currentLayout = layoutStack.back();

    layoutStack.pop_back();
  };

  //---

  mainTab_ = CQUtil::makeWidget<QTabWidget>("mainTab");

  mainTab_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  layout->addWidget(mainTab_);

  //---

  // 3D

  auto *threeDTabFrame = CQUtil::makeWidget<QFrame>("threeD");
  auto *threeDTabLayout = new QVBoxLayout(threeDTabFrame);

  threeDTabFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  mainTab_->addTab(threeDTabFrame, "3D");

  //---

  auto *tab = CQUtil::makeWidget<QTabWidget>("tab");

  tab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  threeDTabLayout->addWidget(tab);

  //------

  // 3D/General

  auto *generalWidget = CQUtil::makeWidget<QFrame>("general");
  auto *generalLayout = new QVBoxLayout(generalWidget);

  tab->addTab(generalWidget, "General");

  currentLayout = generalLayout;

  //---

  startGroup("Show");

  showWireframeCheck_ = addCheck("Wireframe");
  showSolidCheck_     = addCheck("Solid");

  endGroup();

  startGroup("Normals");

  showNormalsCheck_ = addCheck("Show");

  normalsSizeEdit_  = addLabelEdit("Size" , new CQRealSpin);
  normalsColorEdit_ = addLabelEdit("Color", new CQColorEdit);

  endGroup();

  startGroup("Options");

  depthTestCheck_ = addCheck("Depth Test");
  cullFaceCheck_  = addCheck("Cull Face");
  frontFaceCheck_ = addCheck("Front Face");

  endGroup();

  addStretch();

  //------

  // 3D/Camera

  auto *cameraWidget = CQUtil::makeWidget<QFrame>("camera");
  auto *cameraLayout = new QVBoxLayout(cameraWidget);

  tab->addTab(cameraWidget, "Camera");

  currentLayout = cameraLayout;

  //---

  cameraData_.altCheck = addCheck("Alt");

  cameraData_.showCheck  = addCheck("Show Camera");
  cameraData_.eyeLineCheck = addCheck("Show Eye Line");
  cameraData_.planesCheck  = addCheck("Show Planes");

  cameraData_.pitchEdit = addLabelEdit("Pitch", new CQRealSpin);
  cameraData_.yawEdit   = addLabelEdit("Yaw"  , new CQRealSpin);
  cameraData_.rollEdit  = addLabelEdit("Roll" , new CQRealSpin);

  cameraData_.nearEdit = addLabelEdit("Near", new CQRealSpin);
  cameraData_.farEdit  = addLabelEdit("Far" , new CQRealSpin);

  //---

  startGroup("Origin");

  cameraData_.xOriginEdit = addLabelEdit("X", new CQRealSpin);
  cameraData_.yOriginEdit = addLabelEdit("Y", new CQRealSpin);
  cameraData_.zOriginEdit = addLabelEdit("Z", new CQRealSpin);

  endGroup();

  //---

  startGroup("Position");

  cameraData_.xEdit = addLabelEdit("X", new CQRealSpin);
  cameraData_.yEdit = addLabelEdit("Y", new CQRealSpin);
  cameraData_.zEdit = addLabelEdit("Z", new CQRealSpin);

  endGroup();

  //---

  currentLayout = cameraLayout;

  cameraData_.fovEdit = addLabelEdit("FOV", new CQRealSpin);

  cameraData_.eyeZ1Edit = addLabelEdit("Eye Z1", new CQRealSpin);
  cameraData_.eyeZ2Edit = addLabelEdit("Eye Z2", new CQRealSpin);

  auto plusMinusFrame = [&](const QString &name) {
    PlusMinusData data;

    auto *pframe  = new QFrame;
    auto *playout = new QHBoxLayout(pframe);

    data.minusButton = new QToolButton;
    data.minusButton->setText("-");

    data.plusButton = new QToolButton;
    data.plusButton->setText("+");

    playout->addWidget(new QLabel(name));
    playout->addWidget(data.minusButton);
    playout->addWidget(data.plusButton);

    playout->addStretch(1);

    currentLayout->addWidget(pframe);

    return data;
  };

  cameraData_.aroundXButtons = plusMinusFrame("Around X");
  cameraData_.aroundYButtons = plusMinusFrame("Around Y");
  cameraData_.aroundZButtons = plusMinusFrame("Around Z");

  addStretch();

  auto *buttonFrame  = new QFrame;
  auto *buttonLayout = new QHBoxLayout(buttonFrame);

  currentLayout->addWidget(buttonFrame);

  auto *resetButton = new QPushButton("Reset");
  auto *topButton   = new QPushButton("Top");
  auto *sideButton  = new QPushButton("Side");
  auto *frontButton = new QPushButton("Front");

  connect(resetButton, SIGNAL(clicked()), this, SLOT(resetSlot()));
  connect(topButton  , SIGNAL(clicked()), this, SLOT(topSlot()));
  connect(sideButton , SIGNAL(clicked()), this, SLOT(sideSlot()));
  connect(frontButton, SIGNAL(clicked()), this, SLOT(frontSlot()));

  buttonLayout->addWidget(resetButton);
  buttonLayout->addWidget(topButton);
  buttonLayout->addWidget(sideButton);
  buttonLayout->addWidget(frontButton);
  buttonLayout->addStretch(1);

  //------

  // 3D/Lights

  auto *lightsWidget = CQUtil::makeWidget<QFrame>("lights");
  auto *lightsLayout = new QVBoxLayout(lightsWidget);

  tab->addTab(lightsWidget, "Lights");

  currentLayout = lightsLayout;

  lightsData_.lightsList = new CQCamera3DLightList(canvas);

  currentLayout->addWidget(lightsData_.lightsList);

  startGroup("Globals");

  lightsData_.ambientColorEdit    = addLabelEdit("Ambient Color", new CQColorEdit);
  lightsData_.ambientStrengthEdit = addLabelEdit("Ambient Strength", new CQRealSpin);

  lightsData_.diffuseStrengthEdit  = addLabelEdit("Diffuse Strength" , new CQRealSpin);
  lightsData_.emissiveStrengthEdit = addLabelEdit("Emission Strength", new CQRealSpin);
  lightsData_.specularStrengthEdit = addLabelEdit("Specular Strength", new CQRealSpin);

  lightsData_.fixedDiffuseCheck = addLabelEdit("Fixed Diffuse", new QCheckBox);

  endGroup();

  lightsData_.typeCombo = addLabelEdit("Type", new QComboBox);
  lightsData_.typeCombo->addItems(lightTypeInd.names());

  lightsData_.enabledCheck = addLabelEdit("Enabled", new QCheckBox);

  lightsData_.positionEdit = addLabelEdit("Position", new CQPoint3DEdit);

  lightsData_.colorEdit = addLabelEdit("Color", new CQColorEdit);

  // direction light
  lightsData_.directionEdit = addLabelEdit("Direction", new CQPoint3DEdit);

  // point light
  lightsData_.pointRadiusEdit = addLabelEdit("Point Radius", new CQRealSpin);

  lightsData_.attenuation0Edit = addLabelEdit("Constant Attenuation", new CQRealSpin);
  lightsData_.attenuation1Edit = addLabelEdit("Linear Attenuation", new CQRealSpin);
  lightsData_.attenuation2Edit = addLabelEdit("Quadratic Attenuation", new CQRealSpin);

  // spot light
  lightsData_.spotDirectionEdit = addLabelEdit("Spot Direction", new CQPoint3DEdit);
  lightsData_.spotExponentEdit  = addLabelEdit("Spot Exponent", new CQRealSpin);
  lightsData_.spotCutOffAngle   = addLabelEdit("Spot Cut Off", new CQRealSpin);

  addStretch();

  auto *lightsButtonsFrame  = new QFrame;
  auto *lightsButtonsLayout = new QHBoxLayout(lightsButtonsFrame);

  currentLayout->addWidget(lightsButtonsFrame);

  auto *addLightButton   = new QPushButton("Add Light");
  auto *resetLightButton = new QPushButton("Reset Light");

  connect(addLightButton  , SIGNAL(clicked()), this, SLOT(addLightSlot()));
  connect(resetLightButton, SIGNAL(clicked()), this, SLOT(resetLightSlot()));

  lightsButtonsLayout->addWidget(addLightButton);
  lightsButtonsLayout->addWidget(resetLightButton);
  lightsButtonsLayout->addStretch(1);

  //------

  // 3D/Axis

  auto *axisWidget = CQUtil::makeWidget<QFrame>("axis");
  auto *axisLayout = new QVBoxLayout(axisWidget);

  tab->addTab(axisWidget, "Axis");

  currentLayout = axisLayout;

  axisData_.showCheck = addCheck("Show Axes");

  axisData_.xPosEdit = addLabelEdit("X Pos", new CQRealSpin);
  axisData_.yPosEdit = addLabelEdit("Y Pos", new CQRealSpin);
  axisData_.zPosEdit = addLabelEdit("Z Pos", new CQRealSpin);

  addStretch();

  //------

  // 3D/Mouse

  auto *mouseWidget = CQUtil::makeWidget<QFrame>("mouse");
  auto *mouseLayout = new QVBoxLayout(mouseWidget);

  tab->addTab(mouseWidget, "Mouse");

  currentLayout = mouseLayout;

  mouseTypeCombo_ = addLabelEdit("Type", new QComboBox);
  mouseTypeCombo_->addItems(mouseTypeInd.names());

  mouseScaleEdit_ = addLabelEdit("Type", new CQRealSpin);

  addStretch();

  //------

  // 3D/Selection

  auto *selectionWidget = CQUtil::makeWidget<QFrame>("selection");
  auto *selectionLayout = new QVBoxLayout(selectionWidget);

  tab->addTab(selectionWidget, "Selection");

  currentLayout = selectionLayout;

  selectionData_.typeCombo = addLabelEdit("Select", new QComboBox);
  selectionData_.typeCombo->addItems(selectTypeInd.names());

  selectionData_.indLabel = addLabelEdit("Name", new CQTextLabel);
  selectionData_.visCheck = addLabelEdit("Visible", new QCheckBox);

  selectionData_.colorEdit = addLabelEdit("Color", new CQColorEdit);

  //---

  startGroup("Transform");

  selectionData_.matrixEdit = new CQMatrix3D;
  selectionLayout->addWidget(selectionData_.matrixEdit);

  selectionData_.translationEdit = addLabelEdit("Translation", new CQPoint3DEdit);
  selectionData_.rotationEdit    = addLabelEdit("Rotation"   , new CQPoint3DEdit);
  selectionData_.scaleEdit       = addLabelEdit("Scale"      , new CQPoint3DEdit);

  endGroup();

  //---

  startGroup("Textures");

  selectionData_.diffuseTextureEdit =
    addLabelEdit("Diffuse" , new CQCamera3DTextureChooser(app_));
  selectionData_.normalTextureEdit =
    addLabelEdit("Normal"  , new CQCamera3DTextureChooser(app_));
  selectionData_.specularTextureEdit =
    addLabelEdit("Specular", new CQCamera3DTextureChooser(app_));
  selectionData_.emissiveTextureEdit =
    addLabelEdit("Emissive", new CQCamera3DTextureChooser(app_));

  startFrame(/*horizontal*/true);

  auto *addTextureButton     = new QPushButton("Add Texture");
  auto *loadTextureMapButton = new QPushButton("Load Texture Map");
  auto *saveTextureMapButton = new QPushButton("Save Texture Map");

  connect(addTextureButton    , SIGNAL(clicked()), this, SLOT(addTextureSlot()));
  connect(loadTextureMapButton, SIGNAL(clicked()), this, SLOT(loadTextureMapSlot()));
  connect(saveTextureMapButton, SIGNAL(clicked()), this, SLOT(saveTextureMapSlot()));

  currentLayout->addWidget(addTextureButton);
  currentLayout->addWidget(loadTextureMapButton);
  currentLayout->addWidget(saveTextureMapButton);
  currentLayout->addStretch(1);

  endFrame();

  endGroup();

  //---

  startGroup("Material");

  selectionData_.materialNameEdit = addLabelEdit("Name", new QLineEdit);

  startFrame(/*horizontal*/true);

  auto *loadMaterialMapButton = new QPushButton("Load Material Map");
  auto *saveMaterialMapButton = new QPushButton("Save Material Map");

  connect(loadMaterialMapButton, SIGNAL(clicked()), this, SLOT(loadMaterialMapSlot()));
  connect(saveMaterialMapButton, SIGNAL(clicked()), this, SLOT(saveMaterialMapSlot()));

  currentLayout->addWidget(loadMaterialMapButton);
  currentLayout->addWidget(saveMaterialMapButton);
  currentLayout->addStretch(1);

  endFrame();

  endGroup();

  //---

  startGroup("Modify");

  startFrame(/*horizontal*/true);

  auto *swapXYButton = new QPushButton("Swap XY");
  auto *swapYZButton = new QPushButton("Swap YZ");
  auto *swapZXButton = new QPushButton("Swap ZX");

  connect(swapXYButton, SIGNAL(clicked()), this, SLOT(swapSlot()));
  connect(swapYZButton, SIGNAL(clicked()), this, SLOT(swapSlot()));
  connect(swapZXButton, SIGNAL(clicked()), this, SLOT(swapSlot()));

  currentLayout->addWidget(swapXYButton);
  currentLayout->addWidget(swapYZButton);
  currentLayout->addWidget(swapZXButton);

  addStretch();

  endFrame();

  startFrame(/*horizontal*/true);

  auto *invertXButton = new QPushButton("Invert X");
  auto *invertYButton = new QPushButton("Invert Y");
  auto *invertZButton = new QPushButton("Invert Z");

  connect(invertXButton, SIGNAL(clicked()), this, SLOT(invertSlot()));
  connect(invertYButton, SIGNAL(clicked()), this, SLOT(invertSlot()));
  connect(invertZButton, SIGNAL(clicked()), this, SLOT(invertSlot()));

  currentLayout->addWidget(invertXButton);
  currentLayout->addWidget(invertYButton);
  currentLayout->addWidget(invertZButton);

  addStretch();

  endFrame();

  endGroup();

  //---

  selectionData_.objectInfoText = new QTextEdit;

  selectionData_.objectInfoText->setReadOnly(true);

  currentLayout->addWidget(selectionData_.objectInfoText);

  //addStretch();

  //---

  auto *selectButtonFrame  = new QFrame;
  auto *selectButtonLayout = new QHBoxLayout(selectButtonFrame);;

  currentLayout->addWidget(selectButtonFrame);

  auto *selectParentButton  = new QPushButton("Select Parent");
  auto *selectRelatedButton = new QPushButton("Select Related");
  auto *deselectButton      = new QPushButton("Deselect All");

  connect(selectParentButton , SIGNAL(clicked()), this, SLOT(selectParentSlot()));
  connect(selectRelatedButton, SIGNAL(clicked()), this, SLOT(selectRelatedSlot()));
  connect(deselectButton     , SIGNAL(clicked()), this, SLOT(deselectSlot()));

  selectButtonLayout->addWidget(selectParentButton);
  selectButtonLayout->addWidget(selectRelatedButton);
  selectButtonLayout->addWidget(deselectButton);
  selectButtonLayout->addStretch(1);

  //------

  // 3D/Objects

  auto *objectsWidget = CQUtil::makeWidget<QFrame>("objects");
  auto *objectsLayout = new QVBoxLayout(objectsWidget);

  tab->addTab(objectsWidget, "Objects");

  currentLayout = objectsLayout;

  objectsList_ = new CQCamera3DObjectsList(canvas);

  currentLayout->addWidget(objectsList_);

  auto *objectSelectButton = new QPushButton("Select");
  auto *objectZoomButton   = new QPushButton("Zoom");

  connect(objectSelectButton, SIGNAL(clicked()), this, SLOT(objectSelectSlot()));
  connect(objectZoomButton  , SIGNAL(clicked()), this, SLOT(objectZoomSlot()));

  currentLayout->addWidget(objectSelectButton);
  currentLayout->addWidget(objectZoomButton);

  auto *addShapeFrame  = new QFrame;
  auto *addShapeLayout = new QVBoxLayout(addShapeFrame);;

  currentLayout->addWidget(addShapeFrame);

  auto *addCubeButton     = new QPushButton("Add Cube");
  auto *addCylinderButton = new QPushButton("Add Cylinder");
  auto *addPyramidButton  = new QPushButton("Add Pyramid");
  auto *addSphereButton   = new QPushButton("Add Sphere");
  auto *addTorusButton    = new QPushButton("Add Torus");

  addShapeLayout->addWidget(addCubeButton);
  addShapeLayout->addWidget(addCylinderButton);
  addShapeLayout->addWidget(addPyramidButton);
  addShapeLayout->addWidget(addSphereButton);
  addShapeLayout->addWidget(addTorusButton);

  connect(addCubeButton    , SIGNAL(clicked()), this, SLOT(addCubeSlot()));
  connect(addCylinderButton, SIGNAL(clicked()), this, SLOT(addCylinderSlot()));
  connect(addPyramidButton , SIGNAL(clicked()), this, SLOT(addPyramidSlot()));
  connect(addSphereButton  , SIGNAL(clicked()), this, SLOT(addSphereSlot()));
  connect(addTorusButton   , SIGNAL(clicked()), this, SLOT(addTorusSlot()));

  auto *addButton = new QPushButton("Add File");

  connect(addButton, SIGNAL(clicked()), this, SLOT(addModelSlot()));

  currentLayout->addWidget(addButton);

  addStretch();

  //------

  // 3D/Materials

  auto *materialsWidget = CQUtil::makeWidget<QFrame>("materials");
  auto *materialsLayout = new QVBoxLayout(materialsWidget);

  tab->addTab(materialsWidget, "Materials");

  currentLayout = materialsLayout;

  materialsData_.materialList = new CQCamera3DMaterialList(canvas);

  currentLayout->addWidget(materialsData_.materialList);

  materialsData_.ambientEdit  = addLabelEdit("Ambient" , new CQColorEdit);
  materialsData_.diffuseEdit  = addLabelEdit("Diffuse" , new CQColorEdit);
  materialsData_.specularEdit = addLabelEdit("Specular", new CQColorEdit);
  materialsData_.emissionEdit = addLabelEdit("Emission", new CQColorEdit);

  materialsData_.shininessEdit = addLabelEdit("Shininess", new CQRealSpin);

  materialsData_.transparencyEdit = addLabelEdit("Transparency", new CQRealSpin);

  materialsData_.twoSidedCheck = addLabelEdit("Two Sided", new QCheckBox);

  materialsData_.shadingCombo = addLabelEdit("Shading", new QComboBox);
  materialsData_.shadingCombo->addItems(materialShadingInd.names());

  materialsData_.ambientTextureEdit =
    addLabelEdit("Ambient" , new CQCamera3DTextureChooser(app_));
  materialsData_.diffuseTextureEdit =
    addLabelEdit("Diffuse" , new CQCamera3DTextureChooser(app_));
  materialsData_.normalTextureEdit =
    addLabelEdit("Normal"  , new CQCamera3DTextureChooser(app_));
  materialsData_.specularTextureEdit =
    addLabelEdit("Specular", new CQCamera3DTextureChooser(app_));
  materialsData_.emissiveTextureEdit =
    addLabelEdit("Emissive", new CQCamera3DTextureChooser(app_));

  //------

  // Overview

  auto *overviewTabFrame = CQUtil::makeWidget<QFrame>("overview");
  auto *overviewTabLayout = new QVBoxLayout(overviewTabFrame);

  overviewTabFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  mainTab_->addTab(overviewTabFrame, "Overview");

  currentLayout = overviewTabLayout;

  overviewData_.equalScale = addLabelEdit("Equal Scale", new QCheckBox);

  overviewData_.selectTypeCombo = addLabelEdit("Select", new QComboBox);
  overviewData_.selectTypeCombo->addItems(selectTypeInd.names());

  overviewData_.mouseTypeCombo = addLabelEdit("Mouse", new QComboBox);
  overviewData_.mouseTypeCombo->addItems(mouseTypeInd.names());

  startGroup("Model");

  overviewData_.modelTypeCombo = addLabelEdit("Type", new QComboBox);
  overviewData_.modelTypeCombo->addItems(overviewModelTypeInd.names());

  endGroup();

  startGroup("Camera");

  overviewData_.cameraCheck = addLabelEdit("Visible", new QCheckBox);

  endGroup();

  startGroup("Lights");

  overviewData_.lightsCheck = addLabelEdit("Visible", new QCheckBox);

  endGroup();

  addStretch();

  //------

  // UV

  auto *uvTabFrame = CQUtil::makeWidget<QFrame>("UV");
  auto *uvTabLayout = new QVBoxLayout(uvTabFrame);

  uvTabFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  mainTab_->addTab(uvTabFrame, "UV");

  currentLayout = uvTabLayout;

  uvData_.typeCombo = addLabelEdit("Texture", new QComboBox);
  uvData_.typeCombo->addItems(uvTextureTypeInd.names());

  addStretch();

  //------

  // Textures

  auto *texturesTabFrame = CQUtil::makeWidget<QFrame>("textures");
  auto *texturesTabLayout = new QVBoxLayout(texturesTabFrame);

  mainTab_->addTab(texturesTabFrame, "Textures");

  texturesTabFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  currentLayout = texturesTabLayout;

  addStretch();

  //------

  // Bones

  auto *bonesTabFrame = CQUtil::makeWidget<QFrame>("bones");
  auto *bonesTabLayout = new QVBoxLayout(bonesTabFrame);

  bonesTabFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  mainTab_->addTab(bonesTabFrame, "Bones");

  currentLayout = bonesTabLayout;

  bonesData_.modelCheck = addLabelEdit("Show Model", new QCheckBox);

  bonesData_.bonesList = new CQCamera3DBonesList(app_);

  currentLayout->addWidget(bonesData_.bonesList);

  startGroup("Default");

  bonesData_.translationEdit = addLabelEdit("Translation", new CQPoint3DEdit);
  bonesData_.rotationEdit    = addLabelEdit("Rotation"   , new CQPoint4DEdit);
  bonesData_.scaleEdit       = addLabelEdit("Scale"      , new CQPoint3DEdit);

  endGroup();

  startGroup("Animation");

  bonesData_.animTranslationEdit = addLabelEdit("Translation", new CQPoint3DEdit);
  bonesData_.animRotationEdit    = addLabelEdit("Rotation"   , new CQPoint4DEdit);
  bonesData_.animScaleEdit       = addLabelEdit("Scale"      , new CQPoint3DEdit);

  endGroup();

  //addStretch();

  //------

  // Animation

  auto *animTabFrame = CQUtil::makeWidget<QFrame>("animation");
  auto *animTabLayout = new QVBoxLayout(animTabFrame);

  animTabFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  mainTab_->addTab(animTabFrame, "Animation");

  currentLayout = animTabLayout;

  animData_.animCombo = addLabelEdit("Name", new CQCamera3DAnimChooser(app_));
  animData_.timeEdit  = addLabelEdit("Time", new CQRealSpin);

  auto addToolButton = [&](const QString &name, const QString &iconName,
                           const QString &tip, const char *slotName) {
    auto *button = new CQIconButton;

    button->setObjectName(name);
    button->setIcon(iconName);
    button->setIconSize(QSize(32, 32));
    button->setAutoRaise(true);
    button->setToolTip(tip);

    connect(button, SIGNAL(clicked()), this, slotName);

    currentLayout->addWidget(button);

    return button;
  };

  startFrame(/*horizontal*/true);

  animData_.playButton  = addToolButton("play" , "PLAY"    , "Play" , SLOT(playSlot()));
  animData_.pauseButton = addToolButton("pause", "PAUSE"   , "Pause", SLOT(pauseSlot()));
  animData_.stepButton  = addToolButton("step" , "PLAY_ONE", "Step" , SLOT(stepSlot()));

  addStretch();

  endFrame();

  animData_.nodeLabel = addLabelEdit("Node", new CQTextLabel);

  auto createTableWidget = []() {
    auto *table = new QTableWidget;

    table->horizontalHeader()->setStretchLastSection(true);

    return table;
  };

  startGroup("Translation");

  animData_.translationInterpCombo = addLabelEdit("Interpolation", new QComboBox);
  animData_.translationInterpCombo->addItems(animInterpolationInd.names());

  animData_.animTranslationTable = createTableWidget();
  currentLayout->addWidget(animData_.animTranslationTable);

  endGroup();

  startGroup("Rotation");

  animData_.rotationInterpCombo = addLabelEdit("Interpolation", new QComboBox);
  animData_.rotationInterpCombo->addItems(animInterpolationInd.names());

  animData_.animRotationTable = createTableWidget();
  currentLayout->addWidget(animData_.animRotationTable);

  endGroup();

  startGroup("Scale");

  animData_.scaleInterpCombo = addLabelEdit("Interpolation", new QComboBox);
  animData_.scaleInterpCombo->addItems(animInterpolationInd.names());

  animData_.animScaleTable = createTableWidget();
  currentLayout->addWidget(animData_.animScaleTable);

  endGroup();

  //------

  connectSlots(true);

  updateWidgets();
}

void
CQCamera3DControl::
updateWidgets()
{
  connectSlots(false);

  auto *camera = getCamera();
  auto *canvas = app_->canvas();

  // General
  showWireframeCheck_->setChecked(canvas->isWireframe());
  showSolidCheck_    ->setChecked(canvas->isSolid());

  showNormalsCheck_->setChecked(canvas->isShowNormals());
  normalsSizeEdit_ ->setValue(canvas->normalsSize());
  normalsColorEdit_->setColor(canvas->normalsColor());

  depthTestCheck_->setChecked(canvas->isDepthTest());
  cullFaceCheck_ ->setChecked(canvas->isCullFace());
  frontFaceCheck_->setChecked(canvas->isFrontFace());

  // Camera
  cameraData_.showCheck   ->setChecked(canvas->isShowCamera());
  cameraData_.eyeLineCheck->setChecked(canvas->isShowEyeLine());
  cameraData_.planesCheck ->setChecked(canvas->isShowPlanes());

  cameraData_.pitchEdit->setValue(camera->pitch());
  cameraData_.yawEdit  ->setValue(camera->yaw());
  cameraData_.rollEdit ->setValue(camera->roll());

  cameraData_.nearEdit->setValue(camera->near());
  cameraData_.farEdit ->setValue(camera->far());

  cameraData_.xOriginEdit->setValue(camera->origin().getX());
  cameraData_.yOriginEdit->setValue(camera->origin().getY());
  cameraData_.zOriginEdit->setValue(camera->origin().getZ());

  cameraData_.xEdit->setValue(camera->position().getX());
  cameraData_.yEdit->setValue(camera->position().getY());
  cameraData_.zEdit->setValue(camera->position().getZ());

  cameraData_.fovEdit->setValue(camera->fov());

  cameraData_.eyeZ1Edit->setValue(canvas->eyeLineZ1());
  cameraData_.eyeZ2Edit->setValue(canvas->eyeLineZ2());

  // Lights
  lightsData_.lightsList->updateLights();

  lightsData_.ambientColorEdit   ->setColor(RGBAToQColor(canvas->ambientColor()));
  lightsData_.ambientStrengthEdit->setValue(canvas->ambientStrength());

  lightsData_.diffuseStrengthEdit->setValue(canvas->diffuseStrength());
  lightsData_.emissiveStrengthEdit->setValue(canvas->emissiveStrength());
  lightsData_.specularStrengthEdit->setValue(canvas->specularStrength());

  lightsData_.fixedDiffuseCheck->setChecked(canvas->isFixedDiffuse());

  auto *light = canvas->currentLight();

  lightsData_.typeCombo->setCurrentIndex(lightTypeInd.typeToInd(light->getType()));

  lightsData_.enabledCheck->setChecked(light->getEnabled());

  lightsData_.positionEdit->setValue(light->getPosition());

  lightsData_.colorEdit->setColor(RGBAToQColor(light->getDiffuse()));

  /// direction light
  lightsData_.directionEdit->setValue(light->getDirection().point());

  // point light
  lightsData_.pointRadiusEdit->setValue(light->getPointRadius());

  lightsData_.attenuation0Edit->setValue(light->getConstantAttenuation());
  lightsData_.attenuation1Edit->setValue(light->getLinearAttenuation());
  lightsData_.attenuation2Edit->setValue(light->getQuadraticAttenuation());

  // spot light
  lightsData_.spotDirectionEdit->setValue(light->getSpotDirection().point());
  lightsData_.spotExponentEdit ->setValue(light->getSpotExponent());
  lightsData_.spotCutOffAngle  ->setValue(light->getSpotCutOffAngle());

  // Axis
  auto *axes = canvas->axes();

  if (axes) {
    axisData_.showCheck->setChecked(axes->isVisible());

    axisData_.xPosEdit->setValue(axes->xpos());
    axisData_.yPosEdit->setValue(axes->ypos());
    axisData_.zPosEdit->setValue(axes->zpos());
  }

  // Mouse
  mouseTypeCombo_->setCurrentIndex(mouseTypeInd.typeToInd(canvas->mouseType()));

  mouseScaleEdit_->setValue(canvas->mouseScale());

  // Selection
  selectionData_.typeCombo->setCurrentIndex(selectTypeInd.typeToInd(canvas->selectType()));

  auto selectType = canvas->selectType();

  selectionData_.matrixEdit     ->setEnabled(selectType == CQCamera3DSelectType::OBJECT);
  selectionData_.translationEdit->setEnabled(selectType == CQCamera3DSelectType::OBJECT);
  selectionData_.rotationEdit   ->setEnabled(selectType == CQCamera3DSelectType::OBJECT);
  selectionData_.scaleEdit      ->setEnabled(selectType == CQCamera3DSelectType::OBJECT);

  selectionData_.colorEdit->setEnabled(false);

  selectionData_.indLabel->setText("");
  selectionData_.visCheck->setChecked(false);

  selectionData_.materialNameEdit->setText("");

  selectionData_.objectInfoText->setText("");

  if      (selectType == CQCamera3DSelectType::OBJECT) {
    auto *object = canvas->currentObject();

    if (object) {
      auto name = QString::fromStdString(object->getName());

      selectionData_.indLabel->setText(QString("%1 (#%2)").arg(name).arg(object->getInd()));

      selectionData_.visCheck->setChecked(object->getSelected());

      selectionData_.colorEdit->setEnabled(true);
      selectionData_.colorEdit->setColor(RGBAToQColor(object->getFaceColor()));

      selectionData_.matrixEdit->setValue(object->getTransform());

      selectionData_.translationEdit->setValue(object->translationValues());
      selectionData_.rotationEdit   ->setValue(object->rotationValuesDeg());
      selectionData_.scaleEdit      ->setValue(object->scaleValues());

      auto *diffuseTexture  = object->getDiffuseTexture();
      auto *normalTexture   = object->getNormalTexture();
      auto *specularTexture = object->getSpecularTexture();
      auto *emissiveTexture = object->getEmissiveTexture();

      selectionData_.diffuseTextureEdit->setTextureName(
        diffuseTexture ? QString::fromStdString(diffuseTexture->name()) : "");
      selectionData_.normalTextureEdit->setTextureName(
        normalTexture ? QString::fromStdString(normalTexture->name()) : "");
      selectionData_.specularTextureEdit->setTextureName(
        specularTexture ? QString::fromStdString(specularTexture->name()) : "");
      selectionData_.emissiveTextureEdit->setTextureName(
        emissiveTexture ? QString::fromStdString(emissiveTexture->name()) : "");

      auto *objectMaterial = object->getMaterialP();

      selectionData_.materialNameEdit->setText(objectMaterial ?
        QString::fromStdString(objectMaterial->name()) : "");

      //---

      QString objStr;

      const auto &faces = object->getFaces();

      objStr += QString("Faces: %1\n").arg(faces.size());

      uint nv = 0;
      uint nt = 0;

      for (const auto *face : faces) {
        const auto &vertices = face->getVertices();
        const auto &tpoints  = face->getTexturePoints();

        nv += vertices.size();
        nt += tpoints .size();
      }

      objStr += QString("Vertices: %1\n").arg(nv);
      objStr += QString("Texture Points: %1\n").arg(nt);

      selectionData_.objectInfoText->setText(objStr);
    }
  }
  else if (selectType == CQCamera3DSelectType::FACE) {
    auto *face = canvas->currentFace();

    if (face) {
      selectionData_.indLabel->setText(QString("%1").arg(face->getInd()));

      selectionData_.colorEdit->setEnabled(true);
      selectionData_.colorEdit->setColor(RGBAToQColor(face->getColor()));

      selectionData_.visCheck->setChecked(face->getSelected());

      auto *diffuseTexture  = face->getDiffuseTexture();
      auto *normalTexture   = face->getNormalTexture();
      auto *specularTexture = face->getSpecularTexture();
      auto *emissiveTexture = face->getEmissiveTexture();

      selectionData_.diffuseTextureEdit  ->setTextureName(
        diffuseTexture ? QString::fromStdString(diffuseTexture->name()) : "");
      selectionData_.normalTextureEdit->setTextureName(
        normalTexture ? QString::fromStdString(normalTexture->name()) : "");
      selectionData_.specularTextureEdit ->setTextureName(
        specularTexture ? QString::fromStdString(specularTexture->name()) : "");
      selectionData_.emissiveTextureEdit->setTextureName(
        emissiveTexture ? QString::fromStdString(emissiveTexture->name()) : "");

      auto *faceMaterial = face->getMaterialP();

      selectionData_.materialNameEdit->setText(faceMaterial ?
        QString::fromStdString(faceMaterial->name()) : "");

      //---

      QString objStr;

      const auto &vertices = face->getVertices();

      objStr += QString("Vertices: %1\n").arg(vertices.size());

      selectionData_.objectInfoText->setText(objStr);
    }
  }

  // Materials
  materialsData_.materialList->updateMaterials();

  // Overview
  auto *overview = app_->overview();

  overviewData_.equalScale->setChecked(overview->isEqualScale());

  overviewData_.selectTypeCombo->setCurrentIndex(selectTypeInd.typeToInd(overview->selectType()));
  overviewData_.mouseTypeCombo ->setCurrentIndex(mouseTypeInd .typeToInd(overview->mouseType ()));

  overviewData_.modelTypeCombo->setCurrentIndex(
    overviewModelTypeInd.typeToInd(overview->modelType()));

  overviewData_.cameraCheck->setChecked(overview->isCameraVisible());
  overviewData_.lightsCheck->setChecked(overview->isLightsVisible());

  // Bones
  auto *bones = app_->bones();

  bonesData_.modelCheck->setChecked(bones->isShowModel());

  // Animation
  animData_.timeEdit->setRange(animData_.animCombo->tmin(), animData_.animCombo->tmax());

  connectSlots(true);
}

void
CQCamera3DControl::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, app_, SIGNAL(timerStep()),
                            this, SLOT(timerSlot()));

  CQUtil::connectDisconnect(b, app_, SIGNAL(animNameChanged()),
                            this, SLOT(updateBones()));
  CQUtil::connectDisconnect(b, app_, SIGNAL(animTimeChanged()),
                            this, SLOT(updateBones()));
  CQUtil::connectDisconnect(b, app_, SIGNAL(boneNodeChanged()),
                            this, SLOT(updateAnim()));

  CQUtil::connectDisconnect(b, mainTab_, SIGNAL(currentChanged(int)),
                            this, SLOT(mainTabSlot(int)));

  auto *canvas = app_->canvas();

  CQUtil::connectDisconnect(b, canvas, SIGNAL(stateChanged()),
                            this, SLOT(updateWidgets()));

  auto connectCheckBox = [&](QCheckBox *w, const char *slotName) {
    CQUtil::connectDisconnect(b, w, SIGNAL(stateChanged(int)), this, slotName);
  };

  auto connectRealSpin = [&](CQRealSpin *w, const char *slotName) {
    CQUtil::connectDisconnect(b, w, SIGNAL(realValueChanged(double)), this, slotName);
  };

  auto connectToolButton = [&](QToolButton *w, const char *slotName) {
    CQUtil::connectDisconnect(b, w, SIGNAL(clicked()), this, slotName);
  };

  auto connectComboBox = [&](QComboBox *w, const char *slotName) {
    CQUtil::connectDisconnect(b, w, SIGNAL(currentIndexChanged(int)), this, slotName);
  };

  auto connectPointEdit = [&](CQPoint3DEdit *w, const char *slotName) {
    CQUtil::connectDisconnect(b, w, SIGNAL(editingFinished()), this, slotName);
  };

  auto connectColorEdit = [&](CQColorEdit *w, const char *slotName) {
    CQUtil::connectDisconnect(b, w, SIGNAL(colorChanged(const QColor &)), this, slotName);
  };

  auto connectTextureEdit = [&](CQCamera3DTextureChooser *w, const char *slotName) {
    CQUtil::connectDisconnect(b, w, SIGNAL(textureChanged()), this, slotName);
  };

  // General
  connectCheckBox(showWireframeCheck_, SLOT(showWireframeSlot(int)));
  connectCheckBox(showSolidCheck_    , SLOT(showSolidSlot(int)));

  connectCheckBox (showNormalsCheck_, SLOT(showNormalsSlot(int)));
  connectRealSpin (normalsSizeEdit_ , SLOT(normalsSizeSlot(double)));
  connectColorEdit(normalsColorEdit_, SLOT(normalsColorSlot(const QColor &)));

  connectCheckBox(depthTestCheck_, SLOT(depthTestSlot(int)));
  connectCheckBox(cullFaceCheck_ , SLOT(cullSlot(int)));
  connectCheckBox(frontFaceCheck_, SLOT(frontFaceSlot(int)));

  // Camera
  connectCheckBox(cameraData_.altCheck, SLOT(altSlot(int)));

  connectCheckBox(cameraData_.showCheck   , SLOT(showCameraSlot(int)));
  connectCheckBox(cameraData_.eyeLineCheck, SLOT(showEyeLineSlot(int)));
  connectCheckBox(cameraData_.planesCheck , SLOT(showPlanesSlot(int)));

  connectRealSpin(cameraData_.pitchEdit, SLOT(pitchSlot(double)));
  connectRealSpin(cameraData_.yawEdit  , SLOT(yawSlot(double)));
  connectRealSpin(cameraData_.rollEdit , SLOT(rollSlot(double)));

  connectRealSpin(cameraData_.nearEdit, SLOT(nearSlot(double)));
  connectRealSpin(cameraData_.farEdit , SLOT(farSlot(double)));

  connectRealSpin(cameraData_.xOriginEdit, SLOT(xOriginSlot(double)));
  connectRealSpin(cameraData_.yOriginEdit, SLOT(yOriginSlot(double)));
  connectRealSpin(cameraData_.zOriginEdit, SLOT(zOriginSlot(double)));

  connectRealSpin(cameraData_.xEdit, SLOT(xSlot(double)));
  connectRealSpin(cameraData_.yEdit, SLOT(ySlot(double)));
  connectRealSpin(cameraData_.zEdit, SLOT(zSlot(double)));

  connectRealSpin(cameraData_.fovEdit  , SLOT(fovSlot(double)));
  connectRealSpin(cameraData_.eyeZ1Edit, SLOT(eyeZ1Slot(double)));
  connectRealSpin(cameraData_.eyeZ2Edit, SLOT(eyeZ2Slot(double)));

  connectToolButton(cameraData_.aroundXButtons.minusButton, SLOT(aroundXSlot1()));
  connectToolButton(cameraData_.aroundXButtons.plusButton , SLOT(aroundXSlot2()));
  connectToolButton(cameraData_.aroundYButtons.minusButton, SLOT(aroundYSlot1()));
  connectToolButton(cameraData_.aroundYButtons.plusButton , SLOT(aroundYSlot2()));
  connectToolButton(cameraData_.aroundZButtons.minusButton, SLOT(aroundZSlot1()));
  connectToolButton(cameraData_.aroundZButtons.plusButton , SLOT(aroundZSlot2()));

  // Lights
  CQUtil::connectDisconnect(b, lightsData_.lightsList, SIGNAL(currentItemChanged()),
                            this, SLOT(currentLightSlot()));

  connectColorEdit(lightsData_.ambientColorEdit   , SLOT(ambientColorSlot(const QColor &)));
  connectRealSpin (lightsData_.ambientStrengthEdit, SLOT(ambientStrengthSlot(double)));

  connectRealSpin(lightsData_.diffuseStrengthEdit , SLOT(diffuseStrengthSlot(double)));
  connectRealSpin(lightsData_.emissiveStrengthEdit, SLOT(emissiveStrengthSlot(double)));
  connectRealSpin(lightsData_.specularStrengthEdit, SLOT(specularStrengthSlot(double)));

  connectCheckBox(lightsData_.fixedDiffuseCheck, SLOT(fixedDiffuseSlot(int)));

  connectComboBox (lightsData_.typeCombo        , SLOT(lightTypeSlot(int)));
  connectCheckBox (lightsData_.enabledCheck     , SLOT(lightEnabledSlot(int)));
  connectPointEdit(lightsData_.positionEdit     , SLOT(lightPositionSlot()));
  connectColorEdit(lightsData_.colorEdit        , SLOT(lightColorSlot(const QColor &)));
  connectPointEdit(lightsData_.directionEdit    , SLOT(lightDirectionSlot()));
  connectRealSpin (lightsData_.pointRadiusEdit  , SLOT(lightPointRadiusSlot(double)));
  connectRealSpin (lightsData_.attenuation0Edit , SLOT(lightConstantAttenuationSlot(double)));
  connectRealSpin (lightsData_.attenuation1Edit , SLOT(lightLinearAttenuationSlot(double)));
  connectRealSpin (lightsData_.attenuation2Edit , SLOT(lightQuadraticAttenuationSlot(double)));
  connectPointEdit(lightsData_.spotDirectionEdit, SLOT(lightSpotDirectionSlot()));
  connectRealSpin (lightsData_.spotExponentEdit , SLOT(lightSpotExponentSlot(double)));
  connectRealSpin (lightsData_.spotCutOffAngle  , SLOT(lightCutOffAngleSlot(double)));

  // Axis
  connectCheckBox(axisData_.showCheck, SLOT(showAxesSlot(int)));
  connectRealSpin(axisData_.xPosEdit , SLOT(axesXPosSlot(double)));
  connectRealSpin(axisData_.yPosEdit , SLOT(axesYPosSlot(double)));
  connectRealSpin(axisData_.zPosEdit , SLOT(axesZPosSlot(double)));

  // Mouse
  connectComboBox(mouseTypeCombo_, SLOT(mouseTypeSlot(int)));
  connectRealSpin(mouseScaleEdit_, SLOT(mouseScaleSlot(double)));

  // Selection
  connectComboBox(selectionData_.typeCombo, SLOT(selectTypeSlot(int)));
  connectCheckBox(selectionData_.visCheck , SLOT(objectVisSlot(int)));

#if 0
  CQUtil::connectDisconnect(b, selectionData_.matrixEdit, SIGNAL(valueChanged()),
                            this, SLOT(objectMatrixSlot()));
#else
  connectPointEdit(selectionData_.translationEdit, SLOT(translationSlot()));
  connectPointEdit(selectionData_.rotationEdit   , SLOT(rotationSlot()));
  connectPointEdit(selectionData_.scaleEdit      , SLOT(scaleSlot()));
#endif

  connectColorEdit(selectionData_.colorEdit, SLOT(objectColorSlot(const QColor &)));

  connectTextureEdit(selectionData_.diffuseTextureEdit , SLOT(diffuseTextureSlot()));
  connectTextureEdit(selectionData_.normalTextureEdit  , SLOT(normalTextureSlot()));
  connectTextureEdit(selectionData_.specularTextureEdit, SLOT(specularTextureSlot()));
  connectTextureEdit(selectionData_.emissiveTextureEdit, SLOT(emissiveTextureSlot()));

  // Material
  CQUtil::connectDisconnect(b, materialsData_.materialList, SIGNAL(currentItemChanged()),
                            this, SLOT(currentMaterialSlot()));

  connectColorEdit(materialsData_.ambientEdit , SLOT(materialAmbientSlot(const QColor &)));
  connectColorEdit(materialsData_.diffuseEdit , SLOT(materialDiffuseSlot(const QColor &)));
  connectColorEdit(materialsData_.specularEdit, SLOT(materialSpecularSlot(const QColor &)));
  connectColorEdit(materialsData_.emissionEdit, SLOT(materialEmissionSlot(const QColor &)));

  connectRealSpin(materialsData_.shininessEdit   , SLOT(materialShininessSlot(double)));
  connectRealSpin(materialsData_.transparencyEdit, SLOT(materialTransparencySlot(double)));

  connectTextureEdit(materialsData_.ambientTextureEdit , SLOT(materialAmbientTextureSlot()));
  connectTextureEdit(materialsData_.diffuseTextureEdit , SLOT(materialDiffuseTextureSlot()));
  connectTextureEdit(materialsData_.normalTextureEdit  , SLOT(materialNormalTextureSlot()));
  connectTextureEdit(materialsData_.specularTextureEdit, SLOT(materialSpecularTextureSlot()));
  connectTextureEdit(materialsData_.emissiveTextureEdit, SLOT(materialEmissiveTextureSlot()));

  // Objects
  CQUtil::connectDisconnect(b, canvas, SIGNAL(objectsChanged()),
                            objectsList_, SLOT(updateObjects()));

  // Overview
  connectCheckBox(overviewData_.equalScale, SLOT(overviewEqualScaleSlot(int)));

  connectComboBox(overviewData_.selectTypeCombo, SLOT(overviewSelectTypeSlot(int)));
  connectComboBox(overviewData_.mouseTypeCombo , SLOT(overviewMouseTypeSlot(int)));

  connectComboBox(overviewData_.modelTypeCombo, SLOT(overviewModelTypeSlot(int)));

  connectCheckBox(overviewData_.cameraCheck, SLOT(overviewCameraSlot(int)));
  connectCheckBox(overviewData_.lightsCheck, SLOT(overviewLightsSlot(int)));

  // UV
  connectComboBox(uvData_.typeCombo, SLOT(uvTextureTypeSlot(int)));

  // Bones
  connectCheckBox(bonesData_.modelCheck, SLOT(bonesModelSlot(int)));

  CQUtil::connectDisconnect(b, canvas, SIGNAL(objectsChanged()),
                            bonesData_.bonesList, SLOT(updateWidgets()));
  CQUtil::connectDisconnect(b, bonesData_.bonesList, SIGNAL(currentItemChanged()),
                            this, SLOT(currentBoneSlot()));

  // Animation
  CQUtil::connectDisconnect(b, animData_.animCombo, SIGNAL(animChanged()),
                            this, SLOT(animNameSlot()));
  CQUtil::connectDisconnect(b, animData_.timeEdit, SIGNAL(realValueChanged(double)),
                            this, SLOT(animTimeSlot(double)));
}

void
CQCamera3DControl::
mainTabSlot(int i)
{
  connectSlots(false);

  app_->setCurrentView(i);

  connectSlots(true);
}

void
CQCamera3DControl::
setCurrentControl(int i)
{
  connectSlots(false);

  mainTab_->setCurrentIndex(i);

  connectSlots(true);
}

void
CQCamera3DControl::
showWireframeSlot(int i)
{
  auto *canvas = app_->canvas();

  canvas->setWireframe(i);

  canvas->update();
}

void
CQCamera3DControl::
showSolidSlot(int i)
{
  auto *canvas = app_->canvas();

  canvas->setSolid(i);

  canvas->update();
}

void
CQCamera3DControl::
showNormalsSlot(int i)
{
  auto *canvas = app_->canvas();

  canvas->setShowNormals(i);

  canvas->update();
}

void
CQCamera3DControl::
normalsSizeSlot(double s)
{
  auto *canvas  = app_->canvas();
  auto *normals = canvas->getNormals();
  if (! normals) return;

  canvas ->setNormalsSize(s);
  normals->setLineSize(s);

  updateObjects();
}

void
CQCamera3DControl::
normalsColorSlot(const QColor &c)
{
  auto *canvas  = app_->canvas();
  auto *normals = canvas->getNormals();
  if (! normals) return;

  canvas ->setNormalsColor(c);
  normals->setLineColor(c);

  updateObjects();
}

void
CQCamera3DControl::
depthTestSlot(int i)
{
  auto *canvas = app_->canvas();

  canvas->setDepthTest(i);

  canvas->update();
}

void
CQCamera3DControl::
cullSlot(int i)
{
  auto *canvas = app_->canvas();

  canvas->setCullFace(i);

  canvas->update();
}

void
CQCamera3DControl::
frontFaceSlot(int i)
{
  auto *canvas = app_->canvas();

  canvas->setFrontFace(i);

  canvas->update();
}

void
CQCamera3DControl::
mouseTypeSlot(int i)
{
  auto *canvas = app_->canvas();

  canvas->setMouseType(mouseTypeInd.indToType(i));

  updateWidgets();
}

void
CQCamera3DControl::
mouseScaleSlot(double r)
{
  auto *canvas = app_->canvas();

  canvas->setMouseScale(r);

  updateWidgets();
}

void
CQCamera3DControl::
selectTypeSlot(int i)
{
  auto *canvas = app_->canvas();

  canvas->setSelectType(selectTypeInd.indToType(i));

  updateWidgets();
}

void
CQCamera3DControl::
objectVisSlot(int i)
{
  auto *canvas = app_->canvas();

  auto selectType = canvas->selectType();

  if      (selectType == CQCamera3DSelectType::OBJECT) {
    auto *object = canvas->currentObject();

    if (object)
      object->setVisible(i);
  }
  else if (selectType == CQCamera3DSelectType::FACE) {
    auto *face = canvas->currentFace();

    if (face)
      face->setVisible(i);
  }

  updateObjects();
}

void
CQCamera3DControl::
altSlot(int i)
{
  auto *canvas = app_->canvas();

  canvas->setCameraInd(i ? 1 : 0);
  canvas->update();

  updateWidgets();
}

void
CQCamera3DControl::
showCameraSlot(int i)
{
  auto *canvas = app_->canvas();

  canvas->setShowCamera(i);

  canvas->updateShapes();
  canvas->update();
}

void
CQCamera3DControl::
showEyeLineSlot(int i)
{
  auto *canvas = app_->canvas();

  canvas->setShowEyeLine(i);

  canvas->updateShapes();
  canvas->update();
}

void
CQCamera3DControl::
showPlanesSlot(int i)
{
  auto *canvas = app_->canvas();

  canvas->setShowPlanes(i);

  canvas->updateShapes();
  canvas->update();
}

void
CQCamera3DControl::
pitchSlot(double r)
{
  auto *camera = getCamera();

  camera->setPitch(r);
}

void
CQCamera3DControl::
yawSlot(double r)
{
  auto *camera = getCamera();

  camera->setYaw(r);
}

void
CQCamera3DControl::
rollSlot(double r)
{
  auto *camera = getCamera();

  camera->setRoll(r);
}

void
CQCamera3DControl::
nearSlot(double r)
{
  auto *camera = getCamera();

  camera->setNear(r);
}

void
CQCamera3DControl::
farSlot(double r)
{
  auto *camera = getCamera();

  camera->setFar(r);
}

void
CQCamera3DControl::
xOriginSlot(double r)
{
  auto *camera = getCamera();

  auto p = camera->origin();

  p.setX(r);
  camera->setOrigin(p);
}

void
CQCamera3DControl::
yOriginSlot(double r)
{
  auto *camera = getCamera();

  auto p = camera->origin();

  p.setY(r);
  camera->setOrigin(p);
}

void
CQCamera3DControl::
zOriginSlot(double r)
{
  auto *camera = getCamera();

  auto p = camera->origin();

  p.setZ(r);
  camera->setOrigin(p);
}

void
CQCamera3DControl::
xSlot(double r)
{
  auto *camera = getCamera();

  auto p = camera->position();

  p.setX(r);
  camera->setPosition(p);
}

void
CQCamera3DControl::
ySlot(double r)
{
  auto *camera = getCamera();

  auto p = camera->position();

  p.setY(r);
  camera->setPosition(p);
}

void
CQCamera3DControl::
zSlot(double r)
{
  auto *camera = getCamera();

  auto p = camera->position();

  p.setZ(r);
  camera->setPosition(p);
}

void
CQCamera3DControl::
fovSlot(double r)
{
  auto *camera = getCamera();

  camera->setFov(r);
}

void
CQCamera3DControl::
eyeZ1Slot(double r)
{
  auto *canvas = app_->canvas();

  canvas->setEyeLineZ1(r);
}

void
CQCamera3DControl::
eyeZ2Slot(double r)
{
  auto *canvas = app_->canvas();

  canvas->setEyeLineZ2(r);
}

void
CQCamera3DControl::
aroundXSlot1()
{
  auto *canvas = app_->canvas();
  auto *camera = getCamera();

  auto bbox = canvas->bbox();

  auto d = bbox.getMaxSize()/100.0;

  camera->moveAroundX(-3*d);
}

void
CQCamera3DControl::
aroundXSlot2()
{
  auto *canvas = app_->canvas();
  auto *camera = getCamera();

  auto bbox = canvas->bbox();

  auto d = bbox.getMaxSize()/100.0;

  camera->moveAroundX(3*d);
}

void
CQCamera3DControl::
aroundYSlot1()
{
  auto *canvas = app_->canvas();
  auto *camera = getCamera();

  auto bbox = canvas->bbox();

  auto d = bbox.getMaxSize()/100.0;

  camera->moveAroundY(-3*d);
}

void
CQCamera3DControl::
aroundYSlot2()
{
  auto *canvas = app_->canvas();
  auto *camera = getCamera();

  auto bbox = canvas->bbox();

  auto d = bbox.getMaxSize()/100.0;

  camera->moveAroundY(3*d);
}

void
CQCamera3DControl::
aroundZSlot1()
{
  auto *canvas = app_->canvas();
  auto *camera = getCamera();

  auto bbox = canvas->bbox();

  auto d = bbox.getMaxSize()/100.0;

  camera->moveAroundZ(-3*d);
}

void
CQCamera3DControl::
aroundZSlot2()
{
  auto *canvas = app_->canvas();
  auto *camera = getCamera();

  auto bbox = canvas->bbox();

  auto d = bbox.getMaxSize()/100.0;

  camera->moveAroundZ(3*d);
}

#if 0
void
CQCamera3DControl::
objectMatrixSlot()
{
  auto *canvas = app_->canvas();
  auto *object = canvas->currentObject();

  if (object) {
    object->setTransform(objectMatrix_->getValue());

    updateObjects();
  }
}
#else
void
CQCamera3DControl::
translationSlot()
{
  auto *canvas = app_->canvas();
  auto *object = canvas->currentObject();

  if (object) {
    auto p = selectionData_.translationEdit->getValue();

    object->setTranslationValues(p);

    updateObjects();
  }
}

void
CQCamera3DControl::
rotationSlot()
{
  auto *canvas = app_->canvas();
  auto *object = canvas->currentObject();

  if (object) {
    auto p = selectionData_.rotationEdit->getValue();

    object->setRotationValuesDeg(p);

    updateObjects();
  }
}

void
CQCamera3DControl::
scaleSlot()
{
  auto *canvas = app_->canvas();
  auto *object = canvas->currentObject();

  if (object) {
    auto p = selectionData_.scaleEdit->getValue();

    object->setScaleValues(p);

    updateObjects();
  }
}
#endif

void
CQCamera3DControl::
objectColorSlot(const QColor &c)
{
  auto *canvas = app_->canvas();

  auto selectType = canvas->selectType();

  if      (selectType == CQCamera3DSelectType::OBJECT) {
    auto *object = canvas->currentObject();

    if (object)
      object->setFaceColor(QColorToRGBA(c));
  }
  else if (selectType == CQCamera3DSelectType::FACE) {
    auto *face = canvas->currentFace();

    if (face)
      face->setColor(QColorToRGBA(c));
  }

  updateObjects();
}

void
CQCamera3DControl::
diffuseTextureSlot()
{
  auto *canvas = app_->canvas();
  auto *edit = qobject_cast<CQCamera3DTextureChooser *>(sender());

  auto textureName = edit->textureName();
  auto *texture = canvas->getGeomTextureByName(textureName.toStdString());

  auto *object = canvas->currentObject();
  auto *face   = canvas->currentFace();

  if (object)
    object->setDiffuseTexture(texture);
  else if (face)
    face->setDiffuseTexture(texture);

  updateObjects();
}

void
CQCamera3DControl::
normalTextureSlot()
{
  auto *canvas = app_->canvas();
  auto *edit = qobject_cast<CQCamera3DTextureChooser *>(sender());

  auto textureName = edit->textureName();
  auto *texture = canvas->getGeomTextureByName(textureName.toStdString());

  auto *object = canvas->currentObject();
  auto *face   = canvas->currentFace();

  if      (object)
    object->setNormalTexture(texture);
  else if (face)
    face->setNormalTexture(texture);

  updateObjects();
}

void
CQCamera3DControl::
specularTextureSlot()
{
  auto *canvas = app_->canvas();
  auto *edit = qobject_cast<CQCamera3DTextureChooser *>(sender());

  auto textureName = edit->textureName();
  auto *texture = canvas->getGeomTextureByName(textureName.toStdString());

  auto *object = canvas->currentObject();
  auto *face   = canvas->currentFace();

  if      (object)
    object->setSpecularTexture(texture);
  else if (face)
    face->setSpecularTexture(texture);

  updateObjects();
}

void
CQCamera3DControl::
emissiveTextureSlot()
{
  auto *canvas = app_->canvas();
  auto *edit = qobject_cast<CQCamera3DTextureChooser *>(sender());

  auto textureName = edit->textureName();
  auto *texture = canvas->getGeomTextureByName(textureName.toStdString());

  auto *object = canvas->currentObject();
  auto *face   = canvas->currentFace();

  if      (object)
    object->setEmissiveTexture(texture);
  else if (face)
    face->setEmissiveTexture(texture);

  updateObjects();
}

void
CQCamera3DControl::
resetSlot()
{
  auto *camera = getCamera();
  auto *canvas = app_->canvas();

  auto bbox = canvas->bbox();

  auto center = bbox.getCenter();
  auto size   = bbox.getMaxSize();

  auto s2 = std::sqrt(2.0);

  auto origin = CVector3D(center.x, center.y, center.z);
  auto pos    = CVector3D(center.x, center.y, center.z + s2*size);

  camera->setPosition(pos);

  camera->setPitch(0.0);
  camera->setYaw(-M_PI/2.0);
  camera->setRoll(0.0);

  camera->setOrigin(origin);
}

void
CQCamera3DControl::
topSlot()
{
  auto *camera = getCamera();
  auto *canvas = app_->canvas();

  auto bbox = canvas->bbox();

  auto center = bbox.getCenter();
  auto size   = bbox.getMaxSize();

  auto origin = CVector3D(center.x, center.y, center.z);
  auto pos    = CVector3D(center.x, center.y + 2*size, center.z);

  camera->setPosition(pos);
  camera->setOrigin(origin);

  camera->setPitch(-M_PI/2.0);
  camera->setYaw(M_PI/2.0);
  camera->setRoll(0.0);
}

void
CQCamera3DControl::
sideSlot()
{
  auto *camera = getCamera();
  auto *canvas = app_->canvas();

  auto bbox = canvas->bbox();

  auto center = bbox.getCenter();
  auto size   = bbox.getMaxSize();

  auto origin = CVector3D(center.x, center.y, center.z);
  auto pos    = CVector3D(center.x, center.y, center.z + 2*size);

  camera->setPitch(0.0);
  camera->setYaw(M_PI/2.0);
  camera->setRoll(0.0);

  camera->setPosition(pos);
  camera->setOrigin(origin);
}

void
CQCamera3DControl::
frontSlot()
{
  auto *camera = getCamera();
  auto *canvas = app_->canvas();

  auto bbox = canvas->bbox();

  auto center = bbox.getCenter();
  auto size   = bbox.getMaxSize();

  auto origin = CVector3D(center.x, center.y, center.z);
  auto pos    = CVector3D(center.x - 2*size, center.y, center.z);

  camera->setPitch(0.0);
  camera->setYaw(0.0);
  camera->setRoll(0.0);

  camera->setPosition(pos);
  camera->setOrigin(origin);
}

CQCamera3DCamera *
CQCamera3DControl::
getCamera() const
{
  auto *canvas = app_->canvas();

  return canvas->currentCamera();
}

void
CQCamera3DControl::
currentLightSlot()
{
  auto *light = lightsData_.lightsList->getCurrentLight();
  if (! light) return;

  auto *canvas = app_->canvas();

  canvas->setCurrentLight(light);

  updateWidgets();
}

void
CQCamera3DControl::
ambientColorSlot(const QColor &c)
{
  auto *canvas = app_->canvas();

  canvas->setAmbientColor(QColorToRGBA(c));

  canvas->update();
}

void
CQCamera3DControl::
ambientStrengthSlot(double r)
{
  auto *canvas = app_->canvas();

  canvas->setAmbientStrength(r);

  canvas->update();
}

void
CQCamera3DControl::
diffuseStrengthSlot(double r)
{
  auto *canvas = app_->canvas();

  canvas->setDiffuseStrength(r);

  canvas->update();
}

void
CQCamera3DControl::
emissiveStrengthSlot(double r)
{
  auto *canvas = app_->canvas();

  canvas->setEmissiveStrength(r);

  canvas->update();
}

void
CQCamera3DControl::
specularStrengthSlot(double r)
{
  auto *canvas = app_->canvas();

  canvas->setSpecularStrength(r);

  canvas->update();
}

void
CQCamera3DControl::
fixedDiffuseSlot(int i)
{
  auto *canvas = app_->canvas();

  canvas->setFixedDiffuse(i);

  canvas->update();
}

void
CQCamera3DControl::
lightTypeSlot(int ind)
{
  auto *canvas = app_->canvas();
  auto *light  = canvas->currentLight();

  light->setType(lightTypeInd.indToType(ind));

  canvas->update();
}

void
CQCamera3DControl::
lightEnabledSlot(int ind)
{
  auto *canvas = app_->canvas();
  auto *light  = canvas->currentLight();

  light->setEnabled(ind);

  canvas->update();
}

void
CQCamera3DControl::
lightPositionSlot()
{
  auto *canvas = app_->canvas();
  auto *light  = canvas->currentLight();

  auto p = lightsData_.positionEdit->getValue();

  light->setPosition(p);

  canvas->update();
}

void
CQCamera3DControl::
lightColorSlot(const QColor &c)
{
  auto *canvas = app_->canvas();
  auto *light  = canvas->currentLight();

  light->setDiffuse(QColorToRGBA(c));

  canvas->update();
}

void
CQCamera3DControl::
lightDirectionSlot()
{
  auto *canvas = app_->canvas();
  auto *light  = canvas->currentLight();

  auto p = lightsData_.directionEdit->getValue();

  light->setDirection(CVector3D(p));

  canvas->update();
}

void
CQCamera3DControl::
lightPointRadiusSlot(double r)
{
  auto *canvas = app_->canvas();
  auto *light  = canvas->currentLight();

  light->setPointRadius(r);

  canvas->update();
}

void
CQCamera3DControl::
lightConstantAttenuationSlot(double r)
{
  auto *canvas = app_->canvas();
  auto *light  = canvas->currentLight();

  light->setConstantAttenuation(r);

  canvas->update();
}

void
CQCamera3DControl::
lightLinearAttenuationSlot(double r)
{
  auto *canvas = app_->canvas();
  auto *light  = canvas->currentLight();

  light->setLinearAttenuation(r);

  canvas->update();
}

void
CQCamera3DControl::
lightQuadraticAttenuationSlot(double r)
{
  auto *canvas = app_->canvas();
  auto *light  = canvas->currentLight();

  light->setQuadraticAttenuation(r);

  canvas->update();
}

void
CQCamera3DControl::
lightSpotDirectionSlot()
{
  auto *canvas = app_->canvas();
  auto *light  = canvas->currentLight();

  auto p = lightsData_.spotDirectionEdit->getValue();

  light->setSpotDirection(CVector3D(p));

  canvas->update();
}

void
CQCamera3DControl::
lightSpotExponentSlot(double r)
{
  auto *canvas = app_->canvas();
  auto *light  = canvas->currentLight();

  light->setSpotExponent(r);

  canvas->update();
}

void
CQCamera3DControl::
lightCutOffAngleSlot(double r)
{
  auto *canvas = app_->canvas();
  auto *light  = canvas->currentLight();

  light->setSpotCutOffAngle(r);

  canvas->update();
}

void
CQCamera3DControl::
addLightSlot()
{
  auto *canvas = app_->canvas();

  canvas->addLight();

  canvas->update();
}

void
CQCamera3DControl::
resetLightSlot()
{
  auto *canvas = app_->canvas();
  auto *light  = canvas->currentLight();

  canvas->resetLight(light);

  canvas->update();

  updateWidgets();
}

void
CQCamera3DControl::
showAxesSlot(int i)
{
  auto *canvas = app_->canvas();
  auto *axes   = canvas->axes();

  if (axes)
    axes->setVisible(i);

  canvas->update();
}

void
CQCamera3DControl::
axesXPosSlot(double r)
{
  auto *canvas = app_->canvas();
  auto *axes   = canvas->axes();

  if (axes)
    axes->setXPos(r);

  canvas->update();
}

void
CQCamera3DControl::
axesYPosSlot(double r)
{
  auto *canvas = app_->canvas();
  auto *axes   = canvas->axes();

  if (axes)
    axes->setYPos(r);

  canvas->update();
}

void
CQCamera3DControl::
axesZPosSlot(double r)
{
  auto *canvas = app_->canvas();
  auto *axes   = canvas->axes();

  if (axes)
    axes->setZPos(r);

  canvas->update();
}

void
CQCamera3DControl::
swapSlot()
{
  auto name = qobject_cast<QPushButton *>(sender())->text();

  auto *canvas = app_->canvas();
  auto *object = canvas->currentObject(/*includeRoot*/true);
  if (! object) return;

  if      (name == "Swap XY")
    object->swapXY();
  else if (name == "Swap YZ")
    object->swapYZ();
  else if (name == "Swap ZX")
    object->swapYZ();

  updateObjects();
}

void
CQCamera3DControl::
invertSlot()
{
  auto name = qobject_cast<QPushButton *>(sender())->text();

  auto *canvas = app_->canvas();
  auto *object = canvas->currentObject(/*includeRoot*/true);
  if (! object) return;

  if      (name == "Invert X")
    object->invertX();
  else if (name == "Invert Y")
    object->invertY();
  else if (name == "Invert Z")
    object->invertZ();

  updateObjects();
}

void
CQCamera3DControl::
addTextureSlot()
{
  auto dir = QDir::current().dirName();

  auto fileName = QFileDialog::getOpenFileName(this, "Open Texture", dir, "Files (*.*)");
  if (! fileName.length()) return;

  auto *canvas = app_->canvas();

  canvas->addTextureFile(fileName.toStdString());
}

void
CQCamera3DControl::
loadTextureMapSlot()
{
  auto dir = QDir::current().dirName();

  auto fileName = QFileDialog::getOpenFileName(this, "Load Texture Map", dir, "Files (*.*)");
  if (! fileName.length()) return;

  auto *canvas = app_->canvas();

  canvas->loadTextureMap(fileName.toStdString());
}

void
CQCamera3DControl::
saveTextureMapSlot()
{
  auto *scene = app_->getScene();

  auto printTexture = [](const CGeomTexture *texture) {
    std::cerr << " " << (texture ? texture->name() : "none");
  };

  for (auto *object : scene->getObjects()) {
    auto name = object->getName();

    std::cout << name;

    printTexture(object->getDiffuseTexture ());
    printTexture(object->getNormalTexture  ());
    printTexture(object->getSpecularTexture());
    printTexture(object->getEmissiveTexture());

    std::cout << "\n";
  }
}

void
CQCamera3DControl::
loadMaterialMapSlot()
{
  auto dir = QDir::current().dirName();

  auto fileName = QFileDialog::getOpenFileName(this, "Load Material Map", dir, "Files (*.*)");
  if (! fileName.length()) return;

  auto *canvas = app_->canvas();

  canvas->loadMaterialMap(fileName.toStdString());
}

void
CQCamera3DControl::
saveMaterialMapSlot()
{
  auto *scene = app_->getScene();

  auto printTexture = [](const CGeomTexture *texture) {
    std::cerr << " " << (texture ? texture->name() : "none");
  };

  for (auto *material : scene->getMaterials()) {
    auto name = material->name();

    std::cout << name;

    printTexture(material->diffuseTexture ());
    printTexture(material->normalTexture  ());
    printTexture(material->specularTexture());
    printTexture(material->emissiveTexture());

    std::cout << "\n";
  }
}

void
CQCamera3DControl::
selectParentSlot()
{
  auto *canvas = app_->canvas();
  auto *object = canvas->currentObject();
  if (! object) return;

  if (object->parent())
    canvas->selectObject(object->parent());
}

void
CQCamera3DControl::
selectRelatedSlot()
{
  auto *canvas = app_->canvas();
  auto *face = canvas->currentFace();
  if (! face) return;

  auto *object = face->getObject();

  std::vector<CGeomFace3D*> faces;

  auto *faceMaterial = face->getMaterialP();

  if (faceMaterial)
    faces = object->getMaterialFaces(faceMaterial);

  canvas->selectFaces(faces);
}

void
CQCamera3DControl::
deselectSlot()
{
  auto *canvas = app_->canvas();

  canvas->deselectAll();
}

void
CQCamera3DControl::
objectSelectSlot()
{
  auto *canvas = app_->canvas();

  auto *object = objectsList_->getObjectListSelected();
  if (! object) return;

  canvas->selectObject(object);
}

void
CQCamera3DControl::
objectZoomSlot()
{
  auto *canvas = app_->canvas();
  auto *object = canvas->currentObject();
  if (! object) return;

  auto *objectData = canvas->getObjectData(object);
  if (! objectData) return;

  auto bbox = objectData->bbox();

  auto *camera = getCamera();

  auto center = bbox.getCenter();
  auto size   = bbox.getMaxSize();

  auto s2 = std::sqrt(2.0);

  auto origin = CVector3D(center.x, center.y, center.z);
  auto pos    = CVector3D(center.x, center.y, center.z + camera->near() + s2*size);

  camera->setPosition(pos);

  camera->setPitch(0.0);
  camera->setYaw(-M_PI/2.0);
  camera->setRoll(0.0);

  camera->setOrigin(origin);
}

void
CQCamera3DControl::
addCubeSlot()
{
  auto *scene = app_->getScene();

  std::string name = "cube";

  auto *cube = CGeometryInst->createObject3D(scene, name);

  CGeomCube3D::addGeometry(cube, CPoint3D(0, 0, 0), 0.1);

  scene->addObject(cube);

  updateObjects();
}

void
CQCamera3DControl::
addCylinderSlot()
{
  auto *scene = app_->getScene();

  std::string name = "cylinder";

  auto *cylinder = CGeometryInst->createObject3D(scene, name);

  CGeomCylinder3D::addGeometry(cylinder, CPoint3D(0, 0, 0), 0.1, 1.0);

  scene->addObject(cylinder);

  updateObjects();
}

void
CQCamera3DControl::
addPyramidSlot()
{
  auto *scene = app_->getScene();

  std::string name = "pyramid";

  auto *pyramid = CGeometryInst->createObject3D(scene, name);

  CGeomPyramid3D::addGeometry(pyramid, CPoint3D(0, 0, 0), 0.1, 0.1);

  scene->addObject(pyramid);

  updateObjects();
}

void
CQCamera3DControl::
addSphereSlot()
{
  auto *scene = app_->getScene();

  std::string name = "sphere";

  auto *sphere = CGeometryInst->createObject3D(scene, name);

  CGeomSphere3D::addGeometry(sphere, CPoint3D(0, 0, 0), 0.1);

  scene->addObject(sphere);

  updateObjects();
}

void
CQCamera3DControl::
addTorusSlot()
{
  auto *scene = app_->getScene();

  std::string name = "torus";

  auto *torus = CGeometryInst->createObject3D(scene, name);

  auto power1  = 1.0;
  auto power2  = 1.0;

  CGeomTorus3D::addGeometry(torus, CPoint3D(0, 0, 0), 0.1, 0.025, power1, power2, 30, 30);

  scene->addObject(torus);

  updateObjects();
}

void
CQCamera3DControl::
addModelSlot()
{
//auto dir = QDir::current().dirName();
  auto dir = app_->buildDir() + "/models";

  auto fileName = QFileDialog::getOpenFileName(this, "Open Model File", dir, "Files (*.*)");
  if (! fileName.length()) return;

  auto format = CImportBase::filenameToType(fileName.toStdString());

  CQCamera3DApp::LoadData loadData;

  if (! app_->loadModel(fileName, format, loadData))
    std::cerr << "Failed to load model '" << fileName.toStdString() << "'\n";

  updateObjects();
}

void
CQCamera3DControl::
currentMaterialSlot()
{
  auto *material = materialsData_.materialList->getCurrentMaterial();
  if (! material) return;

  connectSlots(false);

  auto setColorEdit = [](CQColorEdit *edit, const CGeomMaterial::OptColor &color) {
    edit->setEnabled(!! color);

    if (color)
      edit->setColor(RGBAToQColor(*color));
  };

  auto setRealEdit = [](CQRealSpin *edit, const CGeomMaterial::OptReal &r) {
    edit->setEnabled(!! r);

    if (r)
      edit->setValue(*r);
  };

  auto setTextureEdit = [](CQCamera3DTextureChooser *edit, const CGeomTexture *texture) {
    edit->setEnabled(!! texture);

    edit->setTextureName(texture ? QString::fromStdString(texture->name()) : "");
  };

  setColorEdit(materialsData_.ambientEdit , material->ambient ());
  setColorEdit(materialsData_.diffuseEdit , material->diffuse ());
  setColorEdit(materialsData_.specularEdit, material->specular());
  setColorEdit(materialsData_.emissionEdit, material->emission());

  setRealEdit(materialsData_.shininessEdit   , material->shininess   ());
  setRealEdit(materialsData_.transparencyEdit, material->transparency());

  materialsData_.twoSidedCheck->setChecked(material->isTwoSided());

  materialsData_.shadingCombo->setCurrentIndex(
    materialShadingInd.typeToInd(material->shading()));

  setTextureEdit(materialsData_.ambientTextureEdit , material->ambientTexture ());
  setTextureEdit(materialsData_.diffuseTextureEdit , material->diffuseTexture ());
  setTextureEdit(materialsData_.normalTextureEdit  , material->normalTexture  ());
  setTextureEdit(materialsData_.specularTextureEdit, material->specularTexture());
  setTextureEdit(materialsData_.emissiveTextureEdit, material->emissiveTexture());

  connectSlots(true);
}

void
CQCamera3DControl::
materialAmbientSlot(const QColor &c)
{
  auto *material = materialsData_.materialList->getSelectedMaterial();
  if (! material) return;

  material->setAmbient(QColorToRGBA(c));

  updateObjects();
}

void
CQCamera3DControl::
materialDiffuseSlot(const QColor &c)
{
  auto *material = materialsData_.materialList->getSelectedMaterial();
  if (! material) return;

  material->setDiffuse(QColorToRGBA(c));

  updateObjects();
}

void
CQCamera3DControl::
materialSpecularSlot(const QColor &c)
{
  auto *material = materialsData_.materialList->getSelectedMaterial();
  if (! material) return;

  material->setSpecular(QColorToRGBA(c));

  updateObjects();
}

void
CQCamera3DControl::
materialEmissionSlot(const QColor &c)
{
  auto *material = materialsData_.materialList->getSelectedMaterial();
  if (! material) return;

  material->setEmission(QColorToRGBA(c));

  updateObjects();
}

void
CQCamera3DControl::
materialShininessSlot(double r)
{
  auto *material = materialsData_.materialList->getSelectedMaterial();
  if (! material) return;

  material->setShininess(r);

  updateObjects();
}

void
CQCamera3DControl::
materialTransparencySlot(double r)
{
  auto *material = materialsData_.materialList->getSelectedMaterial();
  if (! material) return;

  material->setTransparency(r);

  updateObjects();
}

void
CQCamera3DControl::
materialAmbientTextureSlot()
{
  auto *canvas = app_->canvas();
  auto *edit = qobject_cast<CQCamera3DTextureChooser *>(sender());

  auto textureName = edit->textureName();
  auto *texture = canvas->getGeomTextureByName(textureName.toStdString());

  auto *material = materialsData_.materialList->getSelectedMaterial();
  if (! material) return;

  material->setAmbientTexture(texture);

  updateObjects();
}

void
CQCamera3DControl::
materialDiffuseTextureSlot()
{
  auto *canvas = app_->canvas();
  auto *edit = qobject_cast<CQCamera3DTextureChooser *>(sender());

  auto textureName = edit->textureName();
  auto *texture = canvas->getGeomTextureByName(textureName.toStdString());

  auto *material = materialsData_.materialList->getSelectedMaterial();
  if (! material) return;

  material->setDiffuseTexture(texture);

  updateObjects();
}

void
CQCamera3DControl::
materialNormalTextureSlot()
{
  auto *canvas = app_->canvas();
  auto *edit = qobject_cast<CQCamera3DTextureChooser *>(sender());

  auto textureName = edit->textureName();
  auto *texture = canvas->getGeomTextureByName(textureName.toStdString());

  auto *material = materialsData_.materialList->getSelectedMaterial();
  if (! material) return;

  material->setNormalTexture(texture);

  updateObjects();
}

void
CQCamera3DControl::
materialSpecularTextureSlot()
{
  auto *canvas = app_->canvas();
  auto *edit = qobject_cast<CQCamera3DTextureChooser *>(sender());

  auto textureName = edit->textureName();
  auto *texture = canvas->getGeomTextureByName(textureName.toStdString());

  auto *material = materialsData_.materialList->getSelectedMaterial();
  if (! material) return;

  material->setSpecularTexture(texture);

  updateObjects();
}

void
CQCamera3DControl::
materialEmissiveTextureSlot()
{
  auto *canvas = app_->canvas();
  auto *edit = qobject_cast<CQCamera3DTextureChooser *>(sender());

  auto textureName = edit->textureName();
  auto *texture = canvas->getGeomTextureByName(textureName.toStdString());

  auto *material = materialsData_.materialList->getSelectedMaterial();
  if (! material) return;

  material->setEmissiveTexture(texture);

  updateObjects();
}

void
CQCamera3DControl::
overviewEqualScaleSlot(int i)
{
  auto *overview = app_->overview();

  overview->setEqualScale(i);
}

void
CQCamera3DControl::
overviewSelectTypeSlot(int i)
{
  auto *overview = app_->overview();

  overview->setSelectType(selectTypeInd.indToType(i));
}

void
CQCamera3DControl::
overviewMouseTypeSlot(int i)
{
  auto *overview = app_->overview();

  overview->setMouseType(mouseTypeInd.indToType(i));
}

void
CQCamera3DControl::
overviewModelTypeSlot(int i)
{
  auto *overview = app_->overview();

  overview->setModelType(overviewModelTypeInd.indToType(i));
}

void
CQCamera3DControl::
overviewCameraSlot(int i)
{
  auto *overview = app_->overview();

  overview->setCameraVisible(i);
}

void
CQCamera3DControl::
overviewLightsSlot(int i)
{
  auto *overview = app_->overview();

  overview->setLightsVisible(i);
}

void
CQCamera3DControl::
uvTextureTypeSlot(int i)
{
  auto *uvMap = app_->uvMap();

  uvMap->setTextureType(uvTextureTypeInd.indToType(i));
}

//---

void
CQCamera3DControl::
timerSlot()
{
  if (animData_.running)
    animStep();
}

//---

void
CQCamera3DControl::
bonesModelSlot(int i)
{
  auto *bones = app_->bones();

  bones->setShowModel(i);

  bones->update();
}

void
CQCamera3DControl::
currentBoneSlot()
{
  updateBones();
}

void
CQCamera3DControl::
updateBones()
{
  auto *node = bonesData_.bonesList->currentBoneNode();
  if (! node) return;

  bonesData_.translationEdit->setValue(node->localTranslation().point());
  bonesData_.rotationEdit   ->setValue(node->localRotation   ().point());
  bonesData_.scaleEdit      ->setValue(node->localScale      ().point());

  if (app_->animName() != "") {
    auto animName = app_->animName().toStdString();
    auto animTime = app_->animTime();

    if (const_cast<CGeomObject3D *>(node->object())->
          updateNodeAnimationData(*node, animName, animTime)) {
      auto &animationData = node->getAnimationData(animName);

      bonesData_.animTranslationEdit->setValue(animationData.animTranslation().point());
      bonesData_.animRotationEdit   ->setValue(animationData.animRotation   ().point());
      bonesData_.animScaleEdit      ->setValue(animationData.animScale      ().point());
    }
    else {
      bonesData_.translationEdit->setValue(node->localTranslation().point());
      bonesData_.rotationEdit   ->setValue(node->localRotation   ().point());
      bonesData_.scaleEdit      ->setValue(node->localScale      ().point());
    }
  }

  app_->setCurrentBoneObject(node->object()->getInd());
  app_->setCurrentBoneNode  (node->ind());
}

void
CQCamera3DControl::
animNameSlot()
{
  auto name = animData_.animCombo->animName();

  app_->setAnimName(name);
}

void
CQCamera3DControl::
animTimeSlot(double t)
{
  app_->setAnimTime(t);
}

void
CQCamera3DControl::
playSlot()
{
  if (! animData_.running) {
    app_->startTimer();
  }

  animStep();

  animData_.running = true;
}

void
CQCamera3DControl::
pauseSlot()
{
  animData_.running = false;
}

void
CQCamera3DControl::
stepSlot()
{
  animStep();
}

void
CQCamera3DControl::
animStep()
{
  auto min = animData_.timeEdit->minimum();
  auto max = animData_.timeEdit->maximum();

  auto v = animData_.timeEdit->value();

  v += (max - min)/100.0;

  if (v >= max)
    v = min;

  animData_.timeEdit->setValue(v);
}

void
CQCamera3DControl::
updateAnim()
{
  auto animName = app_->animName().toStdString();
  auto animTime = app_->animTime();

  auto objId  = app_->currentBoneObject();
  auto nodeId = app_->currentBoneNode();

  auto *scene = app_->getScene();

  auto *object = scene->getObjectByInd(objId);
  if (! object) return;

  auto *node = object->getNodeByInd(nodeId);
  if (! node) return;

  if (! node->hasAnimationData(animName))
    return;

  if (object->updateNodeAnimationData(*node, animName, animTime)) {
    auto pointToString = [](const CVector3D &p) {
      return QString("%1 %2 %3").arg(p.getX()).arg(p.getY()).arg(p.getZ());
    };

    auto quaternionToString = [](const CQuaternion &q) {
      return QString("%1 %2 %3 %4").arg(q.getW()).arg(q.getX()).arg(q.getY()).arg(q.getZ());
    };

    //---

    const auto &animationData = node->getAnimationData(animName);

    animData_.nodeLabel->setText(QString::fromStdString(node->name()));

    animData_.translationInterpCombo->setCurrentIndex(
      animInterpolationInd.typeToInd(animationData.translationInterpolation()));
    animData_.rotationInterpCombo->setCurrentIndex(
      animInterpolationInd.typeToInd(animationData.rotationInterpolation()));
    animData_.scaleInterpCombo->setCurrentIndex(
      animInterpolationInd.typeToInd(animationData.scaleInterpolation()));

    animData_.animTranslationTable->setColumnCount(2);
    animData_.animRotationTable   ->setColumnCount(2);
    animData_.animScaleTable      ->setColumnCount(2);

    animData_.animTranslationTable->setHorizontalHeaderLabels(QStringList() << "Time" << "Value");
    animData_.animRotationTable   ->setHorizontalHeaderLabels(QStringList() << "Time" << "Value");
    animData_.animScaleTable      ->setHorizontalHeaderLabels(QStringList() << "Time" << "Value");

    const auto &translationRange = animationData.translationRange();
    const auto &translations     = animationData.translations    ();

    int ntr = int(translationRange.size());

    animData_.animTranslationTable->setRowCount(ntr);

    for (int r = 0; r < ntr; ++r) {
      auto *item1 = new QTableWidgetItem(QString("%1").arg(translationRange[r]));
      auto *item2 = new QTableWidgetItem(pointToString(translations[r]));

      animData_.animTranslationTable->setItem(r, 0, item1);
      animData_.animTranslationTable->setItem(r, 1, item2);
    }

    const auto &rotationRange = animationData.rotationRange();
    const auto &rotations     = animationData.rotations    ();

    int nrr = int(rotationRange.size());

    animData_.animRotationTable->setRowCount(nrr);

    for (int r = 0; r < nrr; ++r) {
      auto *item1 = new QTableWidgetItem(QString("%1").arg(rotationRange[r]));
      auto *item2 = new QTableWidgetItem(quaternionToString(rotations[r]));

      animData_.animRotationTable->setItem(r, 0, item1);
      animData_.animRotationTable->setItem(r, 1, item2);
    }

    const auto &scaleRange = animationData.scaleRange();
    const auto &scales     = animationData.scales    ();

    int nsr = int(scaleRange.size());

    animData_.animScaleTable->setRowCount(nsr);

    for (int r = 0; r < nsr; ++r) {
      auto *item1 = new QTableWidgetItem(QString("%1").arg(scaleRange[r]));
      auto *item2 = new QTableWidgetItem(pointToString(scales[r]));

      animData_.animScaleTable->setItem(r, 0, item1);
      animData_.animScaleTable->setItem(r, 1, item2);
    }
  }
}

void
CQCamera3DControl::
updateObjects()
{
  auto *canvas = app_->canvas();

  canvas->addObjectsData();
  canvas->update();

  auto *overview = app_->overview();

  overview->update();
}
