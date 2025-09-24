#ifndef CQNewGLToolbar_H
#define CQNewGLToolbar_H

#include <QFrame>

class CQNewGLModel;
class QToolButton;
class QLabel;

class CQNewGLToolbar : public QFrame {
  Q_OBJECT

 public:
  CQNewGLToolbar(CQNewGLModel *app);

  CQNewGLModel *app() const { return app_; }

  void setPosLabel(const QString &label);

 private Q_SLOTS:
  void cameraSlot();
  void modelSlot();
  void lightSlot();

  void playSlot();
  void pauseSlot();
  void stepSlot();

  void settingsSlot();

 private:
  CQNewGLModel* app_ { nullptr };

  QToolButton* cameraButton_ { nullptr };
  QToolButton* modelButton_  { nullptr };
  QToolButton* lightButton_  { nullptr };

  QToolButton* playButton_  { nullptr };
  QToolButton* pauseButton_ { nullptr };
  QToolButton* stepButton_  { nullptr };

  QToolButton* settingsButton_ { nullptr };

  QLabel *posLabel_ { nullptr };
};

#endif
