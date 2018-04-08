#ifndef PARSER_H
#define PARSER_H

#include <citygml/citygml.h>
#include <vector>

std::vector<citygml::CityObject> parserTargetChildObjects(const citygml::CityObject& object, const std::vector<citygml::CityObject::CityObjectsType>& target);

#endif