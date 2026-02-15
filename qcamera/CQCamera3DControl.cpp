#include <CQCamera3DControl.h>
#include <CQCamera3DCanvas.h>
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
#include <CQCamera3DUI.h>
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

#if 0
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
#endif

#if 0
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
#endif

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

  CQCamera3DUI ui(this, layout);

  //---

  mainTab_ = ui.startTab("main");

  //---

  // 3D

  ui.startTabPage("3D");

  ui.startTab("3D");

  //---

  // 3D/General
  ui.startTabPage("General");

  //---

  ui.startGroup("Show");

  ui.startFrame(/*horizontal*/true);

  generalData_.showWireframeCheck = ui.addCheck("Wireframe");
  generalData_.showSolidCheck     = ui.addCheck("Solid");
  generalData_.showTexturedCheck  = ui.addCheck("Textured");
  generalData_.showPointsCheck    = ui.addCheck("Points");

  ui.addStretch();

  ui.endFrame();

  generalData_.wireframeColorEdit = ui.addLabelEdit("Wireframe Color", new CQColorEdit);

  ui.endGroup();

  generalData_.quadViewCheck    = ui.addLabelEdit("Quad View"   , new QCheckBox);
  generalData_.debugCameraCheck = ui.addLabelEdit("Debug Camera", new QCheckBox);

  generalData_.pointSizeEdit = ui.addLabelEdit("Point Size", new CQRealSpin);
  generalData_.lineWidthEdit = ui.addLabelEdit("Line Width", new CQRealSpin);

  generalData_.cursorEdit = ui.addLabelEdit("Cursor", new CQPoint3DEdit);

  ui.startGroup("Normals");

  ui.startFrame(/*horizontal*/true);

  generalData_.normalsPointsCheck = ui.addCheck("Points");
  generalData_.normalsFacesCheck  = ui.addCheck("Faces" );

  ui.addStretch();

  ui.endFrame();

  generalData_.normalsSizeEdit  = ui.addLabelEdit("Size" , new CQRealSpin);
  generalData_.normalsColorEdit = ui.addLabelEdit("Color", new CQColorEdit);

  ui.endGroup();

  ui.startGroup("Basis");

  generalData_.showBasisCheck = ui.addCheck("Show");

  ui.endGroup();

  ui.startGroup("BBox");

  generalData_.showBBoxCheck   = ui.addCheck("Show");
  generalData_.bboxOrientCheck = ui.addCheck("Oriented");

  ui.endGroup();

  ui.startGroup("Options");

  generalData_.depthTestCheck = ui.addCheck("Depth Test");
  generalData_.cullFaceCheck  = ui.addCheck("Cull Face");
  generalData_.frontFaceCheck = ui.addCheck("Front Face");

  ui.endGroup();

  ui.addStretch();

  ui.endTabPage();

  //------

  // 3D/Camera
  ui.startTabPage("Camera");

  //---

  ui.startGroup("Show", /*horizontal*/true);

  cameraData_.showCheck    = ui.addCheck("Camera");
  cameraData_.eyeLineCheck = ui.addCheck("Eye Line");
  cameraData_.planesCheck  = ui.addCheck("Planes");

  ui.addStretch();

  ui.endGroup();

  cameraData_.cameraList = new CQCamera3DCameraList(canvas);

  ui.addWidget(cameraData_.cameraList);

  ui.startGroup("Angles");

  cameraData_.pitchEdit = ui.addLabelEdit("Pitch", new CQRealSpin);
  cameraData_.yawEdit   = ui.addLabelEdit("Yaw"  , new CQRealSpin);
  cameraData_.rollEdit  = ui.addLabelEdit("Roll" , new CQRealSpin);

  ui.endGroup();

  ui.startGroup("Z");

  cameraData_.nearEdit = ui.addLabelEdit("Near", new CQRealSpin);
  cameraData_.farEdit  = ui.addLabelEdit("Far" , new CQRealSpin);
  cameraData_.fovEdit  = ui.addLabelEdit("FOV" , new CQRealSpin);

  ui.endGroup();

  //---

  ui.startGroup("Origin");

  cameraData_.xOriginEdit = ui.addLabelEdit("X", new CQRealSpin);
  cameraData_.yOriginEdit = ui.addLabelEdit("Y", new CQRealSpin);
  cameraData_.zOriginEdit = ui.addLabelEdit("Z", new CQRealSpin);

  ui.endGroup();

  //---

  ui.startGroup("Position");

  cameraData_.xEdit = ui.addLabelEdit("X", new CQRealSpin);
  cameraData_.yEdit = ui.addLabelEdit("Y", new CQRealSpin);
  cameraData_.zEdit = ui.addLabelEdit("Z", new CQRealSpin);

  ui.endGroup();

  //---

  ui.startGroup("Eye Line");

  cameraData_.eyeZ1Edit = ui.addLabelEdit("Z1", new CQRealSpin);
  cameraData_.eyeZ2Edit = ui.addLabelEdit("Z2", new CQRealSpin);

  ui.endGroup();

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

    ui.addWidget(pframe);

    return data;
  };

  ui.startGroup("Rotate Around", /*horizontal*/true);

  cameraData_.aroundXButtons = plusMinusFrame("X");
  cameraData_.aroundYButtons = plusMinusFrame("Y");
  cameraData_.aroundZButtons = plusMinusFrame("Z");

  cameraData_.aroundDeltaEdit = new CQRealSpin;
  ui.addWidget(cameraData_.aroundDeltaEdit);

  ui.addStretch();

  ui.endGroup();

  ui.addStretch();

  ui.startFrame(/*horizontal*/true);

  ui.addButton("Reset", SLOT(resetCameraSlot()));
  ui.addButton("Top"  , SLOT(topCameraSlot()));
  ui.addButton("Side" , SLOT(sideCameraSlot()));
  ui.addButton("Front", SLOT(frontCameraSlot()));

  ui.addStretch();

  ui.endFrame();

  ui.endTabPage();

  //------

  // 3D/Lights
  ui.startTabPage("Lights");

  lightsData_.lightList = new CQCamera3DLightList(canvas);

  ui.addWidget(lightsData_.lightList);

  ui.startGroup("Globals");

  lightsData_.ambientColorEdit    = ui.addLabelEdit("Ambient Color", new CQColorEdit);
  lightsData_.ambientStrengthEdit = ui.addLabelEdit("Ambient Strength", new CQRealSpin);

  lightsData_.diffuseStrengthEdit  = ui.addLabelEdit("Diffuse Strength" , new CQRealSpin);
  lightsData_.emissiveStrengthEdit = ui.addLabelEdit("Emission Strength", new CQRealSpin);
  lightsData_.specularStrengthEdit = ui.addLabelEdit("Specular Strength", new CQRealSpin);

  lightsData_.fixedDiffuseCheck = ui.addLabelEdit("Fixed Diffuse", new QCheckBox);

  ui.endGroup();

  lightsData_.typeCombo = ui.addLabelEdit("Type", new QComboBox);
  lightsData_.typeCombo->addItems(lightTypeInd.names());

#if 0
  lightsData_.enabledCheck = ui.addLabelEdit("Enabled", new QCheckBox);
#endif

  lightsData_.positionEdit = ui.addLabelEdit("Position", new CQPoint3DEdit);

  lightsData_.colorEdit = ui.addLabelEdit("Color", new CQColorEdit);

  // direction light
  lightsData_.directionEdit = ui.addLabelEdit("Direction", new CQPoint3DEdit);

  // point light
  lightsData_.pointRadiusEdit = ui.addLabelEdit("Point Radius", new CQRealSpin);

  lightsData_.attenuation0Edit = ui.addLabelEdit("Constant Attenuation", new CQRealSpin);
  lightsData_.attenuation1Edit = ui.addLabelEdit("Linear Attenuation", new CQRealSpin);
  lightsData_.attenuation2Edit = ui.addLabelEdit("Quadratic Attenuation", new CQRealSpin);

  // spot light
  lightsData_.spotDirectionEdit = ui.addLabelEdit("Spot Direction", new CQPoint3DEdit);
  lightsData_.spotExponentEdit  = ui.addLabelEdit("Spot Exponent", new CQRealSpin);
  lightsData_.spotCutOffAngle   = ui.addLabelEdit("Spot Cut Off", new CQRealSpin);

  ui.addStretch();

  ui.startFrame(/*horizontal*/true);

  ui.addButton("Add Light"  , SLOT(addLightSlot()));
  ui.addButton("Reset Light", SLOT(resetLightSlot()));

  ui.addStretch();

  ui.endFrame();

  ui.endTabPage();

  //------

  // 3D/Axis
  ui.startTabPage("Axis");

  axisData_.xPosEdit = ui.addEdit(new QCheckBox("X"), new CQRealSpin);
  axisData_.yPosEdit = ui.addEdit(new QCheckBox("Y"), new CQRealSpin);
  axisData_.zPosEdit = ui.addEdit(new QCheckBox("Z"), new CQRealSpin);

  axisData_.gridCheck = ui.addCheck("Grid");

  ui.addStretch();

  ui.endTabPage();

  //------

  // 3D/Mouse
  ui.startTabPage("Mouse");

#if 0
  mouseTypeCombo_ = ui.addLabelEdit("Mouse Type", new QComboBox);
  mouseTypeCombo_->addItems(mouseTypeInd.names());

  editTypeCombo_ = ui.addLabelEdit("Edit Type", new QComboBox);
  editTypeCombo_->addItems(editTypeInd.names());
#endif

  mouseScaleEdit_ = ui.addLabelEdit("Scale", new CQRealSpin);

  mouseBasisCheck_ = ui.addLabelEdit("Basis", new QCheckBox);

  ui.addStretch();

  ui.endTabPage();

  //------

  // 3D/Selection
  ui.startTabPage("Selection");

#if 0
  selectionData_.typeCombo = ui.addLabelEdit("Select", new QComboBox);
  selectionData_.typeCombo->addItems(selectTypeInd.names());
#endif

  selectionData_.indLabel = ui.addLabelEdit("Name", new CQTextLabel);
  selectionData_.visCheck = ui.addLabelEdit("Visible", new QCheckBox);

  selectionData_.colorEdit = ui.addLabelEdit("Color", new CQColorEdit);

  //---

  ui.startTab("objData");

  ui.startTabPage("Geometry");

  selectionData_.centerEdit = ui.addLabelEdit("Center", new CQPoint3DEdit);
  selectionData_.sizeEdit   = ui.addLabelEdit("Size"  , new CQPoint3DEdit);

  selectionData_.meshMatrixEdit = new CQMatrix3D;
  ui.addWidget(selectionData_.meshMatrixEdit);

  ui.addStretch();

  ui.endTabPage();

  //--

  ui.startTabPage("Transform");

  selectionData_.translationEdit = ui.addLabelEdit("Translation", new CQPoint3DEdit);
  selectionData_.rotationEdit    = ui.addLabelEdit("Rotation"   , new CQPoint3DEdit);
  selectionData_.scaleEdit       = ui.addLabelEdit("Scale"      , new CQPoint3DEdit);

  selectionData_.matrixEdit = new CQMatrix3D;
  ui.addWidget(selectionData_.matrixEdit);

  ui.addStretch();

  ui.endTabPage();

  //--

  ui.startTabPage("Textures");

  selectionData_.diffuseTextureEdit =
    ui.addLabelEdit("Diffuse" , new CQCamera3DTextureChooser(app_));
  selectionData_.normalTextureEdit =
    ui.addLabelEdit("Normal"  , new CQCamera3DTextureChooser(app_));
  selectionData_.specularTextureEdit =
    ui.addLabelEdit("Specular", new CQCamera3DTextureChooser(app_));
  selectionData_.emissiveTextureEdit =
    ui.addLabelEdit("Emissive", new CQCamera3DTextureChooser(app_));

  ui.startFrame(/*horizontal*/true);

  ui.addButton("Add Texture"     , SLOT(addTextureSlot()));
  ui.addButton("Load Texture Map", SLOT(loadTextureMapSlot()));
  ui.addButton("Save Texture Map", SLOT(saveTextureMapSlot()));

  ui.addStretch();

  ui.endFrame();

  ui.addStretch();

  ui.endTabPage();

  //--

  ui.startTabPage("Material");

  selectionData_.materialNameChooser =
    ui.addLabelEdit("Name", new CQCamera3DMaterialChooser(app_));

  ui.startFrame(/*horizontal*/true);

  ui.addButton("Load Material Map", SLOT(loadMaterialMapSlot()));
  ui.addButton("Save Material Map", SLOT(saveMaterialMapSlot()));

  ui.addStretch();

  ui.endFrame();

  ui.addStretch();

  ui.endTabPage();

  ui.endTab();

  //---

  ui.startGroup("Swap");

  ui.startFrame(/*horizontal*/true);

  ui.addButton("XY", SLOT(swapSlot()));
  ui.addButton("YZ", SLOT(swapSlot()));
  ui.addButton("ZX", SLOT(swapSlot()));

  ui.addStretch();

  ui.endFrame();

  ui.endGroup();

  //---

  ui.startGroup("Invert");

  ui.startFrame(/*horizontal*/true);

  ui.addButton("X", SLOT(invertSlot()));
  ui.addButton("Y", SLOT(invertSlot()));
  ui.addButton("Z", SLOT(invertSlot()));

  ui.addStretch();

  ui.endFrame();

  ui.endGroup();

  //---

  ui.startGroup("Details");

  selectionData_.objectInfoText = new QTextEdit;

  selectionData_.objectInfoText->setReadOnly(true);

  ui.addWidget(selectionData_.objectInfoText);

  //ui.addStretch();

  ui.endGroup();

  //---

  ui.startGroup("Select", /*horizontal*/true);

  ui.addButton("Parent"     , SLOT(selectParentSlot()));
  ui.addButton("By Material", SLOT(selectRelatedSlot()));
  ui.addButton("Faces"      , SLOT(selectFacesSlot()));
  ui.addButton("Points"     , SLOT(selectPointsSlot()));
  ui.addButton("Clear"      , SLOT(deselectSlot()));
  ui.addButton("Dump"       , SLOT(selectDumpSlot()));

  ui.addStretch();

  ui.endGroup();

  ui.endTabPage();

  //------

  // 3D/Objects
  ui.startTabPage("Objects");

  objectsData_.objectsList = new CQCamera3DObjectsList(canvas);

  ui.addWidget(objectsData_.objectsList);

  ui.startFrame(/*horizontal*/true);

  objectsData_.autoSelectCheck = ui.addLabelEdit("Auto Select", new QCheckBox);

  ui.addButton("Select", SLOT(objectSelectSlot()));
  ui.addButton("Zoom"  , SLOT(objectZoomSlot()));

  ui.addStretch();

  ui.endFrame();

  ui.startGroup("Add");

  ui.startFrame(/*horizontal*/true);

  ui.addButton("Cone"    , SLOT(addConeSlot()));
  ui.addButton("Cube"    , SLOT(addCubeSlot()));
  ui.addButton("Cylinder", SLOT(addCylinderSlot()));
  ui.addButton("Pyramid" , SLOT(addPyramidSlot()));
  ui.addButton("Sphere"  , SLOT(addSphereSlot()));
  ui.addButton("Torus"   , SLOT(addTorusSlot()));

  ui.addButton("Load", SLOT(loadModelSlot()));
  ui.addButton("Save", SLOT(saveModelSlot()));

  ui.addStretch();

  ui.endFrame();

  ui.endGroup();

  ui.endTabPage();

  ui.endTabPage();

  ui.endTab();

  //------

  // Overview

  ui.startTabPage("Overview");

  overviewData_.equalScale = ui.addLabelEdit("Equal Scale", new QCheckBox);

#if 0
  overviewData_.mouseTypeCombo = ui.addLabelEdit("Mouse", new QComboBox);
  overviewData_.mouseTypeCombo->addItems(mouseTypeInd.names());
#endif

#if 0
  overviewData_.selectTypeCombo = ui.addLabelEdit("Select", new QComboBox);
  overviewData_.selectTypeCombo->addItems(selectTypeInd.names());
#endif

  ui.startGroup("Model");

  overviewData_.modelTypeCombo = ui.addLabelEdit("Type", new QComboBox);
  overviewData_.modelTypeCombo->addItems(overviewModelTypeInd.names());

  ui.endGroup();

  ui.startGroup("Camera");

  overviewData_.cameraCheck = ui.addLabelEdit("Visible", new QCheckBox);

  ui.endGroup();

  ui.startGroup("Lights");

  overviewData_.lightsCheck = ui.addLabelEdit("Visible", new QCheckBox);

  ui.endGroup();

  ui.addStretch();

  ui.endTabPage();

  //------

  // UV

  ui.startTabPage("UV");

  uvData_.typeCombo = ui.addLabelEdit("Texture", new QComboBox);
  uvData_.typeCombo->addItems(uvTextureTypeInd.names());

  uvData_.wrapCheck = ui.addLabelEdit("Wrap", new QCheckBox);

  ui.addStretch();

  ui.endTabPage();

  //------

  // Textures

  ui.startTabPage("Textures");

  texturesData_.textureList = new CQCamera3DTextureList(canvas);

  ui.addWidget(texturesData_.textureList);

  ui.startGroup("Current");

#if 0
  texturesData_.flipCheck = ui.addLabelEdit("Flip", new QCheckBox);
#endif
  texturesData_.wrapCheck = ui.addLabelEdit("Wrap", new QCheckBox);

  ui.endGroup();

  ui.startFrame(/*horizontal*/true);

  ui.addButton("Add Texture", SLOT(addTextureSlot()));

  ui.addStretch();

  ui.endFrame();

  ui.addStretch();

  ui.endTabPage();

  //------

  // Materials

  ui.startTabPage("Materials");

  materialsData_.materialList = new CQCamera3DMaterialList(app_);

  ui.addWidget(materialsData_.materialList);

  ui.startGroup("General");

  materialsData_.twoSidedCheck = ui.addLabelEdit("Two Sided", new QCheckBox);

  materialsData_.shadingCombo = ui.addLabelEdit("Shading", new QComboBox);
  materialsData_.shadingCombo->addItems(materialShadingInd.names());

  ui.endGroup();

  ui.startGroup("Colors");

  materialsData_.ambientEdit  = ui.addLabelEdit("Ambient" , new CQColorEdit);
  materialsData_.diffuseEdit  = ui.addLabelEdit("Diffuse" , new CQColorEdit);
  materialsData_.specularEdit = ui.addLabelEdit("Specular", new CQColorEdit);
  materialsData_.emissionEdit = ui.addLabelEdit("Emission", new CQColorEdit);

  materialsData_.shininessEdit = ui.addLabelEdit("Shininess", new CQRealSpin);

  materialsData_.transparencyEdit = ui.addLabelEdit("Transparency", new CQRealSpin);

  ui.endGroup();

  ui.startGroup("Textures");

  materialsData_.ambientTextureEdit =
    ui.addLabelEdit("Ambient" , new CQCamera3DTextureChooser(app_));
  materialsData_.diffuseTextureEdit =
    ui.addLabelEdit("Diffuse" , new CQCamera3DTextureChooser(app_));
  materialsData_.normalTextureEdit =
    ui.addLabelEdit("Normal"  , new CQCamera3DTextureChooser(app_));
  materialsData_.specularTextureEdit =
    ui.addLabelEdit("Specular", new CQCamera3DTextureChooser(app_));
  materialsData_.emissiveTextureEdit =
    ui.addLabelEdit("Emissive", new CQCamera3DTextureChooser(app_));

  ui.endGroup();

  ui.startFrame(/*horizontal*/true);

  ui.addButton("Add Material", SLOT(addMaterialSlot()));

  ui.addStretch();

  ui.endFrame();

  ui.endTabPage();

  //------

  // Bones

  ui.startTabPage("Bones");

  ui.startGroup("Show");

  bonesData_.modelCheck       = ui.addLabelEdit("Model", new QCheckBox);
  bonesData_.boneNodesCheck   = ui.addLabelEdit("Bone Nodes", new QCheckBox);
  bonesData_.pointJointsCheck = ui.addLabelEdit("Point Joints", new QCheckBox);
  bonesData_.onlyJointsCheck  = ui.addLabelEdit("Only Joints", new QCheckBox);

  ui.endGroup();

  bonesData_.bonesList = new CQCamera3DBonesList(app_);
  ui.addWidget(bonesData_.bonesList);

  bonesData_.nodeLabel = ui.addLabelEdit("Node", new CQTextLabel);

  bonesData_.childrenLabel = ui.addLabelEdit("Children", new CQTextLabel);

  bonesData_.jointCheck = ui.addLabelEdit("Is Joint", new QCheckBox);

  bonesData_.objectLabel = ui.addLabelEdit("Object", new CQTextLabel);

  ui.startGroup("Default");

  bonesData_.translationEdit = ui.addLabelEdit("Translation", new CQPoint3DEdit);
  bonesData_.rotationEdit    = ui.addLabelEdit("Rotation"   , new CQPoint4DEdit);
  bonesData_.scaleEdit       = ui.addLabelEdit("Scale"      , new CQPoint3DEdit);

  ui.endGroup();

  ui.startGroup("Animation");

  bonesData_.animTranslationEdit = ui.addLabelEdit("Translation", new CQPoint3DEdit);
  bonesData_.animRotationEdit    = ui.addLabelEdit("Rotation"   , new CQPoint4DEdit);
  bonesData_.animScaleEdit       = ui.addLabelEdit("Scale"      , new CQPoint3DEdit);

  ui.endGroup();

  //ui.addStretch();

  ui.endTabPage();

  //------

  // Animation

  ui.startTabPage("Animation");

  animData_.animCombo    = ui.addLabelEdit("Name", new CQCamera3DAnimChooser(app_));
  animData_.timeEdit     = ui.addLabelEdit("Time", new CQRealSpin);
  animData_.timeStepEdit = ui.addLabelEdit("Step", new CQRealSpin);

  auto addToolButton = [&](const QString &name, const QString &iconName,
                           const QString &tip, const char *slotName) {
    auto *button = new CQIconButton;

    button->setObjectName(name);
    button->setIcon(iconName);
    button->setIconSize(QSize(32, 32));
    button->setAutoRaise(true);
    button->setToolTip(tip);

    connect(button, SIGNAL(clicked()), this, slotName);

    ui.addWidget(button);

    return button;
  };

  ui.startFrame(/*horizontal*/true);

  animData_.playButton  = addToolButton("play" , "PLAY"    , "Play" , SLOT(playSlot()));
  animData_.pauseButton = addToolButton("pause", "PAUSE"   , "Pause", SLOT(pauseSlot()));
  animData_.stepButton  = addToolButton("step" , "PLAY_ONE", "Step" , SLOT(stepSlot()));

  ui.addStretch();

  ui.endFrame();

  animData_.nodeLabel = ui.addLabelEdit("Node", new CQTextLabel);

  auto createTableWidget = []() {
    auto *table = new QTableWidget;

    table->horizontalHeader()->setStretchLastSection(true);

    return table;
  };

  ui.startGroup("Translation");

  animData_.translationInterpCombo = ui.addLabelEdit("Interpolation", new QComboBox);
  animData_.translationInterpCombo->addItems(animInterpolationInd.names());

  animData_.animTranslationTable = createTableWidget();
  ui.addWidget(animData_.animTranslationTable);

  ui.endGroup();

  ui.startGroup("Rotation");

  animData_.rotationInterpCombo = ui.addLabelEdit("Interpolation", new QComboBox);
  animData_.rotationInterpCombo->addItems(animInterpolationInd.names());

  animData_.animRotationTable = createTableWidget();
  ui.addWidget(animData_.animRotationTable);

  ui.endGroup();

  ui.startGroup("Scale");

  animData_.scaleInterpCombo = ui.addLabelEdit("Interpolation", new QComboBox);
  animData_.scaleInterpCombo->addItems(animInterpolationInd.names());

  animData_.animScaleTable = createTableWidget();
  ui.addWidget(animData_.animScaleTable);

  ui.endGroup();

  ui.endTabPage();

  //------

  ui.endTab();

  connectSlots(true);

  updateWidgets();

  ui.validate();
}

void
CQCamera3DControl::
updateWidgets()
{
  connectSlots(false);

  auto *canvas = app_->canvas();

  auto *bbox = canvas->getBBoxOverlay();

  // General
  generalData_.showWireframeCheck->setChecked(canvas->isWireframe());
  generalData_.showSolidCheck    ->setChecked(canvas->isSolid());
  generalData_.showTexturedCheck ->setChecked(canvas->isTextured());
  generalData_.showPointsCheck   ->setChecked(canvas->isPoints());

  generalData_.wireframeColorEdit->setColor(RGBAToQColor(canvas->wireframeColor()));

  generalData_.quadViewCheck   ->setChecked(canvas->isQuadView());
  generalData_.debugCameraCheck->setChecked(canvas->isDebugCamera());

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
  updateCameraWidgets(/*disconnect*/false);

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

#if 0
  lightsData_.enabledCheck->setChecked(light->getEnabled());
#endif

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
    axisData_.xPosEdit.first->setChecked(axes->isShowX());
    axisData_.yPosEdit.first->setChecked(axes->isShowY());
    axisData_.zPosEdit.first->setChecked(axes->isShowZ());

    axisData_.xPosEdit.second->setValue(axes->xpos());
    axisData_.yPosEdit.second->setValue(axes->ypos());
    axisData_.zPosEdit.second->setValue(axes->zpos());

    axisData_.gridCheck->setChecked(axes->isShowGrid());
  }

  // Mouse
#if 0
  mouseTypeCombo_->setCurrentIndex(mouseTypeInd.typeToInd(canvas->mouseType()));
  editTypeCombo_ ->setCurrentIndex(editTypeInd .typeToInd(canvas->editType ()));
#endif

  mouseScaleEdit_->setValue(canvas->mouseScale());

  mouseBasisCheck_->setChecked(canvas->isMouseBasis());

  // Selection
#if 0
  selectionData_.typeCombo->setCurrentIndex(selectTypeInd.typeToInd(canvas->selectType()));
#endif

  auto selectType = canvas->calcSelectType();

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
      auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);

      auto bbox = object1->bbox();

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

      selectionData_.translationEdit->setValue(object1->translationValues());
      selectionData_.rotationEdit   ->setValue(object1->rotationValuesDeg());
      selectionData_.scaleEdit      ->setValue(object1->scaleValues());

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
      auto *object = vertex->getObject();

      auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);

      auto *buffer = object1->buffer();

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
  updateTextureWidgets(/*disconnect*/false);

  // Materials
  materialsData_.materialList->updateMaterials();

  // Objects
  objectsData_.autoSelectCheck->setChecked(objectsData_.autoSelect);

  // Overview
  auto *overview = app_->overview();

  overviewData_.equalScale->setChecked(overview->isEqualScale());

#if 0
  overviewData_.mouseTypeCombo ->setCurrentIndex(mouseTypeInd .typeToInd(overview->mouseType ()));
  overviewData_.selectTypeCombo->setCurrentIndex(selectTypeInd.typeToInd(overview->selectType()));
#endif

  overviewData_.modelTypeCombo->setCurrentIndex(
    overviewModelTypeInd.typeToInd(overview->modelType()));

  overviewData_.cameraCheck->setChecked(overview->isCameraVisible());
  overviewData_.lightsCheck->setChecked(overview->isLightsVisible());

  // UV
  auto *uvMap = app_->uvMap();

  uvData_.wrapCheck->setChecked(uvMap->isWrapValues());

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
updateTextureWidgets(bool disconnect)
{
  if (disconnect)
    connectSlots(false);

  auto *currentTexture = dynamic_cast<CQCamera3DTexture *>(
    app_->getTextureById(app_->currentTextureId()));

#if 0
  texturesData_.flipCheck->setChecked(currentTexture ? currentTexture->isFlipped() : false);
#endif
  texturesData_.wrapCheck->setChecked(currentTexture ? currentTexture->isWrapped() : false);

  if (disconnect)
    connectSlots(true);
}

void
CQCamera3DControl::
updateCameraWidgets(bool disconnect)
{
  if (disconnect)
    connectSlots(false);

  auto *canvas = app_->canvas();
  auto *camera = getCamera();

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

  if (disconnect)
    connectSlots(true);
}

void
CQCamera3DControl::
connectSlots(bool b)
{
  assert(b != connected_);

  connected_ = b;

  CQUtil::connectDisconnect(b, app_, SIGNAL(timerStep()),
                            this, SLOT(timerSlot()));
  CQUtil::connectDisconnect(b, app_, SIGNAL(viewTypeChanged()),
                            this, SLOT(viewTypeSlot()));

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

  connectColorEdit(generalData_.wireframeColorEdit, SLOT(wireframeColorSlot(const QColor &)));

  connectCheckBox(generalData_.quadViewCheck   , SLOT(quadViewSlot(int)));
  connectCheckBox(generalData_.debugCameraCheck, SLOT(debugCameraSlot(int)));

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

  connectRealSpin(cameraData_.aroundDeltaEdit, SLOT(aroundDeltaSlot(double)));

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
#if 0
  connectCheckBox (lightsData_.enabledCheck     , SLOT(lightEnabledSlot(int)));
#endif
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
  connectCheckBox(axisData_.xPosEdit.first, SLOT(showXAxesSlot(int)));
  connectCheckBox(axisData_.yPosEdit.first, SLOT(showYAxesSlot(int)));
  connectCheckBox(axisData_.zPosEdit.first, SLOT(showZAxesSlot(int)));

  connectRealSpin(axisData_.xPosEdit.second, SLOT(axesXPosSlot(double)));
  connectRealSpin(axisData_.yPosEdit.second, SLOT(axesYPosSlot(double)));
  connectRealSpin(axisData_.zPosEdit.second, SLOT(axesZPosSlot(double)));

  connectCheckBox(axisData_.gridCheck, SLOT(axesGridSlot(int)));

  // Mouse
#if 0
  connectComboBox(mouseTypeCombo_ , SLOT(mouseTypeSlot(int)));
  connectComboBox(editTypeCombo_  , SLOT(editTypeSlot(int)));
#endif
  connectRealSpin(mouseScaleEdit_ , SLOT(mouseScaleSlot(double)));
  connectCheckBox(mouseBasisCheck_, SLOT(mouseBasisSlot(int)));

  // Selection
#if 0
  connectComboBox(selectionData_.typeCombo, SLOT(selectTypeSlot(int)));
#endif
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

#if 0
  connectComboBox(overviewData_.mouseTypeCombo , SLOT(overviewMouseTypeSlot(int)));
  connectComboBox(overviewData_.selectTypeCombo, SLOT(overviewSelectTypeSlot(int)));
#endif

  connectComboBox(overviewData_.modelTypeCombo, SLOT(overviewModelTypeSlot(int)));

  connectCheckBox(overviewData_.cameraCheck, SLOT(overviewCameraSlot(int)));
  connectCheckBox(overviewData_.lightsCheck, SLOT(overviewLightsSlot(int)));

  // UV
  connectComboBox(uvData_.typeCombo, SLOT(uvTextureTypeSlot(int)));
  connectCheckBox(uvData_.wrapCheck, SLOT(uvTextureWrapSlot(int)));

  // Textures
  CQUtil::connectDisconnect(b, app_, SIGNAL(currentTextureChanged()),
                            this, SLOT(updateTextureWidgets()));

#if 0
  connectCheckBox(texturesData_.flipCheck, SLOT(textureFlipSlot(int)));
#endif
  connectCheckBox(texturesData_.wrapCheck, SLOT(textureWrapSlot(int)));

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

  app_->setCurrentView(app_->indToViewType(i));

  connectSlots(true);
}

void
CQCamera3DControl::
viewTypeSlot()
{
  connectSlots(false);

  viewType_ = app_->viewType();

  mainTab_->setCurrentIndex(app_->viewTypeToInd(viewType_));

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
wireframeColorSlot(const QColor &c)
{
  auto *canvas = app_->canvas();

  canvas ->setWireframeColor(QColorToRGBA(c));

  updateObjects();
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
debugCameraSlot(int i)
{
  auto *canvas = app_->canvas();

  canvas->setDebugCamera(i);

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

#if 0
void
CQCamera3DControl::
mouseTypeSlot(int i)
{
  auto *canvas = app_->canvas();

  canvas->setMouseType(mouseTypeInd.indToType(i));

  updateWidgets();
}
#endif

#if 0
void
CQCamera3DControl::
editTypeSlot(int i)
{
  auto *canvas = app_->canvas();

  canvas->setEditType(editTypeInd.indToType(i));

  updateWidgets();
}
#endif

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

#if 0
void
CQCamera3DControl::
selectTypeSlot(int i)
{
  auto *canvas = app_->canvas();

  canvas->setSelectType(selectTypeInd.indToType(i));

  updateWidgets();
}
#endif

void
CQCamera3DControl::
objectVisSlot(int i)
{
  auto *canvas = app_->canvas();

  auto selectType = canvas->calcSelectType();

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
  if (! camera) return;

  camera->setPitch(r);
}

void
CQCamera3DControl::
yawSlot(double r)
{
  auto *camera = getCamera();
  if (! camera) return;

  camera->setYaw(r);
}

void
CQCamera3DControl::
rollSlot(double r)
{
  auto *camera = getCamera();
  if (! camera) return;

  camera->setRoll(r);
}

void
CQCamera3DControl::
nearSlot(double r)
{
  auto *camera = getCamera();
  if (! camera) return;

  camera->setNear(r);
}

void
CQCamera3DControl::
farSlot(double r)
{
  auto *camera = getCamera();
  if (! camera) return;

  camera->setFar(r);
}

void
CQCamera3DControl::
xOriginSlot(double r)
{
  auto *camera = getCamera();
  if (! camera) return;

  auto p = camera->origin();

  p.setX(r);
  camera->setOrigin(p);
}

void
CQCamera3DControl::
yOriginSlot(double r)
{
  auto *camera = getCamera();
  if (! camera) return;

  auto p = camera->origin();

  p.setY(r);
  camera->setOrigin(p);
}

void
CQCamera3DControl::
zOriginSlot(double r)
{
  auto *camera = getCamera();
  if (! camera) return;

  auto p = camera->origin();

  p.setZ(r);
  camera->setOrigin(p);
}

void
CQCamera3DControl::
xSlot(double r)
{
  auto *camera = getCamera();
  if (! camera) return;

  auto p = camera->position();

  p.setX(r);
  camera->setPosition(p);
}

void
CQCamera3DControl::
ySlot(double r)
{
  auto *camera = getCamera();
  if (! camera) return;

  auto p = camera->position();

  p.setY(r);
  camera->setPosition(p);
}

void
CQCamera3DControl::
zSlot(double r)
{
  auto *camera = getCamera();
  if (! camera) return;

  auto p = camera->position();

  p.setZ(r);
  camera->setPosition(p);
}

void
CQCamera3DControl::
fovSlot(double r)
{
  auto *camera = getCamera();
  if (! camera) return;

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
  if (! camera) return;

  auto d = cameraData_.aroundDelta;

  if (d < 0)
    d = canvas->bbox().getMaxSize()/30.0;

  camera->moveAroundX(-d);
}

void
CQCamera3DControl::
aroundXSlot2()
{
  auto *canvas = app_->canvas();
  auto *camera = getCamera();
  if (! camera) return;

  auto d = cameraData_.aroundDelta;

  if (d < 0)
    d = canvas->bbox().getMaxSize()/30.0;

  camera->moveAroundX(d);
}

void
CQCamera3DControl::
aroundYSlot1()
{
  auto *canvas = app_->canvas();
  auto *camera = getCamera();
  if (! camera) return;

  auto d = cameraData_.aroundDelta;

  if (d < 0)
    d = canvas->bbox().getMaxSize()/30.0;

  camera->moveAroundY(-d);
}

void
CQCamera3DControl::
aroundYSlot2()
{
  auto *canvas = app_->canvas();
  auto *camera = getCamera();
  if (! camera) return;

  auto d = cameraData_.aroundDelta;

  if (d < 0)
    d = canvas->bbox().getMaxSize()/30.0;

  camera->moveAroundY(d);
}

void
CQCamera3DControl::
aroundZSlot1()
{
  auto *canvas = app_->canvas();
  auto *camera = getCamera();
  if (! camera) return;

  auto d = cameraData_.aroundDelta;

  if (d < 0)
    d = canvas->bbox().getMaxSize()/30.0;

  camera->moveAroundZ(-d);
}

void
CQCamera3DControl::
aroundZSlot2()
{
  auto *canvas = app_->canvas();
  auto *camera = getCamera();
  if (! camera) return;

  auto d = cameraData_.aroundDelta;

  if (d < 0)
    d = canvas->bbox().getMaxSize()/30.0;

  camera->moveAroundZ(d);
}

void
CQCamera3DControl::
aroundDeltaSlot(double d)
{
  cameraData_.aroundDelta = d;
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
    auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);

    auto p = selectionData_.translationEdit->getValue();

    object1->setTranslationValues(p);

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
    auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);

    auto p = selectionData_.rotationEdit->getValue();

    object1->setRotationValuesDeg(p);

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
    auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);

    auto p = selectionData_.scaleEdit->getValue();

    object1->setScaleValues(p);

    updateObjects();
  }
}
#endif

void
CQCamera3DControl::
objectColorSlot(const QColor &c)
{
  auto *canvas = app_->canvas();

  auto selectType = canvas->calcSelectType();

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
  if (! camera) return;

  if (! camera->isPerspective())
    return;

  auto *canvas = app_->canvas();

  canvas->resetCamera(camera);
}

void
CQCamera3DControl::
topCameraSlot()
{
  auto *camera = getCamera();
  if (! camera) return;

  if (! camera->isPerspective())
    return;

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
  if (! camera) return;

  if (! camera->isPerspective())
    return;

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
  if (! camera) return;

  if (! camera->isPerspective())
    return;

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

CGLCameraIFace *
CQCamera3DControl::
getCamera() const
{
#if 0
  auto *canvas = app_->canvas();

  return canvas->getCurrentCamera(/*view*/false);
#else
  return cameraData_.cameraList->getCurrentCamera();
#endif
}

void
CQCamera3DControl::
currentCameraSlot()
{
  //auto *camera = cameraData_.cameraList->getCurrentCamera();
  //if (! camera) return;

  //auto *canvas = app_->canvas();
  //canvas->setCurrentCamera(camera);

  updateCameraWidgets();
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
showXAxesSlot(int i)
{
  auto *canvas = app_->canvas();
  auto *axes   = canvas->axes();

  if (axes)
    axes->setShowX(i);

  canvas->update();
}

void
CQCamera3DControl::
showYAxesSlot(int i)
{
  auto *canvas = app_->canvas();
  auto *axes   = canvas->axes();

  if (axes)
    axes->setShowY(i);

  canvas->update();
}

void
CQCamera3DControl::
showZAxesSlot(int i)
{
  auto *canvas = app_->canvas();
  auto *axes   = canvas->axes();

  if (axes)
    axes->setShowZ(i);

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
axesGridSlot(int i)
{
  auto *canvas = app_->canvas();
  auto *axes   = canvas->axes();

  if (axes)
    axes->setShowGrid(i);

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
    canvas->selectObject(object->parent(), /*clear*/true);
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

  auto selectType = canvas->calcSelectType();

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

      auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);

      auto *rootObject = object->getRootObject();

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

        auto *buffer = object1->buffer();

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

  canvas->selectObject(object, /*clear*/true);
}

void
CQCamera3DControl::
objectZoomSlot()
{
  auto *canvas = app_->canvas();
  auto *object = canvas->currentObject();
  if (! object) return;

  auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);

  auto bbox = object1->bbox();

  auto *camera = getCamera();
  if (! camera) return;

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
addConeSlot()
{
  app_->canvas()->addCone();
}

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

  canvas->selectObject(object, /*clear*/true);
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

#if 0
void
CQCamera3DControl::
overviewSelectTypeSlot(int i)
{
  auto *overview = app_->overview();

  overview->setSelectType(selectTypeInd.indToType(i));
}
#endif

#if 0
void
CQCamera3DControl::
overviewMouseTypeSlot(int i)
{
  auto *overview = app_->overview();

  overview->setMouseType(mouseTypeInd.indToType(i));
}
#endif

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
uvTextureWrapSlot(int i)
{
  auto *uvMap = app_->uvMap();

  uvMap->setWrapValues(i);
}

#if 0
void
CQCamera3DControl::
textureFlipSlot(int i)
{
  auto *currentTexture = dynamic_cast<CQCamera3DTexture *>(
    app_->getTextureById(app_->currentTextureId()));
  if (! currentTexture) return;

  currentTexture->setFlipped(i);

  app_->textureAdded();
}
#endif

void
CQCamera3DControl::
textureWrapSlot(int i)
{
  auto *currentTexture = dynamic_cast<CQCamera3DTexture *>(
    app_->getTextureById(app_->currentTextureId()));
  if (! currentTexture) return;

  currentTexture->setWrapped(i);

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
