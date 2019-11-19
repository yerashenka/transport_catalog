#include "transport_router.h"

using namespace std;


TransportRouter::TransportRouter(const TransportData::StopsDict &stops,
                                 const TransportData::BusesDict &buses,
                                 const Json::Dict &settings)
    : routing_settings_(ParseRoutingSettings(settings))
{
  const size_t vertex_count = stops.size() * 2;
  vertices_info_.resize(vertex_count);
  graph_ = TransportGraph(vertex_count);

  FillGraphWithStops(stops);
  FillGraphWithBuses(stops, buses);

  router_ = std::make_unique<Router>(graph_);
}

TransportRouter::RoutingSettings TransportRouter::ParseRoutingSettings(const Json::Dict &description) {
  return {
      description.at("bus_wait_time").AsInt(),
      description.at("bus_velocity").AsDouble(),
  };
}

void TransportRouter::FillGraphWithStops(const TransportData::StopsDict &stops) {
  Graph::VertexId vertex_id = 0;

  for (const auto &[stop_name, _] : stops) {
    auto &vertex_ids = stops_vertex_ids_[stop_name];
    vertex_ids.wait_on_stop = vertex_id++;
    vertex_ids.depart_from_stop = vertex_id++;
    vertices_info_[vertex_ids.wait_on_stop] = {stop_name};
    vertices_info_[vertex_ids.depart_from_stop] = {stop_name};

    edges_info_.emplace_back(WaitEdgeInfo{});
    graph_.AddEdge({
        vertex_ids.wait_on_stop,
        vertex_ids.depart_from_stop,
        static_cast<double>(routing_settings_.bus_wait_time)
    });
  }
}

void TransportRouter::FillGraphWithBuses(const TransportData::StopsDict &stops,
                                         const TransportData::BusesDict &buses) {
  for (const auto &[_, current_bus] : buses) {
    const size_t stop_count = current_bus.stops.size();
    if (stop_count <= 1) {
      continue;
    }

    for (size_t start_idx = 0; start_idx < stop_count - 1; ++start_idx) {
      const string &start_stop_name = current_bus.stops[start_idx];
      const Graph::VertexId start_vertex = stops_vertex_ids_[start_stop_name].depart_from_stop;
      int total_distance = 0;
      for (size_t finish_idx = start_idx + 1; finish_idx < stop_count; ++finish_idx) {
        const string &finish_stop_name = current_bus.stops[finish_idx];
        total_distance += TransportData::ComputeAdjacentStopsDistance(stops.at(current_bus.stops[finish_idx - 1]),
                                                                      stops.at(finish_stop_name));
        edges_info_.emplace_back(BusEdgeInfo{
            .bus_name = current_bus.name,
            .span_count = finish_idx - start_idx,
        });
        graph_.AddEdge({
            start_vertex,
            stops_vertex_ids_[finish_stop_name].wait_on_stop,
            total_distance * 1.0 / (routing_settings_.bus_velocity * 1000.0 / 60)  // m / (km/h * 1000 / 60) = min
        });
      }
    }
  }
}

optional<Responses::Route> TransportRouter::FindRoute(const string &stop_from, const string &stop_to) const {
  const Graph::VertexId vertex_from = stops_vertex_ids_.at(stop_from).wait_on_stop;
  const Graph::VertexId vertex_to = stops_vertex_ids_.at(stop_to).wait_on_stop;
  const auto route = router_->BuildRoute(vertex_from, vertex_to);
  if (!route) {
    return nullopt;
  }

  Responses::Route route_info = {.total_time = route->weight};
  route_info.items.reserve(route->edge_count);
  for (size_t edge_idx = 0; edge_idx < route->edge_count; ++edge_idx) {
    const Graph::EdgeId edge_id = router_->GetRouteEdge(route->id, edge_idx);
    const auto &edge = graph_.GetEdge(edge_id);
    const auto &edge_info = edges_info_[edge_id];
    if (holds_alternative<BusEdgeInfo>(edge_info)) {
      const auto &bus_edge_info = get<BusEdgeInfo>(edge_info);
      route_info.items.emplace_back(Responses::Route::BusItem{
          .bus_name = bus_edge_info.bus_name,
          .time = edge.weight,
          .span_count = bus_edge_info.span_count,
      });
    } else {
      const Graph::VertexId vertex_id = edge.from;
      route_info.items.emplace_back(Responses::Route::WaitItem{
          .stop_name = vertices_info_[vertex_id].stop_name,
          .time = edge.weight,
      });
    }
  }

  router_->ReleaseRoute(route->id);
  return route_info;
}
