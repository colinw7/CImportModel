#ifndef Status_H
#define Status_H

#include <QFrame>

class CQDocumentLabel;
class CQTextLabel;
class CQIconButton;

namespace CQShadow3D {

class App;

class Status : public QFrame {
  Q_OBJECT

 public:
  Status(App *app);

  void setTipLabel  (const QString &label);
  void setStateLabel(const QString &label);
  void setMouseLabel(const QString &label);

 private:
  App* app_ { nullptr };

  CQDocumentLabel* tipLabel_   { nullptr };
  CQTextLabel*     stateLabel_ { nullptr };
  CQTextLabel*     mouseLabel_ { nullptr };
};

}

#endif
