#pragma once

#include <cmath>

namespace Location {
  double ConvertDegreesToRadians(double degrees);

  struct Point {
    double latitude;
    double longitude;

    static Point FromDegrees(double latitude, double longitude);
  };

  double Distance(Point lhs, Point rhs);
}
