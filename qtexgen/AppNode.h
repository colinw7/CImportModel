#ifndef AppNode_H
#define AppNode_H

#include <CQWinWidget.h>

namespace CQTextureGen {

class App;
class AppPipe;
class NodeInput;
class NodeOutput;
class NodeIO;

class AppNode : public CQWinWidget {
  Q_OBJECT

 public:
  using Inputs  = std::vector<NodeInput *>;
  using Outputs = std::vector<NodeOutput *>;
  using Pipes   = std::vector<AppPipe *>;

 public:
  AppNode(App *app, const std::string &name);

  virtual ~AppNode() { }

  App *app() const { return app_; }

  const std::string &id() const { return id_; }
  void setId(const std::string &s) { id_ = s; }

  const std::string &name() const { return name_; }
  void setName(const std::string &s) { name_ = s; }

  //---

  template<typename T>
  T *addInputT(T *input) {
    return dynamic_cast<T *>(addInput(input));
  }

  template<typename T>
  T *addOutputT(T *output) {
    return dynamic_cast<T *>(addOutput(output));
  }

  NodeInput  *addInput(NodeInput  *input);
  NodeOutput *addOutput(NodeOutput *output);

  const Inputs  &inputs () const { return inputs_ ; }
  const Outputs &outputs() const { return outputs_; }

  NodeIO *getIOByName(const std::string &name) const;

  //---

  const Pipes &pipes() const { return pipes_; }

  void addPipe(AppPipe *pipe);
  void removePipe(AppPipe *pipe);

  //---

  bool isProcessed() const { return processed_; }
  void setProcessed(bool b) { processed_ = b; }

  void reset();

  virtual void process() = 0;

 public Q_SLOTS:
  void invalidate();

 private Q_SLOTS:
  void closeSlot();

 protected:
  App* app_ { nullptr };

  std::string id_;
  std::string name_;

  Inputs  inputs_;
  Outputs outputs_;
  Pipes   pipes_;
  bool    processed_ { false };
};

}

#endif
