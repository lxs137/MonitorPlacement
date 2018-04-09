#ifndef PARSER_H
#define PARSER_H

#include <citygml/citygml.h>
#include <vector>

std::vector<const citygml::CityObject*> parserTargetChildObjects(const citygml::CityObject* object, citygml::CityObject::CityObjectsType target);

#endif