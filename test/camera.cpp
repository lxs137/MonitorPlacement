//
// Created by liao xiangsen on 2018/5/1.
//

#include "camera.h"

#include <memory>
#include <array>
#include <fstream>

#include <citygml/citymodel.h>
#include <citygml/citygml.h>
#include <gtest/gtest.h>
#include "world.h"
#include "parser.h"
#include "camera_chooser.h"
#include "sampler/sampler.h"

#define OUTPUT_OBJ
//#define OUTPUT_RATE
// #define OUTPUT_POS

namespace {
  double resolution[3] = {0.5, 0.5, 0.5};
  double cameraMinDis = resolution[0] * 20;
  double cameraHeight = 10;
  const double CameraPhiV = 0.0;
  int GreedyFindCameraCount = 20;
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
#ifdef OUTPUT_OBJ
      monitor::Mesh targetsMesh;
      for(auto it = targetVoxels.begin(); it != targetVoxels.end(); it++) {
        std::shared_ptr<monitor::Mesh> itMesh = worldGrids->voxelToMesh(*it);
        targetsMesh.merge(*itMesh);
      }
      targetsMesh.writeToFile("./data/target.obj", "Blue");
#endif
//#ifdef OUTPUT_OBJ
//      std::shared_ptr<monitor::Mesh> cityExcludeTarget = worldGrids->gridsToMesh();
//      cityExcludeTarget->writeToFile("./data/city_exclude_target.obj", "Green");
//#endif

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

#ifdef OUTPUT_OBJ
      std::vector<monitor::Camera> allCameras;
      allCameras.reserve(validSamples.size());
      for(auto it = validSamples.begin(); it != validSamples.end(); it++) {
        TVec3d cameraPos(it->x, it->y, cameraHeight);
        allCameras.emplace_back(CameraMonitorTest::worldGrids, cameraPos, 0.0, CameraPhiV);
      }
      monitor::Mesh allCamerasMesh;
      for(auto it = allCameras.begin(); it != allCameras.end(); it++) {
        std::shared_ptr<monitor::Mesh> itMesh = CameraMonitorTest::worldGrids->voxelToMesh(it->getPos());
        allCamerasMesh.merge(*itMesh);
      }
      allCamerasMesh.writeToFile("./data/camera_valid.obj", "Blue");
#endif
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

  TEST_F(CameraMonitorTest, GREEDY_BEFORE_LOCAL_SEARCH) {
    EXPECT_GT(validSamples.size(), 0);

    std::vector<monitor::Camera> cameras;
    cameras.reserve(validSamples.size());
    for(auto it = validSamples.begin(); it != validSamples.end(); it++) {
      TVec3d cameraPos(it->x, it->y, cameraHeight);
      cameras.emplace_back(worldGrids, cameraPos, 0.0, CameraPhiV);
    }

    size_t targetCount = targetVoxels.size(), cameraCount = cameras.size();
    // Allocate Memory
    bool *data = new bool[targetCount * cameraCount]();
    bool **view = new bool*[targetCount];
    for(size_t i = 0; i < targetCount; i++)
      view[i] = &data[i * cameraCount];

    size_t *countPerCamera = new size_t[cameraCount]();
    size_t *countPerTarget = new size_t[targetCount]();

//    for(size_t i = 0; i < cameraCount; i++) {
//      cameras[i].findBestPhiH(targetVoxels);
//    }

    for(size_t i = 0; i < targetCount; i++) {
      for(size_t j = 0; j < cameraCount; j++) {
        view[i][j] = cameras[j].canMonitor(targetVoxels[i]);
        if(view[i][j]) {
          countPerCamera[j]++;
          countPerTarget[i]++;
        }
      }
    }

//    for(size_t i = 0; i < targetCount; i++) {
//      std::cout << "Camera View Rate(" << targetVoxels[i] << "): " << countPerTarget[i] << std::endl;
//    }

    for(size_t i = 0; i < cameraCount; i++) {
      double rate = (double)countPerCamera[i] / targetCount;
      std::cout << "Target View Rate(" << cameras[i] << "): " << rate << std::endl;
    }

    size_t notMonitored = 0;
    for(size_t i = 0; i < targetCount; i++) {
      if(countPerTarget[i] == 0)
        notMonitored++;
    }
    std::cout << "Coverage rate: " << monitor::evalTargetCoverage(view, targetCount, cameraCount) << std::endl;

    // Greedy Pre Choose
    std::vector<int> preChosenCamerasIndex;
    monitor::preChooseGreedy(cameras, preChosenCamerasIndex, view, targetCount, GreedyFindCameraCount);
    EXPECT_GT(preChosenCamerasIndex.size(), 0);
    std::vector<monitor::Camera> preChosenCameras;
    for(auto it = preChosenCamerasIndex.begin(); it != preChosenCamerasIndex.end(); it++) {
      std::cout << cameras.at(*it) << std::endl;
      preChosenCameras.push_back(cameras.at(*it));
    }
    std::cout << "TargetCount: " << targetCount << " CameraCount: " << cameraCount
              << " ChooseCamera: " << preChosenCamerasIndex.size() << std::endl;
    std::cout << "Coverage rate: " << monitor::evalTargetCoverage(view, targetCount, preChosenCamerasIndex) << std::endl;

#ifdef OUTPUT_OBJ
    monitor::Mesh targetsMesh;
    for(auto it = targetVoxels.begin(); it != targetVoxels.end(); it++) {
      std::shared_ptr<monitor::Mesh> itMesh = worldGrids->voxelToMesh(*it);
      targetsMesh.merge(*itMesh);
    }
    targetsMesh.writeToFile("./data/target.obj", "Blue");
#endif
    // Regenerate View Matrix
    size_t cameraCountPreChosen = preChosenCameras.size();
    bool *dataPreChosen = new bool[targetCount * cameraCountPreChosen]();
    bool **viewPreChosen = new bool*[targetCount];
    for(size_t i = 0; i < targetCount; i++) {
      viewPreChosen[i] = &dataPreChosen[i * cameraCountPreChosen];
      for(size_t j = 0; j < cameraCountPreChosen; j++) {
        viewPreChosen[i][j] = view[i][preChosenCamerasIndex[j]];
//        std::cout << viewPreChosen[i][j] << " ";
      }
//      std::cout << std::endl;
    }
    std::cout << "Coverage rate: " << monitor::evalTargetCoverage(viewPreChosen, targetCount, cameraCountPreChosen) << std::endl;

    // Local Search
    std::vector<monitor::Camera> localSearchCameras;
    monitor::localSearch(preChosenCameras, targetVoxels, viewPreChosen, localSearchCameras);
    for(auto it = localSearchCameras.begin(); it != localSearchCameras.end(); it++) {
      std::cout << *it << std::endl;
    }

#ifdef OUTPUT_OBJ
    monitor::Mesh cameraMesh;
    for(auto it = localSearchCameras.begin(); it != localSearchCameras.end(); it++) {
      std::shared_ptr<monitor::Mesh> itMesh = worldGrids->voxelToMesh(it->getPos());
      cameraMesh.merge(*itMesh);
    }
    cameraMesh.writeToFile("./data/camera.obj", "Red");
    monitor::Mesh cameraFOVMesh;
    for(auto it = localSearchCameras.begin(); it != localSearchCameras.end(); it++) {
      std::shared_ptr<monitor::Mesh> itMesh = it->getViewFieldMesh();
      cameraFOVMesh.merge(*itMesh);
    }
    cameraFOVMesh.writeToFile("./data/camera_fov.obj", "Field");
//    std::shared_ptr<monitor::Mesh> cameraFovVoxelMesh =  cameraFOVMesh.voxelizerToMesh(resolution);
//    cameraFovVoxelMesh->writeToFile("./data/camera_fov.obj", "Field");
#endif

    delete[] countPerCamera;
    delete[] countPerTarget;
    // Free Memory
    delete[] view[0];
    delete[] view;
    delete[] viewPreChosen[0];
    delete[] viewPreChosen;
  }

  TEST_F(CameraMonitorTest, GREEDY_AFTER_LOCAL_SEARCH) {
    EXPECT_GT(validSamples.size(), 0);

    std::vector<monitor::Camera> cameras;
    cameras.reserve(validSamples.size());
    for(auto it = validSamples.begin(); it != validSamples.end(); it++) {
      TVec3d cameraPos(it->x, it->y, cameraHeight);
      cameras.emplace_back(worldGrids, cameraPos, 0.0, CameraPhiV);
    }

    size_t targetCount = targetVoxels.size(), cameraCount = cameras.size();
    // Allocate Memory
    bool *data = new bool[targetCount * cameraCount]();
    bool **view = new bool*[targetCount];
    for(size_t i = 0; i < targetCount; i++)
      view[i] = &data[i * cameraCount];

    for(size_t i = 0; i < targetCount; i++) {
      for(size_t j = 0; j < cameraCount; j++) {
        view[i][j] = cameras[j].canMonitor(targetVoxels[i]);
      }
    }
    std::cout << "Coverage rate: " << monitor::evalTargetCoverage(view, targetCount, cameraCount) << std::endl;

    // Local Search
    std::vector<monitor::Camera> localSearchCameras;
    monitor::localSearch(cameras, targetVoxels, view, localSearchCameras);
    std::cout << "After Local Search" << std::endl;
#ifdef OUTPUT_POS
    for(auto it = localSearchCameras.begin(); it != localSearchCameras.end(); it++) {
      std::cout << *it << std::endl;
    }
#endif

    bool *dataLocalSearch = new bool[targetCount * cameraCount]();
    bool **viewLocalSearch = new bool*[targetCount];
    for(size_t i = 0; i < targetCount; i++) {
      viewLocalSearch[i] = &dataLocalSearch[i * cameraCount];
      for(size_t j = 0; j < cameraCount; j++) {
        viewLocalSearch[i][j] = localSearchCameras[j].canMonitor(targetVoxels[i]);
      }
    }
    double localSearchCoverageRate = monitor::evalTargetCoverage(viewLocalSearch, targetCount, cameraCount);
    std::cout << "Coverage rate: " << localSearchCoverageRate << std::endl;

    // Greedy Choose
    std::vector<int> greedyChosenCamerasIndex;
    monitor::preChooseGreedy(localSearchCameras, greedyChosenCamerasIndex, viewLocalSearch, targetCount, GreedyFindCameraCount);
    EXPECT_GT(greedyChosenCamerasIndex.size(), 0);
    std::vector<monitor::Camera> chosenCameras;
    for(auto it = greedyChosenCamerasIndex.begin(); it != greedyChosenCamerasIndex.end(); it++) {
#ifdef OUTPUT_POS
      std::cout << localSearchCameras.at(*it) << std::endl;
#endif
      chosenCameras.push_back(localSearchCameras.at(*it));
    }
    std::cout << "TargetCount: " << targetCount << " CameraCount: " << cameraCount
              << " ChooseCamera: " << chosenCameras.size() << std::endl;
    double finalCoverageRate = monitor::evalTargetCoverage(viewLocalSearch, targetCount, greedyChosenCamerasIndex);
    std::cout << "Coverage rate: " << finalCoverageRate << std::endl;


#ifdef OUTPUT_RATE
    std::ofstream objFile;
    objFile.open("coverage-rate", std::ios::app);
    objFile << localSearchCoverageRate << "  " << finalCoverageRate << std::endl;
#endif

#ifdef OUTPUT_OBJ
    monitor::Mesh cameraMesh;
    for(auto it = chosenCameras.begin(); it != chosenCameras.end(); it++) {
      std::shared_ptr<monitor::Mesh> itMesh = worldGrids->voxelToMesh(it->getPos());
      cameraMesh.merge(*itMesh);
      std::cout << *it << std::endl;
    }
    cameraMesh.writeToFile("./data/camera.obj", "Camera");
    monitor::Mesh cameraFOVMesh;
    for(auto it = chosenCameras.begin(); it != chosenCameras.end(); it++) {
      std::shared_ptr<monitor::Mesh> itMesh = it->getViewFieldMesh();
      cameraFOVMesh.merge(*itMesh);
    }
    cameraFOVMesh.writeToFile("./data/camera_fov.obj", "Field");
#endif

#ifdef OUTPUT_OBJ
    monitor::Mesh targetsMonitorMesh;
    for(size_t i = 0; i < targetCount; i ++) {
      for(auto itIndex = greedyChosenCamerasIndex.begin(); itIndex != greedyChosenCamerasIndex.end(); itIndex++) {
        if(viewLocalSearch[i][*itIndex]) {
          std::shared_ptr<monitor::Mesh> itMesh = worldGrids->voxelToMesh(targetVoxels[i]);
          targetsMonitorMesh.merge(*itMesh);
          break;
        }
      }
    }
    targetsMonitorMesh.writeToFile("./data/target_monitor.obj", "Blue");
#endif

    // Free Memory
    delete[] view[0];
    delete[] view;
    delete[] viewLocalSearch[0];
    delete[] viewLocalSearch;
  }

  TEST_F(CameraMonitorTest, FOV) {
    worldGrids->clear();
    TVec3d pos(100.0, 50.0, 15.0);
    monitor::Camera camera(worldGrids, pos, GreedyFindCameraCount, -20);
    std::shared_ptr<monitor::Mesh> cameraMesh = camera.getViewFieldMesh();
    cameraMesh->writeToFile("./data/single_camera.obj", "Field");
    std::cout << *CameraMonitorTest::worldGrids << std::endl;
    monitor::Mesh targetsMonitorMesh;
    for(int y = 0; y < 250; y++) {
      for(int x = 0; x < 300; x++) {
        for(int z = 0; z < 48; z++) {
          monitor::Voxel voxel(x, y, z);
          if(camera.canMonitor(voxel)) {
            std::shared_ptr<monitor::Mesh> itMesh = worldGrids->voxelToMesh(voxel);
            targetsMonitorMesh.merge(*itMesh);
          }
        }
      }
      std::cout << "line " << y << std::endl;
    }
    targetsMonitorMesh.writeToFile("./data/target_monitor.obj", "Blue");
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::GTEST_FLAG(filter) = "*GREEDY_AFTER_LOCAL_SEARCH*";
//  ::testing::GTEST_FLAG(filter) = "*FOV*";
  GreedyFindCameraCount = atoi(argv[1]);
  std::cout << "ResCameraCount: " << GreedyFindCameraCount << std::endl;
  return RUN_ALL_TESTS();
}
