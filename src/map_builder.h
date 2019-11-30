#pragma once

#include "map_projector.h"
#include "svg.h"
#include "transport_data.h"
#include "transport_database.h"
#include <map>
#include <string>
#include <vector>
#include <unordered_map>

namespace Visualisation {
using StopsDict = TransportData::StopsDict;
using BusesDict = TransportData::BusesDict;

struct RenderSettings {
  double width;
  double height;
  double padding;

  double stop_radius;
  double line_width;
  std::vector<Svg::Color> color_palette;

  int stop_label_font_size;
  Svg::Point stop_label_offset;
  int bus_label_font_size;
  Svg::Point bus_label_offset;
  Svg::Color underlayer_color;
  double undrlayer_width;
};

template <typename T>
std::set<std::string> SortNames(const std::unordered_map<std::string, T> &map) {
  std::set<std::string> sorted_names;
  for (const auto &[name, _] : map) {
    sorted_names.insert(name);
  }
  return sorted_names;
}

Svg::Color ParseColor(const Json::Node &color_node);
RenderSettings ParseRenderSettings(const Json::Dict &render_settings);
std::string EscapeSpecialCharacters(std::string &input);

class MapBuilder {
 public:
  explicit MapBuilder(const TransportDatabase::Database &db, const Json::Dict &render_settings);
  [[nodiscard]] std::string GetMap() const { return map_; }

 private:
  RenderSettings settings_;
  MapProjector projector_;
  std::string map_{};


  void DrawRoutes(Svg::Document &doc, const TransportDatabase::Database &db,
                  const std::set<std::string> &route_names);
  void DrawStops(Svg::Document &doc, const TransportDatabase::Database &db,
                 const std::set<std::string> &stop_names);
  void DrawStopLabels(Svg::Document &doc, const TransportDatabase::Database &db,
                      const std::set<std::string> &stop_names);
//  void DrawBusLabels(Svg::Document &doc, const TransportDatabase::Database &db,
//                     const std::set<std::string> &route_names);
};
}
