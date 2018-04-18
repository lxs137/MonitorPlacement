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

  std::ostream& operator<<(std::ostream& os, const Mesh& mesh) {
    os << "monitor::Mesh" << std::endl;
    os << " nVertices: " << mesh.mesh->nvertices << std::endl;
    if(mesh.mesh->nvertices != 0)
      os << " Vertices: " << std::endl;
    for(size_t i = 0; i < mesh.mesh->nvertices; i++) {
      vx_vertex_t *v = &(mesh.mesh->vertices[i]);
      os << " " << v->x << " " << v->y << " " << v->z << std::endl;
    }
    os << " nIndices: " << mesh.mesh->nindices << std::endl;
    if(mesh.mesh->nindices != 0)
      os << " Indices: " << std::endl;
    for(size_t i = 0; i < mesh.mesh->nindices; i += 3) {
      unsigned int *triIndices = &(mesh.mesh->indices[i]);
      os << " " << triIndices[0] << " " << triIndices[1] << " " << triIndices[2] << std::endl;
    }
    return os;
  }
}