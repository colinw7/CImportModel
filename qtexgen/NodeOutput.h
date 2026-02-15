#ifndef NodeOutput_H
#define NodeOutput_H

#include <NodeIO.h>

namespace CQTextureGen {

class NodeValue;

class NodeOutput : public NodeIO {
 public:
  NodeOutput(const std::string &name) : NodeIO(name) { }

  virtual NodeValue *getValue() = 0;

  bool isInput() const override { return false; }

  void reset() override { }
};

}

#endif
