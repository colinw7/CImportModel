#include <CQCamera3DGeomFace.h>

CQCamera3DGeomFace::
CQCamera3DGeomFace()
{
}

CQCamera3DGeomFace::
CQCamera3DGeomFace(const CQCamera3DGeomFace &face) :
 CGeomFace3D(face)
{
}

const CQCamera3DGeomFace::Edges &
CQCamera3DGeomFace::
edges() const
{
  if (! edgesValid_) {
    auto *th = const_cast<CQCamera3DGeomFace *>(this);

    const auto &vertices = th->getVertices();

    auto nv = vertices.size();

    if (nv > 0) {
      th->edges_.resize(nv);

      size_t i1 = nv - 1;

      for (size_t i2 = 0; i2 < nv; i1 = i2++)
        th->edges_[i1] = Edge(vertices[i1], vertices[i2]);
    }
    else
      th->edges_.clear();

    th->edgesValid_ = true;
  }

  return edges_;
}

CQCamera3DGeomFace *
CQCamera3DGeomFace::
dup() const
{
  return new CQCamera3DGeomFace(*this);
}

const CQCamera3DGeomFace::Edge &
CQCamera3DGeomFace::
edge(int i) const
{
  const auto &edges = this->edges();

  return edges[i];
}

void
CQCamera3DGeomFace::
selectEdge(int i)
{
  selectedEdges_.clear();

  selectedEdges_.push_back(i);
}
