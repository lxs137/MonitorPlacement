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
#include <gtest/gtest.h>
#include "world.h"
#include "print.h"
#include "voxelizer/mesh.h"

namespace {
  double resolution[3] = {0.5, 0.5, 0.5};
  class CityObjectParserTest : public ::testing::Test {
  public:
    static void SetUpTestCase() {
      citygml::ParserParams params;
      city = citygml::load("./data/LOD3_1.gml", params);
      road = citygml::load("./data/Road-LOD0.gml", params);
      std::cout << "CityModel: " << city->getEnvelope() << std::endl;
    }
    static void TearDownTestCase() {
      city.reset();
      road.reset();
    }
  protected:
    virtual void SetUp() {
      parserResult = CityObjectParserTest::road->getAllCityObjectsOfType(
          citygml::CityObject::CityObjectsType::COT_Road);
//      parserResult = CityObjectParserTest::road->getAllCityObjectsOfType(
//          citygml::CityObject::CityObjectsType::COT_Door);
    };

    virtual void TearDown() {
      parserResult.clear();
      parserResult.shrink_to_fit();
    };
    static std::shared_ptr<const citygml::CityModel> city;
    static std::shared_ptr<const citygml::CityModel> road;
    std::vector<const citygml::CityObject*> parserResult;

  };

  std::shared_ptr<const citygml::CityModel> CityObjectParserTest::city = nullptr;
  std::shared_ptr<const citygml::CityModel> CityObjectParserTest::road = nullptr;

  TEST_F(CityObjectParserTest, VOXELIZER_TEST) {
    const citygml::CityObject* object = parserResult.front();
    monitor::Mesh mesh = monitor::parseMeshFromCityObject(object);
    std::vector<monitor::Voxel> voxels;
    mesh.voxelizer(voxels, resolution);
    monitor::printCityObject(object);
    std::cout << mesh << std::endl;
    for(auto it = voxels.begin(); it != voxels.end(); it++) {
      std::cout << *it << std::endl;
//      std::cout << it->x * resolution << " " << it->y * resolution << " " << it->z * resolution << std::endl;
    }
    EXPECT_EQ(object->getGeometry(0).getPolygon(0)->getVertices().size(), mesh.getVerticesCount() + 1);
  }

  TEST_F(CityObjectParserTest, MESH_MERGE_TEST) {
    const citygml::CityObject* object1 = parserResult[0];
    const citygml::CityObject* object2 = parserResult[1];
    monitor::Mesh mesh1 = monitor::parseMeshFromCityObject(object1);
    monitor::Mesh mesh2 = monitor::parseMeshFromCityObject(object2);
    int nVertices1 = mesh1.getVerticesCount(), nVertices2 = mesh2.getVerticesCount();
    std::cout << "Mesh1: \n" << mesh1 << std::endl;
    std::cout << "Mesh2: \n" << mesh2 << std::endl;
    mesh1.merge(mesh2);
    std::cout << "Mesh1: \n" << mesh1 << std::endl;
    EXPECT_EQ(mesh1.getVerticesCount(), nVertices1 + nVertices2);
  }

  TEST_F(CityObjectParserTest, SET_VOXELIZER_WORLD) {
    const citygml::CityObject *object1 = parserResult.front(), *object2 = parserResult.back();
    monitor::Mesh mesh1 = monitor::parseMeshFromCityObject(object1),
        mesh2 = monitor::parseMeshFromCityObject(object2), mesh3(mesh1);
    mesh3.merge(mesh2);
    std::shared_ptr<monitor::Grids> world = monitor::cityModelToGrids(CityObjectParserTest::city, resolution);
    std::vector<monitor::Voxel> voxels1, voxels2, voxels3;
    mesh1.voxelizer(voxels1, resolution);
    mesh2.voxelizer(voxels2, resolution);
    mesh3.voxelizer(voxels3, resolution);
    std::cout << voxels1.size() << " " << voxels2.size() << " " << voxels3.size() << std::endl;
    world->addVoxels(voxels1);
    world->addVoxels(voxels2);
    std::cout << *world << std::endl;
    world->clear();
    world->addVoxels(voxels3);
    std::cout<< *world << std::endl;
    EXPECT_EQ(voxels1.size() + voxels2.size(), voxels3.size());
  }

  TEST_F(CityObjectParserTest, PARSE_ROAD) {
    EXPECT_GT(parserResult.size(), 0);
    monitor::Mesh mesh = monitor::parseMeshFromCityObjects(parserResult);
    mesh.writeToFile("./data/Road.obj", "White");
//    std::vector<monitor::Voxel> voxels;
//    mesh.voxelizer(voxels, resolution);
//    monitor::Grids world = monitor::cityModelToGrids(CityObjectParserTest::road, resolution);
//    world.addVoxels(voxels);
//    std::cout << world << std::endl;
  }

  TEST_F(CityObjectParserTest, PARSE_ROOF) {
    std::vector<const citygml::CityObject*> roofs = CityObjectParserTest::city->getAllCityObjectsOfType(
        citygml::CityObject::CityObjectsType::COT_RoofSurface);
    monitor::Mesh roofMesh = monitor::parseMeshFromCityObjects(roofs);
    roofMesh.writeToFile("./data/city_roof.obj", "Roof");
  }

  TEST_F(CityObjectParserTest, PARSE_WALL) {
    std::vector<const citygml::CityObject*> walls = CityObjectParserTest::city->getAllCityObjectsOfType(
        citygml::CityObject::CityObjectsType::COT_WallSurface);
    std::vector<const citygml::CityObject*> grouds = CityObjectParserTest::city->getAllCityObjectsOfType(
        citygml::CityObject::CityObjectsType::COT_GroundSurface);
    walls.insert(walls.end(), grouds.begin(), grouds.end());
    monitor::Mesh roofMesh = monitor::parseMeshFromCityObjects(walls);
    roofMesh.writeToFile("./data/city_wall.obj", "Wall");
  }

  TEST_F(CityObjectParserTest, PARSE_WINDOW) {
    std::vector<const citygml::CityObject*> windows = CityObjectParserTest::city->getAllCityObjectsOfType(
        citygml::CityObject::CityObjectsType::COT_Window);
    monitor::Mesh windowMesh = monitor::parseMeshFromCityObjects(windows);
    windowMesh.writeToFile("./data/city_window.obj", "Window");
  }

  TEST_F(CityObjectParserTest, TO_VOXEL_MESH) {
    const std::vector<const citygml::CityObject*> roots = CityObjectParserTest::city->getRootCityObjects();
    monitor::Mesh cityMesh;
    std::cout << "Count: " << roots.size() << std::endl;
    for(size_t i = 0; i < roots.size(); i++) {
      cityMesh.merge(monitor::parseMeshFromCityObjectRecursive(roots[i]));
    }
    cityMesh.writeToFile("./data/city.obj");
//    std::shared_ptr<monitor::Mesh> newMesh = cityMesh.voxelizerToMesh(resolution);
//    newMesh->writeToFile("./data/city_voxel.obj");
  }

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  if(argc > 1) {
    ::testing::GTEST_FLAG(filter) = argv[1];
  } else {
   ::testing::GTEST_FLAG(filter) = "*PARSE_WINDOW*";
    // ::testing::GTEST_FLAG(filter) = "*TO_VOXEL_MESH*";
  }
  return RUN_ALL_TESTS();
}

