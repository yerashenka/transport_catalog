#pragma once

#include "json.h"
#include "location.h"

#include <set>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace TransportData {
  struct Stop {
    std::string name;
    Location::Point position;
    std::unordered_map<std::string, int> distances;
    std::set<std::string> bus_names;

    static Stop ParseStop(const Json::Dict &info);
  };

  int ComputeAdjacentStopsDistance(const Stop &lhs, const Stop &rhs);

  std::vector<std::string> ParseStops(const std::vector<Json::Node> &stop_nodes, bool is_roundtrip);

  struct Bus {
    std::string name;
    bool is_roundtrip;
    std::vector<std::string> stops;

    static Bus ParseBus(const Json::Dict &info);
  };

  using DataQuery = std::variant<Stop, Bus>;
  using StopsDict = std::unordered_map<std::string, TransportData::Stop>;
  using BusesDict = std::unordered_map<std::string, TransportData::Bus>;

  std::vector<DataQuery> ReadData(const std::vector<Json::Node> &nodes);
}
