#include <CImportGLTF.h>
#include <iostream>

namespace {

auto exitMsg(const std::string &msg) -> int {
  std::cerr << "^[[33mError^[[0m: " << msg << "\n";
  return 1;
}

}

//---

int
main(int argc, char **argv)
{
  std::string filename;
  bool        debug { false };
  bool        debugData { false };

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      auto arg = std::string(&argv[i][1]);

      if      (arg == "debug")
        debug = true;
      else if (arg == "debug_data")
        debugData = true;
      else if (arg == "h" || arg == "help") {
        std::cerr << "CImportGLTF [-debug] [-debugData] <filename>\n";
        return 0;
      }
      else
        return exitMsg("Invalid arg '" + arg + "'");
    }
    else
      filename = argv[i];
  }

  if (filename == "")
    return exitMsg("Missing filename");

  CImportGLTF gltf;

  gltf.setDebug(debug);
  gltf.setDebugData(debugData);

  CFile file(filename);

  if (! gltf.read(file))
    return exitMsg("Load '" + filename + "' failed");

  return 0;
}
