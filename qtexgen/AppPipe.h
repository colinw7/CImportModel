#ifndef AppPipe_H
#define AppPipe_H

#include <CLine2D.h>

#include <vector>
#include <string>

//---

namespace CQTextureGen {

class NodeOutput;
class NodeInput;

class AppPipe {
 public:
  using Lines = std::vector<CLine2D>;

 public:
  AppPipe(const std::string &name, NodeOutput *output, NodeInput *input);

  virtual ~AppPipe() { }

  NodeOutput *output() const { return output_; }
  NodeInput  *input () const { return input_ ; }

  bool isInputProcessed() const { return inputProcessed_; }
  void setInputProcessed(bool b) { inputProcessed_ = b; }

  bool isOutputProcessed() const { return outputProcessed_; }
  void setOutputProcessed(bool b) { outputProcessed_ = b; }

  virtual void process();

  const CPoint2D &p1() const { return p1_; }
  void setP1(const CPoint2D &v) { p1_ = v; }

  const CPoint2D &p2() const { return p2_; }
  void setP2(const CPoint2D &v) { p2_ = v; }

  void clearLines() { lines_.clear(); }
  void addLine(const CLine2D &line) { lines_.push_back(line); }
  const Lines &lines() const { return lines_; }

  std::string name() const;

 private:
  std::string name_;
  NodeOutput* output_ { nullptr };
  NodeInput*  input_  { nullptr };

  bool inputProcessed_  { false };
  bool outputProcessed_ { false };

  CPoint2D p1_;
  CPoint2D p2_;
  Lines    lines_;
};

}

#endif
