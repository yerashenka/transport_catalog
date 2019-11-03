#include "transport_database.h"

#include <sstream>

using namespace std;

TransportDatabase::TransportDatabase(vector<TransportData::DataQuery> data, const Json::Dict &routing_settings_json) {
  auto stops_end = partition(begin(data), end(data), [](const auto &item) {
    return holds_alternative<TransportData::Stop>(item);
  });

  for (auto &item : Range{begin(data), stops_end}) {
    auto &stop = get<TransportData::Stop>(item);
    stop_responses_.insert({stop.name, {}});
    stops_data_.emplace(stop.name, move(stop));
  }

  for (auto& item : Range{stops_end, end(data)}) {
    auto& bus = get<TransportData::Bus>(item);
    bus_responses_[bus.name] = BusResponse{
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

  router_ = make_unique<TransportRouter>(stops_data_, buses_data_, routing_settings_json);
}

optional<const TransportDatabase::StopResponse> TransportDatabase::GetStopInfo(const string &name) const {
  if (stop_responses_.count(name) != 0) {
    return stop_responses_.at(name);
  } else {
    return nullopt;
  }
}

optional<const TransportDatabase::BusResponse> TransportDatabase::GetBusInfo(const string &name) const {
  if (bus_responses_.count(name) != 0) {
    return bus_responses_.at(name);
  } else {
    return nullopt;
  }
}

optional<const Responses::Route>
    TransportDatabase::FindRoute(const string &stop_from, const string &stop_to) const {
  return router_->FindRoute(stop_from, stop_to);
}

int TransportDatabase::ComputeRoadRouteLength(
    const vector<string> &stops,
    const StopsDict &stops_dict
) {
  int result = 0;
  for (size_t i = 1; i < stops.size(); ++i) {
    result += TransportData::ComputeAdjacentStopsDistance(stops_dict.at(stops[i - 1]), stops_dict.at(stops[i]));
  }
  return result;
}

double TransportDatabase::ComputeGeoRouteDistance(
    const vector<string> &stops,
    const StopsDict &stops_dict
) {
  double result = 0;
  for (size_t i = 1; i < stops.size(); ++i) {
    result += Location::Distance(stops_dict.at(stops[i - 1]).position, stops_dict.at(stops[i]).position);
  }
  return result;
}
