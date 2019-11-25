#include "map_projector.h"

using namespace std;

namespace Visualisation {
void MapProjector::Initialize(const TransportData::StopsDict &stops, double map_width,
                           double map_height, double map_padding) {
  padding_ = map_padding;
  double max_lon = std::numeric_limits<double>::min();
  double min_lat = std::numeric_limits<double>::max();
  for (const auto &[_, stop] : stops) {
  min_lat = min(min_lat, stop.position.latitude);
  min_lon_ = min(min_lon_, stop.position.longitude);
  max_lat_ = max(max_lat_, stop.position.latitude);
  max_lon = max(max_lon, stop.position.longitude);
}

if(min_lat == max_lat_ && min_lon_ == max_lon) {
  zoom_coef_ = 0;
} else if (min_lat == max_lat_) {
  zoom_coef_ = (map_width - 2 * padding_) / (max_lon - min_lon_);
} else if (min_lon_ == max_lon) {
  zoom_coef_ = (map_height - 2 * padding_) / (max_lat_ - min_lat);
} else {
  double width_zoom_coef = (map_width - 2 * padding_) / (max_lon - min_lon_);
  double height_zoom_coef = (map_height - 2 * padding_) / (max_lat_ - min_lat);
  zoom_coef_ = min(width_zoom_coef, height_zoom_coef);
}
}

Svg::Point MapProjector::ProjectPoint(const Location::Point &p) const {
  return Svg::Point{(p.longitude - min_lon_) * zoom_coef_ + padding_,
                    (max_lat_ - p.latitude) * zoom_coef_ + padding_};
}
}