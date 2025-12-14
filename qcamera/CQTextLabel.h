#ifndef CQTextLabel_H
#define CQTextLabel_H

//#include <QLineEdit>
#include <QLabel>

/*!
 * \brief label widget supporting copy
 */
#if 0
class CQTextLabel : public QLineEdit {
 public:
  explicit CQTextLabel(QWidget *parent=nullptr, const QString &text="");
  explicit CQTextLabel(const QString &text, QWidget *parent=nullptr);

 ~CQTextLabel() override { }
};
#else
class CQTextLabel : public QLabel {
 public:
  explicit CQTextLabel(QWidget *parent=nullptr, const QString &text="");
  explicit CQTextLabel(const QString &text, QWidget *parent=nullptr);

 ~CQTextLabel() override { }
};
#endif

#endif
