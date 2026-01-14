#ifndef CQCamera3DToolbar_H
#define CQCamera3DToolbar_H

#include <QFrame>

class CQCamera3DApp;

class CQIconButton;

class QMenuBar;
class QComboBox;

class CQCamera3DToolbar : public QFrame {
  Q_OBJECT

 public:
  CQCamera3DToolbar(CQCamera3DApp *app);

 private Q_SLOTS:
  void pointSelectSlot();
  void edgeSelectSlot();
  void faceSelectSlot();

  void selectAllSlot();
  void selectNoneSlot();

  void addPlaneSlot();
  void addCubeSlot();
  void addCircleSlot();
  void addCylinderSlot();
  void addConeSlot();
  void addTorusSlot();

  void wireframeSlot();
  void solidFillSlot();
  void textureFillSlot();

 private:
  CQCamera3DApp* app_ { nullptr };

  QComboBox* editCombo_ { nullptr };

  CQIconButton* pointSelectButton_ { nullptr };
  CQIconButton* edgeSelectButton_  { nullptr };
  CQIconButton* faceSelectButton_  { nullptr };

  QMenuBar* menuBar_ { nullptr };

  CQIconButton* wireframeButton_   { nullptr };
  CQIconButton* solidFillButton_   { nullptr };
  CQIconButton* textureFillButton_ { nullptr };
};

#endif
