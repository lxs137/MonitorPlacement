//
// Created by liao xiangsen on 2018/4/18.
//

#include "parser.h"

#include <iostream>
#include <vector>
#include <memory>

#include <citygml/citygml.h>
#include <citygml/citymodel.h>
#include <citygml/cityobject.h>
#include "gtest/gtest.h"
#include "print.h"
#include "voxelizer/mesh.h"

TEST(JUST_TEST, FIRST) {
  citygml::ParserParams params;
  std::shared_ptr<const citygml::CityModel> city = citygml::load("../data/LOD3_1.gml", params);
  std::vector<const citygml::CityObject*> parser_result = city->getAllCityObjectsOfType(citygml::CityObject::CityObjectsType::COT_Window);
  for(auto it = parser_result.begin(); it != parser_result.begin() + 1; it++) {
    std::shared_ptr<monitor::Mesh> mesh = monitor::parseMeshFromCityObject(*it);
    monitor::printCityObject(*it);
    std::cout << *mesh << std::endl;
  }
  EXPECT_EQ(5, monitor::addTest(2, 3));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

