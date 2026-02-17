#include <CImportDAE.h>
#include <CGeometry3D.h>
#include <CGeomScene3D.h>

#include <CXML.h>
#include <CXMLToken.h>
#include <CStrUtil.h>
#include <CThrow.h>

CImportDAE::
CImportDAE(CGeomScene3D *scene, const std::string &name) :
 scene_(scene)
{
  if (! scene_) {
    scene_  = CGeometry3DInst->createScene3D();
    pscene_ = SceneP(scene_);
  }

  auto name1 = name;

  if (name1 == "")
    name1 = "dae";

  object_  = CGeometry3DInst->createObject3D(scene_, name);
  pobject_ = ObjectP(object_);

  scene_->addObject(object_);
}

CImportDAE::
~CImportDAE()
{
}

bool
CImportDAE::
read(CFile &file)
{
  file_ = &file;

  if (! file_->open(CFileBase::Mode::READ))
    return false;

  CXML     xml;
  CXMLTag *tag;

  {
  CThrowScope throwScope(CTHROW_PRINT);

  if (! xml.read(file_, &tag) || ! tag)
    return false;
  }

  if (tag->getName() != "COLLADA")
    return false;

  //---

  // handle options
  const auto &options = tag->getOptions();

  auto num_options = tag->getNumOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name = option->getName ();

    if      (opt_name == "xmlns") {
      SKIP(opt_name);
    }
    else if (opt_name == "version") {
      SKIP(opt_name);
    }
    else if (opt_name == "xmlns:xsi") {
      SKIP(opt_name);
    }
    else
      errorMsg("Unrecognised option '" + opt_name + "'");
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

    auto num_options1 = tag1->getNumOptions();

    if      (name1 == "asset") {
      readAssert(name1, tag1);
    }
    else if (name1 == "library_images") {
      readLibraryImages(name1, tag1);
    }
    else if (name1 == "library_materials") {
      readLibraryMaterials(name1, tag1);
    }
    else if (name1 == "library_effects") {
      readLibraryEffects(name1, tag1);
    }
    else if (name1 == "library_geometries") {
      readLibraryGeometries(name1, tag1);
    }
    else if (name1 == "library_controllers") {
      readLibraryControllers(name1, tag1);
    }
    else if (name1 == "library_animations") {
      readLibraryAnimations(name1, tag1);
    }
    else if (name1 == "library_visual_scenes") {
      readLibraryVisualScenes(name1, tag1);
    }
    else if (name1 == "library_cameras") {
      TODO(name1);
    }
    else if (name1 == "library_lights") {
      TODO(name1);
    }
    else if (name1 == "scene") {
      assert(num_options1 == 0);

      Scene scene;

      readScene(name1, tag1, scene);
    }
    else
      errorMsg("Unrecognised tag '" + name1 + "'");
  }

  //---

  delete tag;

  //---

  if (isDebug()) {
    for (const auto &pl : idLibraryEffect_) {
      pl.second->print();
    }
  }

  //---

  if      (! meshPolyLists_.empty()) {
    std::string diffuseFile, normalFile, emissionFile, specularFile;

    for (const auto &polyList : meshPolyLists_) {
      auto *effect = getMaterialEffect(polyList->material);

      if (effect) {
        diffuseFile  = getEffectTexture(effect, effect->diffuseTexture .texture);
        normalFile   = getEffectTexture(effect, effect->normalTexture  .texture);
        emissionFile = getEffectTexture(effect, effect->emissionTexture.texture);
        specularFile = getEffectTexture(effect, effect->specularTexture.texture);

        if (isDebug()) {
          std::cerr << "Diffuse  file: " << diffuseFile  << "\n";
          std::cerr << "Normal   file: " << normalFile   << "\n";
          std::cerr << "Emission file: " << emissionFile << "\n";
          std::cerr << "Specular file: " << specularFile << "\n";
        }
      }

      MeshSource *vertexMesh   = nullptr;
      MeshSource *normalMesh   = nullptr;
      MeshSource *texCoordMesh = nullptr;

      for (const auto &input : polyList->inputs) {
        if      (input.type == MeshPolyListInputType::VERTEX) {
          if (vertexMesh)
            continue;

          auto pv = idMeshVertices_.find(input.source);

          if (pv == idMeshVertices_.end()) {
            std::cerr << "Vertices not found for vertex input\n";
            continue;
          }

          const auto &meshVertices = (*pv).second;

          for (const auto &input1 : meshVertices->inputs) {
            if (input1.type == MeshVerticesInputType::POSITION) {
              auto pv1 = idMeshSource_.find(input1.source);

              if (pv1 == idMeshSource_.end()) {
                std::cerr << "source not found for vertex input\n";
                continue;
              }

              vertexMesh = (*pv1).second.get();

              break;
            }
          }
        }
        else if (input.type == MeshPolyListInputType::NORMAL) {
          if (normalMesh)
            continue;

          auto pv1 = idMeshSource_.find(input.source);

          if (pv1 == idMeshSource_.end()) {
            std::cerr << "source not found for vertex normal\n";
            continue;
          }

          normalMesh = (*pv1).second.get();
        }
        else if (input.type == MeshPolyListInputType::TEXCOORD) {
          if (texCoordMesh)
            continue;

          auto pv1 = idMeshSource_.find(input.source);

          if (pv1 == idMeshSource_.end()) {
            std::cerr << "source not found for vertex tex coord\n";
            continue;
          }

          texCoordMesh = (*pv1).second.get();
        }
      }

      if (! vertexMesh)
        continue;

      //---

      auto *object1 = CGeometry3DInst->createObject3D(scene_, "mesh");

      scene_->addObject(object1);

      pobject_->addChild(object1);

      //---

      std::vector<CPoint3D> points;

      auto nv = vertexMesh->valueArray.values.size();
      auto np = nv/3;

      points.resize(np);

      for (uint i = 0; i < np; ++i) {
        auto p = CPoint3D(vertexMesh->valueArray.values[3*i    ],
                          vertexMesh->valueArray.values[3*i + 1],
                          vertexMesh->valueArray.values[3*i + 2]);

        points[i] = p;
      }

      //---

      std::vector<CVector3D> normals;

      if (normalMesh) {
        auto nnv = normalMesh->valueArray.values.size();
        auto nn  = nnv/3;

        normals.resize(nn);

        for (uint i = 0; i < nn; ++i) {
          auto v = CVector3D(normalMesh->valueArray.values[3*i    ],
                             normalMesh->valueArray.values[3*i + 1],
                             normalMesh->valueArray.values[3*i + 2]);

          normals[i] = v;
        }
      }

      auto nn = normals.size();

      //---

      std::vector<CPoint2D> texCoords;

      if (texCoordMesh) {
        auto ntv = texCoordMesh->valueArray.values.size();
        auto nt  = ntv/2;

        texCoords.resize(nt);

        for (uint i = 0; i < nt; ++i) {
          auto p = CPoint2D(texCoordMesh->valueArray.values[2*i    ],
                            texCoordMesh->valueArray.values[2*i + 1]);

          texCoords[i] = p;
        }
      }

      auto nt = texCoords.size();

      //---

      for (uint i = 0; i < np; ++i)
        object1->addVertex(points[i]);

      //---

      int ip  = 0;
      int npl = int(polyList->p.size());
      int iv  = 0;

      while (ip < npl) {
        std::vector<uint> inds, vinds, ninds, tinds;

        auto vcount = polyList->vcount[iv++];

        for (int iv1 = 0; iv1 < vcount; ++iv1) {
          for (const auto &input : polyList->inputs) {
            auto ind = polyList->p[ip++];

            inds.push_back(ind);

            if      (input.type == MeshPolyListInputType::VERTEX)
              vinds.push_back(ind);
            else if (input.type == MeshPolyListInputType::NORMAL)
              ninds.push_back(ind);
            else if (input.type == MeshPolyListInputType::TEXCOORD) {
              if (input.set == 0)
                tinds.push_back(ind);
            }
          }
        }

        auto faceId = object1->addIPolygon(&vinds[0], int(vinds.size()));

        auto *face = object1->getFaceP(faceId);

        if (normalMesh) {
          std::vector<CVector3D> normals1;

          for (auto &i : ninds) {
            if (i < nn)
              normals1.push_back(normals[i]);
          }

          if (! normals1.empty())
            face->setNormals(normals1);
        }

        if (texCoordMesh) {
          std::vector<CPoint2D> texCoords1;

          for (auto &i : tinds) {
            if (i < nt)
              texCoords1.push_back(texCoords[i]);
          }

          if (! texCoords1.empty())
            face->setTexturePoints(texCoords1);
        }
      }

      //---

      CGeomMaterial *material;

      if (diffuseFile != "" || normalFile != "" || emissionFile != "" || specularFile != "") {
        material = CGeometry3DInst->createMaterial();

        material->setName("material");

        object1->setMaterialP(material);

        scene_->addMaterial(material);
      }

      auto addTexture = [&](const std::string &filename, const std::string &name) {
        CFile cfile(filename);

        CImageFileSrc src(cfile);

        auto image = CImageMgrInst->createImage(src);

        image->flipH();

        auto *texture = CGeometry3DInst->createTexture(image);

        texture->setName(name);

        scene_->addTexture (texture);

        return texture;
      };

      if (diffuseFile != "") {
        auto *texture = addTexture(diffuseFile, "diffuse");

        material->setDiffuseTexture(texture);
      }

      if (normalFile != "") {
        auto *texture = addTexture(normalFile, "normal");

        material->setNormalTexture(texture);
      }

      if (emissionFile != "") {
        auto *texture = addTexture(emissionFile, "emission");

        material->setEmissiveTexture(texture);
      }

      if (specularFile != "") {
        auto *texture = addTexture(specularFile, "specular");

        material->setSpecularTexture(texture);
      }
    }
  }
  else if (! meshTrianglesArray_.empty()) {
    for (const auto &triangles : meshTrianglesArray_) {
      MeshSource *vertexMesh = nullptr;

      for (const auto &input : triangles->inputs) {
        if (input.type == MeshPolyListInputType::VERTEX) {
          auto pv = idMeshVertices_.find(input.source);

          if (pv == idMeshVertices_.end()) {
            std::cerr << "Vertices not found for vertex input\n";
            continue;
          }

          const auto &meshVertices = (*pv).second;

          for (const auto &input1 : meshVertices->inputs) {
            if (input1.type == MeshVerticesInputType::POSITION) {
              auto pv1 = idMeshSource_.find(input1.source);

              if (pv1 == idMeshSource_.end()) {
                std::cerr << "source not found for vertex input\n";
                continue;
              }

              vertexMesh = (*pv1).second.get();

              break;
            }
          }

          if (! vertexMesh)
            break;
        }
      }

      if (! vertexMesh)
        continue;

      //---

      auto *object1 = CGeometry3DInst->createObject3D(scene_, "mesh");

      scene_->addObject(object1);

      pobject_->addChild(object1);

      //---

      int nv = int(vertexMesh->valueArray.values.size());

      for (int i = 0; i < nv; i += 3) {
        auto p = CPoint3D(vertexMesh->valueArray.values[i],
                          vertexMesh->valueArray.values[i + 1],
                          vertexMesh->valueArray.values[i + 2]);

        object1->addVertex(p);
      }

      //---

      int ip  = 0;
      int npl = int(triangles->p.size());

      while (ip < npl) {
        std::vector<uint> inds, vinds;

        for (int iv1 = 0; iv1 < 3; ++iv1) {
          for (const auto &input : triangles->inputs) {
            auto ind = triangles->p[ip++];

            inds.push_back(ind);

            if (input.type == MeshPolyListInputType::VERTEX)
              vinds.push_back(ind);
          }
        }

        object1->addIPolygon(&vinds[0], int(vinds.size()));
      }
    }
  }

  return true;
}

CImportDAE::LibraryEffect *
CImportDAE::
getMaterialEffect(const std::string &name) const
{
  auto pm = idMaterialNameMap_.find(name);

  if (pm == idMaterialNameMap_.end())
    return nullptr;

//std::cerr << "Material: " << (*pm).second << "\n";

  auto pm1 = idMaterial_.find((*pm).second);

  if (pm1 == idMaterial_.end())
    return nullptr;

  auto *material = (*pm1).second.get();

  if (! material)
    return nullptr;

  //std::cerr << "Material Id: " << material->id << "\n";

  auto effectId = (! material->effects.empty() ? material->effects[0] : "");

  auto pe = idLibraryEffect_.find(effectId);

  if (pe == idLibraryEffect_.end())
    return nullptr;

  auto &effect = (*pe).second;

  return effect.get();
}

std::string
CImportDAE::
getEffectTexture(LibraryEffect *effect, const std::string &texture)
{
  auto ps = effect->samplers.find(texture);

  if (ps == effect->samplers.end())
    return "";

  //std::cerr << "Sampler Src: " << (*ps).second.source << "\n";

  auto ps1 = effect->surfaces.find((*ps).second.source);

  if (ps1 == effect->surfaces.end())
    return "";

  //std::cerr << "Surface From: " << (*ps1).second.from << "\n";

  auto pi = idImage_.find((*ps1).second.from);

  if (pi == idImage_.end())
    return "";

  auto filename = (*pi).second->from;

  //std::cerr << "Image From: " << filename << "\n";

  return filename;
}

bool
CImportDAE::
readAssert(const std::string &parentName, CXMLTag *tag)
{
  auto num_options = tag->getNumOptions();

  assert(num_options == 0);

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options1 = tag1->getNumOptions();

    if      (name == "contributor") {
      assert(num_options1 == 0);

      SKIP(parentName + ":" + name);
    }
    else if (name == "created") {
      assert(num_options1 == 0);

      SKIP(parentName + ":" + name);
    }
    else if (name == "modified") {
      assert(num_options1 == 0);

      SKIP(parentName + ":" + name);
    }
    else if (name == "revision") {
      assert(num_options1 == 0);

      SKIP(parentName + ":" + name);
    }
    else if (name == "title") {
      assert(num_options1 == 0);

      SKIP(parentName + ":" + name);
    }
    else if (name == "unit") {
      const auto &options1 = tag1->getOptions();

      for (size_t j = 0; j < num_options1; ++j) {
        auto *option1 = options1[j];

        const auto &opt_name  = option1->getName ();
      //const auto &opt_value = option1->getValue();

        if     (opt_name == "name") {
          SKIP(parentName + "/" + name + " : " + opt_name);
        }
        else if (opt_name == "meter") {
          SKIP(parentName + "/" + name + " : " + opt_name);
        }
        else
          errorMsg("Unrecognised " + name + " option '" + opt_name + "'");
      }

      SKIP(parentName + ":" + name);
    }
    else if (name == "up_axis") {
      assert(num_options1 == 0);

      SKIP(parentName + ":" + name);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readLibraryImages(const std::string &parentName, CXMLTag *tag)
{
  auto num_options = tag->getNumOptions();

  assert(num_options == 0);

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

  //auto num_options1 = tag1->getNumOptions();

    if (name == "image") {
      auto image = std::make_unique<Image>();

      readLibraryImage(parentName + "/" + name, tag1, image);

      if (isDebug())
        image->print();

      idImage_[image->id] = std::move(image);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readLibraryMaterials(const std::string &parentName, CXMLTag *tag)
{
  auto num_options = tag->getNumOptions();

  assert(num_options == 0);

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

  //auto num_options1 = tag1->getNumOptions();

    if (name == "material") {
      auto material = std::make_unique<Material>();

      readLibraryMaterial(parentName + "/" + name, tag1, material);

      if (isDebug())
        material->print();

      idMaterial_[material->id] = std::move(material);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readLibraryEffects(const std::string &parentName, CXMLTag *tag)
{
  auto num_options = tag->getNumOptions();

  assert(num_options == 0);

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

  //auto num_options1 = tag1->getNumOptions();

    if (name == "effect") {
      auto effect = std::make_unique<LibraryEffect>();

      readLibraryEffect(parentName + "/" + name, tag1, effect);

      idLibraryEffect_[effect->id] = std::move(effect);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readLibraryImage(const std::string &parentName, CXMLTag *tag, ImageP &image)
{
  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if      (opt_name == "id") {
      image->id = opt_value;
    }
    else if (opt_name == "name") {
      image->name = opt_value;
    }
    else if (opt_name == "depth") {
      if (! CStrUtil::toInteger(opt_value, &image->depth))
        errorMsg("Invalid integer '" + opt_value + "'");
    }
    else
      errorMsg("Unrecognised " + parentName + " option '" + opt_name + "'");
  }

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options1 = tag1->getNumOptions();

    //const auto &options1 = tag1->getOptions();

    if (name == "init_from") {
      assert(num_options1 == 0);

      const auto &value = tag1->getText();

      image->from = value;

      assert(tag1->getNumTagChildren() == 0);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readLibraryMaterial(const std::string &parentName, CXMLTag *tag, MaterialP &material)
{
  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if      (opt_name == "id") {
      material->id = opt_value;
    }
    else if (opt_name == "name") {
      material->name = opt_value;
    }
    else
      errorMsg("Unrecognised " + parentName + " option '" + opt_name + "'");
  }

  //---

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

    if (name == "instance_effect") {
      for (size_t j = 0; j < num_options1; ++j) {
        auto *option1 = options1[j];

        const auto &opt_name  = option1->getName ();
        const auto &opt_value = option1->getValue();

        if (opt_name == "url") {
          if (opt_value[0] == '#')
            material->effects.push_back(opt_value.substr(1));
          else
            std::cerr << "Invalid url value '" << opt_value << "'\n";
        }
        else
          errorMsg("Unrecognised " + name + " option '" + opt_name + "'");
      }

      assert(tag1->getNumChildren() == 0);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readLibraryEffect(const std::string &parentName, CXMLTag *tag, LibraryEffectP &effect)
{
  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if      (opt_name == "id") {
      effect->id = opt_value;
    }
    else if (opt_name == "name") {
      effect->name = opt_value;
    }
    else
      errorMsg("Unrecognised " + parentName + " option '" + opt_name + "'");
  }

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    //auto num_options1 = tag1->getNumOptions();

    if (name == "profile_COMMON") {
      readLibraryEffectProfileCommon(parentName + "/" + name, tag1, effect);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readLibraryEffectProfileCommon(const std::string &parentName, CXMLTag *tag, LibraryEffectP &effect)
{
  auto num_options = tag->getNumOptions();

  assert(num_options == 0);

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

  //auto num_options1 = tag1->getNumOptions();

  //const auto &options1 = tag1->getOptions();

    if      (name == "newparam") {
      NewParam newParam;

      readLibraryEffectProfileParam(parentName + "/" + name, tag1, newParam, effect);
    }
    else if (name == "technique") {
      readLibraryEffectProfileTechnique(parentName + "/" + name, tag1, effect);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readLibraryEffectProfileParam(const std::string &parentName, CXMLTag *tag, NewParam &param,
                              LibraryEffectP &effect)
{
  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if      (opt_name == "sid") {
      param.sid = opt_value;
    }
    else
      errorMsg("Unrecognised " + parentName + " option '" + opt_name + "'");
  }

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options1 = tag1->getNumOptions();

    if      (name == "surface") {
      Surface surface;

      readLibraryEffectProfileSurface(parentName + "/" + name, tag1, surface);

      effect->surfaces[param.sid] = surface;
    }
    else if (name == "sampler2D") {
      assert(num_options1 == 0);

      Sampler2D sampler2D;

      readLibraryEffectSampler2D(parentName + "/" + name, tag1, param, sampler2D);

      effect->samplers[param.sid] = sampler2D;
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readLibraryEffectProfileTechnique(const std::string &parentName, CXMLTag *tag,
                                  LibraryEffectP &effect)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options1 = tag1->getNumOptions();

    //const auto &options1 = tag1->getOptions();

    if      (name == "phong") {
      assert(num_options1 == 0);

      readLibraryEffectProfilePhong(parentName + "/" + name, tag1, effect);
    }
    else if (name == "blinn") {
      TODO(parentName + "/" + name);
    }
    else if (name == "lambert") {
      TODO(parentName + "/" + name);
    }
    else if (name == "extra") {
      readLibraryEffectProfileExtra(parentName + "/" + name, tag1, effect);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readLibraryEffectProfilePhong(const std::string &parentName, CXMLTag *tag,
                              LibraryEffectP &effect)
{
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

    if      (name == "ambient") {
      assert(num_options1 == 0);

      readAmbient(parentName + "/" + name, tag1);
    }
    else if (name == "diffuse") {
      assert(num_options1 == 0);

      readDiffuse(parentName + "/" + name, tag1, effect);
    }
    else if (name == "emission") {
      assert(num_options1 == 0);

      readEmission(parentName + "/" + name, tag1, effect);
    }
    else if (name == "specular") {
      assert(num_options1 == 0);

      readSpecular(parentName + "/" + name, tag1, effect);
    }
    else if (name == "shininess") {
      assert(num_options1 == 0);

      readShininess(parentName + "/" + name, tag1);
    }
    else if (name == "reflective") {
      assert(num_options1 == 0);

      readReflective(parentName + "/" + name, tag1);
    }
    else if (name == "reflectivity") {
      assert(num_options1 == 0);

      readReflectivity(parentName + "/" + name, tag1);
    }
    else if (name == "transparent") {
      for (size_t j = 0; j < num_options1; ++j) {
        auto *option1 = options1[j];

        const auto &opt_name  = option1->getName ();
        const auto &opt_value = option1->getValue();

        if      (opt_name == "opaque") {
          effect->transparentOpaque = opt_value;
        }
        else
          errorMsg("Unrecognised " + name + " option '" + opt_name + "'");
      }

      readTransparent(parentName + "/" + name, tag1, effect);
    }
    else if (name == "transparency") {
      assert(num_options1 == 0);

      readTransparency(parentName + "/" + name, tag1);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readLibraryEffectProfileExtra(const std::string &parentName, CXMLTag *tag,
                              LibraryEffectP &effect)
{
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

    if (name == "technique") {
      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "profile") {
          effect->technique_profile = opt_value;
        }
        else
          errorMsg("Unrecognised " + name + " option '" + opt_name + "'");
      }

      readLibraryEffectProfileTechExtra(parentName + "/" + name, tag1, effect);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readLibraryEffectProfileTechExtra(const std::string &parentName, CXMLTag *tag,
                                  LibraryEffectP &effect)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options1 = tag1->getNumOptions();

    //const auto &options1 = tag1->getOptions();

    if      (name == "displacement") {
      assert(num_options1 == 0);

      readLibraryEffectProfileDisplacement(parentName + "/" + name, tag1, effect);
    }
    else if (name == "bump") {
      TODO(parentName + "/" + name);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readLibraryEffectProfileDisplacement(const std::string &parentName, CXMLTag *tag,
                                     LibraryEffectP &effect)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

  //auto num_options1 = tag1->getNumOptions();

  //const auto &options1 = tag1->getOptions();

    if (name == "texture") {
      readTexture(parentName + "/" + name, tag1, effect->normalTexture);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readAmbient(const std::string &parentName, CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    //auto num_options1 = tag1->getNumOptions();

    //const auto &options1 = tag1->getOptions();

    if (name == "color") {
      Color c;
      readColor(parentName + "/" + name, tag1, c);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readDiffuse(const std::string &parentName, CXMLTag *tag, LibraryEffectP &effect)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

  //auto num_options1 = tag1->getNumOptions();

  //const auto &options1 = tag1->getOptions();

    if (name == "texture") {
      readTexture(parentName + "/" + name, tag1, effect->diffuseTexture);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readEmission(const std::string &parentName, CXMLTag *tag, LibraryEffectP &effect)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

  //auto num_options1 = tag1->getNumOptions();

  //const auto &options1 = tag1->getOptions();

    if (name == "texture") {
      readTexture(parentName + "/" + name, tag1, effect->emissionTexture);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readSpecular(const std::string &parentName, CXMLTag *tag, LibraryEffectP &effect)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

  //auto num_options1 = tag1->getNumOptions();

  //const auto &options1 = tag1->getOptions();

    if (name == "texture") {
      readTexture(parentName + "/" + name, tag1, effect->specularTexture);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readShininess(const std::string &parentName, CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    //auto num_options1 = tag1->getNumOptions();

    //const auto &options1 = tag1->getOptions();

    if (name == "float") {
      Float f;
      readFloat(parentName + "/" + name, tag1, f);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readReflective(const std::string &parentName, CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    //auto num_options1 = tag1->getNumOptions();

    //const auto &options1 = tag1->getOptions();

    if (name == "color") {
      Color c;
      readColor(parentName + "/" + name, tag1, c);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readReflectivity(const std::string &parentName, CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    //auto num_options1 = tag1->getNumOptions();

    //const auto &options1 = tag1->getOptions();

    if (name == "float") {
      Float f;
      readFloat(parentName + "/" + name, tag1, f);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readTransparent(const std::string &parentName, CXMLTag *tag, LibraryEffectP &effect)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    //auto num_options1 = tag1->getNumOptions();

    //const auto &options1 = tag1->getOptions();

    if (name == "texture") {
      readTexture(parentName + "/" + name, tag1, effect->transparentTexture);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readTexture(const std::string &parentName, CXMLTag *tag, Texture &texture)
{
  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if      (opt_name == "texture") {
      texture.texture = opt_value;
    }
    else if (opt_name == "texcoord") {
      texture.texcoord = opt_value;
    }
    else
      errorMsg("Unrecognised " + parentName + " option '" + opt_name + "'");
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

    //auto num_options1 = tag1->getNumOptions();

    //const auto &options1 = tag1->getOptions();

    if (name1 == "extra") {
      readTextureExtra(parentName + "/" + name1, tag1, texture);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name1 + "'");
  }

  return true;
}

bool
CImportDAE::
readTextureExtra(const std::string &parentName, CXMLTag *tag, Texture &texture)
{
  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if (opt_name == "type") {
      texture.extra_type = opt_value;
    }
    else
      errorMsg("Unrecognised " + parentName + " option '" + opt_name + "'");
  }

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    //auto num_options1 = tag1->getNumOptions();

    //const auto &options1 = tag1->getOptions();

    if (name == "technique") {
      readTextureTechnique(parentName + "/" + name, tag1, texture);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readTextureTechnique(const std::string &parentName, CXMLTag *tag, Texture &texture)
{
  TextureTechnique technique;

  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if (opt_name == "profile") {
      technique.profile = opt_value;
    }
    else
      errorMsg("Unrecognised " + parentName + " option '" + opt_name + "'");
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

    if      (name1 == "wrapU") {
      readTextureWrapU(parentName + "/" + name1, tag1, texture);
    }
    else if (name1 == "wrapV") {
      readTextureWrapV(parentName + "/" + name1, tag1, texture);
    }
    else if (name1 == "blend_mode") {
      readTextureBlendMode(parentName + "/" + name1, tag1, texture);
    }
    else if (name1 == "offsets") {
      readTextureOffsets(parentName + "/" + name1, tag1, texture);
    }
    else if (name1 == "amount") {
      readTextureAmount(parentName + "/" + name1, tag1, texture);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name1 + "'");
  }

  return true;
}

bool
CImportDAE::
readTextureWrapU(const std::string &parentName, CXMLTag *tag, Texture & /*texture*/)
{
  TextureWrap wrap;

  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if (opt_name == "sid") {
      wrap.sid = opt_value;
    }
    else
      errorMsg("Unrecognised " + parentName + " option '" + opt_name + "'");
  }

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readTextureWrapV(const std::string &parentName, CXMLTag *tag, Texture & /*texture*/)
{
  TextureWrap wrap;

  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if (opt_name == "sid") {
      wrap.sid = opt_value;
    }
    else
      errorMsg("Unrecognised " + parentName + " option '" + opt_name + "'");
  }

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readTextureBlendMode(const std::string &parentName, CXMLTag *tag, Texture & /*texture*/)
{
  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();

    errorMsg("Unrecognised " + parentName + " option '" + opt_name + "'");
  }

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readTextureOffsets(const std::string &parentName, CXMLTag *tag, Texture &texture)
{
  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();

    errorMsg("Unrecognised " + parentName + " option '" + opt_name + "'");
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

    auto num_options1 = tag1->getNumOptions();

    if      (name1 == "offsetu") {
      assert(num_options1 == 0);

      const auto &value = tag1->getText();

      if (! CStrUtil::toReal(value, &texture.offsetU))
        errorMsg("Invalid real '" + value + "'");

      assert(tag1->getNumTagChildren() == 0);
    }
    else if (name1 == "offsetv") {
      assert(num_options1 == 0);

      const auto &value = tag1->getText();

      if (! CStrUtil::toReal(value, &texture.offsetV))
        errorMsg("Invalid real '" + value + "'");

      assert(tag1->getNumTagChildren() == 0);
    }
    else if (name1 == "scaleu") {
      assert(num_options1 == 0);

      const auto &value = tag1->getText();

      if (! CStrUtil::toReal(value, &texture.scaleU))
        errorMsg("Invalid real '" + value + "'");

      assert(tag1->getNumTagChildren() == 0);
    }
    else if (name1 == "scalev") {
      assert(num_options1 == 0);

      const auto &value = tag1->getText();

      if (! CStrUtil::toReal(value, &texture.scaleV))
        errorMsg("Invalid real '" + value + "'");

      assert(tag1->getNumTagChildren() == 0);
    }
    else if (name1 == "angle") {
      assert(num_options1 == 0);

      const auto &value = tag1->getText();

      if (! CStrUtil::toReal(value, &texture.angle))
        errorMsg("Invalid real '" + value + "'");

      assert(tag1->getNumTagChildren() == 0);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name1 + "'");
  }

  return true;
}

bool
CImportDAE::
readTextureAmount(const std::string &parentName, CXMLTag *tag, Texture & /*texture*/)
{
  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();

    errorMsg("Unrecognised " + parentName + " option '" + opt_name + "'");
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

    errorMsg("Unrecognised tag '" + parentName + "/" + name1 + "'");
  }

  return true;
}

bool
CImportDAE::
readTransparency(const std::string &parentName, CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name1 = tag1->getName();

    //auto num_options1 = tag1->getNumOptions();

    //const auto &options1 = tag1->getOptions();

    if (name1 == "float") {
      Float f;
      readFloat(parentName + "/" + name1, tag1, f);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name1 + "'");
  }

  return true;
}

bool
CImportDAE::
readLibraryEffectProfileSurface(const std::string &parentName, CXMLTag *tag, Surface &surface)
{
  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if      (opt_name == "type") {
      surface.type = opt_value;
    }
    else
      errorMsg("Unrecognised " + parentName + " option '" + opt_name + "'");
  }

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options1 = tag1->getNumOptions();

    //const auto &options1 = tag1->getOptions();

    if      (name == "init_from") {
      assert(num_options1 == 0);

      const auto &value = tag1->getText();

      surface.from = value;

      assert(tag1->getNumTagChildren() == 0);
    }
    else if (name == "format") {
      TODO(parentName + "/" + name);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readLibraryEffectSampler2D(const std::string &parentName, CXMLTag *tag, NewParam &param,
                           Sampler2D &sampler)
{
  sampler.sid = param.sid;

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options1 = tag1->getNumOptions();

    //const auto &options1 = tag1->getOptions();

    if      (name == "source") {
      assert(num_options1 == 0);

      sampler.source = tag1->getText();

      assert(tag1->getNumTagChildren() == 0);
    }
    else if (name == "minfilter") {
      assert(num_options1 == 0);

      sampler.minfilter = tag1->getText();

      assert(tag1->getNumTagChildren() == 0);
    }
    else if (name == "magfilter") {
      assert(num_options1 == 0);

      sampler.magfilter = tag1->getText();

      assert(tag1->getNumTagChildren() == 0);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readLibraryGeometries(const std::string &parentName, CXMLTag *tag)
{
  auto num_options = tag->getNumOptions();

  assert(num_options == 0);

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

  //auto num_options1 = tag1->getNumOptions();

    if (name == "geometry") {
      readGeometry(parentName + "/" + name, tag1);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readGeometry(const std::string &parentName, CXMLTag *tag)
{
  Geometry geometry;

  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if      (opt_name == "id") {
      geometry.id = opt_value;
    }
    else if (opt_name == "name") {
      geometry.name = opt_value;
    }
    else
      errorMsg("Unrecognised " + parentName + " option '" + opt_name + "'");
  }

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options1 = tag1->getNumOptions();

    //const auto &options1 = tag1->getOptions();

    if (name == "mesh") {
      assert(num_options1 == 0);

      readMesh(parentName + "/" + name, tag1);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readMesh(const std::string &parentName, CXMLTag *tag)
{
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

    if      (name == "source") {
      auto meshSource = std::make_unique<MeshSource>();

      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "id") {
          meshSource->id = opt_value;
        }
        else if (opt_name == "name") {
          meshSource->name = opt_value;
        }
        else
          errorMsg("Unrecognised " + name + " option '" + opt_name + "'");
      }

      readMeshSource(parentName + "/" + name, tag1, meshSource);

      if (isDebug())
        meshSource->print();

      idMeshSource_[meshSource->id] = std::move(meshSource);
    }
    else if (name == "vertices") {
      auto meshVertices = std::make_unique<MeshVertices>();

      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if (opt_name == "id") {
          meshVertices->id = opt_value;
        }
        else
          errorMsg("Unrecognised " + name + " option '" + opt_name + "'");
      }

      readMeshVertices(parentName + "/" + name, tag1, meshVertices);

      if (isDebug())
        meshVertices->print();

      idMeshVertices_[meshVertices->id] = std::move(meshVertices);
    }
    else if (name == "triangles") {
      auto triangles = std::make_unique<MeshTriangles>();

      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "material") {
          triangles->material = opt_value;
        }
        else if (opt_name == "count") {
          if (! CStrUtil::toInteger(opt_value, &triangles->count))
            errorMsg("Invalid integer '" + opt_value + "'");
        }
        else
          errorMsg("Unrecognised " + name + " option '" + opt_name + "'");
      }

      readMeshTriangles(parentName + "/" + name, tag1, triangles);

      if (isDebug())
        triangles->print();

      meshTrianglesArray_.push_back(std::move(triangles));
    }
    else if (name == "polygons") {
      TODO(parentName + "/" + name);
    }
    else if (name == "polylist") {
      auto polyList = std::make_unique<MeshPolyList>();

      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "material") {
          polyList->material = opt_value;
        }
        else if (opt_name == "count") {
          if (! CStrUtil::toInteger(opt_value, &polyList->count))
            errorMsg("Invalid integer '" + opt_value + "'");
        }
        else
          errorMsg("Unrecognised " + name + " option '" + opt_name + "'");
      }

      readMeshPolylist(parentName + "/" + name, tag1, polyList);

      if (isDebug())
        polyList->print();

      meshPolyLists_.push_back(std::move(polyList));
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readMeshSource(const std::string &parentName, CXMLTag *tag, MeshSourceP &meshSource)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

  //auto num_options1 = tag1->getNumOptions();

  //const auto &options1 = tag1->getOptions();

    if      (name == "technique_common") {
      readTechniqueCommon(parentName + "/" + name, tag1, meshSource->techniqueCommon);
    }
    else if (name == "float_array") {
      readFloatArray(parentName + "/" + name, tag1, meshSource->valueArray);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readMeshVertices(const std::string &parentName, CXMLTag *tag, MeshVerticesP &meshVertices)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

  //auto num_options1 = tag1->getNumOptions();

    if (name == "input") {
      MeshVerticesInput input;
      readInput(parentName + "/" + name, tag1, input);

      if      (input.semantic == "POSITION")
        input.type = MeshVerticesInputType::POSITION;
      else if (input.semantic == "NORMAL")
        input.type = MeshVerticesInputType::NORMAL;
      else
        std::cerr << "Invalid mesh vertices semantic '" << input.semantic << "'\n";

      meshVertices->inputs.push_back(input);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readMeshPolylist(const std::string &parentName, CXMLTag *tag, MeshPolyListP &polyList)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options1 = tag1->getNumOptions();

    if      (name == "input") {
      MeshPolyListInput input;
      readInput(parentName + "/" + name, tag1, input);

      if      (input.semantic == "VERTEX")
        input.type = MeshPolyListInputType::VERTEX;
      else if (input.semantic == "NORMAL")
        input.type = MeshPolyListInputType::NORMAL;
      else if (input.semantic == "TEXCOORD")
        input.type = MeshPolyListInputType::TEXCOORD;
      else if (input.semantic == "COLOR")
        input.type = MeshPolyListInputType::COLOR;
      else
        std::cerr << "Invalid mesh polylist semantic '" << input.semantic << "'\n";

      polyList->max_set    = std::max(polyList->max_set   , input.set);
      polyList->max_offset = std::max(polyList->max_offset, input.offset);

      polyList->inputs.push_back(input);
    }
    else if (name == "vcount") {
      assert(num_options1 == 0);

      const auto &value = tag1->getText();

      std::vector<std::string> words;
      CStrUtil::toWords(value, words);

      for (const auto &word : words) {
        int ii;
        if (! CStrUtil::toInteger(word, &ii))
          errorMsg("Invalid integer '" + word + "'");

        polyList->vcount.push_back(ii);

        polyList->vsum += ii;
      }

      assert(tag1->getNumTagChildren() == 0);
    }
    else if (name == "p") {
      assert(num_options1 == 0);

      const auto &value = tag1->getText();

      std::vector<std::string> words;
      CStrUtil::toWords(value, words);

      std::vector<int> ints;

      for (const auto &word : words) {
        int ii;
        if (! CStrUtil::toInteger(word, &ii))
          errorMsg("Invalid integer '" + word + "'");

        polyList->p.push_back(ii);
      }

      assert(tag1->getNumTagChildren() == 0);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readMeshTriangles(const std::string &parentName, CXMLTag *tag, MeshTrianglesP &triangles)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options1 = tag1->getNumOptions();

    if      (name == "input") {
      MeshPolyListInput input;
      readInput(parentName + "/" + name, tag1, input);

      if      (input.semantic == "VERTEX")
        input.type = MeshPolyListInputType::VERTEX;
      else if (input.semantic == "NORMAL")
        input.type = MeshPolyListInputType::NORMAL;
      else if (input.semantic == "TEXCOORD")
        input.type = MeshPolyListInputType::TEXCOORD;
      else if (input.semantic == "COLOR")
        input.type = MeshPolyListInputType::COLOR;
      else
        std::cerr << "Invalid mesh polylist semantic '" << input.semantic << "'\n";

      triangles->max_offset = std::max(triangles->max_offset, input.offset);
      triangles->max_set    = std::max(triangles->max_set   , input.set);

      triangles->inputs.push_back(input);
    }
    else if (name == "p") {
      assert(num_options1 == 0);

      const auto &value = tag1->getText();

      std::vector<std::string> words;
      CStrUtil::toWords(value, words);

      std::vector<int> ints;

      for (const auto &word : words) {
        int ii;
        if (! CStrUtil::toInteger(word, &ii))
          errorMsg("Invalid integer '" + word + "'");

        triangles->p.push_back(ii);
      }

      assert(tag1->getNumTagChildren() == 0);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readTechniqueCommon(const std::string &parentName, CXMLTag *tag, TechniqueCommon &techniqueCommon)
{
  auto num_options = tag->getNumOptions();
  assert(num_options == 0);

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

    if (name == "accessor") {
      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "count") {
          if (! CStrUtil::toInteger(opt_value, &techniqueCommon.accessorCount))
            errorMsg("Invalid integer '" + opt_value + "'");
        }
        else if (opt_name == "offset") {
          if (! CStrUtil::toInteger(opt_value, &techniqueCommon.accessorOffset))
            errorMsg("Invalid integer '" + opt_value + "'");
        }
        else if (opt_name == "source") {
          techniqueCommon.source = opt_value;
        }
        else if (opt_name == "stride") {
          if (! CStrUtil::toInteger(opt_value, &techniqueCommon.accessorStride))
            errorMsg("Invalid integer '" + opt_value + "'");
        }
        else
          errorMsg("Unrecognised " + name + " option '" + opt_name + "'");
      }

      readTechniqueCommonAccessor(parentName + "/" + name, tag1, techniqueCommon);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readTechniqueCommonAccessor(const std::string &parentName, CXMLTag *tag,
                            TechniqueCommon &techniqueCommon)
{
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

    if (name == "param") {
      Param param;

      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "name") {
          param.name = opt_value;
        }
        else if (opt_name == "type") {
          param.type = opt_value;
        }
        else
          errorMsg("Unrecognised " + name + " option '" + opt_name + "'");
      }

      assert(tag1->getNumChildren() == 0);

      techniqueCommon.params.push_back(param);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readLibraryControllers(const std::string &parentName, CXMLTag *tag)
{
  auto num_options = tag->getNumOptions();

  assert(num_options == 0);

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    if (name == "controller") {
      readLibraryController(parentName + "/" + name, tag1);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readLibraryController(const std::string &parentName, CXMLTag *tag)
{
  LibraryController controller;

  //---

  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if      (opt_name == "id") {
      controller.id = opt_value;
    }
    else if (opt_name == "name") {
      controller.name = opt_value;
    }
    else
      errorMsg("Unrecognised " + parentName + " option '" + opt_name + "'");
  }

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

  //auto num_options1 = tag1->getNumOptions();

    if (name == "skin") {
      Skin skin;
      readLibraryControllerSkin(parentName + "/" + name, tag1, skin);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readLibraryControllerSkin(const std::string &parentName, CXMLTag *tag, Skin &skin)
{
  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if (opt_name == "source") {
      skin.source = opt_value;
    }
    else
      errorMsg("Unrecognised " + parentName + " option '" + opt_name + "'");
  }

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options1 = tag1->getNumOptions();

    if      (name == "bind_shape_matrix") {
      assert(num_options1 == 0);

      const auto &value = tag1->getText();

      std::vector<std::string> words;
      CStrUtil::toWords(value, words);

      std::vector<double> reals;

      for (const auto &word : words) {
        double r;
        if (! CStrUtil::toReal(word, &r))
          errorMsg("Invalid real '" + word + "'");

        reals.push_back(r);
      }

      if (reals.size() == 16)
        skin.bind_shape_matrix = CMatrix3D(&reals[0], 16);
      else
        std::cerr << "Invalid matrix size\n";

      assert(tag1->getNumTagChildren() == 0);
    }
    else if (name == "source") {
      readLibraryControllerSkinSource(parentName + "/" + name, tag1, skin);
    }
    else if (name == "joints") {
      readLibraryControllerSkinJoints(parentName + "/" + name, tag1, skin);
    }
    else if (name == "vertex_weights") {
      readLibraryControllerSkinWeights(parentName + "/" + name, tag1, skin);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readLibraryControllerSkinSource(const std::string &parentName, CXMLTag *tag, Skin &skin)
{
  SkinSource source;

  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if      (opt_name == "id") {
      source.id = opt_value;
    }
    else if (opt_name == "count") {
      if (! CStrUtil::toInteger(opt_value, &source.count))
        errorMsg("Invalid integer '" + opt_value + "'");
    }
    else
      errorMsg("Unrecognised " + parentName + " option '" + opt_name + "'");
  }

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

  //auto num_options1 = tag1->getNumOptions();

    if      (name == "float_array") {
      readFloatArray(parentName + "/" + name, tag1, source.valueArray);
    }
    else if (name == "technique_common") {
      readTechniqueCommon(parentName + "/" + name, tag1, source.techniqueCommon);
    }
    else if (name == "Name_array") {
      readNameArray(parentName + "/" + name, tag1, source.nameArray);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  skin.sources.push_back(source);

  return true;
}

bool
CImportDAE::
readLibraryControllerSkinJoints(const std::string &parentName, CXMLTag *tag, Skin &skin)
{
  SkinJoint joint;

  //---

  auto num_options = tag->getNumOptions();

  assert(num_options == 0);

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

  //auto num_options1 = tag1->getNumOptions();

    if (name == "input") {
      Input input;
      readInput(parentName + "/" + name, tag1, input);

      joint.inputs.push_back(input);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  skin.joints.push_back(joint);

  return true;
}

bool
CImportDAE::
readLibraryControllerSkinWeights(const std::string &parentName, CXMLTag *tag, Skin &skin)
{
  SkinWeights weights;

  //---

  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if (opt_name == "count") {
      if (! CStrUtil::toInteger(opt_value, &weights.count))
        errorMsg("Invalid integer '" + opt_value + "'");
    }
    else
      errorMsg("Unrecognised " + parentName + " option '" + opt_name + "'");
  }

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options1 = tag1->getNumOptions();

    if      (name == "input") {
      Input input;
      readInput(parentName + "/" + name, tag1, input);

      weights.inputs.push_back(input);
    }
    else if (name == "vcount") {
      assert(num_options1 == 0);

      const auto &value = tag1->getText();

      std::vector<std::string> words;
      CStrUtil::toWords(value, words);

      for (const auto &word : words) {
        int ii;
        if (! CStrUtil::toInteger(word, &ii))
          errorMsg("Invalid integer '" + word + "'");

        weights.vcount.push_back(ii);
      }

      assert(tag1->getNumTagChildren() == 0);
    }
    else if (name == "v") {
      assert(num_options1 == 0);

      const auto &value = tag1->getText();

      std::vector<std::string> words;
      CStrUtil::toWords(value, words);

      for (const auto &word : words) {
        int ii;
        if (! CStrUtil::toInteger(word, &ii))
          errorMsg("Invalid integer '" + word + "'");

        weights.v.push_back(ii);
      }

      assert(tag1->getNumTagChildren() == 0);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  skin.weights.push_back(weights);

  return true;
}

bool
CImportDAE::
readLibraryAnimations(const std::string &parentName, CXMLTag *tag)
{
  auto num_options = tag->getNumOptions();

  assert(num_options == 0);

  //---

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

    if (name == "animation") {
      Animation animation;

      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "id") {
          animation.id = opt_value;
        }
        else if (opt_name == "name") {
          animation.name = opt_value;
        }
        else
          errorMsg("Unrecognised " + name + " option '" + opt_name + "'");
      }

      readLibraryAnimation(parentName + "/" + name, tag1, animation);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readLibraryAnimation(const std::string &parentName, CXMLTag *tag, Animation &animation)
{
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

    if      (name == "source") {
      readLibraryAnimationSource(parentName + "/" + name, tag1, animation);
    }
    else if (name == "sampler") {
      readLibraryAnimationSampler(parentName + "/" + name, tag1, animation);
    }
    else if (name == "channel") {
      AnimationChannel channel;

      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "source") {
          channel.source = opt_value;
        }
        else if (opt_name == "target") {
          channel.target = opt_value;
        }
        else
          errorMsg("Unrecognised " + name + " option '" + opt_name + "'");
      }

      assert(tag1->getNumChildren() == 0);

      animation.channels.push_back(channel);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readLibraryAnimationSource(const std::string &parentName, CXMLTag *tag, Animation &animation)
{
  AnimationSource source;

  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if      (opt_name == "id") {
      source.id = opt_value;
    }
    else
      errorMsg("Unrecognised " + parentName + " option '" + opt_name + "'");
  }

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

  //auto num_options1 = tag1->getNumOptions();

  //const auto &options1 = tag1->getOptions();

    if      (name == "float_array") {
      readFloatArray(parentName + "/" + name, tag1, source.valueArray);
    }
    else if (name == "technique_common") {
      readTechniqueCommon(parentName + "/" + name, tag1, source.techniqueCommon);
    }
    else if (name == "Name_array") {
      readNameArray(parentName + "/" + name, tag1, source.nameArray);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  animation.sources.push_back(source);

  return true;
}

bool
CImportDAE::
readLibraryAnimationSampler(const std::string &parentName, CXMLTag *tag, Animation &animation)
{
  AnimationSampler sampler;

  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if      (opt_name == "id") {
      sampler.id = opt_value;
    }
    else
      errorMsg("Unrecognised " + parentName + " option '" + opt_name + "'");
  }

  //---

  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

  //auto num_options1 = tag1->getNumOptions();

    if (name == "input") {
      Input input;
      readInput(parentName + "/" + name, tag1, input);

      sampler.inputs.push_back(input);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  animation.samplers.push_back(sampler);

  return true;
}

bool
CImportDAE::
readInput(const std::string &parentName, CXMLTag *tag, Input &input)
{
  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if      (opt_name == "semantic") {
      input.semantic = opt_value;
    }
    else if (opt_name == "source") {
      if (opt_value[0] == '#')
        input.source = opt_value.substr(1);
      else
        std::cerr << "Invalid source value '" << opt_value << "'\n";
    }
    else if (opt_name == "set") {
      if (! CStrUtil::toInteger(opt_value, &input.set))
        errorMsg("Invalid integer '" + opt_value + "'");
    }
    else if (opt_name == "offset") {
      if (! CStrUtil::toInteger(opt_value, &input.offset))
        errorMsg("Invalid integer '" + opt_value + "'");
    }
    else
      errorMsg("Unrecognised " + parentName + " option '" + opt_name + "'");
  }

  assert(tag->getNumChildren() == 0);

  return true;
}

bool
CImportDAE::
readFloatArray(const std::string &parentName, CXMLTag *tag, FloatArray &array)
{
  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t i = 0; i < num_options; ++i) {
    auto *option = options[i];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if      (opt_name == "id") {
      array.id = opt_value;
    }
    else if (opt_name == "count") {
      if (! CStrUtil::toInteger(opt_value, &array.count))
        errorMsg("Invalid integer '" + opt_value + "'");
    }
    else
      errorMsg("Unrecognised " + parentName + " option '" + opt_name + "'");
  }

  const auto &value = tag->getText();

  std::vector<std::string> words;
  CStrUtil::toWords(value, words);

  for (const auto &word : words) {
    double r;
    if (! CStrUtil::toReal(word, &r))
      std::cerr << "Invalid " << parentName << " value '" << word << "'\n";

    array.values.push_back(r);
  }

  assert(tag->getNumTagChildren() == 0);

  return true;
}

bool
CImportDAE::
readNameArray(const std::string &parentName, CXMLTag *tag, NameArray &array)
{
  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t i = 0; i < num_options; ++i) {
    auto *option = options[i];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if      (opt_name == "id") {
      array.id = opt_value;
    }
    else if (opt_name == "count") {
      if (! CStrUtil::toInteger(opt_value, &array.count))
        errorMsg("Invalid integer '" + opt_value + "'");
    }
    else
      errorMsg("Unrecognised " + parentName + " option '" + opt_name + "'");
  }

  const auto &value = tag->getText();

  std::vector<std::string> words;
  CStrUtil::toWords(value, words);

  for (const auto &word : words)
    array.values.push_back(word);

  assert(tag->getNumTagChildren() == 0);

  return true;
}

bool
CImportDAE::
readColor(const std::string &parentName, CXMLTag *tag, Color &c)
{
  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t i = 0; i < num_options; ++i) {
    auto *option = options[i];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if      (opt_name == "sid") {
      c.sid = opt_value;
    }
    else
      errorMsg("Unrecognised " + parentName + " option '" + opt_name + "'");
  }

  const auto &value = tag->getText();

  std::vector<std::string> words;
  CStrUtil::toWords(value, words);

  assert(words.size() == 4);

  if (! CStrUtil::toReal(words[0], &c.r))
    std::cerr << "Invalid " << parentName << " value '" << words[0] << "'\n";
  if (! CStrUtil::toReal(words[1], &c.g))
    std::cerr << "Invalid " << parentName << " value '" << words[1] << "'\n";
  if (! CStrUtil::toReal(words[2], &c.b))
    std::cerr << "Invalid " << parentName << " value '" << words[2] << "'\n";
  if (! CStrUtil::toReal(words[3], &c.a))
    std::cerr << "Invalid " << parentName << " value '" << words[3] << "'\n";

  assert(tag->getNumTagChildren() == 0);

  return true;
}

bool
CImportDAE::
readFloat(const std::string &parentName, CXMLTag *tag, Float &f)
{
  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t i = 0; i < num_options; ++i) {
    auto *option = options[i];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if      (opt_name == "sid") {
      f.sid = opt_value;
    }
    else
      errorMsg("Unrecognised " + parentName + " option '" + opt_name + "'");
  }

  const auto &value = tag->getText();

  if (! CStrUtil::toReal(value, &f.value))
    std::cerr << "Invalid " << parentName << " value '" << value << "'\n";

  assert(tag->getNumTagChildren() == 0);

  return true;
}

bool
CImportDAE::
readLibraryVisualScenes(const std::string &parentName, CXMLTag *tag)
{
  auto num_options = tag->getNumOptions();

  assert(num_options == 0);

  //---

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

    if (name == "visual_scene") {
      VisualScene visualScene;

      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "id") {
          visualScene.id = opt_value;
        }
        else if (opt_name == "name") {
          visualScene.name = opt_value;
        }
        else
          errorMsg("Unrecognised " + name + " option '" + opt_name + "'");
      }

      readLibraryVisualScene(parentName + "/" + name, tag1, visualScene);

      if (isDebug())
        visualScene.print();
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readLibraryVisualScene(const std::string &parentName, CXMLTag *tag, VisualScene &scene)
{
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

    if (name == "node") {
      auto *node = new Node;

      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "id") {
          node->id = opt_value;
        }
        else if (opt_name == "name") {
          node->name = opt_value;
        }
        else if (opt_name == "sid") {
          node->sid = opt_value;
        }
        else if (opt_name == "type") {
          node->type = opt_value;
        }
        else
          errorMsg("Unrecognised " + name + " option '" + opt_name + "'");
      }

      readNode(parentName + "/" + name, tag1, node);

      scene.nodes.push_back(node);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readNode(const std::string &parentName, CXMLTag *tag, Node *parentNode)
{
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

    if      (name == "matrix") {
      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if (opt_name == "sid") {
          parentNode->matrix.sid = opt_value;
        }
        else
          errorMsg("Unrecognised " + name + " option '" + opt_name + "'");
      }

      const auto &value = tag1->getText();

      std::vector<std::string> words;
      CStrUtil::toWords(value, words);

      std::vector<double> reals;

      for (const auto &word : words) {
        double r;
        if (! CStrUtil::toReal(word, &r))
          errorMsg("Invalid real '" + word + "'");

        reals.push_back(r);
      }

      if (reals.size() == 16)
        parentNode->matrix.matrix = CMatrix3D(&reals[0], 16);
      else
        std::cerr << "Invalid matrix size\n";

      assert(tag1->getNumTagChildren() == 0);
    }
    else if (name == "rotate") {
      TODO(parentName + "/" + name);
    }
    else if (name == "translate") {
      TODO(parentName + "/" + name);
    }
    else if (name == "scale") {
      TODO(parentName + "/" + name);
    }
    else if (name == "instance_geometry") {
      TODO(parentName + "/" + name);
    }
    else if (name == "extra") {
      TODO(parentName + "/" + name);
    }
    else if (name == "node") {
      auto *node = new Node;

      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "id") {
          node->id = opt_value;
        }
        else if (opt_name == "name") {
          node->name = opt_value;
        }
        else if (opt_name == "sid") {
          node->sid = opt_value;
        }
        else if (opt_name == "type") {
          node->type = opt_value;
        }
        else
          errorMsg("Unrecognised " + name + " option '" + opt_name + "'");
      }

      readNode(parentName + "/" + name, tag1, node);

      parentNode->children.push_back(node);
    }
    else if (name == "instance_camera") {
      TODO(parentName + "/" + name);
    }
    else if (name == "instance_light") {
      TODO(parentName + "/" + name);
    }
    else if (name == "instance_controller") {
      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if (opt_name == "url") {
          if (opt_value[0] == '#')
            parentNode->instanceController = opt_value.substr(1);
          else
            std::cerr << "Invalid url value '" << opt_value << "'\n";
        }
        else
          errorMsg("Unrecognised " + name + " option '" + opt_name + "'");
      }

      readNodeInstanceController(parentName + "/" + name, tag1, parentNode);
    }
    else if (name == "extra") {
      TODO(parentName + "/" + name);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readNodeInstanceController(const std::string &parentName, CXMLTag *tag,
                                        Node *parentNode)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options1 = tag1->getNumOptions();

    //const auto &options1 = tag1->getOptions();

    if      (name == "skeleton") {
      assert(num_options1 == 0);

      const auto &value = tag1->getText();

      if (value[0] == '#')
        parentNode->skeleton = value.substr(1);
      else
        std::cerr << "Invalid skeleton text '" << value << "'\n";

      assert(tag1->getNumTagChildren() == 0);
    }
    else if (name == "bind_material") {
      assert(num_options1 == 0);

      readNodeBindMaterial(parentName + "/" + name, tag1, parentNode);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readNodeBindMaterial(const std::string &parentName, CXMLTag *tag, Node *parentNode)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

  //auto num_options1 = tag1->getNumOptions();

  //const auto &options1 = tag1->getOptions();

    if (name == "technique_common") {
      //TechniqueCommon techniqueCommon;
      //readTechniqueCommon(parentName + "/" + name, tag1, techniqueCommon);

      readNodeBindMaterialCommon(parentName + "/" + name, tag1, parentNode);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readNodeBindMaterialCommon(const std::string &parentName, CXMLTag *tag, Node *)
{
  auto num_options = tag->getNumOptions();
  assert(num_options == 0);

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

    if (name == "instance_material") {
      std::string symbol, target;

      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "symbol") {
          symbol = opt_value;
        }
        else if (opt_name == "target") {
          if (opt_value[0] == '#')
            target = opt_value.substr(1);
          else
            std::cerr << "Invalid target value '" << opt_value << "'\n";
        }
        else
          errorMsg("Unrecognised " + name + " option '" + opt_name + "'");
      }

      idMaterialNameMap_[symbol] = target;

      assert(tag1->getNumTagChildren() == 0);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readScene(const std::string &parentName, CXMLTag *tag, Scene &scene)
{
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

    if (name == "instance_visual_scene") {
      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "url") {
          if (opt_value[0] == '#')
            scene.url = opt_value.substr(1);
          else
            std::cerr << "Invalid url value '" << opt_value << "'\n";
        }
        else
          errorMsg("Unrecognised " + name + " option '" + opt_name + "'");
      }
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

void
CImportDAE::
errorMsg(const std::string &msg) const
{
  std::cerr << "Error: " << msg << "\n";
}

void
CImportDAE::
TODO(const std::string &name) const
{
//if (isDebug())
    std::cerr << "TODO Tag: " << name << "\n";
}

void
CImportDAE::
TODOOption(const std::string &name) const
{
//if (isDebug())
    std::cerr << "TODO Option: " << name << "\n";
}

void
CImportDAE::
SKIP(const std::string &name) const
{
  if (isDebug())
    std::cerr << "SKIP: " << name << "\n";
}
