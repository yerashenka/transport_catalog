#pragma once

#include "utils.h"
#include "json.h"
#include "transport_data.h"
#include "transport_router.h"
#include "responses.h"

#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>


class TransportDatabase {
private:
  using StopsDict = TransportData::StopsDict;
  using BusesDict = TransportData::BusesDict;
  using BusResponse = Responses::Bus;
  using StopResponse = Responses::Stop;

public:
  TransportDatabase(std::vector<TransportData::DataQuery> data, const Json::Dict &routing_settings_json);

  std::optional<const StopResponse> GetStopInfo(const std::string &name) const;
  std::optional<const BusResponse> GetBusInfo(const std::string &name) const;

  std::optional<const Responses::Route> FindRoute(const std::string &stop_from, const std::string &stop_to) const;


private:
  static int ComputeRoadRouteLength(
      const std::vector<std::string> &stops,
      const StopsDict &stops_dict
  );

  static double ComputeGeoRouteDistance(
      const std::vector<std::string> &stops,
      const StopsDict &stops_dict
  );

  StopsDict stops_data_;
  BusesDict buses_data_;
  std::unordered_map<std::string, StopResponse> stop_responses_;
  std::unordered_map<std::string, BusResponse> bus_responses_;
  std::unique_ptr<TransportRouter> router_;
};
