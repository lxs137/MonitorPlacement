//
// Created by liao xiangsen on 2018/5/1.
//

#include "camera.h"

#include "utils.h"

namespace monitor {
  Camera::Camera(std::shared_ptr<monitor::Grids> worldPointer, const TVec3d &position,
                 double _phiH, double _phiV): world(worldPointer), phiH(_phiH), phiV(_phiV) {
    pos = world->posToVoxel(position);
    phiHCos = cosDegree(phiH);
    phiHSin = sinDegree(phiH);
    phiVCos = cosDegree(phiV);
    phiVSin = sinDegree(phiV);
    double p1Theta = 90.0 - phiV + CAMERA_THETA_V_HALF, p1Phi = phiH - CAMERA_THETA_H_HALF;
    double p2Theta = p1Theta, p2Phi = phiH + CAMERA_THETA_H_HALF;
    double p3Theta = 90.0 - phiV - CAMERA_THETA_V_HALF, p3Phi = p2Phi;
    double p4Theta = p3Theta, p4Phi = p1Phi;
    farPlane[0] = getPosInSphericalCoord(p1Theta, p1Phi);
    farPlane[1] = getPosInSphericalCoord(p2Theta, p2Phi);
    farPlane[2] = getPosInSphericalCoord(p3Theta, p3Phi);
    farPlane[3] = getPosInSphericalCoord(p4Theta, p4Phi);
  }
  bool Camera::canMonitor(const monitor::Voxel &target) const {
    // Test No Blocking
    if(world->intersect(pos, target)) {
      return false;
    }
    int dx = target.x - pos.x, dy = target.y - pos.y, dz = target.z - pos.z;
    // Test Out Of View
    double disTest = dx*phiVCos*phiHCos - dy*phiVCos*phiHSin + dz*phiVSin;
    if(!(disTest <= CAMERA_VIEW_DIS && disTest > 0)) {
      return false;
    }
    // Test Whether Target In Camera's FOV
    double hDegree = atanDegree((double)dy / dx);
    if(dx > 0 && dy > 0) {
      hDegree = 360 - hDegree;
    } else if (dx > 0 && dy < 0) {
      hDegree = -hDegree;
    } else if (dx < 0 && dy < 0) {
      hDegree = 180 - hDegree;
    } else {
      hDegree = 180 - hDegree;
    }
//    double phiHMin = phiH - CAMERA_THETA_H_HALF, phiHMax = phiH + CAMERA_THETA_H_HALF;
    if(!(hDegree >= phiH - CAMERA_THETA_H_HALF && hDegree <= phiH + CAMERA_THETA_H_HALF))
      return false;
    double vDegree = asinDegree(dz / distance3D(dx, dy, dz));
    bool result = (vDegree >= phiV - CAMERA_THETA_V_HALF && vDegree <= phiV + CAMERA_THETA_V_HALF);
//    if(result) {
//      std::cout << "hDegree: " << hDegree << ", vDegree: " << vDegree << std::endl;
//    }
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
  Voxel Camera::getPosInSphericalCoord(const double theta, const double phi) const {
    int x = Clamp((int)(pos.x + CAMERA_VIEW_CORNER_DIS * sinDegree(theta) * cosDegree(phi)),
                  world->gridsIndexStart.x, world->gridsCount.x - 1);
    int y = Clamp((int)(pos.y - CAMERA_VIEW_CORNER_DIS * sinDegree(theta) * sinDegree(phi)),
                  world->gridsIndexStart.y, world->gridsCount.y - 1);
    int z = Clamp((int)(pos.z + CAMERA_VIEW_CORNER_DIS * cosDegree(theta)),
                  world->gridsIndexStart.z, world->gridsCount.z - 1);
    Voxel voxel(x, y, z);
    return voxel;
  }
  std::shared_ptr<monitor::Mesh> Camera::getViewFieldMesh() {
    double p1Theta = 90.0 - phiV + CAMERA_THETA_V_HALF, p1Phi = phiH - CAMERA_THETA_H_HALF;
    double p2Theta = p1Theta, p2Phi = phiH + CAMERA_THETA_H_HALF;
    double p3Theta = 90.0 - phiV - CAMERA_THETA_V_HALF, p3Phi = p2Phi;
    double p4Theta = p3Theta, p4Phi = p1Phi;
    Voxel p1Voxel = getPosInSphericalCoord(p1Theta, p1Phi),
        p2Voxel = getPosInSphericalCoord(p2Theta, p2Phi),
        p3Voxel = getPosInSphericalCoord(p3Theta, p3Phi),
        p4Voxel = getPosInSphericalCoord(p4Theta, p4Phi);
    // Get View Field Area
    std::vector<TVec3d> vertices;
    vertices.push_back(world->voxelToPos(pos));
    vertices.push_back(world->voxelToPos(p1Voxel));
    vertices.push_back(world->voxelToPos(p2Voxel));
    vertices.push_back(world->voxelToPos(p3Voxel));
    vertices.push_back(world->voxelToPos(p4Voxel));
    std::vector<unsigned int> indices = {
        0, 1, 4,
        0, 1, 2,
        0, 2, 3,
        0, 4, 3,
        1, 2, 3,
        1, 3, 4
    };
    return std::make_shared<monitor::Mesh>(vertices, indices);
  }
  void Camera::applyDelta(const monitor::CameraDelta &delta) {
    pos.x = Clamp(pos.x + delta.x, world->gridsIndexStart.x, world->gridsCount.x - 1),
    pos.y = Clamp(pos.y + delta.y, world->gridsIndexStart.y, world->gridsCount.y - 1),
    pos.z = Clamp(pos.z + delta.z, world->gridsIndexStart.z, world->gridsCount.z - 1);
    phiH += delta.phiH , phiV += delta.phiV;
    phiHCos = cosDegree(phiH);
    phiHSin = sinDegree(phiH);
    phiVCos = cosDegree(phiV);
    phiVSin = sinDegree(phiV);
    double p1Theta = 90.0 - phiV + CAMERA_THETA_V_HALF, p1Phi = phiH - CAMERA_THETA_H_HALF;
    double p2Theta = p1Theta, p2Phi = phiH + CAMERA_THETA_H_HALF;
    double p3Theta = 90.0 - phiV - CAMERA_THETA_V_HALF, p3Phi = p2Phi;
    double p4Theta = p3Theta, p4Phi = p1Phi;
    farPlane[0] = getPosInSphericalCoord(p1Theta, p1Phi);
    farPlane[1] = getPosInSphericalCoord(p2Theta, p2Phi);
    farPlane[2] = getPosInSphericalCoord(p3Theta, p3Phi);
    farPlane[3] = getPosInSphericalCoord(p4Theta, p4Phi);
  }

  std::ostream& operator<<(std::ostream &os, const Camera &camera) {
    os << "Camera: " << camera.pos;
    os << " " << std::fixed << camera.phiH << " " << std::fixed << camera.phiV;
    return os;
  }
  std::ostream& operator<<(std::ostream &os, const CameraDelta &delta) {
    os << delta.x << " " << delta.y << " " << delta.z << " "
       << std::fixed << delta.phiH << " " << std::fixed << delta.phiV;
    return os;
  }
}