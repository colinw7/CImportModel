#ifndef CQTreeWidget_H
#define CQTreeWidget_H

#include <QTreeWidget>

class CQHeaderView;

class CQTreeWidget : public QTreeWidget {
  Q_OBJECT

 public:
  CQTreeWidget(QWidget *parent=nullptr);

  virtual ~CQTreeWidget();

  virtual void addMenuActions(QMenu *menu);

  QTreeWidgetItem *itemFromIndex(const QModelIndex &index) const;

 private Q_SLOTS:
  void customContextMenuSlot(const QPoint &pos);

  void expandAll  (const QModelIndex &ind=QModelIndex());
  void collapseAll(const QModelIndex &ind=QModelIndex());

 private:
  QModelIndex menuIndex_;
};

#endif
