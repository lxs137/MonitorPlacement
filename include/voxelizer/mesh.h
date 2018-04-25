#ifndef VOXELIZER_MESH_H_
#define VOXELIZER_MESH_H_

#include <string.h>
#include <vector>
#include <memory>
#include <ostream>

#include <citygml/vecs.hpp>
#include "voxelizer/voxelizer.h"

namespace monitor {
  typedef TVec3<int> Voxel;

  class Mesh {
    friend std::ostream& operator<<(std::ostream& os, const Mesh& mesh);
  public:
    Mesh(const Mesh&);
    Mesh(std::vector<TVec3d> &vertics, std::vector<unsigned int> &indices);
    size_t getVerticesCount() { return this->mesh->nvertices; }
    size_t getIndicesCount() { return this->mesh->nindices; }
    void voxelizer(std::vector<Voxel> &voxels, double resolution[3]);
    void merge(const Mesh&);
    void writeToFile(std::string filename);
    ~Mesh();
  private:
    vx_mesh_t* mesh;
  };
  std::ostream& operator<<(std::ostream& os, const Mesh& mesh);
}

#endif
