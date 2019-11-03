#include "utils.h"
#include "catch.hpp"
#include "json.h"
#include "transport_data.h"
#include "transport_informer.h"
#include "transport_database.h"
#include <algorithm>
#include <fstream>
#include <map>
#include <vector>

using namespace std;

void CheckResponses(const vector<Json::Node> &lhs, const vector<Json::Node> &rhs) {
  REQUIRE(lhs.size() == rhs.size());
  for (const auto &lhs_item : lhs) {
    auto it = std::find_if(rhs.begin(), rhs.end(), [&lhs_item](const auto &item) {
      return lhs_item.AsMap().at("request_id").AsInt() == item.AsMap().at("request_id").AsInt();
    });
    REQUIRE(it != rhs.end());
    REQUIRE(lhs_item == *it);
  }
}

vector<Json::Node> ProcessExample(const Json::Document &doc) {
  const auto input = doc.GetRoot().AsMap();
  const auto &database_input = input.at("base_requests").AsArray();
  const auto &settings = input.at("routing_settings").AsMap();
  TransportDatabase db(TransportData::ReadData(database_input), settings);
  const auto &info_requests = input.at("stat_requests").AsArray();
  return TransportInformer::ProcessRequests(db, info_requests);
}

TEST_CASE("Example1") {
  ifstream input_stream("test_queries/example1-input.json");
  const auto output = ProcessExample(Json::Load(input_stream));

  ifstream correct_stream("test_queries/example1-output.json");
  const auto correct = Json::Load(correct_stream).GetRoot().AsArray();
  CheckResponses(output, correct);
}

TEST_CASE("Example2") {
  ifstream input_stream("test_queries/example2-input.json");
  const auto output = ProcessExample(Json::Load(input_stream));

  ifstream correct_stream("test_queries/example2-output.json");
  const auto correct = Json::Load(correct_stream).GetRoot().AsArray();
  CheckResponses(output, correct);
}

TEST_CASE("Example3") {
  ifstream input_stream("test_queries/example3-input.json");
  const auto output = ProcessExample(Json::Load(input_stream));

  ifstream correct_stream("test_queries/example3-output.json");
  const auto correct = Json::Load(correct_stream).GetRoot().AsArray();
  CheckResponses(output, correct);
}