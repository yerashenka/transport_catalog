#pragma once

#include "transport_data.h"
#include "svg.h"
#include <limits>


namespace Visualisation {
class MapProjector {
 public:
  void Initialize(const TransportData::StopsDict &stops, double map_width,
                  double map_height, double map_padding);
  [[nodiscard]]
  Svg::Point ProjectPoint(const Location::Point &p) const;

 private:
  double padding_{0.};
  double zoom_coef_{0.};

  double min_lon_{std::numeric_limits<double>::max()};
  double max_lat_{std::numeric_limits<double>::min()};
};
}