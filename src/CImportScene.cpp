#include <CImportScene.h>
#include <CGeomScene3D.h>
#include <CGeomObject3D.h>
#include <CGeomSphere3D.h>
#include <CGeomBox3D.h>
#include <CGeometry3D.h>
#include <CStrUtil.h>
#include <CMathGen.h>
#include <CImageMgr.h>
#include <CImage.h>
#include <CRGBName.h>

#define ORIENTATION_CMD 1
#define SCENE_CMD       2
#define OBJECT_CMD      3
#define PRIMITIVE_CMD   4
#define COLORS_CMD      5
#define COLOURS_CMD     6
#define TEXTURES_CMD    7

static const char *
commands[] = {
  "Orientation",
  "Scene",
  "Object",
  "Primitive",
  "Colors",
  "Colours",
  "Textures",
  NULL,
};

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
  NULL,
};

#define PRIMITIVE_FACES_CMD     1
#define PRIMITIVE_SUB_FACES_CMD 2
#define PRIMITIVE_LINES_CMD     3
#define PRIMITIVE_SUB_LINES_CMD 4
#define PRIMITIVE_POINTS_CMD    5
#define PRIMITIVE_ROTATE_CMD    6
#define PRIMITIVE_END_CMD       7

static const char *
primitive_commands[] = {
  "Faces",
  "SubFaces",
  "Lines",
  "SubLines",
  "Points",
  "Rotate",
  "End",
  NULL,
};

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
  NULL,
};

#define FACES_END_CMD 1

static const char *
faces_commands[] = {
  "End",
  NULL,
};

#define LINES_END_CMD 1

static const char *
lines_commands[] = {
  "End",
  NULL,
};

#define POINTS_END_CMD 1

static const char *
points_commands[] = {
  "End",
  NULL,
};

#define ROTATE_END_CMD 1

static const char *
rotate_commands[] = {
  "End",
  NULL,
};

#define COLORS_END_CMD 1

static const char *
colors_commands[] = {
  "End",
  NULL,
};

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
  NULL,
};

CImportScene::
CImportScene(CGeomScene3D *scene, const std::string &) :
 scene_(scene), orientation_(1)
{
  if (scene_ == NULL) {
    scene_  = CGeometryInst->createScene3D();
    pscene_ = scene_;
  }
}

bool
CImportScene::
read(CFile &file)
{
  file_ = &file;

  std::string line;

  bool end_command = false;

  while (! end_command && file_->readLine(line)) {
    if (line == "" || line[0] == ';' || line[0] == '#')
      continue;

    CStrWords words = CStrUtil::toWords(line, NULL);

    int command_num = lookupCommand(words[0].getWord(), commands);

    switch (command_num) {
      case ORIENTATION_CMD:
        orientation_ = CStrUtil::toInteger(words[1].getWord());

        break;
      case SCENE_CMD:
        readScene();

        break;
      case OBJECT_CMD:
        readObject(words[1].getWord());

        break;
      case PRIMITIVE_CMD:
        readPrimitive(words[1].getWord());

        break;
      case COLORS_CMD:
      case COLOURS_CMD:
        readColors();

        break;
      case TEXTURES_CMD:
        readTextures();

        break;
      default:
        std::cerr << "Unrecognised Command " << words[0].getWord() << std::endl;
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
    if (line == "" || line[0] == ';' || line[0] == '#')
      continue;

    CStrWords words = CStrUtil::toWords(line, NULL);

    int command_num = lookupCommand(words[0].getWord(), scene_commands);

    switch (command_num) {
      case SCENE_OBJECT_CMD: {
        CGeomObject3D *object = getPrimitive(words[1].getWord());

        if (object == NULL) {
          std::cerr << "Unrecognised Object " << words[1].getWord() << std::endl;
          return;
        }

        addObject(words[1].getWord());

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
        std::cerr << "Unrecognised Scene Command " << words[0].getWord() << std::endl;
        break;
    }
  }
}

void
CImportScene::
addObject(const std::string &name)
{
  CGeomObject3D *primitive = getPrimitive(name);

  if (primitive == NULL) {
    if      (name == "Sphere") {
      primitive = CGeometryInst->createObject3D(scene_, "sphere");

      CGeomSphere3D::addGeometry(primitive, CPoint3D(0, 0, 0), 1);
    }
    else if (name == "Cube") {
      primitive = CGeometryInst->createObject3D(scene_, "sphere");

      CGeomBox3D::addGeometry(primitive, 0, 0, 0, 1, 1, 1);
    }
  }

  if (primitive == NULL) {
    std::cerr << "Unrecognised Primitive " << name << std::endl;
    return;
  }

  CGeomObject3D *object1 = primitive->dup();

  object1->setScene(scene_);

  scene_->addObject(object1);
}

void
CImportScene::
readPrimitive(const std::string &name)
{
  CGeomObject3D *primitive = CGeometryInst->createObject3D(scene_, name);

  primitive->setName(name);

  std::string line;

  bool end_command = false;

  while (! end_command && file_->readLine(line)) {
    if (line == "" || line[0] == ';' || line[0] == '#')
      continue;

    CStrWords words = CStrUtil::toWords(line, NULL);

    int command_num = lookupCommand(words[0].getWord(), primitive_commands);

    switch (command_num) {
      case PRIMITIVE_FACES_CMD: {
        readFaces(primitive, -1);

        break;
      }
      case PRIMITIVE_SUB_FACES_CMD: {
        int face_num = CStrUtil::toInteger(words[1].getWord());

        if (face_num <= 0 || face_num > (int) primitive->getNumFaces()) {
          std::cout << "SubFace Face " << face_num << " Not Found" << std::endl;
          break;
        }

        readFaces(primitive, face_num - 1);

        break;
      }
      case PRIMITIVE_LINES_CMD: {
        readLines(primitive, -1);

        break;
      }
      case PRIMITIVE_SUB_LINES_CMD: {
        int face_num = CStrUtil::toInteger(words[1].getWord());

        if (face_num <= 0 || face_num > (int) primitive->getNumFaces()) {
          std::cout << "SubLine Face " << face_num << " Not Found" << std::endl;
          break;
        }

        readLines(primitive, face_num - 1);

        break;
      }
      case PRIMITIVE_POINTS_CMD: {
        readVertices(primitive);

        break;
      }
      case PRIMITIVE_ROTATE_CMD: {
        int num_patches = CStrUtil::toInteger(words[1].getWord());

        readRotate(primitive, num_patches);

        break;
      }
      case PRIMITIVE_END_CMD:
        end_command = true;

        break;
      default:
        std::cerr << "Unrecognised Primitive Command " << words[0].getWord() << std::endl;
        break;
    }
  }

  scene_->addPrimitive(primitive);
}

void
CImportScene::
readObject(const std::string &name)
{
  CGeomObject3D *object = CGeometryInst->createObject3D(scene_, name);

  object->setName(name);

  std::string line;

  bool end_command = false;

  while (! end_command && file_->readLine(line)) {
    if (line == "" || line[0] == ';' || line[0] == '#')
      continue;

    CStrWords words = CStrUtil::toWords(line, NULL);

    int command_num = lookupCommand(words[0].getWord(), object_commands);

    switch (command_num) {
      case OBJECT_PRIMITIVE_CMD: {
        std::string name1 = words[1].getWord();

        CGeomObject3D *primitive = getPrimitive(name1);

        if (primitive == NULL) {
          if      (name1 == "Sphere") {
            primitive = CGeometryInst->createObject3D(scene_, name);

            CGeomSphere3D::addGeometry(primitive, CPoint3D(0, 0, 0), 1);
          }
          else if (name1 == "Cube") {
            primitive = CGeometryInst->createObject3D(scene_, name);

            CGeomBox3D::addGeometry(primitive, 0, 0, 0, 1, 1, 1);
          }
        }

        if (primitive == NULL) {
          std::cerr << "Unrecognised Primitive " << name1 << std::endl;
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
        CRGBA rgba;

        if (CStrUtil::isInteger(words[1].getWord())) {
          int color_num = CStrUtil::toInteger(words[1].getWord());

          if (color_num >= 0 && color_num <= (int) colors_.size())
            rgba = CRGBName::toRGBA(colors_[color_num]);
          else
            rgba = CRGBA(0,0,0,0);
        }
        else
          rgba = CRGBName::toRGBA(words[1].getWord());

        object->setFaceColor(rgba);

        break;
      }
      case OBJECT_SUB_FACE_COLOR_CMD:
      case OBJECT_SUB_FACE_COLOUR_CMD: {
        int color_num = CStrUtil::toInteger(words[1].getWord());

        CRGBA rgba = CRGBName::toRGBA(colors_[color_num]);

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
        int texture_num = CStrUtil::toInteger(words[1].getWord());

        if (texture_num >= 1 && texture_num <= (int) textures_.size()) {
          CFile file(textures_[texture_num - 1]);

          CImageFileSrc src(file);

          CImagePtr image = CImageMgrInst->createImage(src);

          object->setTexture(image);
        }
        else
          std::cerr << "Invalid texture number " << texture_num << std::endl;

        break;
      }
      case OBJECT_COVER_TEXTURE_CMD: {
        int texture_num = CStrUtil::toInteger(words[1].getWord());

        if (texture_num >= 1 && texture_num <= (int) textures_.size()) {
          CFile file(textures_[texture_num - 1]);

          CImageFileSrc src(file);

          CImagePtr image = CImageMgrInst->createImage(src);

          object->mapTexture(image);
        }
        else
          std::cerr << "Invalid texture number " << texture_num << std::endl;

        break;
      }
      case OBJECT_MASK_CMD: {
        int mask_num = CStrUtil::toInteger(words[1].getWord());

        if (mask_num >= 1 && mask_num <= (int) textures_.size()) {
          CFile file(textures_[mask_num - 1]);

          CImageFileSrc src(file);

          CImagePtr image = CImageMgrInst->createImage(src);

          object->setMask(image);
        }
        else
          std::cerr << "Invalid mask number " << mask_num << std::endl;

        break;
      }
      case OBJECT_COVER_MASK_CMD: {
        int mask_num = CStrUtil::toInteger(words[1].getWord());

        if (mask_num >= 1 && mask_num <= (int) textures_.size()) {
          CFile file(textures_[mask_num - 1]);

          CImageFileSrc src(file);

          CImagePtr image = CImageMgrInst->createImage(src);

          object->mapMask(image);
        }
        else
          std::cerr << "Invalid mask number " << mask_num << std::endl;

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
        std::cerr << "Unrecognised Object Command " << words[0].getWord() << std::endl;
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
    if (line == "" || line[0] == ';' || line[0] == '#')
      continue;

    CStrWords words = CStrUtil::toWords(line, NULL);

    int command_num = lookupCommand(words[0].getWord(), faces_commands);

    switch (command_num) {
      case FACES_END_CMD:
        end_command = true;

        break;
      default: {
        std::vector<int> points;

        int i = 0;

        while (i < words.size()) {
          if (words[i].getWord() == ":")
            break;

          int point_num = CStrUtil::toInteger(words[i].getWord());

          points.push_back(point_num);

          i++;
        }

        int num_points = points.size();

        std::vector<uint> face_points;

        if (orientation_ == 1) {
          for (int j = 0; j < num_points; ++j) {
            int point_num = points[j] - 1;

            face_points.push_back(point_num);
          }
        }
        else {
          for (int j = num_points - 1; j >= 0; --j) {
            int point_num = points[j] - 1;

            face_points.push_back(point_num);
          }
        }

        uint face_num;

        if (pface_num != -1)
          face_num = object->addFaceSubFace(pface_num, face_points);
        else
          face_num = object->addFace(face_points);

        if (i < words.size() && words[i].getWord() == ":") {
          i++;

          int face_color = CStrUtil::toInteger(words[i].getWord());

          if (face_color >= 0 && face_color < (int) colors_.size()) {
            CRGBA rgba = CRGBName::toRGBA(colors_[face_color]);

            if (pface_num != -1)
              object->setSubFaceColor(pface_num, face_num, rgba);
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
    if (line == "" || line[0] == ';' || line[0] == '#')
      continue;

    CStrWords words = CStrUtil::toWords(line, NULL);

    int command_num = lookupCommand(words[0].getWord(), lines_commands);

    switch (command_num) {
      case LINES_END_CMD:
        end_command = true;

        break;
      default: {
        int start = CStrUtil::toInteger(words[0].getWord());
        int end   = CStrUtil::toInteger(words[1].getWord());

        if (pface_num != -1)
          object->addFaceSubLine(pface_num, start, end);
        else
          object->addLine(start, end);

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
    if (line == "" || line[0] == ';' || line[0] == '#')
      continue;

    CStrWords words = CStrUtil::toWords(line, NULL);

    int command_num = lookupCommand(words[0].getWord(), points_commands);

    switch (command_num) {
      case POINTS_END_CMD:
        end_command = true;

        break;
      default: {
        double x = CStrUtil::toReal(words[0].getWord());
        double y = CStrUtil::toReal(words[1].getWord());
        double z = CStrUtil::toReal(words[2].getWord());

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
    if (line == "" || line[0] == ';' || line[0] == '#')
      continue;

    CStrWords words = CStrUtil::toWords(line, NULL);

    int command_num = lookupCommand(words[0].getWord(), rotate_commands);

    switch (command_num) {
      case ROTATE_END_CMD:
        end_command = true;

        break;
      default: {
        double x = CStrUtil::toReal(words[0].getWord());
        double y = CStrUtil::toReal(words[1].getWord());

        points.push_back(CPoint2D(x, y));

        break;
      }
    }
  }

  int num_xy = points.size();

  if (num_xy < 2)
    return;

  double *x = new double [num_xy];
  double *y = new double [num_xy];

  for (int i = 0; i < num_xy; i++) {
    x[i] = points[i].x;
    y[i] = points[i].y;
  }

  object->addBodyRev(x, y, num_xy, num_patches);

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
    if (line == "" || line[0] == ';' || line[0] == '#')
      continue;

    CStrWords words = CStrUtil::toWords(line, NULL);

    int command_num = lookupCommand(words[0].getWord(), colors_commands);

    switch (command_num) {
      case COLORS_END_CMD:
        end_command = true;

        break;
      default:
        colors_.push_back(words[0].getWord());

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
    if (line == "" || line[0] == ';' || line[0] == '#')
      continue;

    CStrWords words = CStrUtil::toWords(line, NULL);

    int command_num = lookupCommand(words[0].getWord(), colors_commands);

    switch (command_num) {
      case COLORS_END_CMD:
        end_command = true;

        break;
      default:
        textures_.push_back(words[0].getWord());

        break;
    }
  }
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
    if (line == "" || line[0] == ';' || line[0] == '#')
      continue;

    CStrWords words = CStrUtil::toWords(line, NULL);

    int command_num = lookupCommand(words[0].getWord(), transforms_commands);

    switch (command_num) {
      case TRANSFORMS_TRANSLATE_CMD: {
        double x = CStrUtil::toReal(words[1].getWord());
        double y = CStrUtil::toReal(words[2].getWord());
        double z = CStrUtil::toReal(words[3].getWord());

        CMatrix3D matrix1;

        matrix1.setTranslation(x, y, z);

        transform_matrix = matrix1*transform_matrix;

        break;
      }
      case TRANSFORMS_SCALE_CMD: {
        double x = CStrUtil::toReal(words[1].getWord());
        double y = CStrUtil::toReal(words[2].getWord());
        double z = CStrUtil::toReal(words[3].getWord());

        CMatrix3D matrix1;

        matrix1.setScale(x, y, z);

        transform_matrix = matrix1*transform_matrix;

        break;
      }
      case TRANSFORMS_ROTATE_X_CMD: {
        double angle = CStrUtil::toReal(words[1].getWord());

        CMatrix3D matrix1;

        matrix1.setRotation(CMathGen::X_AXIS_3D, CMathGen::DegToRad(angle));

        transform_matrix = matrix1*transform_matrix;

        break;
      }
      case TRANSFORMS_ROTATE_Y_CMD: {
        double angle = CStrUtil::toReal(words[1].getWord());

        CMatrix3D matrix1;

        matrix1.setRotation(CMathGen::Y_AXIS_3D, CMathGen::DegToRad(angle));

        transform_matrix = matrix1*transform_matrix;

        break;
      }
      case TRANSFORMS_ROTATE_Z_CMD: {
        double angle = CStrUtil::toReal(words[1].getWord());

        CMatrix3D matrix1;

        matrix1.setRotation(CMathGen::Z_AXIS_3D, CMathGen::DegToRad(angle));

        transform_matrix = matrix1*transform_matrix;

        break;
      }
      case TRANSFORMS_END_CMD:
        end_command = true;

        break;
      default:
        std::cerr << "Unrecognised Transforms Command " << words[0].getWord() << std::endl;
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
  for (int i = 0; commands[i] != NULL; i++)
    if (strcmp(commands[i], command.c_str()) == 0)
      return i + 1;

  return 0;
}

void
CImportScene::
getRGBA(int color, CRGBA *rgba)
{
  int num_colors = colors_.size();

  if (color > 0 && color <= num_colors)
    *rgba = CRGBName::toRGBA(colors_[color - 1]);
  else
    *rgba = CRGBA(1, 0, 0, 1);
}

CGeomObject3D &
CImportScene::
getObject()
{
  return *scene_->getObjects().front();
}
