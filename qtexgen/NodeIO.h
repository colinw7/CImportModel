#ifndef NodeIO_H
#define NodeIO_H

#include <CPoint2D.h>

#include <QRect>

#include <vector>

class QFrame;
class QWidget;

namespace CQTextureGen {

class AppNode;
class AppPipe;

class NodeIO {
 public:
  using Pipes = std::vector<AppPipe *>;

 public:
  NodeIO(const std::string &name) : name_(name) { }

  virtual ~NodeIO() { }

  const std::string &name() const { return name_; }
  void setName(const std::string &s) { name_ = s; }

  AppNode *node() { return node_; }
  void setNode(AppNode *p) { node_ = p; }

  const QRect &rect() const { return rect_; }
  void setRect(const QRect &r) { rect_ = r; }

  const Pipes &pipes() const { return pipes_; }

  bool isProcessed() const { return processed_; }
  void setProcessed(bool b) { processed_ = b; }

  void process();

  virtual bool isInput() const = 0;

  void addPipe(AppPipe *pipe);
  void removePipe(AppPipe *pipe);

  virtual CPoint2D pos() const = 0;

  virtual void reset() = 0;

  CPoint2D inputPos (QFrame *gui, QWidget *w) const;
  CPoint2D outputPos(QFrame *gui, QWidget *w) const;

 protected:
  std::string name_;
  AppNode*    node_ { nullptr };
  QRect       rect_;
  Pipes       pipes_;
  bool        processed_ { false };
};

}

#endif
