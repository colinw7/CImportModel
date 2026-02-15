#include <XML.h>
#include <App.h>

#include <NoiseTextureNode.h>
#include <MarbleTextureNode.h>
#include <ImageTextureNode.h>
#include <MaterialNode.h>
#include <MixNode.h>
#include <MathNode.h>
#include <BinaryMathNode.h>
#include <RemapNode.h>
#include <TextureCoordNode.h>
#include <ColorNode.h>
#include <SeparateColorNode.h>
#include <DebugNode.h>

#include <CXMLLib.h>
#include <CThrow.h>
#include <CStrUtil.h>
#include <CRGBName.h>

namespace CQTextureGen {

bool
XML::
read(const std::string &filename)
{
  CXML     xml;
  CXMLTag *tag;

  {
  CThrowScope throwScope(CTHROW_PRINT);

  if (! xml.read(filename, &tag) || ! tag)
    return false;
  }

  if (tag->getName() != "texture_gen")
    return false;

  // handle options
  const auto &options = tag->getOptions();

  auto num_options = tag->getNumOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
//  const auto &opt_value = option->getValue();

    std::cerr << "Unrecognised option '" << opt_name << "'\n";
  }

  //------

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options1 = tag1->getNumOptions();

    const auto &options1 = tag1->getOptions();

    std::string id;
    AppNode*    node { nullptr };

    auto handleAppNodeOption = [&](const std::string &opt_name, const std::string &opt_value) {
      if (opt_name == "id")
        id = opt_value;
      else
        return false;

      return true;
    };

    if      (name == "noise_texture") {
      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if (! handleAppNodeOption(opt_name, opt_value))
          std::cerr << "Unrecognised option '" << opt_name << "'\n";
      }

      node = app_->addNodeT<NoiseTextureNode>();
    }
    else if (name == "marble_texture") {
      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if (! handleAppNodeOption(opt_name, opt_value))
          std::cerr << "Unrecognised option '" << opt_name << "'\n";
      }

      node = app_->addNodeT<MarbleTextureNode>();
    }
    else if (name == "image_texture") {
      std::string filename;

      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "filename")
          filename = opt_value;
        else if (! handleAppNodeOption(opt_name, opt_value))
          std::cerr << "Unrecognised option '" << opt_name << "'\n";
      }

      node = app_->addNodeT<ImageTextureNode>();

      if (filename != "")
        dynamic_cast<ImageTextureNode *>(node)->setFilename(filename);
    }
    else if (name == "material") {
      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if (! handleAppNodeOption(opt_name, opt_value))
          std::cerr << "Unrecognised option '" << opt_name << "'\n";
      }

      node = app_->addNodeT<MaterialNode>();
    }
    else if (name == "mix") {
      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if (! handleAppNodeOption(opt_name, opt_value))
          std::cerr << "Unrecognised option '" << opt_name << "'\n";
      }

      node = app_->addNodeT<MixNode>();
    }
    else if (name == "math") {
      std::string type;

      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "type") {
          type = opt_value;
        }
        else if (! handleAppNodeOption(opt_name, opt_value))
          std::cerr << "Unrecognised option '" << opt_name << "'\n";
      }

      node = app_->addNodeT<MathNode>();

      if (type != "")
        dynamic_cast<MathNode *>(node)->setMathTypeFromString(type);
    }
    else if (name == "binary_math") {
      std::string type;

      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "type") {
          type = opt_value;
        }
        else if (! handleAppNodeOption(opt_name, opt_value))
          std::cerr << "Unrecognised option '" << opt_name << "'\n";
      }

      node = app_->addNodeT<BinaryMathNode>();

      if (type != "")
        dynamic_cast<BinaryMathNode *>(node)->setMathTypeFromString(type);
    }
    else if (name == "remap") {
      double from_min = 0.0, from_max = 1.0;
      double to_min   = 0.0, to_max   = 1.0;

      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "from_min") {
          parseReal(opt_name, opt_value, &from_min);
        }
        else if (opt_name == "from_max") {
          parseReal(opt_name, opt_value, &from_max);
        }
        else if (opt_name == "to_max") {
          parseReal(opt_name, opt_value, &to_max);
        }
        else if (opt_name == "to_max") {
          parseReal(opt_name, opt_value, &to_max);
        }
        else if (! handleAppNodeOption(opt_name, opt_value))
          std::cerr << "Unrecognised option '" << opt_name << "'\n";
      }

      node = app_->addNodeT<RemapNode>();

      dynamic_cast<RemapNode *>(node)->setFromMin(from_min);
      dynamic_cast<RemapNode *>(node)->setFromMax(from_max);
      dynamic_cast<RemapNode *>(node)->setToMin  (to_min);
      dynamic_cast<RemapNode *>(node)->setToMax  (to_max);
    }
    else if (name == "texture_coord") {
      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if (! handleAppNodeOption(opt_name, opt_value))
          std::cerr << "Unrecognised option '" << opt_name << "'\n";
      }

      node = app_->addNodeT<TextureCoordNode>();
    }
    else if (name == "color") {
      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if (! handleAppNodeOption(opt_name, opt_value))
          std::cerr << "Unrecognised option '" << opt_name << "'\n";
      }

      node = app_->addNodeT<ColorNode>();
    }
    else if (name == "separate_color") {
      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if (! handleAppNodeOption(opt_name, opt_value))
          std::cerr << "Unrecognised option '" << opt_name << "'\n";
      }

      node = app_->addNodeT<SeparateColorNode>();
    }
    else if (name == "debug") {
      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if (! handleAppNodeOption(opt_name, opt_value))
          std::cerr << "Unrecognised option '" << opt_name << "'\n";
      }

      node = app_->addNodeT<DebugNode>();
    }
    else if (name == "pipe") {
      std::string fromStr, toStr;

      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "from")
          fromStr = opt_value;
        else if (opt_name == "to")
          toStr = opt_value;
        else
          std::cerr << "Unrecognised option '" << opt_name << "'\n";
      }

      auto decodeId = [](const std::string &id, std::string &node, std::string &io) {
        auto p = id.find(':');
        if (p == std::string::npos) return false;
        node = id.substr(0, p);
        io   = id.substr(p + 1);
        return true;
      };

      std::string fromId, fromIOName;
      std::string toId, toIOName;

      if (! decodeId(fromStr, fromId, fromIOName) || ! decodeId(toStr, toId, toIOName)) {
        std::cerr << "Bad from/to syntax : from='" << fromStr << "' to='" << toStr << "'\n";
        continue;
      }

      auto *fromNode = app_->getNodeById(fromId);
      auto *toNode   = app_->getNodeById(toId);

      if (! fromNode || ! toNode) {
        std::cerr << "Bad from/to id : from='" << fromId << "' to='" << toId << "'\n";
        continue;
      }

      auto *fromIO = fromNode->getIOByName(fromIOName);
      auto *toIO   = toNode  ->getIOByName(toIOName);

      if (! fromIO || ! toIO) {
        std::cerr << "Bad from/to IO : from='" << fromIOName << "' to='" << toIOName << "'\n";
        continue;
      }

      auto *fromOutput = dynamic_cast<NodeOutput *>(fromIO);
      auto *toInput    = dynamic_cast<NodeInput  *>(toIO);

      if (! fromOutput || ! toInput) {
        std::cerr << "Bad from/to IO direction : from='" <<
          fromIOName << "' to='" << toIOName << "'\n";
        continue;
      }

      app_->addPipe("", fromOutput, toInput);
    }
    else
      std::cerr << "Unrecognised tag '" << name << "'\n";

    if (node && id != "")
      node->setId(id);
  }

  delete tag;

  return true;
}

bool
XML::
parseReal(const std::string &name, const std::string &value, double *real) const
{
  double real1;

  if (! CStrUtil::toReal(value, &real1)) {
    std::cerr << "Illegal real value for '" << name << "'\n";
    return false;
  }

  *real = real1;

  return true;
}

}
