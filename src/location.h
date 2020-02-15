#pragma once

#include <cmath>
#include <string>

namespace Location {
  double ConvertDegreesToRadians(double degrees);

  struct Point {
    double latitude;
    double longitude;

    static Point FromDegrees(double latitude, double longitude);
  };

  double Distance(Point lhs, Point rhs);

  bool AreSharingCoordinate(const Point &lhs, const Point &rhs);

  bool AreSharingCoordinate(const Point &lhs, const Point &rhs, const std::string &axis);

  bool operator<(const Point &lhs, const Point &rhs);
  bool operator==(const Point &lhs, const Point &rhs);
  bool operator!=(const Point &lhs, const Point &rhs);
}
