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
  vector<StopPosition> stop_positions = ComputeUniformStopPositions(db);
  auto [x_count, y_count] = CoordinatesIndexing(stop_positions);
  stop_projection_ = CoordinatesCompression(stop_positions, x_count, y_count);
}

vector<UniformProjector::StopPosition> UniformProjector::ComputeUniformStopPositions(const TransportDatabase::Database &db) {
  FindReferenceStops(db);
  const TransportData::StopsDict &all_stops = db.GetStopsData();
  const TransportData::BusesDict &all_buses = db.GetBusesData();

  unordered_map<string, Location::Point> locations;
  locations.reserve(all_stops.size());
  for (const auto &[_, bus] : all_buses) {
    const vector<string> &stops = bus.stops;
    if (stops.empty())
      continue;
    size_t i = 0;
    size_t j = 0;
    locations[bus.stops[j]] = all_stops.at(bus.stops[j]).position;
    size_t last_stop_idx = bus.is_roundtrip ? stops.size() - 1 : stops.size() / 2;
    while (i < last_stop_idx) {
      j = i + 1;
      while (count(reference_stops_.begin(), reference_stops_.end(), stops[j]) == 0)
        ++j;
      const Location::Point &i_pos = all_stops.at(stops[i]).position;
      const Location::Point &j_pos = all_stops.at(stops[j]).position;
      double step_lon = (j_pos.longitude - i_pos.longitude) / static_cast<double>(j - i);
      double step_lat = (j_pos.latitude - i_pos.latitude) / static_cast<double>(j - i);
      for (size_t k = i + 1; k < j; ++k) {
        Location::Point k_pos{i_pos.latitude + step_lat * static_cast<double>(k - i),
                              i_pos.longitude + step_lon * static_cast<double>(k - i)};
        locations[bus.stops[k]] = k_pos;
      }

      locations[bus.stops[j]] = all_stops.at(bus.stops[j]).position;
      i = j;
    }
  }

  vector<UniformProjector::StopPosition> result;
  set<string> inserted_stops;
  result.reserve(all_stops.size());
  for (const auto &[_, bus] : all_buses) {
    const vector<string> &stops = bus.stops;
    size_t last_stop_idx = bus.is_roundtrip ? stops.size() - 2 : stops.size() / 2;
    for (size_t i = 0; i <= last_stop_idx; ++i) {
      const string &stop_name = stops[i];
      if (inserted_stops.count(stop_name) == 0) {
        result.emplace_back(StopPosition{stop_name, locations.at(stop_name)});
        inserted_stops.insert(stop_name);
      }
    }
  }

  // adding stops with no buses
  for (const auto &[stop_name, stop] : all_stops) {
    if (stop.bus_names.empty())
      result.emplace_back(StopPosition{stop_name, stop.position});
  }
  return result;
}

std::map<std::string, Svg::Point>
UniformProjector::CoordinatesCompression(const std::vector<StopPosition> &stop_positions, size_t x_count,
                                         size_t y_count) const {
  std::map<std::string, Svg::Point> stop_projection;
  double x_step = x_count > 1
                  ? (width_ - 2 * padding_) / (static_cast<double>(x_count) - 1) : 0;
  double y_step = y_count > 1
                  ? (height_ - 2 * padding_) / (static_cast<double>(y_count) - 1) : 0;
  for (const StopPosition &stop_position: stop_positions) {
    size_t x_idx = x_stop_indexing_.at(stop_position.position);
    double x_projection = x_idx * x_step + padding_;
    size_t y_idx = y_stop_indexing_.at(stop_position.position);
    double y_projection = height_ - (y_idx * y_step + padding_);
    stop_projection[stop_position.stop_name] = {x_projection, y_projection};
  }
  return stop_projection;
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


void UniformProjector::FindReferenceStops(const TransportDatabase::Database &db) {
  // adding stops with more than one bus
  for (const auto &[stop_name, stop] : db.GetStopsData()) {
    if (stop.bus_names.empty() || stop.bus_names.size() > 1)
      reference_stops_.insert(stop_name);
  }

  for (const auto &[_, bus] : db.GetBusesData()) {
    if (bus.stops.empty())
      continue;
    // adding first and last stop
    reference_stops_.insert(bus.stops.front());
    if (!bus.is_roundtrip)
      reference_stops_.insert(bus.stops[bus.stops.size() / 2]);

    for (const string &stop : bus.stops) {
      // adding repeated stops for non roundtrip
      if (count(bus.stops.begin(), bus.stops.end(), stop) > 2)
        reference_stops_.insert(stop);
    }
  }
}

std::pair<std::map<Location::Point, size_t>, size_t> UniformProjector::IndexAxis(const std::vector<StopPosition> &stop_positions) const {
  std::map<Location::Point, size_t> result;
  if (stop_positions.empty())
    return {result, 0};
  size_t idx = 0;
  result[stop_positions[0].position] = idx;
  set<string> glued_stops{stop_positions[0].stop_name};
  for (size_t i = 1; i < stop_positions.size(); ++i) {
    const string stop_name = stop_positions[i].stop_name;
    if (CheckAdjacency(stop_name, glued_stops)) {
      ++idx;
      glued_stops.clear();
    }
    const Location::Point &stop_position = stop_positions[i].position;
    if (result.count(stop_position) == 0)
        result[stop_position] = idx;
    glued_stops.insert(stop_name);
  }
  return {move(result), idx + 1};
}

bool UniformProjector::CheckAdjacency(const std::string &current_stop, const std::set<std::string> &other_stops) const {
  for (const string &bus_name : db_.GetStopsData().at(current_stop).bus_names) {
    const TransportData::Bus &bus = db_.GetBusesData().at(bus_name);
    for (size_t i = 0; i < bus.stops.size(); ++i) {
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