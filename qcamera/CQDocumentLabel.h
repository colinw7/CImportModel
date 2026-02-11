#include <QFrame>
#include <map>

class QPaintEvent;
class CQDocumentLabel;

// html like format string
// @(image name)Text@(link name)Link@(end)

class CQDocumentLabelItem {
 public:
  CQDocumentLabelItem(CQDocumentLabel *label) : label_(label) { }

  virtual ~CQDocumentLabelItem() { }

  virtual QSize sizeHint() const = 0;

  virtual void draw(QPainter *painter, const QRect &rect) = 0;

  void setRect(const QRect &r) { rect_ = r; }

  const QRect &rect() { return rect_; }

 protected:
  CQDocumentLabel* label_ { nullptr };
  QRect            rect_;
};

//------

class CQDocumentLabelLink : public CQDocumentLabelItem {
 public:
  CQDocumentLabelLink(CQDocumentLabel *label, const QString &id, const QString &text) :
   CQDocumentLabelItem(label), id_(id), text_(text) {
  }

  QSize sizeHint() const override;

  const QString &getId() const { return id_; }

  void draw(QPainter *painter, const QRect &rect) override;

 private:
  QString id_;
  QString text_;
};

//------

class CQDocumentLabelImage : public CQDocumentLabelItem {
 public:
  CQDocumentLabelImage(CQDocumentLabel *label, QImage qimage) :
   CQDocumentLabelItem(label), qimage_(qimage) {
  }

  QSize sizeHint() const override;

  void draw(QPainter *painter, const QRect &rect) override;

 private:
  QString name_;
  QImage  qimage_;
};

//------

class CQDocumentLabelText : public CQDocumentLabelItem {
 public:
  CQDocumentLabelText(CQDocumentLabel *label, const QString &text) :
   CQDocumentLabelItem(label), text_(text) {
  }

  QSize sizeHint() const override;

  void draw(QPainter *painter, const QRect &rect) override;

 private:
  QString text_;
};

//------

class CQDocumentLabelDocument {
 public:
  CQDocumentLabelDocument(const QString &def) : def_(def) { }

  const QString &getString() const { return def_; }

 private:
  QString def_;
};

//------

class CQDocumentLabel : public QFrame {
  Q_OBJECT

 public:
  static void addImage(const QString &name, QImage image);

  CQDocumentLabel(QWidget *parent, const QString &text);
  CQDocumentLabel(const QString &text="");

  CQDocumentLabel(QWidget *parent, const CQDocumentLabelDocument &doc);
  CQDocumentLabel(const CQDocumentLabelDocument &doc);

 ~CQDocumentLabel();

  void setText(const QString &sizeText);

  bool isActive() const { return active_; }

  QSize sizeHint() const override;
//QSize minimumSizeHint() const override;

 signals:
  void clicked(const QString &);

 private:
  static QImage getImage(const QString &name);

  void clear();

  void parseDocument(const std::string &def);

  void addLink(const QString &id, const QString &text);
  void addImage(const QString &name);
  void addText(const QString &name);

 private:
  void enterEvent(QEvent *e) override;
  void leaveEvent(QEvent *e) override;

  void paintEvent(QPaintEvent *) override;

  void mousePressEvent(QMouseEvent *) override;
  void mouseReleaseEvent(QMouseEvent *) override;
  void mouseDoubleClickEvent(QMouseEvent *) override;

 private:
  typedef std::vector<CQDocumentLabelItem *> Items;
  typedef std::map<QString, QImage>          Images;

  static Images images_;

  bool  active_  { false };
  bool  pressed_ { false };
  Items items_;
};
