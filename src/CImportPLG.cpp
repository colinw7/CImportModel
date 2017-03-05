#include <CImportPLG.h>
#include <CStrUtil.h>
#include <CStrParse.h>

#define PLX_COLOR_MODE_RGB_FLAG     0x8000
#define PLX_COLOR_MODE_INDEXED_FLAG 0x0000
#define PLX_2SIDED_FLAG             0x1000
#define PLX_1SIDED_FLAG             0x0000
#define PLX_SHADE_MODE_PURE_FLAG    0x0000
#define PLX_SHADE_MODE_FLAT_FLAG    0x2000
#define PLX_SHADE_MODE_GOURAUD_FLAG 0x4000
#define PLX_SHADE_MODE_PHONG_FLAG   0x6000
#define PLX_SHADE_MODE_SHADE_MASK   0x6000

CImportPLG::
CImportPLG() :
 debug_(false), palette_loaded_(false), palette_(NULL)
{
}

CImportPLG::
~CImportPLG()
{
  delete [] palette_;
}

bool
CImportPLG::
read(CFile &file)
{
  file_ = &file;

  if (! readHeader())
    return false;

  if (! readVertices())
    return false;

  if (! readTriangles())
    return false;

  return true;
}

bool
CImportPLG::
readHeader()
{
  if (! readNextLine())
    return false;

  std::vector<std::string> words;

  CStrUtil::addWords(line_, words);

  if (words.size() != 3)
    return false;

  name_ = words[0];

  if (! CStrUtil::toInteger(words[1], &num_vertices_))
    return false;

  if (! CStrUtil::toInteger(words[2], &num_polygons_))
    return false;

  return true;
}

bool
CImportPLG::
readVertices()
{
  double x, y, z;

  for (uint i = 0; i < num_vertices_; ++i) {
    if (! readNextLine())
      return false;

    std::vector<std::string> words;

    CStrUtil::addWords(line_, words);

    if (words.size() != 3)
      return false;

    if (! CStrUtil::toReal(words[0], &x))
      return false;

    if (! CStrUtil::toReal(words[1], &y))
      return false;

    if (! CStrUtil::toReal(words[2], &z))
      return false;

    addVertex(x, y, z);
  }

  return true;
}

bool
CImportPLG::
readTriangles()
{
  int  r, g, b;
  uint attr, shade;
  uint ind, inds[32], num_inds;

  uint max_inds = (int) (sizeof(inds)/sizeof(int));

  for (uint i = 0; i < num_polygons_; ++i) {
    if (! readNextLine())
      return false;

    std::vector<std::string> words;

    CStrUtil::addWords(line_, words);

    if (words.size() <= 1)
      return false;

    std::string lhs = words[0].substr(0, 2);
    std::string rhs = words[0].substr(2);

    if (lhs != "0x" && lhs != "0X")
      return false;

    if (! CStrUtil::toBaseInteger(rhs, 16, &attr))
      return false;

    if (! CStrUtil::toInteger(words[1], &num_inds))
      return false;

    if (num_inds != (words.size() - 2))
      return false;

    if (num_inds > max_inds)
      return false;

    for (uint j = 0; j < num_inds; ++j)
      if (! CStrUtil::toInteger(words[j + 2], &inds[j]))
        return false;

    CRGBA color;

    //setTwoSided(attr & PLX_2SIDED_FLAG);

    if (attr & PLX_COLOR_MODE_RGB_FLAG) {
      r = ((attr & 0x0f00) >> 8) << 4;
      g = ((attr & 0x00f0) >> 4) << 4;
      b = ((attr & 0x000f) >> 0) << 4;

      color = CRGBA(r/255.0, g/255.0, b/255.0);
    }
    else {
      ind = attr & 0x00ff;

      color = paletteLookup(ind);
    }

    CDrawType3D  draw_type;
    CShadeType3D shade_type;

    shade = (attr & PLX_SHADE_MODE_SHADE_MASK);

    if      (shade == PLX_SHADE_MODE_PHONG_FLAG)
      shade_type = CSHADE_TYPE_3D_PHONG;
    else if (shade == PLX_SHADE_MODE_GOURAUD_FLAG)
      shade_type = CSHADE_TYPE_3D_GOURAUD;
    else if (shade == PLX_SHADE_MODE_FLAT_FLAG)
      shade_type = CSHADE_TYPE_3D_FLAT;
    else
      shade_type = CSHADE_TYPE_3D_CONSTANT;

    if (shade_type != CSHADE_TYPE_3D_CONSTANT)
      draw_type = CDRAW_TYPE_3D_SOLID;
    else
      draw_type = CDRAW_TYPE_3D_WIREFRAME;

    if (num_inds == 3)
      addITriangle(inds[0], inds[1], inds[2],
                   draw_type, shade_type, color);
    else
      addITriangles(inds, num_inds,
                    draw_type, shade_type, color);
  }

  return true;
}

bool
CImportPLG::
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

CRGBA
CImportPLG::
paletteLookup(int ind)
{
  if (! palette_loaded_) {
    paletteLoad();

    palette_loaded_ = true;
  }

  return palette_[ind];
}

bool
CImportPLG::
paletteLoad()
{
  CFile file("paldata2.pal");

  palette_ = new CRGBA [256];

  std::string line;
  int         r, g, b;

  int ind = 0;

  while (file.readLine(line)) {
    std::vector<std::string> words;

    CStrUtil::addWords(line, words);

    if (words.size() != 4)
      continue;

    if (! CStrUtil::toInteger(words[0], &r)) continue;
    if (! CStrUtil::toInteger(words[1], &g)) continue;
    if (! CStrUtil::toInteger(words[2], &b)) continue;

    palette_[ind++] = CRGBA(r/255.0, g/255.0, b/255.0);
  }

  if (ind != 256)
    return false;

  return true;
}
