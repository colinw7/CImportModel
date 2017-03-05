#include <CImportV3D.h>
#include <CGeometry3D.h>
#include <CStrUtil.h>

CImportV3D::
CImportV3D(CGeomScene3D *scene, const std::string &name) :
 scene_(scene), both_sides_(false), debug_(false)
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

CImportV3D::
~CImportV3D()
{
}

bool
CImportV3D::
read(CFile &file)
{
  std::string line;

  file.readLine(line);

  if (line != "3DG1") {
    std::cerr << "Not a V3D File" << std::endl;
    return false;
  }

  file.readLine(line);

  long num_vertices = CStrUtil::toInteger(line);

  for (int i = 0; i < num_vertices; i++) {
    file.readLine(line);

    CStrWords words = CStrUtil::toWords(line, NULL);

    if (words.size() != 3)
      throw "Invalid Point";

    double x = CStrUtil::toReal(words[0].getWord());
    double y = CStrUtil::toReal(words[1].getWord());
    double z = CStrUtil::toReal(words[2].getWord());

    if (debug_)
      std::cout << "Point " << i << ": " << x << " " << y << " " << z << std::endl;

    object_->addVertex(CPoint3D(x, y, z));
  }

  int face_num = -1;

  while (file.readLine(line)) {
    CStrWords words = CStrUtil::toWords(line, NULL);

    if (words.size() == 0)
      continue;

    if (line[0] == ' ') {
      if (face_num == -1)
        throw "Invalid Sub Face/Line";

      long num_faces = CStrUtil::toInteger(line);

      for (int i = 0; i < num_faces; i++) {
        file.readLine(line);

        CStrWords words = CStrUtil::toWords(line, NULL);

        long num_vertices = CStrUtil::toInteger(words[0].getWord());

        if (words.size() < num_vertices + 2)
          throw "Invalid Sub Face/Line";

        if (num_vertices <= 1)
          throw "Invalid Sub Face/Line";

        long color = CStrUtil::toInteger(words[num_vertices + 1].getWord());

        CRGBA rgba;

        getColorRGBA(color, &rgba);

        if (num_vertices > 2) {
          std::vector<uint> vertices;

          for (int j = 0; j < num_vertices; j++) {
            long vertex_num = CStrUtil::toInteger(words[j + 1].getWord());

            if (vertex_num < 0 || vertex_num >= (int) object_->getNumVertices())
              throw "Invalid Sub Face";

            vertices.push_back(vertex_num);
          }

          uint ind = object_->addFaceSubFace(face_num, vertices);

          object_->setSubFaceColor(face_num, ind, rgba);
        }
        else {
          long vertex_num1 = CStrUtil::toInteger(words[1].getWord());
          long vertex_num2 = CStrUtil::toInteger(words[2].getWord());

          uint ind =
            object_->addFaceSubLine(face_num, vertex_num1, vertex_num2);

          object_->setSubLineColor(face_num, ind, rgba);
        }
      }
    }
    else {
      long num_vertices = CStrUtil::toInteger(words[0].getWord());

      if (words.size() < num_vertices + 2)
        throw "Invalid Face/Line";

      if (num_vertices <= 1)
        throw "Invalid Face/Line";

      long color = CStrUtil::toInteger(words[num_vertices + 1].getWord());

      CRGBA rgba;

      getColorRGBA(color, &rgba);

      if (num_vertices > 2) {
        std::vector<uint> vertices;

        for (int i = 0; i < num_vertices; i++) {
          long vertex_num = CStrUtil::toInteger(words[i + 1].getWord());

          if (vertex_num < 0 || vertex_num >= (int) object_->getNumVertices())
            throw "Invalid Face";

          vertices.push_back(vertex_num);
        }

        face_num = object_->addFace(vertices);

        object_->setFaceColor(face_num, rgba);

        if (both_sides_) {
          std::vector<uint> vertices;

          for (int i = num_vertices - 1; i >= 0; i--) {
            long vertex_num = CStrUtil::toInteger(words[i + 1].getWord());

            if (vertex_num < 0 || vertex_num >= (int) object_->getNumVertices())
              throw "Invalid Face";

            vertices.push_back(vertex_num);
          }

          face_num = object_->addFace(vertices);

          object_->setFaceColor(face_num, rgba);
        }
      }
      else {
        long vertex_num1 = CStrUtil::toInteger(words[1].getWord());
        long vertex_num2 = CStrUtil::toInteger(words[2].getWord());

        uint ind = object_->addLine(vertex_num1, vertex_num2);

        object_->setLineColor(ind, rgba);
      }
    }
  }

  return true;
}

void
CImportV3D::
getColorRGBA(long color, CRGBA *rgba)
{
  if (color < 0)
    color = -color;

  *rgba = CRGBA(((color >> 0) & 0x7)/7.0,
                ((color >> 3) & 0x7)/7.0,
                ((color >> 6) & 0x7)/7.0,
                1.0);
}
