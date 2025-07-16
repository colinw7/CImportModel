#ifndef CIMPORT_FBX_H
#define CIMPORT_FBX_H

#include <CImportBase.h>
#include <CGeomObject3D.h>
#include <CFile.h>
#include <CDeflate.h>

class CStrParse;

enum class DataType {
  NONE   = 0,
  BYTE   = 1,
  SHORT  = 2,
  INT    = 3,
  LONG   = 4,
  FLOAT  = 5,
  DOUBLE = 6,
  STRING = 7,

  ARRAY = (1<<8),

  BYTE_ARRAY   = (ARRAY | BYTE),
  SHORT_ARRAY  = (ARRAY | SHORT),
  INT_ARRAY    = (ARRAY | INT),
  LONG_ARRAY   = (ARRAY | LONG),
  FLOAT_ARRAY  = (ARRAY | FLOAT),
  DOUBLE_ARRAY = (ARRAY | DOUBLE),

  INVALID = -1
};

namespace CImportFBXUtil {

template<typename T> inline DataType dataType() { return DataType::INVALID; }

template<> inline DataType dataType<uchar      >() { return DataType::BYTE  ; }
template<> inline DataType dataType<short      >() { return DataType::SHORT ; }
template<> inline DataType dataType<int        >() { return DataType::INT   ; }
template<> inline DataType dataType<long       >() { return DataType::LONG  ; }
template<> inline DataType dataType<float      >() { return DataType::FLOAT ; }
template<> inline DataType dataType<double     >() { return DataType::DOUBLE; }
template<> inline DataType dataType<std::string>() { return DataType::STRING; }

template<> inline DataType dataType<std::vector<uchar >>() { return DataType::BYTE_ARRAY; }
template<> inline DataType dataType<std::vector<short >>() { return DataType::SHORT_ARRAY; }
template<> inline DataType dataType<std::vector<int   >>() { return DataType::INT_ARRAY; }
template<> inline DataType dataType<std::vector<long  >>() { return DataType::LONG_ARRAY; }
template<> inline DataType dataType<std::vector<float >>() { return DataType::FLOAT_ARRAY; }
template<> inline DataType dataType<std::vector<double>>() { return DataType::DOUBLE_ARRAY; }

}

class CImportFBX : public CImportBase {
 public:
  template<typename T>
  static void *createData(const T &t) {
    auto *t1 = new T(t);
    return t1;
  }

  template<typename T>
  static void deleteData(void *data) {
    auto *t = reinterpret_cast<T *>(data);
    delete t;
  }

  class PropData {
   public:
    PropData() { }

   ~PropData() {
      freeData();
    }

    PropData(const PropData &) = delete;
    const PropData &operator=(const PropData &) = delete;

    const DataType &type() const { return type_; }

    template<typename T>
    void setData(const T &t) {
      freeData();

      type_ = CImportFBXUtil::dataType<T>();
      data_ = createData(t);

      assert(type_ != DataType::INVALID);
    }

    void freeData() {
      if      (type() == DataType::BYTE  ) deleteData<uchar      >(data_);
      else if (type() == DataType::SHORT ) deleteData<short      >(data_);
      else if (type() == DataType::INT   ) deleteData<int        >(data_);
      else if (type() == DataType::LONG  ) deleteData<long       >(data_);
      else if (type() == DataType::FLOAT ) deleteData<float      >(data_);
      else if (type() == DataType::DOUBLE) deleteData<double     >(data_);
      else if (type() == DataType::STRING) deleteData<std::string>(data_);

      else if (type() == DataType::BYTE_ARRAY  ) deleteData<std::vector<uchar >>(data_);
      else if (type() == DataType::SHORT_ARRAY ) deleteData<std::vector<short >>(data_);
      else if (type() == DataType::INT_ARRAY   ) deleteData<std::vector<int   >>(data_);
      else if (type() == DataType::LONG_ARRAY  ) deleteData<std::vector<long  >>(data_);
      else if (type() == DataType::FLOAT_ARRAY ) deleteData<std::vector<float >>(data_);
      else if (type() == DataType::DOUBLE_ARRAY) deleteData<std::vector<double>>(data_);

      else if (type() != DataType::NONE) assert(false);

      type_ = DataType::NONE;
      data_ = nullptr;
    }

    template<typename T>
    static void printData(std::ostream &os, const T &t) {
      os << t;
    }

    template<typename T>
    static void printArray(std::ostream &os, const std::vector<T> &a) {
      bool first = true;
      for (const auto &t : a) {
        if (! first) os << ", ";
        printData(os, t);
        first = false;
      }
    }

    template<typename T>
    const T &getData() const {
      return *reinterpret_cast<T *>(data_);
    }

    template<typename T>
    const std::vector<T> &getArrayData() const {
      return *reinterpret_cast<std::vector<T> *>(data_);
    }

    friend std::ostream &operator<<(std::ostream &os, const PropData &data) {
      if      (data.type() == DataType::BYTE)
        printData(os, *reinterpret_cast<uchar *>(data.data_));
      else if (data.type() == DataType::SHORT)
        printData(os, *reinterpret_cast<short *>(data.data_));
      else if (data.type() == DataType::INT)
        printData(os, *reinterpret_cast<int *>(data.data_));
      else if (data.type() == DataType::LONG)
        printData(os, *reinterpret_cast<long *>(data.data_));
      else if (data.type() == DataType::FLOAT)
        printData(os, *reinterpret_cast<float *>(data.data_));
      else if (data.type() == DataType::DOUBLE)
        printData(os, *reinterpret_cast<double *>(data.data_));
      else if (data.type() == DataType::STRING)
        printData(os, *reinterpret_cast<std::string *>(data.data_));

      else if (data.type() == DataType::BYTE_ARRAY)
        printArray(os, *reinterpret_cast<std::vector<uchar> *>(data.data_));
      else if (data.type() == DataType::SHORT_ARRAY)
        printArray(os, *reinterpret_cast<std::vector<short> *>(data.data_));
      else if (data.type() == DataType::INT_ARRAY)
        printArray(os, *reinterpret_cast<std::vector<int> *>(data.data_));
      else if (data.type() == DataType::LONG_ARRAY)
        printArray(os, *reinterpret_cast<std::vector<long> *>(data.data_));
      else if (data.type() == DataType::FLOAT_ARRAY)
        printArray(os, *reinterpret_cast<std::vector<float> *>(data.data_));
      else if (data.type() == DataType::DOUBLE_ARRAY)
        printArray(os, *reinterpret_cast<std::vector<double> *>(data.data_));

      else if (data.type() != DataType::NONE)
        assert(false);

      return os;
    }

   private:
    DataType type_ { DataType::NONE };
    void*    data_ { nullptr };
  };

  using PropDataArray = std::map<uint, PropData *>;
  using PropDataMap   = std::map<std::string, PropDataArray>;

  struct PropDataTree {
    using Children = std::vector<PropDataTree *>;

    std::string   name;
    PropDataTree *parent { nullptr };
    PropDataMap   dataMap;
    Children      children;
  };

  struct TextBlock;

  struct NameValue {
    std::string name;
    std::string value;
    TextBlock*  block { nullptr };
  };

  struct TextBlock {
    TextBlock*              parent { nullptr };
    int                     indent { 0 };
    NameValue               nameValue;
    std::vector<NameValue>  nameValues;
    std::vector<TextBlock*> children;
  };

  struct FileData {
    std::vector<uchar> fileBytes;
    ulong              filePos { 0 };
    ulong              fileSize { 0 };

    std::vector<uchar> buffer;

    int depth { 0 };

    bool debug { false };

    //---

    bool readBytes(uint len) {
      if (filePos + len > fileSize) {
        filePos = fileSize;
        return false;
      }

      buffer.resize(len + 1);

      for (uint i = 0; i < len; ++i)
        buffer[i] = fileBytes[filePos++];

      return true;
    }

    template<typename T>
    bool readData(T *t) {
      if (! readBytes(sizeof(T)))
        return false;

      *t = *reinterpret_cast<T *>(&buffer[0]);

      return true;
    }

    bool readUChar(uchar *c) {
      if (! readBytes(1))
        return false;

      *c = buffer[0];

      return true;
    }

    bool readUShort(ushort *i) {
      if (! readBytes(2))
        return false;

      *i = ((buffer[0] & 0xFF)      ) |
           ((buffer[1] & 0xFF) <<  8);

      return true;
    }

    bool readShort(short *i) {
      return readData(i);
    }

    bool readUInt(uint *i) {
      if (! readBytes(4))
        return false;

      *i = ((buffer[0] & 0xFF)      ) |
           ((buffer[1] & 0xFF) <<  8) |
           ((buffer[2] & 0xFF) << 16) |
           ((buffer[3] & 0xFF) << 24);

      return true;
    }

    bool readUInt(ulong *i) {
      if (! readBytes(8))
        return false;

      *i = (ulong(buffer[0] & 0xFF)      ) |
           (ulong(buffer[1] & 0xFF) <<  8) |
           (ulong(buffer[2] & 0xFF) << 16) |
           (ulong(buffer[3] & 0xFF) << 24) |
           (ulong(buffer[0] & 0xFF) << 32) |
           (ulong(buffer[1] & 0xFF) << 40) |
           (ulong(buffer[2] & 0xFF) << 48) |
           (ulong(buffer[3] & 0xFF) << 56);

      return true;
    }

    bool readInt(int *i) {
      return readData(i);
    }

    bool readULong(ulong *i) {
      return readData(i);
    }

    bool readLong(long *i) {
      return readData(i);
    }

    bool readFloat(float *f) {
      return readData(f);
    }

    bool readDouble(double *r) {
      return readData(r);
    }

    bool readStr(uint len, std::string &str) {
      if (! readBytes(len))
        return false;

      buffer[len] = '\0';

      str = std::string(reinterpret_cast<char *>(&buffer[0]));

      return true;
    };

    template<typename T>
    bool readArrayData(std::vector<T> &data, char tc, int d) {
      depth = d;

      uint len;
      if (! readUInt(&len))
        return errorMsg("Failed to read len for " + ucharStr(tc));

      uint encoding;
      if (! readUInt(&encoding))
        return errorMsg("Failed to read encoding for " + ucharStr(tc));

      uint compressedLen;
      if (! readUInt(&compressedLen))
        return errorMsg("Failed to read compress for " + ucharStr(tc));

      infoMsg(" " + std::string(&tc, 1) + " " + std::to_string(encoding) +
              " " + std::to_string(compressedLen));

      if (encoding == 0) {
        std::string vstr;
        for (uint i = 0; i < len; ++i) {
          T t;
          if (! readData(&t))
            return errorMsg("Failed to read data for " + ucharStr(tc));
          if (i > 0) vstr += " ";
          vstr += std::to_string(t);
        }
        infoMsg("  [" + std::to_string(len) + "] " + vstr);
      }
      else {
        if (! readBytes(compressedLen))
          return errorMsg("Failed to read data for " + ucharStr(tc));

        FileData fileData1;

        fileData1.fileSize = len*sizeof(T);
        fileData1.fileBytes.resize(fileData1.fileSize);

        if (! CDeflate::deflate_bytes(buffer, compressedLen,
                                      fileData1.fileBytes, uint(fileData1.fileSize)))
          return errorMsg("Failed to uncompress data");

        std::string vstr;
        for (uint i = 0; i < len; ++i) {
          T t;
          if (! fileData1.readData(&t))
            return errorMsg("Failed to read data for " + ucharStr(tc));
          data.push_back(t);
          if (i > 0) vstr += " ";
          vstr += std::to_string(t);
        }
        infoMsg("  [" + std::to_string(len) + "] " + vstr);
      }
      return true;
    }

    void infoMsg(const std::string &msg) const {
      if (! debug) return;

      for (int i = 0; i < depth; ++i)
        std::cerr << " ";

      std::cerr << msg << "\n";
    }

    bool errorMsg(const std::string &msg) const {
      for (int i = 0; i < depth; ++i)
        std::cerr << " ";

      std::cerr << "Error: " << msg << "\n";

      return false;
    }

    std::string ucharStr(uchar c) const {
      std::string s;
      if (isprint(char(c))) s += char(c);
      s += "(" + std::to_string(int(c)) + ")";
      return s;
    }
  };

 public:
  CImportFBX(CGeomScene3D *scene=nullptr, const std::string &name="fbx");

 ~CImportFBX();

  bool read(CFile &file) override;

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

  bool isDebug() const { return debug_; }
  void setDebug(bool b) { debug_ = b; }

  bool isDump() const { return dump_; }
  void setDump(bool b) { dump_ = b; }

  bool isHierName() const { return hierName_; }
  void setHierName(bool b) { hierName_ = b; }

 private:
  struct GeometryData {
    CGeomObject3D*      object { nullptr };
    std::vector<int>    polygonVertexIndex;
    std::vector<double> vertices;
    std::vector<int>    edges;
    std::vector<int>    normalsIndex;
    std::vector<double> normals;
    std::vector<int>    colorIndex;
    std::vector<double> colors;
    std::vector<int>    uvIndex;
    std::vector<double> uv;
    std::vector<int>    materials;
  };

  bool readBinary();
  bool readAscii();

  bool readTextBlock(CStrParse &parse, TextBlock *block);
  void printTextBlock(TextBlock *block);
  void processTextBlock(TextBlock *block);

  bool readFileData(FileData &fileData);

  bool readScope(FileData &fileData, const std::string &scopeName,
                 PropDataTree *propDataTree);

  bool readScopeData(const std::string &name, uint ind, FileData &fileData,
                     PropDataTree *propDataTree);

  void dumpTree(PropDataTree *tree, int depth=0);

  void addGeometry(PropDataTree *tree);
  void addSubGeometry(PropDataTree *tree, GeometryData &geometryData);
  void addGeometryObject(GeometryData &geometryData);

  std::string hierName(PropDataTree *tree) const;

  std::string getObjectName();

  void infoMsg(const std::string &msg) const;
  bool errorMsg(const std::string &msg) const;

 private:
  CGeomScene3D*  scene_   { nullptr };
  SceneP         pscene_;
  CGeomObject3D* object_  { nullptr };
  ObjectP        pobject_;
  CFile*         file_    { nullptr };
  bool           longInt_ { false };
  int            depth_   { 0 };

  PropDataTree *propDataTree_ { nullptr };

  uint objectInd_ { 0 };

  bool debug_    { false };
  bool dump_     { false };
  bool hierName_ { false };
};

#endif
