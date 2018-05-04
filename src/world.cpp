//
// Created by liao xiangsen on 2018/4/20.
//

#include "world.h"

#include <iterator>

#include <citygml/citymodel.h>
#include "utils.h"

namespace monitor {
  Grids::Grids(double *length, double *resolution)
      :gridsLength(length[0], length[1], length[2]), res(resolution[0], resolution[1], resolution[2]) {
    invRes.x = 1.0 / res.x;
    invRes.y = 1.0 / res.y;
    invRes.z = 1.0 / res.z;
    gridsCount.x = (int)(gridsLength.x * invRes.x);
    gridsCount.y = (int)(gridsLength.y * invRes.y);
    gridsCount.z = (int)(gridsLength.z * invRes.z);
    grids = new bool[gridsCount.x * gridsCount.y * gridsCount.z]();
    lowerP.x = 0.0, lowerP.y = 0.0, lowerP.z = 0.0;
    upperP.x = length[0], upperP.y = length[1], upperP.z = length[2];
    gridsIndexStart.x = 0, gridsIndexStart.y = 0, gridsIndexStart.z = 0;
    gridsIndexEnd.x = gridsCount.x - 1, gridsIndexEnd.y = gridsCount.y - 1, gridsIndexEnd.z = gridsCount.z - 1;
  }
  Grids::Grids(double *length, double *resolution, const TVec3d &lower)
      :gridsLength(length[0], length[1], length[2]), res(resolution[0], resolution[1], resolution[2]) {
    invRes.x = 1.0 / res.x;
    invRes.y = 1.0 / res.y;
    invRes.z = 1.0 / res.z;
    gridsCount.x = (int)(gridsLength.x * invRes.x);
    gridsCount.y = (int)(gridsLength.y * invRes.y);
    gridsCount.z = (int)(gridsLength.z * invRes.z);
    grids = new bool[gridsCount.x * gridsCount.y * gridsCount.z]();
    for(int i = 0; i < 3; i++) {
      lowerP.xyz[i] = lower.xyz[i];
      upperP.xyz[i] = lowerP.xyz[i] + length[i];
      if(!EQZero(lower.xyz[i])) {
        gridsIndexStart.xyz[i] = (int)(lower.xyz[i] * invRes.xyz[i]);
        gridsIndexEnd.xyz[i] = gridsCount.xyz[i] + gridsIndexStart.xyz[i] - 1;
      } else {
        gridsIndexStart.xyz[i] = 0;
        gridsIndexEnd.xyz[i] = gridsCount.xyz[i] - 1;
      }
    }
  }
  Grids::~Grids() {
    delete grids;
  }
  void Grids::addVoxels(std::vector<monitor::Voxel> &data) {
    for(auto it = data.begin(); it != data.end(); it++) {
      grids[offset(it->x, it->y, it->z)] = true;
    }
  }
  void Grids::removeVoxels(std::vector<monitor::Voxel> &data) {
    for(auto it = data.begin(); it != data.end(); it++) {
      grids[offset(it->x, it->y, it->z)] = false;
    }
  }
  Voxel Grids::posToVoxel(const TVec3d &pos) {
    int x = Clamp((int)(pos.x * invRes.x), gridsIndexStart.x, gridsCount.x - 1),
        y = Clamp((int)(pos.y * invRes.y), gridsIndexStart.y, gridsCount.y - 1),
        z = Clamp((int)(pos.z * invRes.z), gridsIndexStart.z, gridsCount.z - 1);
    Voxel voxel(x, y, z);
    return voxel;
  }
  TVec3d Grids::voxelToPos(const monitor::Voxel &voxel) {
    double x = Clamp(voxel.x * res.x, lowerP.x, upperP.x),
        y = Clamp(voxel.y * res.y, lowerP.y, upperP.y),
        z = Clamp(voxel.z * res.z, lowerP.z, upperP.z);
    TVec3d pos(x, y, z);
    return pos;
  }
  void Grids::clear() {
    memset(grids, 0, gridsCount.x * gridsCount.y * gridsCount.z);
  }
  bool Grids::intersect(const Voxel &src, const Voxel &dst) {
    Voxel point(src.x, src.y, src.z);
    int dx = dst.x - src.x, dy = dst.y - src.y, dz = dst.z - src.z;
    int x_inc = (dx < 0) ? -1 : 1, y_inc = (dy < 0) ? -1 : 1, z_inc = (dz < 0) ? -1 : 1;
    int l = std::abs(dx), m = std::abs(dy), n = std::abs(dz);
    int dx2 = l << 1, dy2 = m << 1, dz2 = n << 1;

    if ((l >= m) && (l >= n)) {
      int err_1 = dy2 - l;
      int err_2 = dz2 - l;
      for (int i = 0; i < l; i++) {
        if(grids[offset(point.x, point.y, point.z)]) {
#ifdef DEBUG
          std::cout << "(" << src <<") to (" << dst << ") Block By (" << point << ")" << std::endl;
#endif
          return true;
        }
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
        if(grids[offset(point.x, point.y, point.z)]) {
#ifdef DEBUG
          std::cout << "(" << src <<") to (" << dst << ") Block By (" << point << ")" << std::endl;
#endif
          return true;
        }
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
        if(grids[offset(point.x, point.y, point.z)]) {
#ifdef DEBUG
          std::cout << "(" << src <<") to (" << dst << ") Block By (" << point << ")" << std::endl;
#endif
          return true;
        }
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
  size_t Grids::getDatagridCount() const {
    size_t count = 0;
    for(int i = gridsIndexStart.x; i < gridsIndexEnd.x; i++) {
      for(int j = gridsIndexStart.y; j < gridsIndexEnd.y; j++) {
        for(int k = gridsIndexStart.z; k < gridsIndexEnd.z; k++) {
          if(grids[offset(i, j, k)]) {
            count++;
          }
        }
      }
    }
    return count;
  }
  std::shared_ptr<Grids> cityModelToGrids(std::shared_ptr<const citygml::CityModel> model, double resolution[3]) {
    const citygml::Envelope &envelope = model->getEnvelope();
    const TVec3d &lowerPoint = envelope.getLowerBound(), &upperPoint = envelope.getUpperBound();
    double length[3] = {upperPoint.x - lowerPoint.x, upperPoint.y - lowerPoint.y, upperPoint.z - lowerPoint.z};
    return std::make_shared<Grids>(length, resolution, lowerPoint);
  }
  std::ostream& operator<<(std::ostream &os, const Grids& grids) {
    os << " Length: " << grids.gridsLength << std::endl;
    os << " Grids Count: " << grids.gridsCount << std::endl;
    os << " Data Grids Count: " << grids.getDatagridCount();
    return os;
  }
}