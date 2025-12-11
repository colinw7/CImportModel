#ifndef CQNewGLTextureChooser_H
#define CQNewGLTextureChooser_H

#include <QComboBox>

class CQNewGLControl;

class CQNewGLTextureChooser : public QComboBox {
  Q_OBJECT

 public:
  CQNewGLTextureChooser(CQNewGLControl *control);

  QString textureName() const { return textureName_; }
  void setTextureName(const QString &name);

  void connectSlots(bool b);

 Q_SIGNALS:
  void textureChanged();

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void needsUpdateSlot();
  void currentIndexChanged(int);

 private:
  CQNewGLControl* control_ { nullptr };
  QString         textureName_;
  bool            needsUpdate_ { true };
};

#endif
