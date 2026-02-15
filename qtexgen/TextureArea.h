#ifndef TextureArea_H
#define TextureArea_H

#include <QFrame>

class CQIntegerSpin;

namespace CQTextureGen {

class App;

class TextureArea : public QFrame {
  Q_OBJECT

 public:
  TextureArea(App *app);

  void updateWidgets();

  void connectSlots(bool b);

  void paintEvent(QPaintEvent *) override;

  QSize sizeHint() const override { return QSize(384, 384); }

 private Q_SLOTS:
  void textureSizeSlot(int s);

 private:
  App* app_ { nullptr };

  CQIntegerSpin* textureSizeEdit_;
};

}

#endif
