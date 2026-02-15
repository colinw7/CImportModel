#ifndef ToolBar_H
#define ToolBar_H

#include <QFrame>

namespace CQTextureGen {

class App;

class ToolBar : public QFrame {
  Q_OBJECT

 public:
  ToolBar(App *app);

 private Q_SLOTS:
  void noiseTextureSlot();
  void marbleTextureSlot();
  void imageTextureSlot();
  void mixSlot();
  void mathSlot();
  void binaryMathSlot();
  void remapSlot();
  void textureCoordSlot();
  void debugSlot();
  void colorSlot();
  void separateColorSlot();

  void cubeSlot();
  void sphereSlot();
  void planeSlot();

  void lightSlot();
  void cameraSlot();

  void invalidateSlot();

 private:
  App* app_ { nullptr };
};

}

#endif
