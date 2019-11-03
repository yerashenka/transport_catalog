#pragma once

namespace Svg {

struct Point {
  double x;
  double y;
};

struct Rgb {
  int red;
  int green;
  int blue;
};

class Color {};

class Document {};

class Shape {};

class Circle : public Shape {};

class Polyline : public Shape {};

class Text : public Shape {};
}
