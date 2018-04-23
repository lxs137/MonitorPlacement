#ifndef PARSER_H_
#define PARSER_H_

#include <vector>
#include <memory>
#include <citygml/citygml.h>
#include <citygml/polygon.h>
#include "voxelizer/mesh.h"

namespace monitor {
  std::vector<const citygml::CityObject*> parserTargetChildObjects(const citygml::CityObject* object, citygml::CityObject::CityObjectsType target);

  Mesh parseMeshFromCityObject(const citygml::CityObject* object);

  void parseVerticsFromPolygon(std::shared_ptr<const citygml::Polygon> polygon, std::vector<TVec3d> &allVertices,
                               std::vector<unsigned int> &allIndices);
}


#endif