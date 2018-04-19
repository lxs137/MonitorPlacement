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

namespace {
  class CityObjectParserTest : public ::testing::Test {
  public:
    static void SetUpTestCase() {
      citygml::ParserParams params;
      city = citygml::load("./data/LOD3_1.gml", params);
    }
  protected:
    virtual void SetUp() {
      parserResult = CityObjectParserTest::city->getAllCityObjectsOfType(citygml::CityObject::CityObjectsType::COT_Window);
    };

    virtual void TearDown() {
    };
    static std::shared_ptr<const citygml::CityModel> city;
    std::vector<const citygml::CityObject*> parserResult;

  };

  std::shared_ptr<const citygml::CityModel> CityObjectParserTest::city = nullptr;

  TEST_F(CityObjectParserTest, VOXELIZER_TEST) {
    const citygml::CityObject* object = parserResult.front();
    std::shared_ptr<monitor::Mesh> mesh = monitor::parseMeshFromCityObject(object);
    std::vector<monitor::Voxel> voxels;
    double resolution = 0.15;
    mesh->voxelizer(voxels, resolution);
    monitor::printCityObject(object);
    std::cout << *mesh << std::endl;
    for(auto it = voxels.begin(); it != voxels.end(); it++) {
      std::cout << *it << std::endl;
      std::cout << it->x * resolution << " " << it->y * resolution << " " << it->z * resolution << std::endl;
    }
    EXPECT_EQ(object->getGeometry(0).getPolygon(0)->getVertices().size(), mesh->getVerticesCount() + 1);
  }

  TEST_F(CityObjectParserTest, MESH_MERGE_TEST) {
    const citygml::CityObject* object1 = parserResult[0];
    const citygml::CityObject* object2 = parserResult[1];
    std::shared_ptr<monitor::Mesh> mesh1 = monitor::parseMeshFromCityObject(object1);
    std::shared_ptr<monitor::Mesh> mesh2 = monitor::parseMeshFromCityObject(object2);
    std::cout << "Mesh1: \n" << *mesh1 << std::endl;
    std::cout << "Mesh2: \n" << *mesh2 << std::endl;
    mesh1->merge(mesh2);
    std::cout << "Mesh1: \n" << *mesh1 << std::endl;
    EXPECT_TRUE(mesh1->getVerticesCount() > mesh2->getVerticesCount());
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

