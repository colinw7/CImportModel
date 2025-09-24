#include <CSG.h>
#include <algorithm>
#include <cmath>

namespace CSG {

//---

CSG::
CSG()
{
  root_ = CSGMgrInst->createNode();
}

CSG::
~CSG()
{
  delete root_;
}

CSG *
CSG::
clone() const
{
  auto *bsp = CSGMgrInst->createCSG();

  bsp->root_ = root_->clone();

  return bsp;
}

PolygonArray
CSG::
toPolygons()
{
  return root_->allPolygons();
}

// Return a new CSG solid representing space in either this solid or in the
// solid `bsp`. Neither this solid nor the solid `bsp` are modified.
//
//     A.union(B)
//
//     +-------+            +-------+
//     |       |            |       |
//     |   A   |            |       |
//     |    +--+----+   =   |       +----+
//     +----+--+    |       +----+       |
//          |   B   |            |       |
//          |       |            |       |
//          +-------+            +-------+
//

CSG *
CSG::
unionOp(CSG *bsp)
{
  auto *a = clone();
  auto *b = bsp->clone();

  a->root_->clipTo(b->root_);
  b->root_->clipTo(a->root_);
  b->root_->invert();
  b->root_->clipTo(a->root_);
  b->root_->invert();

  a->root_->build(b->root_->allPolygons());

  delete b;

  return a;
}

// Return a new CSG solid representing space in this solid but not in the
// solid `bsp`. Neither this solid nor the solid `bsp` are modified.
//
//     A.subtract(B)
//
//     +-------+            +-------+
//     |       |            |       |
//     |   A   |            |       |
//     |    +--+----+   =   |    +--+
//     +----+--+    |       +----+
//          |   B   |
//          |       |
//          +-------+
//
CSG *
CSG::
subtractOp(CSG *bsp)
{
  auto *a = clone();
  auto *b = bsp->clone();

  a->root_->invert();
  a->root_->clipTo(b->root_);
  b->root_->clipTo(a->root_);
  b->root_->invert();
  b->root_->clipTo(a->root_);
  b->root_->invert();
  a->root_->build (b->root_->allPolygons());
  a->root_->invert();

  delete b;

  return a;
}

// Return a new CSG solid representing space both this solid and in the
// solid `bsp`. Neither this solid nor the solid `bsp` are modified.
//
//     A.intersect(B)
//
//     +-------+
//     |       |
//     |   A   |
//     |    +--+----+   =   +--+
//     +----+--+    |       +--+
//          |   B   |
//          |       |
//          +-------+
//
CSG *
CSG::
intersectOp(CSG *bsp)
{
  auto *a = clone();
  auto *b = bsp->clone();

  a->root_->invert();
  b->root_->clipTo(a->root_);
  b->root_->invert();
  a->root_->clipTo(b->root_);
  b->root_->clipTo(a->root_);
  a->root_->build (b->root_->allPolygons());
  a->root_->invert();

  delete b;

  return a;
}

// Return a new CSG solid with solid and empty space switched. This solid is
// not modified.
CSG *
CSG::
inverseOp()
{
  auto *bsp = clone();

  bsp->root_->invert();

  return bsp;
}

// Construct an axis-aligned solid cube. Optional parameters are `center` and
// `radius`, which default to `[0, 0, 0]` and `1`.
//
// Example code:
//
//     var cube = CSG.cube({
//       center: [0, 0, 0],
//       radius: 1
//     });
CSG *
cube(const Vector &c, double r)
{
  static double normals[][3] = {
    {-1,  0,  0},
    {+1,  0,  0},
    { 0, -1,  0},
    { 0, +1,  0},
    { 0,  0, -1},
    { 0,  0, +1}
  };

  static int faces[][4] = {
    {0, 4, 6, 2},
    {1, 3, 7, 5},
    {0, 1, 5, 4},
    {2, 6, 7, 3},
    {0, 2, 3, 1},
    {4, 5, 7, 6}
  };

  PolygonArray polygons;

  for (int i = 0; i < 6; ++i) {
    std::vector<Vertex *> vertices;

    auto n = Vector(normals[i][0], normals[i][1], normals[i][2]);

    int *faces1 = faces[i];

    for (int j = 0; j < 4; ++j) {
      Vector pos(c.x + r*(2*(faces1[j] & 1 ? 1 : 0) - 1),
                 c.y + r*(2*(faces1[j] & 2 ? 1 : 0) - 1),
                 c.z + r*(2*(faces1[j] & 4 ? 1 : 0) - 1));

      auto *v = CSGMgrInst->createVertex(pos, n);

      vertices.push_back(v);
    }

    auto *p = CSGMgrInst->createPolygon(vertices);

    polygons.push_back(p);
  }

  return fromPolygons(polygons);
}

// Construct a solid sphere. Optional parameters are `center`, `radius`,
// `slices`, and `stacks`, which default to `[0, 0, 0]`, `1`, `16`, and `8`.
// The `slices` and `stacks` parameters control the tessellation along the
// longitude and latitude directions.
//
// Example usage:
//
//     var sphere = CSG.sphere({
//       center: [0, 0, 0],
//       radius: 1,
//       slices: 16,
//       stacks: 8
//     });
CSG *
sphere(const Vector &c, double r, int slices, int stacks)
{
  using Verices = std::vector<Vertex *>;

  Verices vertices;

  struct VertexHelper {
    VertexHelper(const Vector &c_, double r_, Verices &vertices_) :
     c(c_), r(r_), vertices(vertices_) {
    }

    void operator()(double theta, double phi) {
      theta *= M_PI*2.0;
      phi   *= M_PI;

      Vector dir(std::cos(theta)*std::sin(phi), std::cos(phi), std::sin(theta)*std::sin(phi));

      vertices.push_back(CSGMgrInst->createVertex(c.plus(dir.times(r)), dir));
    }

    Vector   c;
    double   r;
    Verices &vertices;
  };

  VertexHelper vertex(c, r, vertices);

  PolygonArray polygons;

  for (int i = 0; i < slices; i++) {
    for (int j = 0; j < stacks; j++) {
      vertices.clear();

      vertex((1.0*i)/slices, (1.0*j)/stacks);

      if (j > 0         ) vertex((1.0*i + 1)/slices, (1.0*j    )/stacks);
      if (j < stacks - 1) vertex((1.0*i + 1)/slices, (1.0*j + 1)/stacks);

      vertex((1.0*i)/slices, (1.0*j + 1)/stacks);

      polygons.push_back(CSGMgrInst->createPolygon(vertices));
    }
  }

  return fromPolygons(polygons);
}

// Construct a solid cylinder. Optional parameters are `start`, `end`,
// `radius`, and `slices`, which default to `[0, -1, 0]`, `[0, 1, 0]`, `1`, and
// `16`. The `slices` parameter controls the tessellation.
//
// Example usage:
//
//     var cylinder = CSG.cylinder({
//       start: [0, -1, 0],
//       end: [0, 1, 0],
//       radius: 1,
//       slices: 16
//     });

CSG *
cylinder(const Vector &s, const Vector &e, double r, int slices)
{
  auto ray = e.minus(s);

  auto axisZ = ray.unit(); bool isY = (std::abs(axisZ.y) > 0.5);
  auto axisX = Vector(isY, !isY, 0).cross(axisZ).unit();
  auto axisY = axisX.cross(axisZ).unit();

  auto *start = CSGMgrInst->createVertex(s, axisZ.negated());
  auto *end   = CSGMgrInst->createVertex(e, axisZ.unit   ());

  struct PointHelper {
    PointHelper(const Vector &s_, const Vector &ray_, double r_, const Vector &axisX_,
                const Vector &axisY_, const Vector &axisZ_) :
     s(s_), ray(ray_), axisX(axisX_), axisY(axisY_), axisZ(axisZ_), r(r_) {
    }

    Vertex *operator()(int stack, double slice, int normalBlend) {
      double angle = slice*M_PI*2;

      auto out    = axisX.times(cos(angle)).plus(axisY.times(sin(angle)));
      auto pos    = s.plus(ray.times(stack)).plus(out.times(r));
      auto normal = out.times(1 - std::abs(normalBlend)).plus(axisZ.times(normalBlend));

      return CSGMgrInst->createVertex(pos, normal);
    }

    Vector s, ray, axisX, axisY, axisZ;
    double r;
  };

  PolygonArray polygons;

  PointHelper point(s, ray, r, axisX, axisY, axisZ);

  for (int i = 0; i < slices; i++) {
    double t0 = double(i    )/slices;
    double t1 = double(i + 1)/slices;

    polygons.push_back(CSGMgrInst->createPolygon(
      toVertices(start, point(0, t0, -1), point(0, t1, -1))));
    polygons.push_back(CSGMgrInst->createPolygon(
      toVertices(point(0, t1, 0), point(0, t0, 0), point(1, t0, 0), point(1, t1, 0))));
    polygons.push_back(CSGMgrInst->createPolygon(
      toVertices(end, point(1, t1, 1), point(1, t0, 1))));
  }

  return fromPolygons(polygons);
}

std::ostream &operator<<(std::ostream &os, const CSG &csg) {
  os << *csg.root_;

  return os;
}

//--------------

Node::
Node()
{
  plane = nullptr;
  front = nullptr;
  back  = nullptr;
}

Node::
~Node()
{
  delete plane;
  delete front;
  delete back;
}

Node *
Node::
clone() const
{
  Node *node = CSGMgrInst->createNode();

  node->plane = (plane ? plane->clone() : nullptr);
  node->front = (front ? front->clone() : nullptr);
  node->back  = (back  ? back ->clone() : nullptr);

  int n = polygons.size();

  for (int i = 0; i < n; ++i)
    node->polygons.push_back(polygons[i]->clone());

  return node;
}

// Convert solid space to empty space and empty space to solid space.
void
Node::
invert()
{
  for (int i = 0; i < int(polygons.size()); i++)
    polygons[i]->flip();

  plane->flip();

  if (front) front->invert();
  if (back ) back ->invert();

  std::swap(front, back);
}

// Recursively remove all polygons in `polygons` that are inside this BSP
// tree.
PolygonArray
Node::
clipPolygons(const PolygonArray &apolygons)
{
  PolygonArray front1, back1;

  for (int i = 0; i < int(apolygons.size()); i++) {
    plane->splitPolygon(apolygons[i], front1, back1, front1, back1);
  }

  if (this->front) front1 = this->front->clipPolygons(front1);
  if (this->back ) back1  = this->back ->clipPolygons(back1 );
  else             back1.clear();

  std::copy(back1.begin(), back1.end(), std::back_inserter(front1));

  return front1;
}

// Remove all polygons in this BSP tree that are inside the other BSP tree
// `bsp`.
void
Node::
clipTo(Node *bsp)
{
  polygons = bsp->clipPolygons(polygons);

  if (front) front->clipTo(bsp);
  if (back ) back ->clipTo(bsp);
}

// Return a list of all polygons in this BSP tree.
PolygonArray
Node::
allPolygons()
{
  PolygonArray apolygons = this->polygons;

  if (front) {
    PolygonArray fp = front->allPolygons();
    std::copy(fp.begin(), fp.end(), std::back_inserter(apolygons));
  }

  if (back ) {
    PolygonArray bp = back->allPolygons();
    std::copy(bp.begin(), bp.end(), std::back_inserter(apolygons));
  }

  return apolygons;
}

// Build a BSP tree out of `polygons`. When called on an existing tree, the
// new polygons are filtered down to the bottom of the tree and become new
// nodes there. Each set of polygons is partitioned using the first polygon
// (no heuristic is used to pick a good split).
void
Node::
build(const PolygonArray &bpolygons)
{
  if (! bpolygons.size()) return;

  if (! plane) plane = bpolygons[0]->plane->clone();

  PolygonArray front1, back1;

  int n = bpolygons.size();

  for (int i = 0; i < n; i++) {
    plane->splitPolygon(bpolygons[i], this->polygons, this->polygons, front1, back1);
  }

  if (front1.size()) {
    if (! this->front) this->front = CSGMgrInst->createNode();
    this->front->build(front1);
  }

  if (back1.size()) {
    if (! this->back) this->back = CSGMgrInst->createNode();
    this->back->build(back1);
  }
}

//-------------------------

Plane *
Plane::
clone() const
{
  return CSGMgrInst->createPlane(normal, w);
}

Plane *
Plane::
fromPoints(const Vector &a, const Vector &b, const Vector &c)
{
  Vector n = b.minus(a).cross(c.minus(a)).unit();

  return CSGMgrInst->createPlane(n, n.dot(a));
}

void
Plane::
flip()
{
  normal = normal.negated();
  w      = -w;
}

void
Plane::
splitPolygon(Polygon *polygon, PolygonArray &coplanarFront, PolygonArray &coplanarBack,
             PolygonArray &front, PolygonArray &back)
{
  static const int COPLANAR = 0;
  static const int FRONT    = 1;
  static const int BACK     = 2;
  static const int SPANNING = 3;

  // Classify each point as well as the entire polygon into one of the above
  // four classes.
  int polygonType = 0;

  std::vector<int> types;

  auto n = polygon->vertices.size();

  types.resize(n);

  for (uint i = 0; i < n; i++) {
    double t = normal.dot(polygon->vertices[i]->pos) - w;

    int type = COPLANAR;

    if (t < -Plane::EPSILON()) type = BACK;
    if (t >  Plane::EPSILON()) type = FRONT;

    polygonType |= type;

    types[i] = type;
  }

  // Put the polygon in the correct list, splitting it when necessary.
  switch (polygonType) {
    case COPLANAR: {
      double t = normal.dot(polygon->plane->normal);
      if (t > 0) coplanarFront.push_back(polygon);
      else       coplanarBack .push_back(polygon);
      break;
    }
    case FRONT:
      front.push_back(polygon);
      break;
    case BACK:
      back.push_back(polygon);
      break;
    case SPANNING: {
      std::vector<Vertex *> f, b;

      auto np = polygon->vertices.size();

      for (uint i = 0; i < np; i++) {
        uint j = (i + 1) % uint(np);

        int     ti = types[i]            ,  tj = types[j];
        Vertex *vi = polygon->vertices[i], *vj = polygon->vertices[j];

        if (ti != BACK ) f.push_back(vi);
        if (ti != FRONT) b.push_back(ti != BACK ? vi->clone() : vi);

        if ((ti | tj) == SPANNING) {
          double t = (w - normal.dot(vi->pos)) / normal.dot(vj->pos.minus(vi->pos));

          Vertex *v = vi->interpolate(*vj, t);

          f.push_back(v);
          b.push_back(v->clone());
        }
      }

      if (f.size() >= 3) front.push_back(CSGMgrInst->createPolygon(f, polygon));
      if (b.size() >= 3) back .push_back(CSGMgrInst->createPolygon(b, polygon));

      break;
    }
  }
}

//------

Polygon::
Polygon(const std::vector<Vertex *> &vertices_, const Polygon *)
{
  vertices = vertices_;
  plane    = Plane::fromPoints(vertices[0]->pos, vertices[1]->pos, vertices[2]->pos);
}

Polygon::
~Polygon()
{
  auto n = vertices.size();

  for (uint i = 0; i < n; ++i)
    delete vertices[i];

  delete plane;
}

Polygon *
Polygon::
clone() const
{
  return CSGMgrInst->createPolygon(vertices, this);
}

void
Polygon::
flip()
{
  auto n = vertices.size();

  std::vector<Vertex *> vertices1;

  for (int i = int(n - 1); i >= 0; --i)
    vertices1.push_back(vertices[i]->flip());

  vertices = vertices1;

  plane->flip();
}

std::ostream &
operator<<(std::ostream &os, const Polygon &p)
{
  auto n = p.vertices.size();

  for (uint i = 0; i < n; ++i) {
    if (i > 0) os << " ";

    os << *p.vertices[i];
  }

  return os;
}

//------

Vertex *
Vertex::
clone() const
{
  return CSGMgrInst->createVertex(pos, normal);
}

Vertex *
Vertex::
interpolate(const Vertex &other, double t)
{
  return CSGMgrInst->createVertex(pos.lerp(other.pos, t), normal.lerp(other.normal, t));
}

//------

PolygonArray::
~PolygonArray()
{
  //auto n = a.size();

  //for (int i = 0; i < n; ++i) delete a[i];
}

//------

Mgr *
Mgr::
getInstance()
{
  static Mgr *instance;

  if (! instance)
    instance = new Mgr;

  return instance;
}

Mgr::
~Mgr()
{
  for (auto *polygon : polygons_)
    delete polygon;
}

CSG *
Mgr::
createCSG()
{
  if (factory_)
    return factory_->createCSG();
  else
    return new CSG;
}

Node *
Mgr::
createNode()
{
  if (factory_)
    return factory_->createNode();
  else
    return new Node;
}

Vertex *
Mgr::
createVertex(const Vector &p, const Vector &n)
{
  if (factory_)
    return factory_->createVertex(p, n);
  else
    return new Vertex(p, n);
}

Polygon *
Mgr::
createPolygon(const std::vector<Vertex *> &v, const Polygon *ref)
{
  Polygon *p;

  if (factory_)
    p = factory_->createPolygon(v, ref);
  else
    p = new Polygon(v, ref);

  polygons_.push_back(p);

  return p;
}

Plane *
Mgr::
createPlane(const Vector &normal, double w)
{
  return new Plane(normal, w);
}

//---

// Construct a CSG solid from a list of `CSG.Polygon` instances.
CSG *
fromPolygons(const PolygonArray &polygons)
{
  auto *bsp = CSGMgrInst->createCSG();

  bsp->root()->build(polygons);

  return bsp;
}

std::vector<Vertex *>
toVertices(Vertex *v1, Vertex *v2, Vertex *v3)
{
  std::vector<Vertex *> vertices;

  vertices.push_back(v1); vertices.push_back(v2); vertices.push_back(v3);

  return vertices;
}

std::vector<Vertex *>
toVertices(Vertex *v1, Vertex *v2, Vertex *v3, Vertex *v4)
{
  std::vector<Vertex *> vertices;

  vertices.push_back(v1); vertices.push_back(v2); vertices.push_back(v3); vertices.push_back(v4);

  return vertices;
}

}
