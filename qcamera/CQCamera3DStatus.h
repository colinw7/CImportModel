#ifndef CQCamera3DStatus_H
#define CQCamera3DStatus_H

#include <QFrame>

class CQCamera3DApp;

class CQDocumentLabel;
class CQTextLabel;

class CQCamera3DStatus : public QFrame {
  Q_OBJECT

 public:
  CQCamera3DStatus(CQCamera3DApp *app);

  void setTipLabel  (const QString &label);
  void setStateLabel(const QString &label);
  void setModelLabel(const QString &label);
  void setMouseLabel(const QString &label);

 private:
  CQCamera3DApp* app_ { nullptr };

  CQDocumentLabel* tipLabel_   { nullptr };
  CQTextLabel*     stateLabel_ { nullptr };
  CQTextLabel*     modelLabel_ { nullptr };
  CQTextLabel*     mouseLabel_ { nullptr };
};

#endif
