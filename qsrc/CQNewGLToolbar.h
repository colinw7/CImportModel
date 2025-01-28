#ifndef CQNewGLToolbar_H
#define CQNewGLToolbar_H

#include <QFrame>

class CQNewGLModel;
class QToolButton;

class CQNewGLToolbar : public QFrame {
  Q_OBJECT

 public:
  CQNewGLToolbar(CQNewGLModel *app);

  CQNewGLModel *app() const { return app_; }

 private Q_SLOTS:
  void settingsSlot();

 private:
  CQNewGLModel* app_ { nullptr };

  QToolButton* settingsButton_ { nullptr };
};

#endif
