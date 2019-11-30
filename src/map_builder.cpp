#include "map_builder.h"
#include "cctype"
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
  RenderSettings result{};
  result.width = render_settings.at("width").AsDouble();
  result.height = render_settings.at("height").AsDouble();
  result.padding = render_settings.at("padding").AsDouble();
  result.stop_radius = render_settings.at("stop_radius").AsDouble();
  result.line_width = render_settings.at("line_width").AsDouble();
  result.stop_label_font_size = render_settings.at("stop_label_font_size").AsInt();
  const Json::Array &stop_label_offset = render_settings.at("stop_label_offset").AsArray();
  result.stop_label_offset = Svg::Point{stop_label_offset[0].AsDouble(),
                                        stop_label_offset[1].AsDouble()};
  result.underlayer_color = ParseColor(render_settings.at("underlayer_color"));
  result.undrlayer_width = render_settings.at("underlayer_width").AsDouble();
  const vector<Json::Node> &color_palette = render_settings.at("color_palette").AsArray();
  result.color_palette.reserve(color_palette.size());
  for (const Json::Node &color_node : color_palette) {
    result.color_palette.emplace_back(ParseColor(color_node));
  }
  return result;
}

string EscapeSpecialCharacters(string input) {
  string output;
  for (const char c : input) {
    if (c == '"' || c == '\\') {
      output.push_back('\\');
    }
    output.push_back(c);
  }
  return output;
}

void MapBuilder::BuildMap(const StopsDict &stops, const BusesDict &buses) {
  projector_.Initialize(stops, settings_.width, settings_.height, settings_.padding);
  Svg::Document doc{};
  BuildRoutes(doc, buses, stops);
  BuildStops(doc, stops);
  BuildStopLabels(doc, stops);
  ostringstream stream;
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

void MapBuilder::BuildStops(Svg::Document &doc, const StopsDict &stops) {
  set<string> stop_names;
  for (const auto &[stop_name, _] : stops) {
    stop_names.insert(stop_name);
  }
  for (const string &stop_name : stop_names) {
    Svg::Circle stop_circle;
    stop_circle.SetCenter(projector_.ProjectPoint(stops.at(stop_name).position));
    stop_circle.SetRadius(settings_.stop_radius);
    stop_circle.SetFillColor("white");
    doc.Add(move(stop_circle));
  }
}

void MapBuilder::BuildStopLabels(Svg::Document &doc, const StopsDict &stops) {
  set<string> stop_names;
  for (const auto &[stop_name, _] : stops) {
    stop_names.insert(stop_name);
  }
  for (const string &stop_name : stop_names) {
    Svg::Text substrate;
    substrate.SetPoint(projector_.ProjectPoint(stops.at(stop_name).position));
    substrate.SetOffset(settings_.stop_label_offset);
    substrate.SetFontSize(settings_.stop_label_font_size);
    substrate.SetFontFamily("Verdana");
    substrate.SetData(stop_name);
    Svg::Text text = substrate;

    substrate.SetFillColor(settings_.underlayer_color);
    substrate.SetStrokeColor(settings_.underlayer_color);
    substrate.SetStrokeWidth(settings_.undrlayer_width);
    substrate.SetStrokeLineCap("round");
    substrate.SetStrokeLineJoin("round");
    text.SetFillColor("black");

    doc.Add(move(substrate));
    doc.Add(move(text));
  }
}

}