//
// Created by liao xiangsen on 2018/4/18.
//

#include "voxelizer/mesh.h"

#include <vector>
#include "voxelizer/voxelizer.h"

namespace monitor {
  Mesh::Mesh(const monitor::Mesh &other) {
    vx_mesh_t *otherMesh = other.mesh;
    mesh = vx_mesh_alloc((int)otherMesh->nvertices, (int)otherMesh->nindices);
    memcpy(mesh->vertices, otherMesh->vertices, sizeof(vx_vertex_t) * otherMesh->nvertices);
    memcpy(mesh->indices, otherMesh->indices, sizeof(unsigned int) * otherMesh->nindices);
  }

  Mesh::Mesh(std::vector<TVec3d> &vertics, std::vector<unsigned int> &indices) {
    this->mesh = vx_mesh_alloc((int)vertics.size(), (int)indices.size());
    memcpy(this->mesh->indices, indices.data(), sizeof(unsigned int) * indices.size());
    int verticIndex = 0;
    for(auto it = vertics.begin(); it != vertics.end(); it++) {
      this->mesh->vertices[verticIndex].x = (float)it->x;
      this->mesh->vertices[verticIndex].y = (float)it->y;
      this->mesh->vertices[verticIndex].z = (float)it->z;
      verticIndex++;
    }
  }

  Mesh::~Mesh() {
    vx_mesh_free(this->mesh);
  }

  void Mesh::voxelizer(std::vector<monitor::Voxel> &voxels, double resolution[3]) {
    float precF = resolution[0] * 0.1f,
        resF_1[3] = {1.0f / (float)resolution[0], 1.0f / (float)resolution[1], 1.0f / (float)resolution[2]};
    vx_point_cloud_t* result = vx_voxelize_pc(this->mesh, resolution[0], resolution[1], resolution[2], precF);
    std::vector<monitor::Voxel> resultVoxel;
    resultVoxel.reserve(result->nvertices);
    for(size_t i = 0; i < result->nvertices; i++) {
      vx_vertex_t *v = &(result->vertices[i]);
      resultVoxel.emplace_back((int)(v->x * resF_1[0]), (int)(v->y * resF_1[1]), (int)(v->z * resF_1[2]));
    }
    voxels.insert(voxels.end(), resultVoxel.begin(), resultVoxel.end());
    vx_point_cloud_free(result);
  }

  void Mesh::merge(const monitor::Mesh& that) {
    vx_mesh_t *thatMesh = that.mesh;
    if(thatMesh == mesh)
      return;
    vx_mesh_t *newMesh = vx_mesh_alloc((int)(mesh->nvertices + thatMesh->nvertices),
                                       (int)(mesh->nindices + thatMesh->nindices));
    memcpy(newMesh->vertices, this->mesh->vertices, sizeof(vx_vertex_t) * mesh->nvertices);
    memcpy(&newMesh->vertices[mesh->nvertices], thatMesh->vertices, sizeof(vx_vertex_t) * thatMesh->nvertices);
    memcpy(newMesh->indices, mesh->indices, sizeof(unsigned int) * mesh->nindices);
    unsigned int indiceOffset = (unsigned int)mesh->nvertices;
    unsigned int *thatIndices = &newMesh->indices[mesh->nindices];
    for(size_t i = 0; i < thatMesh->nindices; i++) {
      thatIndices[i] = thatMesh->indices[i] + indiceOffset;
    }
    vx_mesh_free(mesh);
    mesh = newMesh;
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