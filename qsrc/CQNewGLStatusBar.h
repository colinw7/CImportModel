#ifndef CQNewGLStatusBar_H
#define CQNewGLStatusBar_H

#include <QFrame>

class CQNewGLModel;
class QLabel;

class CQNewGLStatusBar : public QFrame {
  Q_OBJECT

 public:
  CQNewGLStatusBar(CQNewGLModel *app);

  CQNewGLModel *app() const { return app_; }

  void setScaleLabel(const QString &label);

 private:
  CQNewGLModel* app_ { nullptr };

  QLabel* scaleLabel_ { nullptr };
};

#endif
