//
// Created by liao xiangsen on 2018/4/25.
//

#include "sampler/sampler.h"

#include <memory>
#include <vector>

#include <citygml/citygml.h>
#include <citygml/citymodel.h>
#include <citygml/vecs.hpp>
#include <gtest/gtest.h>
#include "world.h"
#include "parser.h"
#include "utils.h"

namespace {
  double resolution[3] = {0.5, 0.5, 0.5};
  double cameraMinDis = resolution[0] * 20;
  class DiskSamplerTest : public ::testing::Test {
  public:
    static void SetUpTestCase() {
      citygml::ParserParams params;
      road = citygml::load("./data/Road-LOD0.gml", params);
    }
    static void TearDownTestCase() {
      road.reset();
    }
  protected:
    virtual void SetUp() {
      const citygml::Envelope &envelope = DiskSamplerTest::road->getEnvelope();
      const TVec3d &lowerPoint = envelope.getLowerBound(), &upperPoint = envelope.getUpperBound();
      monitor::Sampler2D sampler2D((int)lowerPoint.x, (int)upperPoint.x, (int)lowerPoint.y, (int)upperPoint.y);
      samples = std::make_shared<std::vector<TVec2d>>();
      sampler2D.generateSamples(samples, cameraMinDis);
    }
    virtual void TearDown() {

    }

    static std::shared_ptr<const citygml::CityModel> road;
    std::shared_ptr<std::vector<TVec2d>> samples;
  };

  std::shared_ptr<const citygml::CityModel> DiskSamplerTest::road = nullptr;

  TEST_F(DiskSamplerTest, SAMPLES) {
    std::cout << "Samplers Count: " << samples->size() << std::endl;
    EXPECT_GT(samples->size(), 0);
  }

  TEST_F(DiskSamplerTest, SAMPLES_CUT) {
    std::cout << "Samplers Count: " << samples->size() << std::endl;
    EXPECT_GT(samples->size(), 0);

    std::vector<const citygml::CityObject*> roads = DiskSamplerTest::road->getAllCityObjectsOfType(citygml::CityObject::CityObjectsType::COT_Road);
    monitor::Mesh mesh = monitor::parseMeshFromCityObjects(roads);
    std::vector<monitor::Voxel> voxels;
    mesh.voxelizer(voxels, resolution);
    std::shared_ptr<monitor::Grids> grids = monitor::cityModelToGrids(DiskSamplerTest::road, resolution);
    grids->addVoxels(voxels);


    std::vector<TVec2d> validSamples;
    validSamples.reserve(samples->size());
    for(auto it = samples->begin(); it != samples->end(); it++) {
      TVec3d p(it->x, it->y, 0.01);
      monitor::Voxel voxel = grids->posToVoxel(p);
      if(grids->exist(voxel)) {
        validSamples.push_back(*it);
      }
    }

    std::cout << "Valid Samplers Count: " << validSamples.size() << std::endl;
    EXPECT_GT(validSamples.size(), 0);
    for(auto it = validSamples.begin(); it != validSamples.end(); it++) {
      std::cout << *it << std::endl;
    }
  }

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  if(argc > 1) {
    ::testing::GTEST_FLAG(filter) = argv[1];
  } else {
    ::testing::GTEST_FLAG(filter) = "*SAMPLES_CUT*";
  }
  return RUN_ALL_TESTS();
}
