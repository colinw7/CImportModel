#ifndef CQCamera3DGeomFace_H
#define CQCamera3DGeomFace_H

#include <CGeomFace3D.h>
#include <CGeomFace3D.h>

class CQCamera3DGeomFace : public CGeomFace3D {
 public:
  struct Edge {
    Edge() { }

    Edge(int iv1, int iv2) :
     v1(iv1), v2(iv2) {
    }

    int v1 { 0 };
    int v2 { 0 };
  };

  using Edges    = std::vector<Edge>;
  using EdgeInds = std::vector<int>;

 public:
  CQCamera3DGeomFace();

  CQCamera3DGeomFace(const CQCamera3DGeomFace &face);

  CQCamera3DGeomFace &operator=(const CQCamera3DGeomFace &rhs) = delete;

  CQCamera3DGeomFace *dup() const override;

  const Edges &edges() const;
  const Edge &edge(int i) const;

  void selectEdge(int i);
  void deselecttEdges() { selectedEdges_.clear(); }

  bool hasSelectedEdge() const { return ! selectedEdges_.empty(); }
  const EdgeInds &selectedEdges() const { return selectedEdges_; }

  void setVertices(const VertexList &vertices) override {
    CGeomFace3D::setVertices(vertices);

    edgesValid_ = false;

    edges_        .clear();
    selectedEdges_.clear();
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

  Edges    edges_;
  EdgeInds selectedEdges_;
  bool     edgesValid_ { false };
  Points   points_;
};

#endif
