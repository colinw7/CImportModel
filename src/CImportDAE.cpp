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
    }
    else if (opt_name == "version") {
    }
    else
      std::cerr << "Unrecognised option '" << opt_name << "'\n";
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

    if      (name == "asset") {
      assert(num_options1 == 0);

      readAssert(name, tag1);
    }
    else if (name == "library_images") {
      assert(num_options1 == 0);

      readLibraryImages(name, tag1);
    }
    else if (name == "library_materials") {
      assert(num_options1 == 0);

      readLibraryMaterials(name, tag1);
    }
    else if (name == "library_effects") {
      assert(num_options1 == 0);

      readLibraryEffects(name, tag1);
    }
    else if (name == "library_geometries") {
      assert(num_options1 == 0);

      readLibraryGeometries(name, tag1);
    }
    else if (name == "library_controllers") {
      assert(num_options1 == 0);

      readLibraryControllers(name, tag1);
    }
    else if (name == "library_animations") {
      assert(num_options1 == 0);

      readLibraryAnimations(name, tag1);
    }
    else if (name == "library_visual_scenes") {
      assert(num_options1 == 0);

      readLibraryVisualScenes(name, tag1);
    }
    else if (name == "library_cameras") {
      TODO(name);
    }
    else if (name == "library_lights") {
      TODO(name);
    }
    else if (name == "scene") {
      assert(num_options1 == 0);

      Scene scene;

      readScene(name, tag1, scene);
    }
    else
      std::cerr << "Unrecognised tag '" << name << "'\n";
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

        std::cerr << "Diffuse  file: " << diffuseFile  << "\n";
        std::cerr << "Normal   file: " << normalFile   << "\n";
        std::cerr << "Emission file: " << emissionFile << "\n";
        std::cerr << "Specular file: " << specularFile << "\n";
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

      auto nv = vertexMesh->values.size();
      auto np = nv/3;

      points.resize(np);

      for (uint i = 0; i < np; ++i) {
        auto p = CPoint3D(vertexMesh->values[3*i    ],
                          vertexMesh->values[3*i + 1],
                          vertexMesh->values[3*i + 2]);

        points[i] = p;
      }

      //---

      std::vector<CVector3D> normals;

      if (normalMesh) {
        auto nnv = normalMesh->values.size();
        auto nn  = nnv/3;

        normals.resize(nn);

        for (uint i = 0; i < nn; ++i) {
          auto v = CVector3D(normalMesh->values[3*i    ],
                             normalMesh->values[3*i + 1],
                             normalMesh->values[3*i + 2]);

          normals[i] = v;
        }
      }

      auto nn = normals.size();

      //---

      std::vector<CPoint2D> texCoords;

      if (texCoordMesh) {
        auto ntv = texCoordMesh->values.size();
        auto nt  = ntv/2;

        texCoords.resize(nt);

        for (uint i = 0; i < nt; ++i) {
          auto p = CPoint2D(texCoordMesh->values[2*i    ],
                            texCoordMesh->values[2*i + 1]);

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

      int nv = int(vertexMesh->values.size());

      for (int i = 0; i < nv; i += 3) {
        auto p = CPoint3D(vertexMesh->values[i],
                          vertexMesh->values[i + 1],
                          vertexMesh->values[i + 2]);

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
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if      (name == "contributor") {
      assert(num_options == 0);

      SKIP(parentName + ":" + name);
    }
    else if (name == "created") {
      assert(num_options == 0);

      SKIP(parentName + ":" + name);
    }
    else if (name == "modified") {
      assert(num_options == 0);

      SKIP(parentName + ":" + name);
    }
    else if (name == "revision") {
      assert(num_options == 0);

      SKIP(parentName + ":" + name);
    }
    else if (name == "title") {
      assert(num_options == 0);

      SKIP(parentName + ":" + name);
    }
    else if (name == "unit") {
      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
      //const auto &opt_value = option->getValue();

        if (opt_name == "meter") {
          SKIP(parentName + "/" + name + " : " + opt_name);
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      SKIP(parentName + ":" + name);
    }
    else if (name == "up_axis") {
      assert(num_options == 0);

      SKIP(parentName + ":" + name);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

bool
CImportDAE::
readLibraryImages(const std::string &parentName, CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if (name == "image") {
      auto image = std::make_unique<Image>();

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
            std::cerr << "Invalid integer '" << opt_value << "'\n";
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      readLibraryImage(parentName + "/" + name, tag1, image);

      if (isDebug())
        image->print();

      idImage_[image->id] = std::move(image);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

bool
CImportDAE::
readLibraryMaterials(const std::string &parentName, CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if (name == "material") {
      auto material = std::make_unique<Material>();

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
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      readLibraryMaterial(parentName + "/" + name, tag1, material);

      if (isDebug())
        material->print();

      idMaterial_[material->id] = std::move(material);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

bool
CImportDAE::
readLibraryEffects(const std::string &parentName, CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if (name == "effect") {
      auto effect = std::make_unique<LibraryEffect>();

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
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      readLibraryEffect(parentName + "/" + name, tag1, effect);

      idLibraryEffect_[effect->id] = std::move(effect);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

bool
CImportDAE::
readLibraryImage(const std::string &parentName, CXMLTag *tag, ImageP &image)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options = tag1->getNumOptions();

    //const auto &options = tag1->getOptions();

    if (name == "init_from") {
      assert(num_options == 0);

      const auto &value = tag1->getText();

      image->from = value;

      assert(tag1->getNumTagChildren() == 0);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

bool
CImportDAE::
readLibraryMaterial(const std::string &parentName, CXMLTag *tag, MaterialP &material)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if (name == "instance_effect") {
      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if (opt_name == "url") {
          if (opt_value[0] == '#')
            material->effects.push_back(opt_value.substr(1));
          else
            std::cerr << "Invalid url value '" << opt_value << "'\n";
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      assert(tag1->getNumChildren() == 0);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

bool
CImportDAE::
readLibraryEffect(const std::string &parentName, CXMLTag *tag, LibraryEffectP &effect)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options = tag1->getNumOptions();

    //const auto &options = tag1->getOptions();

    if (name == "profile_COMMON") {
      assert(num_options == 0);

      readLibraryEffectProfileCommon(parentName + "/" + name, tag1, effect);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

bool
CImportDAE::
readLibraryEffectProfileCommon(const std::string &parentName, CXMLTag *tag, LibraryEffectP &effect)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if      (name == "newparam") {
      NewParam newParam;

      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "sid") {
          newParam.sid = opt_value;
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      readLibraryEffectProfileParam(parentName + "/" + name, tag1, newParam, effect);
    }
    else if (name == "technique") {
      readLibraryEffectProfileTechnique(parentName + "/" + name, tag1, effect);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

bool
CImportDAE::
readLibraryEffectProfileParam(const std::string &parentName, CXMLTag *tag, NewParam &param,
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

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if      (name == "surface") {
      Surface surface;

      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "type") {
          surface.type = opt_value;
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      readLibraryEffectProfileSurface(parentName + "/" + name, tag1, surface);

      effect->surfaces[param.sid] = surface;
    }
    else if (name == "sampler2D") {
      assert(num_options == 0);

      Sampler2D sampler2D;

      readLibraryEffectSampler2D(parentName + "/" + name, tag1, param, sampler2D);

      effect->samplers[param.sid] = sampler2D;
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
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

    auto num_options = tag1->getNumOptions();

    //const auto &options = tag1->getOptions();

    if      (name == "phong") {
      assert(num_options == 0);

      readLibraryEffectProfilePhong(parentName + "/" + name, tag1, effect);
    }
    else if (name == "extra") {
      readLibraryEffectProfileExtra(parentName + "/" + name, tag1, effect);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
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

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if      (name == "ambient") {
      assert(num_options == 0);

      readAmbient(parentName + "/" + name, tag1);
    }
    else if (name == "diffuse") {
      assert(num_options == 0);

      readDiffuse(parentName + "/" + name, tag1, effect);
    }
    else if (name == "emission") {
      assert(num_options == 0);

      readEmission(parentName + "/" + name, tag1, effect);
    }
    else if (name == "specular") {
      assert(num_options == 0);

      readSpecular(parentName + "/" + name, tag1, effect);
    }
    else if (name == "shininess") {
      assert(num_options == 0);

      readShininess(parentName + "/" + name, tag1);
    }
    else if (name == "reflective") {
      assert(num_options == 0);

      readReflective(parentName + "/" + name, tag1);
    }
    else if (name == "reflectivity") {
      assert(num_options == 0);

      readReflectivity(parentName + "/" + name, tag1);
    }
    else if (name == "transparent") {
      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
      //const auto &opt_value = option->getValue();

        if      (opt_name == "opaque") {
          TODO(parentName + "/" + name + " : " + opt_name);
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      readTransparent(parentName + "/" + name, tag1);
    }
    else if (name == "transparency") {
      assert(num_options == 0);

      readTransparency(parentName + "/" + name, tag1);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
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

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if (name == "technique") {
      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
      //const auto &opt_value = option->getValue();

        if      (opt_name == "profile") {
          TODO(parentName + "/" + name + " : " + opt_name);
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      readLibraryEffectProfileTechExtra(parentName + "/" + name, tag1, effect);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
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

    auto num_options = tag1->getNumOptions();

    //const auto &options = tag1->getOptions();

    if (name == "displacement") {
      assert(num_options == 0);

      readLibraryEffectProfileDisplacement(parentName + "/" + name, tag1, effect);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
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

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if (name == "texture") {
      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "texture") {
          effect->normalTexture.texture = opt_value;
        }
        else if (opt_name == "texcoord") {
          effect->normalTexture.texcoord = opt_value;
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      readNormalTexture(parentName + "/" + name, tag1);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

bool
CImportDAE::
readNormalTexture(const std::string &parentName, CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    //auto num_options = tag1->getNumOptions();

    //const auto &options = tag1->getOptions();

    if (name == "extra") {
      TODO(parentName + "/" + name);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
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

    //auto num_options = tag1->getNumOptions();

    //const auto &options = tag1->getOptions();

    if (name == "color") {
      TODO(parentName + "/" + name);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
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

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if (name == "texture") {
      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "texture") {
          effect->diffuseTexture.texture = opt_value;
        }
        else if (opt_name == "texcoord") {
          effect->diffuseTexture.texcoord = opt_value;
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      readDiffuseTexture(parentName + "/" + name, tag1);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

bool
CImportDAE::
readDiffuseTexture(const std::string &parentName, CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    //auto num_options = tag1->getNumOptions();

    //const auto &options = tag1->getOptions();

    if (name == "extra") {
      TODO(parentName + "/" + name);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
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

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if (name == "texture") {
      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "texture") {
          effect->emissionTexture.texture = opt_value;
        }
        else if (opt_name == "texcoord") {
          effect->emissionTexture.texcoord = opt_value;
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      readEmissionTexture(parentName + "/" + name, tag1);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

bool
CImportDAE::
readEmissionTexture(const std::string &parentName, CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    //auto num_options = tag1->getNumOptions();

    //const auto &options = tag1->getOptions();

    if (name == "extra") {
      TODO(parentName + "/" + name);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
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

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if (name == "texture") {
      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "texture") {
          effect->specularTexture.texture = opt_value;
        }
        else if (opt_name == "texcoord") {
          effect->specularTexture.texcoord = opt_value;
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      readSpecularTexture(parentName + "/" + name, tag1);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

bool
CImportDAE::
readSpecularTexture(const std::string &parentName, CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    //auto num_options = tag1->getNumOptions();

    //const auto &options = tag1->getOptions();

    if (name == "extra") {
      TODO(parentName + "/" + name);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
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

    //auto num_options = tag1->getNumOptions();

    //const auto &options = tag1->getOptions();

    if (name == "float") {
      TODO(parentName + "/" + name);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
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

    //auto num_options = tag1->getNumOptions();

    //const auto &options = tag1->getOptions();

    if (name == "color") {
      TODO(parentName + "/" + name);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
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

    //auto num_options = tag1->getNumOptions();

    //const auto &options = tag1->getOptions();

    if (name == "float") {
      TODO(parentName + "/" + name);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

bool
CImportDAE::
readTransparent(const std::string &parentName, CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    //auto num_options = tag1->getNumOptions();

    //const auto &options = tag1->getOptions();

    if (name == "texture") {
      TODO(parentName + "/" + name);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
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

    const auto &name = tag1->getName();

    //auto num_options = tag1->getNumOptions();

    //const auto &options = tag1->getOptions();

    if (name == "float") {
      TODO(parentName + "/" + name);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

bool
CImportDAE::
readLibraryEffectProfileSurface(const std::string &parentName, CXMLTag *tag, Surface &surface)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options = tag1->getNumOptions();

    //const auto &options = tag1->getOptions();

    if (name == "init_from") {
      assert(num_options == 0);

      const auto &value = tag1->getText();

      surface.from = value;

      assert(tag1->getNumTagChildren() == 0);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
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

    auto num_options = tag1->getNumOptions();

    //const auto &options = tag1->getOptions();

    if      (name == "source") {
      assert(num_options == 0);

      sampler.source = tag1->getText();

      assert(tag1->getNumTagChildren() == 0);
    }
    else if (name == "minfilter") {
      assert(num_options == 0);

      sampler.minfilter = tag1->getText();

      assert(tag1->getNumTagChildren() == 0);
    }
    else if (name == "magfilter") {
      assert(num_options == 0);

      sampler.magfilter = tag1->getText();

      assert(tag1->getNumTagChildren() == 0);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

bool
CImportDAE::
readLibraryGeometries(const std::string &parentName, CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if (name == "geometry") {
      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
      //const auto &opt_value = option->getValue();

        if      (opt_name == "id") {
          TODO(parentName + "/" + name + " : " + opt_name);
        }
        else if (opt_name == "name") {
          TODO(parentName + "/" + name + " : " + opt_name);
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      readGeometry(parentName + "/" + name, tag1);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

bool
CImportDAE::
readGeometry(const std::string &parentName, CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options = tag1->getNumOptions();

    //const auto &options = tag1->getOptions();

    if (name == "mesh") {
      assert(num_options == 0);

      readMesh(parentName + "/" + name, tag1);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
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

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if      (name == "source") {
      auto meshSource = std::make_unique<MeshSource>();

      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "id") {
          meshSource->id = opt_value;
        }
        else if (opt_name == "name") {
          meshSource->name = opt_value;
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      readMeshSource(parentName + "/" + name, tag1, meshSource);

      if (isDebug())
        meshSource->print();

      idMeshSource_[meshSource->id] = std::move(meshSource);
    }
    else if (name == "vertices") {
      auto meshVertices = std::make_unique<MeshVertices>();

      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if (opt_name == "id") {
          meshVertices->id = opt_value;
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      readMeshVertices(parentName + "/" + name, tag1, meshVertices);

      if (isDebug())
        meshVertices->print();

      idMeshVertices_[meshVertices->id] = std::move(meshVertices);
    }
    else if (name == "triangles") {
      auto triangles = std::make_unique<MeshTriangles>();

      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "material") {
          triangles->material = opt_value;
        }
        else if (opt_name == "count") {
          if (! CStrUtil::toInteger(opt_value, &triangles->count))
            std::cerr << "Invalid integer '" << opt_value << "'\n";
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
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

      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "material") {
          polyList->material = opt_value;
        }
        else if (opt_name == "count") {
          if (! CStrUtil::toInteger(opt_value, &polyList->count))
            std::cerr << "Invalid integer '" << opt_value << "'\n";
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      readMeshPolylist(parentName + "/" + name, tag1, polyList);

      if (isDebug())
        polyList->print();

      meshPolyLists_.push_back(std::move(polyList));
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
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

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if      (name == "technique_common") {
      assert(num_options == 0);

      readTechniqueCommon(parentName + "/" + name, tag1, meshSource);
    }
    else if (name == "float_array") {
      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "id") {
          meshSource->valuesId = opt_value;
        }
        else if (opt_name == "count") {
          if (! CStrUtil::toInteger(opt_value, &meshSource->valuesCount))
            std::cerr << "Invalid integer '" << opt_value << "'\n";
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      const auto &value = tag1->getText();

      std::vector<std::string> words;
      CStrUtil::toWords(value, words);

      for (const auto &word : words) {
        double r;
        if (! CStrUtil::toReal(word, &r))
          std::cerr << "Invalid float '" << word << "'\n";

        meshSource->values.push_back(r);
      }

      assert(tag1->getNumTagChildren() == 0);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
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

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if (name == "input") {
      MeshVerticesInput input;

      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "semantic") {
          if      (opt_value == "POSITION")
            input.type = MeshVerticesInputType::POSITION;
          else if (opt_value == "NORMAL")
            input.type = MeshVerticesInputType::NORMAL;
          else
            std::cerr << "Invalid semantic '" << opt_value << "'\n";
        }
        else if (opt_name == "source") {
          if (opt_value[0] == '#')
            input.source = opt_value.substr(1);
          else
            std::cerr << "Invalid source value '" << opt_value << "'\n";
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      assert(tag1->getNumChildren() == 0);

      meshVertices->inputs.push_back(input);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
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

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if      (name == "input") {
      MeshPolyListInput input;

      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "semantic") {
          if      (opt_value == "VERTEX")
            input.type = MeshPolyListInputType::VERTEX;
          else if (opt_value == "NORMAL")
            input.type = MeshPolyListInputType::NORMAL;
          else if (opt_value == "TEXCOORD")
            input.type = MeshPolyListInputType::TEXCOORD;
          else
            std::cerr << "Invalid semantic '" << opt_value << "'\n";
        }
        else if (opt_name == "offset") {
          if (! CStrUtil::toInteger(opt_value, &input.offset))
            std::cerr << "Invalid integer '" << opt_value << "'\n";

          polyList->max_offset = std::max(polyList->max_offset, input.offset);
        }
        else if (opt_name == "set") {
          if (! CStrUtil::toInteger(opt_value, &input.set))
            std::cerr << "Invalid integer '" << opt_value << "'\n";

          polyList->max_set = std::max(polyList->max_set, input.set);
        }
        else if (opt_name == "source") {
          if (opt_value[0] == '#')
            input.source = opt_value.substr(1);
          else
            std::cerr << "Invalid source value '" << opt_value << "'\n";
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      assert(tag1->getNumChildren() == 0);

      polyList->inputs.push_back(input);
    }
    else if (name == "vcount") {
      assert(num_options == 0);

      const auto &value = tag1->getText();

      std::vector<std::string> words;
      CStrUtil::toWords(value, words);

      for (const auto &word : words) {
        int ii;
        if (! CStrUtil::toInteger(word, &ii))
          std::cerr << "Invalid integer '" << word << "'\n";

        polyList->vcount.push_back(ii);

        polyList->vsum += ii;
      }

      assert(tag1->getNumTagChildren() == 0);
    }
    else if (name == "p") {
      assert(num_options == 0);

      const auto &value = tag1->getText();

      std::vector<std::string> words;
      CStrUtil::toWords(value, words);

      std::vector<int> ints;

      for (const auto &word : words) {
        int ii;
        if (! CStrUtil::toInteger(word, &ii))
          std::cerr << "Invalid integer '" << word << "'\n";

        polyList->p.push_back(ii);
      }

      assert(tag1->getNumTagChildren() == 0);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
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

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if      (name == "input") {
      MeshPolyListInput input;

      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "semantic") {
          if      (opt_value == "VERTEX")
            input.type = MeshPolyListInputType::VERTEX;
          else if (opt_value == "NORMAL")
            input.type = MeshPolyListInputType::NORMAL;
          else if (opt_value == "TEXCOORD")
            input.type = MeshPolyListInputType::TEXCOORD;
          else
            std::cerr << "Invalid semantic '" << opt_value << "'\n";
        }
        else if (opt_name == "offset") {
          if (! CStrUtil::toInteger(opt_value, &input.offset))
            std::cerr << "Invalid integer '" << opt_value << "'\n";

          triangles->max_offset = std::max(triangles->max_offset, input.offset);
        }
        else if (opt_name == "set") {
          if (! CStrUtil::toInteger(opt_value, &input.set))
            std::cerr << "Invalid integer '" << opt_value << "'\n";

          triangles->max_set = std::max(triangles->max_set, input.set);
        }
        else if (opt_name == "source") {
          if (opt_value[0] == '#')
            input.source = opt_value.substr(1);
          else
            std::cerr << "Invalid source value '" << opt_value << "'\n";
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      assert(tag1->getNumChildren() == 0);

      triangles->inputs.push_back(input);
    }
    else if (name == "p") {
      assert(num_options == 0);

      const auto &value = tag1->getText();

      std::vector<std::string> words;
      CStrUtil::toWords(value, words);

      std::vector<int> ints;

      for (const auto &word : words) {
        int ii;
        if (! CStrUtil::toInteger(word, &ii))
          std::cerr << "Invalid integer '" << word << "'\n";

        triangles->p.push_back(ii);
      }

      assert(tag1->getNumTagChildren() == 0);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

bool
CImportDAE::
readTechniqueCommon(const std::string &parentName, CXMLTag *tag, MeshSourceP &meshSource)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if (name == "accessor") {
      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "count") {
          if (! CStrUtil::toInteger(opt_value, &meshSource->accessorCount))
            std::cerr << "Invalid integer '" << opt_value << "'\n";
        }
        else if (opt_name == "offset") {
          if (! CStrUtil::toInteger(opt_value, &meshSource->accessorOffset))
            std::cerr << "Invalid integer '" << opt_value << "'\n";
        }
        else if (opt_name == "source") {
          meshSource->source = opt_value;
        }
        else if (opt_name == "stride") {
          if (! CStrUtil::toInteger(opt_value, &meshSource->accessorStride))
            std::cerr << "Invalid integer '" << opt_value << "'\n";
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      readTechniqueCommonAccessor(parentName + "/" + name, tag1, meshSource);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

bool
CImportDAE::
readTechniqueCommonAccessor(const std::string &parentName, CXMLTag *tag, MeshSourceP &meshSource)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if (name == "param") {
      MeshSourceParam param;

      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "name") {
          param.name = opt_value;
        }
        else if (opt_name == "type") {
          param.type = opt_value;
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      assert(tag1->getNumChildren() == 0);

      meshSource->params.push_back(param);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

bool
CImportDAE::
readLibraryControllers(const std::string &parentName, CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if (name == "controller") {
      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
      //const auto &opt_value = option->getValue();

        if (opt_name == "id") {
          TODO(parentName + "/" + name + " : " + opt_name);
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      readLibraryController(parentName + "/" + name, tag1);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

bool
CImportDAE::
readLibraryController(const std::string &parentName, CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if (name == "skin") {
      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
      //const auto &opt_value = option->getValue();

        if (opt_name == "source") {
          TODO(parentName + "/" + name + " : " + opt_name);
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      readLibraryControllerSkin(parentName + "/" + name, tag1);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

bool
CImportDAE::
readLibraryControllerSkin(const std::string &parentName, CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if      (name == "bind_shape_matrix") {
      assert(num_options == 0);

      const auto &value = tag1->getText();

      std::vector<std::string> words;
      CStrUtil::toWords(value, words);

      std::vector<double> reals;

      for (const auto &word : words) {
        double r;
        if (! CStrUtil::toReal(word, &r))
          std::cerr << "Invalid real '" << word << "'\n";

        reals.push_back(r);
      }

      CMatrix3D matrix;

      if (reals.size() == 16)
        matrix = CMatrix3D(&reals[0], 16);
      else
        std::cerr << "Invalid matrix size\n";

      assert(tag1->getNumTagChildren() == 0);
    }
    else if (name == "source") {
      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
      //const auto &opt_value = option->getValue();

        if (opt_name == "id") {
          TODO(parentName + "/" + name + " : " + opt_name);
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      readLibraryControllerSkinSource(parentName + "/" + name, tag1);
    }
    else if (name == "joints") {
      TODO(parentName + "/" + name);
    }
    else if (name == "vertex_weights") {
      TODO(parentName + "/" + name);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

bool
CImportDAE::
readLibraryControllerSkinSource(const std::string &parentName, CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

  //auto num_options = tag1->getNumOptions();

  //const auto &options = tag1->getOptions();

    if      (name == "Name_array") {
      TODO(parentName + "/" + name);
    }
    else if (name == "technique_common") {
      TODO(parentName + "/" + name);
    }
    else if (name == "float_array") {
      TODO(parentName + "/" + name);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

bool
CImportDAE::
readLibraryAnimations(const std::string &parentName, CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if (name == "animation") {
      struct Animation {
        std::string id;
        std::string name;
      };

      Animation animation;

      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "id") {
          animation.id = opt_value;
        }
        else if (opt_name == "name") {
          animation.name = opt_value;
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      readLibraryAnimation(parentName + "/" + name, tag1);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

bool
CImportDAE::
readLibraryAnimation(const std::string &parentName, CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if      (name == "source") {
      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
      //const auto &opt_value = option->getValue();

        if      (opt_name == "id") {
          TODO(parentName + "/" + name + " : " + opt_name);
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      readLibraryAnimationSource(parentName + "/" + name, tag1);
    }
    else if (name == "sampler") {
      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
      //const auto &opt_value = option->getValue();

        if      (opt_name == "id") {
          TODO(parentName + "/" + name + " : " + opt_name);
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      readLibraryAnimationSampler(parentName + "/" + name, tag1);
    }
    else if (name == "channel") {
      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
      //const auto &opt_value = option->getValue();

        if      (opt_name == "source") {
          TODO(parentName + "/" + name + " : " + opt_name);
        }
        else if (opt_name == "target") {
          TODO(parentName + "/" + name + " : " + opt_name);
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      assert(tag1->getNumChildren() == 0);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

bool
CImportDAE::
readLibraryAnimationSource(const std::string &parentName, CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    //auto num_options = tag1->getNumOptions();

    //const auto &options = tag1->getOptions();

    if      (name == "float_array") {
      TODO(parentName + "/" + name);
    }
    else if (name == "technique_common") {
      TODO(parentName + "/" + name);
    }
    else if (name == "Name_array") {
      TODO(parentName + "/" + name);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

bool
CImportDAE::
readLibraryAnimationSampler(const std::string &parentName, CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if (name == "input") {
      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
      //const auto &opt_value = option->getValue();

        if      (opt_name == "semantic") {
          TODO(parentName + "/" + name + " : " + opt_name);
        }
        else if (opt_name == "source") {
          TODO(parentName + "/" + name + " : " + opt_name);
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      assert(tag1->getNumTagChildren() == 0);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

bool
CImportDAE::
readLibraryVisualScenes(const std::string &parentName, CXMLTag *tag)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if (name == "visual_scene") {
      VisualScene visualScene;

      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "id") {
          visualScene.id = opt_value;
        }
        else if (opt_name == "name") {
          visualScene.name = opt_value;
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      readLibraryVisualScene(parentName + "/" + name, tag1, visualScene);

      if (isDebug())
        visualScene.print();
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
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

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if (name == "node") {
      auto *node = new Node;

      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

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
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      readNode(parentName + "/" + name, tag1, node);

      scene.nodes.push_back(node);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
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

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if      (name == "matrix") {
      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
      //const auto &opt_value = option->getValue();

        if (opt_name == "sid") {
          TODO(parentName + "/" + name + " : " + opt_name);
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      const auto &value = tag1->getText();

      std::vector<std::string> words;
      CStrUtil::toWords(value, words);

      std::vector<double> reals;

      for (const auto &word : words) {
        double r;
        if (! CStrUtil::toReal(word, &r))
          std::cerr << "Invalid real '" << word << "'\n";

        reals.push_back(r);
      }

      if (reals.size() == 16)
        parentNode->matrix = CMatrix3D(&reals[0], 16);
      else
        std::cerr << "Invalid matrix size\n";

      assert(tag1->getNumTagChildren() == 0);
    }
    else if (name == "node") {
      auto *node = new Node;

      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

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
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      readNode(parentName + "/" + name, tag1, node);

      parentNode->children.push_back(node);
    }
    else if (name == "instance_controller") {
      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if (opt_name == "url") {
          if (opt_value[0] == '#')
            parentNode->instanceController = opt_value.substr(1);
          else
            std::cerr << "Invalid url value '" << opt_value << "'\n";
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      readNodeInstanceController(parentName + "/" + name, tag1, parentNode);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
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

    auto num_options = tag1->getNumOptions();

    //const auto &options = tag1->getOptions();

    if      (name == "skeleton") {
      assert(num_options == 0);

      const auto &value = tag1->getText();

      if (value[0] == '#')
        parentNode->skeleton = value.substr(1);
      else
        std::cerr << "Invalid skeleton text '" << value << "'\n";

      assert(tag1->getNumTagChildren() == 0);
    }
    else if (name == "bind_material") {
      assert(num_options == 0);

      readNodeBindMaterial(parentName + "/" + name, tag1, parentNode);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
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

    auto num_options = tag1->getNumOptions();

    //const auto &options = tag1->getOptions();

    if (name == "technique_common") {
      assert(num_options == 0);

      readNodeBindMaterialCommon(parentName + "/" + name, tag1, parentNode);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

bool
CImportDAE::
readNodeBindMaterialCommon(const std::string &parentName, CXMLTag *tag, Node *)
{
  auto children = tag->getChildren();

  auto num_children = tag->getNumChildren();

  for (size_t i = 0; i < num_children; ++i) {
    const auto *token = children[i];

    if (! token->isTag())
      continue;

    auto *tag1 = token->getTag();

    const auto &name = tag1->getName();

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if (name == "instance_material") {
      std::string symbol, target;

      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

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
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }

      idMaterialNameMap_[symbol] = target;

      assert(tag1->getNumTagChildren() == 0);
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
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

    auto num_options = tag1->getNumOptions();

    const auto &options = tag1->getOptions();

    if (name == "instance_visual_scene") {
      for (size_t j = 0; j < num_options; ++j) {
        auto *option = options[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if      (opt_name == "url") {
          if (opt_value[0] == '#')
            scene.url = opt_value.substr(1);
          else
            std::cerr << "Invalid url value '" << opt_value << "'\n";
        }
        else
          std::cerr << "Unrecognised " << name << " option '" << opt_name << "'\n";
      }
    }
    else
      std::cerr << "Unrecognised tag '" << parentName << "/" << name << "'\n";
  }

  return true;
}

void
CImportDAE::
TODO(const std::string &name) const
{
//if (isDebug())
    std::cerr << "TODO: " << name << "\n";
}

void
CImportDAE::
SKIP(const std::string &name) const
{
  if (isDebug())
    std::cerr << "SKIP: " << name << "\n";
}
