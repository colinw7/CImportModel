#include <CQTreeWidget.h>
#include <QMenu>

CQTreeWidget::
CQTreeWidget(QWidget *parent) :
 QTreeWidget(parent)
{
  setObjectName("treeWidget");

  //--

  setSelectionMode(ExtendedSelection);

  setUniformRowHeights(true);

  setAlternatingRowColors(true);

  //---

  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
          this, SLOT(customContextMenuSlot(const QPoint&)));
}

CQTreeWidget::
~CQTreeWidget()
{
}

void
CQTreeWidget::
customContextMenuSlot(const QPoint &pos)
{
  // Map point to global from the viewport to account for the header.
  auto mpos = viewport()->mapToGlobal(pos);

  auto *menu = new QMenu;

  addMenuActions(menu);

  menuIndex_ = indexAt(pos);

  menu->exec(mpos);

  delete menu;
}

void
CQTreeWidget::
addMenuActions(QMenu *menu)
{
  auto *expandAction   = new QAction("Expand All"  , menu);
  auto *collapseAction = new QAction("Collapse All", menu);

  connect(expandAction  , SIGNAL(triggered()), this, SLOT(expandAll()));
  connect(collapseAction, SIGNAL(triggered()), this, SLOT(collapseAll()));

  menu->addAction(expandAction);
  menu->addAction(collapseAction);
}

void
CQTreeWidget::
expandAll(const QModelIndex &ind)
{
  setExpanded(ind, true);

  for (int r = 0; r < model()->rowCount(ind); ++r) {
    QModelIndex ind1 = model()->index(r, 0, ind);

    expandAll(ind1);
  }
}

void
CQTreeWidget::
collapseAll(const QModelIndex &ind)
{
  setExpanded(ind, false);

  for (int r = 0; r < model()->rowCount(ind); ++r) {
    QModelIndex ind1 = model()->index(r, 0, ind);

    collapseAll(ind1);
  }
}
