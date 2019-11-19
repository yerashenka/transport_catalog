#pragma once

#include "transport_data.h"
#include "graph.h"
#include "json.h"
#include "router.h"
#include "responses.h"

#include <memory>
#include <unordered_map>
#include <vector>

class TransportRouter {
private:
  using TransportGraph = Graph::DirectedWeightedGraph<double>;
  using Router = Graph::Router<double>;

public:
  TransportRouter(const TransportData::StopsDict &stops,
                  const TransportData::BusesDict &buses,
                  const Json::Dict &settings);

  std::optional<Responses::Route> FindRoute(const std::string &stop_from, const std::string &stop_to) const;

private:
  struct RoutingSettings {
    int bus_wait_time;  // minutes
    double bus_velocity;  // km/h
  };

  static RoutingSettings ParseRoutingSettings(const Json::Dict &description);

  void FillGraphWithStops(const TransportData::StopsDict &stops);

  void FillGraphWithBuses(const TransportData::StopsDict &stops,
                          const TransportData::BusesDict &buses);

  struct StopVertexIds {
    Graph::VertexId wait_on_stop;
    Graph::VertexId depart_from_stop;
  };
  struct VertexInfo {
    std::string stop_name;
  };

  struct BusEdgeInfo {
    std::string bus_name;
    size_t span_count;
  };
  struct WaitEdgeInfo {};
  using EdgeInfo = std::variant<BusEdgeInfo, WaitEdgeInfo>;

  RoutingSettings routing_settings_;
  TransportGraph graph_;
  std::unique_ptr<Router> router_;
  std::unordered_map<std::string, StopVertexIds> stops_vertex_ids_;
  std::vector<VertexInfo> vertices_info_;
  std::vector<EdgeInfo> edges_info_;
};
