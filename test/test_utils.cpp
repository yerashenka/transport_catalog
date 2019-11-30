#include "test_utils.h"
#include "catch.hpp"
#include "transport_data.h"
#include "transport_informer.h"
#include "transport_database.h"
#include <algorithm>
#include <fstream>
#include <map>
#include <string>

using namespace std;

void CheckResponses(const vector<Json::Node> &lhs, const vector<Json::Node> &rhs) {
  REQUIRE(lhs.size() == rhs.size());
  for (const Json::Node &lhs_response : lhs) {
    auto rhs_response_it = find_if(rhs.begin(), rhs.end(), [&lhs_response](const auto &item) {
      return lhs_response.AsMap().at("request_id").AsInt() == item.AsMap().at("request_id").AsInt();
    });
    REQUIRE(rhs_response_it != rhs.end());
    REQUIRE(lhs_response == *rhs_response_it);
  }
}

void CheckStreams(istream &lhs, istream &rhs) {
  string lhs_str, rhs_str;
  while (lhs.good() || rhs.good()) {
    REQUIRE(lhs.good() && rhs.good());
    lhs >> lhs_str;
    rhs >> rhs_str;
    REQUIRE(lhs_str == rhs_str);
  }
}

std::vector<Json::Node> ProcessExample(const Json::Document &doc) {
  const map<string, Json::Node> input = doc.GetRoot().AsMap();
  const vector<Json::Node> &database_input = input.at("base_requests").AsArray();
  const map<string, Json::Node> &settings = input.at("routing_settings").AsMap();
  TransportDatabase::Database db(TransportData::ReadData(database_input), settings);

  const Json::Dict &render_settings = input.at("render_settings").AsMap();
  const vector<Json::Node> &info_requests = input.at("stat_requests").AsArray();
  TransportInformer::Informer informer(Visualisation::MapBuilder{render_settings});
  return informer.ProcessRequests(db, info_requests);
}