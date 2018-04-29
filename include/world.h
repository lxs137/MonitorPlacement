//
// Created by liao xiangsen on 2018/4/20.
//

#ifndef WORLD_H_
#define WORLD_H_

#include <vector>
#include <memory>

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
    bool intersect(Voxel &src, Voxel &dst);
    // axis: 0, 1, 2
    // axis: x, y, z
    int posToVoxel(double posVal, int axis);
    inline bool exist(int x, int y, int z) {
      return grids[offset(x, y, z)];
    }
    ~Grids();
  private:
    inline int offset(int x, int y, int z) const {
      if(x >= xCount || y >= yCount || z >= zCount || x < 0 || y < 0 || z < 0)
        return 0;
      return z * xCount * yCount + y * xCount + x;
    }
    int xCount, yCount, zCount;
    double xSize, ySize, zSize;
    // Voxel Resolution
    double xRes, yRes, zRes;
    // invXRes = 1 / xRes
    double invXRes, invYRes, invZRes;
    TVec3d lowerP;
    bool *grids;
  };
  std::ostream& operator<<(std::ostream &os, const Grids& grids);
  Grids cityModelToGrids(std::shared_ptr<const citygml::CityModel> model, double resolution[3]);
}

#endif //WORLD_H_
