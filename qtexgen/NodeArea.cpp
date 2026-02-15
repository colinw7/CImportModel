#include <NodeArea.h>
#include <App.h>
#include <AppPipe.h>
#include <AppNode.h>
#include <NodeInput.h>
#include <NodeOutput.h>

#include <CQRubberBand.h>

#include <QPainter>
#include <QMouseEvent>

namespace CQTextureGen {

NodeArea::
NodeArea(App *app) :
 app_(app)
{
}

void
NodeArea::
paintEvent(QPaintEvent *)
{
  auto nodeColor = QColor(80, 80, 80);
  auto edgeColor = QColor(240, 240, 240);

  int is = 32;

  QPainter painter(this);

  painter.fillRect(rect(), QColor(150, 150, 150));

  for (auto *node : app_->nodes()) {
    for (auto *input : node->inputs()) {
      auto p = input->pos();

      painter.setBrush(nodeColor);

      input->setRect(QRect(p.x - is/2, p.y - is/2, is, is));

      painter.drawEllipse(input->rect());
    }

    for (auto *output : node->outputs()) {
      auto p = output->pos();

      painter.setBrush(nodeColor);

      output->setRect(QRect(p.x - is/2, p.y - is/2, is, is));

      painter.drawEllipse(output->rect());
    }
  }

  QPen pen;

  pen.setColor(edgeColor);
  pen.setWidth(4);

  painter.setPen(pen);

  for (auto *pipe : app_->pipes()) {
    pipe->setP1(pipe->output()->pos());
    pipe->setP2(pipe->input ()->pos());

    //painter.drawLine(pipe->p1().x, pipe->p1().y, pipe->p2().x, pipe->p2().y);

    pipe->clearLines();

    auto x1 = pipe->p1().x;
    auto y1 = pipe->p1().y;
    auto x2 = pipe->p2().x;
    auto y2 = pipe->p2().y;

    auto xm = (x1 + x2)/2;
    auto ym = (y1 + y2)/2;

    auto drawHLine = [&](const QPoint &p1, const QPoint &p2) {
      auto x1 = p1.x(), x2 = p2.x();
      if (x1 > x2) std::swap(x1, x2);
      auto y = p1.y();

      painter.drawLine(x1, y, x2, y);

      CLine2D line(CPoint2D(x1, y), CPoint2D(x2, y));
      line.setSegment(true);
      pipe->addLine(line);
    };

    auto drawVLine = [&](const QPoint &p1, const QPoint &p2) {
      auto y1 = p1.y(), y2 = p2.y();
      if (y1 > y2) std::swap(y1, y2);
      auto x = p1.x();

      painter.drawLine(x, y1, x, y2);

      CLine2D line(CPoint2D(x, y1), CPoint2D(x, y2));
      line.setSegment(true);
      pipe->addLine(line);
    };

    if (x2 - x1 > y2 - y1) {
      drawHLine(QPoint(x1, y1), QPoint(xm, y1));
      drawVLine(QPoint(xm, y1), QPoint(xm, y2));
      drawHLine(QPoint(xm, y2), QPoint(x2, y2));
    }
    else {
      drawVLine(QPoint(x1, y1), QPoint(x1, ym));
      drawHLine(QPoint(x1, ym), QPoint(x2, ym));
      drawVLine(QPoint(x2, ym), QPoint(x2, y2));
    }
  }
}

void
NodeArea::
mousePressEvent(QMouseEvent *e)
{
  mouseData_.pressPos = e->pos();
  mouseData_.pressed  = true;
  mouseData_.button   = e->button();

  getMouseObjects(mouseData_.pressPos, mouseData_);

  if      (e->button() == Qt::LeftButton) {
    if      (mouseData_.node)
      std::cerr << "Click: " << mouseData_.node->name() << "\n";
    else if (mouseData_.pipe)
      std::cerr << "Click: " << mouseData_.pipe->name() << "\n";
  }
  else if (e->button() == Qt::MiddleButton) {
    connectLine1_ = new CQRubberBand(QRubberBand::Line, this);
    connectLine2_ = new CQRubberBand(QRubberBand::Line, this);
    connectLine3_ = new CQRubberBand(QRubberBand::Line, this);
  }
  else if (e->button() == Qt::RightButton) {
    if (mouseData_.pipe) {
      std::cerr << "Delete: " << mouseData_.pipe->name() << "\n";

      app_->removePipe(mouseData_.pipe);

      app_->processNodes();

      update();
    }
  }
}

void
NodeArea::
mouseMoveEvent(QMouseEvent *e)
{
  if (mouseData_.pressed)
    mouseData_.movePos = e->pos();

  if (mouseData_.button == Qt::MiddleButton) {
    auto x1 = mouseData_.pressPos.x();
    auto y1 = mouseData_.pressPos.y();
    auto x2 = mouseData_.movePos .x();
    auto y2 = mouseData_.movePos .y();

    auto xm = (x1 + x2)/2;
    auto ym = (y1 + y2)/2;

    auto setConnectHLine = [](CQRubberBand *connectLine, const QPoint &p1, const QPoint &p2) {
      auto x1 = p1.x(), x2 = p2.x();
      if (x1 > x2) std::swap(x1, x2);
      auto y = p1.y();

      connectLine->setGeometry(QRect(x1, y, x2 - x1, 1));
    };

    auto setConnectVLine = [](CQRubberBand *connectLine, const QPoint &p1, const QPoint &p2) {
      auto y1 = p1.y(), y2 = p2.y();
      if (y1 > y2) std::swap(y1, y2);
      auto x = p1.x();

      connectLine->setGeometry(QRect(x, y1, 1, y2 - y1));
    };

    if (x2 - x1 > y2 - y1) {
      setConnectHLine(connectLine1_, QPoint(x1, y1), QPoint(xm, y1));
      setConnectVLine(connectLine2_, QPoint(xm, y1), QPoint(xm, y2));
      setConnectHLine(connectLine3_, QPoint(xm, y2), QPoint(x2, y2));
    }
    else {
      setConnectVLine(connectLine1_, QPoint(x1, y1), QPoint(x1, ym));
      setConnectHLine(connectLine2_, QPoint(x1, ym), QPoint(x2, ym));
      setConnectVLine(connectLine3_, QPoint(x2, ym), QPoint(x2, y2));
    }

    connectLine1_->show();
    connectLine2_->show();
    connectLine3_->show();

    update();
  }
}

void
NodeArea::
mouseReleaseEvent(QMouseEvent *e)
{
  mouseData_.movePos = e->pos();
  mouseData_.pressed = false;

  if (mouseData_.button == Qt::MiddleButton) {
    MouseData releaseData;

    getMouseObjects(mouseData_.movePos, releaseData);

    if (mouseData_.node && releaseData.node) {
      if      (! mouseData_.node->isInput() &&   releaseData.node->isInput())
        app_->addPipe("", dynamic_cast<CQTextureGen::NodeOutput *>(mouseData_ .node),
                          dynamic_cast<CQTextureGen::NodeInput  *>(releaseData.node));
      else if (  mouseData_.node->isInput() && ! releaseData.node->isInput())
        app_->addPipe("", dynamic_cast<CQTextureGen::NodeOutput *>(releaseData.node),
                          dynamic_cast<CQTextureGen::NodeInput  *>(mouseData_ .node));

      app_->processNodes();

      update();
    }
  }

  delete connectLine1_;
  delete connectLine2_;
  delete connectLine3_;
  connectLine1_ = nullptr;
  connectLine2_ = nullptr;
  connectLine3_ = nullptr;
}

void
NodeArea::
getMouseObjects(const QPoint pos, MouseData &mouseData) const
{
  mouseData.node = nullptr;
  mouseData.pipe = nullptr;

  for (auto *node : app_->nodes()) {
    for (auto *input : node->inputs()) {
      if (input->rect().contains(pos))
        mouseData.node = input;
    }

    for (auto *output : node->outputs()) {
      if (output->rect().contains(pos))
        mouseData.node = output;
    }
  }

  CPoint2D pp(pos.x(), pos.y());

  for (auto *pipe : app_->pipes()) {
#if 0
    auto line = CLine2D(pipe->p1(), pipe->p2());
    line.setSegment(true);

    double d;
    if (line.pointDistance(pp, &d) && d < 4) {
      mouseData.pipe = pipe;
    }
#else
    for (const auto &line : pipe->lines()) {
      double d;
      if (line.pointDistance(pp, &d) && d < 4) {
        mouseData.pipe = pipe;
        break;
      }
    }
#endif
  }
}

}
