#include <AppPipe.h>
#include <NodeInput.h>
#include <NodeOutput.h>
#include <App.h>

namespace CQTextureGen {

AppPipe::
AppPipe(const std::string &name, NodeOutput *output, NodeInput *input) :
 name_(name), output_(output), input_(input)
{
}

void
AppPipe::
process()
{
  auto *value = output_->getValue();

  input_->setValue(value);
}

std::string
AppPipe::
name() const
{
  return output_->name() + " -> " + input_->name();
}

}
