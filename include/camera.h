//
// Created by liao xiangsen on 2018/5/1.
//

#ifndef CAMERA_H
#define CAMERA_H

#include <cmath>
#include <memory>

#include <citygml/vecs.hpp>
#include "world.h"

namespace monitor {
  // Degree
  const double CAMERA_THETA_V = 60.0;
  const double CAMERA_THETA_V_HALF = 0.5 * CAMERA_THETA_V;
  const double CAMERA_THETA_H = 100.0;
  const double CAMERA_THETA_H_HALF = 0.5 * CAMERA_THETA_H;
  const double CAMERA_VIEW_DIS = 15.0;
  const double CAMERA_VIEW_DIS_SQUARE = CAMERA_VIEW_DIS * CAMERA_VIEW_DIS;

  class Camera {
    friend std::ostream& operator<<(std::ostream &os, const Camera &camera);
  public:
    Camera(std::shared_ptr<Grids> worldPointer, const TVec3d &position, double _phiH, double _phiV);
    bool canMonitor(const Voxel &target);
  private:
    std::shared_ptr<Grids> world;
    Voxel pos;
    // Degree
    double phiH, phiV;
  };
  std::ostream& operator<<(std::ostream &os, const Camera &camera);
}

#endif //CAMERA_H
