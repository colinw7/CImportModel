#ifndef MaterialNode_H
#define MaterialNode_H

#include <AppNode.h>
#include <NodeInput.h>
#include <CRGBA.h>

class CQColorEdit;
class CQRealSpin;
class QLabel;

namespace CQTextureGen {

class MaterialGUI;

class MaterialNode : public AppNode {
 public:
  class DiffuseInput : public NodeInput {
   public:
    DiffuseInput(MaterialNode *node);

    CPoint2D pos() const override;

   private:
    MaterialNode* node_ { nullptr };
  };

  class NormalInput : public NodeInput {
   public:
    NormalInput(MaterialNode *node);

    CPoint2D pos() const override;

   private:
    MaterialNode* node_ { nullptr };
  };

  class SpecularInput : public NodeInput {
   public:
    SpecularInput(MaterialNode *node);

    CPoint2D pos() const override;

   private:
    MaterialNode* node_ { nullptr };
  };

  class EmissionInput : public NodeInput {
   public:
    EmissionInput(MaterialNode *node);

    CPoint2D pos() const override;

   private:
    MaterialNode* node_ { nullptr };
  };

  class MetallicInput : public NodeInput {
   public:
    MetallicInput(MaterialNode *node);

    CPoint2D pos() const override;

   private:
    MaterialNode* node_ { nullptr };
  };

  class RoughnessInput : public NodeInput {
   public:
    RoughnessInput(MaterialNode *node);

    CPoint2D pos() const override;

   private:
    MaterialNode* node_ { nullptr };
  };

  class OcclusionInput : public NodeInput {
   public:
    OcclusionInput(MaterialNode *node);

    CPoint2D pos() const override;

   private:
    MaterialNode* node_ { nullptr };
  };

  class ORMInput : public NodeInput {
   public:
    ORMInput(MaterialNode *node);

    CPoint2D pos() const override;

   private:
    MaterialNode* node_ { nullptr };
  };

 public:
  MaterialNode(App *app);

  MaterialGUI *gui() const { return gui_; }

  DiffuseInput  *diffuseInput () const { return diffuseInput_ ; }
  NormalInput   *normalInput  () const { return normalInput_  ; }
  SpecularInput *specularInput() const { return specularInput_; }
  EmissionInput *emissionInput() const { return emissionInput_; }

  MetallicInput  *metallicInput () const { return metallicInput_ ; }
  RoughnessInput *roughnessInput() const { return roughnessInput_; }
  OcclusionInput *occlusionInput() const { return occlusionInput_; }
  ORMInput       *ormInput      () const { return ormInput_      ; }

  const CRGBA &diffuse() const { return diffuse_; }
  void setDiffuse(const CRGBA &v) { diffuse_ = v; invalidate(); }

  void process() override;

 private:
  MaterialGUI* gui_ { nullptr };

  DiffuseInput*  diffuseInput_  { nullptr };
  NormalInput*   normalInput_   { nullptr };
  SpecularInput* specularInput_ { nullptr };
  EmissionInput* emissionInput_ { nullptr };

  MetallicInput  *metallicInput_  { nullptr };
  RoughnessInput *roughnessInput_ { nullptr };
  OcclusionInput *occlusionInput_ { nullptr };
  ORMInput       *ormInput_       { nullptr };

  CRGBA diffuse_ { 0.8, 0.8, 0.8 };
};

//---

class MaterialGUI : public QFrame {
  Q_OBJECT

 public:
  MaterialGUI(MaterialNode *node);

  CQColorEdit *diffuseEdit  () const { return diffuseEdit_  ; }
  QLabel      *normalLabel  () const { return normalLabel_  ; }
  QLabel      *specularLabel() const { return specularLabel_; }
  QLabel      *emissionLabel() const { return emissionLabel_; }

  CQRealSpin *metallicEdit () const { return metallicEdit_ ; }
  CQRealSpin *roughnessEdit() const { return roughnessEdit_; }
  CQRealSpin *occlusionEdit() const { return occlusionEdit_; }
  QLabel     *ORMLabel     () const { return ORMLabel_; }

  void updateWidgets();

  void connectSlots(bool b);

 private Q_SLOTS:
  void diffuseSlot(const QColor &);

  void metallicSlot(double);
  void roughnessSlot(double);
  void occlusionSlot(double);

 private:
  MaterialNode* node_ { nullptr };

  CQColorEdit* diffuseEdit_   { nullptr };
  QLabel*      normalLabel_   { nullptr };
  QLabel*      specularLabel_ { nullptr };
  QLabel*      emissionLabel_ { nullptr };

  CQRealSpin *metallicEdit_  { nullptr };
  CQRealSpin *roughnessEdit_ { nullptr };
  CQRealSpin *occlusionEdit_ { nullptr };
  QLabel*     ORMLabel_      { nullptr };
};

}

#endif
