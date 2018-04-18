#include "print.h"

#include <iostream>
#include <memory>
#include <vector>

#include <citygml/attributesmap.h>
#include <citygml/geometry.h>
#include <citygml/vecs.hpp>
#include <citygml/polygon.h>
#include <citygml/cityobject.h>

namespace monitor {

void printCityObject(const citygml::CityObject* object) {
  std::cout << "Object(" << object->getTypeAsString() << "): " << object->getId() << std::endl;
  const citygml::AttributesMap& map = object->getAttributes();
  if(!map.empty())
    std::cout << "Attributes: " << std::endl;
  for(auto it = map.begin(); it != map.end(); it++) {
    std::cout << "  " << it->first << ": " << it->second << std::endl;
  }
  for(unsigned int i = 0; i < object->getGeometriesCount(); i++) {
    const citygml::Geometry& geometry = object->getGeometry(i);
    std::cout << "Geometry(" << i << "): " << std::endl;  
    for(unsigned int j = 0; j < geometry.getPolygonsCount(); j++) {
      std::shared_ptr<const citygml::Polygon> polygon = geometry.getPolygon(j);
      const std::vector<TVec3d>& vertices = polygon->getVertices();
      if(!vertices.empty())
        std::cout << "Polygon: " << std::endl;
      for(auto it = vertices.begin(); it != vertices.end(); it++) {
        std::cout << "  " << (*it) << std::endl;
      }
      const std::vector<unsigned int>& indices = polygon->getIndices();
      if(!indices.empty()) 
        std::cout << "Indices: " << std::endl;
      for(auto it = indices.begin(); it != indices.end(); it++) {
        std::cout << "  " << (*it) << std::endl;
      }
    }  
    std::cout << std::endl;
  }
}

}
