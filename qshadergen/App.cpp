#include <App.h>
#include <Canvas.h>
#include <Control.h>

#include <QHBoxLayout>

namespace CQShaderGen {

class Canvas;

App::
App()
{
  auto *layout = new QHBoxLayout(this);

  canvas_  = new Canvas (this);
  control_ = new Control(this);

  layout->addWidget(canvas_);
  layout->addWidget(control_);
}

void
App::
setInd(int /*i*/)
{
  //canvas_->setInd(i);
}

}
