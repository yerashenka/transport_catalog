#include "transport_data.h"

using namespace std;

namespace TransportData {

  Stop Stop::ParseStop(const Json::Dict &info) {
    Stop stop = {
        .name = info.at("name").AsString(),
        .position = {
            .latitude = info.at("latitude").AsDouble(),
            .longitude = info.at("longitude").AsDouble(),
        }
    };
    if (info.count("road_distances") > 0) {
      for (const auto& [neighbour_stop, distance_node] : info.at("road_distances").AsMap()) {
        stop.distances[neighbour_stop] = distance_node.AsInt();
      }
    }
    return stop;
  }

  vector<string> ParseStops(const vector<Json::Node> &stop_nodes, bool is_roundtrip) {
    vector<string> stops;
    stops.reserve(stop_nodes.size());
    for (const Json::Node &stop_node : stop_nodes) {
      stops.emplace_back(stop_node.AsString());
    }
    if (is_roundtrip || stops.size() <= 1) {
      return stops;
    }

    // adding stops for reverse path
    stops.reserve(stops.size() * 2 - 1);
    for (size_t stop_idx = stops.size() - 1; stop_idx > 0; --stop_idx) {
      stops.emplace_back(stops[stop_idx - 1]);
    }
    return stops;
  }

  int ComputeAdjacentStopsDistance(const Stop &lhs, const Stop &rhs) {
    if (auto it = lhs.distances.find(rhs.name); it != lhs.distances.end()) {
      return it->second;
    } else {
      return rhs.distances.at(lhs.name);
    }
  }

  Bus Bus::ParseBus(const Json::Dict &info) {
    return Bus{
        .name = info.at("name").AsString(),
        .is_roundtrip = info.at("is_roundtrip").AsBool(),
        .stops = ParseStops(info.at("stops").AsArray(), info.at("is_roundtrip").AsBool())
    };
  }

  vector<DataQuery> ReadData(const vector<Json::Node> &nodes) {
    vector<DataQuery> result;
    result.reserve(nodes.size());

    for (const Json::Node &node : nodes) {
      const auto &node_dict = node.AsMap();
      const string type = node_dict.at("type").AsString();
      if (type == "Bus") {
        result.emplace_back(Bus::ParseBus(node_dict));
      } else if (type == "Stop") {
        result.emplace_back(Stop::ParseStop(node_dict));
      } else {
        throw runtime_error("Unexpected query type");
      }
    }

    return result;
  }
}
