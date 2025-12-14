#ifndef CQCamera3DTextures_H
#define CQCamera3DTextures_H

#include <QFrame>

class CQCamera3DApp;
class CGeomObject3D;

class CQCamera3DTextures : public QFrame {
 public:
  CQCamera3DTextures(CQCamera3DApp *model);

  void paintEvent(QPaintEvent *) override;

 private:
  CQCamera3DApp* app_     { nullptr };
  QColor         bgColor_ { 0, 0, 0 };
};

#endif
