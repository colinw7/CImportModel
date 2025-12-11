#ifndef CQPOINT3D_EDIT_H
#define CQPOINT3D_EDIT_H

#include <QLineEdit>
#include <QHBoxLayout>
#include <CPoint3D.h>

class CQRealSpin;

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

  void connectSlots(bool connect);

  void pointToWidget();
  void widgetToPoint();

 private:
  CPoint3D point_;
#if 0
  QLineEdit *x_edit_ { nullptr };
  QLineEdit *y_edit_ { nullptr };
  QLineEdit *z_edit_ { nullptr };
#else
  CQRealSpin *x_edit_ { nullptr };
  CQRealSpin *y_edit_ { nullptr };
  CQRealSpin *z_edit_ { nullptr };
#endif
  QHBoxLayout *layout_ { nullptr };
};

#endif
