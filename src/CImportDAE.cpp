#include <CImportDAE.h>
#include <CGeometry3D.h>
#include <CGeomScene3D.h>
#include <CGeomNodeData.h>

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

  //auto num_options1 = tag1->getNumOptions();

    if      (name1 == "asset") {
      readAsset(name1, tag1);
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
      readScene(name1, tag1, daeScene_);
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

  addGeometry();

  //---

  processAnimation();

  return true;
}

void
CImportDAE::
addGeometry()
{
  for (auto &pg : idLibraryGeometry_)
    addGeometry(pg.second);
}

void
CImportDAE::
addGeometry(Geometry *geometry)
{
  // process geometry meshs
  if      (! geometry->meshPolyLists.empty()) {
    std::string diffuseFile, normalFile, emissionFile, specularFile;

    for (const auto &polyList : geometry->meshPolyLists) {
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

          auto pv = geometry->idMeshVertices.find(input.source);

          if (pv == geometry->idMeshVertices.end()) {
            std::cerr << "Vertices not found for vertex input\n";
            continue;
          }

          const auto &meshVertices = (*pv).second;

          for (const auto &input1 : meshVertices->inputs) {
            if (input1.type == MeshVerticesInputType::POSITION) {
              auto pv1 = geometry->idMeshSource.find(input1.source);

              if (pv1 == geometry->idMeshSource.end()) {
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

          auto pv1 = geometry->idMeshSource.find(input.source);

          if (pv1 == geometry->idMeshSource.end()) {
            std::cerr << "source not found for vertex normal\n";
            continue;
          }

          normalMesh = (*pv1).second.get();
        }
        else if (input.type == MeshPolyListInputType::TEXCOORD) {
          if (texCoordMesh)
            continue;

          auto pv1 = geometry->idMeshSource.find(input.source);

          if (pv1 == geometry->idMeshSource.end()) {
            std::cerr << "source not found for vertex tex coord\n";
            continue;
          }

          texCoordMesh = (*pv1).second.get();
        }
      }

      if (! vertexMesh)
        continue;

      //---

      geometry->object = CGeometry3DInst->createObject3D(scene_, "mesh");

      scene_->addObject(geometry->object);

      pobject_->addChild(geometry->object);

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
        geometry->object->addVertex(points[i]);

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

        auto faceId = geometry->object->addIPolygon(&vinds[0], int(vinds.size()));

        auto *face = geometry->object->getFaceP(faceId);

        if (normalMesh) {
          std::vector<CVector3D> normals1;

          for (auto &i : ninds) {
            if (i < nn)
              normals1.push_back(normals[i]);
          }

          if (! normals1.empty())
            face->setVertexNormals(normals1);
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

        geometry->object->setMaterialP(material);

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
  else if (! geometry->meshTrianglesArray.empty()) {
    for (const auto &triangles : geometry->meshTrianglesArray) {
      MeshSource *vertexMesh = nullptr;

      for (const auto &input : triangles->inputs) {
        if (input.type == MeshPolyListInputType::VERTEX) {
          auto pv = geometry->idMeshVertices.find(input.source);

          if (pv == geometry->idMeshVertices.end()) {
            std::cerr << "Vertices not found for vertex input\n";
            continue;
          }

          const auto &meshVertices = (*pv).second;

          for (const auto &input1 : meshVertices->inputs) {
            if (input1.type == MeshVerticesInputType::POSITION) {
              auto pv1 = geometry->idMeshSource.find(input1.source);

              if (pv1 == geometry->idMeshSource.end()) {
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

      geometry->object = CGeometry3DInst->createObject3D(scene_, "mesh");

      scene_->addObject(geometry->object);

      pobject_->addChild(geometry->object);

      //---

      int nv = int(vertexMesh->valueArray.values.size());

      for (int i = 0; i < nv; i += 3) {
        auto p = CPoint3D(vertexMesh->valueArray.values[i],
                          vertexMesh->valueArray.values[i + 1],
                          vertexMesh->valueArray.values[i + 2]);

        geometry->object->addVertex(p);
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

        geometry->object->addIPolygon(&vinds[0], int(vinds.size()));
      }
    }
  }
}

bool
CImportDAE::
processAnimation()
{
  bool rc = true;

  for (auto &pc : idLibraryController_) {
    if (! processAnimation(pc.second))
      rc = false;
  }

  return rc;
}

bool
CImportDAE::
processAnimation(LibraryController *controller)
{
  // get joint data
  std::vector<CMatrix3D>   inv_bind_matrices;
  std::vector<std::string> joint_names;

  for (const auto &joint : controller->skin.joints) {
    for (const auto &input : joint.inputs) {
      if      (input.semantic == "JOINT") {
        auto ps = controller->skin.sources.find(input.source);

        if (ps == controller->skin.sources.end()) {
          errorMsg("Failed to file JOINT source");
          continue;
        }

        const auto &source = (*ps).second;

        auto n = source.nameArray.values.size();
        std::cerr << "JOINT has " << n << " values\n";

        for (uint i = 0; i < n; ++i)
          joint_names.push_back(source.nameArray.values[i]);
      }
      else if (input.semantic == "INV_BIND_MATRIX") {
        auto ps = controller->skin.sources.find(input.source);

        if (ps == controller->skin.sources.end()) {
          errorMsg("Failed to file INV_BIND_MATRIX source");
          continue;
        }

        const auto &source = (*ps).second;

        auto n = source.valueArray.values.size()/16;
        std::cerr << "INV_BIND_MATRIX has " << n/16 << " matrices\n";

        for (uint i = 0; i < n; ++i) {
          auto matrix = CMatrix3D(&source.valueArray.values[i*16], 16);

          inv_bind_matrices.push_back(matrix);
        }
      }
      else
        errorMsg("Unknown joint input semantic '" + input.semantic + "'");
    }
  }

  // get nodes for joint names and set inverse bind matrix
  auto ps = idLibraryVisualScene_.find(daeScene_.url);
  if (ps == idLibraryVisualScene_.end())
    return errorMsg("Visual Scene not found");

  const auto &visualScene = (*ps).second;

  auto nj = joint_names.size();

  assert(inv_bind_matrices.size() == nj);

  for (uint i = 0; i < nj; ++i) {
    auto pn = visualScene.nodes.find(joint_names[i]);

    if (pn != visualScene.nodes.end())
      (*pn).second->inverseBindMatrix = inv_bind_matrices[i];
    else
      std::cerr << "Unknown node " << joint_names[i] << "\n";
  }

  //---

  Node *rootNode = nullptr;

  for (const auto &pn : visualScene.nodes) {
    auto *node = pn.second;

    if (node->instanceController == controller->id) {
      rootNode = node;
      break;
    }
  }

  if (! rootNode)
    return errorMsg("Missing root node for controller");

  auto pg = idLibraryGeometry_.find(controller->skin.source);

  if (pg == idLibraryGeometry_.end())
    return errorMsg("Missing geometry controller");

  auto *geometry = (*pg).second;

  auto *object = geometry->object;

  int i = 0;

  for (const auto &pn : visualScene.nodes) {
    auto *node = pn.second;

    node->ind = i++;
  }

  for (const auto &pn : visualScene.nodes) {
    auto *node = pn.second;

    CGeomNodeData nodeData;

    nodeData.setInd(node->ind);

    nodeData.setParent(node->parent ? int(node->parent->ind) : -1);

    nodeData.setJoint(true);

    nodeData.setInverseBindMatrix(node->inverseBindMatrix);

    nodeData.resizeChildren(uint(node->children.size()));

    nodeData.setName(node->name);

    for (size_t ic = 0; ic < node->children.size(); ++ic)
      nodeData.setChild(uint(ic), int(node->children[ic]->ind));

    nodeData.setObject(object);

    nodeData.setLocalTransform(node->matrix.matrix);

    object_->addNode(node->ind, nodeData);

    auto &nodeData1 = object_->editNode(node->ind);

    nodeData1.setIndex(node->ind);
  }

  //---

  for (auto &pa : idLibraryAnimation_) {
    auto &animation = pa.second;

    std::cerr << "Anim " << pa.first;
    std::cerr << " Channels=" << animation.channels.size();
    std::cerr << " Sources=" << animation.sources.size();
    std::cerr << " Samplers=" << animation.samplers.size();
    std::cerr << "\n";

    animation.data.clear();

    for (auto &source : animation.sources) {
      if      (source.techniqueCommon.params.size() == 1 &&
               source.techniqueCommon.params[0].type == "float") {
        for (uint ii = 0; ii < source.valueArray.values.size(); ++ii)
          animation.data.range.push_back(source.valueArray.values[ii]);
      }
      else if (source.techniqueCommon.params.size() == 1 &&
               source.techniqueCommon.params[0].type == "float4x4") {
        auto nm = source.valueArray.values.size()/16;

        for (uint ii = 0; ii < nm; ++ii) {
          auto matrix = CMatrix3D(&source.valueArray.values[ii*16], 16);

          animation.data.matrices.push_back(matrix);
        }
      }
      else if (source.techniqueCommon.params.size() == 1 &&
               source.techniqueCommon.params[0].type == "name") {
        for (uint ii = 0; ii < source.nameArray.values.size(); ++ii) {
          auto interpolation = AnimationInterpolation::NONE;

          if (source.nameArray.values[ii] == "LINEAR")
            interpolation = AnimationInterpolation::LINEAR;
          else
            std::cerr << "Invalid interpolation " << source.nameArray.values[ii] << "\n";

          animation.data.interpolations.push_back(interpolation);
        }
      }
      else {
        std::cerr << "Source " << source.id << "\n";
        for (auto &param : source.techniqueCommon.params) {
          std::cerr << "   " << param.type << "\n";
        }
      }
    }

    auto nr = animation.data.range.size();
    assert(nr == animation.data.matrices.size() && nr == animation.data.interpolations.size());
  }

#if 0
    for (uint ii = 0; ii < nr; ++ii)
      std::cerr << range[ii] << " " << matrices[ii] << "\n";
#endif

  // get associated nodes
  for (auto &pa : idLibraryAnimation_) {
    auto &animation = pa.second;

    animation.nodeDatas.clear();

    // get associated nodes
    for (auto &channel : animation.channels) {
      auto ps1 = animation.samplers.find(channel.source);

      if (ps1 != animation.samplers.end()) {
        auto lhs = channel.target;

        auto p1 = lhs.find('/');

        std::string rhs;

        if (p1 != std::string::npos) {
          rhs = lhs.substr(p1 + 1);
          lhs = lhs.substr(0, p1);
        }

        auto pn = visualScene.nodes.find(lhs);

        if (pn == visualScene.nodes.end()) {
          std::cerr << "Invalid node '" << lhs << "'\n";
          continue;
        }

        Animation::NodeData nodeData;

        nodeData.node = (*pn).second;
        nodeData.data = rhs;

        animation.nodeDatas.push_back(nodeData);
      }
      else
        std::cerr << "Invalid sampler '" << channel.source << "'\n";
    }
  }

  bool singleAnim = true;
  int  nv = -1;

  for (auto &pa : idLibraryAnimation_) {
    auto &animation = pa.second;

    auto nv1 = animation.data.range.size();
    auto nn1 = animation.nodeDatas.size();

    std::cerr << "Animation: " << animation.name << " NV = " << nv1 << " NN = " << nn1 << "\n";

    if (nn1 > 1)
      singleAnim = false;

    if (nv >= 0) {
      if (uint(nv) != nv1)
        singleAnim = false;
    }
    else
      nv = int(nv1);
  }

  if (! singleAnim) {
    std::cerr << "Multiple animations not handled\n";
    return false;
  }

  auto animName = "anim";

  // add to model
  for (auto &pa : idLibraryAnimation_) {
    auto &animation = pa.second;

    CGeomAnimationData animationData;

    animationData.setTransformInterpolation(CGeomAnimInterpolation::LINEAR);

    auto nr = animation.data.range.size();

    for (uint ii = 0; ii < nr; ++ii) {
      animationData.addTransformRangeValue(animation.data.range[ii]);

      animationData.addTransform(animation.data.matrices[ii]);
    }

    auto *node = animation.nodeDatas[0].node;

    object_->setNodeAnimationTransformData(node->ind, animName,
      CGeomObject3D::Transform::TRANSFORM, animationData);
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

  auto pm1 = idLibraryMaterial_.find((*pm).second);

  if (pm1 == idLibraryMaterial_.end())
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

  auto pi = idLibraryImage_.find((*ps1).second.from);

  if (pi == idLibraryImage_.end())
    return "";

  auto filename = (*pi).second->from;

  //std::cerr << "Image From: " << filename << "\n";

  return filename;
}

bool
CImportDAE::
readAsset(const std::string &parentName, CXMLTag *tag)
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

      idLibraryImage_[image->id] = std::move(image);
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

      idLibraryMaterial_[material->id] = std::move(material);
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

  //auto num_options1 = tag1->getNumOptions();

    if      (name == "surface") {
      Surface surface;

      readLibraryEffectProfileSurface(parentName + "/" + name, tag1, surface);

      effect->surfaces[param.sid] = surface;
    }
    else if (name == "sampler2D") {
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

  //auto num_options1 = tag1->getNumOptions();

    if      (name == "phong") {
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

    if      (name == "ambient") {
      readAmbient(parentName + "/" + name, tag1);
    }
    else if (name == "diffuse") {
      readDiffuse(parentName + "/" + name, tag1, effect);
    }
    else if (name == "emission") {
      readEmission(parentName + "/" + name, tag1, effect);
    }
    else if (name == "specular") {
      readSpecular(parentName + "/" + name, tag1, effect);
    }
    else if (name == "shininess") {
      readShininess(parentName + "/" + name, tag1);
    }
    else if (name == "reflective") {
      readReflective(parentName + "/" + name, tag1);
    }
    else if (name == "reflectivity") {
      readReflectivity(parentName + "/" + name, tag1);
    }
    else if (name == "transparent") {
      readTransparent(parentName + "/" + name, tag1, effect);
    }
    else if (name == "transparency") {
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

    //auto num_options1 = tag1->getNumOptions();

    if (name == "technique") {
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
  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if      (opt_name == "profile") {
      effect->technique_profile = opt_value;
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

    if      (name == "displacement") {
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
  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if      (opt_name == "opaque") {
      effect->transparentOpaque = opt_value;
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
  auto num_options = tag->getNumOptions();
  assert(num_options == 0);

  //---

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
      readLibraryGeometry(parentName + "/" + name, tag1);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readLibraryGeometry(const std::string &parentName, CXMLTag *tag)
{
  auto *geometry = new Geometry;

  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if      (opt_name == "id") {
      geometry->id = opt_value;
    }
    else if (opt_name == "name") {
      geometry->name = opt_value;
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

    if (name == "mesh") {
      readGeometryMesh(parentName + "/" + name, tag1, geometry);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  idLibraryGeometry_[geometry->id] = geometry;

  return true;
}

bool
CImportDAE::
readGeometryMesh(const std::string &parentName, CXMLTag *tag, Geometry *geometry)
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

    if      (name == "source") {
      auto meshSource = std::make_unique<MeshSource>();

      readGeometryMeshSource(parentName + "/" + name, tag1, meshSource);

      if (isDebug())
        meshSource->print();

      geometry->idMeshSource[meshSource->id] = std::move(meshSource);
    }
    else if (name == "vertices") {
      auto meshVertices = std::make_unique<MeshVertices>();

      readGeometryMeshVertices(parentName + "/" + name, tag1, meshVertices);

      if (isDebug())
        meshVertices->print();

      geometry->idMeshVertices[meshVertices->id] = std::move(meshVertices);
    }
    else if (name == "triangles") {
      auto triangles = std::make_unique<MeshTriangles>();

      readGeometryMeshTriangles(parentName + "/" + name, tag1, triangles);

      if (isDebug())
        triangles->print();

      geometry->meshTrianglesArray.push_back(std::move(triangles));
    }
    else if (name == "polygons") {
      TODO(parentName + "/" + name);
    }
    else if (name == "polylist") {
      auto polyList = std::make_unique<MeshPolyList>();

      readGeometryMeshPolylist(parentName + "/" + name, tag1, polyList);

      if (isDebug())
        polyList->print();

      geometry->meshPolyLists.push_back(std::move(polyList));
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readGeometryMeshSource(const std::string &parentName, CXMLTag *tag, MeshSourceP &meshSource)
{
  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

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
readGeometryMeshVertices(const std::string &parentName, CXMLTag *tag, MeshVerticesP &meshVertices)
{
  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if (opt_name == "id") {
      meshVertices->id = opt_value;
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
readGeometryMeshPolylist(const std::string &parentName, CXMLTag *tag, MeshPolyListP &polyList)
{
  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

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
readGeometryMeshTriangles(const std::string &parentName, CXMLTag *tag, MeshTrianglesP &triangles)
{
  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

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

    if (name == "accessor") {
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
  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

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
      if (opt_value[0] == '#')
        techniqueCommon.source = opt_value.substr(1);
      else
        errorMsg("Invalid source value '" + opt_value + "'");
    }
    else if (opt_name == "stride") {
      if (! CStrUtil::toInteger(opt_value, &techniqueCommon.accessorStride))
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
  auto *controller = new LibraryController;

  //---

  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if      (opt_name == "id") {
      controller->id = opt_value;
    }
    else if (opt_name == "name") {
      controller->name = opt_value;
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
      readLibraryControllerSkin(parentName + "/" + name, tag1, controller->skin);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  //---

  idLibraryController_[controller->id] = controller;

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
      if (opt_value[0] == '#')
        skin.source = opt_value.substr(1);
      else
        errorMsg("Invalid source value '" + opt_value + "'");
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
  Source source;

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

  skin.sources[source.id] = source;

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

  //auto num_options1 = tag1->getNumOptions();

    if (name == "animation") {
      readLibraryAnimation(parentName + "/" + name, tag1);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readLibraryAnimation(const std::string &parentName, CXMLTag *tag)
{
  Animation animation;

  //---

  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

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
          if (opt_value[0] == '#')
            channel.source = opt_value.substr(1);
          else
            errorMsg("Invalid source value '" + opt_value + "'");
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

  //---

  idLibraryAnimation_[animation.id] = animation;

  return true;
}

bool
CImportDAE::
readLibraryAnimationSource(const std::string &parentName, CXMLTag *tag, Animation &animation)
{
  Source source;

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

  animation.samplers[sampler.id] = sampler;

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
        errorMsg("Invalid source value '" + opt_value + "'");
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

    //auto num_options1 = tag1->getNumOptions();

    if (name == "visual_scene") {
      readLibraryVisualScene(parentName + "/" + name, tag1);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readLibraryVisualScene(const std::string &parentName, CXMLTag *tag)
{
  VisualScene scene;

  //---

  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if      (opt_name == "id") {
      scene.id = opt_value;
    }
    else if (opt_name == "name") {
      scene.name = opt_value;
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

    if (name == "node") {
      readNode(parentName + "/" + name, tag1, scene, nullptr);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  //---

  idLibraryVisualScene_[scene.id] = scene;

  if (isDebug())
    scene.print();

  return true;
}

bool
CImportDAE::
readNode(const std::string &parentName, CXMLTag *tag, VisualScene &scene, Node *parentNode)
{
  auto *node = new Node;

  //---

  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

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
      errorMsg("Unrecognised " + parentName + " option '" + opt_name + "'");
  }

  //---

  scene.nodes[node->id] = node;

  if (parentNode) {
    node->parent = parentNode;

    parentNode->children.push_back(node);
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

    if      (name == "matrix") {
      for (size_t j = 0; j < num_options1; ++j) {
        auto *option = options1[j];

        const auto &opt_name  = option->getName ();
        const auto &opt_value = option->getValue();

        if (opt_name == "sid") {
          node->matrix.sid = opt_value;
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
        node->matrix.matrix = CMatrix3D(&reals[0], 16);
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
      readNode(parentName + "/" + name, tag1, scene, node);
    }
    else if (name == "instance_camera") {
      TODO(parentName + "/" + name);
    }
    else if (name == "instance_light") {
      TODO(parentName + "/" + name);
    }
    else if (name == "instance_controller") {
      readNodeInstanceController(parentName + "/" + name, tag1, node);
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
readNodeInstanceController(const std::string &parentName, CXMLTag *tag, Node *node)
{
  auto num_options = tag->getNumOptions();

  const auto &options = tag->getOptions();

  for (size_t j = 0; j < num_options; ++j) {
    auto *option = options[j];

    const auto &opt_name  = option->getName ();
    const auto &opt_value = option->getValue();

    if (opt_name == "url") {
      if (opt_value[0] == '#')
        node->instanceController = opt_value.substr(1);
      else
        std::cerr << "Invalid url value '" << opt_value << "'\n";
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

    if      (name == "skeleton") {
      assert(num_options1 == 0);

      const auto &value = tag1->getText();

      if (value[0] == '#')
        node->skeleton = value.substr(1);
      else
        std::cerr << "Invalid skeleton text '" << value << "'\n";

      assert(tag1->getNumTagChildren() == 0);
    }
    else if (name == "bind_material") {
      readNodeBindMaterial(parentName + "/" + name, tag1, node);
    }
    else
      errorMsg("Unrecognised tag '" + parentName + "/" + name + "'");
  }

  return true;
}

bool
CImportDAE::
readNodeBindMaterial(const std::string &parentName, CXMLTag *tag, Node *node)
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

  //const auto &options1 = tag1->getOptions();

    if (name == "technique_common") {
      //TechniqueCommon techniqueCommon;
      //readTechniqueCommon(parentName + "/" + name, tag1, techniqueCommon);

      readNodeBindMaterialCommon(parentName + "/" + name, tag1, node);
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

bool
CImportDAE::
errorMsg(const std::string &msg) const
{
  std::cerr << "Error: " << msg << "\n";
  return false;
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
