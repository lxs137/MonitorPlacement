//
// Created by liao xiangsen on 2018/4/24.
//

#ifndef SAMPLER_SAMPLER2D_H
#define SAMPLER_SAMPLER2D_H

#include <vector>
#include <memory>

#include <citygml/vecs.hpp>

namespace monitor {
  class Sampler2D {
  public:
    Sampler2D(int x_start, int x_end, int y_start, int y_end): xStart(x_start), xEnd(x_end), yStart(y_start), yEnd(y_end) {
      width = xEnd - xStart;
      length = yEnd - yStart;
    }
    // Poisson Disk Sampler
    // http://devmag.org.za/2009/05/03/poisson-disk-sampling/
    void generateSamples(std::shared_ptr<std::vector<TVec2d>> samples, double minDis, int newPointsCount = 30);
  private:
    // Samples Range
    int xStart, xEnd, yStart, yEnd;
    int width, length;
  };

  class SamplerCell {
  public:
    SamplerCell(int _x, int _y, double _minDis, double _size, std::shared_ptr<std::vector<TVec2d>> _samples);
    bool putSample(const TVec2d &sample);
    bool checkNearCell(int xIndex, int yIndex, const TVec2d &sample);
    ~SamplerCell();
  private:
    inline int offset(int xIndex, int yIndex) {
      return yIndex * xCount + xIndex;
    }
    int xCount, yCount;
    double cellSize;
    double minDisSquare;
    bool *used;
    int *samplesIndex;
    std::shared_ptr<std::vector<TVec2d>> samples;
  };

  inline double pointDisSquare(const TVec2d &p1, const TVec2d &p2) {
    return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
  };
  TVec2d generateRandomPointAround(const TVec2d &point, double minDis);
}

#endif //SAMPLER_SAMPLER2D_H
