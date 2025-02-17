#include "location.h"
#include <tuple>

using namespace std;

namespace Location {
const double PI = 3.1415926535;

double ConvertDegreesToRadians(double degrees) {
  return degrees * PI / 180.0;
}

Point Point::FromDegrees(double latitude, double longitude) {
  return {
    ConvertDegreesToRadians(latitude),
    ConvertDegreesToRadians(longitude)
  };
}

const double EARTH_RADIUS = 6'371'000;

double Distance(Point lhs, Point rhs) {
  lhs = Point::FromDegrees(lhs.latitude, lhs.longitude);
  rhs = Point::FromDegrees(rhs.latitude, rhs.longitude);
  return acos(
    sin(lhs.latitude) * sin(rhs.latitude)
    + cos(lhs.latitude) * cos(rhs.latitude) * cos(abs(lhs.longitude - rhs.longitude))
  ) * EARTH_RADIUS;
}

bool operator<(const Point &lhs, const Point &rhs) {
  return pow(lhs.latitude, 2) + pow(lhs.longitude, 2) < pow(rhs.latitude, 2) + pow(rhs.longitude, 2);
}

bool operator==(const Point &lhs, const Point &rhs) {
  return tie(lhs.longitude, lhs.latitude) == tie(rhs.longitude, rhs.latitude);
}

bool operator!=(const Point &lhs, const Point &rhs) {
  return !(lhs == rhs);
}

bool AreSharingCoordinate(const Point &lhs, const Point &rhs) {
  return lhs.longitude == rhs.longitude || lhs.latitude == rhs.latitude;
}

bool AreSharingCoordinate(const Point &lhs, const Point &rhs, const std::string &axis) {
  if (axis == "longitude") {
    return lhs.longitude == rhs.longitude;
  } else if (axis == "latitude") {
    return lhs.latitude == rhs.latitude;
  } else {
    throw runtime_error("unknown axis");
  }
}
}
