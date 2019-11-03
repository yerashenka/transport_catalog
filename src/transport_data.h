#pragma once

#include "json.h"
#include "location.h"

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace TransportData {
  struct Stop {
    std::string name;
    Location::Point position;
    std::unordered_map<std::string, int> distances;

    static Stop ParseStop(const Json::Dict &info);
  };

  int ComputeAdjacentStopsDistance(const Stop &lhs, const Stop &rhs);

  std::vector<std::string> ParseStops(const std::vector<Json::Node> &stop_nodes, bool is_roundtrip);

  struct Bus {
    std::string name;
    std::vector<std::string> stops;

    static Bus ParseBus(const Json::Dict &info);
  };

  using DataQuery = std::variant<Stop, Bus>;
  using StopsDict = std::unordered_map<std::string, const TransportData::Stop>;
  using BusesDict = std::unordered_map<std::string, const TransportData::Bus>;

  std::vector<DataQuery> ReadData(const std::vector<Json::Node> &nodes);
}
