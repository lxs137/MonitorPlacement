//
// Created by liao xiangsen on 2018/4/24.
//

#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <ctime>
#include <cmath>

#define PI 3.14
#define RADIAN_TO_DEGREE 57.31
#define DOUBLE_ZERO 1e-4

//#define DEBUG

namespace monitor {
  inline int Clamp(const int value, const int low, const int high) {
    if(value <= low)
      return low;
    else if(value >= high)
      return high;
    else
      return value;
  }
  inline double Clamp(const double value, const double low, const double high) {
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
  inline void UpdateRandomSeed() {
    srand(time(nullptr));
  }
  inline bool EQZero(const double val) {
    return val < DOUBLE_ZERO && val > -DOUBLE_ZERO;
  }
  inline double atanDegree(const double val) {
    return std::atan(val) * RADIAN_TO_DEGREE;
  }
  inline double asinDegree(const double val) {
    return std::asin(val) * RADIAN_TO_DEGREE;
  }
  inline double distance3D(const double dx, const double dy, const double dz) {
    return std::sqrt(dx * dx + dy * dy + dz * dz);
  }
}

#endif //UTILS_H
