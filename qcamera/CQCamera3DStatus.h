#ifndef CQCamera3DStatus_H
#define CQCamera3DStatus_H

#include <QFrame>

class CQCamera3DApp;

class QLabel;

class CQCamera3DStatus : public QFrame {
  Q_OBJECT

 public:
  CQCamera3DStatus(CQCamera3DApp *app);

  void setModelLabel(const QString &label);
  void setMouseLabel(const QString &label);

 private:
  CQCamera3DApp* app_ { nullptr };

  QLabel* modelLabel_ { nullptr };
  QLabel* mouseLabel_ { nullptr };
};

#endif
