#ifndef CQGLBuffer_H
#define CQGLBuffer_H

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

#include <vector>
#include <cassert>

class CQGLBuffer {
 public:
  struct Point {
    float x { 0.0 };
    float y { 0.0 };
    float z { 0.0 };

    Point() { }

    Point(float x, float y, float z) :
     x(x), y(y), z(z) {
    }
  };

  struct Color {
    float r { 0.0 };
    float g { 0.0 };
    float b { 0.0 };

    Color() { }

    Color(float r, float g, float b) :
     r(r), g(g), b(b) {
    }
  };

  struct TexturePoint {
    float x { 0.0 };
    float y { 0.0 };

    TexturePoint() { }

    TexturePoint(float x, float y) :
     x(x), y(y) {
    }
  };

  using Points        = std::vector<Point>;
  using Colors        = std::vector<Color>;
  using TexturePoints = std::vector<TexturePoint>;
  using Indices       = std::vector<int>;

 public:
  enum Parts {
    POINT   = (1<<0),
    NORMAL  = (1<<1),
    COLOR   = (1<<2),
    TEXTURE = (1<<3)
  };

 public:
  CQGLBuffer(QOpenGLShaderProgram *program) {
    data_.program = program;

    data_.vObj         = new QOpenGLVertexArrayObject;
    data_.vertexBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    data_.indBuffer    = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);

    initIds();
  }

 ~CQGLBuffer() {
    term();
  }

  CQGLBuffer(const CQGLBuffer &buffer) {
    initFrom(buffer);
  }

  CQGLBuffer &operator=(const CQGLBuffer &buffer) {
    term();

    initFrom(buffer);

    return *this;
  }

  //---

  void clearPoints() { data_.points.clear(); }
  void clearNormals() { data_.normals.clear(); }
  void clearColors() { data_.colors.clear(); }
  void clearTexturePoints() { data_.texturePoints.clear(); }

  //---

  void addPoint(float x, float y, float z) {
    addPoint(Point(x, y, z));
  }

  void addPoint(const Point &p) {
    data_.types |= static_cast<unsigned int>(Parts::POINT);

    data_.points.push_back(p);

    data_.dataValid = false;
  }

  void addNormal(float x, float y, float z) {
    addNormal(Point(x, y, z));
  }

  void addNormal(const Point &p) {
    data_.types |= static_cast<unsigned int>(Parts::NORMAL);

    data_.normals.push_back(p);

    data_.dataValid = false;
  }

  void addColor(float r, float g, float b) {
    addColor(Color(r, g, b));
  }

  void addColor(const Color &c) {
    data_.types |= static_cast<unsigned int>(Parts::COLOR);

    data_.colors.push_back(c);

    data_.dataValid = false;
  }

  void addTexturePoint(float x, float y) {
    addTexturePoint(TexturePoint(x, y));
  }

  void addTexturePoint(const TexturePoint &p) {
    data_.types |= static_cast<unsigned int>(Parts::TEXTURE);

    data_.texturePoints.push_back(p);

    data_.dataValid = false;
  }

  void addIndex(int i) {
    data_.indices.push_back(i);

    data_.indicesSet = true;
  }

  void load() {
    initData();

    // bind the Vertex Array Object first, then bind and set vertex buffer(s),
    // and then configure vertex attributes(s).
    data_.vObj->bind();

    // send geometry data to buffer
    data_.vertexBuffer->bind();
    data_.vertexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    data_.vertexBuffer->allocate(data_.data, int(data_.numData*sizeof(float)));
    //data_.vertexBuffer->release();

    // send indices data to buffer
    if (data_.indicesSet) {
      data_.indBuffer->bind();
      data_.indBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
      data_.indBuffer->allocate(data_.indData, int(data_.numIndData*sizeof(int)));
      //data_.indBuffer->release();
    }

    int  vid  = 0;
    uint span = 0;

    // store points in vertex array
    if (data_.types & static_cast<unsigned int>(Parts::POINT)) {
      data_.program->setAttributeArray(vid, reinterpret_cast<float *>(span*sizeof(float)),
                                       3, int(data_.span*sizeof(float)));
      data_.program->enableAttributeArray(vid++);
      span += 3;
    }

    // store normals in vertex array
    if (data_.types & static_cast<unsigned int>(Parts::NORMAL)) {
      data_.program->setAttributeArray(vid, reinterpret_cast<float *>(span*sizeof(float)),
                                       3, int(data_.span*sizeof(float)));
      data_.program->enableAttributeArray(vid++);
      span += 3;
    }

    // store colors in vertex array
    if (data_.types & static_cast<unsigned int>(Parts::COLOR)) {
      data_.program->setAttributeArray(vid, reinterpret_cast<float *>(span*sizeof(float)),
                                       3, int(data_.span*sizeof(float)));
      data_.program->enableAttributeArray(vid++);
      span += 3;
    }

    // store texture points in vertex array
    if (data_.types & static_cast<unsigned int>(Parts::TEXTURE)) {
      data_.program->setAttributeArray(vid, reinterpret_cast<float *>(span*sizeof(float)),
                                       2, int(data_.span*sizeof(float)));
      data_.program->enableAttributeArray(vid++);
      span += 2;
    }

    // note that this is allowed, the call to setAttributeBuffer registered VBO as the
    // vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    data_.vertexBuffer->release();
    if (data_.indicesSet)
      data_.indBuffer->release();

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object
    // IS stored in the VAO; keep the EBO bound.
    //data_.vertexBuffer->release();

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO,
    // but this rarely happens. Modifying other VAOs requires a call to glBindVertexArray
    // anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    data_.vObj->release();
  }

  void bind() {
    // seeing as we only have a single VAO there's no need to bind it every time,
    // but we'll do so to keep things a bit more organized
    data_.vObj->bind();
  }

  void unbind() {
    data_.vObj->release();
  }

  void drawTriangles() {
    glDrawArrays(GL_TRIANGLES, 0, int(data_.points.size()));
  }

 private:
  void term() {
    delete data_.vObj;
    delete data_.vertexBuffer;
    delete data_.indBuffer;

    delete [] data_.data;
    delete [] data_.indData;

    data_.vertexBuffer->destroy();
    data_.indBuffer   ->destroy();

    data_.vObj->destroy();

    data_ = Data();
  }

  void initFrom(const CQGLBuffer &buffer) {
    data_ = buffer.data_;

    if (buffer.data_.numData) {
      data_.data = new float [buffer.data_.numData];

      memcpy(data_.data, buffer.data_.data, buffer.data_.numData*sizeof(float));
    }
    else
      data_.data = nullptr;

    if (buffer.data_.numIndData) {
      data_.indData = new int [buffer.data_.numIndData];

      memcpy(data_.indData, buffer.data_.indData, buffer.data_.numIndData*sizeof(int));
    }
    else
      data_.indData = nullptr;

    initIds();
  }

  void initIds() {
    data_.vObj->create();

    data_.vertexBuffer->create();
    data_.indBuffer   ->create();
  }

  void initData() {
    if (! data_.dataValid) {
      delete [] data_.data;
      delete [] data_.indData;

      //---

      data_.numData = 0;

      if (data_.types & static_cast<unsigned int>(Parts::POINT)) {
        data_.numData += data_.points.size()*3;
        data_.span += 3;
      }

      if (data_.types & static_cast<unsigned int>(Parts::NORMAL)) {
        data_.numData += data_.normals.size()*3;
        data_.span += 3;
      }

      if (data_.types & static_cast<unsigned int>(Parts::COLOR)) {
        assert(data_.colors.size() == data_.points.size());
        data_.numData += data_.colors.size()*3;
        data_.span += 3;
      }

      if (data_.types & static_cast<unsigned int>(Parts::TEXTURE)) {
        assert(data_.texturePoints.size() == data_.points.size());
        data_.numData += data_.texturePoints.size()*2;
        data_.span += 2;
      }

      data_.data = new float [data_.numData];

      int  i  = 0;
      auto np = data_.points.size();

      for (size_t ip = 0; ip < np; ++ip) {
        if (data_.types & static_cast<unsigned int>(Parts::POINT)) {
          const auto &p = data_.points[ip];

          data_.data[i++] = p.x;
          data_.data[i++] = p.y;
          data_.data[i++] = p.z;
        }

        if (data_.types & static_cast<unsigned int>(Parts::NORMAL)) {
          const auto &p = data_.normals[ip];

          data_.data[i++] = p.x;
          data_.data[i++] = p.y;
          data_.data[i++] = p.z;
        }

        if (data_.types & static_cast<unsigned int>(Parts::COLOR)) {
          const auto &c = data_.colors[ip];

          data_.data[i++] = c.r;
          data_.data[i++] = c.g;
          data_.data[i++] = c.b;
        }

        if (data_.types & static_cast<unsigned int>(Parts::TEXTURE)) {
          const auto &p = data_.texturePoints[ip];

          data_.data[i++] = p.x;
          data_.data[i++] = p.y;
        }
      }

      //---

      data_.numIndData = uint(data_.indices.size());

      data_.indData = new int [data_.numIndData];

      i = 0;

      for (const auto &ind : data_.indices)
        data_.indData[i++] = ind;

      //---

      data_.dataValid = true;
    }
  }

 private:
  struct Data {
    QOpenGLShaderProgram *program { nullptr };

    QOpenGLVertexArrayObject* vObj         { nullptr };
    QOpenGLBuffer*            vertexBuffer { nullptr };
    QOpenGLBuffer*            indBuffer    { nullptr };

    unsigned int  types      { 0 };
    float*        data       { nullptr };
    unsigned int  numData    { 0 };
    int*          indData    { nullptr };
    unsigned int  numIndData { 0 };
    unsigned int  span       { 0 };
    bool          dataValid  { false };
    Points        points;
    Points        normals;
    Colors        colors;
    TexturePoints texturePoints;
    Indices       indices;
    bool          indicesSet { false };
  };

  Data data_;
};

#endif
