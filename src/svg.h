#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace Svg {

struct Point {
  double x;
  double y;
};

struct Color {
  std::string color_string;
  Color() = default;
  Color(const char *str) : color_string(str) {}
  Color(std::string str) : color_string(std::move(str)) {}

  friend std::ostream &operator<<(std::ostream &out, const Color &color) {
    out << (color.color_string.empty() ? "none" : color.color_string);
    return out;
  }
};

const Color NoneColor = Color{};

struct Rgb {
  uint8_t red;
  uint8_t green;
  uint8_t blue;

  operator Color() const {
    return Color("rgb(" + std::to_string(red)
             + ',' + std::to_string(green)
             + ',' + std::to_string(blue) + ')');
  }
};

struct Rgba {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  double alpha;

  operator Color() const {
    return Color("rgba(" + std::to_string(red)
                + ',' + std::to_string(green)
                + ',' + std::to_string(blue)
                + ',' + std::to_string(alpha) + ')');
  }
};

namespace Properties {
struct FillColor{
  Color color;
  friend std::ostream &operator<<(std::ostream &out, const FillColor &fill_color) {
    out << "fill=\"" << fill_color.color << "\" ";
    return out;
  }
};

struct StrokeColor {
  Color color;
  friend std::ostream &operator<<(std::ostream &out, const StrokeColor &stroke_color) {
    out << "stroke=\"" << stroke_color.color << "\" ";
    return out;
  }
};

struct StrokeWidth {
  double width;
  friend std::ostream &operator<<(std::ostream &out, const StrokeWidth &stroke_width) {
    out << "stroke-width=\"" << stroke_width.width << "\" ";
    return out;
  }
};

struct LineCap {
  std::string cap;
  friend std::ostream &operator<<(std::ostream &out, const LineCap &line_cap) {
    if (!line_cap.cap.empty())
      out << "stroke-linecap=\"" << line_cap.cap << "\" ";
    return out;
  }
};

struct LineJoin {
  std::string join;
  friend std::ostream &operator<<(std::ostream &out, const LineJoin &line_join) {
    if (!line_join.join.empty())
      out << "stroke-linejoin=\"" << line_join.join << "\" ";
    return out;
  }
};

struct Center {
  Point point;
  friend std::ostream &operator<<(std::ostream &out, const Center &center) {
    out << "cx=\"" << center.point.x << "\" cy=\"" << center.point.y << "\" ";
    return out;
  }
};

struct Radius {
  double r;
  friend std::ostream &operator<<(std::ostream &out, const Radius &radius) {
    out << "r=\"" << radius.r << "\" ";
    return out;
  }
};

struct Coordinates {
  Point point;
  friend std::ostream &operator<<(std::ostream &out, const Coordinates &coordinates) {
    out << "x=\"" << coordinates.point.x << "\" y=\"" << coordinates.point.y << "\" ";
    return out;
  }
};

struct Offset {
  Point point;
  friend std::ostream &operator<<(std::ostream &out, const Offset &offset) {
    out << "dx=\"" << offset.point.x << "\" dy=\"" << offset.point.y << "\" ";
    return out;
  }
};

struct FontSize {
  uint32_t size;
  friend std::ostream &operator<<(std::ostream &out, const FontSize &font_size) {
    out << "font-size=\"" << font_size.size << "\" ";
    return out;
  }
};

struct FontFamily {
  std::string family;
  friend std::ostream &operator<<(std::ostream &out, const FontFamily &font_family) {
    if (!font_family.family.empty())
      out << "font-family=\"" << font_family.family << "\" ";
    return out;
  }
};

struct FontWeight {
  std::string weight;
  friend std::ostream &operator<<(std::ostream &out, const FontWeight &font_weight) {
    if (!font_weight.weight.empty())
      out << "font-weight=\"" << font_weight.weight << "\" ";
    return out;
  }
};
}

class ISvgObject {
 public:
  virtual ~ISvgObject() = default;
  virtual void Render(std::ostream &out) const = 0;
};

template <typename Shape>
class ShapeBasis {
 public:
  Shape &SetFillColor(const Color &color) { fill_color_ = {color}; return static_cast<Shape&>(*this); }
  Shape &SetStrokeColor(const Color &color) { stroke_color_ = {color}; return static_cast<Shape&>(*this); }
  Shape &SetStrokeWidth(double width) { stroke_width_ = {width}; return static_cast<Shape&>(*this); }
  Shape &SetStrokeLineCap(const std::string &cap) { line_cap_ = {cap}; return static_cast<Shape&>(*this); }
  Shape &SetStrokeLineJoin(const std::string &join) { line_join_ = {join}; return static_cast<Shape&>(*this); }
  void Render(std::ostream &out) const {
    out << fill_color_ << stroke_color_ << stroke_width_ << line_cap_
        << line_join_;
  }

 private:
  Properties::FillColor fill_color_{NoneColor};
  Properties::StrokeColor stroke_color_{NoneColor};
  Properties::StrokeWidth stroke_width_{1.0};
  Properties::LineCap line_cap_;
  Properties::LineJoin line_join_;
};

class Circle : public ShapeBasis<Circle>, public ISvgObject {
 public:
  Circle &SetCenter(Point center) { center_ = {center}; return *this; }
  Circle &SetRadius(double radius) { radius_ = {radius}; return *this; }
  void Render(std::ostream &out) const override {
    out << "<circle " << center_ << radius_;
    ShapeBasis::Render(out);
    out << "/>";
  }

 private:
  Properties::Center center_{0, 0};
  Properties::Radius radius_{1.0};
};

class Polyline : public ShapeBasis<Polyline>, public ISvgObject {
 public:
  Polyline &AddPoint(Point point) { points_.emplace_back(point); return *this; }
  void Render(std::ostream &out) const override {
    out << "<polyline points=\"";
    for (const Point &point : points_) {
      out << point.x << ',' << point.y << ' ';
    }
    out << "\" ";
    ShapeBasis::Render(out);
    out << "/>";
  }

 private:
  std::vector<Point> points_;
};

class Text : public ShapeBasis<Text>, public ISvgObject {
 public:
  Text &SetPoint(Point point) { coordinates_ = {point}; return *this; }
  Text &SetOffset(Point point) { offset_ = {point}; return *this; }
  Text &SetFontSize(uint32_t font_size) {font_size_ = {font_size}; return *this; }
  Text &SetFontFamily(const std::string &font_family) { font_family_ = {font_family}; return *this; }
  Text &SetFontWeight(const std::string &font_weight) { font_weight_ = {font_weight}; return *this; }
  Text &SetData(const std::string &data) { text_ = data; return *this; }
  void Render(std::ostream &out) const override {
    out << "<text " << coordinates_ << offset_ << font_size_
        << font_family_ << font_weight_;
    ShapeBasis::Render(out);
    out <<'>' << text_ << "</text>";
  }

 private:
  Properties::Coordinates coordinates_{0, 0};
  Properties::Offset offset_{0, 0};
  Properties::FontSize font_size_{1};
  Properties::FontFamily font_family_;
  Properties::FontWeight font_weight_;
  std::string text_;
};

class Document {
 public:
  template <typename Type>
  void Add(Type object) {
    objects_.emplace_back(std::make_unique<Type>(std::move(object)));
  }
  void Render(std::ostream &out) const {
    out << R"(<?xml version="1.0" encoding="UTF-8" ?>)";
    out << R"(<svg xmlns="http://www.w3.org/2000/svg" version="1.1">)";
    for (const auto &object : objects_){
      object->Render(out);
    }
    out << R"(</svg>)";
  }

 private:
  std::vector<std::unique_ptr<ISvgObject>> objects_;
};
}
