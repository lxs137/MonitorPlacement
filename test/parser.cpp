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

TEST(PARSE_MESH_FROM_CITY_OBJECT, VERTICES_COUNT) {
  citygml::ParserParams params;
  std::shared_ptr<const citygml::CityModel> city = citygml::load("./data/LOD3_1.gml", params);
  std::vector<const citygml::CityObject*> parser_result = city->getAllCityObjectsOfType(citygml::CityObject::CityObjectsType::COT_Window);
  const citygml::CityObject* object = parser_result.front();
  std::shared_ptr<monitor::Mesh> mesh = monitor::parseMeshFromCityObject(object);
  std::vector<monitor::Voxel> voxels;
  mesh->voxelizer(voxels, 0.25, 0.025);
  monitor::printCityObject(object);
  std::cout << *mesh << std::endl;
  for(auto it = voxels.begin(); it != voxels.end(); it++) {
    std::cout << *it << std::endl;
  }
  EXPECT_EQ(object->getGeometry(0).getPolygon(0)->getVertices().size(), mesh->getVerticesCount() + 1);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

