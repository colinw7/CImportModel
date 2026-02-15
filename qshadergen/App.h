#ifndef App_H
#define App_H

#include <QFrame>

namespace CQShaderGen {

class Canvas;
class Control;

class App : public QFrame {
  Q_OBJECT

 public:
  App();

  Canvas*  canvas () const { return canvas_; }
  Control* control() const { return control_; }

  void setInd(int i);

 private:
  Canvas*  canvas_  { nullptr };
  Control* control_ { nullptr };
};

}

#endif
