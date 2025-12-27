#include <CQPoint4DEdit.h>
#include <CQRealSpin.h>
#include <CStrUtil.h>
#include <QDoubleValidator>

#if 0
class CQPoint4DLineEdit : public QLineEdit {
 public:
  CQPoint4DLineEdit(CQPoint4DEdit *edit) :
   QLineEdit(edit) {
  }

  QSize sizeHint() const override {
    QFontMetrics fm(font());

    int w = fm.horizontalAdvance("XXXX.XXX") + 8;
    int h = fm.height() + 8;

    return QSize(w, h);
  }
};
#else
class CQPoint4DLineEdit : public CQRealSpin {
 public:
  CQPoint4DLineEdit(CQPoint4DEdit *edit) :
   CQRealSpin(edit) {
  }

  QSize sizeHint() const override {
    QFontMetrics fm(font());

    int w = fm.horizontalAdvance("XXXX.XXX") + 8;
    int h = fm.height() + 8;

    return QSize(w, h);
  }
};
#endif

CQPoint4DEdit::
CQPoint4DEdit()
{
  init();
}

CQPoint4DEdit::
CQPoint4DEdit(const QString &name, const CPoint4D &value)
{
  setObjectName(name);

  init();

  setValue(value);
}

void
CQPoint4DEdit::
init()
{
  QFontMetrics fm(font());

#if 0
  auto *validator = new QDoubleValidator(this);
#endif

  w_edit_ = new CQPoint4DLineEdit(this); w_edit_->setPlaceholderText("w");
  x_edit_ = new CQPoint4DLineEdit(this); x_edit_->setPlaceholderText("x");
  y_edit_ = new CQPoint4DLineEdit(this); y_edit_->setPlaceholderText("y");
  z_edit_ = new CQPoint4DLineEdit(this); z_edit_->setPlaceholderText("z");

  w_edit_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  x_edit_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  y_edit_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  z_edit_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

#if 0
  w_edit_->setValidator(validator);
  x_edit_->setValidator(validator);
  y_edit_->setValidator(validator);
  z_edit_->setValidator(validator);
#endif

  layout_ = new QHBoxLayout(this);
  layout_->setMargin(2); layout_->setSpacing(2);

  layout_->addWidget(w_edit_);
  layout_->addWidget(x_edit_);
  layout_->addWidget(y_edit_);
  layout_->addWidget(z_edit_);

  connectSlots(true);
}

void
CQPoint4DEdit::
connectSlots(bool b)
{
  if (b) {
#if 0
    connect(w_edit_, SIGNAL(editingFinished()), this, SLOT(editingFinishedI()));
    connect(x_edit_, SIGNAL(editingFinished()), this, SLOT(editingFinishedI()));
    connect(y_edit_, SIGNAL(editingFinished()), this, SLOT(editingFinishedI()));
    connect(z_edit_, SIGNAL(editingFinished()), this, SLOT(editingFinishedI()));
#else
    connect(w_edit_, SIGNAL(realValueChanged(double)), this, SLOT(editingFinishedI()));
    connect(x_edit_, SIGNAL(realValueChanged(double)), this, SLOT(editingFinishedI()));
    connect(y_edit_, SIGNAL(realValueChanged(double)), this, SLOT(editingFinishedI()));
    connect(z_edit_, SIGNAL(realValueChanged(double)), this, SLOT(editingFinishedI()));
#endif
  }
  else {
#if 0
    disconnect(w_edit_, SIGNAL(editingFinished()), this, SLOT(editingFinishedI()));
    disconnect(x_edit_, SIGNAL(editingFinished()), this, SLOT(editingFinishedI()));
    disconnect(y_edit_, SIGNAL(editingFinished()), this, SLOT(editingFinishedI()));
    disconnect(z_edit_, SIGNAL(editingFinished()), this, SLOT(editingFinishedI()));
#else
    disconnect(w_edit_, SIGNAL(realValueChanged(double)), this, SLOT(editingFinishedI()));
    disconnect(x_edit_, SIGNAL(realValueChanged(double)), this, SLOT(editingFinishedI()));
    disconnect(y_edit_, SIGNAL(realValueChanged(double)), this, SLOT(editingFinishedI()));
    disconnect(z_edit_, SIGNAL(realValueChanged(double)), this, SLOT(editingFinishedI()));
#endif
  }
}

void
CQPoint4DEdit::
setValue(const CPoint4D &point)
{
  point_ = point;

  pointToWidget();
}

const CPoint4D &
CQPoint4DEdit::
getValue() const
{
  return point_;
}

void
CQPoint4DEdit::
editingFinishedI()
{
  widgetToPoint();

  Q_EMIT editingFinished();
}

void
CQPoint4DEdit::
pointToWidget()
{
  connectSlots(false);

#if 0
  w_edit_->setText(CStrUtil::toString(point_.getW()).c_str());
  x_edit_->setText(CStrUtil::toString(point_.getX()).c_str());
  y_edit_->setText(CStrUtil::toString(point_.getY()).c_str());
  z_edit_->setText(CStrUtil::toString(point_.getZ()).c_str());
#else
  w_edit_->setValue(point_.getW());
  x_edit_->setValue(point_.getX());
  y_edit_->setValue(point_.getY());
  z_edit_->setValue(point_.getZ());
#endif

  connectSlots(true);
}

void
CQPoint4DEdit::
widgetToPoint()
{
#if 0
  point_.setW(CStrUtil::toReal(w_edit_->text().toStdString()));
  point_.setX(CStrUtil::toReal(x_edit_->text().toStdString()));
  point_.setY(CStrUtil::toReal(y_edit_->text().toStdString()));
  point_.setZ(CStrUtil::toReal(z_edit_->text().toStdString()));
#else
  point_.setW(w_edit_->value());
  point_.setX(x_edit_->value());
  point_.setY(y_edit_->value());
  point_.setZ(z_edit_->value());
#endif
}
