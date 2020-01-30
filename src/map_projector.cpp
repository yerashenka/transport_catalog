#include "map_projector.h"
#include <algorithm>

using namespace std;

namespace Visualisation {
GeoProjector::GeoProjector(const TransportDatabase::Database &db,
                           double width,
                           double height,
                           double padding) : db_(db) {
  padding_ = padding;
  double max_lon = std::numeric_limits<double>::min();
  double min_lat = std::numeric_limits<double>::max();
  for (const auto &[_, stop] : db.GetStopsData()) {
    min_lat = min(min_lat, stop.position.latitude);
    min_lon_ = min(min_lon_, stop.position.longitude);
    max_lat_ = max(max_lat_, stop.position.latitude);
    max_lon = max(max_lon, stop.position.longitude);
  }

  if(min_lat == max_lat_ && min_lon_ == max_lon) {
    zoom_coef_ = 0;
  } else if (min_lat == max_lat_) {
    zoom_coef_ = (width - 2 * padding_) / (max_lon - min_lon_);
  } else if (min_lon_ == max_lon) {
    zoom_coef_ = (height - 2 * padding_) / (max_lat_ - min_lat);
  } else {
    double width_zoom_coef = (width - 2 * padding_) / (max_lon - min_lon_);
    double height_zoom_coef = (height - 2 * padding_) / (max_lat_ - min_lat);
    zoom_coef_ = min(width_zoom_coef, height_zoom_coef);
  }
}

Svg::Point GeoProjector::ProjectStop(const std::string &stop_name) const {
  const Location::Point &loc = db_.GetStopsData().at(stop_name).position;
  return Svg::Point{(loc.longitude - min_lon_) * zoom_coef_ + padding_,
                    (max_lat_ - loc.latitude) * zoom_coef_ + padding_};
}

UniformProjector::UniformProjector(const TransportDatabase::Database &db,
                                   double width, double height, double padding)
                                   : db_(db), height_(height), width_(width),
                                     padding_(padding) {
  vector<StopPosition> stop_positions;
  for (const auto &[stop_name, stop] : db_.GetStopsData()) {
    stop_positions.emplace_back(StopPosition{stop_name, stop.position});
  }

  auto [x_count, y_count] = CoordinatesIndexing(stop_positions);
  CoordinatesCompression(x_count, y_count);
}

void UniformProjector::CoordinatesCompression(size_t  x_count, size_t  y_count) {
  double x_step = x_count > 1
                  ? (width_ - 2 * padding_) / (static_cast<double >(x_count) - 1) : 0;
  double y_step = y_count > 1
                  ? (height_ - 2 * padding_) / (static_cast<double>(y_count) - 1) : 0;
  for (const auto &[stop_name, _] : db_.GetStopsData()) {
    size_t x_idx = x_stop_indexing_.at(stop_name);
    double x_projection = x_idx * x_step + padding_;
    size_t y_idx = y_stop_indexing_.at(stop_name);
    double y_projection = height_ - (y_idx * y_step + padding_);
    stop_projection_[stop_name] = {x_projection, y_projection};
  }
}

std::pair<size_t, size_t> UniformProjector::CoordinatesIndexing(std::vector<StopPosition> &stop_positions) {
  sort(stop_positions.begin(), stop_positions.end(), [](const auto &lhs, const auto &rhs) {
    return lhs.position.longitude < rhs.position.longitude;
  });
  auto [x_stop_indexing, x_count] = IndexAxis(stop_positions);
  x_stop_indexing_ = move(x_stop_indexing);

  sort(stop_positions.begin(), stop_positions.end(), [](const auto &lhs, const auto &rhs) {
    return lhs.position.latitude < rhs.position.latitude;
  });
  auto [y_stop_indexing, y_count]= IndexAxis(stop_positions);
  y_stop_indexing_ = move(y_stop_indexing);

  return {x_count, y_count};
}

pair<std::map<std::string, size_t>, size_t> UniformProjector::IndexAxis(const std::vector<StopPosition> &stop_positions) {
  std::map<std::string, size_t> result;
  size_t idx = 0;
  result[stop_positions[0].stop_name] = idx;
  set<string> glued_stops{stop_positions[0].stop_name};
  for (size_t i = 1; i < stop_positions.size(); ++i) {
    const string stop = stop_positions[i].stop_name;
    if (CheckAdjacency(stop, glued_stops)) {
      ++idx;
      glued_stops.clear();
    }
    result[stop] = idx;
    glued_stops.insert(stop);
  }
  return {move(result), idx + 1};
}

bool UniformProjector::CheckAdjacency(const std::string &current_stop, const std::set<std::string> &other_stops) {
  for (const string &bus_name : db_.GetStopsData().at(current_stop).bus_names) {
    const TransportData::Bus &bus = db_.GetBusesData().at(bus_name);
    for (int i = 0; i < bus.stops.size(); ++i) {
      if (bus.stops[i] == current_stop) {
        if (i != 0) {
          string previous_stop = bus.stops[i - 1];
          if (other_stops.count(previous_stop) > 0)
            return true;
        }

        if (i != bus.stops.size() - 1) {
          string next_stop = bus.stops[i+1];
          if (other_stops.count(next_stop) > 0)
            return true;
        }
      }
    }
  }

  return false;
}

}