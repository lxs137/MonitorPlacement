//
// Created by liao xiangsen on 2018/5/4.
//

#ifndef CAMERA_CHOOSER_H
#define CAMERA_CHOOSER_H

#include <vector>
#include <array>

#include <citygml/vecs.hpp>
#include "camera.h"

namespace monitor {
  const size_t CAMERA_RES_COUNT = 10;
  const std::array<int, 3> VOXEL_DELTA_X = {{-2, 0, 2}};
  const std::array<int, 3> VOXEL_DELTA_Y = {{-2, 0, 2}};
  const std::array<int, 3> VOXEL_DELTA_Z = {{-2, 0, 2}};
  // const std::array<int, 3> VOXEL_DELTA_X = {{-1, 0, 1}};
  // const std::array<int, 3> VOXEL_DELTA_Y = {{-1, 0, 1}};
  // const std::array<int, 3> VOXEL_DELTA_Z = {{-1, 0, 1}};
  // const std::array<int, 3> VOXEL_DELTA_X = {{-4, 0, 4}};
  // const std::array<int, 3> VOXEL_DELTA_Y = {{-4, 0, 4}};
  // const std::array<int, 3> VOXEL_DELTA_Z = {{-4, 0, 4}};
  // Degree
  const std::array<double, 12> PHI_H_DELTA = {{0.0, 30.0, 60.0, 90.0, 120.0, 150.0, 180.0, 210.0, 240, 270, 300, 330}};
  const std::array<double, 7> PHI_V_DELTA = {{-30.0, -20.0, -10.0, 0.0, 10.0, 20.0, 30.0}};

  void reEvalViewMatrix(bool **viewMatrix, int choosedCameraIndex, size_t targetCount, std::vector<int> &cameraViewedCount);

  void preChooseGreedy(const std::vector<Camera> &src, std::vector<int> &resIndex, bool **view, size_t targetCount,
                       size_t resCount = CAMERA_RES_COUNT);

  double evalTargetCoverage(bool **view, size_t targetCount, std::vector<int> &cameraIndex);

  double evalTargetCoverage(bool **view, size_t targetCount, size_t cameraCount);

  void localSearch(const std::vector<Camera> &src, const std::vector<Voxel> &targets, bool **view, std::vector<Camera> &res);

  void generateSearchSpace(std::vector<CameraDelta> &searchDelta);
}

#endif //CAMERA_CHOOSER_H
