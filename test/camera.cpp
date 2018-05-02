//
// Created by liao xiangsen on 2018/5/1.
//

#include "camera.h"

#include <memory>
#include <array>

#include <citygml/citymodel.h>
#include <citygml/citygml.h>
#include <gtest/gtest.h>
#include "world.h"
#include "parser.h"
#include "sampler/sampler.h"

namespace {
  double resolution[3] = {0.5, 0.5, 0.5};
  double cameraMinDis = resolution[0] * 25;
  double cameraHeight = 5;

  class CameraMonitorTest : public ::testing::Test {
  public:
    static void SetUpTestCase() {
      citygml::ParserParams params;
      road = citygml::load("./data/Road-LOD0.gml", params);
      city = citygml::load("./data/LOD3_1.gml", params);

      const std::vector<const citygml::CityObject*> roots = city->getRootCityObjects();
      monitor::Mesh cityMesh;
      for(size_t i = 0; i < roots.size(); i++) {
        cityMesh.merge(monitor::parseMeshFromCityObjectRecursive(roots[i]));
      }
      std::vector<monitor::Voxel> cityVoxels;
      cityMesh.voxelizer(cityVoxels, resolution);
      worldGrids = monitor::cityModelToGrids(CameraMonitorTest::city, resolution);
      worldGrids->addVoxels(cityVoxels);
      std::cout << "CityGrids: " << *worldGrids << std::endl;
    }
    static void TearDownTestCase() {
      road.reset();
      city.reset();
      worldGrids.reset();
    }
  protected:
    virtual void SetUp() {
      const citygml::Envelope &envelope = CameraMonitorTest::road->getEnvelope();
      const TVec3d &lowerPoint = envelope.getLowerBound(), &upperPoint = envelope.getUpperBound();
      monitor::Sampler2D sampler2D((int)lowerPoint.x, (int)upperPoint.x, (int)lowerPoint.y, (int)upperPoint.y);
      std::shared_ptr<std::vector<TVec2d>> samples = std::make_shared<std::vector<TVec2d>>();
      sampler2D.generateSamples(samples, cameraMinDis);

      std::cout << "Samplers Count: " << samples->size() << std::endl;
      std::vector<const citygml::CityObject*> roads = CameraMonitorTest::road->getAllCityObjectsOfType(citygml::CityObject::CityObjectsType::COT_Road);
      monitor::Mesh roadMesh = monitor::parseMeshFromCityObjects(roads);
      std::vector<monitor::Voxel> roadVoxels;
      roadMesh.voxelizer(roadVoxels, resolution);
      std::shared_ptr<monitor::Grids> grids = monitor::cityModelToGrids(CameraMonitorTest::road, resolution);
      grids->addVoxels(roadVoxels);

      validSamples.reserve(samples->size());
      for(auto it = samples->begin(); it != samples->end(); it++) {
        TVec3d p(it->x, it->y, 0.01);
        monitor::Voxel voxel = grids->posToVoxel(p);
        if(grids->exist(voxel)) {
          validSamples.push_back(*it);
        }
      }
      std::cout << "Valid Samplers Count: " << validSamples.size() << std::endl;

      const std::vector<const citygml::CityObject*> targets = CameraMonitorTest::city->getAllCityObjectsOfType(citygml::CityObject::CityObjectsType::COT_Door);
      monitor::Mesh targetMesh = monitor::parseMeshFromCityObjects(targets);
      targetMesh.voxelizer(targetVoxels, resolution);
      std::cout << "TargetVoxels:" << targetVoxels.size() << std::endl;

    }
    virtual void TearDown() {
      validSamples.clear();
      validSamples.shrink_to_fit();
      targetVoxels.clear();
      targetVoxels.shrink_to_fit();
    }
    static std::shared_ptr<const citygml::CityModel> road;
    static std::shared_ptr<const citygml::CityModel> city;
    static std::shared_ptr<monitor::Grids> worldGrids;
    std::vector<monitor::Voxel> targetVoxels;
    std::vector<TVec2d> validSamples;
  };

  std::shared_ptr<const citygml::CityModel> CameraMonitorTest::road = nullptr;
  std::shared_ptr<const citygml::CityModel> CameraMonitorTest::city = nullptr;
  std::shared_ptr<monitor::Grids> CameraMonitorTest::worldGrids = nullptr;

  TEST_F(CameraMonitorTest, INTEGRATION) {
    EXPECT_GT(validSamples.size(), 0);

    std::vector<monitor::Camera> cameras;
    cameras.reserve(validSamples.size());
    for(auto it = validSamples.begin(); it != validSamples.end(); it++) {
      TVec3d cameraPos(it->x, it->y, cameraHeight);
      cameras.emplace_back(worldGrids, cameraPos, 0.0, 0.0);
    }

    size_t targetCount = targetVoxels.size(), cameraCount = cameras.size();
    // Allocate Memory
    bool **view = new bool*[targetCount];
    for(size_t i = 0; i < targetCount; i++)
      view[i] = new bool[cameraCount]();

    size_t *countArr = new size_t[cameraCount]();
    for(size_t i = 0; i < targetCount; i++) {
      for(size_t j = 0; j < cameraCount; j++) {
        view[i][j] = cameras[j].canMonitor(targetVoxels[i]);
        if(view[i][j]) {
          countArr[j]++;
//          std::cout << "("<< cameras[j] << ") (" << targetVoxels[i] << ")" <<std::endl;
        }
      }
    }

    for(size_t i = 0; i < cameraCount; i++) {
      std::cout << "View Rate: " << std::fixed << (double)countArr[i] / targetCount << std::endl;
    }

//    std::cout << "TargetCount: " << targetCount << " CameraCount: " << cameraCount
//              << " ViewCount: " << count << std::endl;

    delete[] countArr;
    // Free Memory
    for(size_t i = 0; i < targetCount; i++)
      delete[] view[i];
    delete[] view;
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  if(argc > 1) {
    ::testing::GTEST_FLAG(filter) = argv[1];
  } else {
    ::testing::GTEST_FLAG(filter) = "*INTEGRATION*";
  }
  return RUN_ALL_TESTS();
}
