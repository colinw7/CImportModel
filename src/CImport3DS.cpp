#include <CImport3DS.h>
#include <CGeometry3D.h>
#include <CMathGeom3D.h>

#include <deque>

#define M3D_VERSION_ID       0x0002
#define FLT_COLOR_24_ID      0x0010
#define INT_COLOR_24_ID      0x0011
#define LIN_INT_COLOR_24_ID  0x0012 // gamma color
#define LIN_FLT_COLOR_24_ID  0x0013 // gamma color
#define INT_PERCENTAGE_ID    0x0030
#define FLOAT_PERCENTAGE_ID  0x0031
#define DOUBLE_PERCENTAGE_ID 0x0032
#define MASTER_SCALE_ID      0x0100
#define MDATA_ID             0x3d3d
#define MESH_VERSION_ID      0x3d3e
#define NAMED_OBJECT_ID      0x4000
#define N_TRI_OBJECT_ID      0x4100
#define POINT_ARRAY_ID       0x4110
#define FACE_ARRAY_ID        0x4120
#define MSH_MAT_GROUP_ID     0x4130
#define MAP_COORDS_ID        0x4140
#define SMOOTH_GROUP_ID      0x4150
#define MESH_MATRIX_ID       0x4160
#define MESH_COLOR_ID        0x4165
#define TEXT_INFO_ID         0x4170
#define LIGHT_ID             0x4600
#define SPOTLIGHT_ID         0x4610
#define CAMERA_ID            0x4700
#define CAM_UNKNWN01_ID      0x4710
#define CAM_UNKNWN02_ID      0x4720
#define M3DMAGIC_ID          0x4d4d
#define HIERARCHY_ID         0x4f00
#define MAT_NAME_ID          0xa000
#define MAT_AMBIENT_ID       0xa010
#define MAT_DIFFUSE_ID       0xa020
#define MAT_SPECULAR_ID      0xa030
#define MAT_SHININESS_ID     0xa040
#define MAT_SHININESSP_ID    0xa041 // percent
#define MAT_TRANSPARENCY_ID  0xa050
#define MAT_SELF_ILLUM_ID    0xa080
#define MAT_TWO_SIDED_ID     0xa081
#define MAT_SELF_ILPCT_ID    0xa084
#define MAT_SELF_ILLUMP_ID   0xa081 // percent
#define MAT_SHADING_ID       0xa100
#define MAT_SHADING_TYPE_ID  0xa1e0
#define MAT_TEXTURE_ID       0xa200
#define MAT_GLOSSY_ID        0xa204
#define MAP_OPACITY_ID       0xa210
#define REFLECT_MAP_ID       0xa220
#define BUMP_MAP_ID          0xa230
#define MAP_FILENAME_ID      0xa300
#define MAP_PARAM_ID         0xa351
#define MAT_ENTRY_ID         0xafff
#define KEY_FRAME_ID         0xb000
#define MESH_INFO_ID         0xb002
#define SPOTLIGHT_INFO_ID    0xb007
#define FRAMES_BLOCK_ID      0xb008
#define OBJECT_NAME_ID       0xb010
#define OBJECT_PIVOT_ID      0xb013
#define POS_TRACK_ID         0xb020
#define ROT_TRACK_ID         0xb021
#define SCALE_TRACK_ID       0xb022
#define HIER_POS_ID          0xb030

struct ChunkName {
  ushort      id;
  const char *name;
};

static ChunkName
chunk_names[] = {
  { M3D_VERSION_ID     , "M3D_VERSION"     , },
  { INT_COLOR_24_ID    , "INT_COLOR_24"    , },
  { FLT_COLOR_24_ID    , "FLT_COLOR_24"    , },
  { LIN_INT_COLOR_24_ID, "LIN_INT_COLOR_24", },
  { LIN_FLT_COLOR_24_ID, "LIN_FLT_COLOR_24", },
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
  { MAP_COORDS_ID      , "MAP_COORDS"      , },
  { SMOOTH_GROUP_ID    , "SMOOTH_GROUP"    , },
  { MESH_MATRIX_ID     , "MESH_MATRIX"     , },
  { MESH_COLOR_ID      , "MESH_COLOR"      , },
  { TEXT_INFO_ID       , "TEXT_INFO"       , },
  { LIGHT_ID           , "LIGHT"           , },
  { SPOTLIGHT_ID       , "SPOTLIGHT"       , },
  { CAMERA_ID          , "CAMERA"          , },
  { CAM_UNKNWN01_ID    , "CAM_UNKNWN01"    , },
  { CAM_UNKNWN02_ID    , "CAM_UNKNWN02"    , },
  { M3DMAGIC_ID        , "M3DMAGIC"        , },
  { MAT_NAME_ID        , "MAT_NAME"        , },
  { MAT_AMBIENT_ID     , "MAT_AMBIENT"     , },
  { MAT_DIFFUSE_ID     , "MAT_DIFFUSE"     , },
  { MAT_SPECULAR_ID    , "MAT_SPECULAR"    , },
  { MAT_SHININESS_ID   , "MAT_SHININESS"   , },
  { MAT_SHININESSP_ID  , "MAT_SHININESSP"  , },
  { MAT_TRANSPARENCY_ID, "MAT_TRANSPARENCY", },
  { MAT_TWO_SIDED_ID   , "MAT_TWO_SIDED"   , },
  { MAT_SHADING_ID     , "MAT_SHADING"     , },
  { MAT_SHADING_TYPE_ID, "MAT_SHADING_TYPE", },
  { MAT_TEXTURE_ID     , "MAT_TEXTURE"     , },
  { MAT_GLOSSY_ID      , "MAT_GLOSSY"      , },
  { MAP_OPACITY_ID     , "MAP_OPACITY"     , },
  { BUMP_MAP_ID        , "BUMP_MAP"        , },
  { MAT_ENTRY_ID       , "MAT_ENTRY"       , },
  { KEY_FRAME_ID       , "KEY_FRAME"       , },
  { MESH_INFO_ID       , "MESH_INFO_ID"    , },
  { SPOTLIGHT_INFO_ID  , "SPOTLIGHT_INFO"  , },
  { FRAMES_BLOCK_ID    , "FRAMES_BLOCK"    , },
  { OBJECT_NAME_ID     , "OBJECT_NAME"     , },
  { OBJECT_PIVOT_ID    , "OBJECT_PIVOT"    , },
  { POS_TRACK_ID       , "POS_TRACK"       , },
  { ROT_TRACK_ID       , "ROT_TRACK"       , },
  { SCALE_TRACK_ID     , "SCALE_TRACK"     , },
  { HIER_POS_ID        , "HIER_POS"        , },
  { 0                  , nullptr           , },
};

CImport3DS::
CImport3DS(CGeomScene3D *scene, const std::string &) :
 scene_(scene)
{
  if (! scene_) {
    scene_  = CGeometry3DInst->createScene3D();
    pscene_ = SceneP(scene_);
  }
}

CImport3DS::
~CImport3DS()
{
}

bool
CImport3DS::
read(CFile &file)
{
  file_ = &file;

  auto size = file_->getSize();

  CImport3DSChunk chunk(nullptr);

  readChunk(&chunk);

  if (chunk.id != M3DMAGIC_ID) {
    std::cerr << "Not a 3D Studio File\n";
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
      case NAMED_OBJECT_ID: {
        CGeomObject3D *object;
        readNamedObject(&chunk1, object);
        break;
      }
      case KEY_FRAME_ID: {
        readKeyFrame(&chunk1);
        break;
      }
      default:
        skipChunk(&chunk1);
        break;
    }
  }

  if (ulong(file_->getPos()) < size) {
    std::cerr << "Pos: " << file_->getPos() << " Size: " << size << "\n";
    return false;
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
      case NAMED_OBJECT_ID: {
        CGeomObject3D *object;
        readNamedObject(&chunk1, object);
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

  material_ = CGeometry3DInst->createMaterial();

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
      case MAT_SHININESSP_ID:
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
      case MAT_TEXTURE_ID: {
        TextureData texture;
        readMatTexture(&chunk1, texture);
        break;
      }
      case MAT_GLOSSY_ID: {
        TextureData texture;
        readMatTexture(&chunk1, texture);
        break;
      }
      case MAP_OPACITY_ID: {
        TextureData texture;
        readMatTexture(&chunk1, texture);
        break;
      }
      case BUMP_MAP_ID: {
        TextureData texture;
        readMatTexture(&chunk1, texture);
        break;
      }
      default:
        skipChunk(&chunk1);
        break;
    }
  }

  auto *material1 = pscene_->getMaterial(material_->name());

  if (! material1)
    pscene_->addMaterial(material_);

  if (isDebug())
    printMaterial(chunk, material_);

  return true;
}

void
CImport3DS::
printMaterial(CImport3DSChunk *chunk, CGeomMaterial *material)
{
  auto pad = getChunkPad(chunk);

  std::cout << pad  << "Material '" << material->name() << "'\n";

  std::cout << pad  << "  Ambient  " << material->getAmbient()  << "\n";
  std::cout << pad  << "  Diffuse  " << material->getDiffuse()  << "\n";
  std::cout << pad  << "  Specular " << material->getSpecular() << "\n";

  std::cout << pad  << "  Shininess    " << material->getShininess() << "\n";
  std::cout << pad  << "  Transparency " << material->transparency() << "\n";
  std::cout << pad  << "  Shading      " << CGeomMaterial::shadingName(material->shading()) << "\n";
}

bool
CImport3DS::
readMatName(CImport3DSChunk *chunk)
{
  auto name = readString(chunk);

  if (isDebug()) {
    auto pad = getChunkPad(chunk);

    std::cout << pad << "  Material '" << name << "'\n";
  }

  material_->setName(name);

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
      case INT_COLOR_24_ID:
      case LIN_INT_COLOR_24_ID: {
        readIntColor(chunk, rgba);

        material_->setAmbient(rgba);

        break;
      }
      case FLT_COLOR_24_ID:
      case LIN_FLT_COLOR_24_ID: {
        readFltColor(chunk, rgba);

        material_->setAmbient(rgba);

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
      case INT_COLOR_24_ID:
      case LIN_INT_COLOR_24_ID: {
        readIntColor(chunk, rgba);

        material_->setDiffuse(rgba);

        break;
      }
      case FLT_COLOR_24_ID:
      case LIN_FLT_COLOR_24_ID: {
        readFltColor(chunk, rgba);

        material_->setDiffuse(rgba);

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
      case INT_COLOR_24_ID:
      case LIN_INT_COLOR_24_ID: {
        readIntColor(chunk, rgba);

        material_->setSpecular(rgba);

        break;
      }
      case FLT_COLOR_24_ID:
      case LIN_FLT_COLOR_24_ID: {
        readFltColor(chunk, rgba);

        material_->setSpecular(rgba);

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

  float f_shininess { 0.0 };

  while (readChunk(&chunk1)) {
    switch (chunk1.id) {
      case INT_PERCENTAGE_ID: {
        ushort shininess;
        readIntPercentage(chunk, &shininess);
        f_shininess = shininess;
        break;
      }
      case FLOAT_PERCENTAGE_ID: {
        readFloatPercentage(chunk, &f_shininess);
        break;
      }
      default:
        skipChunk(&chunk1);
        break;
    }
  }

  material_->setShininess(f_shininess);

  return true;
}

bool
CImport3DS::
readMatTransparency(CImport3DSChunk *chunk)
{
  CImport3DSChunk chunk1(chunk);

  float f_transparency { 0.0 };

  while (readChunk(&chunk1)) {
    switch (chunk1.id) {
      case INT_PERCENTAGE_ID: {
        ushort transparency;
        readIntPercentage(chunk, &transparency);
        f_transparency = transparency;
        break;
      }
      case FLOAT_PERCENTAGE_ID: {
        readFloatPercentage(chunk, &f_transparency);
        break;
      }
      default:
        skipChunk(&chunk1);
        break;
    }
  }

  material_->setTransparency(f_transparency);

  return true;
}

bool
CImport3DS::
readMatTwoSided(CImport3DSChunk *)
{
  material_->setTwoSided(true);

  return true;
}

bool
CImport3DS::
readMatShading(CImport3DSChunk *chunk)
{
  // shading: 1=flat, 2=gouraud, 3=phong, 4=metal

  ushort shading;
  readShort(chunk, &shading);

  switch (shading) {
    case 0: material_->setShading(CGeomMaterial::Shading::WIRE   ); break;
    case 1: material_->setShading(CGeomMaterial::Shading::FLAT   ); break;
    case 2: material_->setShading(CGeomMaterial::Shading::GOURAUD); break;
    case 3: material_->setShading(CGeomMaterial::Shading::PHONG  ); break;
    case 4: material_->setShading(CGeomMaterial::Shading::METAL  ); break;
    case 5: material_->setShading(CGeomMaterial::Shading::BLINN  ); break;
    default: std::cerr << "Invalid material shading: " << shading << "\n;";
  }

  return true;
}

bool
CImport3DS::
readMatTexture(CImport3DSChunk *chunk, TextureData &texture)
{
  ushort type;
  readShort(chunk, &type);

  switch (type) {
    case MAP_FILENAME_ID:
      uint len;
      readLong(chunk, &len);

      texture.filename = readString(chunk);

      break;
    case INT_PERCENTAGE_ID: {
      ushort iblend;
      readIntPercentage(chunk, &iblend);
      texture.blend = iblend/100.0;
      break;
    }
    default:
      errorMsg("Unhandled texture data type : " + std::to_string(type));
      break;
  }

  skipChunk(chunk);

  if (isDebug()) {
    auto pad = getChunkPad(chunk);

    std::cout << pad << "Texture:" <<
      " Id=" << texture.filename << " Blend=" << texture.blend << "\n";
  }

  return true;
}

bool
CImport3DS::
readIntColor(CImport3DSChunk *chunk, CRGBA &rgba)
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
readFltColor(CImport3DSChunk *chunk, CRGBA &rgba)
{
  float r, g, b;

  readFloat(chunk, &r);
  readFloat(chunk, &g);
  readFloat(chunk, &b);

  rgba = CRGBA(r, g, b, 1.0);

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
readNamedObject(CImport3DSChunk *chunk, CGeomObject3D* &object)
{
  CImport3DSChunk chunk1(chunk);

  auto name = readString(chunk);

  if (isDebug()) {
    auto pad = getChunkPad(chunk);

    std::cout << pad << "  Name: " << name << "\n";
  }

  object = CGeometry3DInst->createObject3D(scene_, name);

  while (readChunk(&chunk1)) {
    switch (chunk1.id) {
      case N_TRI_OBJECT_ID:
        readNTriObject(&chunk1, object);
        break;
      case NAMED_OBJECT_ID: {
        CGeomObject3D *object1;
        readNamedObject(&chunk1, object1);

        object->addChild(object1);

        break;
      }
      case CAMERA_ID: {
        Camera camera;
        readCamera(&chunk1, &camera);
        break;
      }
      default:
        skipChunk(&chunk1);
        break;
    }
  }

  scene_->addObject(object);

  return true;
}

bool
CImport3DS::
readKeyFrame(CImport3DSChunk *)
{
  return false;
}

bool
CImport3DS::
readNTriObject(CImport3DSChunk *chunk, CGeomObject3D *object)
{
  CImport3DSChunk chunk1(chunk);

  while (readChunk(&chunk1)) {
    switch (chunk1.id) {
      case POINT_ARRAY_ID:
        readPointArray(&chunk1, object);
        break;
      case FACE_ARRAY_ID:
        readFaceArray(&chunk1, object);
        break;
      case MSH_MAT_GROUP_ID:
        readMshMatGroup(&chunk1, object);
        break;
      case SMOOTH_GROUP_ID:
        readSmoothGroup(&chunk1);
        break;
      case MESH_MATRIX_ID:
        readMeshMatrix(&chunk1, object);
        break;
      case MAP_COORDS_ID:
        readMapCoords(&chunk1, object);
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
readCamera(CImport3DSChunk *chunk, Camera *camera)
{
  readFloat(chunk, &camera->xPos);
  readFloat(chunk, &camera->yPos);
  readFloat(chunk, &camera->zPos);

  readFloat(chunk, &camera->xTgt);
  readFloat(chunk, &camera->yTgt);
  readFloat(chunk, &camera->zTgt);

  readFloat(chunk, &camera->roll);
  readFloat(chunk, &camera->fov );

  return true;
}

bool
CImport3DS::
readPointArray(CImport3DSChunk *chunk, CGeomObject3D *object)
{
  ushort num_points;
  readShort(chunk, &num_points);

  for (int i = 0; i < num_points; ++i) {
    float x, y, z;

    readFloat(chunk, &x);
    readFloat(chunk, &y);
    readFloat(chunk, &z);

    if (isDebug()) {
      auto pad = getChunkPad(chunk);

      std::cout << pad << "  Point " << i << ": " << x << " " << y << " " << z << "\n";
    }

    uint vertex_num = object->addVertex(CPoint3D(x, y, z));

    auto &vertex = object->getVertex(vertex_num);

    vertex.setViewed(vertex.getCurrent());
  }

  return true;
}

bool
CImport3DS::
readFaceArray(CImport3DSChunk *chunk, CGeomObject3D *object)
{
  vertexFaceList_.clear();

  uint num_vertices = object->getNumVertices();

  ushort num_faces;
  readShort(chunk, &num_faces);

  if (isDebug()) {
    auto pad = getChunkPad(chunk);

    std::cout << pad  << "Num faces " << num_faces << "\n";
  }

  ushort point_num[3];

  for (int i = 0; i < num_faces; ++i) {
    std::vector<uint> vertices;

    for (int j = 0; j < 3; j++) {
      readShort(chunk, &point_num[j]);

      if (point_num[j] < num_vertices)
        vertices.push_back(point_num[j]);
      else
        std::cerr << "Invalid Point Num " << point_num[j] << "\n";
    }

    ushort flags; // point flags

    readShort(chunk, &flags);

    if (isDebug()) {
      auto pad = getChunkPad(chunk);

      std::cout << pad << "  Face " << i << ":";

      for (int j = 0; j < 3; j++)
        std::cout << " " << point_num[j];

      std::cout << " (" << flags << ")";

      std::cout << "\n";
    }

    auto &vertex1 = object->getVertex(vertices[0]);
    auto &vertex2 = object->getVertex(vertices[1]);
    auto &vertex3 = object->getVertex(vertices[2]);

    auto orient = CMathGeom3D::PolygonOrientation(vertex1.getCurrent(),
                                                  vertex2.getCurrent(),
                                                  vertex3.getCurrent(),
                                                  CPoint3D(0, 0, 1));

    if (orient == CPOLYGON_ORIENTATION_ANTICLOCKWISE)
      std::swap(vertices[0], vertices[2]);

    uint face_num = object->addFace(vertices);

    CGeomFace3D &face = object->getFace(face_num);

    CVector3D normal;

    face.calcModelNormal(normal);

    face.setNormal(normal);

    for (int j = 0; j < 3; j++)
      vertexFaceList_[point_num[j]].push_back(face_num);
  }

  for (const auto &pvf : vertexFaceList_) {
    auto &vertex = object->getVertex(pvf.first);

    CVector3D normal(0, 0, 0);

    for (const auto &pf1 : pvf.second) {
      auto &face = object->getFace(pf1);

      normal += face.getNormal();
    }

    normal /= int(pvf.second.size());

    normal.normalize();

    vertex.setNormal(normal);
  }

  return true;
}

bool
CImport3DS::
readMshMatGroup(CImport3DSChunk *chunk, CGeomObject3D *object)
{
  auto name = readString(chunk);

  if (isDebug()) {
    auto pad = getChunkPad(chunk);

    std::cout << pad << "  Mat Group: " << name << "\n";
  }

  ushort num_faces;
  readShort(chunk, &num_faces);

  std::vector<ushort> face_nums;

  for (ushort i = 0; i < num_faces; ++i) {
    ushort face_num;
    readShort(chunk, &face_num);

    face_nums.push_back(face_num);
  }

  if (isDebug()) {
    auto pad = getChunkPad(chunk);

    std::cout << pad << "  Faces:";

    for (const auto &s : face_nums)
      std::cout << " " << s;

    std::cerr << "\n";
  }

  auto pmaterial = materials_.begin();

  for ( ; pmaterial != materials_.end(); ++pmaterial)
    if ((*pmaterial)->name() == name)
      break;

  if (pmaterial == materials_.end()) {
    std::cerr << "Material not found\n";
    return false;
  }

  auto *material = *pmaterial;

  if (isDebug())
    printMaterial(chunk, material);

  for (ushort i = 0; i < num_faces; ++i) {
    auto face_num = face_nums[i];

    if (face_num < object->getNumFaces()) {
      auto &face = object->getFace(face_num);

      face.setMaterialP(material);

      face.setTwoSided(material->isTwoSided());
    }
    else
      std::cerr << "Invalid face num " << face_num << "\n";
  }

  return true;
}

bool
CImport3DS::
readSmoothGroup(CImport3DSChunk *chunk)
{
  smoothGroupFaceList_.clear();

  ushort num_faces = ushort(uint(chunk->left)/sizeof(uint));

  if (isDebug()) {
    auto pad = getChunkPad(chunk);

    std::cout << pad << "Num Faces: " << num_faces << "\n";
  }

  for (ushort i = 0; i < num_faces; ++i) {
    uint smooth;

    if (! readLong(chunk, &smooth))
      return false;

    if (smooth == 0) continue;

    if (isDebug()) {
      auto pad = getChunkPad(chunk);

      std::cout << pad << "  Smooth Groups (" << std::hex << smooth << std::dec << ") = Bits";
    }

    for (uint b = 0; b < 32; ++b) {
      if (smooth & (1<<b)) {
        smoothGroupFaceList_[b].push_back(i);

        if (isDebug())
          std::cout << " " << b;
      }
    }

    if (isDebug())
      std::cout << "\n";
  }

  return true;
}

bool
CImport3DS::
readMeshMatrix(CImport3DSChunk *chunk, CGeomObject3D *)
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

  if (isDebug()) {
    auto pad = getChunkPad(chunk);

    std::cout << pad << "Mesh Matrix\n";
    pad += " ";

    std::cout << pad << matrix[0][0] << " " << matrix[1][0] << " " << matrix[2][0] << "\n";
    std::cout << pad << matrix[0][1] << " " << matrix[1][1] << " " << matrix[2][1] << "\n";
    std::cout << pad << matrix[0][2] << " " << matrix[1][2] << " " << matrix[2][2] << "\n";
    std::cout << pad << matrix[0][3] << " " << matrix[1][3] << " " << matrix[2][3] << "\n";
  }

#if 0
  // TODO
  CMatrix3DH m;
  m.setValues(matrix[0][0], matrix[1][0], matrix[2][0],
              matrix[0][1], matrix[1][1], matrix[2][1],
              matrix[0][2], matrix[1][2], matrix[2][2],
              matrix[0][3], matrix[1][3], matrix[2][3]);
#endif

  return true;
}

bool
CImport3DS::
readMapCoords(CImport3DSChunk *chunk, CGeomObject3D *object)
{
  ushort n;
  if (! readShort(chunk, &n))
    return false;

  for (int i = 0; i < int(n); ++i) {
    float x, y;
    readFloat(chunk, &x);
    readFloat(chunk, &y);

    if (isDebug()) {
      auto pad = getChunkPad(chunk);

      std::cout << pad << "  Texture Point " << i << ": " << x << " " << y << "\n";
    }

    (void) object->addTexturePoint(CPoint2D(x, y));
  }

  return true;
}

bool
CImport3DS::
readChunk(CImport3DSChunk *chunk)
{
  if (chunk->parent != nullptr && chunk->parent->left <= 0) {
    if (isDebug()) {
      auto pad = getChunkPad(chunk);

      std::cerr << pad << "Chunk EOF\n";
    }

    return false;
  }

  chunk->left = 6;

  if (! readShort(chunk, &chunk->id)) {
    std::cerr << "Failed to read chunk id\n";
    return false;
  }

  if (! readLong(chunk, &chunk->len)) {
    std::cerr << "Failed to read chunk length\n";
    return false;
  }

  chunk->left = chunk->len - 6;

  if (isDebug())
    printChunk(chunk);

  return true;
}

bool
CImport3DS::
skipChunk(CImport3DSChunk *chunk)
{
  if (chunk->left <= 0)
    return true;

  if (isDebug()) {
    auto pad = getChunkPad(chunk);

    std::cerr << pad << "  !!Skip Chunk!! " << getChunkName(chunk) <<
                 " (" << chunk->left << ")\n";
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

  *c = uchar(c1);

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

  *s = ushort((file_data.getDataPos(1) << 8) | file_data.getDataPos(0));

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

  *l = uint((file_data.getDataPos(3) << 24) | (file_data.getDataPos(2) << 16) |
            (file_data.getDataPos(1) <<  8) |  file_data.getDataPos(0));

  adjustChunkLeft(chunk, 4);

  return true;
}

std::string
CImport3DS::
readString(CImport3DSChunk *chunk)
{
  if (uint(chunk->left) > bufferMax_) {
    bufferMax_ = uint(chunk->left + 32);

    delete [] buffer_;

    buffer_ = new char [bufferMax_ + 1];
  }

  int i = 0;

  while (chunk->left > 0) {
    int c = file_->getC();

    if (c == EOF)
      break;

    adjustChunkLeft(chunk, sizeof(uchar));

    if (c == '\0')
      break;

    buffer_[i++] = char(c);
  }

  buffer_[i] = '\0';

  std::string str = std::string(buffer_);

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
getChunkName(CImport3DSChunk *chunk, bool showId)
{
  static char name[256];

  if (! chunk)
    return "<NULL>";

  for (int i = 0; chunk_names[i].name != nullptr; ++i) {
    if (chunk_names[i].id != chunk->id)
      continue;

    if (showId)
      sprintf(name, "%s (%x)", chunk_names[i].name, chunk->id);
    else
      sprintf(name, "%s", chunk_names[i].name);

    return std::string(name);
  }

  if (showId)
    sprintf(name, "???? (%x)", chunk->id);
  else
    strcpy(name, "????");

  return std::string(name);
}

void
CImport3DS::
adjustChunkLeft(CImport3DSChunk *chunk, int offset)
{
  chunk->left -= offset;

  if (chunk->parent)
    adjustChunkLeft(chunk->parent, offset);
}

void
CImport3DS::
printChunk(CImport3DSChunk *chunk)
{
  auto pad = getChunkPad(chunk);

  std::cout << pad << "Chunk ";

  std::deque<CImport3DSChunk *> parents;

  auto *parent = chunk->parent;

  while (parent) {
    parents.push_front(parent);

    parent = parent->parent;
  }

  for (auto *p: parents)
    std::cout << getChunkName(p, /*showId*/false) << "/";

  std::cout << getChunkName(chunk) << " " << chunk->left << "\n";
}

uint
CImport3DS::
getChunkDepth(CImport3DSChunk *chunk)
{
  uint depth = 0;

  CImport3DSChunk *parent = chunk->parent;

  while (parent) {
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

void
CImport3DS::
errorMsg(const std::string &msg) const
{
  std::cerr << msg << "\n";
}
