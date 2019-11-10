#include "catch.hpp"
#include "svg.h"
#include <fstream>
#include <string>
#include <sstream>

using namespace std;
using namespace Svg;

TEST_CASE("SVG Library Test") {
  Svg::Document svg{};

  svg.Add(
      Svg::Polyline{}
          .SetStrokeColor(Svg::Rgb{140, 198, 63})  // soft green
          .SetStrokeWidth(16)
          .SetStrokeLineCap("round")
          .AddPoint({50, 50})
          .AddPoint({250, 250})
  );

  for (const auto point : {Svg::Point{50, 50}, Svg::Point{250, 250}}) {
    svg.Add(
        Svg::Circle{}
            .SetFillColor("white")
            .SetRadius(6)
            .SetCenter(point)
    );
  }

  svg.Add(
      Svg::Text{}
          .SetPoint({50, 50})
          .SetOffset({10, -10})
          .SetFontSize(20)
          .SetFontFamily("Verdana")
          .SetFillColor("black")
          .SetData("C")
  );
  svg.Add(
      Svg::Text{}
          .SetPoint({250, 250})
          .SetOffset({10, -10})
          .SetFontSize(20)
          .SetFontFamily("Verdana")
          .SetFillColor("black")
          .SetData("C++")
  );

  stringstream output_stream;
  svg.Render(output_stream);
  ifstream correct_file("svg_examples/example_1.svg");
  string output{output_stream.str()};
  stringstream correct_stream;
  correct_stream << correct_file.rdbuf();
  string correct_output{correct_stream.str()};
  REQUIRE(output == correct_output);
 }