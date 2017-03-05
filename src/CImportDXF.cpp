#include <CImportDXF.h>
#include <CImportDXFColors.h>
#include <CGeometry3D.h>
#include <CStrUtil.h>

CImportDXF::
CImportDXF(CGeomScene3D *scene, const std::string &name) :
 scene_(scene), debug_(false), line_num_(0), buffer_valid_(false)
{
  if (scene_ == NULL) {
    scene_  = CGeometryInst->createScene3D();
    pscene_ = scene_;
  }

  object_ = CGeometryInst->createObject3D(scene_, name);

  scene_->addObject(object_);

  if (scene_ == NULL)
    pobject_ = object_;
}

bool
CImportDXF::
read(CFile &file)
{
  file_ = &file;

  std::string line;

  while (readLine(line)) {
    int code = CStrUtil::toInteger(line);

    if (code != 0) {
      badLine(line);
      break;
    }

    readLine(line);

    if      (line == "SECTION") {
      if (! readSection())
        break;
    }
    else if (line == "EOF")
      break;
    else {
      badLine(line);
      break;
    }
  }

  return true;
}

bool
CImportDXF::
readSection()
{
  std::string line;

  readLine(line);

  int code = CStrUtil::toInteger(line);

  if (code != 2) {
    badLine(line);
    return false;
  }

  readLine(line);

  if      (line == "HEADER"  ) {
    if (! readHeader())
      return false;
  }
  else if (line == "TABLES"  ) {
    if (! readTables())
      return false;
  }
  else if (line == "BLOCKS"  ) {
    if (! readBlocks())
      return false;
  }
  else if (line == "ENTITIES") {
    if (! readEntities())
      return false;
  }
  else {
    badLine(line);
    return false;
  }

  return true;
}

bool
CImportDXF::
readHeader()
{
  std::string line;

  while (readLine(line)) {
    int code = CStrUtil::toInteger(line);

    if (code == 0) {
      readLine(line);

      if (line == "ENDSEC")
        break;
    }
    else
      readLine(line);
  }

  return true;
}

bool
CImportDXF::
readTables()
{
  std::string line;

  while (readLine(line)) {
    int code = CStrUtil::toInteger(line);

    if (code == 0) {
      readLine(line);

      if (line == "ENDSEC")
        break;
    }
    else
      readLine(line);
  }

  return true;
}

bool
CImportDXF::
readBlocks()
{
  std::string line;

  while (readLine(line)) {
    int code = CStrUtil::toInteger(line);

    if (code == 0) {
      readLine(line);

      if (line == "ENDSEC")
        break;
    }
    else
      readLine(line);
  }

  return true;
}

bool
CImportDXF::
readEntities()
{
  std::string line;

  while (readLine(line)) {
    int code = CStrUtil::toInteger(line);

    switch (code) {
      case 0:
        readLine(line);

        if      (line == "3DFACE"  ) {
          if (! read3DFaceEntity())
            return false;
        }
        else if (line == "LINE"    ) {
          if (! readEntity())
            return false;
        }
        else if (line == "POINT"   ) {
          if (! readEntity())
            return false;
        }
        else if (line == "VERTEX"  ) {
          if (! readEntity())
            return false;
        }
        else if (line == "POLYLINE") {
          if (! readPolyLineEntity())
            return false;
        }
        else if (line == "BLOCK"   ) {
          if (! readEntity())
            return false;
        }
        else if (line == "INSERT"  ) {
          if (! readEntity())
            return false;
        }
        else if (line == "SEQEND"  ) {
          if (! readEntity())
            return false;
        }
        else if (line == "ENDSEC"  )
          return true;
        else {
          badLine(line);
          return false;
        }

        break;
      case 8:
        readLine(line);

        break;
      default:
        badLine(line);
        return false;
    }
  }

  return true;
}

bool
CImportDXF::
read3DFaceEntity()
{
  std::string line;
  double      data[4][3];
  bool        visible[4];

  for (int i = 0; i < 4; i++)
    visible[i] = true;

  int color = 0;

  int num_points = 0;

  while (readLine(line)) {
    int code = CStrUtil::toInteger(line);

    if (code == 0) {
      unreadLine(line);
      break;
    }

    switch (code) {
      case 8:
        readLine(line);
        break;
      case 10:
      case 20:
      case 30:
      case 11:
      case 21:
      case 31:
      case 12:
      case 22:
      case 32:
      case 13:
      case 23:
      case 33: {
        readLine(line);

        int i = code/10;
        int j = code - i*10 + 1;

        num_points = std::max(num_points, j);

        data[j - 1][i - 1] = CStrUtil::toReal(line);

        break;
      }
      case 62:
        readLine(line);

        color = CStrUtil::toInteger(line);

        break;
      case 70: {
        readLine(line);

        int flags = CStrUtil::toInteger(line);

        for (int i = 0; i < 4; i++) {
          if (flags & (1<<i))
            visible[i] = false;
        }

        break;
      }
      default:
        badLine(line);
        return false;
    }
  }

  if (num_points == 0)
    return true;

  if (num_points == 4) {
    if (data[3][0] == data[2][0] &&
        data[3][1] == data[2][1] &&
        data[3][2] == data[2][2])
      num_points--;
  }

  std::vector<uint> vertices;

  for (int i = 0; i < num_points; i++) {
    CPoint3D p(data[i][0], data[i][1], data[i][2]);

    uint ind = object_->addVertex(p);

    vertices.push_back(ind);
  }

  int face_num = object_->addFace(vertices);

  for (int i = 0; i < 4; i++) {
    if (! visible[i]) {
      //object_->setFaceEdgeVisible(face_num, i, false);
      std::cout << "Invisible Edge " << i << std::endl;
    }
  }

  if (color > 0 && color < 255) {
    CImportDXFColor *dxf_color = &dxf_colors[color];

    CRGBA rgba(dxf_color->r, dxf_color->g, dxf_color->b, 1.0);

    object_->setFaceColor(face_num, rgba);
  }

  return true;
}

bool
CImportDXF::
readPolyLineEntity()
{
  std::string line;

  int color = 0;

  //double z = 0.0;

  //bool vertices_follow = false;

  //bool closed                    = false;
  //bool curve_fit_vertices_added  = false;
  //bool spline_fit_vertices_added = false;
  bool line_3d                   = false;
  bool mesh_3d                   = false;
  //bool closed_n                  = false;
  //bool polyface_mesh             = false;
  //bool linetype_pattern          = false;

  //int num_m_vertices = 0;
  //int num_n_vertices = 0;

  //int m_density = 0;
  //int n_density = 0;

  while (readLine(line)) {
    int code = CStrUtil::toInteger(line);

    if (code == 0) {
      unreadLine(line);
      break;
    }

    switch (code) {
      case 8:
        readLine(line);
        break;
      case 10:
      case 20:
        readLine(line);
        break;
      case 30:
        readLine(line);

        //z = CStrUtil::toReal(line);

        break;
      case 39:
        readLine(line);
        break;
      case 40:
        readLine(line);
        break;
      case 41:
        readLine(line);
        break;
      case 62:
        readLine(line);

        color = CStrUtil::toInteger(line);

        break;
      case 66:
        readLine(line);

        //vertices_follow = (CStrUtil::toInteger(line) != 0);

        break;
      case 70: {
        readLine(line);

        int flags = CStrUtil::toInteger(line);

        //if (flags & (1<<0)) closed = true;
        //if (flags & (1<<1)) curve_fit_vertices_added = true;
        //if (flags & (1<<2)) spline_fit_vertices_added = true;
        if (flags & (1<<3)) line_3d = true;
        if (flags & (1<<4)) mesh_3d = true;
        //if (flags & (1<<5)) closed_n = true;
        //if (flags & (1<<6)) polyface_mesh = true;
        //if (flags & (1<<7)) linetype_pattern = true;

        break;
      }
      case 71:
        readLine(line);

        //num_m_vertices = CStrUtil::toInteger(line);

        break;
      case 72:
        readLine(line);

        //num_n_vertices = CStrUtil::toInteger(line);

        break;
      case 73:
        readLine(line);

        //m_density = CStrUtil::toInteger(line);

        break;
      case 74:
        readLine(line);

        //n_density = CStrUtil::toInteger(line);

        break;
      case 75:
        readLine(line);
        break;
      case 210:
      case 220:
      case 230:
        readLine(line);
        break;
      default:
        badLine(line);
        return false;
    }
  }

  readLine(line);

  int code = CStrUtil::toInteger(line);

  if (code != 0) {
    badLine(line);
    return false;
  }

  readLine(line);

  while (line != "SEQEND") {
    if (line == "VERTEX") {
      if (! readVertexEntity(color))
        return false;
    }
    else {
      badLine(line);
      return false;
    }

    readLine(line);

    int code = CStrUtil::toInteger(line);

    if (code != 0) {
      badLine(line);
      return false;
    }

    readLine(line);
  }

  if (line_3d) {
    std::cout << "3D Line" << std::endl;
  }

  if (mesh_3d) {
    std::cout << "3D Mesh" << std::endl;
  }

  return true;
}

bool
CImportDXF::
readVertexEntity(int color)
{
  std::string line;

  double x = 0.0;
  double y = 0.0;
  double z = 0.0;

  bool vertex = false;
  bool face   = false;

  int face_num[4] = { 0, 0, 0, 0 };

  while (readLine(line)) {
    int code = CStrUtil::toInteger(line);

    if (code == 0) {
      unreadLine(line);
      break;
    }

    switch (code) {
      case 8:
        readLine(line);
        break;
      case 10:
        readLine(line);

        x = CStrUtil::toReal(line);

        break;
      case 20:
        readLine(line);

        y = CStrUtil::toReal(line);

        break;
      case 30:
        readLine(line);

        z = CStrUtil::toReal(line);

        break;
      case 40:
      case 41:
      case 50:
      case 62:
        readLine(line);

        color = CStrUtil::toInteger(line);

        break;
      case 70: {
        readLine(line);

        int flags = CStrUtil::toInteger(line);

        if (flags & (1<<6))
          vertex = true;
        if (flags & (1<<7))
          face   = true;

        break;
      }
      case 71:
      case 72:
      case 73:
      case 74:
        readLine(line);

        face_num[code - 71] = CStrUtil::toInteger(line);

        break;
      default:
        badLine(line);
        return false;
    }
  }

  if      (vertex) {
    CPoint3D p(x, y, z);

    object_->addVertex(p);
  }
  else if (face) {
    if (face_num[0] != face_num[1] &&
        face_num[1] != face_num[2] &&
        face_num[2] != face_num[0]) {
      if (face_num[2] != face_num[3]) {
        std::vector<uint> vertices;

        vertices.push_back(face_num[0] - 1);
        vertices.push_back(face_num[1] - 1);
        vertices.push_back(face_num[2] - 1);

        uint face_num = object_->addFace(vertices);

        if (color > 0 && color < 255) {
          CImportDXFColor *dxf_color = &dxf_colors[color];

          CRGBA rgba(dxf_color->r, dxf_color->g, dxf_color->b, 1.0);

          object_->setFaceColor(face_num, rgba);
        }
      }
      else {
        std::vector<uint> vertices;

        vertices.push_back(face_num[0] - 1);
        vertices.push_back(face_num[1] - 1);
        vertices.push_back(face_num[2] - 1);
        vertices.push_back(face_num[3] - 1);

        uint face_num = object_->addFace(vertices);

        if (color > 0 && color < 255) {
          CImportDXFColor *dxf_color = &dxf_colors[color];

          CRGBA rgba(dxf_color->r, dxf_color->g, dxf_color->b, 1.0);

          object_->setFaceColor(face_num, rgba);
        }
      }
    }
  }

  return true;
}

bool
CImportDXF::
readEntity()
{
  std::string line;

  while (readLine(line)) {
    int code = CStrUtil::toInteger(line);

    if (code == 0) {
      unreadLine(line);
      break;
    }

    readLine(line);
  }

  return true;
}

bool
CImportDXF::
readLine(std::string &line)
{
  if (buffer_valid_) {
    line = buffer_;

    buffer_valid_ = false;

    return true;
  }

  line_num_++;

  std::string line1;

  bool flag = file_->readLine(line1);

  line = CStrUtil::stripSpaces(line1);

  return flag;
}

void
CImportDXF::
unreadLine(std::string line)
{
  buffer_ = line;

  buffer_valid_ = true;
}

void
CImportDXF::
badLine(std::string line)
{
  std::cerr << "Invalid Line " << line_num_ << " :" << line << std::endl;
}
