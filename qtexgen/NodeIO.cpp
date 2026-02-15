#include <NodeIO.h>
#include <AppNode.h>
#include <NodeArea.h>
#include <App.h>

#include <QFrame>

namespace CQTextureGen {

void
NodeIO::
addPipe(AppPipe *pipe)
{
  pipes_.push_back(pipe);
}

void
NodeIO::
removePipe(AppPipe *pipe)
{
  pipes_.erase(std::remove(pipes_.begin(), pipes_.end(), pipe), pipes_.end());
}

CPoint2D
NodeIO::
inputPos(QFrame *gui, QWidget *w) const
{
  auto p1 = w->mapToGlobal(w->pos());
  auto p2 = gui->mapToGlobal(QPoint(0, 0));
  auto p3 = QPoint(p2.x(), p1.y() + w->height()/2);

  auto *app = node_->app();

  auto p = app->nodeArea()->mapFromGlobal(p3);

  return CPoint2D(p.x(), p.y());
}

CPoint2D
NodeIO::
outputPos(QFrame *gui, QWidget *w) const
{
  auto p1 = w->mapToGlobal(w->pos());
  auto p2 = gui->mapToGlobal(QPoint(gui->width(), 0));
  auto p3 = QPoint(p2.x(), p1.y() + w->height()/2);

  auto *app = node_->app();

  auto p = app->nodeArea()->mapFromGlobal(p3);

  return CPoint2D(p.x(), p.y());
}

}
