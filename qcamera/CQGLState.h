#ifndef CQGLState_H
#define CQGLState_H

#define CQGLStateInst CQGLState::instance()

class CQGLState {
 public:
  static CQGLState *instance() {
    static CQGLState *inst;

    if (! inst)
      inst = new CQGLState;

    return inst;
  }

 ~CQGLState() { }

  //---

  bool isDepthTest() const { return depthTest_; }

  bool setDepthTest(bool b) {
    if (b != depthTest_) {
      std::swap(depthTest_, b);

      if (depthTest_)
        glEnable(GL_DEPTH_TEST);
      else
        glDisable(GL_DEPTH_TEST);
    }

    return b;
  }

  //---

  bool isCullFace() const { return cullFace_; }

  bool setCullFace(bool b) {
    if (b != cullFace_) {
      std::swap(cullFace_, b);

      if (cullFace_)
        glEnable(GL_CULL_FACE);
      else
        glDisable(GL_CULL_FACE);
    }

    return b;
  }

  //---

  bool isBlend() const { return blend_; }

  bool setBlend(bool b) {
    if (b != blend_) {
      std::swap(blend_, b);

      if (blend_)
        glEnable(GL_BLEND);
      else
        glDisable(GL_BLEND);
    }

    return b;
  }

  //---

  bool isPolygonOffsetLine() const { return polygonOffsetLine_; }

  bool setPolygonOffsetLine(bool b) {
    if (b != polygonOffsetLine_) {
      std::swap(polygonOffsetLine_, b);

      if (polygonOffsetLine_)
        glEnable(GL_POLYGON_OFFSET_LINE);
      else
        glDisable(GL_POLYGON_OFFSET_LINE);
    }

    return b;
  }

  //---

  bool isMultiSample() const { return multiSample_; }

  bool setMultiSample(bool b) {
    if (b != multiSample_) {
      std::swap(multiSample_, b);

      if (multiSample_)
        glEnable(GL_MULTISAMPLE);
      else
        glDisable(GL_MULTISAMPLE);
    }

    return b;
  }

  //---

  bool isStencilTest() const { return stenclTest_; }

  bool setStencilTest(bool b) {
    if (b != stenclTest_) {
      std::swap(stenclTest_, b);

      if (stenclTest_)
        glEnable(GL_STENCIL_TEST);
      else
        glDisable(GL_STENCIL_TEST);
    }

    return b;
  }

  //---

  int getFrontFace() const { return frontFace_; }

  int setFrontFace(int frontFace) {
    assert(frontFace == GL_CW || frontFace == GL_CCW);

    if (frontFace != frontFace_) {
      std::swap(frontFace_, frontFace);

      glFrontFace(frontFace_);
    }

    return frontFace;
  }

  //---

  bool isDepthMask() const { return stenclTest_; }

  bool setDepthMask(bool b) {
    if (b != depthMask_) {
      std::swap(depthMask_, b);

      if (depthMask_)
        glDepthMask(GL_TRUE);
      else
        glDepthMask(GL_FALSE);
    }

    return b;
  }

 private:
  CQGLState() { }

 private:
  bool depthTest_         { false };
  bool cullFace_          { false };
  bool blend_             { false };
  bool polygonOffsetLine_ { false };
  bool multiSample_       { false };
  bool stenclTest_        { false };
  int  frontFace_         { GL_CCW };
  bool depthMask_         { true };   // default enabled
};

#endif
