#ifndef CQCamera3DAnimTimeEdit_H
#define CQCamera3DAnimTimeEdit_H

#include <CQRealSpin.h>

class CQCamera3DApp;

class CQCamera3DAnimTimeEdit : public CQRealSpin {
  Q_OBJECT

 public:
  CQCamera3DAnimTimeEdit(CQCamera3DApp *app);

  void setRange(double min, double max);

 private:
  CQCamera3DApp* app_ { nullptr };
};

#endif
