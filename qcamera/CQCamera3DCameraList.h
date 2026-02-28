#ifndef CQCamera3DCameraList_H
#define CQCamera3DCameraList_H

#include <QFrame>
#include <QHeaderView>
#include <QItemDelegate>

class CQCamera3DCameraListDelegate;
class CQCamera3DCameraListHeader;
class CQCamera3DCanvas;
class CGLCameraIFace;

class QTableWidget;
class QTableWidgetItem;

class CQCamera3DCameraList : public QFrame {
  Q_OBJECT

 public:
  CQCamera3DCameraList(CQCamera3DCanvas *canvas);

  CQCamera3DCanvas *canvas() const { return canvas_; }

  QTableWidget *table() const { return table_; }

  void connectSlots(bool b);

  int selectedInd() const { return selectedInd_; }

  CGLCameraIFace *getCurrentCamera() const;
  CGLCameraIFace *getSelectedCamera() const;

  CGLCameraIFace * cameraFromIndex(const QModelIndex &index) const;

  void updateAll(bool geom=false);

  void resizeEvent(QResizeEvent *) override;

  QSize sizeHint() const override;

 public Q_SLOTS:
  void updateCameras();

  void invalidate() { valid_ = false; updateCameras(); }

 private Q_SLOTS:
  void itemSelectedSlot(QTableWidgetItem *, QTableWidgetItem *);
  void tableClickSlot(const QModelIndex &index);

 Q_SIGNALS:
  void currentItemChanged();

 private:
  CQCamera3DCanvas*             canvas_      { nullptr };
  QTableWidget*                 table_       { nullptr };
  CQCamera3DCameraListDelegate* delegate_    { nullptr };
  CQCamera3DCameraListHeader*   header_      { nullptr };
  int                           selectedInd_ { -1 };
  bool                          valid_       { false };
};

//---

class CQCamera3DCameraListDelegate : public QItemDelegate {
  Q_OBJECT

 public:
  CQCamera3DCameraListDelegate(CQCamera3DCameraList *list);

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;

 private:
  CQCamera3DCameraList *list_ { nullptr };
};

//---

class CQCamera3DCameraListHeader : public QHeaderView {
  Q_OBJECT

 public:
  CQCamera3DCameraListHeader(CQCamera3DCameraList *list);

  void paintEvent(QPaintEvent *e) override;

  void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override;

  QSize sectionSizeFromContents(int logicalIndex) const override;

  void contextMenuEvent(QContextMenuEvent *event) override;

 public Q_SLOTS:
  void updateColumnSizes();

 private:
  CQCamera3DCameraList* list_        { nullptr };
  int                   menuSection_ { -1 };
};

#endif
