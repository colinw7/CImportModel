#ifndef CQCamera3DObjectsList_H
#define CQCamera3DObjectsList_H

#include <QFrame>
#include <QItemDelegate>
#include <QHeaderView>

class CQCamera3DCanvas;
class CQCamera3DObjectsListDelegate;
class CQCamera3DObjectsListHeader;

class CQTreeWidget;
class CGeomObject3D;

class QTreeWidgetItem;

class CQCamera3DObjectsList : public QFrame {
  Q_OBJECT

 public:
  CQCamera3DObjectsList(CQCamera3DCanvas *canvas);

  CQTreeWidget *tree() const { return tree_; }

  const QIcon &selectedIcon() const { return selectedIcon_; }
  const QIcon &deselectedIcon() const { return deselectedIcon_; }

  const QIcon &visibleIcon() const { return visibleIcon_; }
  const QIcon &invisibleIcon() const { return invisibleIcon_; }

  void connectSlots(bool b);

  int selectedInd() const { return selectedInd_; }

  bool anyVisible() const { return anyVisible_; }
  bool anySelected() const { return anySelected_; }

  CGeomObject3D *getSelectedIndObject() const;
  CGeomObject3D *getObjectListSelected() const;

  CGeomObject3D *objectFromIndex(const QModelIndex &index) const;

  void resizeEvent(QResizeEvent *) override;

  void paintEvent(QPaintEvent *e) override;

  void updateState();

  void selectAllObjects(bool);
  void showAllObjects  (bool);

  void updateAll();

 public Q_SLOTS:
  void updateObjects();

 private Q_SLOTS:
  void itemSelectedSlot(QTreeWidgetItem *, QTreeWidgetItem *);
  void treeClickSlot(const QModelIndex &);

 private:
  QTreeWidgetItem *createObjectItem(CGeomObject3D *object);

 Q_SIGNALS:
  void currentItemChanged();

 private:
  using ObjectItem = std::map<CGeomObject3D *, QTreeWidgetItem *>;

  CQCamera3DCanvas*              canvas_      { nullptr };
  CQTreeWidget*                  tree_        { nullptr };
  CQCamera3DObjectsListDelegate* delegate_    { nullptr };
  CQCamera3DObjectsListHeader*   header_      { nullptr };
  ObjectItem                     objectItem_;
  int                            selectedInd_ { -1 };

  bool anyVisible_  { false };
  bool anySelected_ { false };

  QIcon selectedIcon_;
  QIcon deselectedIcon_;

  QIcon visibleIcon_;
  QIcon invisibleIcon_;
};

//---

class CQCamera3DObjectsListDelegate : public QItemDelegate {
  Q_OBJECT

 public:
  CQCamera3DObjectsListDelegate(CQCamera3DObjectsList *list);

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;

 private:
  CQCamera3DObjectsList *list_ { nullptr };
};

//---

class CQCamera3DObjectsListHeader : public QHeaderView {
  Q_OBJECT

 public:
  CQCamera3DObjectsListHeader(CQCamera3DObjectsList *list);

  int iconWidth() const { return iconWidth_; }

  void paintEvent(QPaintEvent *e) override;

  void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override;

  QSize sectionSizeFromContents(int logicalIndex) const override;

  void contextMenuEvent(QContextMenuEvent *event) override;

 public Q_SLOTS:
  void clickSlot(int);

  void updateColumnSizes();

 private:
  CQCamera3DObjectsList* list_      { nullptr };
  mutable int            iconWidth_ { 16 };

  int menuSection_ { -1 };
};

#endif
