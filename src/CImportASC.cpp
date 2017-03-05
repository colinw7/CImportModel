#include <CImportASC.h>
#include <CStrUtil.h>
#include <CRegExp.h>
#include <CStrParse.h>

CImportASC::
CImportASC() :
 debug_(false), flags_(0)
{
}

CImportASC::
~CImportASC()
{
}

bool
CImportASC::
read(CFile &file)
{
  file_ = &file;

  if (! readName())
    return false;

  if (! readNumObjects())
    return false;

  if (! readVertices())
    return false;

  if (! readFaces())
    return false;

  return true;
}

bool
CImportASC::
readName()
{
  pattern_ = "Named object: *\"\\(.*\\)\"";

  bool found = false;

  while (readNextLine()) {
    if (CRegExpUtil::parse(line_, pattern_, match_strs_)) {
      name_ = match_strs_[0];

      found = true;

      break;
    }
  }

  if (! found)
    return false;

  return true;
}

bool
CImportASC::
readNumObjects()
{
  pattern_  = "Tri-mesh, Vertices:";
  pattern_ += " *\\([0-9][0-9]*\\) *Faces: *\\([0-9][0-9]*\\)";

  bool found = false;

  while (readNextLine()) {
    if (CRegExpUtil::parse(line_, pattern_, match_strs_)) {
      if (! CStrUtil::toInteger(match_strs_[0], &num_vertices_)) return false;
      if (! CStrUtil::toInteger(match_strs_[1], &num_faces_   )) return false;

      found = true;

      break;
    }
  }

  if (! found)
    return false;

  return true;
}

bool
CImportASC::
readVertices()
{
  pattern_ = "Vertex list:";

  bool found = false;

  while (readNextLine()) {
    if (CRegExpUtil::parse(line_, pattern_)) {
      found = true;
      break;
    }
  }

  if (! found)
    return false;

  pattern_  = "Vertex *\\([0-9][0-9]*\\):";
  pattern_ += " *X:\\([-.0-9][-.0-9]*\\)";
  pattern_ += " *Y:\\([-.0-9][-.0-9]*\\)";
  pattern_ += " *Z:\\([-.0-9][-.0-9]*\\)";

  int i = 0;

  while (i < num_vertices_) {
    if (! readNextLine())
      return false;

    if (! CRegExpUtil::parse(line_, pattern_, match_strs_))
      return false;

    double x, y, z;

    if (! CStrUtil::toReal(match_strs_[1], &x)) return false;
    if (! CStrUtil::toReal(match_strs_[2], &y)) return false;
    if (! CStrUtil::toReal(match_strs_[3], &z)) return false;

    if (flags_ & INVERT_X) x = -x;
    if (flags_ & INVERT_Y) y = -y;
    if (flags_ & INVERT_Z) z = -z;

    if (flags_ & SWAP_XY) std::swap(x, y);
    if (flags_ & SWAP_XZ) std::swap(x, z);
    if (flags_ & SWAP_YZ) std::swap(y, z);

    addVertex(x, y, z);

    ++i;
  }

  return true;
}

bool
CImportASC::
readFaces()
{
  pattern_ = "Face list:";

  bool found = false;

  while (readNextLine()) {
    if (CRegExpUtil::parse(line_, pattern_)) {
      found = true;
      break;
    }
  }

  if (! found)
    return false;

  int i = 0;

  while (i < num_faces_) {
    if (! readNextLine())
      return false;

    pattern_  = "Face *\\([0-9][0-9]*\\):";
    pattern_ += " *A:\\([0-9][0-9]*\\)";
    pattern_ += " *B:\\([0-9][0-9]*\\)";
    pattern_ += " *C:\\([0-9][0-9]*\\)";
    pattern_ += " *AB:\\([0-9][0-9]*\\)";
    pattern_ += " *BC:\\([0-9][0-9]*\\)";
    pattern_ += " *CA:\\([0-9][0-9]*\\)";

    if (! CRegExpUtil::parse(line_, pattern_, match_strs_))
      return false;

    int i1, i2, i3;

    if (! CStrUtil::toInteger(match_strs_[1], &i1)) return false;
    if (! CStrUtil::toInteger(match_strs_[2], &i2)) return false;
    if (! CStrUtil::toInteger(match_strs_[3], &i3)) return false;

    if (flags_ & INVERT_WINDING_ORDER)
      std::swap(i1, i3);

    if (! readNextLine())
      return false;

    pattern_  = "Material: *\"";
    pattern_ += "r\\([0-9][0-9]*\\)";
    pattern_ += "g\\([0-9][0-9]*\\)";
    pattern_ += "b\\([0-9][0-9]*\\)";
    pattern_ += "a\\([0-9][0-9]*\\)\"";

    if (! CRegExpUtil::parse(line_,  pattern_, match_strs_))
      return false;

    int r, g, b;

    if (! CStrUtil::toInteger(match_strs_[0], &r)) return false;
    if (! CStrUtil::toInteger(match_strs_[1], &g)) return false;
    if (! CStrUtil::toInteger(match_strs_[2], &b)) return false;

    if (! readNextLine())
      return false;

    pattern_  = "Smoothing: *\\([0-9][0-9]*\\)";

    if (! CRegExpUtil::parse(line_,  pattern_, match_strs_))
      return false;

    addITriangle(i1, i2, i3, CRGBA(r/255.0, g/255.0, b/255.0));

    ++i;
  }

  return true;
}

bool
CImportASC::
readNextLine()
{
  char c;

  while (true) {
    if (! file_->readLine(line_))
      return false;

    CStrParse parse(line_);

    parse.skipSpace();

    if (! parse.readChar(&c))
      continue;

    if (c == '#')
      continue;

    break;
  }

  return true;
}
