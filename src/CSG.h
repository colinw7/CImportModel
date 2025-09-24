#ifndef CSG_H
#define CSG_H

#include <vector>
#include <ostream>
#include <cmath>

namespace CSG {

// Constructive Solid Geometry (CSG) is a modeling technique that uses Boolean
// operations like union and intersection to combine 3D solids. This library
// implements CSG operations on meshes elegantly and concisely using BSP trees,
// and is meant to serve as an easily understandable implementation of the
// algorithm. All edge cases involving overlapping coplanar polygons in both
// solids are correctly handled.
//
// Example usage:
//
//     CSG *cube    = new CSG::cube();
//     CSG *sphere  = new CSG::sphere({ radius: 1.3 });
//
//     PolygonArray polygons = cube->subtract(sphere)->toPolygons();
//
// ## Implementation Details
//
// All CSG operations are implemented in terms of two functions, `clipTo()` and
// `invert()`, which remove parts of a BSP tree inside another BSP tree and swap
// solid and empty space, respectively. To find the union of `a` and `b`, we
// want to remove everything in `a` inside `b` and everything in `b` inside `a`,
// then combine polygons from `a` and `b` into one solid:
//
//     a.root.clipTo(b.root);
//     b.root.clipTo(a.root);
//     a.root.build(b.root.allPolygons());
//
// The only tricky part is handling overlapping coplanar polygons in both trees.
// The code above keeps both copies, but we need to keep them in one tree and
// remove them in the other tree. To remove them from `b` we can clip the
// inverse of `b` against `a`. The code for union now looks like this:
//
//     a.root.clipTo(b.root);
//     b.root.clipTo(a.root);
//     b.root.invert();
//     b.root.clipTo(a.root);
//     b.root.invert();
//     a.root.build(b.root.allPolygons());
//
// Subtraction and intersection naturally follow from set operations. If
// union is `A | B`, subtraction is `A - B = ~(~A | B)` and intersection is
// `A & B = ~(~A | ~B)` where `~` is the complement operator.
//
// ## License
//
// Copyright (c) 2011 Evan Wallace (http://madebyevan.com/), under the MIT license.

class Polygon;
class Node;

// # class Vector

// Represents a 3D vector.
//
// Example usage:
//
//     Vector(1, 2, 3);
//     Vector([1, 2, 3]);
//     Vector({ x: 1, y: 2, z: 3 });

class Vector {
 public:
  Vector(double x_=0.0, double y_=0.0, double z_=0.0) {
    x = x_;
    y = y_;
    z = z_;
  }

  Vector(const double *v) {
    x = v[0];
    y = v[1];
    z = v[2];
  }

  Vector negated() const {
    return Vector(-x, -y, -z);
  }

  Vector plus(const Vector &a) const {
    return Vector(x + a.x, y + a.y, z + a.z);
  }

  Vector minus(const Vector &a) const {
    return Vector(x - a.x, y - a.y, z - a.z);
  }

  Vector times(double a) const {
    return Vector(x*a, y*a, z*a);
  }

  Vector dividedBy(double a) const {
    return Vector(x/a, y/a, z/a);
  }

  double dot(const Vector &a) const {
    return x*a.x + y*a.y + z*a.z;
  }

  Vector lerp(const Vector &a, double t) const {
    return plus(a.minus(*this).times(t));
  }

  double length() const {
    return sqrt(dot(*this));
  }

  Vector unit() const {
    return dividedBy(length());
  }

  Vector cross(const Vector &a) const {
    return Vector(y * a.z - z * a.y,
                  z * a.x - x * a.z,
                  x * a.y - y * a.x);
  }

  friend std::ostream &operator<<(std::ostream &os, const Vector &v) {
    os << "(" << v.x << "," << v.y << "," << v.z << ")";

    return os;
  }

 public:
  double x { 0.0 };
  double y { 0.0 };
  double z { 0.0 };
};

// # class Vertex

// Represents a vertex of a polygon. Use your own vertex class instead of this
// one to provide additional features like texture coordinates and vertex
// colors. Custom vertex classes need to provide a `pos` property and `clone()`,
// `flip()`, and `interpolate()` methods that behave analogous to the ones
// defined by `CSG.Vertex`. This class provides `normal` so convenience
// functions like `CSG.sphere()` can return a smooth vertex normal, but `normal`
// is not used anywhere else.

class Vertex {
 public:
  Vertex(const Vector &pos_, const Vector &normal_) {
    pos    = pos_;
    normal = normal_;
  }

  Vertex *clone() const;

  // Invert all orientation-specific data (e.g. vertex normal). Called when the
  // orientation of a polygon is flipped.
  Vertex *flip() {
    normal = normal.negated();

    return this;
  }

  // Create a new vertex between this vertex and `other` by linearly
  // interpolating all properties using a parameter of `t`. Subclasses should
  // override this to interpolate additional properties.
  Vertex *interpolate(const Vertex &other, double t);

  friend std::ostream &operator<<(std::ostream &os, const Vertex &v) {
    os << v.pos;

    return os;
  }

 public:
  Vector pos;
  Vector normal;
};

// # class Polygon Array

class PolygonArray {
 public:
  typedef std::vector<Polygon *>::reference       reference;
  typedef std::vector<Polygon *>::const_reference const_reference;
  typedef Polygon*                                value_type;

  PolygonArray() { }
 ~PolygonArray();

  uint size() const { return uint(a.size()); }

  void push_back(Polygon *p) { a.push_back(p); }

  void clear() { a.clear(); }

  Polygon *operator[](int i) const { return a[i]; }

  std::vector<Polygon *>::const_iterator begin() const { return a.begin(); }
  std::vector<Polygon *>::const_iterator end  () const { return a.end  (); }

  std::vector<Polygon *>::iterator begin() { return a.begin(); }
  std::vector<Polygon *>::iterator end  () { return a.end  (); }

 public:
  std::vector<Polygon *> a;
};

class CSG;

class Factory {
 public:
  Factory() { }

  virtual ~Factory() { }

  virtual CSG     *createCSG    () = 0;
  virtual Node    *createNode   () = 0;
  virtual Vertex  *createVertex (const Vector &p, const Vector &n) = 0;
  virtual Polygon *createPolygon(const std::vector<Vertex *> &v, const Polygon *p) = 0;
};

class Plane;

class Mgr {
 public:
  static Mgr *getInstance();

  Mgr() { }

 ~Mgr();

  CSG     *createCSG    ();
  Node    *createNode   ();
  Vertex  *createVertex (const Vector &p, const Vector &n);
  Polygon *createPolygon(const std::vector<Vertex *> &v, const Polygon *p=nullptr);
  Plane   *createPlane  (const Vector &n, double w);

  void setFactory(Factory *factory) {
    factory_ = factory;
  }

 protected:
  using PolygonArray = std::vector<Polygon *>;

  Factory*     factory_ { nullptr };
  PolygonArray polygons_;
};

// # class Plane

// Represents a plane in 3D space.

class Plane {
 public:
  Plane(const Vector &normal_=Vector(0,0,0), double w_=0.0) {
    normal = normal_;
    w      = w_;
  };

  Plane *clone() const;

  // `CSG.Plane.EPSILON` is the tolerance used by `splitPolygon()` to decide if a
  // point is on the plane.
  double EPSILON() const { return 1e-5; }

  static Plane *fromPoints(const Vector &a, const Vector &b, const Vector &c);

  void flip();

  // Split `polygon` by this plane if needed, then put the polygon or polygon
  // fragments in the appropriate lists. Coplanar polygons go into either
  // `coplanarFront` or `coplanarBack` depending on their orientation with
  // respect to this plane. Polygons in front or in back of this plane go into
  // either `front` or `back`.
  void splitPolygon(Polygon *polygon, PolygonArray &coplanarFront, PolygonArray &coplanarBack,
                    PolygonArray &front, PolygonArray &back);

 protected:
  Vector normal;
  double w { 0.0 };
};

// # class Polygon

// Represents a convex polygon. The vertices used to initialize a polygon must
// be coplanar and form a convex loop. They do not have to be `CSG.Vertex`
// instances but they must behave similarly (duck typing can be used for
// customization).
//
// Each convex polygon has a `shared` property, which is shared between all
// polygons that are clones of each other or were split from the same polygon.
// This can be used to define per-polygon properties (such as surface color).

class Polygon {
 public:
  Polygon(const std::vector<Vertex *> &vertices_, const Polygon *);

  virtual ~Polygon();

  Polygon *clone() const;

  void flip();

  friend std::ostream &operator<<(std::ostream &os, const Polygon &p);

 public:
  std::vector<Vertex *> vertices;
  Plane*                plane { nullptr };
};

// # class CSG

// Holds a binary space partition tree representing a 3D solid. Two solids can
// be combined using the `union()`, `subtract()`, and `intersect()` methods.

class CSG {
 public:
  CSG();

  virtual ~CSG();

  CSG *clone() const;

  PolygonArray toPolygons();

  CSG *unionOp(CSG *bsp);
  CSG *subtractOp(CSG *bsp);
  CSG *intersectOp(CSG *bsp);
  CSG *inverseOp();

  Node *root() const { return root_; }

  friend std::ostream &operator<<(std::ostream &os, const CSG &csg);

 protected:
  Node *root_ { nullptr };
};

// # class Node

// Holds a node in a BSP tree. A BSP tree is built from a collection of polygons
// by picking a polygon to split along. That polygon (and all other coplanar
// polygons) are added directly to that node and the other polygons are added to
// the front and/or back subtrees. This is not a leafy BSP tree since there is
// no distinction between internal and leaf nodes.

class Node {
 public:
  Node();

  virtual ~Node();

  Node *clone() const;

  void build(const PolygonArray &polygons);

  PolygonArray allPolygons();

  void invert();

  void clipTo(Node *bsp);

  PolygonArray clipPolygons(const PolygonArray &polygons);

  friend std::ostream &operator<<(std::ostream &os, const Node &n) {
    os << "F: ";
    if (n.front) os << "(" << *n.front << ")"; else os << "NULL";
    os << " B: ";
    if (n.back ) os << "(" << *n.back  << ")"; else os << "NULL";

    int np = n.polygons.size();

    for (int i = 0; i < np; ++i) {
      if (i > 0) os << " ";
      os << *n.polygons[i];
    }

    return os;
  }

 protected:
  Plane *      plane { nullptr };
  Node *       front { nullptr };
  Node *       back  { nullptr };
  PolygonArray polygons;
};

//---

CSG *fromPolygons(const PolygonArray &polygons);

std::vector<Vertex *> toVertices(Vertex *v1, Vertex *v2, Vertex *v3);
std::vector<Vertex *> toVertices(Vertex *v1, Vertex *v2, Vertex *v3, Vertex *v4);

CSG *cube(const Vector &center=Vector(0, 0, 0), double radius=1.0);

CSG *sphere(const Vector &center=Vector(0, 0, 0), double radius=1.0,
            int slices = 48, int stacks = 32);

CSG *cylinder(const Vector &start=Vector(0, -1, 0), const Vector &end=Vector(0, 1, 0),
              double r=1.0, int slices = 48);

}

#define CSGMgrInst ::CSG::Mgr::getInstance()

#endif
