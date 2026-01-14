#ifndef CQCamera3DTextureList_H
#define CQCamera3DTextureList_H

#include <QFrame>

class CQCamera3DCanvas;
class CGeomTexture;

class QListWidget;
class QListWidgetItem;

class CQCamera3DTextureList : public QFrame {
  Q_OBJECT

 public:
  CQCamera3DTextureList(CQCamera3DCanvas *canvas);

  void connectSlots(bool b);

  int selectedInd() const { return selectedInd_; }

  CGeomTexture *getCurrentTexture() const;
  CGeomTexture *getSelectedTexture() const;

 public Q_SLOTS:
  void updateTextures();

  void invalidate() { valid_ = false; }

 private Q_SLOTS:
  void needsUpdateSlot();

  void itemSelectedSlot(QListWidgetItem *, QListWidgetItem *);

 Q_SIGNALS:
  void currentItemChanged();

 private:
  CQCamera3DCanvas* canvas_      { nullptr };
  QListWidget*      list_        { nullptr };
  int               selectedInd_ { -1 };
  bool              valid_       { false };
};

#endif
