#ifndef CQCamera3DGeomFace_H
#define CQCamera3DGeomFace_H

#include <CGeomFace3D.h>
#include <CGeomFace3D.h>

class CQCamera3DGeomFace : public CGeomFace3D {
 public:
  CQCamera3DGeomFace();

  CQCamera3DGeomFace(const CQCamera3DGeomFace &face);

  CQCamera3DGeomFace &operator=(const CQCamera3DGeomFace &rhs) = delete;

  CQCamera3DGeomFace *dup() const override;

  void setVertices(const VertexList &vertices) override {
    CGeomFace3D::setVertices(vertices);
  }

  //---

  void clearPoints() { points_.clear(); }

  void addPoint(const CPoint3D &p) { points_.push_back(p); }

  CPoint3D pointCenter() const {
    CPoint3D c(0, 0, 0);

    if (points_.empty())
      return c;

    for (const auto &p : points_)
      c += p;

    c /= points_.size();

    return c;
  }

 private:
  using Points = std::vector<CPoint3D>;

  bool   edgesValid_ { false };
  Points points_;
};

#endif
