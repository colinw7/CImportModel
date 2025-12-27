#ifndef CQNewGLShapesList_H
#define CQNewGLShapesList_H

#include <QFrame>

class CQNewGLControl;

class QListWidget;
class QListWidgetItem;

class CQNewGLShapesList : public QFrame {
  Q_OBJECT

 public:
  CQNewGLShapesList(CQNewGLControl *control);

  void connectSlots(bool b);

  void updateWidgets();

 private Q_SLOTS:
  void invalidateShapes();

  void currentItemSlot(QListWidgetItem *, QListWidgetItem *);

 Q_SIGNALS:
  void currentItemChanged();

 private:
  CQNewGLControl* control_ { nullptr };
  QListWidget*    list_    { nullptr };

  bool reload_ { false };
};

#endif
