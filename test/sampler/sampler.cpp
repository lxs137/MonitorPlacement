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
#include "utils.h"

namespace {
  double resolution[3] = {0.5, 0.5, 0.5};
  double minDis = resolution[0] * 20;
  class DiskSamplerTest : public ::testing::Test {
  public:
    static void SetUpTestCase() {
      citygml::ParserParams params;
      city = citygml::load("./data/Road-LOD0.gml", params);
    }
    static void TearDownTestCase() {
      city.reset();
    }
  protected:
    virtual void SetUp() {
      const citygml::Envelope &envelope = DiskSamplerTest::city->getEnvelope();
      const TVec3d &lowerPoint = envelope.getLowerBound(), &upperPoint = envelope.getUpperBound();
      monitor::Sampler2D sampler2D((int)lowerPoint.x, (int)upperPoint.x, (int)lowerPoint.y, (int)upperPoint.y);
      samples = std::make_shared<std::vector<TVec2d>>();
      sampler2D.generateSamples(samples, minDis);
    }
    virtual void TearDown() {

    }

    static std::shared_ptr<const citygml::CityModel> city;
    std::shared_ptr<std::vector<TVec2d>> samples;
  };

  std::shared_ptr<const citygml::CityModel> DiskSamplerTest::city = nullptr;

  TEST_F(DiskSamplerTest, SAMPLES) {

    for(auto it = samples->begin(); it != samples->end(); it++) {
      std::cout << *it << std::endl;
    }
  }

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
