#include "utils.h"
#include "catch.hpp"
#include "json.h"
#include "test_utils.h"
#include "transport_data.h"
#include "transport_informer.h"
#include "transport_database.h"
#include <algorithm>
#include <fstream>
#include <map>
#include <vector>

using namespace std;
using namespace Json;



TEST_CASE("RoutingExample1") {
  ifstream input_stream("routing_queries/example1-input.json");
  const auto output = ProcessExample(Json::Load(input_stream));
  ifstream correct_stream("routing_queries/example1-output.json");
  const auto correct = Json::Load(correct_stream).GetRoot().AsArray();
  CheckResponses(output, correct);
}

TEST_CASE("RoutingExample2") {
  ifstream input_stream("routing_queries/example2-input.json");
  const auto output = ProcessExample(Json::Load(input_stream));

  ifstream correct_stream("routing_queries/example2-output.json");
  const auto correct = Json::Load(correct_stream).GetRoot().AsArray();
  CheckResponses(output, correct);
}

TEST_CASE("RoutingExample3") {
  ifstream input_stream("routing_queries/example3-input.json");
  const auto output = ProcessExample(Json::Load(input_stream));

  ifstream correct_stream("routing_queries/example3-output.json");
  const auto correct = Json::Load(correct_stream).GetRoot().AsArray();
  CheckResponses(output, correct);
}