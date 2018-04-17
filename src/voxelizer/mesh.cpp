//
// Created by liao xiangsen on 2018/4/18.
//

#include "voxelizer/mesh.h"
#include <vector>
#include "voxelizer/voxelizer.h"

namespace monitor {
  Mesh::Mesh(std::vector<TVec3d> &vertics, std::vector<unsigned int> &indices) {
    this->mesh = vx_mesh_alloc((int)vertics.size() * 3, (int)indices.size());
    memcpy(this->mesh->indices, indices.data(), sizeof(unsigned int) * indices.size());
    int verticIndex = 0;
    for(auto it = vertics.begin(); it != vertics.end(); it++) {
      this->mesh->vertices[verticIndex].x = (float)it->x;
      this->mesh->vertices[verticIndex].y = (float)it->y;
      this->mesh->vertices[verticIndex].z = (float)it->z;
    }
  }
  Mesh::~Mesh() {
    vx_mesh_free(this->mesh);
  }
}