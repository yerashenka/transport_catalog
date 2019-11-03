#include "json.h"
#include "transport_data.h"
#include "transport_informer.h"
#include "transport_database.h"


#include <iostream>

using namespace std;

int main() {
  const auto input = Json::Load(cin).GetRoot().AsMap();

  const auto &database_input = input.at("base_requests").AsArray();
  const auto &settings = input.at("routing_settings").AsMap();
  TransportDatabase db(TransportData::ReadData(database_input), settings);

  const auto &info_requests = input.at("stat_requests").AsArray();
  cout << TransportInformer::ProcessRequests(db, info_requests) << endl;

  return 0;
}
