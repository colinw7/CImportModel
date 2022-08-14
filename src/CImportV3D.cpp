#include <CImportV3D.h>
#include <CGeometry3D.h>
#include <CStrUtil.h>

CImportV3D::
CImportV3D(CGeomScene3D *scene, const std::string &name) :
 scene_(scene)
{
  if (! scene_) {
    scene_  = CGeometryInst->createScene3D();
    pscene_ = scene_;
  }

  object_ = CGeometryInst->createObject3D(scene_, name);

  scene_->addObject(object_);

  if (! pobject_)
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

    CStrWords words = CStrUtil::toWords(line, nullptr);

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
    CStrWords words = CStrUtil::toWords(line, nullptr);

    if (words.size() == 0)
      continue;

    // sub face
    if (line[0] == ' ') {
      if (face_num == -1)
        throw "Invalid Sub Face/Line";

      long num_faces = CStrUtil::toInteger(line);

      for (int i = 0; i < num_faces; i++) {
        file.readLine(line);

        CStrWords words1 = CStrUtil::toWords(line, nullptr);

        long num_vertices_1 = CStrUtil::toInteger(words1[0].getWord());

        if (int(words1.size()) < num_vertices_1 + 2)
          throw "Invalid Sub Face/Line";

        if (num_vertices_1 <= 1)
          throw "Invalid Sub Face/Line";

        long color = CStrUtil::toInteger(words1[int(num_vertices_1 + 1)].getWord());

        CRGBA rgba;

        getColorRGBA(color, &rgba);

        if (num_vertices_1 > 2) {
          std::vector<uint> vertices;

          for (int j = 0; j < num_vertices_1; j++) {
            long vertex_num = CStrUtil::toInteger(words1[j + 1].getWord());

            if (vertex_num < 0 || vertex_num >= int(object_->getNumVertices()))
              throw "Invalid Sub Face";

            vertices.push_back(uint(vertex_num));
          }

          uint ind = object_->addFaceSubFace(uint(face_num), vertices);

          object_->setSubFaceColor(uint(face_num), ind, rgba);
        }
        else {
          long vertex_num1 = CStrUtil::toInteger(words1[1].getWord());
          long vertex_num2 = CStrUtil::toInteger(words1[2].getWord());

          uint ind = object_->addFaceSubLine(uint(face_num), uint(vertex_num1), uint(vertex_num2));

          object_->setSubLineColor(uint(face_num), ind, rgba);
        }
      }
    }
    else {
      long num_vertices_1 = CStrUtil::toInteger(words[0].getWord());

      if (long(words.size()) < num_vertices_1 + 2)
        throw "Invalid Face/Line";

      if (num_vertices_1 <= 1)
        throw "Invalid Face/Line";

      long color = CStrUtil::toInteger(words[int(num_vertices_1 + 1)].getWord());

      CRGBA rgba;

      getColorRGBA(color, &rgba);

      if (num_vertices_1 > 2) {
        std::vector<uint> vertices;

        for (int i = 0; i < num_vertices_1; i++) {
          long vertex_num = CStrUtil::toInteger(words[i + 1].getWord());

          if (vertex_num < 0 || vertex_num >= int(object_->getNumVertices()))
            throw "Invalid Face";

          vertices.push_back(uint(vertex_num));
        }

        face_num = int(object_->addFace(vertices));

        object_->setFaceColor(uint(face_num), rgba);

        if (both_sides_) {
          std::vector<uint> vertices1;

          for (int i = int(num_vertices_1 - 1); i >= 0; i--) {
            long vertex_num = CStrUtil::toInteger(words[i + 1].getWord());

            if (vertex_num < 0 || vertex_num >= int(object_->getNumVertices()))
              throw "Invalid Face";

            vertices1.push_back(uint(vertex_num));
          }

          face_num = int(object_->addFace(vertices1));

          object_->setFaceColor(uint(face_num), rgba);
        }
      }
      else {
        long vertex_num1 = CStrUtil::toInteger(words[1].getWord());
        long vertex_num2 = CStrUtil::toInteger(words[2].getWord());

        uint ind = object_->addLine(uint(vertex_num1), uint(vertex_num2));

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
