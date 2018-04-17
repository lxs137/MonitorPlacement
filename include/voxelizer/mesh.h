#ifndef VOXELIZER_MESH_H_
#define VOXELIZER_MESH_H_

#include <string.h>
#include <vector>
#include <citygml/vecs.hpp>
#include "voxelizer/voxelizer.h"

namespace monitor {
  class Mesh {
  public:
    Mesh(std::vector<TVec3d> &vertics, std::vector<unsigned int> &indices);
    ~Mesh();
  private:
    vx_mesh_t* mesh;
  };
}

#endif
