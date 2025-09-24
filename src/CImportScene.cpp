#include <CImportScene.h>
#include <CGeomScene3D.h>
#include <CGeomObject3D.h>
#include <CGeometry3D.h>

// shapes
#include <CGeomBox3D.h>
#include <CGeomCone3D.h>
//#include <CGeomCube3D.h> // same as BBox
#include <CGeomCylinder3D.h>
#include <CGeomHyperboloid3D.h>
#include <CGeomPyramid3D.h>
#include <CGeomSphere3D.h>
#include <CGeomTorus3D.h>
#include <CSG.h>

#include <CStrUtil.h>
#include <CMathGen.h>
#include <CImageMgr.h>
#include <CImage.h>
#include <CRGBName.h>

#define Q(x) #x
#define QUOTE(x) Q(x)

//---

#define ORIENTATION_CMD 1
#define SCENE_CMD       2
#define OBJECT_CMD      3
#define PRIMITIVE_CMD   4
#define COLORS_CMD      5
#define COLOURS_CMD     6
#define TEXTURES_CMD    7
#define CSG_CMD         8

static const char *
global_commands[] = {
  "Orientation",
  "Scene",
  "Object",
  "Primitive",
  "Colors",
  "Colours",
  "Textures",
  "CSG",
  nullptr,
};

//---

#define SCENE_OBJECT_CMD     1
#define SCENE_PROJECTION_CMD 2
#define SCENE_EYE_CMD        3
#define SCENE_WINDOW_CMD     4
#define SCENE_END_CMD        5

static const char *
scene_commands[] = {
  "Object",
  "Projection",
  "Eye",
  "Window",
  "End",
  nullptr,
};

//---

#define PRIMITIVE_FACES_CMD     1
#define PRIMITIVE_SUB_FACES_CMD 2
#define PRIMITIVE_LINES_CMD     3
#define PRIMITIVE_SUB_LINES_CMD 4
#define PRIMITIVE_POINTS_CMD    5
#define PRIMITIVE_ROTATE_CMD    6
#define PRIMITIVE_OBJECT_CMD    7
#define PRIMITIVE_END_CMD       8

static const char *
primitive_commands[] = {
  "Faces",
  "SubFaces",
  "Lines",
  "SubLines",
  "Points",
  "Rotate",
  "Object",
  "End",
  nullptr,
};

//---

#define CSG_CUBE_CMD     1
#define CSG_SPHERE_CMD   2
#define CSG_CYLINDER_CMD 3
#define CSG_COLOR_CMD    4
#define CSG_END_CMD      5

static const char *
csg_commands[] = {
  "Cube",
  "Sphere",
  "Cylinder",
  "Color",
  "End",
  nullptr,
};

//---

#define OBJECT_PRIMITIVE_CMD       1
#define OBJECT_FLIP_ORIENTATION    2
#define OBJECT_FACE_COLOR_CMD      3
#define OBJECT_FACE_COLOUR_CMD     4
#define OBJECT_SUB_FACE_COLOR_CMD  5
#define OBJECT_SUB_FACE_COLOUR_CMD 6
#define OBJECT_LINE_COLOR_CMD      7
#define OBJECT_LINE_COLOUR_CMD     8
#define OBJECT_SUB_LINE_COLOR_CMD  9
#define OBJECT_SUB_LINE_COLOUR_CMD 10
#define OBJECT_TEXTURE_CMD         11
#define OBJECT_COVER_TEXTURE_CMD   12
#define OBJECT_MASK_CMD            13
#define OBJECT_COVER_MASK_CMD      14
#define OBJECT_TRANSFORMS_CMD      15
#define OBJECT_END_CMD             16

static const char *
object_commands[] = {
  "Primitive",
  "Flip_Orientation",
  "Face_Color",
  "Face_Colour",
  "SubFace_Color",
  "SubFace_Colour",
  "Line_Color",
  "Line_Colour",
  "SubLine_Color",
  "SubLine_Colour",
  "Texture",
  "CoverTexture",
  "Mask",
  "CoverMask",
  "Transforms",
  "End",
  nullptr,
};

//---

#define FACES_END_CMD 1

static const char *
faces_commands[] = {
  "End",
  nullptr,
};

//---

#define LINES_END_CMD 1

static const char *
lines_commands[] = {
  "End",
  nullptr,
};

//---

#define POINTS_END_CMD 1

static const char *
points_commands[] = {
  "End",
  nullptr,
};

//---

#define ROTATE_END_CMD 1

static const char *
rotate_commands[] = {
  "End",
  nullptr,
};

//---

#define COLORS_END_CMD 1

static const char *
colors_commands[] = {
  "End",
  nullptr,
};

//---

#define TRANSFORMS_TRANSLATE_CMD 1
#define TRANSFORMS_SCALE_CMD     2
#define TRANSFORMS_ROTATE_X_CMD  3
#define TRANSFORMS_ROTATE_Y_CMD  4
#define TRANSFORMS_ROTATE_Z_CMD  5
#define TRANSFORMS_END_CMD       6

static const char *
transforms_commands[] = {
  "Translate",
  "Scale",
  "RotateX",
  "RotateY",
  "RotateZ",
  "End",
  nullptr,
};

//---

class LineWords {
 public:
  LineWords(const std::string &line) {
    words_ = CStrUtil::toWords(line, nullptr);
  }

  std::string getWord(uint i, const std::string &def="") const {
    if (i >= words_.size())
      return def;

    return words_[i].getWord();
  }

  int getInteger(uint i, int def=0) const {
    if (i >= words_.size())
      return def;

    return int(CStrUtil::toInteger(words_[i].getWord()));
  }

  double getReal(uint i, double def=0.0) const {
    if (i >= words_.size())
      return def;

    return CStrUtil::toReal(words_[i].getWord());
  }

  size_t size() const { return words_.size(); }

 private:
  CStrWords words_;
};

//---

CImportScene::
CImportScene(CGeomScene3D *scene, const std::string &) :
 scene_(scene)
{
  modelDir_ = QUOTE(MODEL_DIR);

  if (! scene_) {
    scene_  = CGeometryInst->createScene3D();
    pscene_ = SceneP(scene_);
  }
}

CImportScene::
~CImportScene()
{
}

bool
CImportScene::
read(CFile &file)
{
  file_ = &file;

  std::string line;

  bool end_command = false;

  while (! end_command && file_->readLine(line)) {
    if (isSkipLine(line))
      continue;

    auto words = LineWords(line);

    int command_num = lookupCommand(words.getWord(0), global_commands);

    switch (command_num) {
      case ORIENTATION_CMD:
        orientation_ = words.getInteger(1);

        break;
      case SCENE_CMD:
        readScene();

        break;
      case OBJECT_CMD:
        readObject(words.getWord(1));

        break;
      case PRIMITIVE_CMD:
        readPrimitive(words.getWord(1));

        break;
      case COLORS_CMD:
      case COLOURS_CMD:
        readColors();

        break;
      case TEXTURES_CMD:
        readTextures();

        break;
      case CSG_CMD:
        readCSG(words.getWord(1));

        break;
      default:
        errorMsg("Unrecognised Command '" + words.getWord(0) + "'");
        break;
    }
  }

  return true;
}

void
CImportScene::
readScene()
{
  std::string line;

  bool end_command = false;

  while (! end_command && file_->readLine(line)) {
    if (isSkipLine(line))
      continue;

    auto words = LineWords(line);

    int command_num = lookupCommand(words.getWord(0), scene_commands);

    switch (command_num) {
      case SCENE_OBJECT_CMD: {
        auto *object = getPrimitive(words.getWord(1));

        if (! object) {
          errorMsg("Unrecognised Object '" + words.getWord(1) + "'");
          return;
        }

        addObject(words.getWord(1));

        break;
      }
      case SCENE_PROJECTION_CMD:
        break;
      case SCENE_EYE_CMD:
        break;
      case SCENE_WINDOW_CMD:
        break;
      case SCENE_END_CMD:
        end_command = true;

        break;
      default:
        errorMsg("Unrecognised Scene Command '" + words.getWord(0) + "'");
        break;
    }
  }
}

void
CImportScene::
addObject(const std::string &name)
{
  auto *primitive = getPrimitive(name);

  if (! primitive) {
    if      (name == "Sphere") {
      primitive = CGeometryInst->createObject3D(scene_, "sphere");

      CGeomSphere3D::addGeometry(primitive, CPoint3D(0.0, 0.0, 0.0), 1.0);

      CGeomSphere3D::addTexturePoints(primitive);
      CGeomSphere3D::addNormals(primitive, 1.0);
    }
    else if (name == "Cube" || name == "Box") {
      primitive = CGeometryInst->createObject3D(scene_, "cube");

      CGeomBox3D::addGeometry(primitive, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0);
    }
    else if (name == "Cone") {
      primitive = CGeometryInst->createObject3D(scene_, "cone");

      CGeomCone3D::addGeometry(primitive, CPoint3D(0.0, 0.0, 0.0), 1.0, 1.0);
      CGeomCone3D::addNormals(primitive, 1.0, 1.0);
    }
    else if (name == "Cylinder") {
      primitive = CGeometryInst->createObject3D(scene_, "cylinder");

      CGeomCylinder3D::addGeometry(primitive, CPoint3D(0.0, 0.0, 0.0), 1.0, 1.0);

      CGeomCylinder3D::addNormals(primitive, 1.0, 1.0);
    }
    else if (name == "Torus") {
      primitive = CGeometryInst->createObject3D(scene_, "torus");

      CGeomTorus3D::addGeometry(primitive,
                                0.0, 0.0, 0.0, // center
                                1.0, 0.25, // radii);
                                1.0, 0.2, 36, 36);
    }
    else if (name == "Pyramid") {
      primitive = CGeometryInst->createObject3D(scene_, "pyramid");

      CGeomPyramid3D::addGeometry(primitive,
                                  0.0, 0.0, 0.0, // center
                                  1.0, 1.0); // w, h
    }
    else if (name == "Hyperboloid") {
      primitive = CGeometryInst->createObject3D(scene_, "hyperboloid");

      CGeomHyperboloid3D::addGeometry(primitive, CPoint3D(0.0, 0.0, 0.0), CPoint3D(1.0, 1.0, 1.0));

      CGeomHyperboloid3D::addNormals(primitive, CPoint3D(0.0, 0.0, 0.0), CPoint3D(1.0, 1.0, 1.0));
    }
  }

  if (! primitive) {
    errorMsg("Unrecognised Primitive '" + name + "'");
    return;
  }

  auto *object1 = primitive->dup();

  object1->setScene(scene_);

  scene_->addObject(object1);
}

void
CImportScene::
readPrimitive(const std::string &name)
{
  auto *primitive = CGeometryInst->createObject3D(scene_, name);

  primitive->setName(name);

  //---

  std::string line;

  bool end_command = false;

  while (! end_command && file_->readLine(line)) {
    if (isSkipLine(line))
      continue;

    auto words = LineWords(line);

    int command_num = lookupCommand(words.getWord(0), primitive_commands);

    switch (command_num) {
      case PRIMITIVE_FACES_CMD: {
        readFaces(primitive, -1);

        break;
      }
      case PRIMITIVE_SUB_FACES_CMD: {
        auto face_num = words.getInteger(1);

        if (face_num <= 0 || face_num > int(primitive->getNumFaces())) {
          errorMsg("SubFace Face " + std::to_string(face_num) + " Not Found");
          break;
        }

        readFaces(primitive, int(face_num - 1));

        break;
      }
      case PRIMITIVE_LINES_CMD: {
        readLines(primitive, -1);

        break;
      }
      case PRIMITIVE_SUB_LINES_CMD: {
        auto face_num = words.getInteger(1);

        if (face_num <= 0 || face_num > int(primitive->getNumFaces())) {
          errorMsg("SubLine Face " + std::to_string(face_num) + " Not Found");
          break;
        }

        readLines(primitive, int(face_num - 1));

        break;
      }
      case PRIMITIVE_POINTS_CMD: {
        readVertices(primitive);

        break;
      }
      case PRIMITIVE_ROTATE_CMD: {
        auto num_patches = words.getInteger(1);

        readRotate(primitive, int(num_patches));

        break;
      }
      case PRIMITIVE_OBJECT_CMD: {
        auto modelName = words.getWord(1);
        auto format    = CImportBase::filenameToType(modelName);

        // import model
        auto *model = CImportBase::createModel(format, "");

        if (! model) {
          errorMsg("Invalid format for file '" + modelName + "'");
          break;
        }

        auto fileName = modelName;

        if (! CFile::exists(fileName)) {
          fileName = modelDir_ + "/" + modelName;

          if (! CFile::exists(fileName)) {
            errorMsg("Model file does not exist for '" + modelName + "'");
            break;
          }
        }

        CFile file(fileName);

        if (! model->read(file)) {
          errorMsg("Failed to read model '" + fileName + "'");
          break;
        }

        auto *scene = model->releaseScene();

        delete model;

        delete primitive;

        primitive = nullptr;

        for (auto *object : scene->getObjects()) {
          primitive = object;
          break;
        }

        if (primitive)
          primitive->setName(name);

        break;
      }
      case PRIMITIVE_END_CMD:
        end_command = true;

        break;
      default:
        errorMsg("Unrecognised Primitive Command '" + words.getWord(0) + "'");
        break;
    }
  }

  scene_->addPrimitive(primitive);
}

void
CImportScene::
readObject(const std::string &name)
{
  auto *object = CGeometryInst->createObject3D(scene_, name);

  object->setName(name);

  std::string line;

  bool end_command = false;

  while (! end_command && file_->readLine(line)) {
    if (isSkipLine(line))
      continue;

    auto words = LineWords(line);

    int command_num = lookupCommand(words.getWord(0), object_commands);

    switch (command_num) {
      case OBJECT_PRIMITIVE_CMD: {
        std::string name1 = words.getWord(1);

        auto *primitive = getPrimitive(name1);

        if (! primitive) {
          if      (name1 == "Sphere") {
            primitive = CGeometryInst->createObject3D(scene_, name);

            CGeomSphere3D::addGeometry(primitive, CPoint3D(0.0, 0.0, 0.0), 1.0);

            CGeomSphere3D::addTexturePoints(primitive);
            CGeomSphere3D::addNormals(primitive, 1.0);
          }
          else if (name1 == "Cube" || name == "Box") {
            primitive = CGeometryInst->createObject3D(scene_, name);

            CGeomBox3D::addGeometry(primitive, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0);
          }
          else if (name1 == "Cone") {
            primitive = CGeometryInst->createObject3D(scene_, name);

            CGeomCone3D::addGeometry(primitive, CPoint3D(0.0, 0.0, 0.0), 1.0, 1.0);
            CGeomCone3D::addNormals(primitive, 1.0, 1.0);
          }
          else if (name1 == "Cylinder") {
            primitive = CGeometryInst->createObject3D(scene_, name);

            CGeomCylinder3D::addGeometry(primitive, CPoint3D(0.0, 0.0, 0.0), 1.0, 1.0);

            CGeomCylinder3D::addNormals(primitive, 1.0, 1.0);
          }
          else if (name == "Torus") {
            primitive = CGeometryInst->createObject3D(scene_, name);

            CGeomTorus3D::addGeometry(primitive,
                                      0.0, 0.0, 0.0, // center
                                      1.0, 0.25, // radii);
                                      1.0, 0.2, 36, 36);
          }
          else if (name == "Pyramid") {
            primitive = CGeometryInst->createObject3D(scene_, name);

            CGeomPyramid3D::addGeometry(primitive,
                                        0.0, 0.0, 0.0, // center
                                        1.0, 1.0); // w, h
          }
          else if (name == "Hyperboloid") {
            primitive = CGeometryInst->createObject3D(scene_, "hyperboloid");

            CGeomHyperboloid3D::addGeometry(primitive, CPoint3D(0.0, 0.0, 0.0),
                                            CPoint3D(1.0, 1.0, 1.0));

            CGeomHyperboloid3D::addNormals(primitive, CPoint3D(0.0, 0.0, 0.0),
                                           CPoint3D(1.0, 1.0, 1.0));
          }
        }

        if (! primitive) {
          errorMsg("Unrecognised Primitive '" + name1 + "'");
          break;
        }

        delete object;

        object = primitive->dup();

        object->setScene(scene_);

        object->setName(name);

        break;
      }
      case OBJECT_FLIP_ORIENTATION:
        //object->flip_orientation = true;

        break;
      case OBJECT_FACE_COLOR_CMD:
      case OBJECT_FACE_COLOUR_CMD: {
        auto rgba = wordToColor(words.getWord(1));

        object->setFaceColor(rgba);

        break;
      }
      case OBJECT_SUB_FACE_COLOR_CMD:
      case OBJECT_SUB_FACE_COLOUR_CMD: {
        auto rgba = wordToColor(words.getWord(1));

        object->setSubFaceColor(rgba);

        break;
      }
      case OBJECT_LINE_COLOR_CMD:
      case OBJECT_LINE_COLOUR_CMD:
        break;
      case OBJECT_SUB_LINE_COLOR_CMD:
      case OBJECT_SUB_LINE_COLOUR_CMD:
        break;
      case OBJECT_TEXTURE_CMD: {
        auto texture_num = words.getInteger(1);

        if (texture_num >= 1 && texture_num <= int(textures_.size())) {
          CFile file(textures_[uint(texture_num - 1)]);

          CImageFileSrc src(file);

          auto image = CImageMgrInst->createImage(src);

          object->setTexture(image);
        }
        else
          errorMsg("Invalid texture number " + std::to_string(texture_num));

        break;
      }
      case OBJECT_COVER_TEXTURE_CMD: {
        auto texture_num = words.getInteger(1);

        if (texture_num >= 1 && texture_num <= int(textures_.size())) {
          CFile file(textures_[uint(texture_num - 1)]);

          CImageFileSrc src(file);

          auto image = CImageMgrInst->createImage(src);

          object->mapTexture(image);
        }
        else
          errorMsg("Invalid texture number " + std::to_string(texture_num));

        break;
      }
      case OBJECT_MASK_CMD: {
        auto mask_num = words.getInteger(1);

        if (mask_num >= 1 && mask_num <= int(textures_.size())) {
          CFile file(textures_[uint(mask_num - 1)]);

          CImageFileSrc src(file);

          auto image = CImageMgrInst->createImage(src);

          object->setMask(image);
        }
        else
          errorMsg("Invalid mask number " + std::to_string(mask_num));

        break;
      }
      case OBJECT_COVER_MASK_CMD: {
        auto mask_num = words.getInteger(1);

        if (mask_num >= 1 && mask_num <= int(textures_.size())) {
          CFile file(textures_[uint(mask_num - 1)]);

          CImageFileSrc src(file);

          auto image = CImageMgrInst->createImage(src);

          object->mapMask(image);
        }
        else
          errorMsg("Invalid mask number " + std::to_string(mask_num));

        break;
      }
      case OBJECT_TRANSFORMS_CMD: {
        CMatrix3D matrix = readTransforms();

        object->transform(matrix);

        break;
      }
      case OBJECT_END_CMD:
        end_command = true;

        break;
      default:
        errorMsg("Unrecognised Object Command '" + words.getWord(0) + "'");
        break;
    }
  }

  scene_->addPrimitive(object);
}

void
CImportScene::
readFaces(CGeomObject3D *object, int pface_num)
{
  std::string line;

  bool end_command = false;

  while (! end_command && file_->readLine(line)) {
    if (isSkipLine(line))
      continue;

    auto words = LineWords(line);

    int command_num = lookupCommand(words.getWord(0), faces_commands);

    switch (command_num) {
      case FACES_END_CMD:
        end_command = true;

        break;
      default: {
        std::vector<int> points;

        int i = 0;

        while (i < int(words.size())) {
          if (words.getWord(i) == ":")
            break;

          auto point_num = words.getInteger(i);

          points.push_back(int(point_num));

          i++;
        }

        auto num_points = points.size();

        std::vector<uint> face_points;

        if (orientation_ == 1) {
          for (size_t j = 0; j < num_points; ++j) {
            int point_num = points[j] - 1;

            face_points.push_back(uint(point_num));
          }
        }
        else {
          for (int j = int(num_points) - 1; j >= 0; --j) {
            int point_num = points[uint(j)] - 1;

            face_points.push_back(uint(point_num));
          }
        }

        uint face_num;

        if (pface_num != -1)
          face_num = object->addFaceSubFace(uint(pface_num), face_points);
        else
          face_num = object->addFace(face_points);

        if (i < int(words.size()) && words.getWord(i) == ":") {
          i++;

          auto face_color = words.getInteger(i);

          if (face_color >= 0 && face_color < int(colors_.size())) {
            CRGBA rgba = CRGBName::toRGBA(colors_[uint(face_color)]);

            if (pface_num != -1)
              object->setSubFaceColor(uint(pface_num), face_num, rgba);
            else
              object->setFaceColor(face_num, rgba);
          }
        }

        break;
      }
    }
  }
}

void
CImportScene::
readLines(CGeomObject3D *object, int pface_num)
{
  std::string line;

  bool end_command = false;

  while (! end_command && file_->readLine(line)) {
    if (isSkipLine(line))
      continue;

    auto words = LineWords(line);

    int command_num = lookupCommand(words.getWord(0), lines_commands);

    switch (command_num) {
      case LINES_END_CMD:
        end_command = true;

        break;
      default: {
        auto start = words.getInteger(0);
        auto end   = words.getInteger(1);

        if (pface_num != -1)
          object->addFaceSubLine(uint(pface_num), uint(start), uint(end));
        else
          object->addLine(uint(start), uint(end));

        break;
      }
    }
  }
}

void
CImportScene::
readVertices(CGeomObject3D *object)
{
  std::string line;

  bool end_command = false;

  while (! end_command && file_->readLine(line)) {
    if (isSkipLine(line))
      continue;

    auto words = LineWords(line);

    int command_num = lookupCommand(words.getWord(0), points_commands);

    switch (command_num) {
      case POINTS_END_CMD:
        end_command = true;

        break;
      default: {
        auto x = words.getReal(0);
        auto y = words.getReal(1);
        auto z = words.getReal(2);

        object->addVertex(CPoint3D(x, y, z));

        break;
      }
    }
  }
}

void
CImportScene::
readRotate(CGeomObject3D *object, int num_patches)
{
  std::string line;

  std::vector<CPoint2D> points;

  bool end_command = false;

  while (! end_command && file_->readLine(line)) {
    if (isSkipLine(line))
      continue;

    auto words = LineWords(line);

    int command_num = lookupCommand(words.getWord(0), rotate_commands);

    switch (command_num) {
      case ROTATE_END_CMD:
        end_command = true;

        break;
      default: {
        auto x = words.getReal(0);
        auto y = words.getReal(1);

        points.push_back(CPoint2D(x, y));

        break;
      }
    }
  }

  auto num_xy = points.size();

  if (num_xy < 2)
    return;

  auto *x = new double [num_xy];
  auto *y = new double [num_xy];

  for (size_t i = 0; i < num_xy; i++) {
    x[i] = points[i].x;
    y[i] = points[i].y;
  }

  object->addBodyRev(x, y, uint(num_xy), uint(num_patches));

  delete [] x;
  delete [] y;
}

void
CImportScene::
readColors()
{
  std::string line;

  bool end_command = false;

  while (! end_command && file_->readLine(line)) {
    if (isSkipLine(line))
      continue;

    auto words = LineWords(line);

    int command_num = lookupCommand(words.getWord(0), colors_commands);

    switch (command_num) {
      case COLORS_END_CMD:
        end_command = true;

        break;
      default:
        colors_.push_back(words.getWord(0));

        break;
    }
  }
}

void
CImportScene::
readTextures()
{
  std::string line;

  bool end_command = false;

  while (! end_command && file_->readLine(line)) {
    if (isSkipLine(line))
      continue;

    auto words = LineWords(line);

    int command_num = lookupCommand(words.getWord(0), colors_commands);

    switch (command_num) {
      case COLORS_END_CMD:
        end_command = true;

        break;
      default:
        textures_.push_back(words.getWord(0));

        break;
    }
  }
}

//----

class CCSGPolygon : public CSG::Polygon {
 public:
  CCSGPolygon(const std::vector<CSG::Vertex *> &v, const Polygon *p) :
   Polygon(v, p) {
    if (p) c_ = static_cast<const CCSGPolygon *>(p)->getColor();
  }

  const CRGBA &getColor() const { return c_; }
  void setColor(const CRGBA &c) { c_ = c; }

 private:
  CRGBA c_ { 1.0, 1.0, 1.0 };
};

class CCSGNode : public CSG::Node {
 public:
  CCSGNode() : Node() { }

  void setColor(const CRGBA &c) {
    int np = polygons.size();

    for (int i = 0; i < np; ++i)
      static_cast<CCSGPolygon *>(polygons[i])->setColor(c);

    if (back ) static_cast<CCSGNode *>(back )->setColor(c);
    if (front) static_cast<CCSGNode *>(front)->setColor(c);
  }
};

class CCSG : public CSG::CSG {
 public:
  CCSG() : CSG() { }

  void setColor(const CRGBA &c) {
    static_cast<CCSGNode *>(root())->setColor(c);
  }
};

class CCSGFactory : public CSG::Factory {
 public:
  CCSGFactory() { }

  CSG::CSG *createCSG() override {
    return new CCSG;
  }

  CSG::Node *createNode() override {
    return new CCSGNode;
  }

  CSG::Vertex *createVertex(const CSG::Vector &p, const CSG::Vector &n) override {
    return new CSG::Vertex(p, n);
  }

  CSG::Polygon *createPolygon(const std::vector<CSG::Vertex *> &v, const CSG::Polygon *p) override {
    auto *p1 = new CCSGPolygon(v, p);

    if (p)
      p1->setColor(static_cast<const CCSGPolygon *>(p)->getColor());

    return p1;
  }
};

void
CImportScene::
readCSG(const std::string &name)
{
  auto *object = CGeometryInst->createObject3D(scene_, name);

  //---

  CRGBA color;

  struct Shape {
    CSG::CSG*   csg { nullptr };
    std::string op;
  };

  std::vector<Shape> shapes;

  //---

  CSGMgrInst->setFactory(new CCSGFactory);

  std::string line;

  bool end_command = false;

  while (! end_command && file_->readLine(line)) {
    if (isSkipLine(line))
      continue;

    auto words = LineWords(line);

    int command_num = lookupCommand(words.getWord(0), csg_commands);

    switch (command_num) {
      case CSG_CUBE_CMD: {
        // center and radius
        auto x = words.getReal(2);
        auto y = words.getReal(3);
        auto z = words.getReal(4);
        auto r = words.getReal(5);

        Shape shape;

        shape.csg = CSG::cube(CSG::Vector(x, y, z), r);
        shape.op  = words.getWord(1);

        static_cast<CCSG *>(shape.csg)->setColor(color);

        shapes.push_back(shape);

        break;
      }
      case CSG_SPHERE_CMD: {
        // center and radius
        auto x = words.getReal(2);
        auto y = words.getReal(3);
        auto z = words.getReal(4);
        auto r = words.getReal(5);

        Shape shape;

        shape.csg = CSG::sphere(CSG::Vector(x, y, z), r);
        shape.op  = words.getWord(1);

        static_cast<CCSG *>(shape.csg)->setColor(color);

        shapes.push_back(shape);

        break;
      }
      case CSG_CYLINDER_CMD: {
        // start, end and radius
        auto x1 = words.getReal(2);
        auto y1 = words.getReal(3);
        auto z1 = words.getReal(4);
        auto x2 = words.getReal(5);
        auto y2 = words.getReal(6);
        auto z2 = words.getReal(7);
        auto r  = words.getReal(8);

        Shape shape;

        shape.csg = CSG::cylinder(CSG::Vector(x1, y1, z1), CSG::Vector(x2, y2, z2), r);
        shape.op  = words.getWord(1);

        static_cast<CCSG *>(shape.csg)->setColor(color);

        shapes.push_back(shape);

        break;
      }
      case CSG_COLOR_CMD: {
        color = wordToColor(words.getWord(1));

        break;
      }
      case CSG_END_CMD:
        end_command = true;

        break;
      default:
        errorMsg("Unrecognised CSG Command '" + words.getWord(0) + "'");
        break;
    }
  }

  //---

  CSG::CSG *csg { nullptr };

  for (const auto &shape : shapes) {
    if (! csg)
      csg = shape.csg;
    else {
      auto op = shape.op;

      bool negate = false;

      if (op[0] == '!') {
        op     = op.substr(1);
        negate = true;
      }

      if (negate)
        shape.csg->inverseOp();

      if      (op == "&")
        csg = csg->unionOp(shape.csg);
      else if (op == "|")
        csg = csg->intersectOp(shape.csg);
      else if (op == "^")
        csg = csg->subtractOp(shape.csg);
    }
  }

  //---

  auto polygons = csg->toPolygons();

  for (auto *poly : polygons) {
    std::vector<uint> inds;

    auto *poly1 = static_cast<CCSGPolygon *>(poly);

    for (auto *v : poly->vertices) {
      auto ind = object->addVertex(CPoint3D(v->pos.x, v->pos.y, v->pos.z));

      object->setVertexNormal(ind, CVector3D(v->normal.x, v->normal.y, v->normal.z));
      object->setVertexColor (ind, poly1->getColor());

      inds.push_back(ind);
    }

    object->addFace(inds);
  }

  scene_->addPrimitive(object);

  //---

  CSGMgrInst->setFactory(nullptr);
}

CMatrix3D
CImportScene::
readTransforms()
{
  CMatrix3D transform_matrix;

  transform_matrix.setIdentity();

  std::string line;

  bool end_command = false;

  while (! end_command && file_->readLine(line)) {
    if (isSkipLine(line))
      continue;

    auto words = LineWords(line);

    int command_num = lookupCommand(words.getWord(0), transforms_commands);

    switch (command_num) {
      case TRANSFORMS_TRANSLATE_CMD: {
        auto x = words.getReal(1);
        auto y = words.getReal(2);
        auto z = words.getReal(3);

        CMatrix3D matrix1;

        matrix1.setTranslation(x, y, z);

        transform_matrix = matrix1*transform_matrix;

        break;
      }
      case TRANSFORMS_SCALE_CMD: {
        auto x = words.getReal(1);
        auto y = words.getReal(2);
        auto z = words.getReal(3);

        CMatrix3D matrix1;

        matrix1.setScale(x, y, z);

        transform_matrix = matrix1*transform_matrix;

        break;
      }
      case TRANSFORMS_ROTATE_X_CMD: {
        auto angle = words.getReal(1);

        CMatrix3D matrix1;

        matrix1.setRotation(CMathGen::X_AXIS_3D, CMathGen::DegToRad(angle));

        transform_matrix = matrix1*transform_matrix;

        break;
      }
      case TRANSFORMS_ROTATE_Y_CMD: {
        auto angle = words.getReal(1);

        CMatrix3D matrix1;

        matrix1.setRotation(CMathGen::Y_AXIS_3D, CMathGen::DegToRad(angle));

        transform_matrix = matrix1*transform_matrix;

        break;
      }
      case TRANSFORMS_ROTATE_Z_CMD: {
        auto angle = words.getReal(1);

        CMatrix3D matrix1;

        matrix1.setRotation(CMathGen::Z_AXIS_3D, CMathGen::DegToRad(angle));

        transform_matrix = matrix1*transform_matrix;

        break;
      }
      case TRANSFORMS_END_CMD:
        end_command = true;

        break;
      default:
        errorMsg("Unrecognised Transforms Command '" + words.getWord(0) + "'");
        break;
    }
  }

  return transform_matrix;
}

CGeomObject3D *
CImportScene::
getObject(const std::string &name)
{
  return scene_->getObjectP(name);
}

CGeomObject3D *
CImportScene::
getPrimitive(const std::string &name)
{
  return scene_->getPrimitiveP(name);
}

int
CImportScene::
lookupCommand(const std::string &command, const char **commands)
{
  for (int i = 0; commands[i] != nullptr; i++)
    if (strcmp(commands[i], command.c_str()) == 0)
      return i + 1;

  return 0;
}

CGeomObject3D &
CImportScene::
getObject()
{
  return *scene_->getObjects().front();
}

bool
CImportScene::
isSkipLine(const std::string &line) const
{
  int i = 0;

  while (i < int(line.size()) && isspace(line[i]))
    ++i;

  if (i >= int(line.size()))
    return true;

  if (line[i] == ';' || line[i] == '#')
    return true;

  return false;
}

CRGBA
CImportScene::
wordToColor(const std::string &word) const
{
  CRGBA rgba;

  if (CStrUtil::isInteger(word)) {
    auto color_num = CStrUtil::toInteger(word);

    rgba = getRGBA(int(color_num));
  }
  else
    rgba = CRGBName::toRGBA(word);

  return rgba;
}

CRGBA
CImportScene::
getRGBA(int color) const
{
  auto num_colors = colors_.size();

  if (color >= 1 && color <= int(num_colors))
    return CRGBName::toRGBA(colors_[uint(color - 1)]);
  else
    return CRGBA(1.0, 0.0, 0.0, 1.0);
}

void
CImportScene::
errorMsg(const std::string &msg) const
{
  std::cerr << "Error: " << msg << "\n";
}
