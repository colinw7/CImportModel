#include <CImport3DS.h>
#include <CGeometry3D.h>
#include <CMathGeom3D.h>

#define M3D_VERSION_ID      0x0002
#define COLOR_24_ID         0x0011
#define LIN_COLOR_24_ID     0x0012
#define INT_PERCENTAGE_ID   0x0030
#define FLOAT_PERCENTAGE_ID 0x0031
#define MASTER_SCALE_ID     0x0100
#define MDATA_ID            0x3d3d
#define MESH_VERSION_ID     0x3d3e
#define NAMED_OBJECT_ID     0x4000
#define N_TRI_OBJECT_ID     0x4100
#define POINT_ARRAY_ID      0x4110
#define FACE_ARRAY_ID       0x4120
#define MSH_MAT_GROUP_ID    0x4130
#define MAP_COORDS_ID       0x4140
#define SMOOTH_GROUP_ID     0x4150
#define MESH_MATRIX_ID      0x4160
#define M3DMAGIC_ID         0x4d4d
#define MAT_NAME_ID         0xa000
#define MAT_AMBIENT_ID      0xa010
#define MAT_DIFFUSE_ID      0xa020
#define MAT_SPECULAR_ID     0xa030
#define MAT_SHININESS_ID    0xa040
#define MAT_TRANSPARENCY_ID 0xa050
#define MAT_TWO_SIDED_ID    0xa081
#define MAT_SHADING_ID      0xa100
#define MAT_ENTRY_ID        0xafff

struct ChunkName {
  ushort      id;
  const char *name;
};

static ChunkName
chunk_names[] = {
  { M3D_VERSION_ID     , "M3D_VERSION"     , },
  { COLOR_24_ID        , "COLOR_24"        , },
  { LIN_COLOR_24_ID    , "LIN_COLOR_24"    , },
  { INT_PERCENTAGE_ID  , "INT_PERCENTAGE"  , },
  { FLOAT_PERCENTAGE_ID, "FLOAT_PERCENTAGE", },
  { MASTER_SCALE_ID    , "MASTER_SCALE"    , },
  { MDATA_ID           , "MDATA"           , },
  { MESH_VERSION_ID    , "MESH_VERSION"    , },
  { NAMED_OBJECT_ID    , "NAMED_OBJECT"    , },
  { N_TRI_OBJECT_ID    , "N_TRI_OBJECT"    , },
  { POINT_ARRAY_ID     , "POINT_ARRAY"     , },
  { FACE_ARRAY_ID      , "FACE_ARRAY"      , },
  { MSH_MAT_GROUP_ID   , "MSH_MAT_GROUP"   , },
  { MAP_COORDS_ID      , "MAP_COORDS_ID"   , },
  { SMOOTH_GROUP_ID    , "SMOOTH_GROUP_ID" , },
  { MESH_MATRIX_ID     , "MESH_MATRIX"     , },
  { M3DMAGIC_ID        , "M3DMAGIC"        , },
  { MAT_NAME_ID        , "MAT_NAME"        , },
  { MAT_AMBIENT_ID     , "MAT_AMBIENT"     , },
  { MAT_DIFFUSE_ID     , "MAT_DIFFUSE"     , },
  { MAT_SPECULAR_ID    , "MAT_SPECULAR"    , },
  { MAT_SHININESS_ID   , "MAT_SHININESS"   , },
  { MAT_TRANSPARENCY_ID, "MAT_TRANSPARENCY", },
  { MAT_TWO_SIDED_ID   , "MAT_TWO_SIDED_ID", },
  { MAT_SHADING_ID     , "MAT_SHADING"     , },
  { MAT_ENTRY_ID       , "MAT_ENTRY"       , },
  { 0                  , NULL              , },
};

CImport3DS::
CImport3DS(CGeomScene3D *scene, const std::string &) :
 scene_(scene), debug_(false)
{
  if (scene_ == NULL) {
    scene_  = CGeometryInst->createScene3D();
    pscene_ = scene_;
  }
}

bool
CImport3DS::
read(CFile &file)
{
  file_ = &file;

  CImport3DSChunk chunk(NULL);

  readChunk(&chunk);

  if (chunk.id != M3DMAGIC_ID) {
    std::cerr << "Not a 3D Studio File" << std::endl;
    return false;
  }

  CImport3DSChunk chunk1(&chunk);

  while (readChunk(&chunk1)) {
    switch (chunk1.id) {
      case M3D_VERSION_ID:
        readM3DVersion(&chunk1);
        break;
      case MDATA_ID:
        readMData(&chunk1);
        break;
      default:
        skipChunk(&chunk1);
        break;
    }
  }

  return true;
}

bool
CImport3DS::
readM3DVersion(CImport3DSChunk *chunk)
{
  uint version;

  if (! readLong(chunk, &version))
    return false;

  return true;
}

bool
CImport3DS::
readMData(CImport3DSChunk *chunk)
{
  CImport3DSChunk chunk1(chunk);

  while (readChunk(&chunk1)) {
    switch (chunk1.id) {
      case MESH_VERSION_ID:
        readMeshVersion(&chunk1);
        break;
      case MAT_ENTRY_ID:
        readMatEntry(&chunk1);
        break;
      case MASTER_SCALE_ID:
        readMasterScale(&chunk1);
        break;
      case NAMED_OBJECT_ID:
        readNamedObject(&chunk1);
        break;
      default:
        skipChunk(&chunk1);
        break;
    }
  }

  return true;
}

bool
CImport3DS::
readMeshVersion(CImport3DSChunk *chunk)
{
  uint version;

  if (! readLong(chunk, &version))
    return false;

  return true;
}

bool
CImport3DS::
readMatEntry(CImport3DSChunk *chunk)
{
  CImport3DSChunk chunk1(chunk);

  material_ = new CImport3DSMaterial;

  materials_.push_back(material_);

  while (readChunk(&chunk1)) {
    switch (chunk1.id) {
      case MAT_NAME_ID:
        readMatName(&chunk1);
        break;
      case MAT_AMBIENT_ID:
        readMatAmbient(&chunk1);
        break;
      case MAT_DIFFUSE_ID:
        readMatDiffuse(&chunk1);
        break;
      case MAT_SPECULAR_ID:
        readMatSpecular(&chunk1);
        break;
      case MAT_SHININESS_ID:
        readMatShininess(&chunk1);
        break;
      case MAT_TRANSPARENCY_ID:
        readMatTransparency(&chunk1);
        break;
      case MAT_TWO_SIDED_ID:
        readMatTwoSided(&chunk1);
        break;
      case MAT_SHADING_ID:
        readMatShading(&chunk1);
        break;
      default:
        skipChunk(&chunk1);
        break;
    }
  }

  if (debug_) {
    std::string pad = getChunkPad(chunk);

    CMaterial *material = &material_->material;

    std::cout << pad  << "Material '" << material_->name << "'" << std::endl;

    std::cout << pad  << "  Ambient  " << material->getAmbient()  << std::endl;
    std::cout << pad  << "  Diffuse  " << material->getDiffuse()  << std::endl;
    std::cout << pad  << "  Specular " << material->getSpecular() << std::endl;

    std::cout << pad  << "  Shininess    " << material->getShininess() << std::endl;
    std::cout << pad  << "  Transparency " << material_->transparency << std::endl;
    std::cout << pad  << "  Shading      " << material_->shading      << std::endl;
  }

  return true;
}

bool
CImport3DS::
readMatName(CImport3DSChunk *chunk)
{
  std::string name = readString(chunk);

  if (debug_) {
    std::string pad = getChunkPad(chunk);

    std::cout << pad << "  Material '" << name << "'" << std::endl;
  }

  material_->name = name;

  return true;
}

bool
CImport3DS::
readMatAmbient(CImport3DSChunk *chunk)
{
  CImport3DSChunk chunk1(chunk);

  CRGBA rgba;

  while (readChunk(&chunk1)) {
    switch (chunk1.id) {
      case COLOR_24_ID: {
        readColor(chunk, rgba);

        material_->material.setAmbient(rgba);

        break;
      }
      case LIN_COLOR_24_ID: {
        readColor(chunk, rgba);

        material_->material.setAmbient(rgba);

        break;
      }
      default:
        skipChunk(&chunk1);
        break;
    }
  }

  return true;
}

bool
CImport3DS::
readMatDiffuse(CImport3DSChunk *chunk)
{
  CImport3DSChunk chunk1(chunk);

  CRGBA rgba;

  while (readChunk(&chunk1)) {
    switch (chunk1.id) {
      case COLOR_24_ID: {
        readColor(chunk, rgba);

        material_->material.setDiffuse(rgba);

        break;
      }
      case LIN_COLOR_24_ID: {
        readColor(chunk, rgba);

        material_->material.setDiffuse(rgba);

        break;
      }
      default:
        skipChunk(&chunk1);
        break;
    }
  }

  return true;
}

bool
CImport3DS::
readMatSpecular(CImport3DSChunk *chunk)
{
  CImport3DSChunk chunk1(chunk);

  CRGBA rgba;

  while (readChunk(&chunk1)) {
    switch (chunk1.id) {
      case COLOR_24_ID: {
        readColor(chunk, rgba);

        material_->material.setSpecular(rgba);

        break;
      }
      case LIN_COLOR_24_ID: {
        readColor(chunk, rgba);

        material_->material.setSpecular(rgba);

        break;
      }
      default:
        skipChunk(&chunk1);
        break;
    }
  }

  return true;
}

bool
CImport3DS::
readMatShininess(CImport3DSChunk *chunk)
{
  CImport3DSChunk chunk1(chunk);

  ushort shininess;
  float  f_shininess;

  while (readChunk(&chunk1)) {
    switch (chunk1.id) {
      case INT_PERCENTAGE_ID:
        readIntPercentage(chunk, &shininess);
        f_shininess = shininess;
        break;
      case FLOAT_PERCENTAGE_ID:
        readFloatPercentage(chunk, &f_shininess);
        break;
      default:
        skipChunk(&chunk1);
        break;
    }
  }

  material_->material.setShininess(f_shininess);

  return true;
}

bool
CImport3DS::
readMatTransparency(CImport3DSChunk *chunk)
{
  CImport3DSChunk chunk1(chunk);

  ushort transparency;
  float  f_transparency;

  while (readChunk(&chunk1)) {
    switch (chunk1.id) {
      case INT_PERCENTAGE_ID:
        readIntPercentage(chunk, &transparency);
        f_transparency = transparency;
        break;
      case FLOAT_PERCENTAGE_ID:
        readFloatPercentage(chunk, &f_transparency);
        break;
      default:
        skipChunk(&chunk1);
        break;
    }
  }

  material_->transparency = f_transparency;

  return true;
}

bool
CImport3DS::
readMatTwoSided(CImport3DSChunk *)
{
  material_->two_sided = true;

  return true;
}

bool
CImport3DS::
readMatShading(CImport3DSChunk *chunk)
{
  ushort shading;

  readShort(chunk, &shading);

  material_->shading = shading;

  return true;
}

bool
CImport3DS::
readColor(CImport3DSChunk *chunk, CRGBA &rgba)
{
  uchar r, g, b;

  readChar(chunk, &r);
  readChar(chunk, &g);
  readChar(chunk, &b);

  rgba = CRGBA(r/255.0, g/255.0, b/255.0, 1.0);

  return true;
}

bool
CImport3DS::
readIntPercentage(CImport3DSChunk *chunk, ushort *percent)
{
  readShort(chunk, percent);

  return true;
}

bool
CImport3DS::
readFloatPercentage(CImport3DSChunk *chunk, float *percent)
{
  readFloat(chunk, percent);

  return true;
}

bool
CImport3DS::
readMasterScale(CImport3DSChunk *chunk)
{
  float scale;

  readFloat(chunk, &scale);

  return true;
}

bool
CImport3DS::
readNamedObject(CImport3DSChunk *chunk)
{
  CImport3DSChunk chunk1(chunk);

  std::string name = readString(chunk);

  if (debug_) {
    std::string pad = getChunkPad(chunk);

    std::cout << pad << "Object " << name << std::endl;
  }

  object_ = CGeometryInst->createObject3D(scene_, name);

  while (readChunk(&chunk1)) {
    switch (chunk1.id) {
      case N_TRI_OBJECT_ID:
        readNTriObject(&chunk1);
        break;
      default:
        skipChunk(&chunk1);
        break;
    }
  }

  scene_->addObject(object_);

  return true;
}

bool
CImport3DS::
readNTriObject(CImport3DSChunk *chunk)
{
  CImport3DSChunk chunk1(chunk);

  while (readChunk(&chunk1)) {
    switch (chunk1.id) {
      case POINT_ARRAY_ID:
        readPointArray(&chunk1);
        break;
      case FACE_ARRAY_ID:
        readFaceArray(&chunk1);
        break;
      case MSH_MAT_GROUP_ID:
        readMshMatGroup(&chunk1);
        break;
      case SMOOTH_GROUP_ID:
        readSmoothGroup(&chunk1);
        break;
      case MESH_MATRIX_ID:
        readMeshMatrix(&chunk1);
        break;
      default:
        skipChunk(&chunk1);
        break;
    }
  }

  return true;
}

bool
CImport3DS::
readPointArray(CImport3DSChunk *chunk)
{
  ushort num_points;

  readShort(chunk, &num_points);

  for (int i = 0; i < num_points; ++i) {
    float x, y, z;

    readFloat(chunk, &x);
    readFloat(chunk, &y);
    readFloat(chunk, &z);

    if (debug_) {
      std::string pad = getChunkPad(chunk);

      std::cout << pad << "  Point " << i << ": " << x << " " << y << " " << z << std::endl;
    }

    uint vertex_num = object_->addVertex(CPoint3D(x, y, z));

    CGeomVertex3D &vertex = object_->getVertex(vertex_num);

    vertex.setViewed(vertex.getCurrent());
  }

  return true;
}

bool
CImport3DS::
readFaceArray(CImport3DSChunk *chunk)
{
  vertexFaceList_.clear();

  uint num_vertices = object_->getNumVertices();

  ushort num_faces;

  readShort(chunk, &num_faces);

  if (debug_) {
    std::string pad = getChunkPad(chunk);

    std::cout << pad  << "Num faces " << num_faces << std::endl;
  }

  ushort point_num[3];

  for (int i = 0; i < num_faces; ++i) {
    std::vector<uint> vertices;

    for (int j = 0; j < 3; j++) {
      readShort(chunk, &point_num[j]);

      if (point_num[j] < num_vertices)
        vertices.push_back(point_num[j]);
      else
        std::cerr << "Invalid Point Num " << point_num[j] << std::endl;
    }

    ushort flags; // point flags

    readShort(chunk, &flags);

    if (debug_) {
      std::string pad = getChunkPad(chunk);

      std::cout << pad << "  Face " << i << ":";

      for (int j = 0; j < 3; j++)
        std::cout << " " << point_num[j];

      std::cout << " (" << flags << ")";

      std::cout << std::endl;
    }

    CGeomVertex3D &vertex1 = object_->getVertex(vertices[0]);
    CGeomVertex3D &vertex2 = object_->getVertex(vertices[1]);
    CGeomVertex3D &vertex3 = object_->getVertex(vertices[2]);

    CPolygonOrientation orient =
      CMathGeom3D::PolygonOrientation(vertex1.getCurrent(),
                                      vertex2.getCurrent(),
                                      vertex3.getCurrent(),
                                      CPoint3D(0,0,1));

    if (orient == CPOLYGON_ORIENTATION_ANTICLOCKWISE)
      std::swap(vertices[0], vertices[2]);

    uint face_num = object_->addFace(vertices);

    CGeomFace3D &face = object_->getFace(face_num);

    CVector3D normal;

    face.calcNormal(normal);

    face.setNormal(normal);

    for (int j = 0; j < 3; j++)
      vertexFaceList_[point_num[j]].push_back(face_num);
  }

  VertexFaceList::const_iterator p1, p2;

  for (p1 = vertexFaceList_.begin(), p2 = vertexFaceList_.end(); p1 != p2; ++p1) {
    CGeomVertex3D &vertex = object_->getVertex((*p1).first);

    FaceList::const_iterator pf1, pf2;

    CVector3D normal(0,0,0);

    for (pf1 = (*p1).second.begin(), pf2 = (*p1).second.end(); pf1 != pf2; ++pf1) {
      CGeomFace3D &face = object_->getFace(*pf1);

      normal += face.getNormal();
    }

    normal /= (*p1).second.size();

    normal.normalize();

    vertex.setNormal(normal);
  }

  return true;
}

bool
CImport3DS::
readMshMatGroup(CImport3DSChunk *chunk)
{
  ushort face_num;
  ushort num_faces;

  std::string name = readString(chunk);

  if (debug_) {
    std::string pad = getChunkPad(chunk);

    std::cout << pad << "Mat Group " << name << std::endl;
  }

  readShort(chunk, &num_faces);

  std::vector<ushort> face_nums;

  for (ushort i = 0; i < num_faces; ++i) {
    readShort(chunk, &face_num);

    face_nums.push_back(face_num);
  }

  MaterialList::const_iterator pmaterial;

  for (pmaterial = materials_.begin();
         pmaterial != materials_.end(); ++pmaterial)
    if ((*pmaterial)->name == name)
      break;

  if (pmaterial != materials_.end()) {
    if (debug_) {
      std::string pad = getChunkPad(chunk);

      std::cout << pad << "  Ambient  " << (*pmaterial)->material.getAmbient()  << std::endl;
      std::cout << pad << "  Diffuse  " << (*pmaterial)->material.getDiffuse()  << std::endl;
      std::cout << pad << "  Specular " << (*pmaterial)->material.getSpecular() << std::endl;

      std::cout << pad << "  Shininess    " << (*pmaterial)->material.getShininess() << std::endl;
      std::cout << pad << "  Transparency " << (*pmaterial)->transparency << std::endl;
      std::cout << pad << "  Shading      " << (*pmaterial)->shading      << std::endl;
    }

    for (ushort i = 0; i < num_faces; ++i) {
      face_num = face_nums[i];

      if (face_num < object_->getNumFaces()) {
        CGeomFace3D &face = object_->getFace(face_num);

        face.setMaterial((*pmaterial)->material);

        face.setTwoSided((*pmaterial)->two_sided);
      }
      else
        std::cerr << "Invalid face num " << face_num << std::endl;
    }
  }

  return true;
}

bool
CImport3DS::
readSmoothGroup(CImport3DSChunk *chunk)
{
  smoothGroupFaceList_.clear();

  ushort num_faces = chunk->left/sizeof(uint);

  if (debug_)
    std::cout << num_faces << std::endl;

  for (ushort i = 0; i < num_faces; ++i) {
    uint smooth;

    if (! readLong(chunk, &smooth))
      return false;

    if (smooth == 0) continue;

    if (debug_)
      std::cout << "Smooth Groups";

    for (uint b = 0; b < 32; ++b) {
      if (smooth & (1<<b)) {
        smoothGroupFaceList_[b].push_back(i);

        if (debug_)
          std::cout << ":" << b;
      }
    }

    if (debug_)
      std::cout << std::endl;
  }

  return true;
}

bool
CImport3DS::
readMeshMatrix(CImport3DSChunk *chunk)
{
  float matrix[3][4];

  readFloat(chunk, &matrix[0][0]);
  readFloat(chunk, &matrix[1][0]);
  readFloat(chunk, &matrix[2][0]);
  readFloat(chunk, &matrix[0][1]);
  readFloat(chunk, &matrix[1][1]);
  readFloat(chunk, &matrix[2][1]);
  readFloat(chunk, &matrix[0][2]);
  readFloat(chunk, &matrix[1][2]);
  readFloat(chunk, &matrix[2][2]);
  readFloat(chunk, &matrix[0][3]);
  readFloat(chunk, &matrix[1][3]);
  readFloat(chunk, &matrix[2][3]);

  if (debug_) {
    std::string pad = getChunkPad(chunk);

    std::cout << pad << matrix[0][0] << " " << matrix[1][0] << " " << matrix[2][0] << std::endl;
    std::cout << pad << matrix[0][1] << " " << matrix[1][1] << " " << matrix[2][1] << std::endl;
    std::cout << pad << matrix[0][2] << " " << matrix[1][2] << " " << matrix[2][2] << std::endl;
    std::cout << pad << matrix[0][3] << " " << matrix[1][3] << " " << matrix[2][3] << std::endl;
  }

  return true;
}

bool
CImport3DS::
readChunk(CImport3DSChunk *chunk)
{
  if (chunk->parent != NULL && chunk->parent->left <= 0)
    return false;

  chunk->left = 6;

  if (! readShort(chunk, &chunk->id))
    return false;

  if (! readLong(chunk, &chunk->len))
    return false;

  chunk->left = chunk->len - 6;

  if (debug_)
    printChunk(chunk);

  return true;
}

bool
CImport3DS::
skipChunk(CImport3DSChunk *chunk)
{
  if (debug_) {
    std::string pad = getChunkPad(chunk);

    std::cerr << pad << "  !!Skip Chunk!! " << getChunkName(chunk) << std::endl;
  }

  int c;

  while (chunk->left > 0) {
    c = file_->getC();

    if (c == EOF)
      break;

    adjustChunkLeft(chunk, sizeof(uchar));
  }

  return true;
}

bool
CImport3DS::
readChar(CImport3DSChunk *chunk, uchar *c)
{
  int c1 = file_->getC();

  if (c1 == EOF)
    return false;

  *c = c1;

  adjustChunkLeft(chunk, sizeof(uchar));

  return true;
}

bool
CImport3DS::
readShort(CImport3DSChunk *chunk, ushort *s)
{
  CFileData file_data(sizeof(ushort));

  try {
    file_->read(&file_data);
  }
  catch (...) {
    return false;
  }

  *s = (file_data.getDataPos(1) << 8) | file_data.getDataPos(0);

  adjustChunkLeft(chunk, sizeof(ushort));

  return true;
}

bool
CImport3DS::
readLong(CImport3DSChunk *chunk, uint *l)
{
  CFileData file_data(4);

  try {
    file_->read(&file_data);
  }
  catch (...) {
    return false;
  }

  *l = (file_data.getDataPos(3) << 24) | (file_data.getDataPos(2) << 16) |
       (file_data.getDataPos(1) <<  8) |  file_data.getDataPos(0);

  adjustChunkLeft(chunk, 4);

  return true;
}

std::string
CImport3DS::
readString(CImport3DSChunk *chunk)
{
  static char  *buffer = NULL;
  static int    buffer_max = 0;

  if (chunk->left > buffer_max) {
    buffer_max = chunk->left + 32;

    delete [] buffer;

    buffer = new char [buffer_max + 1];
  }

  int i = 0;

  while (chunk->left > 0) {
    int c = file_->getC();

    if (c == EOF)
      break;

    adjustChunkLeft(chunk, sizeof(uchar));

    if (c == '\0')
      break;

    buffer[i++] = c;
  }

  buffer[i] = '\0';

  std::string str = std::string(buffer);

  return str;
}

bool
CImport3DS::
readFloat(CImport3DSChunk *chunk, float *f)
{
  CFileData file_data(sizeof(float));

  try {
    file_->read(&file_data);
  }
  catch (...) {
    return false;
  }

  memcpy(f, file_data.getData(), sizeof(float));

  adjustChunkLeft(chunk, sizeof(float));

  return true;
}

std::string
CImport3DS::
getChunkName(CImport3DSChunk *chunk)
{
  static char name[256];

  if (chunk == NULL)
    return("<NULL>");

  for (int i = 0; chunk_names[i].name != NULL; ++i) {
    if (chunk_names[i].id != chunk->id) continue;

    sprintf(name, "%s (%x)", chunk_names[i].name, chunk->id);

    return std::string(name);
  }

  sprintf(name, "%x", chunk->id);

  return std::string(name);
}

void
CImport3DS::
adjustChunkLeft(CImport3DSChunk *chunk, int offset)
{
  chunk->left -= offset;

  if (chunk->parent != NULL)
    adjustChunkLeft(chunk->parent, offset);
}

void
CImport3DS::
printChunk(CImport3DSChunk *chunk)
{
  std::string pad = getChunkPad(chunk);

  std::cout << pad << "Chunk " << getChunkName(chunk) << " " << chunk->left << std::endl;
}

uint
CImport3DS::
getChunkDepth(CImport3DSChunk *chunk)
{
  uint depth = 0;

  CImport3DSChunk *parent = chunk->parent;

  while (parent != NULL) {
    ++depth;

    parent = parent->parent;
  }

  return depth;
}

std::string
CImport3DS::
getChunkPad(CImport3DSChunk *chunk)
{
  uint depth = getChunkDepth(chunk);

  std::string pad;

  for (uint i = 0; i < depth; ++i)
    pad += "  ";

  return pad;
}
