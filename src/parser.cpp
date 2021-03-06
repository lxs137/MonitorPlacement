#include "parser.h"

#include <iostream>
#include <memory>
#include <vector>
#include <cmath>

#include <citygml/cityobject.h>
#include <citygml/polygon.h>
#include <citygml/vecs.hpp>

#include "print.h"
#include "voxelizer/mesh.h"
#include "utils.h"

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

Mesh parseMeshFromCityObject(const citygml::CityObject* object) {
  std::vector<TVec3d> allVertices;
  std::vector<unsigned int> allIndices;
  for(unsigned int i = 0; i < object->getGeometriesCount(); i++) {
    const citygml::Geometry &geometry = object->getGeometry(i);
    for (unsigned int j = 0; j < geometry.getPolygonsCount(); j++) {
      std::shared_ptr<const citygml::Polygon> polygon = geometry.getPolygon(j);
      parseVerticsFromPolygon(polygon, allVertices, allIndices);
    }
  }
  Mesh mesh(allVertices, allIndices);
  return mesh;
}

Mesh parseMeshFromCityObjectRecursive(const citygml::CityObject* object) {
  Mesh mesh = parseMeshFromCityObject(object);
  for (unsigned int i = 0; i < object->getChildCityObjectsCount(); i++) {
    mesh.merge(parseMeshFromCityObjectRecursive(&object->getChildCityObject(i)));
//    mesh.merge(parseMeshFromCityObject(&object->getChildCityObject(i)));
  }
  return mesh;
}

Mesh parseMeshFromCityObjects(const std::vector<const citygml::CityObject*> &objects) {
  Mesh mesh;
  if(objects.size() == 0) {
    return mesh;
  }
  for(auto it = objects.begin(); it != objects.end(); it++) {
    mesh.merge(parseMeshFromCityObject(*it));
  }
  return mesh;
}

void parseVerticsFromPolygon(std::shared_ptr<const citygml::Polygon> polygon, std::vector<TVec3d> &allVertices,
                             std::vector<unsigned int> &allIndices) {
  const std::vector<TVec3d> &vertices = polygon->getVertices();
  const std::vector<unsigned int> &indices = polygon->getIndices();
  const TVec3d &v1 = vertices.front();
  const TVec3d &v2 = vertices.back();
  bool isRing = EQZero(v1.x - v2.x) && EQZero(v1.y - v2.y) && EQZero(v1.z - v2.z);
  unsigned int indiceOffset = (unsigned int)allVertices.size();

  if(isRing) {
    allVertices.insert(allVertices.end(), vertices.begin(), vertices.end() - 1);
    unsigned int sameIndex1 = 0, sameIndex2 = (unsigned int)vertices.size() - 1;
    for(unsigned long i = 0; i < indices.size(); i += 3) {
      unsigned int p1 = indices[i] % sameIndex2, p2 = indices[i + 1] % sameIndex2, p3 = indices[i + 2] % sameIndex2;
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


}

