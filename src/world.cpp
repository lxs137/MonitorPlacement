//
// Created by liao xiangsen on 2018/4/20.
//

#include "world.h"

#include <iterator>

#include <citygml/citymodel.h>
#include "utils.h"

namespace monitor {
  Grids::Grids(double *length, double *resolution) {
    xSize = length[0], ySize = length[1], zSize = length[2];
    xRes = resolution[0], yRes = resolution[1], zRes = resolution[2];
    invXRes = 1.0 / xRes;
    invYRes = 1.0 / yRes;
    invZRes = 1.0 / zRes;
    xCount = (int)(xSize * invXRes);
    yCount = (int)(ySize * invYRes);
    zCount = (int)(zSize * invZRes);
    grids = new bool[xCount * yCount * zCount]();
    lowerP = TVec3d(0.0, 0.0, 0.0);
  }
  Grids::Grids(double *length, double *resolution, const TVec3d &lower) {
    xSize = length[0], ySize = length[1], zSize = length[2];
    xRes = resolution[0], yRes = resolution[1], zRes = resolution[2];
    invXRes = 1.0 / xRes;
    invYRes = 1.0 / yRes;
    invZRes = 1.0 / zRes;
    xCount = (int)(xSize * invXRes);
    yCount = (int)(ySize * invYRes);
    zCount = (int)(zSize * invZRes);
    grids = new bool[xCount * yCount * zCount]();
    lowerP = TVec3d(lower.x, lower.y, lower.z);
  }
  Grids::~Grids() {
    delete grids;
  }
  void Grids::addVoxels(std::vector<monitor::Voxel> &data) {
    for(auto it = data.begin(); it != data.end(); it++) {
      grids[offset(it->x, it->y, it->z)] = true;
    }
  }
  int Grids::posToVoxel(double posVal, int axis) {
    switch(axis) {
      case 0:
        return Clamp((int)((posVal - lowerP.x) * invXRes), 0, xCount - 1);
      case 1:
        return Clamp((int)((posVal - lowerP.y) * invYRes), 0, yCount - 1);
      case 2:
        return Clamp((int)((posVal - lowerP.z) * invZRes), 0, zCount - 1);
    }
    return 0;
  }
  void Grids::clear() {
    memset(grids, 0, xCount * yCount * zCount);
  }
  bool Grids::intersect(monitor::Voxel &src, monitor::Voxel &dst) {
    Voxel point(src.x, src.y, src.z);
    int dx = dst.x - src.x, dy = dst.y - src.y, dz = dst.z - src.z;
    int x_inc = (dx < 0) ? -1 : 1, y_inc = (dy < 0) ? -1 : 1, z_inc = (dz < 0) ? -1 : 1;
    int l = std::abs(dx), m = std::abs(dy), n = std::abs(dz);
    int dx2 = l << 1, dy2 = m << 1, dz2 = n << 1;

    if ((l >= m) && (l >= n)) {
      int err_1 = dy2 - l;
      int err_2 = dz2 - l;
      for (int i = 0; i < l; i++) {
        if(grids[offset(point.x, point.y, point.z)])
          return true;
        if (err_1 > 0) {
          point.y += y_inc;
          err_1 -= dx2;
        }
        if (err_2 > 0) {
          point.z += z_inc;
          err_2 -= dx2;
        }
        err_1 += dy2;
        err_2 += dz2;
        point.x += x_inc;
      }
    } else if ((m >= l) && (m >= n)) {
      int err_1 = dx2 - m;
      int err_2 = dz2 - m;
      for (int i = 0; i < m; i++) {
        if(grids[offset(point.x, point.y, point.z)])
          return true;
        if (err_1 > 0) {
          point.x += x_inc;
          err_1 -= dy2;
        }
        if (err_2 > 0) {
          point.z += z_inc;
          err_2 -= dy2;
        }
        err_1 += dx2;
        err_2 += dz2;
        point.y += y_inc;
      }
    } else {
      int err_1 = dy2 - n;
      int err_2 = dx2 - n;
      for (int i = 0; i < n; i++) {
        if(grids[offset(point.x, point.y, point.z)])
          return true;
        if (err_1 > 0) {
          point.y += y_inc;
          err_1 -= dz2;
        }
        if (err_2 > 0) {
          point.x += x_inc;
          err_2 -= dz2;
        }
        err_1 += dy2;
        err_2 += dx2;
        point.z += z_inc;
      }
    }
    return grids[offset(point.x, point.y, point.z)];
  }
  Grids cityModelToGrids(std::shared_ptr<const citygml::CityModel> model, double resolution[3]) {
    const citygml::Envelope &envelope = model->getEnvelope();
    const TVec3d &lowerPoint = envelope.getLowerBound(), &upperPoint = envelope.getUpperBound();
    double length[3] = {upperPoint.x - lowerPoint.x, upperPoint.y - lowerPoint.y, upperPoint.z - lowerPoint.z};
    Grids grids(length, resolution, lowerPoint);
    return grids;
  }
  std::ostream& operator<<(std::ostream &os, const Grids& grids) {
    os << "Grdis: " << std::endl;
    os << " Size: " << grids.xSize << " " << grids.ySize << " " << grids.zSize << std::endl;
    os << " Grids Count: " << grids.xCount << " " << grids.yCount << " " << grids.zCount << std::endl;
    unsigned long count = 0;
    for(int i = 0; i < grids.xCount; i++) {
      for(int j = 0; j < grids.yCount; j++) {
        for(int k = 0; k < grids.zCount; k++) {
          if(grids.grids[grids.offset(i, j, k)]) {
//            os << " (" << i << ", " << j << ", " << k << ") ";
            count++;
          }
        }
      }
    }
    os << "\n Data Grids Count: " << count << std::endl;
    return os;
  }
}