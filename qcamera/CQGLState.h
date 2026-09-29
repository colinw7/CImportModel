#ifndef CQGLState_H
#define CQGLState_H

#define CQGLStateInst CQGLState::instance()

#include <iostream>

#include <GL/glut.h>

class CQGLState {
 public:
  class BoolMap {
   public:
    BoolMap() { }

    bool get(int i) const {
      auto ps = state_.find(i);
      if (ps == state_.end())
        return false;
      return (*ps).second;
    }

    bool set(int i, bool b) {
      bool old = get(i);
      state_[i] = b;
      return old;
    }

   private:
    std::map<int, bool> state_;
  };

 public:
  static CQGLState *instance() {
    static CQGLState *inst;

    if (! inst)
      inst = new CQGLState;

    return inst;
  }

 ~CQGLState() { }

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

  bool isEnableLighting() const { return lighting_; }

  bool setEnableLighting(bool b) {
    if (b != lighting_) {
      std::swap(lighting_, b);

      if (lighting_)
        glEnable(GL_LIGHTING);
      else
        glDisable(GL_LIGHTING);
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

  bool setFrontFaceFlag(bool b) {
    auto oldFrontFace = setFrontFace(b ? GL_CCW : GL_CW);
    return (oldFrontFace == GL_CCW);
  }

  //---

  bool isSmoothShade() const { return smooth_; }

  bool setSmoothShade(bool b) {
    if (b != smooth_) {
      std::swap(smooth_, b);

      if (smooth_)
        glShadeModel(GL_SMOOTH);
      else
        glShadeModel(GL_FLAT);
    }

    return b;
  }

  //---

  int getPolygonMode() const { return polygonMode_; }

  int setPolygonMode(int mode) {
    if (mode != polygonMode_) {
      glPolygonMode(GL_FRONT_AND_BACK, mode);

      std::swap(polygonMode_, mode);
    }

    return mode;
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

  bool isEnableTexture() const { return texture_; }

  bool setEnableTexture(bool b) {
    if (b != texture_) {
      std::swap(texture_, b);

      if (texture_)
        glEnable(GL_TEXTURE_2D);
      else
        glDisable(GL_TEXTURE_2D);
    }

    return b;
  }

  //---

  bool isActiveTextureNum(int i) const { return textureNum_.get(i); }

  bool setActiveTextureNum(int i, bool b) {
    if (b != textureNum_.get(i)) {
      if (b)
        glActiveTexture(GL_TEXTURE0 + i);
      else
        glActiveTexture(GL_TEXTURE0);

      b = textureNum_.set(i, b);
    }

    return b;
  }

  //---

  bool isEnableCubeMap() const { return cubeMap_; }

  bool setEnableCubeMap(bool b) {
    if (b != cubeMap_) {
      std::swap(cubeMap_, b);

      if (cubeMap_) {
        glEnable(GL_TEXTURE_CUBE_MAP);
        glEnable(GL_TEXTURE_CUBE_MAP_EXT);
      }
      else {
        glDisable(GL_TEXTURE_CUBE_MAP);
        glDisable(GL_TEXTURE_CUBE_MAP_EXT);
      }
    }

    return b;
  }

  //---

  bool isEnableClip(int i) const { return clip_.get(i); }

  bool setEnableClip(int i, bool b) {
    if (b != clip_.get(i)) {
      if (b)
        glEnable(GL_CLIP_DISTANCE0 + i);
      else
        glDisable(GL_CLIP_DISTANCE0 + i);

      b = clip_.set(i, b);
    }

    return b;
  }

  //---

  bool checkError(const char *msg) {
    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
      std::cerr << "OpenGL Error: " << gluErrorString(err) << "(" << msg << ")\n";
      return false;
    }

    return true;
  }

 private:
  CQGLState() { }

 private:
  bool    depthTest_         { false };
  bool    cullFace_          { false };
  bool    blend_             { false };
  bool    polygonOffsetLine_ { false };
  bool    multiSample_       { false };
  bool    stenclTest_        { false };
  int     frontFace_         { GL_CCW };
  bool    depthMask_         { true };   // default true (GL_CCW)
  bool    texture_           { false };
  bool    cubeMap_           { false };
  bool    lighting_          { false };
  bool    smooth_            { true };   // default enabled
  BoolMap textureNum_;
  BoolMap clip_;
  int     polygonMode_       { GL_FILL };
};

#endif
