#pragma once

#include "json.h"
#include "transport_database.h"
#include "map_builder.h"

#include <string>
#include <utility>
#include <variant>


namespace Requests {
using TransportDatabase = TransportDatabase::Database;

struct Stop {
  std::string name;

  [[nodiscard]] Json::Dict Process(const TransportDatabase &db) const;
};

struct Bus {
  std::string name;

  [[nodiscard]] Json::Dict Process(const TransportDatabase &db) const;
};

struct Route {
  std::string stop_from;
  std::string stop_to;

  [[nodiscard]] Json::Dict Process(const TransportDatabase &db) const;
};

struct Map {
  std::shared_ptr<Visualisation::MapBuilder> map_builder;
  [[nodiscard]] Json::Dict Process(const TransportDatabase &db) const;
};

using Request = std::variant<Stop, Bus, Route, Map>;
}

namespace TransportInformer {
using TransportDatabase = TransportDatabase::Database;

class Informer {
 public:
  Informer() = default;
  explicit Informer(std::shared_ptr<Visualisation::MapBuilder> map_builder)
    : map_builder_(std::move(map_builder)) {}
  Requests::Request Read(const Json::Dict &attrs);
  std::vector<Json::Node> ProcessRequests(const TransportDatabase &db, const std::vector<Json::Node> &requests);

 private:
  std::shared_ptr<Visualisation::MapBuilder> map_builder_;
};

}


