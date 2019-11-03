#pragma once

#include <set>

namespace Responses {
    struct Stop {
        std::set<std::string> bus_names;
    };

    struct Bus {
        size_t stop_count = 0;
        size_t unique_stop_count = 0;
        int road_route_length = 0;
        double geo_route_length = 0.0;
    };

  struct Route {
    double total_time;

    struct BusItem {
      std::string bus_name;
      double time;
      size_t span_count;
    };
    struct WaitItem {
      std::string stop_name;
      double time;
    };

    using Item = std::variant<BusItem, WaitItem>;
    std::vector<Item> items;
  };
}