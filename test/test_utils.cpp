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