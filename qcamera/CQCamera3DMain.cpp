#include <CQCamera3DMain.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DCanvas.h>

#include <CQImage.h>
#include <CQMetaEdit.h>
#include <CImportBase.h>
#include <CGeomScene3D.h>
#include <CGeometry3D.h>

#include <QApplication>
#include <QKeyEvent>

class CQCamera3DEventFilter : public QObject {
 public:
  CQCamera3DEventFilter() { }

 protected:
  bool eventFilter(QObject *obj, QEvent *event) override {
    if (event->type() == QEvent::KeyPress) {
      auto *keyEvent = static_cast<QKeyEvent *>(event);

      if (keyEvent->modifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_Home) {
        showMetaEdit();
        return true;
      }

#if 0
      if (keyEvent->modifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_End) {
        CQApp::showPerfDialog();
        return true;
      }

      if (keyEvent->modifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_PageDown) {
        CQApp::showOptions();
        return true;
      }
#endif
    }

    // standard event processing
    return QObject::eventFilter(obj, event);
  }

  void showMetaEdit() {
    auto *metaEdit = new CQMetaEdit;

    metaEdit->show();

    metaEdit->raise();
  }
};

int
main(int argc, char **argv)
{
  QApplication qapp(argc, argv);

  CQImage::setPrototype();

  //---

  QString     modelName;
  CGeom3DType format  = CGEOM_3D_TYPE_V3D;
  bool        swapXY  = false;
  bool        swapYZ  = false;
  bool        swapZX  = false;
  bool        invertX = false;
  bool        invertY = false;
  bool        invertZ = false;
  std::string textureMap;
  std::string materialMap;

  std::vector<std::string> args;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      auto arg = QString(&argv[i][1]);

      if      (arg == "3ds" || arg == "3drw" || arg == "asc" || arg == "blend" ||
               arg == "cob" || arg == "dxf"  || arg == "fbx" || arg == "gltf"  ||
               arg == "obj" || arg == "plg"  || arg == "ply" || arg == "scene" ||
               arg == "stl"  || arg == "v3d" || arg == "vox" || arg == "x3d") {
        ++i;

        if (i < argc) {
          format    = CImportBase::suffixToType(arg.toStdString());
          modelName = QString(argv[i]);
        }
        else
          std::cerr << "Missing filename for " << argv[i - 1] << "\n";
      }
      else if (arg == "swap_xy") {
        swapXY = true;
      }
      else if (arg == "swap_yz") {
        swapYZ = true;
      }
      else if (arg == "swap_zx") {
        swapZX = true;
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
      else if (arg == "texture_map") {
        ++i;

        if (i < argc)
          textureMap = argv[i];
        else
          std::cerr << "Missing filename for " << argv[i - 1] << "\n";
      }
      else if (arg == "material_map") {
        ++i;

        if (i < argc)
          materialMap = argv[i];
        else
          std::cerr << "Missing filename for " << argv[i - 1] << "\n";
      }
      else if (arg == "h" || arg == "help") {
        std::cerr << "CQCamera3D "
         "[-3ds|-3drw|-asc|-blend|-cob|-dxf|-fbx|-gltf|-obj|-plg|-ply|-scene|-stl|-v3d|-vox|-x3d] "
         "[-swap_xy|swap_yz|swap_zx] "
         "[-invert_x|-invert_y|-invert_z] "
         "[-texture_map <file>] [<file>]"
         "[-material_map <file>] [<file>]"
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
      "3ds" << "3drw" << "asc" << "blend" << "cob" << "dxf" << "fbx" << "glb" << "gltf" <<
      "obj" << "plg" << "ply"<< "scene"<< "stl"<< "v3d" << "vox" << "x3d";

    for (const auto &str : strs)
      isSuffix(str);
  }

  //---

  auto *app = new CQCamera3DApp;

  if (modelName != "") {
    CQCamera3DApp::LoadData loadData;

    loadData.invertX = invertX;
    loadData.invertY = invertY;
    loadData.invertZ = invertZ;
    loadData.swapXY  = swapXY;
    loadData.swapYZ  = swapYZ;
    loadData.swapZX  = swapZX;

    if (! app->loadModel(modelName, format, loadData))
      std::cerr << "Failed to load model '" << modelName.toStdString() << "'\n";
  }

  //---

  auto *canvas = app->canvas();

  if (textureMap != "")
    canvas->setInitTextureMap(textureMap);

  if (materialMap != "")
    canvas->setInitMaterialMap(materialMap);

  //---

  auto *eventFilter = new CQCamera3DEventFilter;

  app->installEventFilter(eventFilter);

  //---

  app->show();

  //---

  qapp.exec();
}
