#ifndef CQCamera3DTextures_H
#define CQCamera3DTextures_H

#include <QFrame>

class CQCamera3DApp;
class CGeomObject3D;
class CGeomTexture;

class CQCamera3DTextures : public QFrame {
  Q_OBJECT

 public:
  CQCamera3DTextures(CQCamera3DApp *model);

  void resizeEvent(QResizeEvent *) override;

  void paintEvent(QPaintEvent *) override;

  void mousePressEvent(QMouseEvent *e) override;

 private:
  void placeTextures();

 Q_SIGNALS:
  void selectionChanged();

 private Q_SLOTS:
  void invalidateSlot();

 private:
  struct TextureData {
    QRect         rect;
    CGeomTexture* texture { nullptr };
    QImage        qimage;
  };

  using TextureDatas = std::vector<TextureData>;

  CQCamera3DApp* app_     { nullptr };
  QColor         bgColor_ { 0, 0, 0 };

  TextureDatas textureDatas_;

  int border_ { 4 };
  int w_      { 100 };
  int h_      { 100 };
};

#endif
