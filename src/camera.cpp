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
  bool Camera::canMonitor(const monitor::Voxel &target) const {
    // Test No Blocking
    if(world->intersect(pos, target)) {
      return false;
    }
    int dx = target.x - pos.x, dy = target.y - pos.y, dz = target.z - pos.z;
    // Test Out Of View
    if(distance3D(dx, dy, dx) >= CAMERA_VIEW_DIS_SQUARE) {
      return false;
    }
    // Test Whether Target In Camera's FOV
    double hDegree = atanDegree((double)dx / dy);
    if(!(hDegree >= phiH - CAMERA_THETA_H_HALF && hDegree <= phiH + CAMERA_THETA_H_HALF))
      return false;
    double vDegree = asinDegree(dz / distance3D(dx, dy, dz));
    bool result = (vDegree >= phiV - CAMERA_THETA_V_HALF && vDegree <= phiV + CAMERA_THETA_V_HALF);
    return result;
  }
  int Camera::getViewedCount(const std::vector<monitor::Voxel> &targets) const {
    int viewedTargetCount = 0;
    for(size_t i = 0; i < targets.size(); i++) {
      if(canMonitor(targets[i]))
        viewedTargetCount++;
    }
    return viewedTargetCount;
  }
  void Camera::findBestPhiH(const std::vector<monitor::Voxel> &targets){
    int planCount = (int)(360.0 / CAMERA_PHI_H_DELTA);
    int mostTargetsCount = 0;
    double startPhiH = phiH;
    double bestPhiH = phiH;
    for(int i = 0; i < planCount; i++) {
      phiH = startPhiH + i * CAMERA_PHI_H_DELTA;
      int viewedTargetCount = getViewedCount(targets);
      if(viewedTargetCount > mostTargetsCount) {
        bestPhiH = phiH;
        mostTargetsCount = viewedTargetCount;
      }
    }
    phiH = bestPhiH;
  }

  std::ostream& operator<<(std::ostream &os, const Camera &camera) {
    os << "Camera: " << camera.pos;
    os << " " << std::fixed << camera.phiH << " " << std::fixed << camera.phiV;
    return os;
  }
}