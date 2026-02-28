#include <Main.h>
#include <App.h>

#include <CQImage.h>
#include <CImportBase.h>
#include <CGeom3DType.h>

#include <QApplication>

int
main(int argc, char **argv)
{
  QApplication qapp(argc, argv);

  CQImage::setPrototype();

  //---

  QString     modelName;
  CGeom3DType format { CGEOM_3D_TYPE_V3D };

  std::vector<std::string> args;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      auto arg = QString(&argv[i][1]);

      if      (arg == "3ds"   || arg == "3drw" || arg == "asc" || arg == "blend" ||
               arg == "cob"   || arg == "dae"  || arg == "dxf" || arg == "fbx"   ||
               arg == "gltf"  || arg == "obj"  || arg == "plg" || arg == "ply"   ||
               arg == "scene" || arg == "stl"  || arg == "v3d" || arg == "vox"   ||
               arg == "x3d") {
        ++i;

        if (i < argc) {
          format    = CImportBase::suffixToType(arg.toStdString());
          modelName = QString(argv[i]);
        }
        else
          std::cerr << "Missing filename for " << argv[i - 1] << "\n";
      }
      else if (arg == "h" || arg == "help") {
        std::cerr << "CQModel3DView "
         "[-3ds|-3drw|-asc|-blend|-cob|-dae|-dxf|-fbx|-gltf|"
          "-obj|-plg|-ply|-scene|-stl|-v3d|-vox|-x3d] "
         "\n";
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
      "3ds" << "3drw" << "asc" << "blend" << "cob" << "dae" << "dxf" << "fbx" << "glb" << "gltf" <<
      "obj" << "plg" << "ply"<< "scene"<< "stl"<< "v3d" << "vox" << "x3d";

    for (const auto &str : strs)
      isSuffix(str);
  }

  //---

  auto *app = new CQModel3DView::App;

  if (modelName != "") {
    if (! app->loadModel(modelName, format))
      std::cerr << "Failed to load model '" << modelName.toStdString() << "'\n";
  }

  //---

  app->show();

  //---

  qapp.exec();
}
