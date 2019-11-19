#pragma once

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

RenderSettings ParseRenderSettings(const Json::Dict &render_settings);
Svg::Color ParseColor(const Json::Node &color_node);

class MapBuilder {
 public:
  explicit MapBuilder(const Json::Dict &render_settings)
    : render_settings_(ParseRenderSettings(render_settings)) {}
  void BuildMap(const StopsDict &stops, const BusesDict &buses);
  bool IsMapBuilt() const { return !map_.empty(); }
  std::string GetMap() const { return map_; }

 private:
  RenderSettings render_settings_;
  double width_zoom_coef_{1.};
  double height_zoom_coef_{1.};
  std::string map_;

  void RenderRoutes(Svg::Document &doc, const BusesDict &buses) const;
  void RenderStops(Svg::Document &doc, const StopsDict &stops) const;
  void RenderStopLabels(Svg::Document &doc, const StopsDict &stops) const;
  double CalculateZoomCoefficient(double param) const;
};
}
