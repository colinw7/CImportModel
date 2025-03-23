#ifndef CQPOINT3D_EDIT_H
#define CQPOINT3D_EDIT_H

#include <QLineEdit>
#include <QHBoxLayout>
#include <CPoint3D.h>

class CQPoint3DEdit : public QWidget {
  Q_OBJECT

 public:
  CQPoint3DEdit();
  CQPoint3DEdit(const QString &name, const CPoint3D &value);

  virtual ~CQPoint3DEdit() { }

  const CPoint3D &getValue() const;
  void setValue(const CPoint3D &point);

 private Q_SLOTS:
  void editingFinishedI();

 Q_SIGNALS:
  void editingFinished();

 private:
  void init();

  void pointToWidget();
  void widgetToPoint();

 private:
  CPoint3D     point_;
  QLineEdit   *x_edit_ { nullptr };
  QLineEdit   *y_edit_ { nullptr };
  QLineEdit   *z_edit_ { nullptr };
  QHBoxLayout *layout_ { nullptr };
};

#endif
