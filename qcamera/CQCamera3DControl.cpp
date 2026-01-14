#include <CQCamera3DControl.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DObjectData.h>
#include <CQCamera3DOverview.h>
#include <CQCamera3DCamera.h>
#include <CQCamera3DLight.h>
#include <CQCamera3DGeomObject.h>
#include <CQCamera3DStatus.h>
#include <CQCamera3DAxes.h>
#include <CQCamera3DUVMap.h>
#include <CQCamera3DNormals.h>
#include <CQCamera3DBasis.h>
#include <CQCamera3DBBox.h>
#include <CQCamera3DBones.h>
#include <CQCamera3DObjectsList.h>
#include <CQCamera3DMaterialList.h>
#include <CQCamera3DMaterialChooser.h>
#include <CQCamera3DLightList.h>
#include <CQCamera3DCameraList.h>
#include <CQCamera3DTextureList.h>
#include <CQCamera3DTextureChooser.h>
#include <CQCamera3DTexture.h>
#include <CQCamera3DAnimChooser.h>
#include <CQCamera3DBonesList.h>
#include <CQCamera3DGeomFace.h>
#include <CQCamera3DUtil.h>

#include <CQGLBuffer.h>
#include <CImportBase.h>
#include <CGeometry3D.h>
#include <CGeomScene3D.h>
#include <CGeomObject3D.h>
#include <CGeomFace3D.h>
#include <CGeomNodeData.h>
#include <CGeomTexture.h>

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

#include <fstream>

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

class EditTypeInd : public ValueMap<CQCamera3DEditType, int> {
 public:
  EditTypeInd() {
    add("Select", CQCamera3DEditType::SELECT, 0);
    add("Cursor", CQCamera3DEditType::CURSOR, 1);
    add("Move"  , CQCamera3DEditType::MOVE  , 2);
    add("Rotate", CQCamera3DEditType::ROTATE, 3);
    add("Scale" , CQCamera3DEditType::SCALE , 4);
  };

  CQCamera3DEditType indToType(int ind) {
    return lookup(ind);
  }

  int typeToInd(const CQCamera3DEditType &type) {
    return lookup(type);
  }
};

EditTypeInd editTypeInd;

class MouseTypeInd : public ValueMap<CQCamera3DMouseType, int> {
 public:
  MouseTypeInd() {
    add("Camera", CQCamera3DMouseType::CAMERA, 0);
    add("Light" , CQCamera3DMouseType::LIGHT , 1);
    add("Object", CQCamera3DMouseType::OBJECT, 2);
    add("Face"  , CQCamera3DMouseType::FACE  , 3);
    add("Edge"  , CQCamera3DMouseType::EDGE  , 4);
    add("Point" , CQCamera3DMouseType::POINT , 5);
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

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  //---

  auto *canvas = app_->canvas();

  //---

  auto *layout = new QVBoxLayout(this);
  layout->setMargin(2); layout->setSpacing(2);

  //---

  QBoxLayout*               currentLayout = layout;
  std::vector<QBoxLayout *> layoutStack;

  auto startLayout = [&](QBoxLayout *layout) {
    layoutStack.push_back(currentLayout);

    currentLayout = layout;
  };

  auto endLayout = [&]() {
    currentLayout = layoutStack.back();

    layoutStack.pop_back();
  };

  //---

  auto addLabelEdit = [&](const QString &label, auto *w) {
    auto *frame = new QFrame;

    auto *layout1 = new QHBoxLayout(frame);
    layout1->setMargin(2); layout1->setSpacing(2);

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

  QGroupBox*               currentGroup = nullptr;
  std::vector<QGroupBox *> groupStack;

  auto startGroup = [&](const QString &name, bool horizontal=false) {
    groupStack.push_back(currentGroup);

    currentGroup = new QGroupBox(name);

    currentGroup->setObjectName(name);

    QBoxLayout *layout = nullptr;

    if (horizontal)
      layout = new QHBoxLayout(currentGroup);
    else
      layout = new QVBoxLayout(currentGroup);

    layout->setMargin(2); layout->setSpacing(2);

    currentLayout->addWidget(currentGroup);

    startLayout(layout);
  };

  auto endGroup = [&]() {
    endLayout();

    currentGroup = groupStack.back();

    groupStack.pop_back();
  };

  //---

  QFrame*               currentFrame = nullptr;
  std::vector<QFrame *> frameStack;

  auto startFrame = [&](bool horizontal=false) {
    frameStack.push_back(currentFrame);

    currentFrame = new QFrame;

    currentFrame->setObjectName("frame");

    QBoxLayout *layout = nullptr;

    if (horizontal)
      layout = new QHBoxLayout(currentFrame);
    else
      layout = new QVBoxLayout(currentFrame);

    layout->setMargin(2); layout->setSpacing(2);

    currentLayout->addWidget(currentFrame);

    startLayout(layout);
  };

  auto endFrame = [&]() {
    endLayout();

    currentFrame = frameStack.back();

    frameStack.pop_back();
  };

  //---

  auto addButton = [&](const QString &name, const char *slotName) {
    auto *button = new QPushButton(name);

    connect(button, SIGNAL(clicked()), this, slotName);

    currentLayout->addWidget(button);

    return button;
  };

  //---

  QTabWidget*               currentTab = nullptr;
  std::vector<QTabWidget *> tabStack;

  auto startTab = [&](const QString &name) {
    tabStack.push_back(currentTab);

    currentTab = CQUtil::makeWidget<QTabWidget>(name + "_tab");

    currentTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    currentLayout->addWidget(currentTab);

    return currentTab;
  };

  auto endTab = [&]() {
    currentTab = tabStack.back();

    tabStack.pop_back();
  };

  //---

  QFrame*               currentPage = nullptr;
  std::vector<QFrame *> pageStack;

  auto startTabPage = [&](const QString &name) {
    pageStack.push_back(currentPage);

    currentPage = CQUtil::makeWidget<QFrame>(name + "_page");

    auto *layout = new QVBoxLayout(currentPage);

    layout->setMargin(2); layout->setSpacing(2);

    currentTab->addTab(currentPage, name);

    startLayout(layout);
  };

  auto endTabPage = [&]() {
    endLayout();

    currentPage = pageStack.back();

    pageStack.pop_back();
  };

  //---

  mainTab_ = startTab("main");

  //---

  // 3D

  startTabPage("3D");

  startTab("3D");

  //---

  // 3D/General
  startTabPage("General");

  //---

  startGroup("Show", /*horizontal*/true);

  generalData_.showWireframeCheck = addCheck("Wireframe");
  generalData_.showSolidCheck     = addCheck("Solid");
  generalData_.showTexturedCheck  = addCheck("Textured");
  generalData_.showPointsCheck    = addCheck("Points");

  addStretch();

  endGroup();

  generalData_.quadViewCheck = addLabelEdit("Quad View", new QCheckBox);

  generalData_.pointSizeEdit = addLabelEdit("Point Size", new CQRealSpin);
  generalData_.lineWidthEdit = addLabelEdit("Line Width", new CQRealSpin);

  generalData_.cursorEdit = addLabelEdit("Cursor", new CQPoint3DEdit);

  startGroup("Normals");

  startFrame(/*horizontal*/true);

  generalData_.normalsPointsCheck = addCheck("Points");
  generalData_.normalsFacesCheck  = addCheck("Faces" );

  addStretch();

  endFrame();

  generalData_.normalsSizeEdit  = addLabelEdit("Size" , new CQRealSpin);
  generalData_.normalsColorEdit = addLabelEdit("Color", new CQColorEdit);

  endGroup();

  startGroup("Basis");

  generalData_.showBasisCheck = addCheck("Show");

  endGroup();

  startGroup("BBox");

  generalData_.showBBoxCheck   = addCheck("Show");
  generalData_.bboxOrientCheck = addCheck("Oriented");

  endGroup();

  startGroup("Options");

  generalData_.depthTestCheck = addCheck("Depth Test");
  generalData_.cullFaceCheck  = addCheck("Cull Face");
  generalData_.frontFaceCheck = addCheck("Front Face");

  endGroup();

  addStretch();

  endTabPage();

  //------

  // 3D/Camera
  startTabPage("Camera");

  //---

  startGroup("Show", /*horizontal*/true);

  cameraData_.showCheck    = addCheck("Camera");
  cameraData_.eyeLineCheck = addCheck("Eye Line");
  cameraData_.planesCheck  = addCheck("Planes");

  addStretch();

  endGroup();

  cameraData_.cameraList = new CQCamera3DCameraList(canvas);

  currentLayout->addWidget(cameraData_.cameraList);

  startGroup("Angles");

  cameraData_.pitchEdit = addLabelEdit("Pitch", new CQRealSpin);
  cameraData_.yawEdit   = addLabelEdit("Yaw"  , new CQRealSpin);
  cameraData_.rollEdit  = addLabelEdit("Roll" , new CQRealSpin);

  endGroup();

  startGroup("Z");

  cameraData_.nearEdit = addLabelEdit("Near", new CQRealSpin);
  cameraData_.farEdit  = addLabelEdit("Far" , new CQRealSpin);
  cameraData_.fovEdit  = addLabelEdit("FOV" , new CQRealSpin);

  endGroup();

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

  startGroup("Eye Line");

  cameraData_.eyeZ1Edit = addLabelEdit("Z1", new CQRealSpin);
  cameraData_.eyeZ2Edit = addLabelEdit("Z2", new CQRealSpin);

  endGroup();

  //---

  auto plusMinusFrame = [&](const QString &name) {
    PlusMinusData data;

    auto *pframe  = new QFrame;
    auto *playout = new QHBoxLayout(pframe);

    playout->setMargin(2); playout->setSpacing(2);

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

  startGroup("Rotate Around", /*horizontal*/true);

  cameraData_.aroundXButtons = plusMinusFrame("X");
  cameraData_.aroundYButtons = plusMinusFrame("Y");
  cameraData_.aroundZButtons = plusMinusFrame("Z");

  addStretch();

  endGroup();

  addStretch();

  startFrame(/*horizontal*/true);

  addButton("Reset", SLOT(resetCameraSlot()));
  addButton("Top"  , SLOT(topCameraSlot()));
  addButton("Side" , SLOT(sideCameraSlot()));
  addButton("Front", SLOT(frontCameraSlot()));

  addStretch();

  endFrame();

  endTabPage();

  //------

  // 3D/Lights
  startTabPage("Lights");

  lightsData_.lightList = new CQCamera3DLightList(canvas);

  currentLayout->addWidget(lightsData_.lightList);

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

  startFrame(/*horizontal*/true);

  addButton("Add Light"  , SLOT(addLightSlot()));
  addButton("Reset Light", SLOT(resetLightSlot()));

  addStretch();

  endFrame();

  endTabPage();

  //------

  // 3D/Axis
  startTabPage("Axis");

  axisData_.showCheck = addCheck("Show Axes");

  axisData_.xPosEdit = addLabelEdit("X Pos", new CQRealSpin);
  axisData_.yPosEdit = addLabelEdit("Y Pos", new CQRealSpin);
  axisData_.zPosEdit = addLabelEdit("Z Pos", new CQRealSpin);

  addStretch();

  endTabPage();

  //------

  // 3D/Mouse
  startTabPage("Mouse");

  mouseTypeCombo_ = addLabelEdit("Mouse Type", new QComboBox);
  mouseTypeCombo_->addItems(mouseTypeInd.names());

  editTypeCombo_ = addLabelEdit("Edit Type", new QComboBox);
  editTypeCombo_->addItems(editTypeInd.names());

  mouseScaleEdit_ = addLabelEdit("Scale", new CQRealSpin);

  mouseBasisCheck_ = addLabelEdit("Basis", new QCheckBox);

  addStretch();

  endTabPage();

  //------

  // 3D/Selection
  startTabPage("Selection");

  selectionData_.typeCombo = addLabelEdit("Select", new QComboBox);
  selectionData_.typeCombo->addItems(selectTypeInd.names());

  selectionData_.indLabel = addLabelEdit("Name", new CQTextLabel);
  selectionData_.visCheck = addLabelEdit("Visible", new QCheckBox);

  selectionData_.colorEdit = addLabelEdit("Color", new CQColorEdit);

  //---

  startTab("objData");

  startTabPage("Geometry");

  selectionData_.centerEdit = addLabelEdit("Center", new CQPoint3DEdit);
  selectionData_.sizeEdit   = addLabelEdit("Size"  , new CQPoint3DEdit);

  selectionData_.meshMatrixEdit = new CQMatrix3D;
  currentLayout->addWidget(selectionData_.meshMatrixEdit);

  addStretch();

  endTabPage();

  //--

  startTabPage("Transform");

  selectionData_.translationEdit = addLabelEdit("Translation", new CQPoint3DEdit);
  selectionData_.rotationEdit    = addLabelEdit("Rotation"   , new CQPoint3DEdit);
  selectionData_.scaleEdit       = addLabelEdit("Scale"      , new CQPoint3DEdit);

  selectionData_.matrixEdit = new CQMatrix3D;
  currentLayout->addWidget(selectionData_.matrixEdit);

  addStretch();

  endTabPage();

  //--

  startTabPage("Textures");

  selectionData_.diffuseTextureEdit =
    addLabelEdit("Diffuse" , new CQCamera3DTextureChooser(app_));
  selectionData_.normalTextureEdit =
    addLabelEdit("Normal"  , new CQCamera3DTextureChooser(app_));
  selectionData_.specularTextureEdit =
    addLabelEdit("Specular", new CQCamera3DTextureChooser(app_));
  selectionData_.emissiveTextureEdit =
    addLabelEdit("Emissive", new CQCamera3DTextureChooser(app_));

  startFrame(/*horizontal*/true);

  addButton("Add Texture"     , SLOT(addTextureSlot()));
  addButton("Load Texture Map", SLOT(loadTextureMapSlot()));
  addButton("Save Texture Map", SLOT(saveTextureMapSlot()));

  addStretch();

  endFrame();

  addStretch();

  endTabPage();

  //--

  startTabPage("Material");

  selectionData_.materialNameChooser =
    addLabelEdit("Name", new CQCamera3DMaterialChooser(app_));

  startFrame(/*horizontal*/true);

  addButton("Load Material Map", SLOT(loadMaterialMapSlot()));
  addButton("Save Material Map", SLOT(saveMaterialMapSlot()));

  addStretch();

  endFrame();

  addStretch();

  endTabPage();

  endTab();

  //---

  startGroup("Modify");

  startFrame(/*horizontal*/true);

  addButton("Swap XY", SLOT(swapSlot()));
  addButton("Swap YZ", SLOT(swapSlot()));
  addButton("Swap ZX", SLOT(swapSlot()));

  addStretch();

  endFrame();

  startFrame(/*horizontal*/true);

  addButton("Invert X", SLOT(invertSlot()));
  addButton("Invert Y", SLOT(invertSlot()));
  addButton("Invert Z", SLOT(invertSlot()));

  addStretch();

  endFrame();

  endGroup();

  //---

  selectionData_.objectInfoText = new QTextEdit;

  selectionData_.objectInfoText->setReadOnly(true);

  currentLayout->addWidget(selectionData_.objectInfoText);

  //addStretch();

  //---

  startGroup("Select", /*horizontal*/true);

  addButton("Parent"     , SLOT(selectParentSlot()));
  addButton("By Material", SLOT(selectRelatedSlot()));
  addButton("Faces"      , SLOT(selectFacesSlot()));
  addButton("Points"     , SLOT(selectPointsSlot()));
  addButton("Clear"      , SLOT(deselectSlot()));
  addButton("Dump"       , SLOT(selectDumpSlot()));

  addStretch();

  endGroup();

  endTabPage();

  //------

  // 3D/Objects
  startTabPage("Objects");

  objectsData_.objectsList = new CQCamera3DObjectsList(canvas);

  currentLayout->addWidget(objectsData_.objectsList);

  startFrame(/*horizontal*/true);

  objectsData_.autoSelectCheck = addLabelEdit("Auto Select", new QCheckBox);

  addButton("Select", SLOT(objectSelectSlot()));
  addButton("Zoom"  , SLOT(objectZoomSlot()));

  addStretch();

  endFrame();

  startGroup("Add");

  startFrame(/*horizontal*/true);

  addButton("Cube"    , SLOT(addCubeSlot()));
  addButton("Cylinder", SLOT(addCylinderSlot()));
  addButton("Pyramid" , SLOT(addPyramidSlot()));
  addButton("Sphere"  , SLOT(addSphereSlot()));
  addButton("Torus"   , SLOT(addTorusSlot()));

  addButton("Load", SLOT(loadModelSlot()));
  addButton("Save", SLOT(saveModelSlot()));

  addStretch();

  endFrame();

  endGroup();

  endTabPage();

  endTabPage();

  endTab();

  //------

  // Overview

  startTabPage("Overview");

  overviewData_.equalScale = addLabelEdit("Equal Scale", new QCheckBox);

  overviewData_.mouseTypeCombo = addLabelEdit("Mouse", new QComboBox);
  overviewData_.mouseTypeCombo->addItems(mouseTypeInd.names());

  overviewData_.selectTypeCombo = addLabelEdit("Select", new QComboBox);
  overviewData_.selectTypeCombo->addItems(selectTypeInd.names());

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

  endTabPage();

  //------

  // UV

  startTabPage("UV");

  uvData_.typeCombo = addLabelEdit("Texture", new QComboBox);
  uvData_.typeCombo->addItems(uvTextureTypeInd.names());

  addStretch();

  endTabPage();

  //------

  // Textures

  startTabPage("Textures");

  texturesData_.textureList = new CQCamera3DTextureList(canvas);

  currentLayout->addWidget(texturesData_.textureList);

  startGroup("Current");

  texturesData_.flipCheck = addLabelEdit("Flip", new QCheckBox);

  endGroup();

  startFrame(/*horizontal*/true);

  addButton("Add Texture", SLOT(addTextureSlot()));

  addStretch();

  endFrame();

  addStretch();

  endTabPage();

  //------

  // Materials

  startTabPage("Materials");

  materialsData_.materialList = new CQCamera3DMaterialList(app_);

  currentLayout->addWidget(materialsData_.materialList);

  startGroup("General");

  materialsData_.twoSidedCheck = addLabelEdit("Two Sided", new QCheckBox);

  materialsData_.shadingCombo = addLabelEdit("Shading", new QComboBox);
  materialsData_.shadingCombo->addItems(materialShadingInd.names());

  endGroup();

  startGroup("Colors");

  materialsData_.ambientEdit  = addLabelEdit("Ambient" , new CQColorEdit);
  materialsData_.diffuseEdit  = addLabelEdit("Diffuse" , new CQColorEdit);
  materialsData_.specularEdit = addLabelEdit("Specular", new CQColorEdit);
  materialsData_.emissionEdit = addLabelEdit("Emission", new CQColorEdit);

  materialsData_.shininessEdit = addLabelEdit("Shininess", new CQRealSpin);

  materialsData_.transparencyEdit = addLabelEdit("Transparency", new CQRealSpin);

  endGroup();

  startGroup("Textures");

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

  endGroup();

  startFrame(/*horizontal*/true);

  addButton("Add Material", SLOT(addMaterialSlot()));

  addStretch();

  endFrame();

  endTabPage();

  //------

  // Bones

  startTabPage("Bones");

  startGroup("Show");

  bonesData_.modelCheck       = addLabelEdit("Model", new QCheckBox);
  bonesData_.boneNodesCheck   = addLabelEdit("Bone Nodes", new QCheckBox);
  bonesData_.pointJointsCheck = addLabelEdit("Point Joints", new QCheckBox);
  bonesData_.onlyJointsCheck  = addLabelEdit("Only Joints", new QCheckBox);

  endGroup();

  bonesData_.bonesList = new CQCamera3DBonesList(app_);
  currentLayout->addWidget(bonesData_.bonesList);

  bonesData_.nodeLabel = addLabelEdit("Node", new CQTextLabel);

  bonesData_.childrenLabel = addLabelEdit("Children", new CQTextLabel);

  bonesData_.jointCheck = addLabelEdit("Is Joint", new QCheckBox);

  bonesData_.objectLabel = addLabelEdit("Object", new CQTextLabel);

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

  endTabPage();

  //------

  // Animation

  startTabPage("Animation");

  animData_.animCombo    = addLabelEdit("Name", new CQCamera3DAnimChooser(app_));
  animData_.timeEdit     = addLabelEdit("Time", new CQRealSpin);
  animData_.timeStepEdit = addLabelEdit("Step", new CQRealSpin);

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

  endTabPage();

  //------

  endTab();

  connectSlots(true);

  updateWidgets();

  if (! layoutStack.empty()) std::cerr << "Bad layout stack\n";
  if (! groupStack.empty()) std::cerr << "Bad group stack\n";
  if (! frameStack.empty()) std::cerr << "Bad frame stack\n";
  if (! tabStack.empty()) std::cerr << "Bad tab stack\n";
  if (! pageStack.empty()) std::cerr << "Bad page stack\n";
}

void
CQCamera3DControl::
updateWidgets()
{
  connectSlots(false);

  auto *camera = getCamera();
  auto *canvas = app_->canvas();

  auto *bbox = canvas->getBBoxOverlay();

  // General
  generalData_.showWireframeCheck->setChecked(canvas->isWireframe());
  generalData_.showSolidCheck    ->setChecked(canvas->isSolid());
  generalData_.showTexturedCheck ->setChecked(canvas->isTextured());
  generalData_.showPointsCheck   ->setChecked(canvas->isPoints());

  generalData_.quadViewCheck->setChecked(canvas->isQuadView());

  generalData_.pointSizeEdit->setValue(canvas->pointSize());
  generalData_.lineWidthEdit->setValue(canvas->lineWidth());

  generalData_.cursorEdit->setValue(canvas->cursor());

  auto *normals = canvas->getNormals();

  generalData_.normalsPointsCheck->setChecked(normals ? normals->isPointNormals() : false);
  generalData_.normalsFacesCheck ->setChecked(normals ? normals->isFaceNormals () : false);
  generalData_.normalsSizeEdit   ->setValue(canvas->normalsSize());
  generalData_.normalsColorEdit  ->setColor(canvas->normalsColor());

  generalData_.showBasisCheck->setChecked(canvas->isShowBasis());

  generalData_.showBBoxCheck  ->setChecked(canvas->isShowBBox());
  generalData_.bboxOrientCheck->setChecked(bbox ? bbox->isOriented() : false);

  generalData_.depthTestCheck->setChecked(canvas->isDepthTest());
  generalData_.cullFaceCheck ->setChecked(canvas->isCullFace());
  generalData_.frontFaceCheck->setChecked(canvas->isFrontFace());

  // Camera
  cameraData_.showCheck   ->setChecked(canvas->isShowCamera());
  cameraData_.eyeLineCheck->setChecked(canvas->isShowEyeLine());
  cameraData_.planesCheck ->setChecked(canvas->isShowPlanes());

  cameraData_.cameraList->updateCameras();

  cameraData_.pitchEdit->setValue(camera ? camera->pitch() : 0.0);
  cameraData_.yawEdit  ->setValue(camera ? camera->yaw  () : 0.0);
  cameraData_.rollEdit ->setValue(camera ? camera->roll () : 0.0);

  cameraData_.nearEdit->setValue(camera ? camera->near() : 0.0);
  cameraData_.farEdit ->setValue(camera ? camera->far () : 0.0);

  cameraData_.xOriginEdit->setValue(camera ? camera->origin().getX() : 0.0);
  cameraData_.yOriginEdit->setValue(camera ? camera->origin().getY() : 0.0);
  cameraData_.zOriginEdit->setValue(camera ? camera->origin().getZ() : 0.0);

  cameraData_.xEdit->setValue(camera ? camera->position().getX() : 0.0);
  cameraData_.yEdit->setValue(camera ? camera->position().getY() : 0.0);
  cameraData_.zEdit->setValue(camera ? camera->position().getZ() : 0.0);

  cameraData_.fovEdit->setValue(camera ? camera->fov() : 0.0);

  cameraData_.eyeZ1Edit->setValue(canvas->eyeLineZ1());
  cameraData_.eyeZ2Edit->setValue(canvas->eyeLineZ2());

  // Lights
  lightsData_.lightList->updateLights();

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

  editTypeCombo_->setCurrentIndex(editTypeInd.typeToInd(canvas->editType()));

  mouseScaleEdit_->setValue(canvas->mouseScale());

  mouseBasisCheck_->setChecked(canvas->isMouseBasis());

  // Selection
  selectionData_.typeCombo->setCurrentIndex(selectTypeInd.typeToInd(canvas->selectType()));

  auto selectType = canvas->selectType();

  selectionData_.meshMatrixEdit->setEnabled(selectType == CQCamera3DSelectType::OBJECT);

  selectionData_.translationEdit->setEnabled(selectType == CQCamera3DSelectType::OBJECT);
  selectionData_.rotationEdit   ->setEnabled(selectType == CQCamera3DSelectType::OBJECT);
  selectionData_.scaleEdit      ->setEnabled(selectType == CQCamera3DSelectType::OBJECT);
  selectionData_.matrixEdit     ->setEnabled(selectType == CQCamera3DSelectType::OBJECT);

  selectionData_.colorEdit->setEnabled(false);

  selectionData_.indLabel->setText("");
  selectionData_.visCheck->setChecked(false);

  selectionData_.materialNameChooser->setMaterialName("");

  selectionData_.objectInfoText->setText("");

  if      (selectType == CQCamera3DSelectType::OBJECT) {
    auto *object = canvas->currentObject();

    if (object) {
      CBBox3D bbox;

      auto *objectData = canvas->getObjectData(object);

      if (objectData)
        bbox = objectData->bbox();

      auto name     = QString::fromStdString(object->getName());
      auto meshName = QString::fromStdString(object->getMeshName());

      selectionData_.indLabel->setText(QString("%1 (Mesh: %2, #%3)").
        arg(name).arg(meshName).arg(object->getInd()));

      selectionData_.visCheck->setChecked(object->getSelected());

      selectionData_.colorEdit->setEnabled(true);
      selectionData_.colorEdit->setColor(RGBAToQColor(object->getFaceColor()));

      selectionData_.translationEdit->setValue(bbox.getCenter());
      selectionData_.sizeEdit       ->setValue(bbox.getSize().point());

      selectionData_.meshMatrixEdit->setValue(object->getMeshGlobalTransform());
//    selectionData_.meshMatrixEdit->setValue(object->getMeshLocalTransform());
//    selectionData_.meshMatrixEdit->setValue(object->getLocalTransform());

      selectionData_.translationEdit->setValue(object->translationValues());
      selectionData_.rotationEdit   ->setValue(object->rotationValuesDeg());
      selectionData_.scaleEdit      ->setValue(object->scaleValues());

      selectionData_.matrixEdit->setValue(object->getTransform());

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

      selectionData_.materialNameChooser->setMaterialName(objectMaterial ?
        QString::fromStdString(objectMaterial->name()) : "");
    }

    //---

    QString objStr;

    auto objects = canvas->getSelectedObjects();

    if      (objects.size() > 1) {
      for (auto *object : objects) {
        auto name     = QString::fromStdString(object->getName());
        auto meshName = QString::fromStdString(object->getMeshName());

        objStr += QString("Object: %1 (%2)").arg(name).arg(meshName);

        const auto &faces = object->getFaces();

        objStr += QString("  Faces: %1\n").arg(faces.size());

        uint nv = 0;
        uint nt = 0;

        for (const auto *face : faces) {
          const auto &vertices = face->getVertices();
          const auto &tpoints  = face->getTexturePoints();

          nv += vertices.size();
          nt += tpoints .size();
        }

        objStr += QString("  Vertices: %1\n").arg(nv);
        objStr += QString("  Texture Points: %1\n").arg(nt);

        const auto &nodes = object->getNodes();
        objStr += QString("  Nodes : %1\n").arg(nodes.size());

        objStr += QString("  MeshNode : %1\n").arg(object->getMeshNode());
      }
    }
    else if (object) {
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

      const auto &nodes = object->getNodes();
      objStr += QString("Nodes : %1\n").arg(nodes.size());

      objStr += QString("MeshNode : %1\n").arg(object->getMeshNode());
    }

    selectionData_.objectInfoText->setText(objStr);
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

      selectionData_.materialNameChooser->setMaterialName(faceMaterial ?
        QString::fromStdString(faceMaterial->name()) : "");
    }

    //---

    QString objStr;

    auto faces = canvas->getSelectedFaces();

    if      (faces.size() > 1) {
      for (const auto *face : faces) {
        auto faceStr = QString("Face %1: Vertices ").arg(face->getInd());

        const auto &vertices = face->getVertices();

        for (const auto &v : vertices)
          faceStr += QString(" %1").arg(v);

        faceStr += "\n";

        objStr += faceStr;
      }
    }
    else if (face) {
      auto faceStr = QString("Face %1: Vertices ").arg(face->getInd());

      const auto &vertices = face->getVertices();

      for (const auto &v : vertices)
        faceStr += QString(" %1").arg(v);

      faceStr += "\n";

      objStr += faceStr;
    }

    selectionData_.objectInfoText->setText(objStr);
  }
  else if (selectType == CQCamera3DSelectType::EDGE) {
    auto faceEdges = canvas->getSelectedFaceEdges();

    QString objStr;

    for (const auto &pf : faceEdges) {
      auto faceStr = QString("Face %1: Ind:").arg(pf.first->getInd());

      for (const auto &edge : pf.second)
        faceStr += QString(" %1").arg(edge);

      objStr += faceStr + "\n";
    }

    selectionData_.objectInfoText->setText(objStr);
  }
  else if (selectType == CQCamera3DSelectType::POINT) {
    auto *vertex = canvas->currentVertex();

    if (vertex) {
      selectionData_.indLabel->setText(QString("%1").arg(vertex->getInd()));

      selectionData_.visCheck->setChecked(vertex->isSelected());
    }

    //---

    auto selectedVertices = canvas->getSelectedVertices();

    bool multipleVertices = false;

    if      (selectedVertices.size() > 1)
      multipleVertices = true;
    else if (selectedVertices.size() == 1 && selectedVertices.begin()->second.size() > 1)
      multipleVertices = true;

    QString objStr;

    if      (multipleVertices) {
      for (const auto &pv : selectedVertices) {
        auto *object = pv.first;

        for (const auto &v : pv.second) {
          const auto &vertex = object->getVertex(v);

          objStr += QString("Vertex: %1\n").arg(vertex.getInd());
        }
      }
    }
    else if (vertex) {
      auto *object     = vertex->getObject();
      auto *objectData = canvas->getObjectData(object);

      auto *buffer = objectData->buffer();

      auto bufferInd = buffer->mapInd(vertex->getInd());

      CQGLBuffer::PointData pointData;
      buffer->getPointData(bufferInd, pointData);

      if (pointData.point)
        objStr += QString("Point: %1 %2 %3\n").
          arg(pointData.point->x).arg(pointData.point->y).arg(pointData.point->z);
      if (pointData.normal)
        objStr += QString("Normal: %1 %2 %3\n").
          arg(pointData.normal->x).arg(pointData.normal->y).arg(pointData.normal->z);
      if (pointData.color)
        objStr += QString("Color: %1 %2 %3\n").
          arg(pointData.color->r).arg(pointData.color->g).arg(pointData.color->b);
      if (pointData.texturePoint)
        objStr += QString("Texture: %1 %2\n").
          arg(pointData.texturePoint->x).arg(pointData.texturePoint->y);
      if (pointData.boneId)
        objStr += QString("Bone: %1 %2 %3 %4\n").
          arg(pointData.boneId->x).arg(pointData.boneId->y).
          arg(pointData.boneId->z).arg(pointData.boneId->w);
      if (pointData.boneWeight)
        objStr += QString("Weights: %1 %2 %3 %4\n").
          arg(pointData.boneWeight->x).arg(pointData.boneWeight->y).
          arg(pointData.boneWeight->z).arg(pointData.boneWeight->w);

      auto *rootObject = object->getRootObject();

      const auto &jointData = vertex->getJointData();

      for (int i = 0; i < 4; ++i) {
        const auto &jointNodeData = jointData.nodeDatas[i];

        int nodeId = -1;

        if (jointNodeData.node >= 0)
          nodeId = rootObject->mapNodeIndex(jointNodeData.node);

        if (nodeId >= 0) {
          auto node = rootObject->getNode(nodeId);
          auto name = QString::fromStdString(node.name());

          objStr += QString("Joint: %1 %2(#%3) %4\n").
            arg(name).arg(nodeId).arg(jointNodeData.node).arg(jointNodeData.weight);
        }
      }
    }

    selectionData_.objectInfoText->setText(objStr);
  }

  // Textures
  auto *currentTexture = dynamic_cast<CQCamera3DTexture *>(
    app_->getTextureByName(app_->currentTexture().toStdString()));

  texturesData_.flipCheck->setChecked(currentTexture ? currentTexture->isFlipped() : false);

  // Materials
  materialsData_.materialList->updateMaterials();

  // Objects
  objectsData_.autoSelectCheck->setChecked(objectsData_.autoSelect);

  // Overview
  auto *overview = app_->overview();

  overviewData_.equalScale->setChecked(overview->isEqualScale());

  overviewData_.mouseTypeCombo ->setCurrentIndex(mouseTypeInd .typeToInd(overview->mouseType ()));
  overviewData_.selectTypeCombo->setCurrentIndex(selectTypeInd.typeToInd(overview->selectType()));

  overviewData_.modelTypeCombo->setCurrentIndex(
    overviewModelTypeInd.typeToInd(overview->modelType()));

  overviewData_.cameraCheck->setChecked(overview->isCameraVisible());
  overviewData_.lightsCheck->setChecked(overview->isLightsVisible());

  // Bones
  auto *bones = app_->bones();

  bonesData_.modelCheck      ->setChecked(bones->isShowModel());
  bonesData_.boneNodesCheck  ->setChecked(bones->isShowBoneNodes());
  bonesData_.pointJointsCheck->setChecked(bones->isShowPointJoints());
  bonesData_.onlyJointsCheck ->setChecked(bones->isOnlyJoints());

  // Animation
  animData_.timeEdit    ->setRange(animData_.animCombo->tmin(), animData_.animCombo->tmax());
  animData_.timeStepEdit->setValue(app_->animTimeStep());

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
                            this, SLOT(updateCurrentBone()));

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

  auto connectPoint4Edit = [&](CQPoint4DEdit *w, const char *slotName) {
    CQUtil::connectDisconnect(b, w, SIGNAL(editingFinished()), this, slotName);
  };

  auto connectColorEdit = [&](CQColorEdit *w, const char *slotName) {
    CQUtil::connectDisconnect(b, w, SIGNAL(colorChanged(const QColor &)), this, slotName);
  };

  auto connectTextureEdit = [&](CQCamera3DTextureChooser *w, const char *slotName) {
    CQUtil::connectDisconnect(b, w, SIGNAL(textureChanged()), this, slotName);
  };

  auto connectMaterialEdit = [&](CQCamera3DMaterialChooser *w, const char *slotName) {
    CQUtil::connectDisconnect(b, w, SIGNAL(materialChanged()), this, slotName);
  };

  // General
  connectCheckBox(generalData_.showWireframeCheck, SLOT(showWireframeSlot(int)));
  connectCheckBox(generalData_.showSolidCheck    , SLOT(showSolidSlot(int)));
  connectCheckBox(generalData_.showTexturedCheck , SLOT(showTexturedSlot(int)));
  connectCheckBox(generalData_.showPointsCheck   , SLOT(showPointsSlot(int)));

  connectCheckBox(generalData_.quadViewCheck, SLOT(quadViewSlot(int)));

  connectRealSpin(generalData_.pointSizeEdit, SLOT(pointSizeSlot(double)));
  connectRealSpin(generalData_.lineWidthEdit, SLOT(lineWidthSlot(double)));

  connectPointEdit(generalData_.cursorEdit, SLOT(cursorSlot()));

  connectCheckBox (generalData_.normalsPointsCheck, SLOT(normalsPointsSlot(int)));
  connectCheckBox (generalData_.normalsFacesCheck , SLOT(normalsFacesSlot(int)));
  connectRealSpin (generalData_.normalsSizeEdit   , SLOT(normalsSizeSlot(double)));
  connectColorEdit(generalData_.normalsColorEdit  , SLOT(normalsColorSlot(const QColor &)));

  connectCheckBox(generalData_.showBasisCheck, SLOT(showBasisSlot(int)));

  connectCheckBox(generalData_.showBBoxCheck  , SLOT(showBBoxSlot(int)));
  connectCheckBox(generalData_.bboxOrientCheck, SLOT(bboxOrientSlot(int)));

  connectCheckBox(generalData_.depthTestCheck, SLOT(depthTestSlot(int)));
  connectCheckBox(generalData_.cullFaceCheck , SLOT(cullSlot(int)));
  connectCheckBox(generalData_.frontFaceCheck, SLOT(frontFaceSlot(int)));

  // Camera
  connectCheckBox(cameraData_.showCheck   , SLOT(showCameraSlot(int)));
  connectCheckBox(cameraData_.eyeLineCheck, SLOT(showEyeLineSlot(int)));
  connectCheckBox(cameraData_.planesCheck , SLOT(showPlanesSlot(int)));

  CQUtil::connectDisconnect(b, cameraData_.cameraList, SIGNAL(currentItemChanged()),
                            this, SLOT(currentCameraSlot()));

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
  CQUtil::connectDisconnect(b, lightsData_.lightList, SIGNAL(currentItemChanged()),
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
  connectComboBox(mouseTypeCombo_ , SLOT(mouseTypeSlot(int)));
  connectComboBox(editTypeCombo_  , SLOT(editTypeSlot(int)));
  connectRealSpin(mouseScaleEdit_ , SLOT(mouseScaleSlot(double)));
  connectCheckBox(mouseBasisCheck_, SLOT(mouseBasisSlot(int)));

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

  connectMaterialEdit(selectionData_.materialNameChooser, SLOT(connectMaterialEdit()));

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
                            objectsData_.objectsList, SLOT(updateObjects()));

  CQUtil::connectDisconnect(b, objectsData_.objectsList, SIGNAL(currentItemChanged()),
                            this, SLOT(currentObjectChangedSlot()));
  connectCheckBox(objectsData_.autoSelectCheck, SLOT(objectAutoSelectSlot(int)));

  // Overview
  connectCheckBox(overviewData_.equalScale, SLOT(overviewEqualScaleSlot(int)));

  connectComboBox(overviewData_.mouseTypeCombo , SLOT(overviewMouseTypeSlot(int)));
  connectComboBox(overviewData_.selectTypeCombo, SLOT(overviewSelectTypeSlot(int)));

  connectComboBox(overviewData_.modelTypeCombo, SLOT(overviewModelTypeSlot(int)));

  connectCheckBox(overviewData_.cameraCheck, SLOT(overviewCameraSlot(int)));
  connectCheckBox(overviewData_.lightsCheck, SLOT(overviewLightsSlot(int)));

  // UV
  connectComboBox(uvData_.typeCombo, SLOT(uvTextureTypeSlot(int)));

  // Textures
  connectCheckBox(texturesData_.flipCheck, SLOT(textureFlipSlot(int)));

  // Bones
  connectCheckBox(bonesData_.modelCheck      , SLOT(bonesModelSlot(int)));
  connectCheckBox(bonesData_.boneNodesCheck  , SLOT(bonesBoneNodeSlot(int)));
  connectCheckBox(bonesData_.pointJointsCheck, SLOT(bonesPointJointsSlot(int)));
  connectCheckBox(bonesData_.onlyJointsCheck , SLOT(bonesOnlyJointsSlot(int)));

  connectPointEdit (bonesData_.translationEdit, SLOT(bonesTranslationSlot()));
  connectPoint4Edit(bonesData_.rotationEdit   , SLOT(bonesRotationSlot()));
  connectPointEdit (bonesData_.scaleEdit      , SLOT(bonesScaleSlot()));

  CQUtil::connectDisconnect(b, canvas, SIGNAL(objectsChanged()),
                            bonesData_.bonesList, SLOT(updateWidgets()));
  CQUtil::connectDisconnect(b, bonesData_.bonesList, SIGNAL(currentItemChanged()),
                            this, SLOT(currentBoneSlot()));

  // Animation
  CQUtil::connectDisconnect(b, animData_.animCombo, SIGNAL(animChanged()),
                            this, SLOT(animNameSlot()));
  CQUtil::connectDisconnect(b, animData_.timeEdit, SIGNAL(realValueChanged(double)),
                            this, SLOT(animTimeSlot(double)));
  CQUtil::connectDisconnect(b, animData_.timeStepEdit, SIGNAL(realValueChanged(double)),
                            this, SLOT(animTimeStepSlot(double)));
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
showTexturedSlot(int i)
{
  auto *canvas = app_->canvas();

  canvas->setTextured(i);

  canvas->update();
}

void
CQCamera3DControl::
showPointsSlot(int i)
{
  auto *canvas = app_->canvas();

  canvas->setPoints(i);

  canvas->update();
}

void
CQCamera3DControl::
quadViewSlot(int i)
{
  auto *canvas = app_->canvas();

  canvas->setQuadView(i);

  canvas->update();
}

void
CQCamera3DControl::
pointSizeSlot(double r)
{
  auto *canvas = app_->canvas();

  canvas->setPointSize(r);

  canvas->update();
}

void
CQCamera3DControl::
lineWidthSlot(double r)
{
  auto *canvas = app_->canvas();

  canvas->setLineWidth(r);

  canvas->update();
}

void
CQCamera3DControl::
cursorSlot()
{
  auto *canvas = app_->canvas();

  auto p = generalData_.cursorEdit->getValue();

  canvas->setCursor(p);

  canvas->updateBillboard();

  canvas->update();
}

void
CQCamera3DControl::
normalsPointsSlot(int i)
{
  auto *canvas = app_->canvas();
  auto *normals = canvas->getNormals();
  if (! normals) return;

  normals->setPointNormals(i);

  updateObjects();
}

void
CQCamera3DControl::
normalsFacesSlot(int i)
{
  auto *canvas = app_->canvas();
  auto *normals = canvas->getNormals();
  if (! normals) return;

  normals->setFaceNormals(i);

  updateObjects();
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
showBasisSlot(int i)
{
  auto *canvas = app_->canvas();

  canvas->setShowBasis(i);

  canvas->update();
}

void
CQCamera3DControl::
showBBoxSlot(int i)
{
  auto *canvas = app_->canvas();

  canvas->setShowBBox(i);

  canvas->update();
}

void
CQCamera3DControl::
bboxOrientSlot(int i)
{
  auto *canvas = app_->canvas();
  auto *bbox   = canvas->getBBoxOverlay();
  if (! bbox) return;

  bbox->setOriented(i);

  canvas->update();
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
editTypeSlot(int i)
{
  auto *canvas = app_->canvas();

  canvas->setEditType(editTypeInd.indToType(i));

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
mouseBasisSlot(int i)
{
  auto *canvas = app_->canvas();

  canvas->setMouseBasis(i);

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
  else if (selectType == CQCamera3DSelectType::EDGE) {
  }
  else if (selectType == CQCamera3DSelectType::POINT) {
    auto *vertex = canvas->currentVertex();

    if (vertex)
      vertex->setVisible(i);
  }

  updateObjects();
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
  else if (selectType == CQCamera3DSelectType::EDGE) {
  }
  else if (selectType == CQCamera3DSelectType::POINT) {
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
  auto *texture = app_->getTextureByName(textureName.toStdString());

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
  auto *texture = app_->getTextureByName(textureName.toStdString());

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
  auto *texture = app_->getTextureByName(textureName.toStdString());

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
  auto *texture = app_->getTextureByName(textureName.toStdString());

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
connectMaterialEdit()
{
  auto *canvas = app_->canvas();
  auto *edit = qobject_cast<CQCamera3DMaterialChooser *>(sender());

  auto materialName = edit->materialName();
  auto *material = app_->getMaterialByName(materialName.toStdString());

  auto *object = canvas->currentObject();
  auto *face   = canvas->currentFace();

  if      (object)
    object->setMaterialP(material);
  else if (face)
    face->setMaterialP(material);

  updateObjects();
}

void
CQCamera3DControl::
resetCameraSlot()
{
  auto *camera = getCamera();

  if (! camera->isPerspective())
    return;

  auto *canvas = app_->canvas();

  auto bbox = canvas->bbox();

  auto center  = bbox.getCenter();
  auto maxSize = bbox.getMaxSize();

  auto s2 = std::sqrt(2.0);

  auto maxSize1 = s2*maxSize + camera->near();

  auto origin = CVector3D(center.x, center.y, center.z);
  auto pos    = CVector3D(center.x, center.y, center.z + maxSize1);

  camera->setPosition(pos);

  camera->setPitch(0.0);
  camera->setYaw(-M_PI/2.0);
  camera->setRoll(0.0);

  camera->setOrigin(origin);
}

void
CQCamera3DControl::
topCameraSlot()
{
  auto *camera = getCamera();
  if (! camera->isPerspective()) return;

  auto *canvas = app_->canvas();

  auto bbox   = canvas->bbox();
  auto center = bbox.getCenter();
  auto size   = bbox.getMaxSize();

  camera->setPosition(CVector3D(center.x, center.y + 2*size, center.z));
  camera->setOrigin(CVector3D(center));

  camera->setPitch(-M_PI/2.0);
  camera->setYaw(M_PI/2.0);
  camera->setRoll(0.0);
}

void
CQCamera3DControl::
sideCameraSlot()
{
  auto *camera = getCamera();
  if (! camera->isPerspective()) return;

  auto *canvas = app_->canvas();

  auto bbox   = canvas->bbox();
  auto center = bbox.getCenter();
  auto size   = bbox.getMaxSize();

  camera->setPitch(0.0);
  camera->setYaw(M_PI/2.0);
  camera->setRoll(0.0);

  camera->setPosition(CVector3D(center.x, center.y, center.z + 2*size));
  camera->setOrigin(CVector3D(center));
}

void
CQCamera3DControl::
frontCameraSlot()
{
  auto *camera = getCamera();
  if (! camera->isPerspective()) return;

  auto *canvas = app_->canvas();

  auto bbox   = canvas->bbox();
  auto center = bbox.getCenter();
  auto size   = bbox.getMaxSize();

  camera->setPitch(0.0);
  camera->setYaw(0.0);
  camera->setRoll(0.0);

  camera->setPosition(CVector3D(center.x - 2*size, center.y, center.z));
  camera->setOrigin(CVector3D(center));
}

CQCamera3DCamera *
CQCamera3DControl::
getCamera() const
{
  auto *canvas = app_->canvas();

  return canvas->getCurrentCamera();
}

void
CQCamera3DControl::
currentCameraSlot()
{
  auto *camera = cameraData_.cameraList->getCurrentCamera();
  if (! camera) return;

  auto *canvas = app_->canvas();

  canvas->setCurrentCamera(camera);

  updateWidgets();
}

void
CQCamera3DControl::
currentLightSlot()
{
  auto *light = lightsData_.lightList->getCurrentLight();
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
addMaterialSlot()
{
  auto *material = CGeometry3DInst->createMaterial();

  auto *scene = app_->getScene();

  auto n = scene->getMaterials().size();

  material->setName("material." + std::to_string(n + 1));

  scene->addMaterial(material);

  app_->materialAdded();
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

  canvas->selectFaces(faces, /*clear*/true, /*update*/true);
}

void
CQCamera3DControl::
selectFacesSlot()
{
  auto *canvas = app_->canvas();

  auto *object = canvas->currentObject();
  if (! object) return;

  canvas->deselectAll();

  if (object) {
    auto &faces = object->getFaces();

    for (auto *face : faces)
      face->setSelected(true);
  }

  canvas->update();
}

void
CQCamera3DControl::
selectPointsSlot()
{
  auto *canvas = app_->canvas();

  auto *object = canvas->currentObject();
  auto *face   = canvas->currentFace();
  if (! object && ! face) return;

  canvas->deselectAll();

  if      (object) {
    auto &faces = object->getFaces();

    for (auto *face : faces) {
      auto &vertices = face->getVertices();

      for (auto &v : vertices) {
        auto &vertex = object->getVertex(v);

        vertex.setSelected(true);
      }
    }
  }
  else if (face) {
    auto &vertices = face->getVertices();

    for (auto &v : vertices) {
      auto &vertex = object->getVertex(v);

      vertex.setSelected(true);
    }
  }

  canvas->update();
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
selectDumpSlot()
{
  auto os = std::ofstream("dump.txt", std::ofstream::out);

  auto *canvas = app_->canvas();

  auto selectType = canvas->selectType();

  if      (selectType == CQCamera3DSelectType::OBJECT) {
  }
  else if (selectType == CQCamera3DSelectType::FACE) {
  }
  else if (selectType == CQCamera3DSelectType::POINT) {
    auto animName = app_->animName().toStdString();
    auto animTime = app_->animTime();

    auto objectNodeMatrices = app_->calcNodeMatrices();

    auto selectedVertices = canvas->getSelectedVertices();

    for (const auto &pv : selectedVertices) {
      auto *object = pv.first;

      auto *rootObject = object->getRootObject();
      auto *objectData = canvas->getObjectData(object);

      auto meshMatrix = CMatrix3DH(object->getMeshGlobalTransform());

      auto &nodeMatrices = objectNodeMatrices[rootObject->getInd()];

      object->updateNodesAnimationData(animName, animTime);

      for (const auto &v : pv.second) {
        const auto &vertex = object->getVertex(v);

        os << "Vertex: " << vertex.getInd() << "\n";

        const auto &model = vertex.getModel();
        auto p = meshMatrix*model;
        os << " Model: " << p << "\n";

        auto animPoint = app_->adjustAnimPoint(vertex, p, nodeMatrices);
        os << " Anim: " << animPoint << "\n";

        auto *buffer = objectData->buffer();

        auto bufferInd = buffer->mapInd(vertex.getInd());
        if (bufferInd < 0) continue;

        CQGLBuffer::PointData pointData;
        buffer->getPointData(bufferInd, pointData);

        if (pointData.point)
          os << " Point: " << pointData.point->point() << "\n";
        if (pointData.normal)
          os << " Normal: " << pointData.normal->point() << "\n";
        if (pointData.color)
          os << " Color: " << pointData.color->point() << "\n";
        if (pointData.texturePoint)
          os << " Texture: " << pointData.texturePoint->point() << "\n";
        if (pointData.boneId)
          os << " Bone: " << pointData.boneId->point() << "\n";
        if (pointData.boneWeight)
          os << " Weights: " << pointData.boneWeight->point() << "\n";

        auto *rootObject = object->getRootObject();

        const auto &jointData = vertex.getJointData();

        os << " Joints:\n";

        for (int i = 0; i < 4; ++i) {
          const auto &jointNodeData = jointData.nodeDatas[i];

          if (jointNodeData.node < 0 || jointNodeData.weight == 0.0)
            continue;

          int nodeId = rootObject->mapNodeIndex(jointNodeData.node);
          if (nodeId < 0) continue;

          auto node = rootObject->getNode(nodeId);

          os << "  " << node.name() << " " << nodeId << "(#" <<
            jointNodeData.node << ") " << jointNodeData.weight << "\n";
        }
      }
    }
  }
}

void
CQCamera3DControl::
objectSelectSlot()
{
  auto *canvas = app_->canvas();

  auto *object = objectsData_.objectsList->getObjectListSelected();
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

//---

void
CQCamera3DControl::
addCubeSlot()
{
  app_->canvas()->addCube();
}

void
CQCamera3DControl::
addCylinderSlot()
{
  app_->canvas()->addCylinder();
}

void
CQCamera3DControl::
addPyramidSlot()
{
  app_->canvas()->addPyramid();
}

void
CQCamera3DControl::
addSphereSlot()
{
  app_->canvas()->addSphere();
}

void
CQCamera3DControl::
addTorusSlot()
{
  app_->canvas()->addTorus();
}

//---

void
CQCamera3DControl::
loadModelSlot()
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
saveModelSlot()
{
  auto *scene = app_->getScene();

  app_->writeScene(scene);
}

void
CQCamera3DControl::
currentMaterialSlot()
{
  auto *material = materialsData_.materialList->getCurrentMaterial();
  if (! material) return;

  app_->setCurrentMaterial(QString::fromStdString(material->name()));

  connectSlots(false);

  auto setColorEdit = [](CQColorEdit *edit, const CGeomMaterial::OptColor &color) {
    if (color)
      edit->setColor(RGBAToQColor(*color));
    else
      edit->setColor(QColor());
  };

  auto setRealEdit = [](CQRealSpin *edit, const CGeomMaterial::OptReal &r) {
    if (r)
      edit->setValue(*r);
    else
      edit->setValue(0.0);
  };

  auto setTextureEdit = [](CQCamera3DTextureChooser *edit, const CGeomTexture *texture) {
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

  app_->materialChanged();
}

void
CQCamera3DControl::
materialSpecularSlot(const QColor &c)
{
  auto *material = materialsData_.materialList->getSelectedMaterial();
  if (! material) return;

  material->setSpecular(QColorToRGBA(c));

  updateObjects();

  app_->materialChanged();
}

void
CQCamera3DControl::
materialEmissionSlot(const QColor &c)
{
  auto *material = materialsData_.materialList->getSelectedMaterial();
  if (! material) return;

  material->setEmission(QColorToRGBA(c));

  updateObjects();

  app_->materialChanged();
}

void
CQCamera3DControl::
materialShininessSlot(double r)
{
  auto *material = materialsData_.materialList->getSelectedMaterial();
  if (! material) return;

  material->setShininess(r);

  updateObjects();

  app_->materialChanged();
}

void
CQCamera3DControl::
materialTransparencySlot(double r)
{
  auto *material = materialsData_.materialList->getSelectedMaterial();
  if (! material) return;

  material->setTransparency(r);

  updateObjects();

  app_->materialChanged();
}

void
CQCamera3DControl::
materialAmbientTextureSlot()
{
  auto *edit = qobject_cast<CQCamera3DTextureChooser *>(sender());

  auto textureName = edit->textureName();
  auto *texture = app_->getTextureByName(textureName.toStdString());

  auto *material = materialsData_.materialList->getSelectedMaterial();
  if (! material) return;

  material->setAmbientTexture(texture);

  updateObjects();

  app_->materialChanged();
}

void
CQCamera3DControl::
materialDiffuseTextureSlot()
{
  auto *edit = qobject_cast<CQCamera3DTextureChooser *>(sender());

  auto textureName = edit->textureName();
  auto *texture = app_->getTextureByName(textureName.toStdString());

  auto *material = materialsData_.materialList->getSelectedMaterial();
  if (! material) return;

  material->setDiffuseTexture(texture);

  updateObjects();

  app_->materialChanged();
}

void
CQCamera3DControl::
materialNormalTextureSlot()
{
  auto *edit = qobject_cast<CQCamera3DTextureChooser *>(sender());

  auto textureName = edit->textureName();
  auto *texture = app_->getTextureByName(textureName.toStdString());

  auto *material = materialsData_.materialList->getSelectedMaterial();
  if (! material) return;

  material->setNormalTexture(texture);

  updateObjects();

  app_->materialChanged();
}

void
CQCamera3DControl::
materialSpecularTextureSlot()
{
  auto *edit = qobject_cast<CQCamera3DTextureChooser *>(sender());

  auto textureName = edit->textureName();
  auto *texture = app_->getTextureByName(textureName.toStdString());

  auto *material = materialsData_.materialList->getSelectedMaterial();
  if (! material) return;

  material->setSpecularTexture(texture);

  updateObjects();

  app_->materialChanged();
}

void
CQCamera3DControl::
materialEmissiveTextureSlot()
{
  auto *edit = qobject_cast<CQCamera3DTextureChooser *>(sender());

  auto textureName = edit->textureName();
  auto *texture = app_->getTextureByName(textureName.toStdString());

  auto *material = materialsData_.materialList->getSelectedMaterial();
  if (! material) return;

  material->setEmissiveTexture(texture);

  updateObjects();

  app_->materialChanged();
}

void
CQCamera3DControl::
currentObjectChangedSlot()
{
  if (! objectsData_.autoSelect)
    return;

  auto *object = objectsData_.objectsList->getSelectedIndObject();
  if (! object) return;

  auto *canvas = app_->canvas();

  canvas->selectObject(object);
}

void
CQCamera3DControl::
objectAutoSelectSlot(int i)
{
  objectsData_.autoSelect = i;
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

void
CQCamera3DControl::
textureFlipSlot(int i)
{
  auto *currentTexture = dynamic_cast<CQCamera3DTexture *>(
    app_->getTextureByName(app_->currentTexture().toStdString()));
  if (! currentTexture) return;

  currentTexture->setFlipped(i);

  app_->textureAdded();
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
}

void
CQCamera3DControl::
bonesBoneNodeSlot(int i)
{
  auto *bones = app_->bones();

  bones->setShowBoneNodes(i);
}

void
CQCamera3DControl::
bonesPointJointsSlot(int i)
{
  auto *bones = app_->bones();

  bones->setShowPointJoints(i);
}

void
CQCamera3DControl::
bonesOnlyJointsSlot(int i)
{
  auto *bones = app_->bones();

  bones->setOnlyJoints(i);
}

void
CQCamera3DControl::
bonesTranslationSlot()
{
  auto *bones = app_->bones();

  auto p = bonesData_.translationEdit->getValue();

  auto *node = bonesData_.bonesList->currentBoneNode();
  if (! node) return;

  node->setLocalTranslation(CTranslate3D(p));

  bones->update();
}

void
CQCamera3DControl::
bonesRotationSlot()
{
  auto *bones = app_->bones();

  auto p = bonesData_.rotationEdit->getValue();

  auto *node = bonesData_.bonesList->currentBoneNode();
  if (! node) return;

  node->setLocalRotation(CRotate3D(p));

  bones->update();
}

void
CQCamera3DControl::
bonesScaleSlot()
{
  auto *bones = app_->bones();

  auto p = bonesData_.scaleEdit->getValue();

  auto *node = bonesData_.bonesList->currentBoneNode();
  if (! node) return;

  node->setLocalScale(CScale3D(p));

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

  bonesData_.nodeLabel->setText(QString::fromStdString(node->name()));

  QString childrenStr;

  for (auto &c : node->children()) {
    if (childrenStr != "")
      childrenStr += " ";

    childrenStr += QString::number(c);
  }

  bonesData_.childrenLabel->setText(childrenStr);

  bonesData_.jointCheck->setChecked(node->isJoint());

  bonesData_.objectLabel->setText(
    node->object() ? QString::fromStdString(node->object()->getName()) : "");

  bonesData_.translationEdit->setValue(node->localTranslation().point());
  bonesData_.rotationEdit   ->setValue(node->localRotation   ().point());
  bonesData_.scaleEdit      ->setValue(node->localScale      ().point());

  if (app_->animName() != "") {
    auto animName = app_->animName().toStdString();
    auto animTime = app_->animTime();

    if (const_cast<CGeomObject3D *>(node->rootObject())->
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

  app_->setCurrentBoneObject(node->rootObject()->getInd());
  app_->setCurrentBoneNode  (node->ind());
}

void
CQCamera3DControl::
animNameSlot()
{
  auto *canvas = app_->canvas();

  auto name = animData_.animCombo->animName();

  auto *object = canvas->currentObject();

  if (object) {
    auto *rootObject = object->getRootObject();

    rootObject->setAnimName(name.toStdString());
  }

  app_->setAnimName(name);

  animData_.timeEdit->setRange(animData_.animCombo->tmin(), animData_.animCombo->tmax());
}

void
CQCamera3DControl::
animTimeSlot(double t)
{
  app_->setAnimTime(t);
}

void
CQCamera3DControl::
animTimeStepSlot(double t)
{
  app_->setAnimTimeStep(t);
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
  auto step = std::max(app_->animTimeStep(), 1.0);

  auto min = animData_.timeEdit->minimum();
  auto max = animData_.timeEdit->maximum();

  auto v = animData_.timeEdit->value();

  v += (max - min)/step;

  if (v > max)
    v = min;

  animData_.timeEdit->setValue(v);
}

void
CQCamera3DControl::
updateCurrentBone()
{
  connectSlots(false);

  updateAnim();

  auto objId  = app_->currentBoneObject();
  auto nodeId = app_->currentBoneNode();

  bonesData_.bonesList->setCurrentBoneNode(objId, nodeId);

  updateBones();

  connectSlots(true);
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
