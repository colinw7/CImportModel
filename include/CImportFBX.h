#ifndef CIMPORT_FBX_H
#define CIMPORT_FBX_H

#include <CImportBase.h>
#include <CGeomObject3D.h>
#include <CFile.h>
#include <CDeflate.h>

#include <optional>

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

template<> inline DataType dataType<std::vector<uchar >>() { return DataType::BYTE_ARRAY  ; }
template<> inline DataType dataType<std::vector<short >>() { return DataType::SHORT_ARRAY ; }
template<> inline DataType dataType<std::vector<int   >>() { return DataType::INT_ARRAY   ; }
template<> inline DataType dataType<std::vector<long  >>() { return DataType::LONG_ARRAY  ; }
template<> inline DataType dataType<std::vector<float >>() { return DataType::FLOAT_ARRAY ; }
template<> inline DataType dataType<std::vector<double>>() { return DataType::DOUBLE_ARRAY; }

template<typename T> inline const char *dataTypeString() { return ""; }

template<> inline const char *dataTypeString<uchar      >() { return "BYTE"  ; }
template<> inline const char *dataTypeString<short      >() { return "SHORT" ; }
template<> inline const char *dataTypeString<int        >() { return "INT"   ; }
template<> inline const char *dataTypeString<long       >() { return "LONG"  ; }
template<> inline const char *dataTypeString<float      >() { return "FLOAT" ; }
template<> inline const char *dataTypeString<double     >() { return "DOUBLE"; }
template<> inline const char *dataTypeString<std::string>() { return "STRING"; }

template<> inline const char *dataTypeString<std::vector<uchar >>() { return "BYTE_ARRAY"  ; }
template<> inline const char *dataTypeString<std::vector<short >>() { return "SHORT_ARRAY" ; }
template<> inline const char *dataTypeString<std::vector<int   >>() { return "INT_ARRAY"   ; }
template<> inline const char *dataTypeString<std::vector<long  >>() { return "LONG_ARRAY"  ; }
template<> inline const char *dataTypeString<std::vector<float >>() { return "FLOAT_ARRAY" ; }
template<> inline const char *dataTypeString<std::vector<double>>() { return "DOUBLE_ARRAY"; }

}

class CImportFBX : public CImportBase {
 public:
  using OptReal   = std::optional<double>;
  using OptPoint3 = std::optional<CPoint3D>;
  using OptColor  = std::optional<CRGBA>;

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
    static void printDataType(std::ostream &os, const T &t) {
      os << t << " " << CImportFBXUtil::dataTypeString<T>();
    }

    template<typename T>
    static void printData(std::ostream &os, const T &t) {
      os << t;
    }

    template<typename T>
    static void printArray(std::ostream &os, const std::vector<T> &a) {
      os << CImportFBXUtil::dataTypeString<T>() << "_ARRAY ";
      os << "[" << a.size() << "] ";
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
        printDataType(os, *reinterpret_cast<uchar *>(data.data_));
      else if (data.type() == DataType::SHORT)
        printDataType(os, *reinterpret_cast<short *>(data.data_));
      else if (data.type() == DataType::INT)
        printDataType(os, *reinterpret_cast<int *>(data.data_));
      else if (data.type() == DataType::LONG)
        printDataType(os, *reinterpret_cast<long *>(data.data_));
      else if (data.type() == DataType::FLOAT)
        printDataType(os, *reinterpret_cast<float *>(data.data_));
      else if (data.type() == DataType::DOUBLE)
        printDataType(os, *reinterpret_cast<double *>(data.data_));
      else if (data.type() == DataType::STRING)
        printDataType(os, *reinterpret_cast<std::string *>(data.data_));

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

    std::string toString() const {
      if      (type() == DataType::BYTE)
        return std::to_string(*reinterpret_cast<uchar *>(data_));
      else if (type() == DataType::SHORT)
        return std::to_string(*reinterpret_cast<short *>(data_));
      else if (type() == DataType::INT)
        return std::to_string(*reinterpret_cast<int *>(data_));
      else if (type() == DataType::LONG)
        return std::to_string(*reinterpret_cast<long *>(data_));
      else if (type() == DataType::FLOAT)
        return std::to_string(*reinterpret_cast<float *>(data_));
      else if (type() == DataType::DOUBLE)
        return std::to_string(*reinterpret_cast<double *>(data_));
      else if (type() == DataType::STRING)
        return *reinterpret_cast<std::string *>(data_);
      else if (type() == DataType::BYTE_ARRAY) {
        std::ostringstream os;
        printArray(os, *reinterpret_cast<std::vector<uchar> *>(data_));
        return os.str();
      }
      else if (type() == DataType::SHORT_ARRAY) {
        std::ostringstream os;
        printArray(os, *reinterpret_cast<std::vector<short> *>(data_));
        return os.str();
      }
      else if (type() == DataType::INT_ARRAY) {
        std::ostringstream os;
        printArray(os, *reinterpret_cast<std::vector<int> *>(data_));
        return os.str();
      }
      else if (type() == DataType::LONG_ARRAY) {
        std::ostringstream os;
        printArray(os, *reinterpret_cast<std::vector<long> *>(data_));
        return os.str();
      }
      else if (type() == DataType::FLOAT_ARRAY) {
        std::ostringstream os;
        printArray(os, *reinterpret_cast<std::vector<float> *>(data_));
        return os.str();
      }
      else if (type() == DataType::DOUBLE_ARRAY) {
        std::ostringstream os;
        printArray(os, *reinterpret_cast<std::vector<double> *>(data_));
        return os.str();
      }
      else if (type() == DataType::NONE)
        return "";
      else
        assert(false);

      return "";
    }

    long toLong() const {
     if      (type() == DataType::BYTE)
        return *reinterpret_cast<uchar *>(data_);
      else if (type() == DataType::SHORT)
        return *reinterpret_cast<short *>(data_);
      else if (type() == DataType::INT)
        return *reinterpret_cast<int *>(data_);
      else if (type() == DataType::LONG)
        return *reinterpret_cast<long *>(data_);
      else
        assert(false);

      return 0;
    }

    double toReal() const {
     if      (type() == DataType::BYTE)
        return *reinterpret_cast<uchar *>(data_);
      else if (type() == DataType::SHORT)
        return *reinterpret_cast<short *>(data_);
      else if (type() == DataType::INT)
        return *reinterpret_cast<int *>(data_);
      else if (type() == DataType::LONG)
        return static_cast<double>(*reinterpret_cast<long *>(data_));
      else if (type() == DataType::FLOAT)
        return *reinterpret_cast<float *>(data_);
      else if (type() == DataType::DOUBLE)
        return *reinterpret_cast<double *>(data_);
      else
        assert(false);

      return 0;
    }

    std::vector<int> toIntArray() const {
      if (type() == DataType::INT_ARRAY)
        return *reinterpret_cast<std::vector<int> *>(data_);
      else {
        std::vector<int> ia;
        ia.push_back(int(toLong()));
        return ia;
      }
    }

    std::vector<long> toLongArray() const {
      if (type() == DataType::LONG_ARRAY)
        return *reinterpret_cast<std::vector<long> *>(data_);
      else
        assert(false);

      return std::vector<long>();
    }

    std::vector<float> toFloatArray() const {
      if (type() == DataType::FLOAT_ARRAY)
        return *reinterpret_cast<std::vector<float> *>(data_);
      else
        assert(false);

      return std::vector<float>();
    }

    std::vector<double> toDoubleArray() const {
      if (type() == DataType::DOUBLE_ARRAY)
        return *reinterpret_cast<std::vector<double> *>(data_);
      else {
        std::vector<double> da;
        da.push_back(toReal());
        return da;
      }
    }

   private:
    DataType type_ { DataType::NONE };
    void*    data_ { nullptr };
  };

  using PropDataArray    = std::map<uint, PropData *>;
  using PropIndDataArray = std::map<uint, PropDataArray>;
  using PropDataMap      = std::map<std::string, PropIndDataArray>;

  struct PropDataTree {
    using Children  = std::vector<PropDataTree *>;
    using NameCount = std::map<std::string, uint>;

    std::string   name;
    PropDataTree* parent { nullptr };
    PropDataMap   dataMap;
    NameCount     nameCount;
    PropDataArray parentDataMap;
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

  struct IndName {
    long        ind  { -1 };
    std::string name;

    IndName() { }

    IndName(long i) : ind(i) { }
    IndName(const std::string &n) : name(n) { }

    friend bool operator<(const IndName &lhs, const IndName &rhs) {
      if (lhs.ind  >= 0  && rhs.ind  >= 0 ) return (lhs.ind  < rhs.ind );
      if (lhs.name != "" && rhs.name != "") return (lhs.name < rhs.name);
      return (lhs.ind >= 0);
    }

    friend bool operator==(const IndName &lhs, const IndName &rhs) {
      return (lhs.ind == rhs.ind && lhs.name == rhs.name);
    }

    friend bool operator!=(const IndName &lhs, const IndName &rhs) {
      return ! operator==(lhs, rhs);
    }

    friend std::ostream &operator<<(std::ostream &os, const IndName &i) {
      if      (i.ind  >= 0 ) os << i.ind;
      else if (i.name != "") os << i.name;
      return os;
    }

    bool isEmpty() const { return (ind < 0 && name == ""); }

    std::string to_string() const {
      if (ind  >= 0 ) return std::to_string(ind);
      else if (name != "") return name;
      return "";
    }

    long to_long() const {
      return ind;
    }
  };

  template<typename T>
  struct IndNameMap : public std::map<IndName, T> {
    int lastInd { 0 };

    bool isEmpty() const { return (*this).empty(); }

    void add(T &d, const IndName &indName) {
      d.indName = indName;

      if (indName.name == "")
        d.indName.ind = lastInd++;

      (*this)[d.indName] = d;
    }
  };

  struct FileData {
    std::vector<uchar> fileMem;
    const uchar*       fileBytes { nullptr };
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
      if (! readBytes(4))
        return false;

      *i = ((buffer[0] & 0xFF)      ) |
           ((buffer[1] & 0xFF) <<  8) |
           ((buffer[2] & 0xFF) << 16) |
           ((buffer[3] & 0xFF) << 24);

      return true;
    }

    bool readULong(ulong *i) {
      if (! readBytes(8))
        return false;

      *i = (ulong(buffer[0] & 0xFF)      ) |
           (ulong(buffer[1] & 0xFF) <<  8) |
           (ulong(buffer[2] & 0xFF) << 16) |
           (ulong(buffer[3] & 0xFF) << 24) |
           (ulong(buffer[4] & 0xFF) << 32) |
           (ulong(buffer[5] & 0xFF) << 40) |
           (ulong(buffer[6] & 0xFF) << 48) |
           (ulong(buffer[7] & 0xFF) << 56);

      return true;
    }

    bool readInt(int *i) {
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
        fileData1.fileMem.resize(fileData1.fileSize);

        if (! CDeflate::deflate_bytes(buffer, compressedLen,
                                      fileData1.fileMem, uint(fileData1.fileSize)))
          return errorMsg("Failed to uncompress data");

        fileData1.fileBytes = &fileData1.fileMem[0];

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

  void addFileMap(const std::string &oldName, const std::string &newName);

 private:
  struct ModelData;
  struct TextureData;
  struct MaterialData;
  struct AnimationDeformerData;

  using TextureMap = std::map<std::string, TextureData *>;

  // Geometry Mesh Data
  struct VertexData {
    bool                set { false };
    std::vector<int>    indices;
    std::vector<double> values;
  };

  struct NormalData {
    bool                set { false };
    std::vector<int>    indices;
    std::vector<double> values;
  };

  struct ColorData {
    bool                set { false };
    std::vector<int>    indices;
    std::vector<double> values;
  };

  struct UVData {
    bool                set { false };
    std::vector<int>    indices;
    std::vector<double> values;
  };

  struct GeometryData {
    std::string             name;
    std::string             type;
    std::vector<VertexData> vertexData;
    std::vector<NormalData> normalData;
    std::vector<ColorData>  colorData;
    std::vector<UVData>     uvData;
    std::vector<int>        edges;
    std::vector<int>        materials;
    std::vector<int>        textures;
    OptPoint3               translation;
    OptPoint3               rotation;
    OptPoint3               scale;

    ModelData*             modelData    { nullptr };
    AnimationDeformerData* deformerData { nullptr };

    CGeomObject3D* object { nullptr };
  };

  // Geometry Instance
  struct ModelData {
    IndName id;

    std::string name;
    std::string type;
    OptPoint3   localTranslation;
    OptPoint3   localRotation;
    OptPoint3   localScaling;

    TextureMap                          textureMap;
    MaterialData*                       materialData { nullptr };
    std::vector<AnimationDeformerData*> deformerData;

    ModelData*               parent { nullptr };
    std::vector<ModelData *> children;

    GeometryData*  geometryData { nullptr };

    std::vector<GeometryData *> geometryDataList;
  };

  struct MaterialData;
  struct VideoData;

  struct TextureData {
    IndName id;

    std::string   name;
    std::string   type;
    std::string   fileName;
    std::string   media;
    CGeomTexture* texture { nullptr };

    ModelData*    modelData    { nullptr };
    MaterialData* materialData { nullptr };
    VideoData*    videoData    { nullptr };
  };

  struct MaterialData {
    IndName id;

    std::string name;
    OptColor    ambientColor;
    OptReal     ambientFactor;
    OptColor    diffuseColor;
    OptReal     diffuseFactor;
    OptColor    emissionColor;
    OptReal     emissionFactor;
    OptColor    specularColor;
    OptReal     specularFactor;
    OptReal     shininess;

    TextureMap               textureMap;
    std::vector<ModelData *> modelData;
  };

  //---

  bool readBinary(bool &isBin);
  bool readAscii();

  bool readTextBlock(CStrParse &parse, TextBlock *block);

  void printTextBlock(TextBlock *block);
  void printNameValues(TextBlock *block);

  bool processTextBlock(TextBlock *block);
  bool processObjectsTextBlock(TextBlock *block);

  std::vector<std::string> readValues(const std::string &value) const;

  bool readFileData(FileData &fileData);

  bool readScope(FileData &fileData, const std::string &scopeName,
                 PropDataTree *propDataTree);

  bool readScopeData(const std::string &name, uint ni, uint ind, FileData &fileData,
                     const std::string &scopeName, PropDataTree *propDataTree);

  void dumpTree(PropDataTree *tree, int depth=0);
  void dumpDataMap(PropDataTree *tree, int depth=0);

  void processDataTree(PropDataTree *tree);
  void processGeometryDataTree(PropDataTree *tree, GeometryData *geometryData);

  void addGeometryObject(GeometryData *geometryData);

  CMatrix3D calcModelDataLocalTransform(ModelData *m) const;
  CMatrix3D calcModelDataHierTransform(ModelData *m) const;

  std::string calcModelDataHierName(ModelData *m) const;

  std::string hierName(PropDataTree *tree) const;
  std::string hierName(TextBlock *block) const;

  bool loadTexture(TextureData *textureData, std::string &fileName);

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

  using ConnectionMap = std::map<std::string, std::string>;

  ConnectionMap connectionMap_;

  struct GlobalData {
    long   upAxis          { 0L };
    long   upAxisSign      { 0L };
    long   frontAxis       { 0L };
    long   frontAxisSign   { 0L };
    long   coordAxis       { 0L };
    long   coordAxisSign   { 0L };
    double unitScaleFactor { 1.0 };
  };

  // text format data
  using TextureDataMap  = std::map<std::string, TextureData *>;
  using MaterialDataMap = std::map<std::string, MaterialData *>;

  TextureDataMap  textureDataMap_;
  MaterialDataMap materialDataMap_;

  // binary format data
  GlobalData globalData_;

  using IdType         = IndNameMap<std::string>;
  using IdGeometryData = IndNameMap<GeometryData *>;
  using IdModelData    = IndNameMap<ModelData *>;
  using IdTextureData  = IndNameMap<TextureData *>;
  using IdMaterialData = IndNameMap<MaterialData *>;

  IdType         idType_;
  IdGeometryData idGeometryData_;
  IdModelData    idModelData_;
  IdTextureData  idTextureData_;
  IdMaterialData idMaterialData_;

  struct AnimationLayerData;
  struct AnimationCurveNodeData;
  struct AnimationStackData;

  struct AnimationCurveData {
    std::string        name;
    double             def { 0.0 };
    std::vector<long>  keyTime;
    std::vector<float> keyValueFloat;

    AnimationCurveNodeData* animationCurveNode { nullptr };
  };

  struct AnimationCurveNodeData {
    std::string type;
    CPoint3D    p;

    ModelData*          modelData          { nullptr };
    AnimationLayerData* animationLayerData { nullptr };
  };

  struct AnimationDeformerData {
    std::string         name;
    std::string         type;
    std::vector<int>    indexes;
    std::vector<double> weights;

    AnimationDeformerData*               parent { nullptr };
    std::vector<AnimationDeformerData *> children;

    GeometryData* geometryData { nullptr };
    ModelData*    modelData    { nullptr };
  };

  struct AnimationLayerData {
    std::string           name;
    std::vector<CPoint3D> points;

    AnimationStackData*                   animationStack { nullptr };
    std::vector<AnimationCurveNodeData *> animationCurveNodes;
  };

  struct AnimationStackData {
    std::string name;

    std::vector<AnimationLayerData *> animationLayers;
  };

  struct NodeAttributeData {
    std::string name;

    ModelData* modelData { nullptr };
  };

  struct VideoData {
    IndName id;

    ModelData* modelData   { nullptr };
    TextureMap textureMap;
  };

  struct ConstraintData {
    std::string name;
  };

  using IdAnimationCurveData     = IndNameMap<AnimationCurveData *>;
  using IdAnimationCurveNodeData = IndNameMap<AnimationCurveNodeData *>;
  using IdAnimationDeformerData  = IndNameMap<AnimationDeformerData *>;
  using IdAnimationLayerData     = IndNameMap<AnimationLayerData *>;
  using IdAnimationStackData     = IndNameMap<AnimationStackData *>;
  using IdNodeAttributeData      = IndNameMap<NodeAttributeData *>;
  using IdVideoData              = IndNameMap<VideoData *>;
  using IdConstraintData         = IndNameMap<ConstraintData *>;

  IdAnimationCurveData     animationCurveData_;
  IdAnimationCurveNodeData animationCurveNodeData_;
  IdAnimationDeformerData  animationDeformerData_;
  IdAnimationLayerData     animationLayerData_;
  IdAnimationStackData     animationStackData_;
  IdNodeAttributeData      idNodeAttributeData_;
  IdVideoData              idVideoData_;
  IdConstraintData         constraintData_;

  bool colorAlpha_ { true };
};

#endif
