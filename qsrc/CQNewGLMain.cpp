#include <CQNewGLModel.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLModelObject.h>

#include <CImportBase.h>
#include <CGeomScene3D.h>
#include <CGeometry3D.h>
#include <CImageLib.h>
#include <CStrUtil.h>

#include <CQApp.h>

int
main(int argc, char **argv)
{
  CQApp app(argc, argv);

  //---

  QString     modelName;
  CGeom3DType format = CGEOM_3D_TYPE_V3D;

  using InitTextureData = CQNewGLCanvas::InitTextureData;

  std::vector<InitTextureData> initTextures;
  std::string                  textureMap;
  std::string                  fileNameMap;
  std::vector<std::string>     args;

  double  time = 0.0;
  QString animName;

  bool swapYZ      = false;
  bool invertDepth = false;
  bool invertX     = false;
  bool invertY     = false;
  bool invertZ     = false;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      auto arg = QString(&argv[i][1]);

      if      (arg == "3ds"   || arg == "3drw" || arg == "asc" || arg == "cob" || arg == "dxf" ||
               arg == "fbx"   || arg == "gltf" || arg == "obj" || arg == "plg" || arg == "ply" ||
               arg == "scene" || arg == "stl"  || arg == "v3d" || arg == "vox" || arg == "x3d") {
        ++i;

        if (i < argc) {
          format    = CImportBase::suffixToType(arg.toStdString());
          modelName = QString(argv[i]);
        }
        else
          std::cerr << "Missing filename for " << argv[i] << "\n";
      }
      else if (arg == "swap_yz") {
        swapYZ = true;
      }
      else if (arg == "invert_depth") {
        invertDepth = true;
      }
      else if (arg == "invert_x") {
        invertX = true;
      }
      else if (arg == "invert_y") {
        invertY = true;
      }
      else if (arg == "invert_z") {
        invertZ = true;
      }
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
          textureMap = argv[i];
        else
          std::cerr << "Missing value for " << argv[i] << "\n";
      }
      else if (arg == "filename_map") {
        ++i;

        if (i < argc)
          fileNameMap = argv[i];
        else
          std::cerr << "Missing value for " << argv[i] << "\n";
      }
      else if (arg == "h" || arg == "help") {
        std::cout << "CGLNewModel [-swap_yz] [-invert_depth] [[-3ds|-3drw|-asc|-cob|-dxf|-fbx|"
                     "-gltf|-obj|-plg|-ply|-scene|-stl|-v3d|-vox|-x3d] <filename>]\n";
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
      auto suffix2 = qstr.right(suffix1.length()).toLower();

      if (suffix2 == suffix1) {
        format    = CImportBase::suffixToType(suffix.toStdString());
        modelName = qstr;
      }
    };

    auto strs = QStringList() <<
      "3ds" << "3drw" << "asc" << "cob" << "dxf" << "fbx" << "glb" << "gltf" <<
      "obj" << "plg" << "ply"<< "scene"<< "stl"<< "v3d" << "vox" << "x3d";

    for (const auto &str : strs)
      isSuffix(str);
  }

  //---

  auto *modelApp = new CQNewGLModel;

#if 0
  if (modelName == "") {
    modelName = modelApp->buildDir() + "/models/v3d/Lightplane.V3D";
    //std::cerr << "Missing model name\n";
    //exit(1);
  }
#endif

  //---

  auto *canvas = modelApp->canvas();

  modelApp->setTime    (time);
  modelApp->setAnimName(animName);

  CQNewGLModel::LoadData loadData;

  loadData.invertX     = invertX;
  loadData.invertY     = invertY;
  loadData.invertZ     = invertZ;
  loadData.swapYZ      = swapYZ;
  loadData.fileNameMap = fileNameMap;

  if (modelName != "") {
    if (! modelApp->loadModel(modelName, format, loadData))
      std::cerr << "Failed to load model '" << modelName.toStdString() << "'\n";

    for (auto *object : loadData.objects) {
      auto *objectData = canvas->getObjectData(object);

      objectData->setInvertDepth(invertDepth);
    }
  }

  modelApp->resize(canvas->windowWidth(), canvas->windowHeight());

  canvas->setInitTextures(initTextures);

  canvas->setTextureMap(textureMap);

  //---

  modelApp->show();

  modelApp->update();

  app.exec();
}
