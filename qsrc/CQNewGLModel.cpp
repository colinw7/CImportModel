#include <CQNewGLModel.h>
#include <CQNewGLControl.h>
#include <CQNewGLCanvas.h>

#include <CImportBase.h>
#include <CGeomScene3D.h>

#include <CQApp.h>

#include <QApplication>
#include <QVBoxLayout>
#include <QSplitter>

#include <cmath>
#include <iostream>

//---

int
main(int argc, char **argv)
{
  CQApp app(argc, argv);

  //---

  QString     modelName = "/home/colinw/dev/models/v3d/Lightplane.V3D";
  CGeom3DType format    = CGEOM_3D_TYPE_V3D;

  bool flipYZ = false;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      auto arg = QString(&argv[i][1]);

      if      (arg == "3ds"  || arg == "3drw" || arg == "asc" || arg == "cob"   || arg == "dxf" ||
               arg == "gltf" || arg == "obj"  || arg == "plg" || arg == "scene" || arg == "v3d" ||
               arg == "x3d") {
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
      else if (arg == "h" || arg == "help") {
        std::cout << "CGLNewModel [-flip_yz] [[-3ds|-obj|-scene|-v3d|-x3d] <filename>]\n";
        return 0;
      }
      else {
        std::cerr << "Invalid option " << argv[i] << "\n";
      }
    }
    else
      std::cerr << "Invalid arg " << argv[i] << "\n";
  }

  //---

  auto *modelApp = new CQNewGLModel;

  auto *canvas = modelApp->canvas();

  canvas->setFlipYZ(flipYZ);

  if (! canvas->loadModel(modelName, format))
    exit(1);

  modelApp->resize(modelApp->windowWidth(), modelApp->windowHeight());

  //---

  modelApp->show();

  app.exec();
}

//---

CQNewGLModel::
CQNewGLModel() :
 QFrame()
{
  auto *layout = new QHBoxLayout(this);

  auto *splitter = new QSplitter;

  layout->addWidget(splitter);

  //---

  canvas_  = new CQNewGLCanvas(this);
  control_ = new CQNewGLControl(this);

  splitter->addWidget(canvas_);
  splitter->addWidget(control_);
}
