#ifndef XML_H
#define XML_H

#include <string>

namespace CQTextureGen {

class App;

class XML {
 public:
  XML(App *app) :
   app_(app) {
  }

  bool read(const std::string &filename);

 private:
  bool parseReal(const std::string &name, const std::string &value, double *real) const;

 private:
  App* app_ { nullptr };
};

}

#endif
