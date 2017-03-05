#ifndef CGEOM_IMPORT_COB_H
#define CGEOM_IMPORT_COB_H

#include <CAutoPtr.h>
#include <CImportCOB.h>
#include <CGeomObject3D.h>

class CGeomImportCOB : public CImportCOB {
 private:
  typedef std::vector<uint>      FaceList;
  typedef std::map<int,FaceList> MaterialFaceList;

  CGeomScene3D            *scene_;
  CAutoPtr<CGeomScene3D>   pscene_;
  CGeomObject3D           *object_;
  CAutoPtr<CGeomObject3D>  pobject_;
  MaterialFaceList         material_face_list_;

 public:
  CGeomImportCOB(CGeomScene3D *scene=NULL, const std::string &name="cob");

 ~CGeomImportCOB() { }

  CGeomObject3D &getObject() { return *object_; }

  CGeomScene3D *releaseScene() {
    pscene_ .release();
    pobject_.release();

    return scene_;
  }

  CGeomObject3D *releaseObject() {
    pobject_.release();

    return object_;
  }

  void addVertex(double x, double y, double z);

  void addTexturePoint(double x, double y);

  void addITriangle(int i1, int i2, int i3, int t1, int t2, int t3, int imat);

  void setFacesMaterial(int imat, CShadeType3D shade_type,
                        const CRGBA &ambient, const CRGBA &diffuse,
                        const CRGBA &specular, const CRGBA &emission,
                        double shininess, const std::string &fileName);

 private:
  void addMaterialFace(int imat, uint face_num);
  void setFacesMaterial(int imat, CMaterial *material);

  FaceList &getMaterialFaces(int num);
};

#endif
