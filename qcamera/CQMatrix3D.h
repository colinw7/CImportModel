#ifndef CQMATRIX_3D_H
#define CQMATRIX_3D_H

#include <QWidget>
#include <CMatrix3D.h>

class CQTableWidget;

class CQMatrix3D : public QWidget {
  Q_OBJECT

 public:
  CQMatrix3D(QWidget *parent, const CMatrix3D &m=CMatrix3D::identity());
  CQMatrix3D(const CMatrix3D &m=CMatrix3D::identity());

  QSize minimumSizeHint() const override;

  QSize sizeHint() const override;

  CQTableWidget *getTableWidget() const { return table_; }

  const CMatrix3D &getValue() const { return m_; }
  void setValue(const CMatrix3D &m);

  double getValue(int r, int c) const;
  void setValue(int r, int c, double v);

 private:
  void init();
  void updateValues();

 signals:
  void valueChanged();

 private:
  CQTableWidget *table_ { nullptr };
  CMatrix3D      m_     { CMatrix3D::identity() };
};

#endif
