#ifndef VOXELIZER_MESH_H_
#define VOXELIZER_MESH_H_

#include <string.h>
#include <vector>
#include <citygml/vecs.hpp>
#include "voxelizer/voxelizer.h"

namespace monitor {
  class Mesh {
  public:
    Mesh(std::vector<TVec3d> &vertics, std::vector<unsigned int> &indices) {
      mesh = vx_mesh_alloc(vertics.size() * 3, indices.size());
      memcpy(mesh->indices, indices.data(), sizeof(unsigned int) * indices.size());
      int verticIndex = 0;
      for(auto it = vertics.begin(); it != vertics.end(); it++) {
        mesh->vertices[verticIndex].x = it->x;
        mesh->vertices[verticIndex].y = it->y;
        mesh->vertices[verticIndex].z = it->z;
      }
    }
    ~Mesh() {
      vx_mesh_free(mesh);
    }
  private:
    vx_mesh_t* mesh;
  };
}

#endif
