#include <CDeflate.h>
#include <zlib.h>

namespace CDeflate {

bool
deflate_bytes(const std::vector<unsigned char> &idata, unsigned int idataLen,
              std::vector<unsigned char> &odata, unsigned int odataLen)
{
  ulong ilen = idataLen;
  ulong olen = odataLen;
  if (uncompress(odata.data(), &olen, idata.data(), ilen) != Z_OK)
    return false;

#if 0
  z_stream zs;
  memset(&zs, 0, sizeof(zs));

  if (deflateInit(&zs) != Z_OK)
    return false;

  zs.next_in  = const_cast<unsigned char *>(idata.data());
  zs.avail_in = idataLen;

//auto buffer_size = deflateBound(&zs, dataLen);
//std::vector<unsigned char> out_buffer(buffer_size);
//zs.next_out = out_buffer.data();
//zs.avail_out = static_cast<unsigned int>(buffer_size);

  zs.next_out  = odata.data();
  zs.avail_out = odataLen;

  deflate(&zs, Z_FINISH);
  deflateEnd(&zs);
#endif

  return true;
}

}
