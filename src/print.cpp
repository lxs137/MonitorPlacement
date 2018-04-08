#include <iostream>
#include <citygml/attributesmap.h>
#include <citygml/geometry.h>
#include "print.h"

void printCityObject(const citygml::CityObject& object) {
  std::cout << "Object(" << object.getTypeAsString() << "): " << object.getId() << std::endl;
  const citygml::AttributesMap& map = object.getAttributes();
  if(!map.empty())
    std::cout << "Attributes: " << std::endl;
  for(auto it = map.begin(); it != map.end(); it++) {
    std::cout << " " << it->first << ": " << it->second << std::endl;
  }
  for(unsigned int i = 0; i < object.getGeometriesCount(); i++) {
    const citygml::Geometry& geometry = object.getGeometry(i); 
    std::cout << geometry.getTypeAsString() << std::endl;
  }
}