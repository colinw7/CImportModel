#ifndef CGEOM_IMPORT_COB_H
#define CGEOM_IMPORT_COB_H

#include <CImportCOB.h>
#include <CGeomObject3D.h>

class CGeomImportCOB : public CImportCOB {
 private:
  typedef std::vector<uint>      FaceList;
  typedef std::map<int,FaceList> MaterialFaceList;

 public:
  CGeomImportCOB(CGeomScene3D *scene=nullptr, const std::string &name="cob");

 ~CGeomImportCOB();

  CGeomScene3D &getScene() override { return *scene_; }

  CGeomObject3D &getObject() { return *object_; }

  CGeomScene3D *releaseScene() override {
    pscene_ .release();
    pobject_.release();

    return scene_;
  }

  CGeomObject3D *releaseObject() {
    pobject_.release();

    return object_;
  }

  void addVertex(double x, double y, double z) override;

  void addTexturePoint(double x, double y) override;

  void addITriangle(int i1, int i2, int i3, int t1, int t2, int t3, int imat) override;

  void setFacesMaterial(int imat, CShadeType3D shade_type,
                        const CRGBA &ambient, const CRGBA &diffuse,
                        const CRGBA &specular, const CRGBA &emission,
                        double shininess, const std::string &fileName) override;

 private:
  void addMaterialFace(int imat, uint face_num);
  void setFacesMaterial(int imat, CMaterial *material);

  FaceList &getMaterialFaces(int num);

 private:
  CGeomScene3D*    scene_ { nullptr };
  SceneP           pscene_;
  CGeomObject3D*   object_ { nullptr };
  ObjectP          pobject_;
  MaterialFaceList material_face_list_;
};

#endif
