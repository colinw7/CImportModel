#include <CImport3MF.h>
#include <CGeometry3D.h>
#include <miniz.h>

#include <CXML.h>
#include <CXMLToken.h>
#include <CXMLVisitor.h>
#include <CStrUtil.h>
#include <CThrow.h>
#include <CRGBName.h>

#include <filesystem>
#include <iostream>

namespace {

class XMLVisitor : public CXMLVisitor {
 public:
  XMLVisitor(CXMLTag *tag) :
   CXMLVisitor(tag) {
    setRecurse(false);
  }

  bool processTagOption(CXMLTagOption *option) override {
    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    return processOption(opt_name, opt_value);
  }

  bool processTagChildTag(CXMLTag *tag) override {
    const auto &name = tag->getName();

    return processTag(tag, name);
  }

  virtual bool processOption(const std::string &, const std::string &) {
    return false;
  }

  virtual bool processTag(CXMLTag *, const std::string &) {
    return false;
  }
};

std::string pathToFile(const std::string &str) {
  auto pos = str.rfind('/');
  if (pos == std::string::npos)
    return str;
  auto str1 = str.substr(pos + 1);
  return str1;
}

}

//---

CImport3MF::
CImport3MF(CGeomScene3D *scene, const std::string &name) :
 scene_(scene)
{
  if (! scene_) {
    scene_  = CGeometry3DInst->createScene3D();
    pscene_ = SceneP(scene_);
  }

  auto name1 = name;

  if (name1 == "")
    name1 = "3mf";

  object_  = CGeometry3DInst->createObject3D(scene_, name1);
  pobject_ = ObjectP(object_);

  scene_->addObject(object_);
}

CImport3MF::
~CImport3MF()
{
}

bool
CImport3MF::
read(CFile &file)
{
  auto path = file.getPath();

  extractFiles(path);

  auto pf = fileData_.files.find("3dmodel.model");
  if (pf == fileData_.files.end())
    return false;

  //---

  currentObject_ = nullptr;

  if (! readModel((*pf).second))
    return false;

  //---

  return true;
}

bool
CImport3MF::
readModel(const std::string &model)
{
  CXML     xml;
  CXMLTag *tag;

  {
  CThrowScope throwScope(CTHROW_PRINT);

  if (! xml.readString(model, &tag) || ! tag)
    return false;
  }

  if (tag->getName() != "model")
    return false;

  //---

  const auto &options = tag->getOptions();

  auto num_options = tag->getNumOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
//  const auto &opt_value = option->getValue();

    if      (opt_name == "unit") {
    }
    else if (opt_name == "xml:lang") {
    }
    else if (opt_name == "xmlns") {
    }
    else if (opt_name == "xmlns:b") {
    }
    else if (opt_name == "xmlns:bt") {
    }
    else if (opt_name == "xmlns:l") {
    }
    else if (opt_name == "xmlns:m") {
    }
    else if (opt_name == "xmlns:p") {
    }
    else if (opt_name == "xmlns:s") {
    }
    else if (opt_name == "xmlns:t") {
    }
    else if (opt_name == "xmlns:BambuStudio") {
    }
    else if (opt_name == "requiredextensions") {
    }
    else {
      std::cerr << "Unknown " << tag->getName() << " option '" + opt_name + "'\n";
    }
  }

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name1 = tag1->getName();

    if      (name1 == "metadata") {
    }
    else if (name1 == "resources") {
      if (! readResources(tag1))
        return false;
    }
    else if (name1 == "build") {
      if (! readBuild(tag1))
        return false;
    }
    else {
      std::cerr << "Unknown " << tag->getName() << " tag '" << name1 << "'\n";
    }
  }

  //---

  delete tag;

  return true;
}

bool
CImport3MF::
readResources(CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name1 = tag1->getName();

    if      (name1 == "object") {
      if (! readObject(tag1))
        return false;
    }
    else if (name1 == "basematerials") {
      if (! readBaseMaterials(tag1))
        return false;
    }
    else if (name1 == "m:colorgroup") {
      if (! readColorGroup(tag1))
        return false;
    }
    else if (name1 == "m:texture2d") {
      if (! readTexture2D(tag1))
        return false;
    }
    else if (name1 == "m:texture2dgroup") {
      if (! readTexture2DGroup(tag1))
        return false;
    }
    else if (name1 == "m:multiproperties") {
      if (! readMultiProperties(tag1))
        return false;
    }
    else if (name1 == "m:pbmetallicdisplayproperties") {
      if (! readMetallicDisplayProperties(tag1))
        return false;
    }
    else {
      std::cerr << "Unknown " << tag->getName() << " tag '" << name1 << "'\n";
    }
  }

  return true;
}

bool
CImport3MF::
readBuild(CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name1 = tag1->getName();

    if (name1 == "item") {
      if (! readItem(tag1))
        return false;
    }
    else {
      std::cerr << "Unknown " << tag->getName() << " tag '" << name1 << "'\n";
    }
  }

  return true;
}

bool
CImport3MF::
readObject(CXMLTag *tag)
{
  const auto &options = tag->getOptions();

  auto num_options = tag->getNumOptions();

  struct Data {
    std::string name;
    int         id     { -1 };
    int         pid    { -1 };
    int         pindex { -1 };
    std::string type;
    CMatrix3D   transform { CMatrix3D::identity() };
  };

  Data data;

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if      (opt_name == "name") {
      data.name = opt_value;
    }
    else if (opt_name == "id") {
      if (! CStrUtil::toInteger(opt_value, &data.id))
        return false;
    }
    else if (opt_name == "pid") {
      if (! CStrUtil::toInteger(opt_value, &data.pid))
        return false;
    }
    else if (opt_name == "pindex") {
       if (! CStrUtil::toInteger(opt_value, &data.pindex))
        return false;
    }
    else if (opt_name == "type") {
      data.type = opt_value;
    }
    else if (opt_name == "transform") {
      std::vector<std::string> words;
      CStrUtil::toWords(opt_value, words);

      if (words.size() != 12)
        return false;

      std::vector<double> values;
      values.resize(12);

      for (uint i = 0; i < 12; ++i) {
        double r;
        if (! CStrUtil::toReal(words[i], &r))
          return false;

        values[i] = r;
      }

      data.transform = CMatrix3D(&values[0], 12);
    }
    else if (opt_name == "partnumber") {
    }
    else if (opt_name == "thumbnail") {
    }
    else if (opt_name == "p:UUID") {
    }
    else {
      std::cerr << "Unknown " << tag->getName() << " option '" + opt_name + "'\n";
    }
  }

  //---

  if (data.name == "")
    data.name = "object." + std::to_string(objectCount_ + 1);

  currentObject_ = CGeometry3DInst->createObject3D(scene_, data.name);

  scene_->addObject(currentObject_);

  pobject_->addChild(currentObject_);

  ++objectCount_;

  idObject_[data.id] = currentObject_;

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name1 = tag1->getName();

    if      (name1 == "mesh") {
      if (! readMesh(tag1))
        return false;
    }
    else if (name1 == "components") {
      if (! readComponents(tag1))
        return false;
    }
    else {
      std::cerr << "Unknown " << tag->getName() << " tag '" << name1 << "'\n";
    }
  }

  return true;
}

bool
CImport3MF::
readMesh(CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name1 = tag1->getName();

    if      (name1 == "vertices") {
      if (! readVertices(tag1))
        return false;
    }
    else if (name1 == "triangles") {
      if (! readTriangles(tag1))
        return false;
    }
    else if (name1 == "b:beamlattice") {
    } else {
      std::cerr << "Unknown " << tag->getName() << " tag '" << name1 << "'\n";
    }
  }

  return true;
}

bool
CImport3MF::
readVertices(CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name1 = tag1->getName();

    if      (name1 == "vertex") {
      double x, y, z;

      const auto &options1 = tag1->getOptions();

      auto num_options1 = tag1->getNumOptions();

      for (size_t j = 0; j < num_options1; ++j) {
        auto *option1 = options1[j];

        const auto &opt_name  = option1->getName ();
        const auto &opt_value = option1->getValue();

        if      (opt_name == "x") {
          if (! CStrUtil::toReal(opt_value, &x))
            return false;
        }
        else if (opt_name == "y") {
          if (! CStrUtil::toReal(opt_value, &y))
            return false;
        }
        else if (opt_name == "z") {
          if (! CStrUtil::toReal(opt_value, &z))
            return false;
        }
        else {
          std::cerr << "Unknown " << tag1->getName() << " option '" + opt_name + "'\n";
        }
      }

      if (isDebug())
        std::cerr << "Vertex: " << x << " " << y << " " << z << "\n";

      currentObject_->addVertex(CPoint3D(x, y, z));
    }
    else {
      std::cerr << "Unknown " << tag->getName() << " tag '" << name1 << "'\n";
    }
  }

  return true;
}

bool
CImport3MF::
readTriangles(CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name1 = tag1->getName();

    if      (name1 == "triangle") {
      int v1 = -1, v2 = -1, v3 = -1;
      int pid = -1, p1 = -1, p2 = -1, p3 = -1;

      const auto &options1 = tag1->getOptions();

      auto num_options1 = tag1->getNumOptions();

      for (size_t j = 0; j < num_options1; ++j) {
        auto *option1 = options1[j];

        const auto &opt_name  = option1->getName ();
        const auto &opt_value = option1->getValue();

        if      (opt_name == "v1") {
          if (! CStrUtil::toInteger(opt_value, &v1))
            return false;
        }
        else if (opt_name == "v2") {
          if (! CStrUtil::toInteger(opt_value, &v2))
            return false;
        }
        else if (opt_name == "v3") {
          if (! CStrUtil::toInteger(opt_value, &v3))
            return false;
        }
        else if (opt_name == "pid") {
          if (! CStrUtil::toInteger(opt_value, &pid))
            return false;
        }
        else if (opt_name == "p1") {
          if (! CStrUtil::toInteger(opt_value, &p1))
            return false;
        }
        else if (opt_name == "p2") {
          if (! CStrUtil::toInteger(opt_value, &p2))
            return false;
        }
        else if (opt_name == "p3") {
          if (! CStrUtil::toInteger(opt_value, &p3))
            return false;
        }
        else if (opt_name == "paint_color") {
        }
        else {
          std::cerr << "Unknown " << tag1->getName() << " option '" + opt_name + "'\n";
        }
      }

      if (isDebug())
        std::cerr << "Triangle: " << v1 << " " << v2 << " " << v3 << "\n";

      auto faceInd = currentObject_->addITriangle(uint(v1), uint(v2), uint(v3));

      if (pid > 0) {
        auto *resource = getResource(pid);
        if (! resource) {
          std::cerr << "Resource " << pid << " not found\n";
          return false;
        }

        if (resource->rtype == ResourceType::COLOR_GROUP) {
          auto *colorGroup = static_cast<ColorGroup *>(resource);

          auto nc = colorGroup->colors.size();

          if      (p1 >= 0 && p2 >= 0 && p3>= 0) {
            if (p1 < int(nc) && p2 < int(nc) && p3< int(nc)) {
              auto c1 = colorGroup->colors[p1];
              auto c2 = colorGroup->colors[p2];
              auto c3 = colorGroup->colors[p3];

              auto *pv1 = currentObject_->getVertexP(uint(v1));
              auto *pv2 = currentObject_->getVertexP(uint(v2));
              auto *pv3 = currentObject_->getVertexP(uint(v3));

              pv1->setColor(c1);
              pv2->setColor(c2);
              pv3->setColor(c3);
            }
            else
              std::cerr << "Invalid color index\n";
          }
          else if (p1 >= 0) {
            if (p1 < int(nc)) {
              auto c = colorGroup->colors[p1];

              currentObject_->setFaceColor(faceInd, c);
            }
            else
              std::cerr << "Invalid color index\n";
          }
          else
            std::cerr << "Missing color id\n";
        }
        else if (resource->rtype == ResourceType::TEXTURE_GROUP) {
          auto *face = currentObject_->getFaceP(faceInd);

          auto *textureGroup = static_cast<TextureGroup *>(resource);

          face->setDiffuseTexture(textureGroup->geomTexture);

          if (p1 >= 0 && p2 >= 0 && p3 >= 0) {
            auto np = textureGroup->points.size();

            if (p1 < int(np) && p2 < int(np) && p3< int(np)) {
              auto tp1 = textureGroup->points[p1];
              auto tp2 = textureGroup->points[p2];
              auto tp3 = textureGroup->points[p3];

              std::vector<CPoint2D> points = { tp1, tp2, tp3 };

              face->setTextureMapping(points);
            }
            else
              std::cerr << "Invalid texture point index\n";
          }
          else
            std::cerr << "Missing texture points\n";
        }
        else
          std::cerr << "Unhandled Resource Type " << int(resource->rtype) << "\n";
      }
    }
    else {
      std::cerr << "Unknown " << tag->getName() << " tag '" << name1 << "'\n";
    }
  }

  return true;
}

bool
CImport3MF::
readComponents(CXMLTag *tag)
{
  class Visitor : public XMLVisitor {
   public:
    Visitor(CImport3MF *th, CXMLTag *tag) :
     XMLVisitor(tag), th_(th) {
    }

    bool processTag(CXMLTag *tag, const std::string &name) override {
      if (name == "component") {
        if (! th_->readComponent(tag))
          return false;
      }
      else
        return false;

      return true;
    }

   private:
    CImport3MF* th_ { nullptr };
  };

  Visitor visitor(this, tag);

  if (! visitor.process())
    return false;

  return true;
}

bool
CImport3MF::
readComponent(CXMLTag *tag)
{
  struct Data {
    int         objectid  { -1 };
    CMatrix3D   transform { CMatrix3D::identity() };
    std::string path;
  };

  class Visitor : public XMLVisitor {
   public:
    Visitor(CImport3MF *th, CXMLTag *tag, Data &data) :
     XMLVisitor(tag), th_(th), data_(data) {
    }

    bool processOption(const std::string &opt_name, const std::string &opt_value) override {
      if      (opt_name == "objectid") {
        if (! CStrUtil::toInteger(opt_value, &data_.objectid))
          return false;
      }
      else if (opt_name == "transform") {
        std::vector<std::string> words;
        CStrUtil::toWords(opt_value, words);

        if (words.size() != 12)
          return false;

        std::vector<double> values;
        values.resize(12);

        for (uint i = 0; i < 12; ++i) {
          double r;
          if (! CStrUtil::toReal(words[i], &r))
            return false;

          values[i] = r;
        }

        data_.transform = CMatrix3D(&values[0], 12);
      }
      else if (opt_name == "p:path") {
        data_.path = opt_value;
      }
      else if (opt_name == "p:UUID") {
      }
      else
        return false;

      return true;
    }

   private:
    CImport3MF* th_ { nullptr };
    Data&       data_;
  };

  Data data;
  Visitor visitor(this, tag, data);

  if (! visitor.process())
    return false;

  if (data.path != "") {
    auto filename = pathToFile(data.path);

    auto pf = fileData_.files.find(filename);
    if (pf == fileData_.files.end())
      return false;
//std::cerr << "Component Path '" << filename << "'\n";

    CGeomObject3D *currentObject = nullptr;

    std::swap(currentObject_, currentObject);

    if (! readModel((*pf).second))
      return false;

    if (currentObject_)
      currentObject_->transform(data.transform);

    std::swap(currentObject_, currentObject);
  }

  return true;
}

bool
CImport3MF::
readBaseMaterials(CXMLTag *tag)
{
  const auto &options = tag->getOptions();

  auto num_options = tag->getNumOptions();

  int id = -1;

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if (opt_name == "id") {
      if (! CStrUtil::toInteger(opt_value, &id))
        return false;
    }
    else {
      std::cerr << "Unknown " << tag->getName() << " option '" + opt_name + "'\n";
    }
  }

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name1 = tag1->getName();

    if (name1 == "base") {
      if (! readBaseMaterial(tag1))
        return false;
    }
    else {
      std::cerr << "Unknown " << tag->getName() << " tag '" << name1 << "'\n";
    }
  }

  return true;
}

bool
CImport3MF::
readBaseMaterial(CXMLTag *tag)
{
  const auto &options = tag->getOptions();

  auto num_options = tag->getNumOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();

    if      (opt_name == "name") {
    }
    else if (opt_name == "displaycolor") {
    }
    else if (opt_name == "displaypropertiesid") {
    }
    else {
      std::cerr << "Unknown " << tag->getName() << " option '" + opt_name + "'\n";
    }
  }

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name1 = tag1->getName();

    std::cerr << "Unknown " << tag->getName() << " tag '" << name1 << "'\n";
  }

  return true;
}

bool
CImport3MF::
readColorGroup(CXMLTag *tag)
{
  const auto &options = tag->getOptions();

  auto num_options = tag->getNumOptions();

  int id = -1;

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if (opt_name == "id") {
      if (! CStrUtil::toInteger(opt_value, &id))
        return false;
    }
    else {
      std::cerr << "Unknown " << tag->getName() << " option '" + opt_name + "'\n";
    }
  }

  //---

  auto *colorGroup = new ColorGroup;

  colorGroup->rtype = ResourceType::COLOR_GROUP;
  colorGroup->id    = id;

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name1 = tag1->getName();

    if (name1 == "m:color") {
      CRGBA c;
      if (! readColor(tag1, c))
        return false;

      colorGroup->colors.push_back(c);
    }
    else {
      std::cerr << "Unknown " << tag->getName() << " tag '" << name1 << "'\n";
    }
  }

  //---

  idResource_[id] = colorGroup;

  return true;
}

bool
CImport3MF::
readColor(CXMLTag *tag, CRGBA &c)
{
  const auto &options = tag->getOptions();

  auto num_options = tag->getNumOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if (opt_name == "color") {
      c = CRGBName::toRGBA(opt_value);
    }
    else {
      std::cerr << "Unknown " << tag->getName() << " option '" + opt_name + "'\n";
    }
  }

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name1 = tag1->getName();

    std::cerr << "Unknown " << tag->getName() << " tag '" << name1 << "'\n";
  }

  return true;
}

bool
CImport3MF::
readTexture2D(CXMLTag *tag)
{
  const auto &options = tag->getOptions();

  auto num_options = tag->getNumOptions();

  struct Data {
    int         id { -1 };
    std::string path;
    std::string type;
  };

  Data data;

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if      (opt_name == "id") {
      if (! CStrUtil::toInteger(opt_value, &data.id))
        return false;
    }
    else if (opt_name == "path") {
      data.path = opt_value;
    }
    else if (opt_name == "contenttype") {
      data.type = opt_value;
    }
    else {
      std::cerr << "Unknown " << tag->getName() << " option '" + opt_name + "'\n";
    }
  }

  //---

  auto *texture = new Texture;

  texture->rtype = ResourceType::TEXTURE;
  texture->id    = data.id;
  texture->path  = data.path;
  texture->type  = data.type;

  //---

  auto filename = pathToFile(data.path);

  auto pf = fileData_.files.find(filename);
  if (pf == fileData_.files.end())
    return false;
//std::cerr << "Texture '" << filename << "'\n";

  CImageDataSrc src((*pf).second);

  texture->image = CImageMgrInst->lookupImage(src);

  if (! texture->image) {
    std::cerr << "Failed to read image data\n";
    return false;
  }

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name1 = tag1->getName();

    std::cerr << "Unknown " << tag->getName() << " tag '" << name1 << "'\n";
  }

  //---

  idResource_[data.id] = texture;

  return true;
}

bool
CImport3MF::
readTexture2DGroup(CXMLTag *tag)
{
  const auto &options = tag->getOptions();

  auto num_options = tag->getNumOptions();

  struct Data {
    int id    { -1 };
    int texid { -1 };
  };

  Data data;

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if      (opt_name == "id") {
      if (! CStrUtil::toInteger(opt_value, &data.id))
        return false;
    }
    else if (opt_name == "texid") {
      if (! CStrUtil::toInteger(opt_value, &data.texid))
        return false;
    }
    else {
      std::cerr << "Unknown " << tag->getName() << " option '" + opt_name + "'\n";
    }
  }

  //---

  auto *textureGroup = new TextureGroup;

  textureGroup->rtype = ResourceType::TEXTURE_GROUP;
  textureGroup->id    = data.id;

  if (data.texid >= 0) {
    auto *resource = getResource(data.texid);
    if (! resource) {
      std::cerr << "Resource " << data.texid << " not found\n";
      return false;
    }

    if (resource->rtype != ResourceType::TEXTURE) {
      std::cerr << "Invalid texture group resource type\n";
      return false;
    }

    auto *texture = static_cast<Texture *>(resource);

    textureGroup->geomTexture = CGeometry3DInst->createTexture(texture->image);
  }

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name1 = tag1->getName();

    if (name1 == "m:tex2coord") {
      double u, v;

      const auto &options1 = tag1->getOptions();

      auto num_options1 = tag1->getNumOptions();

      for (size_t j = 0; j < num_options1; ++j) {
        auto *option1 = options1[j];

        const auto &opt_name  = option1->getName ();
        const auto &opt_value = option1->getValue();

        if      (opt_name == "u") {
          if (! CStrUtil::toReal(opt_value, &u))
            return false;
        }
        else if (opt_name == "v") {
          if (! CStrUtil::toReal(opt_value, &v))
            return false;
        }
        else {
          std::cerr << "Unknown " << tag1->getName() << " option '" + opt_name + "'\n";
        }
      }

      if (isDebug())
        std::cerr << "Tex Coord: " << u << " " << v << "\n";

      textureGroup->points.push_back(CPoint2D(u, v));
    }
    else {
      std::cerr << "Unknown " << tag->getName() << " tag '" << name1 << "'\n";
    }
  }

  //---

  idResource_[data.id] = textureGroup;

  return true;
}

bool
CImport3MF::
readMultiProperties(CXMLTag *tag)
{
  struct Data {
    int              id { -1 };
    std::vector<int> pids;
  };

  class Visitor : public XMLVisitor {
   public:
    Visitor(CImport3MF *th, CXMLTag *tag) :
     XMLVisitor(tag), th_(th) {
    }

    bool processOption(const std::string &opt_name, const std::string &opt_value) override {
      if      (opt_name == "id") {
        if (! CStrUtil::toInteger(opt_value, &data_.id))
          return false;
      }
      else if (opt_name == "pids") {
        std::vector<std::string> words;
        CStrUtil::toWords(opt_value, words);

        for (const auto &word : words) {
          int id;
          if (! CStrUtil::toInteger(word, &id))
            return false;

          data_.pids.push_back(id);
        }
      }
      else
        return false;

      return true;
    }

    bool processTag(CXMLTag *tag, const std::string &name) override {
      if (name == "multi") {
        if (! th_->readMulti(tag))
          return false;
      }
      else
        return false;

      return true;
    }

   private:
    CImport3MF* th_ { nullptr };
    Data        data_;
  };

  Visitor visitor(this, tag);

  if (! visitor.process())
    return false;

  return true;
}

bool
CImport3MF::
readMulti(CXMLTag *tag)
{
  struct Data {
    std::vector<int> pindices;
  };

  class Visitor : public XMLVisitor {
   public:
    Visitor(CImport3MF *th, CXMLTag *tag) :
     XMLVisitor(tag), th_(th) {
    }

    bool processOption(const std::string &opt_name, const std::string &opt_value) override {
      if (opt_name == "pindices") {
        std::vector<std::string> words;
        CStrUtil::toWords(opt_value, words);

        for (const auto &word : words) {
          int id;
          if (! CStrUtil::toInteger(word, &id))
            return false;

          data_.pindices.push_back(id);
        }
      }
      else
        return false;

      return true;
    }

   private:
    CImport3MF* th_ { nullptr };
    Data        data_;
  };

  Visitor visitor(this, tag);

  if (! visitor.process())
    return false;

  return true;
}

bool
CImport3MF::
readMetallicDisplayProperties(CXMLTag *tag)
{
  struct Data {
    int id { -1 };
  };

  class Visitor : public XMLVisitor {
   public:
    Visitor(CImport3MF *th, CXMLTag *tag) :
     XMLVisitor(tag), th_(th) {
    }

    bool processOption(const std::string &opt_name, const std::string &opt_value) override {
      if (opt_name == "id") {
        if (! CStrUtil::toInteger(opt_value, &data_.id))
          return false;
      }
      else
        return false;

      return true;
    }

    bool processTag(CXMLTag *tag, const std::string &name) override {
      if (name == "ms:pbmetallic") {
        if (! th_->readMetallic(tag))
          return false;
      }
      else
        return false;

      return true;
    }

   private:
    CImport3MF* th_ { nullptr };
    Data        data_;
  };

  Visitor visitor(this, tag);

  if (! visitor.process())
    return false;

  return true;
}

bool
CImport3MF::
readMetallic(CXMLTag *tag)
{
  struct Data {
    int         id           { -1 };
    std::string name;
    double      metallicness { 0 };
    double      roughness    { 0 };
  };

  class Visitor : public XMLVisitor {
   public:
    Visitor(CImport3MF *th, CXMLTag *tag) :
     XMLVisitor(tag), th_(th) {
    }

    bool processOption(const std::string &opt_name, const std::string &opt_value) override {
      if      (opt_name == "name") {
        data_.name = opt_value;
      }
      else if (opt_name == "metallicness") {
        if (! CStrUtil::toReal(opt_value, &data_.metallicness))
          return false;
      }
      else if (opt_name == "roughness") {
        if (! CStrUtil::toReal(opt_value, &data_.roughness))
          return false;
      }
      else
        return false;

      return true;
    }

   private:
    CImport3MF* th_ { nullptr };
    Data        data_;
  };

  Visitor visitor(this, tag);

  if (! visitor.process())
    return false;

  return true;
}

bool
CImport3MF::
readItem(CXMLTag *tag)
{
  const auto &options = tag->getOptions();

  auto num_options = tag->getNumOptions();

  struct Data {
    int       objectid  { -1 };
    CMatrix3D transform { CMatrix3D::identity() };
  };

  Data data;

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if      (opt_name == "objectid") {
      if (! CStrUtil::toInteger(opt_value, &data.objectid))
        return false;
    }
    else if (opt_name == "transform") {
      std::vector<std::string> words;
      CStrUtil::toWords(opt_value, words);

      if (words.size() != 12)
        return false;

      std::vector<double> values;
      values.resize(12);

      for (uint i = 0; i < 12; ++i) {
        double r;
        if (! CStrUtil::toReal(words[i], &r))
          return false;

        values[i] = r;
      }

      data.transform = CMatrix3D(&values[0], 12);
    }
    else if (opt_name == "printable") {
    }
    else if (opt_name == "p:UUID") {
    }
    else {
      std::cerr << "Unknown " << tag->getName() << " option '" + opt_name + "'\n";
    }
  }

  auto po = idObject_.find(data.objectid);
  if (po == idObject_.end())
    return false;

  auto *object = (*po).second;

  object->transform(data.transform);

  return true;
}

bool
CImport3MF::
extractFiles(const std::string &path)
{
  namespace fs = std::filesystem;

  mz_zip_archive zipArchive { };
  if (! mz_zip_reader_init_file(&zipArchive, path.c_str(), 0))
    return false;

  bool found = false;

  int fileCount = static_cast<int>(mz_zip_reader_get_num_files(&zipArchive));

  for (int i = 0; i < fileCount; ++i) {
    mz_zip_archive_file_stat fileStat;
    if (! mz_zip_reader_file_stat(&zipArchive, i, &fileStat))
      continue;

    std::string filename = fileStat.m_filename;

    if      (CStrUtil::endsWith(filename, ".model")) {
      auto filename1 = pathToFile(filename);

      size_t buffer_size;
      void *buffer = mz_zip_reader_extract_to_heap(&zipArchive, i, &buffer_size, 0);
      if (buffer) {
        fileData_.files[filename1] = std::string(reinterpret_cast<char *>(buffer), buffer_size);
        mz_free(buffer);
        found = true;
      }
    }
    else if (filename.find("3D/Textures/") != std::string::npos) {
      auto filename1 = pathToFile(filename);

      size_t buffer_size;
      void *buffer = mz_zip_reader_extract_to_heap(&zipArchive, i, &buffer_size, 0);
      if (buffer) {
        fileData_.files[filename1] = std::string(reinterpret_cast<char *>(buffer), buffer_size);
        mz_free(buffer);
        found = true;
      }
    }
    else if (CStrUtil::endsWith(filename, ".rels")) {
      // skip
    }
    else if (CStrUtil::endsWith(filename, "[Content_Types].xml")) {
      // skip
    }
    else {
      std::cerr << "Unhandled file '" << filename << "'\n";
    }
  }

  mz_zip_reader_end(&zipArchive);

  return found;
}

CImport3MF::Resource *
CImport3MF::
getResource(int id) const
{
  auto pi = idResource_.find(id);
  if (pi == idResource_.end())
    return nullptr;

  return (*pi).second;
}
