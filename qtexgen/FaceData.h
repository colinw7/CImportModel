#ifndef FaceData_H
#define FaceData_H

#include <vector>

struct FaceData {
  int pos { 0 };
  int len { 0 };
};

struct FaceDataList {
  void clear() { faceDatas.clear(); pos = 0; }

  void add(const FaceDataList &faceDataList) {
    for (const auto &faceData : faceDataList.faceDatas)
      faceDatas.push_back(faceData);
  }

  int                   pos { 0 };
  std::vector<FaceData> faceDatas;
};

#endif
