#ifndef CQCamera3DToolbar_H
#define CQCamera3DToolbar_H

#include <CQCamera3DApp.h>

#include <QFrame>

class CQCamera3DApp;
class CQCamera3DCanvas;
class CQCamera3DOverview;

class CQIconButton;

class QMenuBar;
class QComboBox;
class QToolButton;

class CQCamera3DToolbar : public QFrame {
  Q_OBJECT

 public:
  CQCamera3DToolbar(CQCamera3DApp *app);

  CQCamera3DCanvas   *canvas  () const { return app_->canvas(); }
  CQCamera3DOverview *overview() const { return app_->overview(); }

 private Q_SLOTS:
  void viewTypeSlot();

  void editModeSlot(int);

  void pointSelectSlot(bool);
  void edgeSelectSlot(bool);
  void faceSelectSlot(bool);
  void objectSelectSlot(bool);

  void perspectiveSlot();
  void topSlot();
  void bottomSlot();
  void leftSlot();
  void rightSlot();
  void frontSlot();
  void backSlot();

  void localSlot(bool);

  void selectAllSlot();
  void selectNoneSlot();

  void addPlaneSlot();
  void addCubeSlot();
  void addCircleSlot();
  void addSphereSlot();
  void addCylinderSlot();
  void addConeSlot();
  void addTorusSlot();

  void wireframeSlot();
  void solidFillSlot();
  void textureFillSlot();

  void updateWidgets();

  void metaEditSlot();
  void performanceSlot();
  void optionsSlot();

 private:
  QToolButton *addDebugButton();

  void connectSlots(bool b);
  void updateSelectType();

 private:
  CQCamera3DApp* app_ { nullptr };

  CQCamera3DViewType viewType_ { CQCamera3DViewType::NONE };

  struct CanvasSelectData {
    CQCamera3DSelectType selectType { CQCamera3DSelectType::FACE };

    QFrame *frame { nullptr };

    QComboBox* editModeCombo { nullptr };

    CQIconButton* pointSelectButton { nullptr };
    CQIconButton* edgeSelectButton  { nullptr };
    CQIconButton* faceSelectButton  { nullptr };
  };

  CanvasSelectData canvasSelectData_;

  struct OverviewSelectData {
    CQCamera3DSelectType selectType { CQCamera3DSelectType::OBJECT };

    QFrame *frame { nullptr };

    CQIconButton* pointSelectButton  { nullptr };
    CQIconButton* edgeSelectButton   { nullptr };
    CQIconButton* faceSelectButton   { nullptr };
    CQIconButton* objectSelectButton { nullptr };
  };

  OverviewSelectData overviewSelectData_;

  QFrame* menuFrame_ { nullptr };

  QMenuBar* menuBar_ { nullptr };

  CQIconButton* wireframeButton_   { nullptr };
  CQIconButton* solidFillButton_   { nullptr };
  CQIconButton* textureFillButton_ { nullptr };
};

#endif
