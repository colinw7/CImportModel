#include <CQNewGLModel.h>
#include <CQNewGLCanvas.h>

#include <CImportBase.h>
#include <CGeomScene3D.h>
#include <CGeometry3D.h>
#include <CImageLib.h>

#include <CQApp.h>

int
main(int argc, char **argv)
{
  CQApp app(argc, argv);

  //---

  QString     modelName = "/home/colinw/dev/models/v3d/Lightplane.V3D";
  CGeom3DType format    = CGEOM_3D_TYPE_V3D;

  using InitTextureData = CQNewGLCanvas::InitTextureData;

  std::vector<InitTextureData> initTextures;
  std::string                  texture_map;
  std::vector<std::string>     args;

  double  time = 0.0;
  QString animName;

  bool flipYZ      = false;
  bool invertDepth = false;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      auto arg = QString(&argv[i][1]);

      if      (arg == "3ds"   || arg == "3drw" || arg == "asc" || arg == "cob" || arg == "dxf" ||
               arg == "fbx"   || arg == "gltf" || arg == "obj" || arg == "plg" || arg == "ply" ||
               arg == "scene" || arg == "stl"  || arg == "v3d" || arg == "x3d") {
        ++i;

        if (i < argc) {
          format    = CImportBase::suffixToType(arg.toStdString());
          modelName = QString(argv[i]);
        }
        else
          std::cerr << "Missing filename for " << argv[i] << "\n";
      }
      else if (arg == "flip_yz")
        flipYZ = true;
      else if (arg == "invert_depth")
        invertDepth = true;
      else if (arg == "time") {
        ++i;

        if (i < argc)
          time = std::atof(argv[i]);
        else
          std::cerr << "Missing value for " << argv[i] << "\n";
      }
      else if (arg == "animation") {
        ++i;

        if (i < argc)
          animName = argv[i];
        else
          std::cerr << "Missing value for " << argv[i] << "\n";
      }
      else if (arg == "texture" || arg == "texture_flipv") {
        ++i;

        if (i < argc) {
          InitTextureData textureData;

          textureData.name  = argv[i];
          textureData.flipV = (arg == "texture_flipv");

          initTextures.push_back(textureData);
        }
        else
          std::cerr << "Missing value for " << argv[i] << "\n";
      }
      else if (arg == "texture_map") {
        ++i;

        if (i < argc)
          texture_map = argv[i];
        else
          std::cerr << "Missing value for " << argv[i] << "\n";
      }
      else if (arg == "h" || arg == "help") {
        std::cout << "CGLNewModel [-flip_yz] [-invert_depth] [[-3ds|-3drw|-asc|-cob|-dxf|-fbx|"
                     "-gltf|-obj|-plg|-ply|-scene|-stl|-v3d|-x3d] <filename>]\n";
        return 0;
      }
      else {
        std::cerr << "Invalid option " << argv[i] << "\n";
      }
    }
    else {
      args.push_back(argv[i]);
    }
  }

  for (const auto &arg : args) {
    auto qstr = QString::fromStdString(arg);

    auto isSuffix = [&](const QString &suffix) {
      auto suffix1 = "." + suffix;

      if (qstr.right(suffix1.length()) == suffix1) {
        format    = CImportBase::suffixToType(suffix.toStdString());
        modelName = qstr;
      }
    };

    auto strs = QStringList() <<
      "3ds" << "3drw" << "asc" << "cob" << "dxf" << "fbx" << "gltf" <<
      "obj" << "plg" << "ply"<< "scene"<< "stl"<< "v3d" << "x3d";

    for (const auto &str : strs)
      isSuffix(str);
  }

  if (modelName == "") {
    std::cerr << "Missing model name\n";
    exit(1);
  }

  //---

  auto *modelApp = new CQNewGLModel;

  auto *canvas = modelApp->canvas();

  canvas->setFlipYZ     (flipYZ);
  canvas->setInvertDepth(invertDepth);
  canvas->setTime       (time);
  canvas->setAnimName   (animName);

  if (! canvas->loadModel(modelName, format))
    exit(1);

  modelApp->resize(modelApp->windowWidth(), modelApp->windowHeight());

  canvas->setInitTextures(initTextures);

  canvas->setTextureMap(texture_map);

  //---

  modelApp->show();

  modelApp->update();

  app.exec();
}
