#ifndef VOXELIZER_MESH_H_
#define VOXELIZER_MESH_H_

#include <string.h>
#include <vector>
#include <ostream>

#include <citygml/vecs.hpp>
#include "voxelizer/voxelizer.h"

namespace monitor {
  class Mesh {
    friend std::ostream& operator<<(std::ostream& os, const Mesh& mesh);
  public:
    Mesh(std::vector<TVec3d> &vertics, std::vector<unsigned int> &indices);
    ~Mesh();
  private:
    vx_mesh_t* mesh;
  };
  std::ostream& operator<<(std::ostream& os, const Mesh& mesh);
}

#endif
