#include "json.h"
#include "transport_data.h"
#include "transport_informer.h"
#include "transport_database.h"
#include "map_builder.h"

#include <iostream>

using namespace std;
using namespace TransportDatabase;
using namespace TransportInformer;

int main() {
  const auto input = Json::Load(cin).GetRoot().AsMap();

  const auto &database_input = input.at("base_requests").AsArray();
  const auto &routing_settings = input.at("routing_settings").AsMap();
  Database db(TransportData::ReadData(database_input), routing_settings);

  const auto &render_settings = input.at("render_settings").AsMap();
  Informer informer(Visualisation::MapBuilder{render_settings});
  const auto &info_requests = input.at("stat_requests").AsArray();
  cout << informer.ProcessRequests(db, info_requests) << endl;

  return 0;
}
