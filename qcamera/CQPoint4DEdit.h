#ifndef CQPoint4DEdit_H
#define CQPoint4DEdit_H

#include <QLineEdit>
#include <QHBoxLayout>
#include <CPoint4D.h>

class CQRealSpin;

class CQPoint4DEdit : public QWidget {
  Q_OBJECT

 public:
  CQPoint4DEdit();
  CQPoint4DEdit(const QString &name, const CPoint4D &value);

  virtual ~CQPoint4DEdit() { }

  const CPoint4D &getValue() const;
  void setValue(const CPoint4D &point);

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
  CPoint4D point_;
#if 0
  QLineEdit *w_edit_ { nullptr };
  QLineEdit *x_edit_ { nullptr };
  QLineEdit *y_edit_ { nullptr };
  QLineEdit *z_edit_ { nullptr };
#else
  CQRealSpin *w_edit_ { nullptr };
  CQRealSpin *x_edit_ { nullptr };
  CQRealSpin *y_edit_ { nullptr };
  CQRealSpin *z_edit_ { nullptr };
#endif
  QHBoxLayout *layout_ { nullptr };
};

#endif
