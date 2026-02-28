#include <CQCamera3DAnimTimeEdit.h>
#include <CQCamera3DApp.h>

CQCamera3DAnimTimeEdit::
CQCamera3DAnimTimeEdit(CQCamera3DApp *app) :
 app_(app)
{
}

void
CQCamera3DAnimTimeEdit::
setRange(double min, double max)
{
  CQRealSpin::setRange(min, max);

  setToolTip(QString("Range: %1 -> %2").arg(min).arg(max));
}
