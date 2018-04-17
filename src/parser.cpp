#include "parser.h"

#include <memory>
#include <vector>
#include <cmath>

#include <citygml/cityobject.h>
#include <citygml/polygon.h>
#include <citygml/vecs.hpp>
#include "voxelizer/mesh.h"

#define EQUAL(a,b) std::abs(a-b)<1e-4

namespace monitor {

std::vector<const citygml::CityObject*> parserTargetChildObjects(const citygml::CityObject* object,
                                                                 citygml::CityObject::CityObjectsType target) {
  std::vector<const citygml::CityObject*> results;
  if(object->getType() == target) {
    results.push_back(object);
  }
  for(unsigned int i = 0; i < object->getChildCityObjectsCount(); i++) {
    std::vector<const citygml::CityObject*> child = parserTargetChildObjects(&(object->getChildCityObject(i)), target);
    results.insert(results.end(), child.begin(), child.end());
  }
  return results;
}

const std::shared_ptr<Mesh> parseMeshFromCityObject(const citygml::CityObject* object) {
  std::vector<TVec3d> allVertices;
  std::vector<unsigned int> allIndices;
  for(unsigned int i = 0; i < object->getGeometriesCount(); i++) {
    const citygml::Geometry &geometry = object->getGeometry(i);
    for (unsigned int j = 0; j < geometry.getPolygonsCount(); j++) {
      std::shared_ptr<const citygml::Polygon> polygon = geometry.getPolygon(j);
      parseVerticsFromPolygon(polygon, allVertices, allIndices);
    }
  }
  return std::make_shared<Mesh>(allVertices, allIndices);
}

void parseVerticsFromPolygon(std::shared_ptr<const citygml::Polygon> polygon, std::vector<TVec3d> &allVertices,
                             std::vector<unsigned int> &allIndices) {
  const std::vector<TVec3d> &vertices = polygon->getVertices();
  const std::vector<unsigned int> &indices = polygon->getIndices();
  const TVec3d &v1 = vertices.front();
  const TVec3d &v2 = vertices.back();
  bool isRing = EQUAL(v1.x, v2.x) && EQUAL(v1.y, v2.y) && EQUAL(v1.z, v2.z);
  unsigned int indiceOffset = (unsigned int)allVertices.size();

  if(isRing) {
    allVertices.insert(allVertices.end(), vertices.begin(), vertices.end() - 1);
    unsigned int sameIndex1 = 0, sameIndex2 = (unsigned int)vertices.size() - 1;
    for(unsigned long i = 0; i < indices.size(); i += 3) {
      unsigned int p1 = indices[i], p2 = indices[i + 1], p3 = indices[i + 2];
      if((sameIndex1 != p1 && sameIndex1 != p2 && sameIndex1 != p3)
              || (sameIndex2 != p1 && sameIndex2 != p2 && sameIndex2 != p3)) {
        allIndices.push_back(p1 + indiceOffset);
        allIndices.push_back(p2 + indiceOffset);
        allIndices.push_back(p3 + indiceOffset);
      }
    }
  } else {
    allVertices.insert(allVertices.end(), vertices.begin(), vertices.end());
    for(unsigned int i : indices) {
      allIndices.push_back(i + indiceOffset);
    }
  }
}

int addTest(int i, int j) {
  return i + j;
}


}

