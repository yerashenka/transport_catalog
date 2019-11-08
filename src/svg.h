#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace Svg {

struct Point {
  double x;
  double y;
};

struct Rgb {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

class Color {
 public:
  Color() : color_("none") {}
  Color(std::string color) : color_(std::move(color)) {}
  Color(const Rgb &rgb) : color_("rgb(" + std::to_string(rgb.red)
                                 + ',' + std::to_string(rgb.green)
                                 + ',' + std::to_string(rgb.blue) + ')') {}

  [[nodiscard]]
  const std::string &ToString() const { return color_; }

 private:
  std::string color_;
};

class Shape {
 public:
  Shape &SetFillColor(const Color &color) { fill_color_ = color; return *this; }
  Shape &SetStrokeColor(const Color &color) { stroke_color_ = color; return *this; }
  Shape &SetStrokeWidth(double width) { stroke_width_ = width; return *this; }
  Shape &SetStrokeLineCap(const std::string &cap) { line_cap_ = cap; return *this; }
  Shape &SetStrokeLineJoin(const std::string &join) { line_join_ = join; return *this; }
  virtual void Render(std::ostream &out) const = 0;

 protected:
  virtual void RenderBase(std::ostream &out) const {
    out << "fill=\"" << fill_color_.ToString() << "\" ";
    out << "stroke=\"" << stroke_color_.ToString() << "\"";
    out << "stroke-width=\"" << stroke_width_ << "\"";
    if (!line_cap_.empty())
      out << "stroke-linecap=\"" << line_cap_ << "\"";
    if (!line_join_.empty())
      out << "stroke-linejoin=\"" << line_join_ << "\"";
  }

 private:
  Color fill_color_;
  Color stroke_color_;
  double stroke_width_{1.0};
  std::string line_cap_;
  std::string line_join_;
};

class Circle : public Shape {
 public:
  Circle &SetCenter(Point center) { center_ = center; return *this; }
  Circle &SetRadius(double radius) { radius_ = radius; return *this; }
  void Render(std::ostream &out) const override {
    out << "<circle ";
    out << "cx=\"" << center_.x << "\" cy=\"" << center_.y << "\" ";
    out << "r=\"" << radius_ << "\" ";
    Shape::RenderBase(out);
    out << "/>";
  }

 private:
  Point center_{0, 0};
  double radius_{1.0};
};

class Polyline : public Shape {
 public:
  Polyline &AddPoint(Point point) { points_.emplace_back(point); return *this; }
  void Render(std::ostream &out) const override {
    out << "<poline ";
    out << "points=\"";
    for (const Point &point : points_) {
      out << point.x << ',' << point.y << ' ';
    }
    out << "\" ";
    Shape::RenderBase(out);
    out << "/>";
  }

 private:
  std::vector<Point> points_;
};

class Text : public Shape {
 public:
  Text &SetPoint(Point point) { point_ = point; return *this; }
  Text &SetOffset(Point point) { offset_ = point; return *this; }
  Text &SetFontSize(uint32_t font_size) {font_size_ = font_size; return *this; }
  Text &SetFontFamily(const std::string &font_family) { font_family_ = font_family; return *this; }
  Text &SetData(const std::string &data) { data_ = data; return *this; }
  void Render(std::ostream &out) const override {
    out << "<text ";
    out << "x=\"" << point_.x << ", " << "y=\"" << point_.y << "\" ";
    out << "dx=\"" << offset_.x << ", " << "dy=\"" << offset_.y << "\" ";
    out << "font-size=\"" << font_size_ << "\" ";
    out << "font-family=\"" << font_family_ << "\" ";
    out << '<' << data_ << "</text>";
  }

 private:
  Point point_{0, 0};
  Point offset_{0, 0};
  uint32_t font_size_{1};
  std::string font_family_;
  std::string data_;
};

class Document {
 public:
  void Add(Circle circle);
  void Add(Polyline polyline);
  void Add(Text text);
  void Render(std::ostream &out) const {
    out << R"(<?xml version="1.0" encoding="UTF-8" ?>)";
    out << R"(<svg xmlns="http://www.w3.org/2000/svg" version="1.1">)";
    for (const auto &shape : shapes_) {
      shape->Render(out);
    }
    out << R"(</svg>)";
  }

 private:
  std::vector<std::shared_ptr<Shape>> shapes_;
};
}
