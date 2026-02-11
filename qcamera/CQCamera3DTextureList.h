#ifndef CQCamera3DTextureList_H
#define CQCamera3DTextureList_H

#include <QFrame>
#include <QHeaderView>
#include <QItemDelegate>

class CQCamera3DTextureListDelegate;
class CQCamera3DTextureListHeader;
class CQCamera3DCanvas;
class CGeomTexture;

class QTableWidget;
class QTableWidgetItem;

class CQCamera3DTextureList : public QFrame {
  Q_OBJECT

 public:
  CQCamera3DTextureList(CQCamera3DCanvas *canvas);

  QTableWidget *table() const { return table_; }

  void connectSlots(bool b);

  int selectedInd() const { return selectedInd_; }

  CGeomTexture *getCurrentTexture() const;
  CGeomTexture *getSelectedTexture() const;

  CGeomTexture *textureFromIndex(const QModelIndex &index) const;

  void updateAll();

  void resizeEvent(QResizeEvent *) override;

  QSize sizeHint() const override;

 public Q_SLOTS:
  void updateTextures();

  void invalidate() { valid_ = false; }

 private Q_SLOTS:
  void needsUpdateSlot();

  void itemSelectedSlot(QTableWidgetItem *, QTableWidgetItem *);
  void tableClickSlot(const QModelIndex &index);

 Q_SIGNALS:
  void currentItemChanged();

 private:
  CQCamera3DCanvas*              canvas_      { nullptr };
  QTableWidget*                  table_       { nullptr };
  CQCamera3DTextureListDelegate* delegate_    { nullptr };
  CQCamera3DTextureListHeader*   header_      { nullptr };
  int                            selectedInd_ { -1 };
  bool                           valid_       { false };
};

//---

class CQCamera3DTextureListDelegate : public QItemDelegate {
  Q_OBJECT

 public:
  CQCamera3DTextureListDelegate(CQCamera3DTextureList *list);

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;

 private:
  CQCamera3DTextureList *list_ { nullptr };
};

//---

class CQCamera3DTextureListHeader : public QHeaderView {
  Q_OBJECT

 public:
  CQCamera3DTextureListHeader(CQCamera3DTextureList *list);

  void paintEvent(QPaintEvent *e) override;

  void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override;

  QSize sectionSizeFromContents(int logicalIndex) const override;

  void contextMenuEvent(QContextMenuEvent *event) override;

 public Q_SLOTS:
  void updateColumnSizes();

 private:
  CQCamera3DTextureList* list_        { nullptr };
  int                    menuSection_ { -1 };
};

#endif
