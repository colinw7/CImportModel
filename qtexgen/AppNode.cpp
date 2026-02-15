#include <AppNode.h>
#include <App.h>
#include <NodeArea.h>
#include <NodeInput.h>
#include <NodeOutput.h>

namespace CQTextureGen {

AppNode::
AppNode(App *app, const std::string &name) :
 CQWinWidget(app->nodeArea()), app_(app), name_(name)
{
  connect(this, SIGNAL(closed()), this, SLOT(closeSlot()));
}

void
AppNode::
closeSlot()
{
  app_->removeNode(this);
}

NodeInput *
AppNode::
addInput(NodeInput *input)
{
  input->setNode(this);

  inputs_.push_back(input);

  return input;
}

NodeOutput *
AppNode::
addOutput(NodeOutput *output)
{
  output->setNode(this);

  outputs_.push_back(output);

  return output;
}

NodeIO *
AppNode::
getIOByName(const std::string &name) const
{
  for (auto *io : inputs_) {
    if (io->name() == name)
      return io;
  }

  for (auto *io : outputs_) {
    if (io->name() == name)
      return io;
  }

  return nullptr;
}

void
AppNode::
invalidate()
{
  app_->processNodes();
}

void
AppNode::
reset()
{
  for (auto *input : inputs_)
    input->reset();

  for (auto *output : outputs_)
    output->reset();
}

void
AppNode::
addPipe(AppPipe *pipe)
{
  pipes_.push_back(pipe);
}

void
AppNode::
removePipe(AppPipe *pipe)
{
  pipes_.erase(std::remove(pipes_.begin(), pipes_.end(), pipe), pipes_.end());
}

}
