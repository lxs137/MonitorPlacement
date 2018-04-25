//
// Created by liao xiangsen on 2018/4/24.
//

#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <ctime>

namespace monitor {
  inline int Clamp(const int value, const int low, const int high) {
    if(value <= low)
      return low;
    else if(value >= high)
      return high;
    else
      return value;
  }
  inline int RandomInt(const int min, const int max) {
    if(min >= max)
      return min;
    int range = max - min + 1;
    return min + (rand() % range);
  }
  inline float RandomFloat(const float min, const float max) {
    if(min >= max)
      return min;
    int range = max - min;
    return (rand() / (float)RAND_MAX * range) + min;
  }
  inline double RandomDouble(const double min, const double max) {
    if(min >= max)
      return min;
    int range = max - min;
    return (rand() / (double)RAND_MAX * range) + min;
  }
  inline void update_random_seed() {
    srand(time(nullptr));
  }
}

#endif //UTILS_H