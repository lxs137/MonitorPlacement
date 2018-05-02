//
// Created by liao xiangsen on 2018/5/1.
//

#include "camera.h"

#include "utils.h"

namespace monitor {
  Camera::Camera(std::shared_ptr<monitor::Grids> worldPointer, const TVec3d &position,
                 double _phiH, double _phiV): world(worldPointer), phiH(_phiH), phiV(_phiV) {
    pos = world->posToVoxel(position);
  }
  bool Camera::canMonitor(const monitor::Voxel &target) {
    // Test Distance And No Blocking
    if(!world->intersect(pos, target, CAMERA_VIEW_DIS)) {
      return false;
    }
    // Test Whether Target In Camera's FOV
    int dx = target.x - pos.x, dy = target.y - pos.y, dz = target.z - pos.z;
    double hDegree = atanDegree((double)dx / dy);
    if(!(hDegree >= phiH - CAMERA_THETA_H_HALF && hDegree <= phiH + CAMERA_THETA_H_HALF))
      return false;
    double vDegree = asinDegree(dz / distance3D(dx, dy, dz));
    return (vDegree >= phiV - CAMERA_THETA_V_HALF && vDegree <= phiV + CAMERA_THETA_V_HALF);
  }

  std::ostream& operator<<(std::ostream &os, const Camera &camera) {
    os << "Camera: " << camera.pos;
    os << " " << std::fixed << camera.phiH << " " << std::fixed << camera.phiV;
    return os;
  }
}