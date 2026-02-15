#include <Main.h>
#include <App.h>

#include <QApplication>

#include <iostream>

int
main(int argc, char **argv)
{
  QApplication qapp(argc, argv);

  int ind = 0;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      auto opt = std::string(&argv[i][1]);

      if (opt == "ind") {
        ++i;

        if (i < argc)
          ind = std::atoi(argv[i]);
        else
          std::cerr << "Missing value for -ind\n";
      }
      else
        std::cerr << "Invalid option '" << argv[i] << "\n";
    }
    else
      std::cerr << "Invalid arg '" << argv[i] << "\n";
  }

  auto *app = new CQShaderGen::App;

  app->setInd(ind);

  app->show();

  return qapp.exec();
}
