#ifndef CQCamera3DSidebar_H
#define CQCamera3DSidebar_H

#include <CQCamera3DCanvas.h>

#include <QFrame>

class CQCamera3DApp;

class CQIconButton;

class CQCamera3DSidebar : public QFrame {
  Q_OBJECT

 public:
  enum class View {
    NONE,
    CANVAS,
    OVERVIEW
  };

 public:
  CQCamera3DSidebar(CQCamera3DApp *app);

  const View &view() const { return view_; }
  void setView(const View &v);

 private Q_SLOTS:
  void selectSlot(bool state);
  void cursorSlot(bool state);
  void moveSlot(bool state);
  void rotateSlot(bool state);
  void scaleSlot(bool state);

  void extrudeSlot();
  void loopCutSlot();

  void updateButtonState();

 private:
  void connectSlots(bool b);
  void updateEditType();

 private:
  CQCamera3DApp* app_ { nullptr };

  View view_ { View::CANVAS };

  CQCamera3DCanvas::EditType editType_ { CQCamera3DCanvas::EditType::SELECT };

  CQIconButton* selectButton_ { nullptr };
  CQIconButton* cursorButton_ { nullptr };
  CQIconButton* moveButton_   { nullptr };
  CQIconButton* rotateButton_ { nullptr };
  CQIconButton* scaleButton_  { nullptr };

  CQIconButton* extrudeButton_ { nullptr };
  CQIconButton* loopCutButton_ { nullptr };

  std::vector<CQIconButton *> checkButtons_;
};

#endif
