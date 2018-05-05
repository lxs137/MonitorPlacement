//
// Created by liao xiangsen on 2018/5/4.
//

#include "camera_chooser.h"

#include <algorithm>
#include <iomanip>

//#define CHOOSER_DEBUG

namespace monitor {
  void reEvalViewMatrix(bool **viewMatrix, int choosedCameraIndex, size_t targetCount,
                        std::vector<int> &cameraViewedCount) {
    for(size_t i = 0; i < targetCount; i++) {
      if(viewMatrix[i][choosedCameraIndex]) {
        for(size_t j = 0, n = cameraViewedCount.size(); j < n; j++) {
          if(viewMatrix[i][j]) {
            cameraViewedCount[j]--;
          }
        }
        viewMatrix[i][choosedCameraIndex] = false;
      }
    }
    cameraViewedCount[choosedCameraIndex] = 0;
  }

  void preChooseGreedy(const std::vector<monitor::Camera> &src, std::vector<monitor::Camera> &res, bool **sourceView,
                                size_t targetCount, size_t resCount) {
    size_t cameraCount = src.size();
#ifdef CHOOSER_DEBUG
    std::cout << "Camera Count: " << cameraCount << ", Target Count:" << targetCount << std::endl;
#endif
    bool **view = new bool*[targetCount]();
    view[0] = new bool[cameraCount * targetCount];
    memcpy(view[0], sourceView[0], cameraCount * targetCount);
    for(size_t i = 1, linePtrStep = sizeof(bool) * cameraCount; i < targetCount; i++) {
      view[i] = view[i - 1] + linePtrStep;
    }

    std::vector<int> cameraViewedCount;
    cameraViewedCount.assign(cameraCount, 0);
    for(size_t i = 0; i < targetCount; i++) {
      for(size_t j = 0; j < cameraCount; j++)
        cameraViewedCount[j] += view[i][j] ? 1 : 0;
    }

    std::cout.setf(std::ios::left);
    do {
#ifdef CHOOSER_DEBUG
      std::cout << "resCount: " << resCount << std::endl;
      for(auto it = cameraViewedCount.begin(); it != cameraViewedCount.end(); it++) {
        std::cout << std::setw(3) << *it << " ";
      }
      std::cout << "\n\n";
#endif
      auto result = std::max_element(cameraViewedCount.begin(), cameraViewedCount.end());
      int mostViewedIndex = (int)std::distance(cameraViewedCount.begin(), result);
      if(cameraViewedCount[mostViewedIndex] <= 1)
        break;
      res.push_back(src[mostViewedIndex]);
      resCount--;
      reEvalViewMatrix(view, mostViewedIndex, targetCount, cameraViewedCount);
    } while(resCount > 0);
    std::cout.setf(std::ios::right);

    delete[] view[0];
    delete[] view;
  }
}