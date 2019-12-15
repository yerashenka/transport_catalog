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

vector<Json::Node> ProcessRoutingExample(const Json::Document &doc) {
  const map<string, Json::Node> input = doc.GetRoot().AsMap();
  const vector<Json::Node> &database_input = input.at("base_requests").AsArray();
  const map<string, Json::Node> &settings = input.at("routing_settings").AsMap();
  TransportDatabase::Database db(TransportData::ReadData(database_input), settings);

  TransportInformer::Informer informer;
  const vector<Json::Node> &info_requests = input.at("stat_requests").AsArray();
  return informer.ProcessRequests(db, info_requests);
}

TEST_CASE("RoutingExample1") {
  ifstream input_stream("routing_queries/example1-input.json");
  const auto output = ProcessRoutingExample(Json::Load(input_stream));
  ifstream correct_stream("routing_queries/example1-output.json");
  const auto correct = Json::Load(correct_stream).GetRoot().AsArray();
  CheckResponses(output, correct);
}

TEST_CASE("RoutingExample2") {
  ifstream input_stream("routing_queries/example2-input.json");
  const auto output = ProcessRoutingExample(Json::Load(input_stream));

  ifstream correct_stream("routing_queries/example2-output.json");
  const auto correct = Json::Load(correct_stream).GetRoot().AsArray();
  CheckResponses(output, correct);
}

TEST_CASE("RoutingExample3") {
  ifstream input_stream("routing_queries/example3-input.json");
  const auto output = ProcessRoutingExample(Json::Load(input_stream));

  ifstream correct_stream("routing_queries/example3-output.json");
  const auto correct = Json::Load(correct_stream).GetRoot().AsArray();
  CheckResponses(output, correct);
}