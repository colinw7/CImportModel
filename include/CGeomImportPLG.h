#ifndef CGEOM_IMPORT_PLG_H
#define CGEOM_IMPORT_PLG_H

#include <CImportPLG.h>
#include <CGeomObject3D.h>

class CGeomImportPLG : public CImportPLG {
 public:
  CGeomImportPLG(CGeomScene3D *scene=nullptr, const std::string &name="plg");

 ~CGeomImportPLG();

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

  void addITriangle(int i1, int i2, int i3, CDrawType3D draw_type,
                    CShadeType3D shade_type, const CRGBA &rgba) override;
  void addITriangles(uint *inds, uint num_inds, CDrawType3D draw_type,
                     CShadeType3D shade_type, const CRGBA &rgba) override;

 private:
  CGeomScene3D*  scene_ { nullptr };
  SceneP         pscene_;
  CGeomObject3D* object_ { nullptr };
  ObjectP        pobject_;
};

#endif
