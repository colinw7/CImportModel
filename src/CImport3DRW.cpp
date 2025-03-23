#include <CImport3DRW.h>
#include <CStrUtil.h>
#include <CGeometry3D.h>

struct Point {
  short x, y, z;
};

struct Face {
  short color;
  short point11, point12;
  short point21, point22;
  short point31, point32;
};

CImport3DRW::
CImport3DRW(CGeomScene3D *scene, const std::string &name) :
 scene_(scene)
{
  if (! scene_) {
    scene_  = CGeometryInst->createScene3D();
    pscene_ = SceneP(scene_);
  }

  object_ = CGeometryInst->createObject3D(scene_, name);

  scene_->addObject(object_);

  if (! pobject_)
    pobject_ = ObjectP(object_);
}

CImport3DRW::
~CImport3DRW()
{
}

bool
CImport3DRW::
read(CFile &file)
{
  char id[4];

  if (! file.read(reinterpret_cast<uchar *>(id), sizeof(id)))
    return false;

  if (strncmp(id, "3DRW", 4) != 0)
    return false;

  short ver;

  if (! file.read(reinterpret_cast<uchar *>(&ver), sizeof(ver)))
    return false;

  int offset;

  if (! file.read(reinterpret_cast<uchar *>(&offset), sizeof(offset)))
    return false;

  swapInt(&offset);

  short num_faces;

  if (! file.read(reinterpret_cast<uchar *>(&num_faces), sizeof(num_faces)))
    return false;

  swapShort(&num_faces);

  ++num_faces;

  if (debug_)
    std::cerr << "Num Faces = " << num_faces << std::endl;

  auto *faces = new Face [uint(num_faces)];

  if (! file.read(reinterpret_cast<uchar *>(faces), uint(num_faces)*sizeof(Face)))
    return false;

  for (int i = 0; i < num_faces; ++i) {
    swapShort(&faces[i].point11); swapShort(&faces[i].point12);
    swapShort(&faces[i].point21); swapShort(&faces[i].point22);
    swapShort(&faces[i].point31); swapShort(&faces[i].point32);

    //++faces[i].point11; ++faces[i].point12;
    //++faces[i].point21; ++faces[i].point22;
    //++faces[i].point31; ++faces[i].point32;
  }

  if (debug_)
    std::cerr <<
      CStrUtil::strprintf("%d -> %d, %d -> %d, %d -> %d",
                          faces[0].point11, faces[0].point12,
                          faces[0].point21, faces[0].point22,
                          faces[0].point31, faces[0].point32) << std::endl;

  file.setPos(offset);

  short num_points;

  if (! file.read(reinterpret_cast<uchar *>(&num_points), sizeof(num_points)))
    return false;

  swapShort(&num_points);

  ++num_points;

  if (debug_)
    std::cerr << "Num Points = " << num_points << std::endl;

  auto *points = new Point [uint(num_points)];

  if (! file.read(reinterpret_cast<uchar *>(points), uint(num_points)*sizeof(Point)))
    return false;

  for (int i = 0; i < num_points; ++i) {
    swapShort(&points[i].x);
    swapShort(&points[i].y);
    swapShort(&points[i].z);
  }

  for (int i = 0; i < num_points; i++) {
    double x = points[i].x/32768.0;
    double y = points[i].y/32768.0;
    double z = points[i].z/32768.0;

    object_->addVertex(CPoint3D(x, y, z));
  }

  std::vector<uint> face_points;

  face_points.resize(3);

  for (int i = 0; i < num_faces; i++) {
    face_points[0] = uint(faces[i].point11);
    face_points[1] = uint(faces[i].point21);
    face_points[2] = uint(faces[i].point31);

    object_->addFace(face_points);
  }

  delete [] points;
  delete [] faces;

  return true;
}

void
CImport3DRW::
swapInt(int *word)
{
  int word1 =  *word        & 0x000000FF;
  int word2 = (*word >> 8 ) & 0x000000FF;
  int word3 = (*word >> 16) & 0x000000FF;
  int word4 = (*word >> 24) & 0x000000FF;

  *word = int(word1 << 24 | word2 << 16 | word3 << 8 | word4);
}

void
CImport3DRW::
swapShort(short *word)
{
  int word1 =  *word        & 0x000000FF;
  int word2 = (*word >> 8 ) & 0x000000FF;

  *word = short(word1 << 8 | word2);
}
