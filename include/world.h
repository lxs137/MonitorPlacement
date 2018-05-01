//
// Created by liao xiangsen on 2018/4/20.
//

#ifndef WORLD_H_
#define WORLD_H_

#include <vector>
#include <memory>
#include <climits>

#include <citygml/vecs.hpp>
#include <citygml/citymodel.h>
#include "voxelizer/mesh.h"

namespace monitor {
  class Grids {
    friend std::ostream& operator<<(std::ostream &os, const Grids& grids);
  public:
    Grids(double length[3], double resolution[3], const TVec3d &lower);
    Grids(double length[3], double resolution[3]);
    void addVoxels(std::vector<Voxel>& data);
    void clear();
    // Fast Intersect Use Bresenham
    bool intersect(Voxel &src, Voxel &dst, int maxStep = INT_MAX);
    Voxel posToVoxel(TVec3d &pos);
    inline bool exist(int x, int y, int z) {
      return grids[offset(x, y, z)];
    }
    inline bool exist(Voxel &voxel) {
      return grids[offset(voxel.x, voxel.y, voxel.z)];
    }
    ~Grids();
  private:
    inline int offset(int x, int y, int z) const {
      if(x > gridsIndexEnd.x || y > gridsIndexEnd.y || z > gridsIndexEnd.z
         || x < gridsIndexStart.x || y < gridsIndexStart.y || z < gridsIndexStart.z)
        return 0;
      return (z - gridsIndexStart.z) * gridsCount.x * gridsCount.y
             + (y - gridsIndexStart.y) * gridsCount.x + (x - gridsIndexStart.x);
    }
    TVec3<int> gridsCount, gridsIndexStart, gridsIndexEnd;
    TVec3d gridsLength;
    TVec3d res;
    TVec3d invRes;
    TVec3d lowerP;
    bool *grids;
  };
  std::ostream& operator<<(std::ostream &os, const Grids& grids);
  Grids cityModelToGrids(std::shared_ptr<const citygml::CityModel> model, double resolution[3]);
}

#endif //WORLD_H_
