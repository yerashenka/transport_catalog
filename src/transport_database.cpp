#include "transport_database.h"

#include <sstream>

namespace TransportDatabase {
using namespace std;

Database::Database(vector<TransportData::DataQuery> data, const Json::Dict &routing_settings) {
  auto stops_end = partition(begin(data), end(data), [](const auto &item) {
    return holds_alternative<TransportData::Stop>(item);
  });

  for (auto &item : Range{begin(data), stops_end}) {
    auto &stop = get<TransportData::Stop>(item);
    stop_responses_.insert({stop.name, {}});
    stops_data_.emplace(stop.name, move(stop));
  }

  for (auto &item : Range{stops_end, end(data)}) {
    auto &bus = get<TransportData::Bus>(item);
    bus_responses_[bus.name] = Responses::Bus{
        bus.stops.size(),
        ComputeUniqueItemsCount(AsRange(bus.stops)),
        ComputeRoadRouteLength(bus.stops, stops_data_),
        ComputeGeoRouteDistance(bus.stops, stops_data_)
    };

    for (const string &stop_name : bus.stops) {
      stop_responses_.at(stop_name).bus_names.insert(bus.name);
    }

    buses_data_.emplace(bus.name, move(bus));
  }

  router_ = make_unique<TransportRouter>(stops_data_, buses_data_, routing_settings);
}

optional<const Responses::Stop> Database::GetStopInfo(const string &name) const {
  if (stop_responses_.count(name) != 0) {
    return stop_responses_.at(name);
  } else {
    return nullopt;
  }
}

optional<const Responses::Bus> Database::GetBusInfo(const string &name) const {
  if (bus_responses_.count(name) != 0) {
    return bus_responses_.at(name);
  } else {
    return nullopt;
  }
}

optional<const Responses::Route>
Database::FindRoute(const string &stop_from, const string &stop_to) const {
  return router_->FindRoute(stop_from, stop_to);
}

int Database::ComputeRoadRouteLength(
    const vector<string> &stops,
    const TransportData::StopsDict &stops_dict
) {
  int result = 0;
  for (size_t i = 1; i < stops.size(); ++i) {
    result += TransportData::ComputeAdjacentStopsDistance(stops_dict.at(stops[i - 1]), stops_dict.at(stops[i]));
  }
  return result;
}

double Database::ComputeGeoRouteDistance(
    const vector<string> &stops,
    const TransportData::StopsDict &stops_dict
) {
  double result = 0;
  for (size_t i = 1; i < stops.size(); ++i) {
    result += Location::Distance(stops_dict.at(stops[i - 1]).position, stops_dict.at(stops[i]).position);
  }
  return result;
}
}
