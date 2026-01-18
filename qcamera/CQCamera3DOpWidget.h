#ifndef CQCamera3DOpWidget_H
#define CQCamera3DOpWidget_H

#include <CQWinWidget.h>

class CQCamera3DCanvas;
class CQCamera3DOpContents;
class CQCamera3DOptions;

class QVBoxLayout;

class CQCamera3DOpWidget : public CQWinWidget {
  Q_OBJECT

 public:
  CQCamera3DOpWidget(CQCamera3DCanvas *canvas);

  void setOptions(CQCamera3DOptions *options);

 private Q_SLOTS:
  void closeSlot();

 private:
  CQCamera3DCanvas*     canvas_   { nullptr };
  CQCamera3DOpContents* contents_ { nullptr };
};

class CQCamera3DOpContents : public QFrame {
  Q_OBJECT

 public:
  CQCamera3DOpContents(CQCamera3DOpWidget *widget);
 ~CQCamera3DOpContents() override;

  void setOptions(CQCamera3DOptions *options);

  QSize sizeHint() const override;

 private:
  CQCamera3DOpWidget* widget_      { nullptr };
  QVBoxLayout*        layout_      { nullptr };
  QFrame*             frame_       { nullptr };
  QVBoxLayout*        frameLayout_ { nullptr };
  CQCamera3DOptions*  options_     { nullptr };
};

#endif
