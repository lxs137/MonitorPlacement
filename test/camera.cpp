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
#include "camera_chooser.h"
#include "sampler/sampler.h"

namespace {
  double resolution[3] = {0.5, 0.5, 0.5};
  double cameraMinDis = resolution[0] * 20;
  double cameraHeight = 6;
  std::shared_ptr<monitor::Grids> gridsPtr = nullptr;

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

      const std::vector<const citygml::CityObject*> targets = city->getAllCityObjectsOfType(citygml::CityObject::CityObjectsType::COT_Door);
      monitor::Mesh targetMesh = monitor::parseMeshFromCityObjects(targets);
      targetMesh.voxelizer(targetVoxels, resolution);

      worldGrids->removeVoxels(targetVoxels);

//      std::shared_ptr<monitor::Mesh> voxelMesh = worldGrids->gridsToMesh();
//      voxelMesh->writeToFile("./data/city_voxel.obj", "Green");

    }
    static void TearDownTestCase() {
      road.reset();
      city.reset();
      worldGrids.reset();
      targetVoxels.clear();
      targetVoxels.shrink_to_fit();
    }
  protected:
    virtual void SetUp() {
      const citygml::Envelope &envelope = CameraMonitorTest::road->getEnvelope();
      const TVec3d &lowerPoint = envelope.getLowerBound(), &upperPoint = envelope.getUpperBound();
//      monitor::Sampler2D sampler2D((int)lowerPoint.x, (int)upperPoint.x, (int)lowerPoint.y, (int)upperPoint.y);
      monitor::Sampler2D sampler2D(0, (int)(upperPoint.x - lowerPoint.x), 0, (int)(upperPoint.y - lowerPoint.y));
      std::shared_ptr<std::vector<TVec2d>> samples = std::make_shared<std::vector<TVec2d>>();
      sampler2D.generateSamples(samples, cameraMinDis);
      for(auto it = samples->begin(); it != samples->end(); it++) {
        it->x += lowerPoint.x;
        it->y += lowerPoint.y;
      }

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

      std::vector<monitor::Camera> allCameras;
      allCameras.reserve(samples->size());
      for(auto it = samples->begin(); it != samples->end(); it++) {
        TVec3d cameraPos(it->x, it->y, cameraHeight);
        allCameras.emplace_back(CameraMonitorTest::worldGrids, cameraPos, 0.0, -20.0);
      }
      monitor::Mesh allCamerasMesh;
      for(auto it = allCameras.begin(); it != allCameras.end(); it++) {
        std::shared_ptr<monitor::Mesh> itMesh = CameraMonitorTest::worldGrids->voxelToMesh(it->getVoxel());
        allCamerasMesh.merge(*itMesh);
      }
      allCamerasMesh.writeToFile("./data/camera_all.obj", "Blue");

    }
    virtual void TearDown() {
      validSamples.clear();
      validSamples.shrink_to_fit();
    }
    static std::shared_ptr<const citygml::CityModel> road;
    static std::shared_ptr<const citygml::CityModel> city;
    static std::shared_ptr<monitor::Grids> worldGrids;
    static std::vector<monitor::Voxel> targetVoxels;
    std::vector<TVec2d> validSamples;
  };

  std::shared_ptr<const citygml::CityModel> CameraMonitorTest::road = nullptr;
  std::shared_ptr<const citygml::CityModel> CameraMonitorTest::city = nullptr;
  std::shared_ptr<monitor::Grids> CameraMonitorTest::worldGrids = nullptr;
  std::vector<monitor::Voxel> CameraMonitorTest::targetVoxels;

  TEST_F(CameraMonitorTest, INTERSECT_TEST) {
    monitor::Voxel src(-6,125,10), target(160, 209, 3);
    gridsPtr->intersect(src, target);
  }

  TEST_F(CameraMonitorTest, CALCULATE_VIEW) {
    EXPECT_GT(validSamples.size(), 0);

    std::vector<monitor::Camera> cameras;
    cameras.reserve(validSamples.size());
    for(auto it = validSamples.begin(); it != validSamples.end(); it++) {
      TVec3d cameraPos(it->x, it->y, cameraHeight);
      cameras.emplace_back(worldGrids, cameraPos, 0.0, -20.0);
    }

    size_t targetCount = targetVoxels.size(), cameraCount = cameras.size();
    // Allocate Memory
    bool *data = new bool[targetCount * cameraCount]();
    bool **view = new bool*[targetCount];
    for(size_t i = 0; i < targetCount; i++)
      view[i] = &data[i * cameraCount];

    size_t *countPerCamera = new size_t[cameraCount]();
    size_t *countPerTarget = new size_t[targetCount]();

    for(size_t i = 0; i < cameraCount; i++) {
      cameras[i].findBestPhiH(targetVoxels);
    }

    for(size_t i = 0; i < targetCount; i++) {
      for(size_t j = 0; j < cameraCount; j++) {
        view[i][j] = cameras[j].canMonitor(targetVoxels[i]);
        if(view[i][j]) {
          countPerCamera[j]++;
          countPerTarget[i]++;
        }
      }
    }

    for(size_t i = 0; i < cameraCount; i++) {
      double rate = (double)countPerCamera[i] / targetCount;
      std::cout << "Target View Rate(" << cameras[i] << "): " << rate << std::endl;
    }

    size_t notMonitored = 0;
    for(size_t i = 0; i < targetCount; i++) {
      if(countPerTarget[i] == 0)
        notMonitored++;
    }
    std::cout << "Not Be Monitored: " << notMonitored << std::endl;
    std::cout << "TargetCount: " << targetCount << " CameraCount: " << cameraCount << std::endl;

    std::vector<monitor::Camera> preChooseCameras;
    monitor::preChooseGreedy(cameras, preChooseCameras, view, targetCount, 15);

    for(auto it = preChooseCameras.begin(); it != preChooseCameras.end(); it++) {
      std::cout << *it << std::endl;
    }
    monitor::Mesh cameraMesh;
    for(auto it = cameras.begin(); it != cameras.end(); it++) {
      std::shared_ptr<monitor::Mesh> itMesh = worldGrids->voxelToMesh(it->getVoxel());
      cameraMesh.merge(*itMesh);
    }
    cameraMesh.writeToFile("./data/camera.obj", "Red");

    monitor::Mesh targetsMesh;
    for(auto it = targetVoxels.begin(); it != targetVoxels.end(); it++) {
      std::shared_ptr<monitor::Mesh> itMesh = worldGrids->voxelToMesh(*it);
      targetsMesh.merge(*itMesh);
    }
    targetsMesh.writeToFile("./data/target.obj", "Blue");

    delete[] countPerCamera;
    delete[] countPerTarget;
    // Free Memory
    delete[] view[0];
    delete[] view;
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  if(argc > 1) {
    ::testing::GTEST_FLAG(filter) = argv[1];
  } else {
//    ::testing::GTEST_FLAG(filter) = "*INTERSECT_TEST*";
    ::testing::GTEST_FLAG(filter) = "*CALCULATE_VIEW*";
  }
  return RUN_ALL_TESTS();
}
