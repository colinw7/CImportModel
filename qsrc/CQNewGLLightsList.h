#ifndef CQNewGLLightsList_H
#define CQNewGLLightsList_H

#include <QFrame>

class CQNewGLControl;

class QListWidget;
class QListWidgetItem;

class CQNewGLLightsList : public QFrame {
  Q_OBJECT

 public:
  CQNewGLLightsList(CQNewGLControl *control);

  void connectSlots(bool b);

  void updateWidgets();

 private Q_SLOTS:
  void currentItemSlot(QListWidgetItem *, QListWidgetItem *);

 Q_SIGNALS:
  void currentItemChanged();

 private:
  CQNewGLControl* control_ { nullptr };
  QListWidget*    list_    { nullptr };
};

#endif
