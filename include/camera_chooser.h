//
// Created by liao xiangsen on 2018/5/4.
//

#ifndef CAMERA_CHOOSER_H
#define CAMERA_CHOOSER_H

#include <vector>


#include "camera.h"

namespace monitor {
  const size_t CAMERA_RES_COUNT = 10;
  void preChooseGreedy(const std::vector<Camera> &src, std::vector<Camera> &res, bool **view, size_t targetCount,
                       size_t resCount = CAMERA_RES_COUNT);
}

#endif //CAMERA_CHOOSER_H
