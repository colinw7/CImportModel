#include <CQMatrix3D.h>
#include <CQTableWidget.h>
#include <CQRealEdit.h>
#include <CStrUtil.h>

#include <QVBoxLayout>
#include <QPointer>

class MatrixCell : public CQTableWidgetItem {
 public:
  enum { TYPE = QTableWidgetItem::UserType + 1 };

  MatrixCell(CQMatrix3D *m, int row=-1, int col=-1) :
   CQTableWidgetItem(m->getTableWidget()), m_(m), row_(row), col_(col) {
  }

  MatrixCell *clone() const override { return new MatrixCell(m_); }

  QString getString() const override {
    double v = getValue();

    return CStrUtil::toString(v).c_str();
  }

  QWidget *createEditor(QWidget *parent) const override {
    edit_ = new CQRealEdit(parent);

    return edit_;
  }

  void setEditorData() override {
    if (edit_.isNull()) return;

    double v = getValue();

    edit_->setValue(v);
  }

  void getEditorData(QString &str) override {
    if (edit_.isNull()) return;

    double v = edit_->getValue();

    m_->setValue(col_, row_, v);

    str = getString();
  }

  bool sizeHint(const QStyleOptionViewItem &, QSize &size) const override {
    QFontMetrics fm(font());

    size = QSize(fm.horizontalAdvance("XXX.XXXX") + 8, fm.ascent() + fm.descent() + 8);

    return true;
  }

  double getValue() const {
    double v = 0.0;

    if (row_ >= 0 && row_ < 4 && col_ >= 0 && col_ < 4)
      v = m_->getValue(col_, row_);

    return v;
  }

  void setValue(double value) {
    setText(CStrUtil::toString(value).c_str());
  }

 private:
  using CQRealEditP = QPointer<CQRealEdit>;

  static uint type_;

  mutable CQRealEditP edit_;
  CQMatrix3D*         m_;
  int                 row_ { 0 };
  int                 col_ { 0 };
};

CQMatrix3D::
CQMatrix3D(const CMatrix3D &m) :
 QWidget(nullptr), m_(m)
{
  init();
}

CQMatrix3D::
CQMatrix3D(QWidget *parent, const CMatrix3D &m) :
 QWidget(parent), m_(m)
{
  init();
}

QSize
CQMatrix3D::
minimumSizeHint() const
{
  return sizeHint();
}

QSize
CQMatrix3D::
sizeHint() const
{
  QFontMetrics fm(font());

  QSize size(fm.horizontalAdvance("XXX.XXXX") + 8, fm.ascent() + fm.descent() + 8);

  return QSize(4*size.width() + 80, 4*size.height() + 40);
}

void
CQMatrix3D::
init()
{
  auto *layout = new QVBoxLayout(this);

  table_ = new CQTableWidget;

  table_->setColumnCount(4);

  table_->setColumnLabels(QStringList() << "1" << "2" << "3" << "4");
  table_->setRowLabels   (QStringList() << "1" << "2" << "3" << "4");

  layout->addWidget(table_);

  table_->registerType(MatrixCell::TYPE, new MatrixCell(this));

  table_->setRowCount(4);

  table_->setItem(0, 0, new MatrixCell(this, 0, 0));
  table_->setItem(0, 1, new MatrixCell(this, 0, 1));
  table_->setItem(0, 2, new MatrixCell(this, 0, 2));
  table_->setItem(0, 3, new MatrixCell(this, 0, 3));
  table_->setItem(1, 0, new MatrixCell(this, 1, 0));
  table_->setItem(1, 1, new MatrixCell(this, 1, 1));
  table_->setItem(1, 2, new MatrixCell(this, 1, 2));
  table_->setItem(1, 3, new MatrixCell(this, 1, 3));
  table_->setItem(2, 0, new MatrixCell(this, 2, 0));
  table_->setItem(2, 1, new MatrixCell(this, 2, 1));
  table_->setItem(2, 2, new MatrixCell(this, 2, 2));
  table_->setItem(2, 3, new MatrixCell(this, 2, 3));
  table_->setItem(3, 0, new MatrixCell(this, 3, 0));
  table_->setItem(3, 1, new MatrixCell(this, 3, 1));
  table_->setItem(3, 2, new MatrixCell(this, 3, 2));
  table_->setItem(3, 3, new MatrixCell(this, 3, 3));
}

void
CQMatrix3D::
setValue(const CMatrix3D &m)
{
  m_ = m;

  updateValues();

  update();
}

void
CQMatrix3D::
updateValues()
{
  for (uint row = 0; row < 4; ++row)
    for (uint col = 0; col < 4; ++col)
      table_->getItem<MatrixCell>(int(row), int(col))->setValue(m_.getValue(col, row));
}

void
CQMatrix3D::
setValue(int row, int col, double v)
{
  m_.setValue(uint(col), uint(row), v);

  emit valueChanged();
}

double
CQMatrix3D::
getValue(int row, int col) const
{
  return m_.getValue(uint(col), uint(row));
}
