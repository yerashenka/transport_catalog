#pragma once

#include "json.h"
#include "transport_database.h"

#include <string>
#include <variant>


namespace Requests {
  struct Stop {
    std::string name;

    Json::Dict Process(const TransportDatabase &db) const;
  };

  struct Bus {
    std::string name;

    Json::Dict Process(const TransportDatabase &db) const;
  };

  struct Route {
    std::string stop_from;
    std::string stop_to;

    Json::Dict Process(const TransportDatabase &db) const;
  };


}

namespace TransportInformer {
  using namespace Requests;

  std::variant<Stop, Bus, Route> Read(const Json::Dict &attrs);

  std::vector<Json::Node> ProcessRequests(const TransportDatabase &db, const std::vector<Json::Node> &requests);
}


