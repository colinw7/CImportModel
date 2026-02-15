#ifndef NodeInput_H
#define NodeInput_H

#include <NodeIO.h>

namespace CQTextureGen {

class NodeValue;

class NodeInput : public NodeIO {
 public:
  NodeInput(const std::string &name) : NodeIO(name) { }

  NodeValue *getValue() const { return value_; }

  virtual void setValue(NodeValue *value) { value_ = value; }

  bool isInput() const override { return true; }

  void reset() override { value_ = nullptr; }

 protected:
  NodeValue* value_ { nullptr };
};

}

#endif
