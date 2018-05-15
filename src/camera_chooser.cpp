//
// Created by liao xiangsen on 2018/5/4.
//

#include "camera_chooser.h"

#include <algorithm>
#include <iomanip>

#define PRE_CHOOSE_DEBUG
#define LOCAL_SEARCH_DEBUG

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

  void preChooseGreedy(const std::vector<monitor::Camera> &src, std::vector<int> &resIndex, bool **sourceView,
                                size_t targetCount, size_t resCount) {
    size_t cameraCount = src.size();
#ifdef PRE_CHOOSE_DEBUG
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
#ifdef PRE_CHOOSE_DEBUG
    std::cout.setf(std::ios::left);
#endif
    do {
#ifdef PRE_CHOOSE_DEBUG
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
      resIndex.push_back(mostViewedIndex);
      resCount--;
      reEvalViewMatrix(view, mostViewedIndex, targetCount, cameraViewedCount);
    } while(resCount > 0);
#ifdef PRE_CHOOSE_DEBUG
    std::cout.setf(std::ios::right);
#endif

    delete[] view[0];
    delete[] view;
  }

  void localSearch(const std::vector<Camera> &src, const std::vector<Voxel> &targets, bool **sourceView, std::vector<Camera> &res) {
    const size_t cameraCount = src.size();
    const size_t targetCount = targets.size();
    const size_t viewMatrixSize = cameraCount * targetCount;
    bool **view = new bool*[targetCount]();
    view[0] = new bool[viewMatrixSize]();
    memcpy(view[0], sourceView[0], viewMatrixSize);
    for(size_t i = 1, linePtrStep = sizeof(bool) * cameraCount; i < targetCount; i++) {
      view[i] = view[i - 1] + linePtrStep;
    }
    res.assign(src.begin(), src.end());

    std::vector<CameraDelta> searchSpace;
    searchSpace.reserve(VOXEL_DELTA_X.size() * VOXEL_DELTA_Y.size()
                        * VOXEL_DELTA_Z.size() * PHI_H_DELTA.size() * PHI_V_DELTA.size());
    generateSearchSpace(searchSpace);
    double maxCoverage = evalTargetCoverage(view, targetCount, cameraCount);
    bool *bestViewSnapshot = new bool[viewMatrixSize]();

    for(size_t j = 0; j < cameraCount; j++) {
      CameraDelta bestDelta(0, 0, 0, 0, 0);
      for(const CameraDelta& delta : searchSpace) {
        Camera searchCamera(res[j]);
        searchCamera.applyDelta(delta);
        for(size_t i = 0; i < targetCount; i++) {
          view[i][j] = searchCamera.canMonitor(targets[i]);
        }
        double coverage = evalTargetCoverage(view, targetCount, cameraCount);
//#ifdef LOCAL_SEARCH_DEBUG
//        std::cout << "After Delta " << searchCamera << ", Coverage: " << coverage << std::endl;
//#endif
        if(coverage > maxCoverage) {
#ifdef LOCAL_SEARCH_DEBUG
//          std::cout << "Delta (" << delta << ") For" << res[j] << std::endl;
          std::cout << "Coverage: " << coverage << std::endl;
#endif
          maxCoverage = coverage;
          bestDelta = delta;
          memcpy(bestViewSnapshot, view[0], viewMatrixSize);
        }
      }
      memcpy(view[0], bestViewSnapshot, viewMatrixSize);
      res[j].applyDelta(bestDelta);
    }

    // Free Memory
    delete[] bestViewSnapshot;
    delete[] view[0];
    delete[] view;
  }

  void generateSearchSpace(std::vector<CameraDelta> &searchDelta) {
    for(int dx : VOXEL_DELTA_X) {
      for(int dy : VOXEL_DELTA_Y) {
        for(int dz : VOXEL_DELTA_Z) {
          for(double dh : PHI_H_DELTA) {
            for(double dv : PHI_V_DELTA) {
              searchDelta.emplace_back(dx, dy, dz, dh, dv);
            }
          }
        }
      }
    }
  }

  double evalTargetCoverage(bool **view, size_t targetCount, std::vector<int> &cameraIndex) {
    size_t viewedCount = 0;
    for(size_t i = 0; i < targetCount; i++) {
      bool isViewed = false;
      for(auto it = cameraIndex.begin(); it != cameraIndex.end(); it++) {
        isViewed |= view[i][*it];
      }
      if(isViewed)
        viewedCount++;
    }
    return viewedCount / (double)targetCount;
  }

  double evalTargetCoverage(bool **view, size_t targetCount, size_t cameraCount) {
    size_t viewedCount = 0;
    for(size_t i = 0; i < targetCount; i++) {
      bool isViewed = false;
      for(size_t j = 0; j < cameraCount; j++) {
        isViewed |= view[i][j];
      }
      if(isViewed)
        viewedCount++;
    }
    return viewedCount / (double)targetCount;
  }

}