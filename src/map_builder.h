#pragma once

#include "map_projector.h"
#include "svg.h"
#include "transport_data.h"
#include <string>
#include <vector>

namespace Visualisation {
using StopsDict = TransportData::StopsDict;
using BusesDict = TransportData::BusesDict;

struct RenderSettings {
  double width;
  double height;
  double padding;
  double stop_radius;
  double line_width;
  int stop_label_font_size;
  Svg::Point stop_label_offset;
  Svg::Color underlayer_color;
  double undrlayer_width;
  std::vector<Svg::Color> color_palette;
};

Svg::Color ParseColor(const Json::Node &color_node);
RenderSettings ParseRenderSettings(const Json::Dict &render_settings);
std::string EscapeSpecialCharacters(const std::string &test);

class MapBuilder {
 public:
  explicit MapBuilder(const Json::Dict &render_settings)
    : settings_(ParseRenderSettings(render_settings)) {}
  void BuildMap(const StopsDict &stops, const BusesDict &buses);
  bool IsMapBuilt() const { return !map_.empty(); }
  std::string GetMap() const { return map_; }

 private:
  RenderSettings settings_;
  MapProjector projector_;
  std::string map_;

  void BuildRoutes(Svg::Document &doc, const BusesDict &buses, const StopsDict &stops);
  void BuildStops(Svg::Document &doc, const StopsDict &stops);
  void BuildStopLabels(Svg::Document &doc, const StopsDict &stops);
};
}
