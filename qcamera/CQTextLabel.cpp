#include <CQTextLabel.h>

#if 0
CQTextLabel::
CQTextLabel(QWidget *parent, const QString &text) :
 QLineEdit(parent)
{
  setReadOnly(true);
  setText(text);
}

CQTextLabel::
CQTextLabel(const QString &text, QWidget *parent) :
 QLineEdit(parent)
{
  setReadOnly(true);
  setText(text);
}
#else
CQTextLabel::
CQTextLabel(QWidget *parent, const QString &text) :
 QLabel(parent)
{
  setTextInteractionFlags(Qt::TextSelectableByMouse);
  setText(text);
}

CQTextLabel::
CQTextLabel(const QString &text, QWidget *parent) :
 QLabel(parent)
{
  setTextInteractionFlags(Qt::TextSelectableByMouse);
  setText(text);
}
#endif
