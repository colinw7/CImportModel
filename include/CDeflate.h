#ifndef CDeflate_H
#define CDeflate_H

#include <vector>

namespace CDeflate {

bool
deflate_bytes(const std::vector<unsigned char> &idata, unsigned int idataLen,
              std::vector<unsigned char> &odata, unsigned int odataLen);

}

#endif
