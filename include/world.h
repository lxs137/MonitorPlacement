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
  class Camera;
  class Grids {
    friend Camera;
    friend std::ostream& operator<<(std::ostream &os, const Grids& grids);
  public:
    Grids() {};
    Grids(double length[3], double resolution[3], const TVec3d &lower);
    Grids(double length[3], double resolution[3]);
    void addVoxels(std::vector<Voxel> &data);
    void removeVoxels(std::vector<Voxel> &data);
    void clear();
    // Fast Intersect Use Bresenham
    bool intersect(const Voxel &src, const Voxel &dst);
    Voxel posToVoxel(const TVec3d &pos) const;
    TVec3d voxelToPos(const Voxel &voxel) const;
    std::shared_ptr<monitor::Mesh> voxelToMesh(const Voxel &voxel) const;
    std::shared_ptr<monitor::Mesh> gridsToMesh() const;
    size_t getDatagridCount() const;
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
    TVec3d lowerP, upperP;
    bool *grids;
  };
  std::ostream& operator<<(std::ostream &os, const Grids& grids);
  std::shared_ptr<Grids> cityModelToGrids(std::shared_ptr<const citygml::CityModel> model, double resolution[3]);
}

#endif //WORLD_H_
