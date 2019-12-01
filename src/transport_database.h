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

namespace TransportDatabase {
class Database {
 public:
  Database(std::vector<TransportData::DataQuery> data, const Json::Dict &routing_settings);

  [[nodiscard]] const TransportData::StopsDict &GetStopsData() const { return stops_data_; }
  [[nodiscard]] const TransportData::BusesDict &GetBusesData() const { return buses_data_; }

  [[nodiscard]] std::optional<const Responses::Stop> GetStopInfo(const std::string &name) const;
  [[nodiscard]] std::optional<const Responses::Bus> GetBusInfo(const std::string &name) const;

  [[nodiscard]]
  std::optional<const Responses::Route> FindRoute(const std::string &stop_from, const std::string &stop_to) const;

 private:
  static int ComputeRoadRouteLength(
      const std::vector<std::string> &stops,
      const TransportData::StopsDict &stops_dict
  );

  static double ComputeGeoRouteDistance(
      const std::vector<std::string> &stops,
      const TransportData::StopsDict &stops_dict
  );

  TransportData::StopsDict stops_data_{};
  TransportData::BusesDict buses_data_{};
  std::unordered_map<std::string, Responses::Stop> stop_responses_{};
  std::unordered_map<std::string, Responses::Bus> bus_responses_{};
  std::unique_ptr<TransportRouter> router_ = nullptr;
};
}
