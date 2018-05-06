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

  const double CAMERA_PHI_H_DELTA = 45;
  const double CAMERA_PHI_V_DELTA = 10;

  struct CameraDelta {
    friend std::ostream& operator<<(std::ostream &os, const CameraDelta &delta);
  public:
    int x, y, z;
    double phiH, phiV;
    CameraDelta(int _x, int _y, int _z, double _h, double _v): x(_x), y(_y), z(_z), phiH(_h), phiV(_v) {}
  };

  class Camera {
    friend std::ostream& operator<<(std::ostream &os, const Camera &camera);
  public:
    Camera(std::shared_ptr<Grids> worldPointer, const TVec3d &position, double _phiH, double _phiV);
    Camera(const Camera &other): world(other.world), pos(other.pos.xyz), phiH(other.phiH), phiV(other.phiV) {};
    void applyDelta(const CameraDelta &delta);
    bool canMonitor(const Voxel &target) const;
    int getViewedCount(const std::vector<Voxel> &targets) const;
    inline Voxel getVoxel() const { return Voxel(pos.xyz); }
    void findBestPhiH(const std::vector<Voxel> &targets);
  private:
    std::shared_ptr<Grids> world;
    Voxel pos;
    // Degree
    double phiH, phiV;
  };

  std::ostream& operator<<(std::ostream &os, const Camera &camera);
  std::ostream& operator<<(std::ostream &os, const CameraDelta &delta);
}

#endif //CAMERA_H
