#include <CQDocumentLabel.h>
#include <CQHtmlTextPainter.h>

#include <QApplication>
#include <QPainter>
#include <QStylePainter>
#include <QStyleOptionFrame>
#include <QPaintEvent>

#include <cassert>

class CQDocumentLabelLabel;

CQDocumentLabel::Images CQDocumentLabel::images_;

//---------

namespace CQDocumentLabelUtil {
  void drawText(QPainter *painter, const QString &text, const QPalette &palette,
                const QRect &rect, bool active) {
    CQHtmlTextPainter textPainter;

    textPainter.setText(text);

    if (active)
      textPainter.setTextColor(palette.highlightedText().color());
    else
      textPainter.setTextColor(palette.text().color());

    textPainter.drawInRect(painter, rect);
  }
}

//---------

void
CQDocumentLabel::
addImage(const QString &name, QImage image)
{
  images_[name] = image;
}

QImage
CQDocumentLabel::
getImage(const QString &name)
{
  return images_[name];
}

CQDocumentLabel::
CQDocumentLabel(QWidget *parent, const QString &text) :
 QFrame(parent)
{
  setFrameShape(QFrame::NoFrame);

  addLink("", text);

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

CQDocumentLabel::
CQDocumentLabel(const QString &text) :
 QFrame(nullptr)
{
  addLink("", text);

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

CQDocumentLabel::
CQDocumentLabel(QWidget *parent, const CQDocumentLabelDocument &doc) :
 QFrame(parent)
{
  setFrameShape(QFrame::NoFrame);

  parseDocument(doc.getString().toStdString());

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

CQDocumentLabel::
CQDocumentLabel(const CQDocumentLabelDocument &doc) :
 QFrame(nullptr)
{
  parseDocument(doc.getString().toStdString());

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

CQDocumentLabel::
~CQDocumentLabel()
{
  clear();
}

void
CQDocumentLabel::
clear()
{
  Items::iterator p1, p2;

  for (auto *item : items_)
    delete item;

  items_.clear();
}

void
CQDocumentLabel::
setText(const QString &text)
{
  clear();

  addLink("", text);

  hide();
  show();
}

void
CQDocumentLabel::
parseDocument(const std::string &str)
{
  std::string link;

  uint pos = 0;
  uint len = uint(str.size());

  while (pos < len) {
    if (str[pos] != '@') {
      std::string text;

      while (pos < len && str[pos] != '@') {
        if (str[pos] == '\\' && pos + 1 < len)
          ++pos;

        text += str[pos++];
      }

      if (text.size()) {
        if (! link.empty())
          addLink(link.c_str(), text.c_str());
        else
          addText(text.c_str());

        link = "";
      }
    }
    else {
      ++pos;

      char c1 = (pos < len ? str[pos++] : '\0');
      char c2 = c1;

      if      (c1 == '(') c2 = ')';
      else if (c1 == '[') c2 = ']';
      else if (c1 == '{') c2 = '}';
      else if (c1 == '<') c2 = '>';

      std::string text;

      while (pos < len && str[pos] != c2)
        text += str[pos++];

      if (str[pos] == c2)
        ++pos;

      uint pos1 = 0;
      uint len1 = uint(text.size());

      while (pos1 < len1 && ! isspace(text[pos1]))
        ++pos1;

      std::string tag = text.substr(0, pos1);

      while (pos1 < len1 && isspace(text[pos1]))
        ++pos1;

      if      (tag == "image")
        addImage(text.substr(pos1).c_str());
      else if (tag == "link")
        link = text.substr(pos1);
    }
  }
}

void
CQDocumentLabel::
addLink(const QString &id, const QString &text)
{
  items_.push_back(new CQDocumentLabelLink(this, id, text));
}

void
CQDocumentLabel::
addImage(const QString &name)
{
  items_.push_back(new CQDocumentLabelImage(this, getImage(name)));
}

void
CQDocumentLabel::
addText(const QString &text)
{
  items_.push_back(new CQDocumentLabelText(this, text));
}

QSize
CQDocumentLabel::
sizeHint() const
{
  int w = 0, h = 0;

  Items::const_iterator p1, p2;

  for (auto *item : items_) {
    auto s = item->sizeHint();

    w += s.width();
    h =  std::max(h, s.height());
  }

  return QSize(w, h);
}

#if 0
QSize
CQDocumentLabel::
minimumSizeHint() const
{
  QFontMetrics fm(font());

  return QSize(fm.horizontalAdvance(text_), fm.height());
}
#endif

void
CQDocumentLabel::
enterEvent(QEvent *)
{
  active_ = true;

  update();
}

void
CQDocumentLabel::
leaveEvent(QEvent *)
{
  active_ = false;

  update();
}

void
CQDocumentLabel::
paintEvent(QPaintEvent *)
{
  QStylePainter painter(this);

  painter.setPen(palette().color(QPalette::Text));

  QStyleOptionFrame opt;

  opt.initFrom(this);

  if (frameShape() != QFrame::NoFrame)
    painter.drawPrimitive(QStyle::PE_Frame, opt);

  //------

  QSize s = sizeHint();

  int x = 0, y = 0;

  for (auto *item : items_) {
    QSize s1 = item->sizeHint();

    QRect r(x, y, s1.width(), s.height());

    item->setRect(r);

    item->draw(&painter, r);

    x += s1.width();
  }
}

void
CQDocumentLabel::
mousePressEvent(QMouseEvent *)
{
  pressed_ = true;

  update();
}

void
CQDocumentLabel::
mouseReleaseEvent(QMouseEvent *e)
{
  if (! pressed_) return;

  for (auto *item : items_) {
    if (item->rect().contains(e->pos())) {
      CQDocumentLabelLink *link = dynamic_cast<CQDocumentLabelLink *>(item);

      if (link)
        emit clicked(QString(link->getId()));

      break;
    }
  }

  pressed_ = false;

  update();
}

void
CQDocumentLabel::
mouseDoubleClickEvent(QMouseEvent *e)
{
  pressed_ = true;

  mouseReleaseEvent(e);
}

//-----------

QSize
CQDocumentLabelLink::
sizeHint() const
{
  QFontMetrics fm(label_->font());

  return QSize(fm.horizontalAdvance(text_) + 4, fm.height() + 4);
}

void
CQDocumentLabelLink::
draw(QPainter *painter, const QRect &r)
{
  //if (pressed_) r.adjust(1, 1, 1, 1);

  QString text;

  if (label_->isActive())
    text = "<u><font color=\"#0000ff\">" + text_ + "</font></u>";
  else
    text = "<font color=\"#0000ff\">" + text_ + "</font>";

  CQDocumentLabelUtil::drawText(painter, text, label_->palette(), r, false);
}

//-----------

QSize
CQDocumentLabelImage::
sizeHint() const
{
  return qimage_.size();
}

void
CQDocumentLabelImage::
draw(QPainter *painter, const QRect &r)
{
  int dy = r.height() - qimage_.height();

  painter->drawImage(r.left(), r.top() + dy, qimage_);
}

//-----------

QSize
CQDocumentLabelText::
sizeHint() const
{
  QFontMetrics fm(label_->font());

  return QSize(fm.horizontalAdvance(text_) + 4, fm.height() + 4);
}

void
CQDocumentLabelText::
draw(QPainter *painter, const QRect &r)
{
  CQDocumentLabelUtil::drawText(painter, text_, label_->palette(), r, false);
}
