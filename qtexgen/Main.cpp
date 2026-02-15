#include <Main.h>

#include <CQImage.h>

#include <QApplication>

int
main(int argc, char **argv)
{
  QApplication qapp(argc, argv);

  CQImage::setPrototype();

  //---

  auto *app = new CQTextureGen::App;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      auto arg = std::string(&argv[i][1]);

      if (arg == "xml") {
        ++i;

        if (i < argc)
          app->loadXML(argv[i]);
        else {
          std::cerr << "Missing filename for -xml\n";
          exit(1);
        }
      }
    }
  }

  app->show();

  return qapp.exec();
}
