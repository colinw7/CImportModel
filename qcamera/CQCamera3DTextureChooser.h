#ifndef CQCamera3DTextureChooser_H
#define CQCamera3DTextureChooser_H

#include <QComboBox>

class CQCamera3DApp;

class CQCamera3DTextureChooser : public QComboBox {
  Q_OBJECT

 public:
  CQCamera3DTextureChooser(CQCamera3DApp *app);

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
  CQCamera3DApp* app_         { nullptr };
  QString        textureName_;
  bool           needsUpdate_ { true };
};

#endif
