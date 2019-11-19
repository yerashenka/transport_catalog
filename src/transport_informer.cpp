#include "transport_informer.h"
#include "transport_router.h"

#include <vector>

using namespace std;

namespace Requests {

  Json::Dict Stop::Process(const TransportDatabase &db) const {
    const auto stop = db.GetStopInfo(name);
    Json::Dict response;
    if (!stop) {
      response["error_message"] = Json::Node("not found"s);
    } else {
      vector<Json::Node> bus_nodes;
      bus_nodes.reserve(stop->bus_names.size());
      for (const auto& bus_name : stop->bus_names) {
        bus_nodes.emplace_back(bus_name);
      }
      response["buses"] = Json::Node(move(bus_nodes));
    }
    return response;
  }

  Json::Dict Bus::Process(const TransportDatabase &db) const {
    const auto bus = db.GetBusInfo(name);
    Json::Dict response;
    if (!bus) {
      response["error_message"] = Json::Node("not found"s);
    } else {
      response = {
          {"stop_count", Json::Node(static_cast<int>(bus->stop_count))},
          {"unique_stop_count", Json::Node(static_cast<int>(bus->unique_stop_count))},
          {"route_length", Json::Node(bus->road_route_length)},
          {"curvature", Json::Node(bus->road_route_length / bus->geo_route_length)},
      };
    }
    return response;
  }

  struct RouteItemResponseBuilder {
    Json::Dict operator()(const Responses::Route::BusItem &bus_item) const {
      return Json::Dict{
          {"type", Json::Node("Bus"s)},
          {"bus", Json::Node(bus_item.bus_name)},
          {"time", Json::Node(bus_item.time)},
          {"span_count", Json::Node(static_cast<int>(bus_item.span_count))}
      };
    }
    Json::Dict operator()(const Responses::Route::WaitItem &wait_item) const {
      return Json::Dict{
          {"type", Json::Node("Wait"s)},
          {"stop_name", Json::Node(wait_item.stop_name)},
          {"time", Json::Node(wait_item.time)},
      };
    }
  };

  Json::Dict Route::Process(const TransportDatabase &db) const {
    Json::Dict response;
    const auto route = db.FindRoute(stop_from, stop_to);
    if (!route) {
      response["error_message"] = Json::Node("not found"s);
    } else {
      response["total_time"] = Json::Node(route->total_time);
      vector<Json::Node> items;
      items.reserve(route->items.size());
      for (const auto& item : route->items) {
        items.emplace_back(visit(RouteItemResponseBuilder{}, item));
      }

      response["items"] = move(items);
    }

    return response;
  }

  Json::Dict Map::Process(const TransportDatabase &db) const {
    Json::Dict response;
    if (!map_builder.IsMapBuilt()) {
      map_builder.BuildMap(db.GetStopsData(), db.GetBusesData());
    }
    response["map"] = map_builder.GetMap();
    return response;
  }
}

namespace TransportInformer {
using namespace Requests;

Request Informer::Read(const Json::Dict &attrs) {
  const string &type = attrs.at("type").AsString();
  if (type == "Bus") {
    return Bus{attrs.at("name").AsString()};
  } else if (type == "Stop") {
    return Stop{attrs.at("name").AsString()};
  } else if (type == "route") {
    return Route{attrs.at("from").AsString(), attrs.at("to").AsString()};
  } else if (type == "map") {
    return Map{map_builder_};
  } else {
    throw runtime_error("unknown request type");
  }
}

vector<Json::Node> Informer::ProcessRequests(const TransportDatabase &db, const vector<Json::Node> &requests) {
  vector<Json::Node> responses;
  responses.reserve(requests.size());
  for (const auto &request_info : requests) {
    auto process_lambda = [&db](const auto &request) {
      return request.Process(db);
    };
    Json::Dict dict = visit(process_lambda, Read(request_info.AsMap()));
    dict["request_id"] = Json::Node(request_info.AsMap().at("id").AsInt());
    responses.emplace_back(dict);
  }
  return responses;
}
}
