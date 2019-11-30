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
  if (render_settings.empty())
    return result;
  result.width = render_settings.at("width").AsDouble();
  result.height = render_settings.at("height").AsDouble();
  result.padding = render_settings.at("padding").AsDouble();
  result.stop_radius = render_settings.at("stop_radius").AsDouble();
  result.line_width = render_settings.at("line_width").AsDouble();
  const vector<Json::Node> &color_palette = render_settings.at("color_palette").AsArray();
  result.color_palette.reserve(color_palette.size());
  for (const Json::Node &color_node : color_palette) {
    result.color_palette.emplace_back(ParseColor(color_node));
  }

  result.stop_label_font_size = render_settings.at("stop_label_font_size").AsInt();
  const Json::Array &stop_label_offset = render_settings.at("stop_label_offset").AsArray();
  result.stop_label_offset = Svg::Point{stop_label_offset[0].AsDouble(),
                                        stop_label_offset[1].AsDouble()};
  result.bus_label_font_size = render_settings.at("bus_label_font_size").AsInt();
  const Json::Array &bus_label_offset = render_settings.at("bus_label_offset").AsArray();
  result.bus_label_offset = Svg::Point{bus_label_offset[0].AsDouble(),
                                        bus_label_offset[1].AsDouble()};
  result.underlayer_color = ParseColor(render_settings.at("underlayer_color"));
  result.undrlayer_width = render_settings.at("underlayer_width").AsDouble();
  return result;
}

string EscapeSpecialCharacters(const string &input) {
  string output;
  for (const char c : input) {
    if (c == '"' || c == '\\') {
      output.push_back('\\');
    }
    output.push_back(c);
  }
  return output;
}

MapBuilder::MapBuilder(const TransportDatabase::Database &db, const Json::Dict &render_settings)
  : settings_(ParseRenderSettings(render_settings)),
    projector_(db.GetStopsData(), settings_.width, settings_.height, settings_.padding) {
  set<string> route_names_sorted = SortNames(db.GetRoutesData());
  set<string> stop_names_sorted = SortNames(db.GetStopsData());

  Svg::Document doc{};
  DrawRoutes(doc, db, route_names_sorted);
  DrawRouteLabels(doc, db, route_names_sorted);
  DrawStops(doc, db, stop_names_sorted);
  DrawStopLabels(doc, db, stop_names_sorted);
  ostringstream stream;
  doc.Render(stream);
  map_ = EscapeSpecialCharacters(stream.str());
}

void MapBuilder::DrawRoutes(Svg::Document &doc, const TransportDatabase::Database &db,
                            const set<string> &route_names) {
  size_t color_id = 0;
  for (const string &route_name : route_names) {
    Svg::Polyline polyline;
    polyline.SetStrokeColor(settings_.color_palette[color_id++ % settings_.color_palette.size()]);
    polyline.SetStrokeWidth(settings_.line_width);
    polyline.SetStrokeLineCap("round").SetStrokeLineJoin("round");
    for (const string &stop_name : db.GetRoutesData().at(route_name).stops) {
      polyline.AddPoint(projector_.ProjectPoint(db.GetStopsData().at(stop_name).position));
    }
    doc.Add(move(polyline));
  }
}

void MapBuilder::DrawStops(Svg::Document &doc, const TransportDatabase::Database &db,
                           const set<string> &stop_names) {
  for (const string &stop_name : stop_names) {
    Svg::Circle stop_circle;
    stop_circle.SetCenter(projector_.ProjectPoint(db.GetStopsData().at(stop_name).position));
    stop_circle.SetRadius(settings_.stop_radius);
    stop_circle.SetFillColor("white");
    doc.Add(move(stop_circle));
  }
}

void MapBuilder::DrawStopLabels(Svg::Document &doc, const TransportDatabase::Database &db,
                                const set<string> &stop_names) {
  for (const string &stop_name : stop_names) {
    Svg::Text substrate;
    substrate.SetPoint(projector_.ProjectPoint(db.GetStopsData().at(stop_name).position));
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

void MapBuilder::DrawRouteLabels(Svg::Document &doc, const TransportDatabase::Database &db,
                                 const std::set<std::string> &route_names) {
  size_t color_id = 0;
  for (const string &route_name : route_names) {
    const TransportData::Bus &route = db.GetRoutesData().at(route_name);

    Svg::Text first_stop_substrate;
    const string &first_stop_name = route.stops.front();
    first_stop_substrate.SetPoint(projector_.ProjectPoint(db.GetStopsData().at(first_stop_name).position));
    first_stop_substrate.SetOffset(settings_.bus_label_offset);
    first_stop_substrate.SetFontSize(settings_.bus_label_font_size);
    first_stop_substrate.SetFontFamily("Verdana");
    first_stop_substrate.SetFontWeight("bold");
    first_stop_substrate.SetData(route_name);
    Svg::Text first_stop_text = first_stop_substrate;

    first_stop_substrate.SetFillColor(settings_.underlayer_color);
    first_stop_substrate.SetStrokeColor(settings_.underlayer_color);
    first_stop_substrate.SetStrokeWidth(settings_.undrlayer_width);
    first_stop_substrate.SetStrokeLineCap("round");
    first_stop_substrate.SetStrokeLineJoin("round");
    first_stop_text.SetFillColor(settings_.color_palette[color_id++ % settings_.color_palette.size()]);
    doc.Add(first_stop_substrate);
    doc.Add(first_stop_text);

    const string &last_stop_name = route.stops[route.stops.size() / 2];
    if (route.is_roundtrip || last_stop_name == first_stop_name)
      continue;

    Svg::Text last_stop_substrate = first_stop_substrate;
    last_stop_substrate.SetPoint(projector_.ProjectPoint(db.GetStopsData().at(last_stop_name).position));
    Svg::Text last_stop_text = first_stop_text;
    last_stop_text.SetPoint(projector_.ProjectPoint(db.GetStopsData().at(last_stop_name).position));
    doc.Add(move(last_stop_substrate));
    doc.Add(move(last_stop_text));
  }
}
}