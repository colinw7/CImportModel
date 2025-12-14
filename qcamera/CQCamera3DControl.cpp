#include <CQCamera3DControl.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DOverview.h>
#include <CQCamera3DCamera.h>
#include <CQCamera3DGeomObject.h>
#include <CQCamera3DStatus.h>
#include <CQCamera3DAxes.h>
#include <CQCamera3DObjectsList.h>
#include <CQCamera3DTextureChooser.h>
#include <CQCamera3DUtil.h>

#include <CImportBase.h>
#include <CGeometry3D.h>
#include <CGeomScene3D.h>
#include <CGeomObject3D.h>
#include <CGeomFace3D.h>

#include <CGeomCube3D.h>
#include <CGeomCylinder3D.h>
#include <CGeomPyramid3D.h>
#include <CGeomSphere3D.h>
#include <CGeomTorus3D.h>
#include <CQGLTexture.h>

#include <CQMatrix3D.h>
#include <CQPoint3DEdit.h>
#include <CQRealSpin.h>
#include <CQColorEdit.h>
#include <CQTextLabel.h>
#include <CQUtil.h>
#include <CStrUtil.h>

#include <QFileDialog>
#include <QTextEdit>
#include <QGroupBox>
#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QMouseEvent>

namespace {

class SelectTypeInd : public ValueMap<CQCamera3DCanvas::SelectType, int> {
 public:
  SelectTypeInd() {
    add("Object", CQCamera3DCanvas::SelectType::OBJECT, 0);
    add("Face"  , CQCamera3DCanvas::SelectType::FACE  , 1);
    add("Edge"  , CQCamera3DCanvas::SelectType::EDGE  , 2);
    add("Point" , CQCamera3DCanvas::SelectType::POINT , 3);
  };

  CQCamera3DCanvas::SelectType indToSelectType(int ind) {
    return lookup(ind);
  }

  int selectTypeToInd(const CQCamera3DCanvas::SelectType &type) {
    return lookup(type);
  }
};

SelectTypeInd selectTypeInd;

class MouseTypeInd : public ValueMap<CQCamera3DCanvas::MouseType, int> {
 public:
  MouseTypeInd() {
    add("Camera", CQCamera3DCanvas::MouseType::CAMERA, 0);
    add("Object", CQCamera3DCanvas::MouseType::OBJECT, 1);
  };

  CQCamera3DCanvas::MouseType indToMouseType(int ind) {
    return lookup(ind);
  }

  int mouseTypeToInd(const CQCamera3DCanvas::MouseType &type) {
    return lookup(type);
  }
};

MouseTypeInd mouseTypeInd;

}

CQCamera3DControl::
CQCamera3DControl(CQCamera3DApp *app) :
 app_(app)
{
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

  auto *tab = new QTabWidget;

  tab->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  layout->addWidget(tab);

  //------

  auto *generalWidget = new QFrame;
  auto *generalLayout = new QVBoxLayout(generalWidget);

  tab->addTab(generalWidget, "General");

  currentLayout = generalLayout;

  //---

  showWireframeCheck_ = addCheck("Show Wireframe");
  showSolidCheck_     = addCheck("Show Solid");

  depthTestCheck_ = addCheck("Depth Test");
  cullFaceCheck_  = addCheck("Cull Face");
  frontFaceCheck_ = addCheck("Front Face");

  addStretch();

  //------

  auto *cameraWidget = new QFrame;
  auto *cameraLayout = new QVBoxLayout(cameraWidget);

  tab->addTab(cameraWidget, "Camera");

  currentLayout = cameraLayout;

  //---

  altCheck_ = addCheck("Alt");

  showCameraCheck_  = addCheck("Show Camera");
  showEyeLineCheck_ = addCheck("Show Eye Line");
  showPlanesCheck_  = addCheck("Show Planes");

  pitchEdit_ = addLabelEdit("Pitch", new CQRealSpin);
  yawEdit_   = addLabelEdit("Yaw"  , new CQRealSpin);
  rollEdit_  = addLabelEdit("Roll" , new CQRealSpin);

  nearEdit_ = addLabelEdit("Near", new CQRealSpin);
  farEdit_  = addLabelEdit("Far" , new CQRealSpin);

  //---

  auto *originGroup  = new QGroupBox("Origin");
  auto *originLayout = new QVBoxLayout(originGroup);

  currentLayout->addWidget(originGroup);

  currentLayout = originLayout;

  xOriginEdit_ = addLabelEdit("X", new CQRealSpin);
  yOriginEdit_ = addLabelEdit("Y", new CQRealSpin);
  zOriginEdit_ = addLabelEdit("Z", new CQRealSpin);

  //---

  auto *positionGroup  = new QGroupBox("Position");
  auto *positionLayout = new QVBoxLayout(positionGroup);

  currentLayout->addWidget(positionGroup);

  currentLayout = positionLayout;

  xEdit_ = addLabelEdit("X", new CQRealSpin);
  yEdit_ = addLabelEdit("Y", new CQRealSpin);
  zEdit_ = addLabelEdit("Z", new CQRealSpin);

  //---

  currentLayout = cameraLayout;

  fovEdit_ = addLabelEdit("FOV", new CQRealSpin);

  eyeZ1Edit_ = addLabelEdit("Eye Z1", new CQRealSpin);
  eyeZ2Edit_ = addLabelEdit("Eye Z2", new CQRealSpin);

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

  aroundXButtons_ = plusMinusFrame("Around X");
  aroundYButtons_ = plusMinusFrame("Around Y");
  aroundZButtons_ = plusMinusFrame("Around Z");

  addStretch();

  auto *buttonFrame  = new QFrame;
  auto *buttonLayout = new QHBoxLayout(buttonFrame);

  currentLayout->addWidget(buttonFrame);

  auto *resetButton = new QPushButton("Reset");
  auto *topButton   = new QPushButton("Top");
  auto *sideButton  = new QPushButton("Side");
  auto *frontButton = new QPushButton("Front");

  connect(resetButton, SIGNAL(clicked()), this, SLOT(resetSlot()));
  connect(topButton, SIGNAL(clicked()), this, SLOT(topSlot()));
  connect(sideButton, SIGNAL(clicked()), this, SLOT(sideSlot()));
  connect(frontButton, SIGNAL(clicked()), this, SLOT(frontSlot()));

  buttonLayout->addWidget(resetButton);
  buttonLayout->addWidget(topButton);
  buttonLayout->addWidget(sideButton);
  buttonLayout->addWidget(frontButton);
  buttonLayout->addStretch(1);

  //------

  auto *axisWidget = new QFrame;
  auto *axisLayout = new QVBoxLayout(axisWidget);

  tab->addTab(axisWidget, "Axis");

  currentLayout = axisLayout;

  showAxesCheck_ = addCheck("Show Axes");

  axesXPos_ = addLabelEdit("X Pos", new CQRealSpin);
  axesYPos_ = addLabelEdit("Y Pos", new CQRealSpin);
  axesZPos_ = addLabelEdit("Z Pos", new CQRealSpin);

  addStretch();

  //------

  auto *mouseWidget = new QFrame;
  auto *mouseLayout = new QVBoxLayout(mouseWidget);

  tab->addTab(mouseWidget, "Mouse");

  currentLayout = mouseLayout;

  mouseTypeCombo_ = addLabelEdit("Type", new QComboBox);
  mouseTypeCombo_->addItems(mouseTypeInd.names());

  addStretch();

  //------

  auto *selectionWidget = new QFrame;
  auto *selectionLayout = new QVBoxLayout(selectionWidget);

  tab->addTab(selectionWidget, "Selection");

  currentLayout = selectionLayout;

  selectTypeCombo_ = addLabelEdit("Select", new QComboBox);
  selectTypeCombo_->addItems(selectTypeInd.names());

  objectIndLabel_ = addLabelEdit("Name", new CQTextLabel);
  objectVisCheck_ = addLabelEdit("Visible", new QCheckBox);

  objectColor_ = addLabelEdit("Color", new CQColorEdit);

  //---

  auto *transformGroup  = new QGroupBox("Transform");
  auto *transformLayout = new QVBoxLayout(transformGroup);

  currentLayout->addWidget(transformGroup);

  currentLayout = transformLayout;

  objectMatrix_ = new CQMatrix3D;
  selectionLayout->addWidget(objectMatrix_);

  translationEdit_ = addLabelEdit("Translation", new CQPoint3DEdit);
  rotationEdit_    = addLabelEdit("Rotation"   , new CQPoint3DEdit);
  scaleEdit_       = addLabelEdit("Scale"      , new CQPoint3DEdit);

  currentLayout = selectionLayout;

  //---

  auto *texturesGroup  = new QGroupBox("Textures");
  auto *texturesLayout = new QVBoxLayout(texturesGroup);

  currentLayout->addWidget(texturesGroup);

  currentLayout = texturesLayout;

  diffuseTextureEdit_  = addLabelEdit("Diffuse" , new CQCamera3DTextureChooser(app_));
  specularTextureEdit_ = addLabelEdit("Specular", new CQCamera3DTextureChooser(app_));
  normalTextureEdit_   = addLabelEdit("Normal"  , new CQCamera3DTextureChooser(app_));
  emissiveTextureEdit_ = addLabelEdit("Emissive", new CQCamera3DTextureChooser(app_));

  auto *texturesButtonsFrame  = new QFrame;
  auto *texturesButtonsLayout = new QHBoxLayout(texturesButtonsFrame);

  currentLayout->addWidget(texturesButtonsFrame);

  auto *addTextureButton     = new QPushButton("Add Texture");
  auto *loadTextureMapButton = new QPushButton("Load Texture Map");

  connect(addTextureButton, SIGNAL(clicked()), this, SLOT(addTextureSlot()));
  connect(loadTextureMapButton, SIGNAL(clicked()), this, SLOT(loadTextureMapSlot()));

  texturesButtonsLayout->addWidget(addTextureButton);
  texturesButtonsLayout->addWidget(loadTextureMapButton);
  texturesButtonsLayout->addStretch(1);

  currentLayout = selectionLayout;

  //---

  auto *modGroup  = new QGroupBox("Modify");
  auto *modLayout = new QHBoxLayout(modGroup);

  currentLayout->addWidget(modGroup);

  currentLayout = modLayout;

  auto *swapYZButton  = new QPushButton("Swap YZ");
  auto *invertXButton = new QPushButton("Invert X");
  auto *invertYButton = new QPushButton("Invert Y");
  auto *invertZButton = new QPushButton("Invert Z");

  connect(swapYZButton , SIGNAL(clicked()), this, SLOT(swapYZSlot()));
  connect(invertXButton, SIGNAL(clicked()), this, SLOT(invertXSlot()));
  connect(invertYButton, SIGNAL(clicked()), this, SLOT(invertYSlot()));
  connect(invertZButton, SIGNAL(clicked()), this, SLOT(invertZSlot()));

  currentLayout->addWidget(swapYZButton);
  currentLayout->addWidget(invertXButton);
  currentLayout->addWidget(invertYButton);
  currentLayout->addWidget(invertZButton);

  currentLayout = selectionLayout;

  //---

  objectInfoText_ = new QTextEdit;

  objectInfoText_->setReadOnly(true);

  currentLayout->addWidget(objectInfoText_);

  //addStretch();

  //---

  auto *selectButtonFrame  = new QFrame;
  auto *selectButtonLayout = new QHBoxLayout(selectButtonFrame);;

  currentLayout->addWidget(selectButtonFrame);

  auto *selectParentButton = new QPushButton("Select Parent");

  connect(selectParentButton, SIGNAL(clicked()), this, SLOT(selectParentSlot()));

  auto *deselectButton = new QPushButton("Deselect All");

  connect(deselectButton, SIGNAL(clicked()), this, SLOT(deselectSlot()));

  selectButtonLayout->addWidget(selectParentButton);
  selectButtonLayout->addWidget(deselectButton);
  selectButtonLayout->addStretch(1);

  //------

  auto *objectsWidget = new QFrame;
  auto *objectsLayout = new QVBoxLayout(objectsWidget);

  tab->addTab(objectsWidget, "Objects");

  currentLayout = objectsLayout;

  objectsList_ = new CQCamera3DObjectsList(canvas);

  currentLayout->addWidget(objectsList_);

  auto *objectSelectButton = new QPushButton("Select");
  auto *objectZoomButton   = new QPushButton("Zoom");

  connect(objectSelectButton, SIGNAL(clicked()), this, SLOT(objectSelectSlot()));
  connect(objectZoomButton, SIGNAL(clicked()), this, SLOT(objectZoomSlot()));

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

  connect(addCubeButton, SIGNAL(clicked()), this, SLOT(addCubeSlot()));
  connect(addCylinderButton, SIGNAL(clicked()), this, SLOT(addCylinderSlot()));
  connect(addPyramidButton, SIGNAL(clicked()), this, SLOT(addPyramidSlot()));
  connect(addSphereButton, SIGNAL(clicked()), this, SLOT(addSphereSlot()));
  connect(addTorusButton, SIGNAL(clicked()), this, SLOT(addTorusSlot()));

  auto *addButton = new QPushButton("Add File");

  connect(addButton, SIGNAL(clicked()), this, SLOT(addModelSlot()));

  currentLayout->addWidget(addButton);

  addStretch();

  //------

  auto *overviewWidget = new QFrame;
  auto *overviewLayout = new QVBoxLayout(overviewWidget);

  tab->addTab(overviewWidget, "Overview");

  currentLayout = overviewLayout;

  overviewEqualScale_ = addLabelEdit("Equal Scale", new QCheckBox);

  addStretch();

  //---

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

  depthTestCheck_->setChecked(canvas->isDepthTest());
  cullFaceCheck_ ->setChecked(canvas->isCullFace());
  frontFaceCheck_->setChecked(canvas->isFrontFace());

  // Camera
  showCameraCheck_ ->setChecked(canvas->isShowCamera());
  showEyeLineCheck_->setChecked(canvas->isShowEyeLine());
  showPlanesCheck_ ->setChecked(canvas->isShowPlanes());

  pitchEdit_->setValue(camera->pitch());
  yawEdit_  ->setValue(camera->yaw());
  rollEdit_ ->setValue(camera->roll());

  nearEdit_->setValue(camera->near());
  farEdit_ ->setValue(camera->far());

  xOriginEdit_->setValue(camera->origin().getX());
  yOriginEdit_->setValue(camera->origin().getY());
  zOriginEdit_->setValue(camera->origin().getZ());

  xEdit_->setValue(camera->position().getX());
  yEdit_->setValue(camera->position().getY());
  zEdit_->setValue(camera->position().getZ());

  fovEdit_->setValue(camera->fov());

  eyeZ1Edit_->setValue(canvas->eyeLineZ1());
  eyeZ2Edit_->setValue(canvas->eyeLineZ2());

  // Axis
  auto *axes = canvas->axes();

  if (axes) {
    showAxesCheck_->setChecked(axes->isVisible());

    axesXPos_->setValue(axes->xpos());
    axesYPos_->setValue(axes->ypos());
    axesZPos_->setValue(axes->zpos());
  }

  // Mouse
  mouseTypeCombo_->setCurrentIndex(mouseTypeInd.mouseTypeToInd(canvas->mouseType()));

  // Selection
  selectTypeCombo_->setCurrentIndex(selectTypeInd.selectTypeToInd(canvas->selectType()));

  auto selectType = canvas->selectType();

  objectMatrix_   ->setEnabled(selectType == CQCamera3DCanvas::SelectType::OBJECT);
  translationEdit_->setEnabled(selectType == CQCamera3DCanvas::SelectType::OBJECT);
  rotationEdit_   ->setEnabled(selectType == CQCamera3DCanvas::SelectType::OBJECT);
  scaleEdit_      ->setEnabled(selectType == CQCamera3DCanvas::SelectType::OBJECT);

  objectColor_->setEnabled(false);

  objectIndLabel_->setText("");
  objectVisCheck_->setChecked(false);

  objectInfoText_->setText("");

  if      (selectType == CQCamera3DCanvas::SelectType::OBJECT) {
    auto *object = canvas->currentObject();

    if (object) {
      auto name = QString::fromStdString(object->getName());

      objectIndLabel_->setText(QString("%1 (#%2)").arg(name).arg(object->getInd()));

      objectVisCheck_->setChecked(object->getSelected());

      objectColor_->setEnabled(true);
      objectColor_->setColor(RGBAToQColor(object->getFaceColor()));

      objectMatrix_->setValue(object->getTransform());

      translationEdit_->setValue(object->translationValues());
      rotationEdit_   ->setValue(object->rotationValuesDeg());
      scaleEdit_      ->setValue(object->scaleValues());

      auto *diffuseTexture  = object->getDiffuseTexture();
      auto *specularTexture = object->getSpecularTexture();
      auto *normalTexture   = object->getNormalTexture();
      auto *emissiveTexture = object->getEmissiveTexture();

      diffuseTextureEdit_  ->setTextureName(
        diffuseTexture ? QString::fromStdString(diffuseTexture->name()) : "");
      specularTextureEdit_ ->setTextureName(
        specularTexture ? QString::fromStdString(specularTexture->name()) : "");
      normalTextureEdit_   ->setTextureName(
        normalTexture ? QString::fromStdString(normalTexture->name()) : "");
      emissiveTextureEdit_ ->setTextureName(
        emissiveTexture ? QString::fromStdString(emissiveTexture->name()) : "");

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

      objectInfoText_->setText(objStr);
    }
  }
  else if (selectType == CQCamera3DCanvas::SelectType::FACE) {
    auto *face = canvas->currentFace();

    if (face) {
      objectIndLabel_->setText(QString("%1").arg(face->getInd()));

      objectColor_->setEnabled(true);
      objectColor_->setColor(RGBAToQColor(face->getColor()));

      objectVisCheck_->setChecked(face->getSelected());

      auto *diffuseTexture  = face->getDiffuseTexture();
      auto *specularTexture = face->getSpecularTexture();
      auto *normalTexture   = face->getNormalTexture();
      auto *emissiveTexture = face->getEmissiveTexture();

      diffuseTextureEdit_  ->setTextureName(
        diffuseTexture ? QString::fromStdString(diffuseTexture->name()) : "");
      specularTextureEdit_ ->setTextureName(
        specularTexture ? QString::fromStdString(specularTexture->name()) : "");
      normalTextureEdit_   ->setTextureName(
        normalTexture ? QString::fromStdString(normalTexture->name()) : "");
      emissiveTextureEdit_ ->setTextureName(
        emissiveTexture ? QString::fromStdString(emissiveTexture->name()) : "");
    }
  }

  // Overview
  auto *overview = app_->overview();

  overviewEqualScale_->setChecked(overview->isEqualScale());

  connectSlots(true);
}

void
CQCamera3DControl::
connectSlots(bool b)
{
  auto *canvas = app_->canvas();

  CQUtil::connectDisconnect(b, canvas, SIGNAL(stateChanged()),
                            this, SLOT(updateWidgets()));

  // General
  CQUtil::connectDisconnect(b, showWireframeCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(showWireframeSlot(int)));
  CQUtil::connectDisconnect(b, showSolidCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(showSolidSlot(int)));

  CQUtil::connectDisconnect(b, depthTestCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(depthTestSlot(int)));
  CQUtil::connectDisconnect(b, cullFaceCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(cullSlot(int)));
  CQUtil::connectDisconnect(b, frontFaceCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(frontFaceSlot(int)));

  // Camera
  CQUtil::connectDisconnect(b, altCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(altSlot(int)));

  CQUtil::connectDisconnect(b, showCameraCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(showCameraSlot(int)));
  CQUtil::connectDisconnect(b, showEyeLineCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(showEyeLineSlot(int)));
  CQUtil::connectDisconnect(b, showPlanesCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(showPlanesSlot(int)));

  CQUtil::connectDisconnect(b, pitchEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(pitchSlot(double)));
  CQUtil::connectDisconnect(b, yawEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(yawSlot(double)));
  CQUtil::connectDisconnect(b, rollEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(rollSlot(double)));

  CQUtil::connectDisconnect(b, nearEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(nearSlot(double)));
  CQUtil::connectDisconnect(b, farEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(farSlot(double)));

  CQUtil::connectDisconnect(b, xOriginEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(xOriginSlot(double)));
  CQUtil::connectDisconnect(b, yOriginEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(yOriginSlot(double)));
  CQUtil::connectDisconnect(b, zOriginEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(zOriginSlot(double)));

  CQUtil::connectDisconnect(b, xEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(xSlot(double)));
  CQUtil::connectDisconnect(b, yEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(ySlot(double)));
  CQUtil::connectDisconnect(b, zEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(zSlot(double)));

  CQUtil::connectDisconnect(b, fovEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(fovSlot(double)));
  CQUtil::connectDisconnect(b, eyeZ1Edit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(eyeZ1Slot(double)));
  CQUtil::connectDisconnect(b, eyeZ2Edit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(eyeZ2Slot(double)));

  CQUtil::connectDisconnect(b, aroundXButtons_.minusButton, SIGNAL(clicked()),
                            this, SLOT(aroundXSlot1()));
  CQUtil::connectDisconnect(b, aroundXButtons_.plusButton, SIGNAL(clicked()),
                            this, SLOT(aroundXSlot2()));
  CQUtil::connectDisconnect(b, aroundYButtons_.minusButton, SIGNAL(clicked()),
                            this, SLOT(aroundYSlot1()));
  CQUtil::connectDisconnect(b, aroundYButtons_.plusButton, SIGNAL(clicked()),
                            this, SLOT(aroundYSlot2()));
  CQUtil::connectDisconnect(b, aroundZButtons_.minusButton, SIGNAL(clicked()),
                            this, SLOT(aroundZSlot1()));
  CQUtil::connectDisconnect(b, aroundZButtons_.plusButton, SIGNAL(clicked()),
                            this, SLOT(aroundZSlot2()));

  // Axis
  CQUtil::connectDisconnect(b, showAxesCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(showAxesSlot(int)));

  CQUtil::connectDisconnect(b, axesXPos_, SIGNAL(realValueChanged(double)),
                            this, SLOT(axesXPosSlot(double)));
  CQUtil::connectDisconnect(b, axesYPos_, SIGNAL(realValueChanged(double)),
                            this, SLOT(axesYPosSlot(double)));
  CQUtil::connectDisconnect(b, axesZPos_, SIGNAL(realValueChanged(double)),
                            this, SLOT(axesZPosSlot(double)));

  // Selection
  CQUtil::connectDisconnect(b, mouseTypeCombo_, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(mouseTypeSlot(int)));

  // Selection
  CQUtil::connectDisconnect(b, selectTypeCombo_, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(selectTypeSlot(int)));

  CQUtil::connectDisconnect(b, objectVisCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(objectVisSlot(int)));

#if 0
  CQUtil::connectDisconnect(b, objectMatrix_, SIGNAL(valueChanged()),
                            this, SLOT(objectMatrixSlot()));
#else
  CQUtil::connectDisconnect(b, translationEdit_, SIGNAL(editingFinished()),
                            this, SLOT(translationSlot()));
  CQUtil::connectDisconnect(b, rotationEdit_, SIGNAL(editingFinished()),
                            this, SLOT(rotationSlot()));
  CQUtil::connectDisconnect(b, scaleEdit_, SIGNAL(editingFinished()),
                            this, SLOT(scaleSlot()));
#endif

  CQUtil::connectDisconnect(b, objectColor_, SIGNAL(colorChanged(const QColor &)),
                            this, SLOT(objectColorSlot(const QColor &)));

  CQUtil::connectDisconnect(b, diffuseTextureEdit_ , SIGNAL(textureChanged()),
                            this, SLOT(diffuseMapSlot()));
  CQUtil::connectDisconnect(b, normalTextureEdit_  , SIGNAL(textureChanged()),
                            this, SLOT(normalMapSlot()));
  CQUtil::connectDisconnect(b, specularTextureEdit_, SIGNAL(textureChanged()),
                            this, SLOT(specularMapSlot()));
  CQUtil::connectDisconnect(b, emissiveTextureEdit_, SIGNAL(textureChanged()),
                            this, SLOT(emissiveMapSlot()));

  // Selection
  CQUtil::connectDisconnect(b, canvas, SIGNAL(objectsChanged()),
                            objectsList_, SLOT(updateObjects()));

  // Overview
  CQUtil::connectDisconnect(b, overviewEqualScale_, SIGNAL(stateChanged(int)),
                            this, SLOT(overviewEqualScaleSlot(int)));
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

  canvas->setMouseType(mouseTypeInd.indToMouseType(i));

  updateWidgets();
}

void
CQCamera3DControl::
selectTypeSlot(int i)
{
  auto *canvas = app_->canvas();

  canvas->setSelectType(selectTypeInd.indToSelectType(i));

  updateWidgets();
}

void
CQCamera3DControl::
objectVisSlot(int i)
{
  auto *canvas = app_->canvas();

  auto selectType = canvas->selectType();

  if      (selectType == CQCamera3DCanvas::SelectType::OBJECT) {
    auto *object = canvas->currentObject();

    if (object)
      object->setVisible(i);
  }
  else if (selectType == CQCamera3DCanvas::SelectType::FACE) {
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
    auto p = translationEdit_->getValue();

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
    auto p = rotationEdit_->getValue();

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
    auto p = scaleEdit_->getValue();

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

  if      (selectType == CQCamera3DCanvas::SelectType::OBJECT) {
    auto *object = canvas->currentObject();

    if (object)
      object->setFaceColor(QColorToRGBA(c));
  }
  else if (selectType == CQCamera3DCanvas::SelectType::FACE) {
    auto *face = canvas->currentFace();

    if (face)
      face->setColor(QColorToRGBA(c));
  }

  updateObjects();
}

void
CQCamera3DControl::
diffuseMapSlot()
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
normalMapSlot()
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
specularMapSlot()
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
emissiveMapSlot()
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
  auto pos    = CVector3D(center.x, center.y, center.z + camera->near() + s2*size/2.0);

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
swapYZSlot()
{
  auto *canvas = app_->canvas();
  auto *object = canvas->currentObject();
  if (! object) return;

  object->swapYZ();

  updateObjects();
}

void
CQCamera3DControl::
invertXSlot()
{
  auto *canvas = app_->canvas();
  auto *object = canvas->currentObject();
  if (! object) return;

  object->invertX();

  updateObjects();
}

void
CQCamera3DControl::
invertYSlot()
{
  auto *canvas = app_->canvas();
  auto *object = canvas->currentObject();
  if (! object) return;

  object->invertY();

  updateObjects();
}

void
CQCamera3DControl::
invertZSlot()
{
  auto *canvas = app_->canvas();
  auto *object = canvas->currentObject();
  if (! object) return;

  object->invertZ();

  updateObjects();
}

void
CQCamera3DControl::
loadTextureMapSlot()
{
  struct TextureMapData {
    std::string diffuse;
    std::string normal;
    std::string specular;
    std::string emissive;
  };

  using TextureMap = std::map<std::string, TextureMapData>;

  TextureMap textureMap;

  auto dir = QDir::current().dirName();

  auto fileName = QFileDialog::getOpenFileName(this, "Load Texture Map", dir, "Files (*.*)");
  if (! fileName.length()) return;

  CFile file(fileName.toStdString());
  if (! file.exists()) return;

  while (! file.eof()) {
    std::string line;

    file.readLine(line);

    std::vector<std::string> words;

    CStrUtil::toWords(line, words);
    if (words.size() < 2) continue;

    TextureMapData textureData;

    textureData.diffuse = words[1];

    if (words.size() > 2)
      textureData.normal = words[2];

    if (words.size() > 3)
      textureData.specular = words[3];

    textureMap[words[0]] = textureData;
  }

  for (auto &pt : textureMap) {
    const auto &data = pt.second;

    addTextureFile(data.diffuse);
    addTextureFile(data.normal);
    addTextureFile(data.specular);
    addTextureFile(data.emissive);
  }

  auto *scene  = app_->getScene();
  auto *canvas = app_->canvas();

  for (auto *object : scene->getObjects()) {
    auto name = object->getName();

    auto pt = textureMap.find(name);
    if (pt == textureMap.end())
      continue;

    const auto &data = (*pt).second;

    auto *texture = canvas->getGeomTextureByName(data.diffuse);

    if (texture)
      object->setDiffuseTexture(texture);

    if (data.normal != "") {
      auto *texture = canvas->getGeomTextureByName(data.normal);

      if (texture)
        object->setNormalTexture(texture);
    }

    if (data.specular != "") {
      auto *texture = canvas->getGeomTextureByName(data.specular);

      if (texture)
        object->setSpecularTexture(texture);
    }

    if (data.emissive != "") {
      auto *texture = canvas->getGeomTextureByName(data.emissive);

      if (texture)
        object->setEmissiveTexture(texture);
    }
  }

  updateObjects();
}

void
CQCamera3DControl::
addTextureSlot()
{
  auto dir = QDir::current().dirName();

  auto fileName = QFileDialog::getOpenFileName(this, "Open Texture", dir, "Files (*.*)");
  if (! fileName.length()) return;

  addTextureFile(fileName.toStdString());
}

void
CQCamera3DControl::
addTextureFile(const std::string &fileName)
{
  if (fileName == "")
    return;

  auto *canvas = app_->canvas();

  auto fileName1 = fileName;

  auto len = fileName1.size();

  if (len > 5 && fileName1.substr(len - 5) == ".flip")
    fileName1 = fileName1.substr(0, len - 5);

  auto name1 = fileName1;
  auto name2 = fileName1 + ".flip";

  auto *texture1 = canvas->getTextureByName(name1);
  auto *texture2 = canvas->getTextureByName(name2);

  if (! texture1 || ! texture2) {
    CImageFileSrc src(fileName1);

    auto image = CImageMgrInst->createImage(src);

    auto *gtexture1 = CGeometryInst->createTexture(image);

    gtexture1->setName(name1);

    (void) canvas->getTexture(gtexture1, /*add*/true);

    auto image1 = image->dup();

    image1->flipH();

    auto *gtexture2 = CGeometryInst->createTexture(image1);

    gtexture2->setName(name2);

    (void) canvas->getTexture(gtexture2, /*add*/true);
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
  auto pos    = CVector3D(center.x, center.y, center.z + camera->near() + s2*size/2.0);

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
overviewEqualScaleSlot(int i)
{
  auto *overview = app_->overview();

  overview->setEqualScale(i);
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
