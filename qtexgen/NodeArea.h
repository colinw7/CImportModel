#ifndef NodeArea_H
#define NodeArea_H

#include <QFrame>

class CQRubberBand;

namespace CQTextureGen {

class App;
class AppPipe;
class NodeIO;

class NodeArea : public QFrame {
  Q_OBJECT

 public:
  struct MouseData {
    QPoint   pressPos;
    QPoint   movePos;
    int      button  { 0 };
    bool     pressed { false };
    AppPipe* pipe    { nullptr };
    NodeIO*  node    { nullptr };
  };

 public:
  NodeArea(App *app);

  void paintEvent(QPaintEvent *) override;

  void mousePressEvent  (QMouseEvent *) override;
  void mouseMoveEvent   (QMouseEvent *) override;
  void mouseReleaseEvent(QMouseEvent *) override;

 private:
  void getMouseObjects(const QPoint pos, MouseData &mouseData) const;

 private:
  App* app_ { nullptr };

  MouseData mouseData_;

  CQRubberBand* connectLine1_ { nullptr };
  CQRubberBand* connectLine2_ { nullptr };
  CQRubberBand* connectLine3_ { nullptr };
};

}

#endif
