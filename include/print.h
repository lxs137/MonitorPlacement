#ifndef PRINT_H_
#define PRINT_H_

#include <vector>
#include <citygml/citygml.h>

namespace monitor {
  void printCityObject(const citygml::CityObject* object);
}

#endif