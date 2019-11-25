#include "map_builder.h"
#include <set>
#include <sstream>

using namespace std;

namespace Visualisation {
Svg::Color ParseColor(const Json::Node &color_node) {
  if (color_node.IsString()) {
    return Svg::Color(color_node.AsString());
  } else if (color_node.IsArray()) {
    const vector<Json::Node> &color_array = color_node.AsArray();
    if (color_array.size() == 3) {
      return Svg::Color(Svg::Rgb{static_cast<uint8_t>(color_array[0].AsInt()),
                                 static_cast<uint8_t>(color_array[1].AsInt()),
                                 static_cast<uint8_t>(color_array[2].AsInt())});
    } else if (color_array.size() == 4) {
      return Svg::Color(Svg::Rgba{static_cast<uint8_t>(color_array[0].AsInt()),
                                 static_cast<uint8_t>(color_array[1].AsInt()),
                                 static_cast<uint8_t>(color_array[2].AsInt()),
                                 color_array[3].AsDouble()});
    } else {
      throw runtime_error("can't parse color");
    }
  } else {
    throw runtime_error("can't parse color");
  }
}

RenderSettings ParseRenderSettings(const Json::Dict &render_settings) {
  RenderSettings result;
  result.width = render_settings.at("width").AsDouble();
  result.height = render_settings.at("height").AsDouble();
  result.padding = render_settings.at("padding").AsDouble();
  result.stop_radius = render_settings.at("stop_radius").AsDouble();
  result.line_width = render_settings.at("line_width").AsDouble();
  result.stop_label_font_size = render_settings.at(".stop_label_font_size").AsInt();
  const Json::Dict &stop_label_offset = render_settings.at("stop_label_offset").AsMap();
  result.stop_label_offset = Svg::Point{stop_label_offset.at("0").AsDouble(),
                                        stop_label_offset.at("1").AsDouble()};
  result.underlayer_color = ParseColor(render_settings.at("underlayer_color"));
  result.undrlayer_width = render_settings.at("undrlayer_width").AsDouble();
  const vector<Json::Node> &color_palette = render_settings.at("color_palette").AsArray();
  result.color_palette.reserve(color_palette.size());
  for (const Json::Node &color_node : color_palette) {
    result.color_palette.emplace_back(ParseColor(color_node));
  }
  return result;
}

void MapBuilder::BuildMap(const StopsDict &stops, const BusesDict &buses) {
  projector_.Initialize(stops, settings_.width, settings_.height, settings_.padding);
  Svg::Document doc;
  BuildRoutes(doc, buses, stops);
  BuildStops(doc, stops);
  BuildStopLabels(doc, stops);
  stringstream stream;
  doc.Render(stream);
  map_ = EscapeSpecialCharacters(stream.str());
}
void MapBuilder::BuildRoutes(Svg::Document &doc,
                             const Visualisation::BusesDict &buses,
                             const StopsDict &stops) {
  set<string> route_names;
  for (const auto &[route_name, _] : buses) {
    route_names.insert(route_name);   // sorting
  }
  size_t color_id = 0;
  for (const string &route_name : route_names) {
    Svg::Polyline polyline;
    polyline.SetStrokeColor(settings_.color_palette[color_id % settings_.color_palette.size()]);
    color_id++;
    polyline.SetStrokeWidth(settings_.line_width);
    polyline.SetStrokeLineCap("round").SetStrokeLineJoin("round");
    for (const string &stop_name : buses.at(route_name).stops) {
      polyline.AddPoint(projector_.ProjectPoint(stops.at(stop_name).position));
    }
    doc.Add(move(polyline));
  }
}

}