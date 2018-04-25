//
// Created by liao xiangsen on 2018/4/24.
//

#include "sampler/sampler.h"

#include <cmath>

#include "utils.h"

namespace monitor {
  void Sampler2D::generateSamples(std::shared_ptr<std::vector<TVec2d>> samples, double minDis, int newPointsCount) {
    double cellSize = minDis / std::sqrt(2);
    int xCellCount = (int)(width / cellSize), yCellCount = (int)(length / cellSize);
    SamplerCell cells(xCellCount, yCellCount, minDis, cellSize, samples);
    std::vector<TVec2d> processList;
    processList.reserve(xCellCount * yCellCount);

    update_random_seed();
    TVec2d curPoint(RandomDouble(xStart, xEnd), RandomDouble(yStart, yEnd)), newPoint;
    cells.putSample(curPoint);
    processList.push_back(curPoint);

    int randomIndex;
    while(!processList.empty()) {
      randomIndex = RandomInt(0, (int)processList.size() - 1);
      curPoint = processList[randomIndex];
      processList[randomIndex] = processList.back();
      processList.pop_back();
      for(int i = 0; i < newPointsCount; i++) {
        newPoint = generateRandomPointAround(curPoint, minDis);
        if(newPoint.x > xStart && newPoint.x < xEnd && newPoint.y > yStart && newPoint.y < yEnd
            && cells.putSample(newPoint)) {
          processList.push_back(newPoint);
        }
      }
    }
  }

  SamplerCell::SamplerCell(int _x, int _y, double _minDis, double _size, std::shared_ptr<std::vector<TVec2d>> _samples) {
    xCount = _x, yCount = _y, minDisSquare = _minDis * _minDis, cellSize = _size, samples = _samples;
    used = new bool[xCount * yCount]();
    samplesIndex = new int[xCount * yCount]();
  }

  SamplerCell::~SamplerCell() {
    delete []used;
    delete []samplesIndex;
  }

  bool SamplerCell::putSample(const TVec2d &sample) {
    int xIndex = (int)(sample.x / cellSize), yIndex = (int)(sample.y / cellSize);
    if(checkNearCell(xIndex, yIndex, sample)) {
//      std::cout << "Put Sample False: " << sample << " index: " << xIndex << " " << yIndex << std::endl;
      return false;
    }
    int arrOffset = offset(xIndex, yIndex);
    used[arrOffset] = true;
    samples->emplace_back(sample.x, sample.y);
    samplesIndex[arrOffset] = (int)samples->size() - 1;
//    std::cout << "Put Sample True: " << sample << std::endl;
    return true;
  }

  bool SamplerCell::checkNearCell(int xIndex, int yIndex, const TVec2d &sample) {
    if(xIndex >= xCount || yIndex >= yCount)
      return true;
    int xBegin = Clamp(xIndex - 2, 0, xCount - 1);
    int xEnd = Clamp(xIndex + 2, 0, xCount - 1);
    int yBegin = Clamp(yIndex - 2, 0, yCount - 1);
    int yEnd = Clamp(yIndex + 2, 0, yCount - 1);
    for(int i = xBegin; i <= xEnd; i++) {
      for(int j = yBegin; j <= yEnd; j++) {
        int arrOffset = offset(i, j);
        if(used[arrOffset] && pointDisSquare(sample, samples->at(samplesIndex[arrOffset])) < minDisSquare)
          return true;
      }
    }
    return false;
  }

  // http://devmag.org.za/img/articles/Poisson_Disk_Sampling-figure%202.jpg
  TVec2d generateRandomPointAround(const TVec2d &point, double minDis) {
    double radius = minDis * RandomDouble(1.0, 2.0), angle = RandomDouble(0.0, 6.28);
    TVec2d dst(point.x + radius * std::cos(angle), point.y + radius * std::sin(angle));
    return dst;
  }
}

