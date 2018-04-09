#include "parser.h"
#include <citygml/cityobject.h>

std::vector<const citygml::CityObject*> parserTargetChildObjects(const citygml::CityObject* object, citygml::CityObject::CityObjectsType target) {
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